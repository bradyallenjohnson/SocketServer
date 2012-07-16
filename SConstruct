env = Environment()

sourceFiles = [
    'ServerHandler.cpp',
    'SocketServer.cpp',
    'TcpSocketServerImpl.cpp',
    'UdpSocketServerImpl.cpp',
]

env.Append(CPPPATH = '#.')
env.Library(target = 'socketServer', source = sourceFiles)
