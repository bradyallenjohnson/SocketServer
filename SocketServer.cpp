
#include <iostream>
#include <algorithm>

#include <string.h>

#include <sys/select.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <errno.h>

#include "SocketServer.h"
#include "ServerHandler.h"

using namespace std;

const int SocketServer::MAX_MESSAGE_LENGTH = 2048;

SocketServer::SocketServer() :
  handler_(NULL),
  port_(-1),
  sockListenFd_(-1),
  isDebug_(false),
  timeout_msec_(0),
  tval_(NULL),
  runServer_(true)
{
}

SocketServer::SocketServer(int port, int timeout_millis /*defaults 0*/) :
  handler_(NULL),
  port_(port),
  sockListenFd_(-1),
  isDebug_(false),
  timeout_msec_(timeout_millis),
  tval_(NULL),
  runServer_(true)
{
}

SocketServer::~SocketServer()
{
  if(sockListenFd_ >= 0)
  {
    close(sockListenFd_);
  }
  if(tval_ != NULL)
  {
    delete tval_;
  }
}

SocketServer::SocketServerStatistics::SocketServerStatistics() :
      numConnects(0),
      numDisconnects(0),
      numReads(0),
      numWrites(0),
      numSocketExceptions(0),
      numTimeouts(0)
{
}

SocketServer::SocketServerStatistics::SocketServerStatistics(const SocketServerStatistics &rhs)
{
  numConnects          =  rhs.numConnects;
  numDisconnects       =  rhs.numDisconnects;
  numReads             =  rhs.numReads;
  numWrites            =  rhs.numWrites;
  numSocketExceptions  =  rhs.numSocketExceptions;
  numTimeouts          =  rhs.numTimeouts;
}

void SocketServer::SocketServerStatistics::clear()
{
  numConnects = 0;
  numDisconnects = 0;
  numReads = 0;
  numWrites = 0;
  numSocketExceptions = 0;
  numTimeouts = 0;
}

bool SocketServer::initialize()
 {
  // We're always going to use IP
  memset(&serverAddress_, 0, sizeof(struct sockaddr_in));
  serverAddress_.sin_family = AF_INET;
  serverAddress_.sin_addr.s_addr = INADDR_ANY;
  serverAddress_.sin_port = htons(port_);

  clientAddrLen_ = sizeof(struct sockaddr_in);
  memset((void *) &clientAddress_, 0, sizeof(struct sockaddr_in));

  if(timeout_msec_ > 0)
  {
    tval_ = new struct timeval();
    if(timeout_msec_ > 999)
    {
      tval_->tv_sec  = timeout_msec_/1000;
      tval_->tv_usec = (timeout_msec_ - (tval_->tv_sec*1000))*1000;
    }
    else
    {
      tval_->tv_sec = 0;
      tval_->tv_usec = timeout_msec_*1000;
    }
    timeout_sec_  = tval_->tv_sec;
    timeout_usec_ = tval_->tv_usec;
  }

  FD_ZERO(&socketFdReadSet_);
  FD_ZERO(&socketFdWriteSet_);
  FD_ZERO(&socketFdErrorSet_);
  FD_ZERO(&allSocketFdReadSet_);
  FD_ZERO(&allSocketFdWriteSet_);
  FD_ZERO(&allSocketFdErrorSet_);

  // Template method design pattern
  // Perform common operations, then let concrete subclasses do their part
  bool retval = initializeSpecific();

  // The listen socket should always be readable
  FD_SET(sockListenFd_, &allSocketFdReadSet_);

  clientSockFdList_.push_back(sockListenFd_);
  maxFd_ = sockListenFd_;

  return retval;
}

// protected
// virtual
int SocketServer::getMaxFd()
{
  return maxFd_ + 1;
}

// protected
// virtual
int SocketServer::getNumFd()
{
  return clientSockFdList_.size();
}

// protected
// virtual
void SocketServer::closeSocket(int sockFd)
{
  if(isDebug())
  {
    cout << "Socket closure for sockFd: " << sockFd << ", closing it" << endl;
  }

  close(sockFd);

  FD_CLR(sockFd, &allSocketFdWriteSet_);
  FD_CLR(sockFd, &allSocketFdReadSet_);

  // std::remove() was giving strange results
  // remove(clientSockFdList_.begin(), clientSockFdList_.end(), sockFd);

  ClientSockFdListType::iterator iter;
  bool foundIt(false);
  for(iter = clientSockFdList_.begin();
      (iter != clientSockFdList_.end() && !foundIt);
      ++iter)
  {
    if(*iter == sockFd)
    {
      clientSockFdList_.erase(iter);
      foundIt = true;
    }
  }

  if(sockFd == sockListenFd_)
  {
    sockListenFd_ = -1;
  }
}

// NOTICE: This method call blocks
// virtual
void SocketServer::run(int numMessagesToRead /*default 0*/)
{
  int numMessagesRead(0);
  int nready;
  char msgBuffer[MAX_MESSAGE_LENGTH];
  int socketToClose(0);

  while((numMessagesToRead == 0 || numMessagesRead < numMessagesToRead) && runServer_)
  {
    // We could be more extensible by having a list of handlers and iterate it

    // Set the Fd's to be read and written to, structure copy
    socketFdReadSet_ = allSocketFdReadSet_;
    socketFdWriteSet_ = allSocketFdWriteSet_;

    // This call blocks
    if((nready = select(getMaxFd(), &socketFdReadSet_, &socketFdWriteSet_, &socketFdErrorSet_, tval_)) < 0)
    {
      // If the application was interrupted via signal handlers, dont treat it as an error
      int theError(errno);
      if(theError != EINTR)
      {
        cerr << "Error in select, errno: " << theError
             << ", " << strerror(theError)
             << endl;
        // TODO should we exit? set runServer_ = false if so
      }

      continue;
    }

    if(nready == 0 && tval_ != NULL)
    {
      handler_->handleTimeout();
      // tval_ is reset to the number of seconds not slept
      tval_->tv_sec  = timeout_sec_;
      tval_->tv_usec = timeout_usec_;
      numMessagesRead++;
      ++stats_.numTimeouts;

      continue;
    }

    // Iterate the socket FDs to see which needs to be read or written
    ClientSockFdListType::const_iterator iter;
    for(iter = clientSockFdList_.begin(); iter != clientSockFdList_.end(); ++iter)
    {
      int sockFd = *iter;

      // Socket is ready to read
      if(FD_ISSET(sockFd, &socketFdReadSet_))
      {
        int numBytesRead = readSpecific(sockFd, msgBuffer);
        numMessagesRead++;

        if(isDebug())
        {
          cout << "\nReceived a message[" << numMessagesRead
               << "] of size: " << numBytesRead
               << ", on sockFd: " << sockFd
               << endl;
        }

        // A zero byte read is typically a socket closure on the other end
        if(numBytesRead == 0)
        {
          // Will be closed outside of this List iteration loop
          ++stats_.numDisconnects;
          socketToClose = sockFd;
          handler_->handleDisconnect(sockFd);
        }
        else if(numBytesRead > 0)
        {
          ++stats_.numReads;
          handler_->handleMessage(sockFd, msgBuffer, numBytesRead, &clientAddress_);
          // Set the socket for writing if the handler has a response ready
          if(handler_->hasResponse(sockFd))
          {
            FD_SET(sockFd, &allSocketFdWriteSet_);
          }
        }
      } // if socketFdReadSet

      // Socket is ready to write
      if(FD_ISSET(sockFd, &socketFdWriteSet_))
      {
        ++stats_.numWrites;
        ServerHandler::ServerMessage *message = handler_->getMessage(sockFd);
        int numBytesWritten =
            writeSpecific(sockFd,
                          message->message,
                          message->messageLength);
        // Turn off writing now that we've sent a response
        // the sockets should always be set for reading
        FD_CLR(sockFd, &allSocketFdWriteSet_);

        if(isDebug())
        {
          cout << "Replied with a message of size: " << numBytesWritten
               << ", on sockFd: " << sockFd
               << endl;
        }
      } // if socketFdWriteSet

      // Socket has an error
      if(FD_ISSET(sockFd, &socketFdErrorSet_))
      {
        // TODO should we pass this to the handler too?
          ++stats_.numSocketExceptions;

        cerr << "Exception on socket: " << sockFd << endl;
        // socketToClose = sockFd;
      }

    } // for loop

    // Causes problems if we close it while in the list iteration loop
    if(socketToClose)
    {
      closeSocket(socketToClose);
      socketToClose = 0;
    }

  } // while loop
}

