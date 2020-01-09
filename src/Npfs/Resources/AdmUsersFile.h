/*
 * AdmUsersFile.h
 *
 *  Created on: 14.06.2012
 *      Author: lode
 */

#ifndef ADMUSERSFILE_H_
#define ADMUSERSFILE_H_

#include "Resource.h"

#include "AdmUsersEntry.h"

#include "../NpStr.h"
#include "../NpStr.h"

namespace Npfs
{

/***
 * This class represents the file /adm/users of the 9P2000 server.
 * This implmentation implies, that the whole server uses the same file across all
 * namespaces.
 *
 * The resource should be mounted below the /adm directory. It is read only by intent.
 *
 * Its content is active anyway, even if it is not mounted. So one can establish
 * the content by calls to appendEntry() and removeEntry() as well other methods, to manage user
 * names and group names for access control, without exposing the details as a file.
 *
 * The
 */
class AdmUsersFile: public Npfs::Resource
{
public:
  AdmUsersFile(Directory* parent = 0);
//  virtual ~AdmUsersFile();

  virtual
  ResultMessage open(Npfs::OpenIoState& workRef, uint8_t mode, uint32_t& iounit);
  virtual
  ResultMessage read(Npfs::OpenIoState& workRef, unsigned char* dataBuffer, uint32_t& count, uint64_t offset);  // in count will be returned the actual read amount of data
  virtual
  void flush(Npfs::OpenIoState& workRef);  ///< cancels the referenced I/O operation immediately, the server forgets about the operation after return of this method
  virtual
  const NpStat& getStat(Npfs::BlockingOpState* &opRef);

  static
  void appendEntry(AdmUsersEntry& nUser);
  static
  void removeEntry(const AdmUsersEntry& user);
  static
  AdmUsersEntry* getEntry(const NpUser& spUser);
  static
  AdmUsersEntry* getEntry(const NpStr& uname);
  static
  AdmUsersEntry* getEntry(const char* uname);
  static
  const NpUser* getNpUser(const NpStr& uname);
  static
  const NpUser* getNpUser(const char* uname);

  static void catchAllUnknownAs(const char* uname);
  static void catchAllUnknownAs(AdmUsersEntry& uname);

protected:

  static AdmUsersEntry* file;
  static AdmUsersEntry* nilUser;
  static uint64_t fileLength;
};

}

#endif /* ADMUSERSFILE_H_ */
