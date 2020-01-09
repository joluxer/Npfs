/*
 * Message.cpp
 *
 *  Created on: 17.07.2012
 *      Author: jlode
 */

#include "Message.h"
#include "../NpfsConstants.h"
#include "Util/PrintfToStream.h"

namespace Npfs
{

Message::Message(MsgSerDes& _io, MessageType _type, uint16_t tag)
: type(_type), io(_io), tagValue(tag), fidValue((typeof(fidValue))(~0))
{

}

Message::~Message()
{
  // Auto-generated destructor stub
  // intentionally left empty
}

ByteStream* Message::logIo = 0;

void Message::logHeader(const char* msgType) const
{
  if (logIo)
  {
    const char* dirStr;

    // detect the normal data direction
    if (1 & type)
    {
      dirStr = "<<<";
    }
    else
    {
      dirStr = ">>>";
    }

    unsigned connId = (long unsigned) &io;

    PrintfToStream fLog(*logIo);

    fLog.printf("%s (0x%08x) %s tag %u", dirStr, connId, msgType, tagValue);
  }
}

void Message::logFid() const
{
  log("fid", fidValue);
}

void Message::log(const char* tag, uint32_t i) const
{
  if(logIo)
  {
    PrintfToStream fLog(*logIo);

    fLog.printf(" %s %u", tag, i);
  }
}

void Message::log(const char* tag, const NpStr& str) const
{
  if(logIo)
  {
    logIo->put_char(' ');
    logIo->writeBlock(tag);
    logIo->put_char(' ');
    unsigned i;
    for (i = 0; i < str.len; i++)
    {
      logIo->put_char(str.str[i]);
    }
  }
}

void Message::log(const char* tag, const NpQid& qid) const
{
  if (logIo)
  {
    int n;
    char buf[10];

    n = 0;
    if (qid.type & Qtdir)
      buf[n++] = 'd';
    if (qid.type & Qtappend)
      buf[n++] = 'a';
    if (qid.type & Qtauth)
      buf[n++] = 'A';
    if (qid.type & Qtexcl)
      buf[n++] = 'l';
    if (qid.type & Qttmp)
      buf[n++] = 't';
    if (qid.type & Qtsymlink)
      buf[n++] = 'L';
    buf[n] = '\0';

    PrintfToStream fLog(*logIo);

    fLog.printf(" %s (0x%.8x%.8x 0x%x '%s')", tag, (int)(qid.path >> 32), (int)(qid.path >> 0), qid.version, buf);
  }
}

void Message::log(const char* tag, const NpStat& stat) const
{
  if (logIo)
  {

    log("", stat.name);
    log("", stat.uid);
    log("", stat.gid);
    log("", stat.muid);

    log("", stat.qid);
    logPerm("mod", stat.mode);

    PrintfToStream fLog(*logIo);

    fLog.printf(" %s at %d mt %d len %d type %d dev %d", tag,
      stat.atime, stat.mtime, (unsigned int) stat.length, stat.type, stat.dev);

//    if (dotu)
//      log("ext", stat.extension);
  }
}

void Message::logPerm(const char* tag, uint32_t perm) const
{
  if (logIo)
  {
    int n;
    char b[10];

    n = 0;
    if (perm & Dmdir)
      b[n++] = 'd';
    if (perm & Dmappend)
      b[n++] = 'a';
    if (perm & Dmauth)
      b[n++] = 'A';
    if (perm & Dmexcl)
      b[n++] = 'l';
    if (perm & Dmtmp)
      b[n++] = 't';
//    if (perm & Dmdevice)
//      b[n++] = 'D';
//    if (perm & Dmsocket)
//      b[n++] = 'S';
//    if (perm & Dmnamedpipe)
//      b[n++] = 'P';
    if (perm & Dmsymlink)
      b[n++] = 'L';
    b[n] = '\0';

    PrintfToStream fLog(*logIo);

    fLog.printf(" %s %s 0%u%u%u", tag, b, (perm & 0700) >> 6, (perm & 0070) >> 3, (perm & 0007) >> 0);
  }
}

void Message::logNewLine() const
{
  if (logIo)
    logIo->put_char('\n');
}

}
