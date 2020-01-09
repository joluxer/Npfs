/*
 * LinuxNpfsTcpConnection.h
 *
 *  Created on: 04.10.2012
 *      Author: lode
 */

#ifndef LINUXNPFSCONNECTION_H_
#define LINUXNPFSCONNECTION_H_

#include "FdSelectHandler.h"

#include "TcpSockets/TcpSocket.h"
#include "Npfs/Server.h"
#include "Npfs/Connection.h"
#include "Util/AsyncFifoBuffer.h"
#include "Callback.h"

/**
 * Diese Klasse stellt eine aktive Client-Verbindung dar, die in einer Kette
 * von Objekten via select() gemanaged wird.
 *
 * @note Objekte dieser Klasse müssen mit new erzeugt werden und sollten nur
 * von der zugehörigen Server-Klasse (LinuxNpfsTcpServer) erzeugt werden. Sie
 * zerstören sich selber ("delete this"), wenn die Netzwerkverbindung nicht mehr
 * intakt ist.
 */
class LinuxNpfsTcpConnection: public FdSelectHandler, public Npfs::Connection
{
public:
//  virtual ~LinuxNpfsTcpConnection();

  TcpSocket& getSocket();

  void limitMaxMsgSize(unsigned msize);

  virtual unsigned maxMsgSize() const;

  virtual
  void destroy();

protected:
  TcpSocket socket;
  AsyncFifoBuffer ioBuffer;
  CalleeMember<LinuxNpfsTcpConnection, void, bool> txAvailableHandler;
  Npfs::Server& npfsServer;
  bool& triggerServer;

  static const unsigned IoBufferSize = 8000;  // bei den Tests zeigte der Linux-Client im Kernel eine Fehlfunktion bei Lese-Anforderungen oberhalb von 1437 Bytes
  unsigned char rxBuffer[IoBufferSize], txBuffer[IoBufferSize];

  unsigned bufferMaxMsgSize;

  void startAsyncTx(bool txReady);

  // LinuxFdSelectHandler Interface
  virtual
  void execute(bool readable, bool writable);  ///< this implements the workhorse for reacting to readable and writable I/O

  virtual
  int fd();  ///< this must return the fd of object

  // Npfs::Connection Interface
  virtual unsigned bytesReadable();
  virtual unsigned bytesWritable();

  virtual unsigned readData(unsigned char* buf, unsigned length);
  virtual void startTxPacket();
  virtual unsigned sendData(const unsigned char* buf, unsigned length);
  virtual void finishTxPacket();

  virtual bool driveTransmitter();  ///< return true, if TX is pending
  virtual void doAliveCheck();

private:
  LinuxNpfsTcpConnection(Npfs::Server& resServer, bool& triggerServerFlag);
  friend class LinuxNpfsTcpServer;
};

inline
TcpSocket& LinuxNpfsTcpConnection::getSocket()
{
  return socket;
}

#endif /* LINUXNPFSCONNECTION_H_ */
