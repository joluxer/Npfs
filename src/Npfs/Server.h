/*
 * Server.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "Resources/IoState.h"
#include "Resources/ServerNamespace.h"
#include "Fcalls/Fcall.h"
#include "Connection.h"
#include "Fid.h"

#include "MemoryManager.h"

/**
 * @namespace Npfs
 * @brief Der Namespace für den Npfs-Server-Code.
 * Der Npfs Namespace enthält die Implementation des 9P2000-Servers mit allen Ressourcen, die durch
 * den Code-Stamm bereit gestellt werden.
 */
namespace Npfs
{

/***
 * This class is the workhorse for serving incoming connections with
 * shared ressources from a ServerNamespace through a client connection
 * via 9P2000 conformant messages. This server class is independent from any
 * underlaying transport type.
 *
 * Incoming connections will be connected to the server by an external
 * connection management. The same connection management must disconnect
 * dead connections from the server.
 *
 * A fully functional 9P2000 server needs a server object, one or more ServerNamespace objects,
 * several Ressources mounted at a ServerNamespace, and one or more managed connections.
 */

class Server
{
public:
  /***
   * The constructor for the Server object.
   *
   * The given memory manager object is used for allocation of all internal
   * dynamic data structures: the T- and R-Messages, the FID-Pools for the connections, ConnectionListElement objects and the Fcalls.
   * @param mm is a MemoryManager object, must not be zero
   */
  Server(MemoryManager* mm);
//  virtual
  ~Server();

  /***
   * A setup routine, that provides the server with a name space object.
   * A namespace object holds the reference to a directory to attach to by aname.
   *
   * Several name spaces may be provided by several calls to this method. All provided
   * name spaces must be distinct and must have distinct names.
   *
   * @param ns The reference of a namespace object to serve a directory tree from.
   */
  void serve(ServerNamespace& ns);

  /***
   * This method connects given connection to the server.
   * The connection must be fully established and operational on transport level, as
   * the server expects client communication without further setup.
   *
   * @note: This method must be called from the same CPU context as the disconnect()
   * method and the operate() method (ie. must not be run in
   * parallel with the disconnect() or operate() method).
   *
   * @param conn the new incoming client connection, established on transport level
   * @return true, if connection was successful attached to the server, else false (should be disconnected from the outer space)
   */
  bool connect(Connection& conn);

  /***
   * This method disconnects the given connection from the server.
   * The connection does not need to be operational for the disconnection,
   * as the server does not use it any more in or after the call to this method.
   * All incomplete operations for this connection inside the server are
   * canceled/flushed/clunked to clean up the internal states.
   *
   * @note: This method must be called from the same CPU context as the connect()
   * method and the operate() method (ie. must not be run in parallel with the
   * connect() or operate() method).
   *
   * @param conn the old, probably dead, client connection
   */
  void disconnect(Connection& conn);

  /***
   * This method is the workhorse for the server function. It must be called in regular
   * intervals to schedule and drive the server operation.
   *
   * Pending and delayed actions of the server, resp. the resources, are scheduled and driven
   * by calls to this method (the server itself has no internal deferred or delayed actions).
   *
   * All open connections are polled for new incoming data and all open actions
   * in resources are tried to complete, when this method is called.
   *
   * @note: This method must be called from the same CPU context as the connect()
   * method and the disconnect() method (ie. must not be run in
   * parallel with the connect() or disconnect() method).
   *
   * @return true, if some operations are pending, thus the operate() method wants to be
   * called soon again, after other actions, that are needed to complete the pending operations.
   * Returns false, if no operations are pending.
   */
  bool operate();

  /***
   * This method schedules the transmission queue of the connections inside the server.
   *
   * All unsent data is tried to send. This method needs to be called at least at
   * the frequency of the operate() calls. If it returns true, the call frequency
   * can be higher to pump out the pending data.
   *
   * @note: This method must be called from the same CPU context as the connect()
   * method and the disconnect() method (ie. must not be run in
   * parallel with the connect() or disconnect() method).
   *
   * @return true, if there is still unsent data, which must be sent soon.
   */
  bool scheduleTx();

protected:
  MemoryManager* mm;
  struct ConnectionListElement
  {
    ConnectionListElement(Connection& c) : io(c), previous(0), next(0), fidPool(0), workQueue(0) {}
    Connection& io;
    ConnectionListElement *previous, *next;
    Fid** fidPool;
    Fcall* workQueue;

    void * operator new(size_t size, MemoryManager* mm) throw() // this class should be constructed using the NPFSNEW() macro to avoid null pointer referencing
    {
      return npfsNew(size, mm);
    }
    void operator delete(void * mem) throw()
    {
      npfsDelete(mem);
    }

  } *connections;

  Fid **orphanedFidPool, *orphanedFid;
  BlockingOpState* orphanCleanupState;

  ServerNamespace* srvNamespaces;

  void pollConnections();
  bool processWorkQueues(); // return true, if server has pending operations in the work queues (operations may still block)

  Fcall* createRequest(Tmessage* tMsg, ConnectionListElement* conn);
  bool processRequest(Fcall* req, ConnectionListElement* conn);  ///< return true, if processing is finished
};

}

#endif /* SERVER_H_ */
