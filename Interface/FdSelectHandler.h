/*
 * FdSelectHandler.h
 *
 *  Created on: 04.10.2012
 *      Author: lode
 */

#ifndef FDSELECTHANDLER_H_
#define FDSELECTHANDLER_H_

#include <sys/time.h>

/**
 * Diese Klasse kapselt die Details von select()-Aufrufen an File-Deskriptoren.
 * Abgeleitete Klassen implementieren die Reaktion auf Ereignisse am File-Deskriptor.
 */
class FdSelectHandler
{
public:
  FdSelectHandler();
  virtual ~FdSelectHandler();

  void addTo(FdSelectHandler* &chain); ///< append the object to given the chain
  void removeFromChain(); ///< remove object from the chain, it is in

  /**
   * Diese Methode dient dem Abbau der Verbindung und allen damit verbundenen Aufräum-Arbeiten.
   *
   * Dynamisch allozierte Objekte stehen nach diesem Aufruf nicht mehr zur Verfügung.
   * Statisch erzeugte Objekte dürfen nicht mehr benutzt werden, ihre Funktion ist zerstört.
   */
  virtual
  void destroy() =0;

  /***
   * Diese statische Methode wartet auf die Verfügbarkeit aller FdSelectHandler-Objekte
   * der chain, so sie eine Wartebedingung konfiguriert haben.
   *
   * Wird kein Timeout (NULL) angegeben, so wird endlos gewartet.
   * Läuft der angegebene Timeout ab, ohne daß ein FdSelectHandler-Objekt verfügbar wird
   * (schreibbar oder lesbar), so gibt die Methode false zurück, sonst true.
   *
   * Die Methode verwendet die pselect()-Routine des Systems für die Ausführung des Warte-Vorgangs.
   *
   * @param chain ist die Kette der beobachteten Objekte
   * @param relativeTimeout ist die Zeitvorgabe, wie lange die Kette beobachtet werden soll, oder 0 (NULL) für endlos.
   * @return true, wenn mindestens ein Objekt verfügbar wurde, bevor der Timeout auslief, sonst (bei Timeout) false
   */
  static
  bool waitOn(FdSelectHandler* &chain, struct timespec* relativeTimeout = 0);

  /***
   * Diese Methode ruft waitOn() auf mit einem Timeout von 0 (keine Wartezeit).
   *
   * Es wird true zurückgegeben, wenn ein Objekt für I/O verfügbar war, sonst false.
   * @param chain ist die Kette der beobachteten Objekte
   * @return true, wenn mindestens ein Objekt verfügbar war, sonst (bei Timeout) false
   */
  static
  bool checkAll(FdSelectHandler* &chain); ///< executes a select on all descriptors of the given chain with zero timeout

protected:
  bool waitForRead:1;   ///< Dieses Flag muss von der erbenden Klasse nach Bedarf gesetzt werden.
  bool waitForWrite:1;  ///< Dieses Flag muss von der erbenden Klasse nach Bedarf gesetzt werden.

  FdSelectHandler *prev, *next, **chain;

  virtual
  void execute(bool readable, bool writable) =0;  ///< this implements the workhorse for reacting to readable and writable I/O

  virtual
  int fd() =0;  ///< this must return the fd of object
};

#endif /* FDSELECTHANDLER_H_ */
