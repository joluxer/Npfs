/*
 * AsyncFifo.h
 *
 */

#ifndef ASYNCFIFO_H_
#define ASYNCFIFO_H_

#include "ByteStreamDecorator.h"
#include "Callback.h"

#include <atomic>

/***
 * Diese Klasse ist ein Decorator, der ein Objekt mit Bytestream-Interface
 * um eine FIFO-Pufferung erweitert. Für beide Datenfluss-
 * Richtungen existieren getrennte Puffer. Es handelt sich um eine Full-Duplex-Verbindung.
 *
 * Bei Puffer-Überlauf werden neue Daten nicht mehr abgeholt (RX/read) bzw.
 * nicht angenommen (TX/write).
 *
 * Der Transport der Daten vom/zum Backend-Objekt erfolgt durch die Methoden sendDownstream() und
 * receiveUpstream(). Diese müssen durch einen Operator (z.B. ISR, Scheduler)
 * aufgerufen werden. Diese Methoden dürfen die read()/write()-Methoden unterbrechen;
 * umgekehrt dürfen die read()/write()-Methoden die Upstream-/Downstream-Methoden nicht
 * unterbrechen (nicht in ISR aufgerufen werden, die Upstream/Downstream unterbrechen können).
 *
 * Zudem ist abfragbar, wie viel Platz noch in der jeweiligen Fifo verfügbar ist, um z.B.
 * XON/XOFF-Protokolle zu triggern.
 *
 * @note Wenn kein Puffer zugewiesen wird, dann verhält sich das Objekt transparent, ohne Fehlermeldung.
 */

class AsyncFifo: public ByteStreamDecorator
{
public:
  AsyncFifo(ByteStream& backend);
//  virtual ~AsyncFifo();

  typedef const Callback<void, bool> TxAvailableSignal; ///< This signal is sent with true, if data is available, it is sent with false, if all data is consumed.

  AsyncFifo& assignTxBufferSpace(unsigned char* buffer, unsigned bufferSize);
  AsyncFifo& assignRxBufferSpace(unsigned char* buffer, unsigned bufferSize);
  AsyncFifo& connectTxAvailableSignal(TxAvailableSignal* signal);

  void sendDownstream();
  void receiveUpstream();

  // Bytestream Interface
  virtual
  unsigned write(unsigned char b);
  virtual
  unsigned writeBlock(const unsigned char* blk, unsigned size);
  virtual
  unsigned read(unsigned char& b);
  virtual
  unsigned readBlock(unsigned char* blk, unsigned size);

  unsigned remainingRxSpace() const;
  unsigned remainingTxSpace() const;

protected:
  struct _Buffer
  {
    unsigned char *data;
    unsigned putIdx, getIdx;
    std::atomic<unsigned> fill;
    unsigned size;
  } txBuffer, rxBuffer;

  void tellTxDataAvailable();
  void tellTxBufferDrained();
private:
  TxAvailableSignal* txDataReadyHandler;
};

inline
AsyncFifo& AsyncFifo::connectTxAvailableSignal(TxAvailableSignal* signal)
{
  txDataReadyHandler = signal;

  return *this;
}

#endif /* ASYNCFIFO_H_ */
