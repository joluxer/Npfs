/*
 * Tmessage.h
 *
 *  Created on: 17.07.2012
 *      Author: jlode
 */

#ifndef TMESSAGE_H_
#define TMESSAGE_H_

#include "Message.h"
#include "../NpStr.h"

namespace Npfs
{

class Tmessage: public Npfs::Message
{
protected:
  Tmessage(MsgSerDes& io, MessageType type, uint16_t tag = (uint16_t)(~0));

public:
//  virtual ~Tmessage();

  static bool isBufferComplete(unsigned char* bufferHeader, unsigned rxLength);

  static Tmessage* create(MemoryManager* mm, unsigned char* bufferHeader, MsgSerDes& io, unsigned maxMsgSize); ///< Messages werden mit new erzeugt, müssen mit delete gelöscht werden

  const NpStr* getError() const;

  void append(Tmessage* m);  ///< sucht das Ende der Kette und haengt die angegebene Nachricht an
  Tmessage* removeNext();    ///< liest den next-Pointer aus und setzt ihn dann zu Null

protected:
  Tmessage* next;  ///< in den Queues im Code (Server, Connection etc.)
  NpStr deserError;
};

}

#endif /* TMESSAGE_H_ */
