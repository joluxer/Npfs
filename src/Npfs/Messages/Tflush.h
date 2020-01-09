/*
 * Tflush.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef TFLUSH_H_
#define TFLUSH_H_

#include "Tmessage.h"

namespace Npfs
{

class Tflush: public Npfs::Tmessage
{
public:
  Tflush(MsgSerDes& io, uint16_t tag);
//  virtual ~Tflush();

  uint16_t oldTag;
};

}

#endif /* TFLUSH_H_ */
