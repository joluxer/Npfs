# CMakeLists.txt
# 
# Created on: 17.04.2014
#     Author: lode

ADD_LIBRARY(NpfsTcpServer STATIC
    LinuxNpfsTcpConnection.cpp
    LinuxNpfsTcpServer.cpp
    ${byteStreamUtil}/AsyncFifoBuffer.cpp
    ${byteStreamUtil}/AsyncFifo.cpp
    ${posixUtilInterface}/FdSelectHandler.cpp
    ${byteStreamInterface}/ByteStream.cpp
    ${byteStreamInterface}/ByteStreamDecorator.cpp
    ${byteStreamInterface}/SeekStream.cpp
    ${byteStreamInterface}/BufferedIo.cpp
)

TARGET_LINK_LIBRARIES(NpfsTcpServer
  tcpServer
)
