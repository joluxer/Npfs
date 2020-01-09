/*
 * Rmessage.h
 *
 *  Created on: 17.07.2012
 *      Author: jlode
 */

#ifndef RMESSAGE_H_
#define RMESSAGE_H_

#include "Message.h"
#include "Tmessage.h"

namespace Npfs
{

class Rmessage: public Npfs::Message
{
public:
//  virtual ~Rmessage();

  virtual uint32_t msgLength() const; ///< die default-Implementation gibt Null zurück
  virtual void serialize(void) = 0;

  void append(Rmessage* m);  ///< sucht das Ende der Kette und haengt die angegebene Nachricht an
  Rmessage* removeGetNext();    ///< liest den next-Pointer aus und setzt ihn dann zu Null

protected:
  Rmessage(const Tmessage& tref);
  Rmessage(const Tmessage& tref, MessageType type);
  Rmessage(MsgSerDes& io, uint16_t tag, MessageType type);

  void serializeHeader();   ///< to be called at the beginning

  Rmessage* next;  ///< in den Queues im Code (Server, Connection etc.)
};

}

#endif /* RMESSAGE_H_ */
