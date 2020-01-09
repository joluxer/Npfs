/*
 * Rerror.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RERROR_H_
#define RERROR_H_

#include "Rmessage.h"
#include "../NpStr.h"

namespace Npfs
{

class Rerror: public Npfs::Rmessage
{
public:
  Rerror(const Npfs::Tmessage& tref, const char* ename);
  Rerror(const Npfs::Tmessage& tref, const NpStr& ename);
  Rerror(MsgSerDes& io, uint16_t tag, const char* _ename);
//  virtual ~Rerror();

  virtual uint32_t msgLength() const;
  virtual void serialize(void);

  NpStr ename;

  void setTag(uint16_t tag);

};

inline
void Rerror::setTag(uint16_t tag)
{
  tagValue = tag;
}

}

#endif /* RERROR_H_ */
