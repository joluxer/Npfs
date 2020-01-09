/*
 * BufferedIo.h
 *
 */

#ifndef BUFFEREDIO_H_
#define BUFFEREDIO_H_

/**
 * Diese abstrakte Klasse beschreibt das BufferedIo-Interface. Dies stellt eine Ergänzung zum
 * ByteStream-Interface dar, um auf die Verfügbarkeiten von evtl. vorhandenen Puffern einer
 * Interface-Implementation zugreifen zu können.
 *
 * @note Die Abtrennung in eine eigene Inteface-Klasse neben ByteStream ermöglicht eine schlankere
 * Implementation im embedded-Bereich. Die Bereistellung eines BufferedIo-Interfaces erfolgt wesentlich
 * seltener und ausgewählter, so daß sich die Abtrennung in der Praxis bewährt hat.
 */
class BufferedIo
{
public:
//  BufferedIo();
  virtual
  ~BufferedIo();

  /**
   * Gibt den freien Platz im Puffer des Ausgangsdatenstrom zurück.
   *
   * Die zurückgegebene Anzahl an Bytes kann non-blocking auf einmal in den Ausgangsdatenstrom
   * geschrieben werden.
   *
   * Die Default-Implementation gibt 0 zurück.
   *
   * \return Platz im Ausgabepuffer
   */
  virtual unsigned bytesWritable() const;

  /**
   * Gibt die Anzahl der ungelesenen Bytes im Eingangs-Datenstrom zurück.
   *
   * Die Default-Implementation gibt 0 zurück.
   */
  virtual unsigned bytesReadable(void) const;

  /**
   * Diese Methode sendet alle zwischengespeicherten Daten zum Device/File hinaus. Wenn
   * die Methode false zurückliefert, sind alle gepufferten Daten garantiert geschrieben und die
   * Puffer könnten sofort zerstört werden. Anderfalls ist ein weiterer Aufruf nötig/empfohlen, um
   * weitere Puffer zu leeren.
   *
   * Die Default-Implementation gibt immer false zurück.
   *
   * @return false, wenn alle Puffer geleert sind, sonst true, wenn noch Daten in Puffern liegen.
   */
  virtual bool flush(void);

  /**
   * Übergibt dem Objekt einen entsprechenden Datenpuffer für den Eingangs- (Read-) oder Ausgangs- (Write-) Datenstrom.
   *
   * Die Default-Implementation ignoriert die übergebenen Datenpuffer einfach.
   */
  virtual void setBigWriteBuffer(unsigned char *bigBuffer, unsigned bufferSize);
  virtual void setBigReadBuffer(unsigned char *bigBuffer, unsigned bufferSize);

};

#endif /* BUFFEREDIO_H_ */
