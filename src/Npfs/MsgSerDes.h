/*
 * MsgSerDes.h
 *
 *  Created on: 18.07.2012
 *      Author: jlode
 */

#ifndef MSGSERDES_H_
#define MSGSERDES_H_

#include <stdint.h>

#include "NpStr.h"
#include "NpQid.h"
#include "NpStat.h"

#include "MemoryManager.h"

namespace Npfs
{

/***
 * This class serializes and derserializes data types of 9P2000 messages
 * from and to a transport, that is implemented in spezialisations of this
 * base class.
 */

class MsgSerDes
{
public:
  MsgSerDes();
  virtual ~MsgSerDes();

  void connectMemMan(MemoryManager* mm);  // wird vom Server benutzt, um den MemoryManager einzusetzen

  uint64_t getUI64();
  uint32_t getUI32();
  uint16_t getUI16();
  uint8_t getUI8();

  void get(NpQid& qid);
  void get(unsigned char* &buffer, uint32_t count); ///< buffer muss mit der release() Methode wieder freigegeben werden
  void release(unsigned char* &buffer);
  void get(NpStr& string); ///< NpCstr.str muss mit der release() Methode wieder freigegeben werden
  void release(NpStr& string);

  unsigned consumedByteCount() const;

  void putUI64(uint64_t v);
  void putUI32(uint32_t v);
  void putUI16(uint16_t v);
  void putUI8(uint8_t v);

  void put(const NpStr& string);
  void put(const NpQid& qid);
  void put(const unsigned char* buffer, uint32_t count);
  void put(const NpStat& stat);

protected:
  MemoryManager* mm;
  unsigned rbyteCount;
  virtual unsigned readData(unsigned char* readBuffer, unsigned readLength) = 0;
  virtual unsigned sendData(const unsigned char* writeBuffer, unsigned writeLength) = 0;
};

inline
void MsgSerDes::connectMemMan(MemoryManager* mm_)
{
  mm = mm_;
}

inline
unsigned MsgSerDes::consumedByteCount() const
{
  return rbyteCount;
}

}

#endif /* MSGSERDES_H_ */
