/*
 * Connection.cpp
 *
 *  Created on: 02.07.2012
 *      Author: lode
 */

#include "Connection.h"
#include "NpfsConstants.h"

#include <assert.h>

namespace Npfs
{

const char Connection::EnoMemory[] = "Cannot allocate memory";

Connection::Connection()
: connectionAlive(true), headerRead(false), currentMsize(0), rErrorNoMem(*this, NOTAG, EnoMemory),
  txQueue(0), rxQueue(0)
{

}

Connection::~Connection()
{
  // Auto-generated destructor stub
  // intentionally left empty
}

Tmessage* Connection::poll()
{
  Tmessage* tmsg = 0;

  if (connectionAlive)
  {
    // TODO: RX-Timeout noch implementieren

    // lese zuerst Daten aus der Verbindung und versuche diese zu verarbeiten
    unsigned rxLen = 0;

    // pruefe, ob schon Header Daten vorliegen
    if (!headerRead)
    {
      // verfuegbare Datenmenge abfragen
      rxLen = bytesReadable();
      if (rxLen > Message::HeaderSize)
      {
        // wenn Header vorliegt, diesen einlesen
        unsigned readLen = readData(messageHeader, Message::HeaderSize);
        rbyteCount += readLen;

        // Paranoia check
        assert(Message::HeaderSize == readLen);
        headerRead = true;    // der Header liegt jetzt vor
      }
    }
    else
    {
      rxLen = Message::HeaderSize + bytesReadable();
    }

    // checken, ob eine ganze Message vorliegt
    if (headerRead && Tmessage::isBufferComplete(messageHeader, rxLen))
    {
      // Message einlesen
      tmsg = Tmessage::create(mm, messageHeader, *this, maxMsgSize());
      headerRead = false; // der Header-Buffer ist nun wieder frei
      rbyteCount = 0;      // für eine Message sind alle Bytes gelesen
      if (tmsg)
      {
        // Nachricht in Warteschlange legen
        if (!rxQueue)
        {
          rxQueue = tmsg;
        }
        else
        {
          rxQueue->append(tmsg);
        }
      }
    }

    // dem Server eine Message liefern
    if (rxQueue)
    {
      // die erste Nachricht der Kette verarbeiten
      tmsg = rxQueue;
      rxQueue = tmsg->removeNext();
    }
    else
    {
      tmsg = 0;
    }
  }

  doAliveCheck();

  return tmsg;
}

void Connection::send(Rmessage* msg)
{
  doAliveCheck();

  if (connectionAlive)
  {
    if (msg)
    {
      // Nachricht in Warteschlange legen
      if (!txQueue)
      {
        txQueue = msg;
      }
      else
      {
        txQueue->append(msg);
      }
    }

  }
}

bool Connection::runTxMsgQueue()
{
  // sende ausstehende Daten raus
  unsigned i;
  static const unsigned burstTxTries = 4;
  Rmessage* msg;

  for (i = 0; txQueue && bytesWritable() && (i < burstTxTries); i++)
  {
    // die erste Nachricht der Kette verarbeiten
    msg = txQueue;
    unsigned txSize = msg->msgLength();
    if (bytesWritable() >= txSize)
    {
      txQueue = msg->removeGetNext();
      startTxPacket();
      msg->serialize();
      finishTxPacket();
      if (&rErrorNoMem != msg)
        delete msg;
    }
    else
    {
      break;
    }
  }

  bool callAgain = driveTransmitter();
  callAgain |= (0 != txQueue);

  return callAgain;
}

void Connection::sendNoMemError(uint16_t tag)
{
  rErrorNoMem.setTag(tag);
  send(&rErrorNoMem);
}

}
