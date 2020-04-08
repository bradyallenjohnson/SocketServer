
AR= ar
ARFLAGS=rc
RANLIB = ranlib
COPY_CMD=cp
DELETE_CMD=rm -f
CC=g++
INCLUDE_PATH=-I. -I$(CLP_DIR)
LIB_PATH=-L$(LIB_DIR) -L$(CLP_DIR)
CCFLAGS= -g -Wall -Werror -fPIC

BUILD_DIR=./build
INCLUDE_DIR=$(BUILD_DIR)/include
BIN_DIR=$(BUILD_DIR)/bin
LIB_DIR=$(BUILD_DIR)/lib
OBJ_DIR=$(BUILD_DIR)/obj
SRC_DIR=.
CLP_DIR=../CommandLineParser

CLP_LIB_NAME=CmdLineParser
LIB_NAME=socketServer
LIB=$(LIB_DIR)/lib$(LIB_NAME).a

TCP_NAME= exampleMainTcp
TCP_CPP = $(SRC_DIR)/$(TCP_NAME).cpp
TCP_OBJ = $(OBJ_DIR)/$(TCP_NAME).o
TCP_BIN = $(BIN_DIR)/$(TCP_NAME)

UDP_NAME= exampleMainUdp
UDP_CPP = $(SRC_DIR)/$(UDP_NAME).cpp
UDP_OBJ = $(OBJ_DIR)/$(UDP_NAME).o
UDP_BIN = $(BIN_DIR)/$(UDP_NAME)

DEPS= *.h
EXTERNAL_DEPS=$(CLP_DIR)/*.h

_OBJS=EchoTcpServerHandler.o EchoUdpServerHandler.o ServerHandler.o SocketServer.o TcpSocketServerImpl.o UdpSocketServerImpl.o
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))

#
# Targets
#

all: $(OBJS) $(LIB) $(TCP_BIN) $(UDP_BIN)

$(LIB): $(OBJS)
	$(shell [ ! -d $(@D) ] && mkdir -p $(@D))
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@
	$(shell [ ! -d $(INCLUDE_DIR) ] && mkdir -p $(INCLUDE_DIR))
	$(COPY_CMD) *.h $(INCLUDE_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS) $(EXTERNAL_DEPS)
	$(shell [ ! -d $(@D) ] && mkdir -p $(@D))
	$(CC) -c -o $@ $< $(CCFLAGS) $(INCLUDE_PATH)

$(TCP_BIN): $(TCP_OBJ) $(LIB)
	$(shell [ ! -d $(@D) ] && mkdir -p $(@D))
	$(CC) -o $@ $(TCP_OBJ) $(CCFLAGS) $(LIB_PATH) -l$(LIB_NAME) -l:lib$(CLP_LIB_NAME).a

$(TCP_OBJ): $(TCP_CPP) $(DEPS) $(EXTERNAL_DEPS)
	$(shell [ ! -d $(@D) ] && mkdir -p $(@D))
	$(CC) -c -o $@ $(TCP_CPP) $(CCFLAGS) $(INCLUDE_PATH)

$(UDP_BIN): $(UDP_OBJ) $(LIB)
	$(shell [ ! -d $(@D) ] && mkdir -p $(@D))
	$(CC) -o $@ $(UDP_OBJ) $(CCFLAGS) $(LIB_PATH) -l$(LIB_NAME) -l:lib$(CLP_LIB_NAME).a

$(UDP_OBJ): $(UDP_CPP) $(DEPS) $(EXTERNAL_DEPS)
	$(shell [ ! -d $(@D) ] && mkdir -p $(@D))
	$(CC) -c -o $@ $(UDP_CPP) $(CCFLAGS) $(INCLUDE_PATH)

clean:
	$(DELETE_CMD) $(OBJS) $(LIB) $(INCLUDE_DIR)/*.h $(TCP_BIN) $(UDP_BIN)
