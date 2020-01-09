/*
 * IpDataTypes.h
 *
 */

#ifndef IPDATATYPES_H_
#define IPDATATYPES_H_

typedef struct
{
  unsigned char b[4]; // Big Endian Order: b[3] ist LSB, b[0] ist MSB
} IpV4Addr;

typedef unsigned short IpPort;

typedef struct _EthMacAddr
{
  unsigned char b[6]; // Big Endian Order: b[5] ist LSB, b[0] ist MSB
} EthMacAddr;


#endif /* IPDATATYPES_H_ */
