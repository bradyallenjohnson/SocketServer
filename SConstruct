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
clpPath = '../cmdLineParser'
exampleEnv.Append(CPPPATH = [clpPath, '#'], LIBPATH = [clpPath, '#'], LIBS = ['CmdLineParser', 'socketServer'])
exampleTcpTarget = exampleEnv.Program(target = 'exampleTcp', source = ['EchoTcpServerHandler.cpp', 'exampleMainTcp.cpp'])
exampleUdpTarget = exampleEnv.Program(target = 'exampleUdp', source = ['EchoUdpServerHandler.cpp', 'exampleMainUdp.cpp'])
exampleEnv.Alias('example', [exampleTcpTarget, exampleUdpTarget])
