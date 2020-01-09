/*
 * LinuxNpfsTcpServer.h
 *
 *  Created on: 04.10.2012
 *      Author: lode
 */

#ifndef LINUXNPFSSERVER_H_
#define LINUXNPFSSERVER_H_

#include "FdSelectHandler.h"

#include "TcpSockets/TcpServerSocket.h"
#include "Npfs/Server.h"

/**
 * Diese Klasse implementiert einen FdSelectHandler für die Wartung und Pflege eines
 * TCP-Server-Sockets. Dieses Objekt wird in einer Kette mit anderen Objekten gepflegt.
 *
 * Das Server-Objekt aktzeptiert eingehende Verbindungen und erzeugt für jede akzeptierte Verbindung
 * mittels new ein Objekt vom Typ LinuxNpfsTcpConnection und richtet es passend ein. Das neue Objekt
 * wird in die Kette der bestehenden Objekte, zu der der Server gehört, eingebunden.
 */
class LinuxNpfsTcpServer: public FdSelectHandler
{
public:
  /**
   * Der Konstruktor.
   * @param portNum TCP Portnummer für den 9p-Server
   * @param msize msize für den 9p-Server
   * @param resServer der 9p Resourcen-Server
   * @param triggerServerFlag eine Trigger-Rückmeldung, welche die Hauptschleife zum Triggern des 9p-Servers auffordert (unverarbeitete Daten liegen an)
   */
  LinuxNpfsTcpServer(int portNum, unsigned msize, Npfs::Server& resServer, bool& triggerServerFlag);
//  virtual ~LinuxNpfsTcpServer();

  virtual
  void destroy();

  bool hasPosixError() const;
  int getErrno() const;

protected:
  TcpServerSocket serverSocket;
  Npfs::Server& npfsServer;
  unsigned msize;
  bool& triggerServerFlag;

  virtual
  void execute(bool readable, bool writable);  ///< this implements the workhorse for reacting to readable and writable I/O

  virtual
  int fd();  ///< this must return the fd of object
};

inline
bool LinuxNpfsTcpServer::hasPosixError() const
{
  return serverSocket.hasPosixError();
}

inline
int LinuxNpfsTcpServer::getErrno() const
{
  return serverSocket.getErrno();
}

#endif /* LINUXNPFSSERVER_H_ */
