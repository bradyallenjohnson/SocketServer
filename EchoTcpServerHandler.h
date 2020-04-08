/*
 * File:   EchoTcpServerHandler.h
 * Author: ebrjohn
 *
 * Created on July 11, 2012, 4:16 PM
 */

#ifndef ECHO_TCP_SERVERHANDLER_H
#define	ECHO_TCP_SERVERHANDLER_H

#include <map>

#include "ServerHandler.h"

using namespace std;

class EchoTcpServerHandler : public ServerHandler
{
public:
  EchoTcpServerHandler();
  virtual ~EchoTcpServerHandler();

  virtual ServerHandler::ServerMessage *getMessage(int clientKey);

  virtual void handleConnect(int clientKey, const SocketAddrIn &clientAddr, socklen_t clientAddrLen);
  virtual void handleMessage(int clientKey,
                             char *requestMsg,
                             int requestMsgLength,
                             const SocketAddrIn &clientAddr,
                             ServerHandler::ServerMessage *responseMsg);
  virtual void handleTimeout();

private:
  typedef map<int, ServerHandler::ServerMessage*> ResponseMessageMapType;
  ResponseMessageMapType responseMessageMap_;
};

#endif	/* ECHO_TCP_SERVERHANDLER_H */

