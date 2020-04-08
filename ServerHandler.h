
#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "SocketAddrIn.h"

//
// Abstract base class ServerHandler
// handles SocketServer incoming messages
// For now centered around UDP
//
class ServerHandler
{
public:
  ServerHandler();
  virtual ~ServerHandler();

  class ServerMessage
  {
  public:
    char *message;
    uint32_t messageLength;
    ServerMessage() : message(0), messageLength(0) {}
    ServerMessage(char *msg, uint32_t len) : message(msg), messageLength(len) {}
    ~ServerMessage() {if(message != 0) { delete [] message; messageLength = 0; } }
  };

  // Set chained ServerHandlers
  // Each ServerHandler will call the next one if it has one
  void setChainedHandler(ServerHandler *chainedHandler) { chainedHandler_ = chainedHandler; }
  ServerHandler *getChainedHandler() { return chainedHandler_; }

  // Called when a message is read from a client
  // The clientKey should be used when more there will be more
  // than one client and more than one message can be stored
  // The responseMsg is only set when calling the method internally
  virtual void handleMessage(int clientKey,
                             char *msg,
                             int msgLength,
                             const SocketAddrIn &clientAddr,
                             ServerHandler::ServerMessage *responseMsg = 0) = 0;
  virtual void handleTimeout() = 0;

  // Called to accept connections/disconnects, default behavior is provided
  // May never be called from some protocols, thus they arent pure virtual
  virtual void handleConnect(int clientKey, const SocketAddrIn &clientAddr, socklen_t clientAddrLen) {};
  virtual void handleDisconnect(int clientKey) {};

  // Called to get a message to write to the client
  // The key is in case this class is overridden and more than one
  // message is stored, it is the same as passed to handleMessage
  inline virtual ServerHandler::ServerMessage *getMessage(int clientKey)  {return &serverMessage_;}
  inline virtual bool hasResponse(int clientKey)       {return hasResponse_;}

protected:
  ServerMessage serverMessage_;
  bool hasResponse_;
  ServerHandler *chainedHandler_;
};

#endif
