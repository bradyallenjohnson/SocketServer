env = Environment()

sourceFiles = [
    'ServerHandler.cpp',
    'SocketServer.cpp',
    'TcpSocketServerImpl.cpp',
    'UdpSocketServerImpl.cpp',
]

env.Append(CPPPATH = '#.')
libTarget = env.Library(target = 'socketServer', source = sourceFiles)
env.Default(libTarget)

exampleEnv = env.Clone()
clpPath = '../CommandLineParser'
exampleEnv.Append(CPPPATH = [clpPath, '#'], LIBPATH = [clpPath, '#'], LIBS = ['CmdLineParser', 'socketServer'])
exampleTarget = exampleEnv.Program(target = 'example', source = ['EchoTcpServerHandler.cpp', 'exampleMain.cpp'])
exampleEnv.Alias('example', exampleTarget)
