/*
 * Rread.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RREAD_H_
#define RREAD_H_

#include "Rmessage.h"
#include "Tread.h"

namespace Npfs
{

class Rread: public Npfs::Rmessage
{
  MemoryManager *const mm;
public:
  Rread(const Npfs::Tread& tref, MemoryManager* mm);
  virtual ~Rread();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  uint32_t count;
  unsigned char* data;

private:
};

}

#endif /* RREAD_H_ */
