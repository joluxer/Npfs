/*
 * Rflush.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RFLUSH_H_
#define RFLUSH_H_

#include "Rmessage.h"
#include "Tflush.h"

namespace Npfs
{

class Rflush: public Npfs::Rmessage
{
public:
  Rflush(const Npfs::Tflush& tref);
//  virtual ~Rflush();

  virtual void serialize(void);
};

}

#endif /* RFLUSH_H_ */
