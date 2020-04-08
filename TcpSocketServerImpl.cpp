
#include <iostream>
#include <map>

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

#include "SocketServer.h"
#include "TcpSocketServerImpl.h"

using namespace std;

TcpSocketServerImpl::TcpSocketServerImpl()
{
}

TcpSocketServerImpl::TcpSocketServerImpl(int port, int timeout_millis /*default 0*/) :
  SocketServer(port, timeout_millis)
{
}

TcpSocketServerImpl::TcpSocketServerImpl(const string &serverAddress, int listenPort, bool isIpv6, int timeout_millis /* defaults 0 */) :
        SocketServer(serverAddress, listenPort, isIpv6, timeout_millis)
{
}

// virtual
bool TcpSocketServerImpl::initializeSpecific()
{
  sockListenFd_ = socket((isIpv6_ ? AF_INET6 : AF_INET), SOCK_STREAM, IPPROTO_TCP);
  if(sockListenFd_ < 0)
  {
    cerr << "Error initializing TCP socket, errno: " << endl;
    return false;
  }

  if(bind(sockListenFd_,
          (isIpv6_ ? (struct sockaddr *) &serverAddress_.socketAddrIn.sockAddrIpv6 :
                     (struct sockaddr *) &serverAddress_.socketAddrIn.sockAddrIpv4),
          (isIpv6_ ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in))) < 0)
  {
    cerr << "Error binding socket, errno: " << errno << endl;
    return false;
  }

  if(listen(sockListenFd_, 5) < 0)
  {
    cerr << "Error listening on socket, errno: " << errno << endl;
    return false;
  }

  if(isDebug())
  {
    cout << "TCP server successfully bound to port: " << port_ << endl;
  }

  return true;
}

void TcpSocketServerImpl::acceptConnection()
{
  int clientFd(accept(sockListenFd_,
          (isIpv6_ ? (sockaddr*) &clientAddress_.socketAddrIn.sockAddrIpv6 :
                     (sockaddr*) &clientAddress_.socketAddrIn.sockAddrIpv4),
          (socklen_t*) &clientAddrLen_));
  clientSockFdList_.push_back(clientFd);
  ++stats_.numConnects;

  if(isDebug())
  {
    cout << "Accepted a new client connection, listenSockFd: " << sockListenFd_
         << ", clientSockFd: " << clientFd
         << endl;
  }

  FD_SET(clientFd, &allSocketFdReadSet_);

  if(clientFd > maxFd_)
  {
    maxFd_ = clientFd;
  }

  handler_->handleConnect(clientFd, clientAddress_, clientAddrLen_);
}

// virtual
int TcpSocketServerImpl::readSpecific(int sockFd, char *buffer)
{
  int numBytesRead(-1);

  if(sockFd == sockListenFd_)
  {
    acceptConnection();
  }
  else
  {
    // TODO need to check if more bytes are avail to be read
    // zero byte reads handled in SocketServer::run()
    numBytesRead = read( sockFd, buffer, MAX_MESSAGE_LENGTH);
  }

  return numBytesRead;
}

// virtual
int TcpSocketServerImpl::writeSpecific(int sockFd, char *buffer, int bufLength)
{
  int numBytesWritten(write( sockFd, buffer, bufLength));

  return numBytesWritten;
}

