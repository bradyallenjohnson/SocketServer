
#include "ServerHandler.h"
#include "SocketServer.h"

ServerHandler::ServerHandler() :
  hasResponse_(true),
  chainedHandler_(NULL)
{
  // will be deleted in ServerMessage dtor
  serverMessage_.message = new char[SocketServer::MAX_MESSAGE_LENGTH];
}

ServerHandler::~ServerHandler()
{
}
