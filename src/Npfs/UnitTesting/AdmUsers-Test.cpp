/*
 * Connection-Test.cpp
 *
 *  Created on: 19.07.2012
 *      Author: jlode
 */

#include "tut/tut.h"

#include "../Resources/AdmUsersFile.h"
#include "../NpfsConstants.h"
#include "../Resources/Directory.h"
#include "HeapMemory.h"

extern "C" uint32_t getCurrentTime()
{
  return 0;
}

namespace tut
{
struct AdmUsersTest
{
  Npfs::AdmUsersEntry userNobody;
  Npfs::AdmUsersEntry userRoot;
  Npfs::AdmUsersEntry userAnne;
  Npfs::AdmUsersEntry userJohnny;
  Npfs::AdmUsersEntry userSysop;
  Npfs::AdmUsersEntry userUsers;

  Npfs::Directory     rootDir;
  Npfs::Directory     admDir;
  Npfs::AdmUsersFile  admUsersFile;

  MemoryManager* mm;

  AdmUsersTest()
  : userNobody("nobody", 0, 0),
    userRoot("root", &userNobody),
    userAnne("anne", &userRoot),
    userJohnny("johnny", &userAnne),
    userSysop("Sysop", &userJohnny),
    userUsers("User", &userSysop, &userAnne),
    rootDir("/"),
    admDir("adm", &rootDir),
    admUsersFile(&admDir),
    mm(new HeapMemory)
  {
    userUsers.addMember(userAnne);
    userUsers.addMember(userJohnny);
    userUsers.addMember(userSysop);

    Npfs::AdmUsersFile::appendEntry(userNobody);
    Npfs::Directory::useMemory(mm);
  }

  ~AdmUsersTest()
  {
    Npfs::AdmUsersFile::removeEntry(userNobody);
    Npfs::AdmUsersFile::removeEntry(userAnne);
    Npfs::AdmUsersFile::removeEntry(userRoot);
    Npfs::AdmUsersFile::removeEntry(userJohnny);
    Npfs::AdmUsersFile::removeEntry(userSysop);
    Npfs::AdmUsersFile::removeEntry(userUsers);
    Npfs::Directory::useMemory(0);
    delete mm;
  }

};

namespace
{
typedef test_group<AdmUsersTest> TestGroup;
TestGroup connectionTest_group("3 AdmUsers Ressources");
typedef TestGroup::object testobject;
}

template<> template<>
void testobject::test<1>()
{
  set_test_name("get entry by string");

  Npfs::NpUser user = *Npfs::AdmUsersFile::getEntry(Npfs::NpStr("nobody"));
  ensure("User nobody name", 0 == strcmp(user.uname, "nobody"));
  ensure("User nobody ngroups", 0 == user.ngroups);
}

template<> template<>
void testobject::test<2>()
{
  set_test_name("check /adm/users file properties");

  Npfs::NpQid qid = admUsersFile.qid();
  ensure("Qid.type is file", Npfs::Qtfile == qid.type);
  ensure("Qid.path is not null", 0 != qid.path);

  const char* accRes = 0;
  Npfs::NpStat rstat;
  Npfs::NpUser user = *Npfs::AdmUsersFile::getEntry("root");
  Npfs::BlockingOpState* op;

  rstat = admUsersFile.getStat(op);
  ensure("rstat.length", 111 == rstat.length);
  ensure("rstat.mode is 0444", 0444 == (rstat.mode & 07777));
  ensure("file name length", 5 == rstat.name.len);
  ensure("file name string", 0 == memcmp("users", rstat.name.str, rstat.name.len));
}

template<> template<>
void testobject::test<3>()
{
  set_test_name("reading /adm/users file content");

  const char* accRes = 0;
  Npfs::IoState* ioRef = 0;
  uint32_t iounit = 0;
  Npfs::BlockingOpState* op;
  Npfs::OpenIoState ioSt(ioRef, op);

  accRes = admUsersFile.open(ioSt, Npfs::Oread, iounit);
  ensure("open() result is success", Npfs::Resource::OpSuccess == accRes);
  ensure("iounit is null", 0 == iounit);

  unsigned char rData[50];

  memset(rData, 0xfe, sizeof(rData));
  uint32_t count = sizeof(rData) - 10;

  accRes = admUsersFile.read(ioSt, rData, count, 0);
  ensure("read() result is success", Npfs::Resource::OpSuccess == accRes);
  ensure("read() count", (sizeof(rData) - 10) == count);
  ensure("read() data content", 0 == memcmp(rData, "nobody:nobody::\nroot:root:root:\nrainer:r\xfe\xfe\xfe", count + 3));

  accRes = admUsersFile.close(ioSt);
  ensure("clunk() result is success", Npfs::Resource::OpSuccess == accRes);
  ensure("clunk() sets ioRef zero", 0 == ioRef);
}

template<> template<>
void testobject::test<4>()
{
  set_test_name("directory tests");

  const char* accRes = 0;
  Npfs::IoState* ioRef = 0;
  uint32_t iounit = 0;
  Npfs::BlockingOpState* op;
  Npfs::OpenIoState ioSt(ioRef, op);

  accRes = rootDir.open(ioSt, Npfs::Oread, iounit);
  ensure("open() result is success", Npfs::Resource::OpSuccess == accRes);
  ensure("open() ioRef not zero", 0 != ioRef);
  ensure("iounit is null", 0 == iounit);

  unsigned char readData[100];

  memset(readData, 0xfe, sizeof(readData));
  uint32_t count = sizeof(readData) - 10;

  accRes = rootDir.read(ioSt, readData, count, 0);
  ensure("read() result is success", Npfs::Resource::OpSuccess == accRes);
  ensure("read() count", 64 == count);
  unsigned char refData[68] = ">\000\000\000\000\000\000\000\200\000\000\000\000P\262c\000\000\000\000\000\355\001\000\200\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\003\000adm\004\000root\004\000root\004\000root\xfe\xfe\xfe";
  uint64_t pv = admDir.qid().path;
  refData[13] = (pv >>  0) & 0xff;
  refData[14] = (pv >>  8) & 0xff;
  refData[15] = (pv >> 16) & 0xff;
  refData[16] = (pv >> 24) & 0xff;
  refData[17] = (pv >> 32) & 0xff;
  refData[18] = (pv >> 40) & 0xff;
  refData[19] = (pv >> 48) & 0xff;
  refData[20] = (pv >> 56) & 0xff;
//  ensure("read() data content", 0 == memcmp(readData, refData, count + 3));

  accRes = rootDir.close(ioSt);
  ensure("clunk() result is success", Npfs::Resource::OpSuccess == accRes);
  ensure("clunk() sets ioRef zero", 0 == ioRef);

  Npfs::Resource* r = rootDir.walkTo(op, "adm");
  ensure("adm ressoruce found", 0 != r);
  ensure("adm resource isDir", r->isDir());

  Npfs::Directory* dir = static_cast<Npfs::Directory*>(r);
  ensure("adm directory object", &admDir == dir);
}

}
