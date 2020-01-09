/*
 * NpStat.h
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#ifndef NPSTAT_H_
#define NPSTAT_H_

#include "NpStr.h"
#include "NpQid.h"

#include <stdint.h>

namespace Npfs
{

/***
 * Diese Klasse ist eine interne Klasse des 9P2000 Servers.
 */
class NpStat
{
public:
  enum InitWstat {};
  enum CreateEntry {};
  enum ForceDirStat {};
  NpStat();
  NpStat(InitWstat);
  NpStat(const NpStat& other);
  NpStat(ForceDirStat, const NpStat& other);
  NpStat(CreateEntry, uint32_t perm, const NpStr& name, const NpStr& user, const NpStr& gid);
//  virtual ~NpStat();

  uint16_t size;
  uint16_t type;
  uint32_t dev;
  NpQid qid;
  uint32_t mode;
  uint32_t atime;
  uint32_t mtime;
  uint64_t length;
  NpStr name;
  NpStr uid;
  NpStr gid;
  NpStr muid;

  unsigned statSize() const;
  void updateStatSize();

  void setMode(uint32_t newMode);
  void setType(uint32_t newType);
  void setType(uint8_t newType);

  void cleanWstat();

  NpStat& operator =(const NpStat& other);
};

}

#endif /* NPSTAT_H_ */
