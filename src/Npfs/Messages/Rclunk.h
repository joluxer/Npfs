/*
 * Rclunk.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef RCLUNK_H_
#define RCLUNK_H_

#include "Rmessage.h"
#include "Tclunk.h"

namespace Npfs
{

class Rclunk: public Npfs::Rmessage
{
public:
  Rclunk(const Npfs::Tclunk& tref);
//  virtual ~Rclunk();

  virtual void serialize(void);
};

}

#endif /* RCLUNK_H_ */
