/*
 * NpQid.h
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#ifndef NPQID_H_
#define NPQID_H_

#include <stdint.h>

namespace Npfs
{

/***
 * Diese Klasse ist eine interne Klasse des 9P2000 Servers.
 * Die Repräsentation eines QID.
 */
class NpQid
{
public:
  NpQid();
//  virtual ~NpQid();

  uint8_t    type;
  uint32_t   version;
  uint64_t   path;

};

}

#endif /* NPQID_H_ */
