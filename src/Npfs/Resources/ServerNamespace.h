/*
 * ServerNamespace.h
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#ifndef SERVERNAMESPACE_H_
#define SERVERNAMESPACE_H_

#include "Resource.h"
#include "Directory.h"
#include "../NpStr.h"
#include "Callback.h"

namespace Npfs
{

/***
 * This class represents a name space for the 9P2000 server.
 *
 * A name space is a collection of ressources, that are to be mounted together, when a special
 * name space of the server is requested.
 *
 * Each namespace is referenced by its name.
 */
class ServerNamespace
{
public:
  /***
   * Constructing the name space with the identifier aname. If no aname is given, this
   * is a catchall namespace, that will be responsible for all name space identifier,
   * it is asked for.
   *
   * @param aname the attach name, the namespace is responsible for.
   */
  ServerNamespace(const char* aname = 0);
  virtual ~ServerNamespace();

  /***
   * This method mounts the given ressource in the rootDirectory of the ServerNamespace.
   *
   * @param rootEntry
   * @return
   */
  ServerNamespace& mount(Resource& rootEntry);

  /***
   * This method chains up the newNs name space to the current chain of name spaces,
   * represented by the called name space object.
   *
   * @param newNs
   * @return
   */
  ServerNamespace& append(ServerNamespace& newNs);

  /***
   * This method returns the root directory of the called name space.
   *
   * @note The name of the directory is the same as the name of the name space.
   *
   * @return
   */
  Directory& rootDirectory();

  /***
   * This method queries the chain of name spaces represented by the called object for
   * an ServerNamespace object, that is responsible for the given aname.
   *
   * @param aname
   * @return
   */
  ServerNamespace* searchFor(const NpStr& aname);

  /***
   * This method is used internally by the Fattach fcall of the 9P2000 server.
   *
   * It returns the root directory of the name space object, if it is responsible for the
   * name space in question.
   * @param aname
   * @return
   */
  virtual
  Directory* attach(const NpStr& aname);

  /***
   * This method is used internally by the 92000 server routines.
   *
   * It returns true, if the name space object is responsible for the name space in question.
   * @param aname
   * @return
   */
  virtual
  bool isResponsibleFor(const NpStr& aname);

  /***
   * This member is used internally by the Fclunk fcall of the 9P2000 server.
   *
   */
  const CalleeMember<ServerNamespace, bool, Directory*> detachHandler;
protected:
  const char* aname;
  Directory myRootDir;
  unsigned attachCount;

  ServerNamespace* nextNs;

  /***
   * This method is used internally by the Fclunk fcall of the 9P2000 server.
   *
   * @return true, while detach is running, false after completion
   */
  virtual
  bool detach(Directory* root);

private:
  bool doDetach(Directory* root);
};

inline
Directory& ServerNamespace::rootDirectory()
{
  return myRootDir;
}

}

#endif /* SERVERNAMESPACE_H_ */
