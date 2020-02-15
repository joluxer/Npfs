/*
 * NpfsConstants.h
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */
#ifndef NPFSCONSTANTS_H_
#define NPFSCONSTANTS_H_

/***
 * @file
 *
 * In dieser Datei sind Konstanten für den 9P2000-Server definiert, die durch den Standard in ihrem
 * Wert festgelegt sind.
 */

#include <stdint.h>

namespace Npfs
{

/***
 * @brief Bits returned to the client in the QID of Rmessages.
 */
enum QidTypeBit
{
  Qtdir     = 0x80,//!< directory FID
  Qtappend  = 0x40,//!< append only file FID
  Qtexcl    = 0x20,//!< open exclusive (no double open possible)
  Qtmount   = 0x10,//!< mount point (not used in this framework) FID
  Qtauth    = 0x08,//!< auth FID
  Qttmp     = 0x04,//!< not backups (not used in this framework)
  Qtsymlink = 0x02,//!< symlink FID (not used in this framework)
  Qtlink    = 0x01,//!< hard link FID (not used in this framework)
  Qtfile    = 0x00,//!< ordinary file FID
};

/**
 * @brief Directory mode bits.
 */
enum ResourcePermissionBit
{
  Dmdir     = 0x80000000, ///< directory ressource
  Dmappend  = 0x40000000, ///< append only file
  Dmexcl    = 0x20000000, ///< exclusive open file
  Dmmount   = 0x10000000, ///<
  Dmauth    = 0x08000000, ///<
  Dmtmp     = 0x04000000, ///<
  Dmsymlink = 0x02000000, ///<
  Dmlink    = 0x01000000, ///<
};

/***
 * @brief Ressource open mode bits.
 */
enum ResourceModeBits
{
  Oread     = 0x00, ///< open for read only
  Owrite    = 0x01, ///< open for write only
  Ordwr     = 0x02, ///< open for read/write
  Oexec     = 0x03, ///< open for execution
  Oexcl     = 0x04, ///< exclusive open flag
  Otrunc    = 0x10, ///< truncate on open flag
  Orexec    = 0x20, ///< remove after exec flag (not used in this framework)
  Orclose   = 0x40, ///< remove after close flag
  Oappend   = 0x80, ///< append only flag
};

/**
 * @brief Internal constants.
 */
enum Constant
{
  NOTAG = (uint16_t)(~0), //!< NOTAG
  NOFID =  (uint32_t)(~0),//!< NOFID
  MaxWalkElements = 16,   //!< MaxWalkElements
  IOHDRSZ  = 24,          //!< IOHDRSZ
};

/***
 * @brief Predefined Errorstrings, as understood by the Linux kernel.
 */
struct ErrorString
{
  static const char Eunknownfid[];
  static const char Enomem[];
  static const char Enoauth[];
  static const char Enotimpl[];
  static const char Einuse[];
  static const char Ebadusefid[];
  static const char Enotdir[];
  static const char Etoomanywnames[];
  static const char Eperm[];
  static const char Etoolarge[];
  static const char Ebadoffset[];
  static const char Edirchange[];
  static const char Enotfound[];
  static const char Eopen[];
  static const char Eexist[];
  static const char Enotempty[];
  static const char Eunknownuser[];
  static const char Enospace[];
  static const char Eblocked[];
  static const char InternalServerError[];
  static const char Eio[];
  static const char Erange[];
  static const char Eremoteio[];
  static const char Etimedout[];
  static const char Ehostunreach[];
  static const char Enetunreach[];
  static const char Ecomm[];
  static const char Eproto[];
  static const char Enobufs[];
  static const char Econnaborted[];
  static const char Ebusy[];
  static const char Ebadmsg[];
};

static const int DefaultTcpPortNumber = 564;

}  // namespace Npfs

#endif

