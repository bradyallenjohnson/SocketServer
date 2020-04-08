/*
 * File:   EchoTcpServerHandler.cpp
 * Author: ebrjohn
 *
 * Created on July 11, 2012, 4:16 PM
 */

#include <iostream>
#include <string.h> // memcpy

#include "SocketServer.h"
#include "EchoTcpServerHandler.h"

using namespace std;

EchoTcpServerHandler::EchoTcpServerHandler()
{
  // Take advantage of the message already created, for
  // the rest, we'll have to create them on the fly
  // index 3 will be the TCP listen socket, so lets use 4
  responseMessageMap_[4] = &serverMessage_;
}

EchoTcpServerHandler::~EchoTcpServerHandler()
{
  ResponseMessageMapType::iterator iter;
  for(iter = responseMessageMap_.begin(); iter != responseMessageMap_.end(); ++iter)
  {
    // The fourth was created in the ServerHandler class, and
    // will be deleted there so dont double delete it here
    if(iter->first != 4)
    {
      delete iter->second;
      responseMessageMap_.erase(iter);
    }
  }
}

// private
// virtual
ServerHandler::ServerMessage *EchoTcpServerHandler::getMessage(int clientKey)
{
  ServerHandler::ServerMessage *msgPtr(NULL);

  ResponseMessageMapType::iterator iter = responseMessageMap_.find(clientKey);
  if(iter != responseMessageMap_.end())
  {
    msgPtr = iter->second;
  }

  return msgPtr;
}

// virtual
void EchoTcpServerHandler::handleConnect(int clientKey, const SocketAddrIn &clientAddr, socklen_t clientAddrLen)
{
  ServerHandler::ServerMessage *responseMsg(getMessage(clientKey));
  if(!responseMsg)
  {
    responseMessageMap_[clientKey] =
            new ServerHandler::ServerMessage(new char[SocketServer::MAX_MESSAGE_LENGTH], 0);
  }
}

// The responseMsg is passed down the chain so each member can see or modify it
// virtual
void EchoTcpServerHandler::handleMessage(int clientKey,
                                         char *requestMsg,
                                         int requestMsgLength,
                                         const SocketAddrIn &clientAddr,
                                         ServerHandler::ServerMessage *responseMsg)
{
  if(!responseMsg)
  {
    // The messages will be reused
    responseMsg = getMessage(clientKey);
    if(!responseMsg)
    {
      cerr << "Error, clientKey: " << clientKey << " does not exist!" << endl;
      return;
    }
  }
  responseMsg->messageLength = requestMsgLength;

  // have to copy the input message since it may be overwritten before replying

  memcpy(responseMsg->message, requestMsg, requestMsgLength);

  // Send it down the chain, if there is one
  if(chainedHandler_)
  {
    chainedHandler_->handleMessage(clientKey,
                                   requestMsg,
                                   requestMsgLength,
                                   clientAddr,
                                   responseMsg);
  }
}

// virtual
void EchoTcpServerHandler::handleTimeout()
{
  cout << "EchoTcpServerHandler::handleTimeout() received a timeout" << endl;
}
