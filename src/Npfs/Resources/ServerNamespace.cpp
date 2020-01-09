/*
 * ServerNamespace.cpp
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#include "ServerNamespace.h"

#include <string.h>
#include <assert.h>

namespace Npfs
{

ServerNamespace::ServerNamespace(const char* aname)
: detachHandler(this, &ServerNamespace::doDetach),
  aname(aname), myRootDir(aname), attachCount(0), nextNs(0)
{}

ServerNamespace::~ServerNamespace()
{
  // Auto-generated destructor stub
  // intentionally left empty
}

ServerNamespace& ServerNamespace::mount(Resource& rootEntry)
{
  myRootDir.mount(rootEntry);

  return *this;
}

bool ServerNamespace::isResponsibleFor(const NpStr& nsAname)
{
  if (aname)
  {
    if (nsAname.len)
    {
      return ((strlen(aname) == nsAname.len) && (0 == strncmp(aname, nsAname.str, nsAname.len)));
    }
    else
    {
      return false;
    }
  }
  else
  {
    return true;
  }
}

ServerNamespace* ServerNamespace::searchFor(const NpStr& nsAname)
{
  ServerNamespace* ns = this;

  if (aname)
  {
    for(; ns != 0; ns = ns->nextNs)
    {
      if (ns->isResponsibleFor(nsAname))
      {
        break;
      }
    }
  }

  return ns;
}

Directory* ServerNamespace::attach(const NpStr& nsAname)
{
  Directory* result = 0;

  if (this->isResponsibleFor(nsAname))
  {
    result = &myRootDir;
    attachCount++;
  }

  return result;
}

bool ServerNamespace::doDetach(Directory* root)
{
  return this->detach(root);
}

bool ServerNamespace::detach(Directory* root)
{
  assert(0 < attachCount);
  attachCount--;

  return false;
}

ServerNamespace& ServerNamespace::append(ServerNamespace& newNs)
{
  ServerNamespace* ns;

  for (ns = this; ns->nextNs != 0; ns = ns->nextNs)
    ;

  newNs.nextNs = 0;
  ns->nextNs = &newNs;

  return newNs;
}

}
