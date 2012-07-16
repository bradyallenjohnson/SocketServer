#include <iostream>

#include <sys/socket.h>

#include "SocketServer.h"
#include "UdpSocketServerImpl.h"

UdpSocketServerImpl::UdpSocketServerImpl()
{
}

UdpSocketServerImpl::UdpSocketServerImpl(int port, int timeout_millis /*default 0*/) :
  SocketServer(port, timeout_millis)
{
}

// virtual
bool UdpSocketServerImpl::initializeSpecific()
{
  sockListenFd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockListenFd_ < 0)
  {
    std::cerr << "Error initializing UDP socket" << std::endl;
    return false;
  }

  if(bind(sockListenFd_, (struct sockaddr *) &serverAddress_, sizeof(struct sockaddr_in)) < 0)
  {
    std::cerr << "Error binding socket" << std::endl;
    return false;
  }

  if(isDebug())
  {
    std::cout << "UDP server successfully bound to port: " << port_;
  }

  return true;
}

// virtual
int UdpSocketServerImpl::readSpecific(int sockFd, char *buffer)
{
  // TODO need to check if more bytes are avail to be read
  int numBytesRead =
        recvfrom(
            sockFd,
            buffer,
            MAX_MESSAGE_LENGTH,
            0, // flags
            (struct sockaddr *) &clientAddress_,
            &clientAddrLen_);

  return numBytesRead;
}

// virtual
int UdpSocketServerImpl::writeSpecific(int sockFd, char *buffer, int bufLength)
{
  int numBytesWritten = 
        sendto(
            sockFd,
            buffer,
            bufLength,
            0, // flags
            (struct sockaddr *) &clientAddress_,
            clientAddrLen_);

  return numBytesWritten;
}

