
#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <list>
#include <string>

#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "ServerHandler.h"

using namespace std;

//
// Abstract base class SocketServer
// Concrete implementations are UdpSocketServer and TcpSocketServer
// The server is a simple single threaded server
//
class SocketServer
{
  public:
    SocketServer();
    // uses INADDR_ANY for local IP
    SocketServer(int listenPort, int timeout_millis = 0);
    // connects to a specific local IP
    SocketServer(const string &serverAddress, int listenPort, bool isIpv6, int timeout_millis = 0);
    virtual ~SocketServer();

    class SocketServerStatistics
    {
    public:
      uint64_t numConnects;
      uint64_t numDisconnects;
      uint64_t numReads;
      uint64_t numWrites;
      uint64_t numSocketExceptions;
      uint64_t numTimeouts;
      SocketServerStatistics();
      SocketServerStatistics(const SocketServerStatistics &rhs);
      void clear();
    };

    inline int getPort() {return port_;}
    inline void setPort(int port) {port_ = port;}
    inline void setHandler(ServerHandler *handler) {handler_ = handler;}
    inline ServerHandler *getHandler() {return handler_;}
    inline SocketServer::SocketServerStatistics getStatistics() const {return stats_;}

    inline bool isDebug() {return isDebug_;}
    inline void setDebug(bool dbg) {isDebug_ = dbg;}

    bool initialize();

    // NOTICE: This method call may be BLOCKING
    // If numMessagesToRead is set, this call will execute until that number of messages is read
    // If numMessagesToRead is 0, then the call will block and run continuously
    virtual void run(int numMessagesToRead=0);
    virtual inline void stop() { runServer_ = false; }

    const static string ANY_IP_STR;
    const static int MAX_MESSAGE_LENGTH;

  protected:
    // Template method design pattern
    virtual bool initializeSpecific() = 0;
    virtual int writeSpecific(int sockFd, char *buffer, int bufLength) = 0;
    virtual int readSpecific(int sockFd, char *buffer) = 0;

    virtual int getMaxFd();
    virtual int getNumFd();
    virtual void closeSocket(int sockFd);

    ServerHandler *handler_;
    int port_;
    string serverAddressStr_;
    int sockListenFd_;
    typedef list<int> ClientSockFdListType;
    ClientSockFdListType clientSockFdList_; // used by TCP
    int maxFd_;

    fd_set socketFdReadSet_;
    fd_set socketFdWriteSet_;
    fd_set socketFdErrorSet_;
    fd_set allSocketFdReadSet_;
    fd_set allSocketFdWriteSet_;
    fd_set allSocketFdErrorSet_;

    SocketAddrIn serverAddress_;
    SocketAddrIn clientAddress_;
    socklen_t clientAddrLen_;
    bool isIpv6_;

    bool isDebug_;
    int timeout_sec_;
    int timeout_msec_;
    int timeout_usec_;
    struct timeval *tval_;
    bool runServer_;
    SocketServer::SocketServerStatistics stats_;
};

#endif
