/*
 * Connection.h
 *
 *  Created on: 02.07.2012
 *      Author: lode
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "MsgSerDes.h"

#include "Messages/Message.h"
#include "Messages/Tmessage.h"
#include "Messages/Rmessage.h"
#include "Messages/Rerror.h"

namespace Npfs
{

/***
 * Diese Klasse stellt die Verbindung zwischen einem gepufferten Interface
 * und dem Server her.
 *
 * Empfangene Messages werden deserialisiert und vom Server abgeholt.
 * Vom Server versendete Messages werden serialisiert und über den
 * Puffer verschickt.
 *
 * Der Puffer wird durch wiederholtes Aufrufen von runTxMsgQueue() abgearbeitet. Dies kann
 * durch eine externen Connection-Manager erfolgen, wenn die Liste der zu bearbeitenden
 * Verbindungen ohnhin bekannt ist. Einfacher ist der Aufruf der Server-Methode scheduleTx(),
 * die für alle dem Server bekannten Verbindungen runTxMsgQueue() aufruft.
 *
 * In der Implementierungsklasse müssen Fehler der Verbindung abgefangen und ggf.
 * durch einen separaten Manager bearbeitet werden, um den Disconnect vom Server
 * durchzuführen.
 */

class Connection: public MsgSerDes
{
public:
  Connection();
  virtual
  ~Connection();

  bool runTxMsgQueue(); ///< return true, if TX data is pending

  Tmessage* poll();  ///< der Server polled die Verbindung und bekommt, wenn vollständig vorliegend, eine Tmessage zurück, sonst Null

  void send(Rmessage* rMessage);  ///< der Server sendet Messages, die erst mal nur in einer Queue landen. Es ist sichergestellt, daß eine Message ungeteilt als ein Paket verschickt wird.
  void sendNoMemError(uint16_t tag = (uint16_t)(~0)); ///< sende eine vorbereitete NoMem-Message

  /***
   * Diese Methode ist muss durch die abgeleitete Klasse implementiert werden. Sie muss die maximal
   * verträgliche Nachrichtengröße (z.B. die Größe des Sendepuffers) zurück liefern.
   *
   * @note Die implementierende Klasse muss sicherstellen, daß Nachrichten möglichst ungeteilt als ein Datenblock
   * verschickt werden.
   */
  virtual unsigned maxMsgSize() const = 0;

  uint32_t msize() const; ///< liefert die für die Verbindung gesetzte msize zurück.
  void setMsize(uint32_t newMsize); ///< stellt die für die Verbindung festgelegte/ausgehandelte msize ein.

protected:
  bool connectionAlive;
  unsigned char messageHeader[Message::HeaderSize];
  bool headerRead;
  uint32_t currentMsize;


  virtual unsigned bytesReadable() = 0;
  virtual unsigned bytesWritable() = 0;

  virtual unsigned readData(unsigned char* buf, unsigned length) = 0;
  virtual void startTxPacket() = 0;
  virtual unsigned sendData(const unsigned char* buf, unsigned length) = 0;
  virtual void finishTxPacket() = 0;

  virtual bool driveTransmitter() = 0;  ///< return true, if TX is pending
  virtual void doAliveCheck() = 0;  ///< this is called after polling the connection and before sending, should clear connectionAlive on error

  Npfs::Rerror rErrorNoMem;
  static const char EnoMemory[];

  Npfs::Rmessage* txQueue;
  Npfs::Tmessage* rxQueue;
};

inline
uint32_t Connection::msize() const
{
  return currentMsize;
}

inline
void Connection::setMsize(uint32_t nMsize)
{
  currentMsize = nMsize;
}

}

#endif /* CONNECTION_H_ */
