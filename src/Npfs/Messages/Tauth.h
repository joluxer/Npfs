/*
 * Tauth.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TAUTH_H_
#define TAUTH_H_

#include "Tmessage.h"
#include "../NpStr.h"

namespace Npfs
{

class Tauth: public Npfs::Tmessage
{
public:
  Tauth(MsgSerDes& io, uint16_t tag);
  virtual ~Tauth();

  uint32_t afid;
  NpStr uname;
  NpStr aname;
};

}

#endif /* TAUTH_H_ */
