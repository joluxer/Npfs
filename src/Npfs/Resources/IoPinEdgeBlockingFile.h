/*
 * IoPinEdgeBlockingFile.h
 *
 *  Created on: 30.08.2015
 *      Author: lode
 */

#ifndef IOPINEDGEBLOCKINGFILE_H_
#define IOPINEDGEBLOCKINGFILE_H_

#include "IoPinFile.h"

#include <atomic>

namespace Npfs
{

/**
 * Diese Klasse implementiert die Funktionalität, welche im Linux-Kernel als
 * Interrupt-Fähigkeit eines I/O-Pins bereitgestellt wird, es können read()-Aufrufe
 * blockieren, bis die konfigurierte Flanke am Input aufgetreten ist.
 *
 * Das Pin wird immer beim open() eingelesen und danach auf die konfigurierte Flanke gewartet.
 *
 * @note Die Klasse ermittelt die erste relevante Flanke (im Fall von BothEdgeTigger), der
 * letzte Wert kann immer in einem IoPinFile direkt ausgelesen werden.
 */
class IoPinEdgeBlockingFile: public Npfs::IoPinFile
{
public:
  IoPinEdgeBlockingFile(const char* filename, Npfs::Directory* parent, IoPin& variable, uint16_t umask = 0177);
//  virtual ~IoPinEdgeBlockingFile();

  /**
   * Verbindet den angegebenen Handler mit der read()-Methode; der Handler wird
   * mit true aufgerufen, wenn der nächste read() blockieren wird (keine Daten
   * an den Client liefert), der Handler wird mit false gerufen, wenn der
   * nächste read() nicht blockiert (Daten liefert).
   *
   * Ohne einen solchen Handler kann/wird die read()-Methode eine hohe
   * CPU-Last verursachen, weil das Input-Pin immer wieder gepollt wird, bis
   * der read() Daten liefert.
   *
   * Mit einem solchen Handler wird auf inputChanged()-Aufrufe gewartet und
   * das Pin nicht gepollt.
   *
   * @param handler
   */
  void connectBlockHandler(const Callback<void, bool>* handler);

  /**
   * Diese Meldung muss an das Objekt geliefert werden, wenn ein Block-Handler
   * angeschlossen ist, weil sonst die Blockierung nicht aufgehoben, wird.
   *
   * Diese Methode muss aufgerufen werden, um die read()-Blockierung zu lösen
   * und den damit erreichten Zustand mitzuteilen.
   *
   * Die Methode darf immer für beide Flanke aufgerufen werden, die Klasse sortiert
   * die konfigurierte Flanke selber aus, es ist jedoch erlaubt, nur die
   * Trigger-Flanken mitzuteilen.
   *
   * @note Diese Methode ist threading- und interrrupt-sicher realisiert.
   *
   * @param isActive
   */
  void inputChanged(bool isActive);

  bool lastKnownState() const;

  typedef enum
  {
    NoEdgeTrigger,
    RisingEdgeTrigger,
    FallingEdgeTrigger,
    BothEdgeTigger
  } EdgeTriggerType;

  bool getEdgeMode(EdgeTriggerType& trg);
  bool setEdgeMode(EdgeTriggerType& trg);

  bool getEdgeCountClear(uint32_t& cnt);

protected:
  std::atomic<uint32_t> edgeCount;
  const Callback<void, bool> *fileReadWillBlock;
  std::atomic<bool> lastInputState;
  bool blockForRisingEdge:1,
       blockForFallingEdge:1;

  struct IoBufferForBlocking: public IoBuffer
  {
    IoBufferForBlocking(IoBufferForBlocking * &anchor);
    ~IoBufferForBlocking();
    std::atomic<bool> hadTriggerEdge;
    bool triggerState;
    IoBufferForBlocking **prev;
    IoBufferForBlocking * next;
  } *clients;

  virtual
  IoBuffer* makeIoBuffer() override;

  virtual
  bool blockRead(IoBuffer& workRef) override; // wird in open() gerufen, gibt true zurück, wenn der read()-Aufruf blockiert werden soll, z.B. bis der Input ein Flanke erfüllt, die Default-Implementation gibt false zurück

  virtual
  bool releaseRead(IoBuffer& workRef) override; // wird in read() gerufen, gibt true zurück, wenn der read()-Aufruf den Wert lesen soll, die Default-Implementation gibt true zurück

  virtual
  const char* readValue(IoBuffer& workRef) override; // wird in open() oder read() gerufen, abhängig davon, ob blockRead() false oder true ergibt, gibt den Wert zurück, der an den Nutzer ausgegeben werden soll, die Default-Implementation gibt den aktuellen Wert für das IoPin zurück

//  virtual
//  bool isWriteAccepted(IoBuffer& workRef, const unsigned char* buffer, unsigned bufLen, uint64_t offset);  // hier kann eine abgeleitete Klasse weitere Strings und Aktionen akzeptieren; die Default-Implementation gibt false zurück

  virtual
  void setEdgeTriggerInHw(EdgeTriggerType t); // die Default-Implementation ist leer
};

inline
bool IoPinEdgeBlockingFile::lastKnownState() const
{
  return lastInputState;
}

} /* namespace Npfs */
#endif /* IOPINEDGEBLOCKINGFILE_H_ */
