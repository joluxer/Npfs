/*
 * BinaryStringSerDes.h
 *
 *  Created on: 31.08.2015
 *      Author: lode
 */

#ifndef BINARYSTRINGSERDES_H_
#define BINARYSTRINGSERDES_H_

#include "../NpStr.h"

#include <cstdint>
#include <utility>

namespace Npfs
{

class BinaryStringSerDesBase
{
protected:
//  BinaryStringSerDesBase();
//  virtual ~BinaryStringSerDes();

  unsigned serializeFromInt(int val, const std::pair<int, const NpStr *const>* array, const NpStr& UnknownValue, unsigned char* buffer, unsigned bufferLength);
  int deserializeToInt(const std::pair<int, const NpStr *const>* array, int DefaultValue, const unsigned char* buffer, unsigned bufferLength);
};


/**
 * Diese Klasse serialisiert einen Datentypen, der zu/von int konvertiert werden kann
 * (i.a. enum und andere Basis-Typen) in einen Text bzw. binären Datenstrom.
 * Sie deserialisiert den Datenstrom zu int und konvertiert diesen Wert dann zum
 * Ziel-Datentyp.
 *
 * Für identische Variablen-Typen können trotzdem unterschiedliche Texte implementiert werden, wenn
 * unterschiedliche VariantId's vergeben werden.
 *
 * One can use a macro like this for creating the command string objects
 * #define NpstrOf(STR) const char STR##String[] = #STR; \
 *   Npfs::NpStr Npstr##STR{STR##String, sizeof(STR##String) - 1}
 * together with const char arrays (string) in an anonymous namespace. This results per line in an NpStr constant of the name NpstrSTR
 *
 */
template<typename VT, unsigned VariantId = 0>
class BinaryStringSerDes: public BinaryStringSerDesBase
{
public:
  typedef VT VarType;
  typedef std::pair<int, const NpStr *const> ValuePair;

  BinaryStringSerDes(VarType& myVar)
  : variable(myVar)
  {}
//  virtual ~BinaryStringSerDes();

  static const bool canSerialize = true;
  static const bool canDeserialize = true;
  static const uint64_t DataLength_bytes;// Dieser Wert muss vom  Verwender des Klassen-Templates erstellt werden; er begrenzt die ausgegebenen Stringlänge inclusive NL und gibt die Dateilänge an.
  static const ValuePair Values[];  // Dieses Array muss vom Verwender des Klassen-Templates erstellt werden, es muss mit einem nullptr enden; die NpStr's dürfen keine führenden und nachfolgenden Whitespaces haben!
  static const NpStr& UnknownValue;    // Dieser Wert muss vom  Verwender des Klassen-Templates erstellt werden, er wird immer dann ausgegeben, wenn im Value[]-Array kein Wert gefunden wurde.
  static const int DefaultValue;  // Dieser Wert muss vom  Verwender des Klassen-Templates erstellt werden, er wird immer dann eingelesen, wenn im Value[]-Array kein NpStr gefunden wurde.

  unsigned serializeTo(unsigned char* buffer, unsigned bufferLength)
  {
    return serializeFromInt(int(variable), Values, UnknownValue, buffer, bufferLength > DataLength_bytes ? DataLength_bytes : bufferLength);
  }

  bool deserializeFrom(const unsigned char* buffer, unsigned bufferLength)
  {
    variable = VarType(deserializeToInt(Values, DefaultValue, buffer, bufferLength > DataLength_bytes ? DataLength_bytes : bufferLength));
    return true;
  }

protected:
  VarType& variable;
};

} /* namespace Npfs */
#endif /* BINARYSTRINGSERDES_H_ */
