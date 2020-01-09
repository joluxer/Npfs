/*
 * Connection-Test.cpp
 *
 *  Created on: 19.07.2012
 *      Author: jlode
 */

#include "tut/tut.h"

#include "../Connection.h"

#include "../Messages/Tversion.h"
#include "../Messages/Rversion.h"
#include "../Messages/Tauth.h"
#include "../Messages/Rauth.h"
#include "../Messages/Tflush.h"
#include "../Messages/Rflush.h"
#include "../Messages/Tattach.h"
#include "../Messages/Rattach.h"
#include "../Messages/Twalk.h"
#include "../Messages/Rwalk.h"
#include "../Messages/Topen.h"
#include "../Messages/Ropen.h"
#include "../Messages/Tcreate.h"
#include "../Messages/Rcreate.h"
#include "../Messages/Tread.h"
#include "../Messages/Rread.h"
#include "../Messages/Twrite.h"
#include "../Messages/Rwrite.h"
#include "../Messages/Tclunk.h"
#include "../Messages/Rclunk.h"
#include "../Messages/Tremove.h"
#include "../Messages/Rremove.h"
#include "../Messages/Tstat.h"
#include "../Messages/Rstat.h"
#include "../Messages/Twstat.h"
#include "../Messages/Rwstat.h"

#include "HeapMemory.h"

static uint32_t getCurrentTime()
{
  return 0;
}

namespace tut
{
struct ConnectionTest: public Npfs::Connection
{
  static const unsigned char testPattern[8];
  static const unsigned char rxTestData[320];

  unsigned char writeBuffer[256];
  static unsigned readPos;
  unsigned writePos;
  static const unsigned BufferLength = sizeof(writeBuffer);

  ConnectionTest(): writePos(0)
  {
    memset(writeBuffer, 0, sizeof(writeBuffer));
    mm = new HeapMemory;
  }

  ~ConnectionTest()
  {
    delete mm;
  }

  virtual unsigned bytesReadable()
  {
    return sizeof(rxTestData) - readPos;
  }

  virtual unsigned bytesWritable()
  {
    return BufferLength - writePos;
  }

  virtual unsigned readData(unsigned char* buf, unsigned length)
  {
    if ((readPos + length) > sizeof(rxTestData))
    {
      length = sizeof(rxTestData) - readPos;
    }

    if (length)
    {
      memcpy(buf, rxTestData + readPos, length);
      readPos += length;
    }

    return length;
  }

  virtual unsigned sendData(const unsigned char* buf, unsigned length)
  {
    if ((writePos + length) > BufferLength)
    {
      length = BufferLength - writePos;
    }

    if (length)
    {
      memcpy(writeBuffer + writePos, buf, length);
      writePos += length;
    }

    return length;
  }

  virtual unsigned maxMsgSize() const
  {
    return sizeof(writeBuffer);
  }

  virtual void doAliveCheck()
  {
    // intentionally left empty
  }

  virtual void startTxPacket()
  {
    // intentionally left empty
  }

  virtual void finishTxPacket()
  {
    // intentionally left empty
  }

  virtual bool driveTransmitter()
  {
    return false;
  }
};

const unsigned char ConnectionTest::rxTestData[] =
{
    /* Tversion */ 0x13, 0x00, 0x00, 0x00, 0x64, 0x00, 0x10, 0x55, 0x03, 0x00, 0x00, 0x06, 0x00,  '9',  'P',  '2',  '0',  '0', '0',
    /* Tauth */    0x1d, 0x00, 0x00, 0x00, 0x66, 0x01, 0x10, 0x0a, 0x21, 0x43, 0x7f, 0x0a, 0x00,  'u',  '-',  'n',  'i',  'c',  'a',  '.',   'c',  'o',  'm', 0x04,  0x00, '/',  'c',  'l',  'm',
    /* Tattach */  0x21, 0x00, 0x00, 0x00, 0x68, 0x02, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0x0a, 0x21, 0x43, 0x7f, 0x0a, 0x00,  'u',  '-',  'n',  'i',  'c',  'a',  '.',   'c',  'o',  'm', 0x04,  0x00, '/',  'c',  'l',  'm',
    /* Tflush */   0x09, 0x00, 0x00, 0x00, 0x6c, 0x03, 0x10, 0x02, 0x10,
    /* Twalk */    0x17, 0x00, 0x00, 0x00, 0x6e, 0x04, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0x0a, 0x21, 0x43, 0x7f, 0x01, 0x00, 0x04, 0x00, 's',  'd',  'i',  'r',
    /* Topen */    0x0c, 0x00, 0x00, 0x00, 0x70, 0x05, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0xa3,
    /* Tcreate */  0x17, 0x00, 0x00, 0x00, 0x72, 0x06, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0x05, 0x00,  'n',  'f',  'i',  'l',  'e', 0x12, 0x34, 0x56, 0x78, 0x3a,
    /* Tread */    0x17, 0x00, 0x00, 0x00, 0x74, 0x07, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x3a, 0x72, 0x01, 0x01,
    /* Twrite */   0x1c, 0x00, 0x00, 0x00, 0x76, 0x08, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, 0x05, 0x00, 0x00, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x70,
    /* Tclunk */   0x0b, 0x00, 0x00, 0x00, 0x78, 0x09, 0x10, 0xa3, 0x32, 0x65, 0x7e,
    /* Tremove */  0x0b, 0x00, 0x00, 0x00, 0x7a, 0x0a, 0x10, 0xa3, 0x32, 0x65, 0x7e,
    /* Tstat */    0x0b, 0x00, 0x00, 0x00, 0x7c, 0x0b, 0x10, 0xa3, 0x32, 0x65, 0x7e,
    /* Twstat */   0x51, 0x00, 0x00, 0x00, 0x7e, 0x0c, 0x10, 0xa3, 0x32, 0x65, 0x7e, 0x42, 0x00, 0x42, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x33, 0xff, 0x01, 0x23, 0x45, 0x67, 0x01, 0x23, 0x45, 0x67, 0x67, 0x45, 0x23, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, '/', 0x06, 0x00, 'u', '-', 'n', 'i', 'c', 'a', 0x06, 0x00, 'u', '-', 'n', 'i', 'c', 'a', 0x06, 0x00, 'u', '-', 'n', 'i', 'c', 'a',
};

const unsigned char ConnectionTest::testPattern[8] = { 0x26, 0x95, 0xd0, 0x05, 0xaa, 0x5b, 0xb8, 0x61 };

unsigned ConnectionTest::readPos = 0;

namespace
{
typedef test_group<ConnectionTest> TestGroup;
TestGroup connectionTest_group("2 Connection and Messages");
typedef TestGroup::object testobject;
}

template<> template<>
void testobject::test<1>()
{
  set_test_name("Version messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tversion returns non-zero", msg != 0);
  ensure("Tversion type", msg->type == Npfs::Message::Tversion);
  ensure("Tversion tag", msg->tag() == 0x1000);
  {
    Npfs::Tversion* m = static_cast<Npfs::Tversion*>(msg);

    ensure("Tversion mSize", m->mSize == 0x0355);
    ensure("Tversion version.len", m->version.len == 6);
    ensure("Tversion version.str", memcmp(m->version.str, "9P2000", m->version.len) == 0);
    ensure("Tversion version check", m->is9P2000());

    Npfs::Rversion* rmsg = new(mm) Npfs::Rversion(*m);
    rmsg->mSize = 495;
    rmsg->version = Npfs::NpStr("9P2000");
    send(rmsg);
    runTxMsgQueue();

    ensure("Rversion byte count", 19 == writePos);
    ensure("Rversion data content", 0 == memcmp(writeBuffer, "\x13\x00\x00\x00\x65\x00\x10\xef\x01\x00\x00\x06\x00" "9P2000", 19));
  }
  delete msg;
}

template<> template<>
void testobject::test<2>()
{
  set_test_name("Auth messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tauth returns non-zero", msg != 0);
  ensure("Tauth type", msg->type == Npfs::Message::Tauth);
  ensure("Tauth tag", msg->tag() == 0x1001);
  {
    Npfs::Tauth* m = static_cast<Npfs::Tauth*>(msg);

    ensure("Tauth afid", m->afid == 0x7f43210a);
    ensure("Tauth uname.len", 10 == m->uname.len);
    ensure("Tauth uname.str", memcmp(m->uname.str, "u-nica.com", m->uname.len) == 0);
    ensure("Tauth aname.len", 4 == m->aname.len);
    ensure("Tauth aname.str", memcmp(m->aname.str, "/clm", m->aname.len) == 0);

    Npfs::Rauth* rmsg = new(mm) Npfs::Rauth(*m);
    rmsg->aqid.type = 0xa3;
    rmsg->aqid.version = 3;
    rmsg->aqid.path = 0xdeadbeefd001b011ull;
    send(rmsg);
    runTxMsgQueue();

    ensure("Rauth byte count", 20 == writePos);
    ensure("Rauth data content", 0 == memcmp(writeBuffer, "\x14\x00\x00\x00\x67\x01\x10\xa3\x03\x00\x00\x00\x11\xb0\x01\xd0\xef\xbe\xad\xde", 20));
  }
  delete msg;
}

template<> template<>
void testobject::test<3>()
{
  set_test_name("Attach messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tattach returns non-zero", msg != 0);
  ensure("Tattach type", msg->type == Npfs::Message::Tattach);
  ensure("Tattach tag", msg->tag() == 0x1002);
  {
    Npfs::Tattach* m = static_cast<Npfs::Tattach*>(msg);

    ensure("Tattach fid", m->fid() == 0x7e6532a3);
    ensure("Tattach afid", m->afid == 0x7f43210a);
    ensure("Tattach uname.len", 10 == m->uname.len);
    ensure("Tattach uname.str", memcmp(m->uname.str, "u-nica.com", m->uname.len) == 0);
    ensure("Tattach aname.len", 4 == m->aname.len);
    ensure("Tattach aname.str", memcmp(m->aname.str, "/clm", m->aname.len) == 0);

    Npfs::Rattach* rmsg = new(mm) Npfs::Rattach(*m);
    rmsg->qid.type = 0xa3;
    rmsg->qid.version = 3;
    rmsg->qid.path = 0xdeadbeefd001b011ull;
    send(rmsg);
    runTxMsgQueue();

    ensure("Rattach byte count", 20 == writePos);
    ensure("Rattach data content", 0 == memcmp(writeBuffer, "\x14\x00\x00\x00\x69\x02\x10\xa3\x03\x00\x00\x00\x11\xb0\x01\xd0\xef\xbe\xad\xde", 20));
  }
  delete msg;
}

template<> template<>
void testobject::test<4>()
{
  set_test_name("Flush messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tflush returns non-zero", msg != 0);
  ensure("Tflush type", msg->type == Npfs::Message::Tflush);
  ensure("Tflush tag", msg->tag() == 0x1003);
  {
    Npfs::Tflush* m = static_cast<Npfs::Tflush*>(msg);

    ensure("Tflush oldtag", m->oldTag == 0x1002);

    Npfs::Rflush* rmsg = new(mm) Npfs::Rflush(*m);
    send(rmsg);
    runTxMsgQueue();

    ensure("Rflush byte count", 7 == writePos);
    ensure("Rflush data content", 0 == memcmp(writeBuffer, "\x07\x00\x00\x00\x6d\x03\x10", 7));
  }
  delete msg;
}

template<> template<>
void testobject::test<5>()
{
  set_test_name("Walk messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Twalk returns non-zero", msg != 0);
  ensure("Twalk type", msg->type == Npfs::Message::Twalk);
  ensure("Twalk tag", msg->tag() == 0x1004);
  {
    Npfs::Twalk* m = static_cast<Npfs::Twalk*>(msg);

    ensure("Twalk fid", m->fid() == 0x7e6532a3);
    ensure("Twalk nWnames", 1 == m->nWnames);
    ensure("Twalk wnames.len", 4 == m->wnames->len);
    ensure("Twalk wnames.str", 0 == memcmp(m->wnames->str, "sdir", m->wnames->len));

    Npfs::Rwalk* rmsg = new(mm) Npfs::Rwalk(*m);
    rmsg->nwqids = 1;
    rmsg->wqids[0].type = 0xa3;
    rmsg->wqids[0].version = 3;
    rmsg->wqids[0].path = 0xdeadbeefd001b011ull;
    send(rmsg);
    runTxMsgQueue();

    ensure("Rwalk byte count", 22 == writePos);
    ensure("Rwalk data content", 0 == memcmp(writeBuffer, "\x16\x00\x00\x00\x6f\x04\x10\x01\x00\xa3\x03\x00\x00\x00\x11\xb0\x01\xd0\xef\xbe\xad\xde", 22));
  }
  delete msg;
}

template<> template<>
void testobject::test<6>()
{
  set_test_name("Open messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Topen returns non-zero", msg != 0);
  ensure("Topen type", msg->type == Npfs::Message::Topen);
  ensure("Topen tag", msg->tag() == 0x1005);
  {
    Npfs::Topen* m = static_cast<Npfs::Topen*>(msg);

    ensure("Topen fid", m->fid() == 0x7e6532a3);
    ensure("Topen mode", 0xa3 == m->mode);

    Npfs::Ropen* rmsg = new(mm) Npfs::Ropen(*m);
    rmsg->qid.type = 0xa3;
    rmsg->qid.version = 3;
    rmsg->qid.path = 0xdeadbeefd001b011ull;
    rmsg->iounit = 1;
    send(rmsg);
    runTxMsgQueue();

    ensure("Ropen byte count", 24 == writePos);
    ensure("Ropen data content", 0 == memcmp(writeBuffer, "\x18\x00\x00\x00\x71\x05\x10\xa3\x03\x00\x00\x00\x11\xb0\x01\xd0\xef\xbe\xad\xde\x01\x00\x00\x00", 24));
  }
  delete msg;
}

template<> template<>
void testobject::test<7>()
{
  set_test_name("Create messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tcreate returns non-zero", msg != 0);
  ensure("Tcreate type", msg->type == Npfs::Message::Tcreate);
  ensure("Tcreate tag", msg->tag() == 0x1006);
  {
    Npfs::Tcreate* m = static_cast<Npfs::Tcreate*>(msg);

    ensure("Tcreate fid", m->fid() == 0x7e6532a3);
    ensure("Tcreate name.len", 5 == m->name.len);
    ensure("Tcreate name.str", 0 == memcmp(m->name.str, "nfile", m->name.len));
    ensure("Tcreate perm", 0x78563412 == m->perm);
    ensure("Tcreate mode", 0x3a == m->mode);

    Npfs::Rcreate* rmsg = new(mm) Npfs::Rcreate(*m);
    rmsg->qid.type = 0xa3;
    rmsg->qid.version = 3;
    rmsg->qid.path = 0xdeadbeefd001b011ull;
    rmsg->iounit = 1;
    send(rmsg);
    runTxMsgQueue();

    ensure("Rcreate byte count", 24 == writePos);
    ensure("Rcreate data content", 0 == memcmp(writeBuffer, "\x18\x00\x00\x00\x73\x06\x10\xa3\x03\x00\x00\x00\x11\xb0\x01\xd0\xef\xbe\xad\xde\x01\x00\x00\x00", 24));
  }
  delete msg;
}

template<> template<>
void testobject::test<8>()
{
  set_test_name("Read messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tread returns non-zero", msg != 0);
  ensure("Tread type", msg->type == Npfs::Message::Tread);
  ensure("Tread tag", msg->tag() == 0x1007);
  {
    Npfs::Tread* m = static_cast<Npfs::Tread*>(msg);

    ensure("Tread fid", m->fid() == 0x7e6532a3);
    ensure("Tread offset", 0x7856341278563412ull == m->offset);
    ensure("Tread count", 0x0101723aul == m->count);

    Npfs::Rread* rmsg = new(mm) Npfs::Rread(*m, mm);
    rmsg->count = 8;
    rmsg->data[0] = 'd';
    rmsg->data[1] = 'e';
    rmsg->data[2] = 'a';
    rmsg->data[3] = 'd';
    rmsg->data[4] = 'b';
    rmsg->data[5] = 'e';
    rmsg->data[6] = 'e';
    rmsg->data[7] = 'f';
    send(rmsg);
    runTxMsgQueue();

    ensure("Rread byte count", 19 == writePos);
    ensure("Rread data content", 0 == memcmp(writeBuffer, "\x13\x00\x00\x00\x75\x07\x10\x08\x00\x00\x00" "deadbeef", 19));
  }
  delete msg;
}

template<> template<>
void testobject::test<9>()
{
  set_test_name("Write messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Twrite returns non-zero", msg != 0);
  ensure("Twrite type", msg->type == Npfs::Message::Twrite);
  ensure("Twrite tag", msg->tag() == 0x1008);
  {
    Npfs::Twrite* m = static_cast<Npfs::Twrite*>(msg);

    ensure("Twrite fid", m->fid() == 0x7e6532a3);
    ensure("Twrite offset", 0x7856341278563412ull == m->offset);
    ensure("Twrite count", 0x05ul == m->count);
    static const unsigned char testData[5] = { 0xde, 0xad, 0xbe, 0xef, 0x70 };
    ensure("Twrite data", 0 == memcmp(m->data, testData, sizeof(testData)));

    Npfs::Rwrite* rmsg = new(mm) Npfs::Rwrite(*m);
    rmsg->count = 5;
    send(rmsg);
    runTxMsgQueue();

    ensure("Rwrite byte count", 11 == writePos);
    ensure("Rwrite data content", 0 == memcmp(writeBuffer, "\x0b\x00\x00\x00\x77\x08\x10\x05\x00\x00\x00", 11));
  }
  delete msg;
}

template<> template<>
void testobject::test<10>()
{
  set_test_name("Clunk messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tclunk returns non-zero", msg != 0);
  ensure("Tclunk type", msg->type == Npfs::Message::Tclunk);
  ensure("Tclunk tag", msg->tag() == 0x1009);
  {
    Npfs::Tclunk* m = static_cast<Npfs::Tclunk*>(msg);

    ensure("Tclunk fid", m->fid() == 0x7e6532a3);

    Npfs::Rclunk* rmsg = new(mm) Npfs::Rclunk(*m);
    send(rmsg);
    runTxMsgQueue();

    ensure("Rclunk byte count", 7 == writePos);
    ensure("Rclunk data content", 0 == memcmp(writeBuffer, "\x07\x00\x00\x00\x79\x09\x10", 7));
  }
  delete msg;
}

template<> template<>
void testobject::test<11>()
{
  set_test_name("Remove messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tremove returns non-zero", msg != 0);
  ensure("Tremove type", msg->type == Npfs::Message::Tremove);
  ensure("Tremove tag", msg->tag() == 0x100a);
  {
    Npfs::Tremove* m = static_cast<Npfs::Tremove*>(msg);

    ensure("Tremove fid", m->fid() == 0x7e6532a3);

    Npfs::Rremove* rmsg = new(mm) Npfs::Rremove(*m);
    send(rmsg);
    runTxMsgQueue();

    ensure("Rremove byte count", 7 == writePos);
    ensure("Rremove data content", 0 == memcmp(writeBuffer, "\x07\x00\x00\x00\x7b\x0a\x10", 7));
  }
  delete msg;
}

template<> template<>
void testobject::test<12>()
{
  set_test_name("Stat messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Tstat returns non-zero", msg != 0);
  ensure("Tstat type", msg->type == Npfs::Message::Tstat);
  ensure("Tstat tag", msg->tag() == 0x100b);
  {
    Npfs::Tstat* m = static_cast<Npfs::Tstat*>(msg);

    ensure("Tstat fid", m->fid() == 0x7e6532a3);

    Npfs::Rstat* rmsg = new(mm) Npfs::Rstat(*m);
    rmsg->stat.type = 0;
    rmsg->stat.dev = 0;
    rmsg->stat.qid.type = 0x80;
    rmsg->stat.qid.version = 3;
    rmsg->stat.qid.path = uint64_t(this);
    rmsg->stat.mode = 0x33;
    rmsg->stat.atime = getCurrentTime();
    rmsg->stat.mtime = getCurrentTime() + 13;
    rmsg->stat.length = 0x345687;
    rmsg->stat.name = Npfs::NpStr("testFile");
    rmsg->stat.uid = Npfs::NpStr("testUser1");
    rmsg->stat.gid = Npfs::NpStr("testUser2");
    rmsg->stat.muid = Npfs::NpStr("testUser3");

    unsigned char refData[] = "]\000\000\000}\v\020R\000R\000\000\000\000\000\000\000\200\003\000\000\000P\330c\000\000\000\000\000\063\000\000\000\000\000\000\000\r\000\000\000\207V4\000\000\000\000\000\010\000testFile\011\000testUser1\011\000testUser2\011\000testUser3";
    uint64_t pv = rmsg->stat.qid.path;
    int n = 22;
    refData[n++] = (pv >>  0) & 0xff;
    refData[n++] = (pv >>  8) & 0xff;
    refData[n++] = (pv >> 16) & 0xff;
    refData[n++] = (pv >> 24) & 0xff;
    refData[n++] = (pv >> 32) & 0xff;
    refData[n++] = (pv >> 40) & 0xff;
    refData[n++] = (pv >> 48) & 0xff;
    refData[n++] = (pv >> 56) & 0xff;

    send(rmsg);
    runTxMsgQueue();

    ensure("Rstat byte count", 93 == writePos);
    ensure("Rstat data content", 0 == memcmp(writeBuffer, refData, 93));
  }
  delete msg;
}

template<> template<>
void testobject::test<13>()
{
  set_test_name("Wstat messages");

  Npfs::Tmessage* msg;

  msg = poll();
  ensure("poll() for Twstat returns non-zero", msg != 0);
  ensure("Twstat type", msg->type == Npfs::Message::Twstat);
  ensure("Twstat tag", msg->tag() == 0x100c);
  {
    Npfs::Twstat* m = static_cast<Npfs::Twstat*>(msg);

    ensure("Twstat fid", m->fid() == 0x7e6532a3);
    ensure("Twstat fid", 0 == m->getError());
    ensure("Twstat stat.size",        0x42 == m->stat.size);
    ensure("Twstat stat.type",        0x0000 == m->stat.type);
    ensure("Twstat stat.dev",         0x33ffffff == m->stat.dev);
    ensure("Twstat stat.qid.type",    0xff == m->stat.qid.type);
    ensure("Twstat stat.qid.version", 0x67452301 == m->stat.qid.version);
    ensure("Twstat stat.qid.path",    0x123456767452301ull == m->stat.qid.path);
    ensure("Twstat stat.mode",        0xffffffff == m->stat.mode);
    ensure("Twstat stat.atime",       0xffffffff == m->stat.atime);
    ensure("Twstat stat.mtime",       0xffffffff == m->stat.mtime);
    ensure("Twstat stat.length",      0x01ull == m->stat.length);
    ensure("Twstat stat.name.len", 1 == m->stat.name.len);
    ensure("Twstat stat,name.str", 0 == memcmp(m->stat.name.str, "/", m->stat.name.len));
    ensure("Twstat stat.uid.len", 6 == m->stat.uid.len);
    ensure("Twstat stat,uid.str", 0 == memcmp(m->stat.uid.str, "u-nica", m->stat.uid.len));
    ensure("Twstat stat.gid.len", 6 == m->stat.gid.len);
    ensure("Twstat stat,gid.str", 0 == memcmp(m->stat.gid.str, "u-nica", m->stat.gid.len));
    ensure("Twstat stat.muid.len", 6 == m->stat.muid.len);
    ensure("Twstat stat,muid.str", 0 == memcmp(m->stat.muid.str, "u-nica", m->stat.muid.len));

    Npfs::Rwstat* rmsg = new(mm) Npfs::Rwstat(*m);
    send(rmsg);
    runTxMsgQueue();

    ensure("Rwstat byte count", 7 == writePos);
    ensure("Rwstat data content", 0 == memcmp(writeBuffer, "\x07\x00\x00\x00\x7f\x0c\x10", 7));
  }
  delete msg;
}

}
