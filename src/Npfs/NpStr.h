/*
 * NpStr.h
 *
 *  Created on: 20.07.2012
 *      Author: jlode
 */

#ifndef NPSTR_H_
#define NPSTR_H_

namespace Npfs
{

/***
 * Diese Klasse ist eine interne Klasse des 9P2000 Servers.
 *
 * In 9P2000-Implementationen werden Strings durch ihr Datenfeld und seine Länge
 * repräsentiert.
 */
class NpStr
{
public:
  NpStr();
  NpStr(const char* s);
  constexpr
  NpStr(const char* s, unsigned len)
  : len(len), str(s)
  {}
//  virtual ~NpCstr();

  unsigned len;
  const char* str;

  NpStr& operator=(const char* s);

  bool operator==(const NpStr& s) const;

};

}

#endif /* NPSTR_H_ */
