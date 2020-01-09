

SET(serverlib_src
    TcpServerSocket.cpp
    TcpSocket.cpp
    ${posixUtil}/FdIoStream.cpp
)

ADD_LIBRARY(tcpServer STATIC ${serverlib_src})

SET(clientlib_src
    TcpClientSocket.cpp
    TcpSocket.cpp
    ${posixUtil}/FdIoStream.cpp
)

ADD_LIBRARY(tcpClient STATIC ${clientlib_src})
