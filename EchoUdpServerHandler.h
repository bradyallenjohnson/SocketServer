/*
 * File:   EchoUdpServerHandler.h
 * Author: ebrjohn
 *
 * Created on July 19, 2012, 4:16 PM
 */

#ifndef ECHO_UDP_SERVERHANDLER_H
#define	ECHO_UDP_SERVERHANDLER_H

#include <map>

#include "ServerHandler.h"

using namespace std;

class EchoUdpServerHandler : public ServerHandler
{
public:
  EchoUdpServerHandler();
  virtual ~EchoUdpServerHandler();

  virtual ServerHandler::ServerMessage *getMessage(int clientKey);

  virtual void handleMessage(int clientKey,
                             char *requestMsg,
                             int requestMsgLength,
                             struct sockaddr_in *clientAddr,
                             ServerHandler::ServerMessage *responseMsg);
  virtual void handleTimeout();

private:
  typedef map<int, ServerHandler::ServerMessage*> ResponseMessageMapType;
  ResponseMessageMapType responseMessageMap_;
};

#endif	/* ECHO_UDP_SERVERHANDLER_H */

