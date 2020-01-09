/*
 * NpStat.cpp
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#include "NpStat.h"
#include "NpfsConstants.h"

namespace Npfs
{

NpStat::NpStat()
: type(0), dev(0), atime(0), mtime(0), length(0)
{
  setMode(0644);
}

NpStat::NpStat(InitWstat)
{
  cleanWstat();
}

NpStat::NpStat(const NpStat& other)
{
  *this = other;
}

NpStat::NpStat(ForceDirStat, const NpStat& other)
{
  *this = other;
  mode |= Dmdir;
  qid.type |= Qtdir;
}

NpStat::NpStat(CreateEntry, uint32_t perm, const NpStr& name, const NpStr& user, const NpStr& gid)
: NpStat()
{
  setMode(perm);
  this->name = name;
  this->uid  = user;
  this->muid = user;
  this->gid  = gid;
  updateStatSize();
}

//NpStat::~NpStat()
//{
//  // Auto-generated destructor stub
//}

unsigned NpStat::statSize() const
{
  unsigned size = 0;

  size = 2 + 4 + 13 + 4 +  /* type[2] dev[4] qid[13] mode[4] */
    4 + 4 + 8 +    /* atime[4] mtime[4] length[8] */
    8;     /* name[s] uid[s] gid[s] muid[s] */

  size += name.len;
  size += uid.len;
  size += gid.len;
  size += muid.len;

  return size;
}

void NpStat::updateStatSize()
{
  size = statSize();
}

void NpStat::setMode(uint32_t newMode)
{
  mode = newMode;
  qid.type = (newMode >> 24) & 0xff;
}

void NpStat::cleanWstat()
{
  size = ~((typeof(size)) 0);
  type = ~((typeof(type)) 0);
  dev = ~((typeof(dev)) 0);
  qid.path = ~((typeof(qid.path)) 0);
  qid.type = ~((typeof(qid.type)) 0);
  qid.version = ~((typeof(qid.version)) 0);
  mode = ~((typeof(mode)) 0);
  atime = ~((typeof(atime)) 0);
  mtime = ~((typeof(mtime)) 0);
  length = ~((typeof(length)) 0);
  name.len = 0;
  name.str = 0;
  uid.len = 0;
  uid.str = 0;
  gid.len = 0;
  gid.str = 0;
  muid.len = 0;
  muid.str = 0;
}


NpStat& NpStat::operator =(const NpStat& other)
{
//  this->size = other.size;
  this->type = other.type;
  this->dev = other.dev;
  this->qid = other.qid;
  this->mode = other.mode;
  this->atime = other.atime;
  this->mtime = other.mtime;
  this->length = other.length;
  this->name = other.name;
  this->uid = other.uid;
  this->gid = other.gid;
  this->muid = other.muid;
  this->updateStatSize();

  return *this;
}

}
