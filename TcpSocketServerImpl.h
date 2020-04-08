
#ifndef TCP_SOCKET_SERVER_IMPL_H
#define TCP_SOCKET_SERVER_IMPL_H

#include "SocketServer.h"

//
// Concrete TCP implementation of SocketServer
//
class TcpSocketServerImpl : public SocketServer
{
  public:
    TcpSocketServerImpl();
    TcpSocketServerImpl(int listenPort, int timeout_millis = 0);
    TcpSocketServerImpl(const string &serverAddress, int listenPort, bool isIpv6, int timeout_millis = 0);

  protected:
    virtual bool initializeSpecific();

    virtual int readSpecific(int sockFd, char *buffer);
    virtual int writeSpecific(int sockFd, char *buffer, int bufLength);

  private:
    void acceptConnection();

};

#endif
