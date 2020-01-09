/*
 * NpfsConstants.cpp
 *
 *  Created on: 30.07.2012
 *      Author: jlode
 */

#include "NpfsConstants.h"

namespace Npfs
{

 const char ErrorString::Eunknownfid[] = "fid unknown or out of range";
 const char ErrorString::Enomem[] = "Cannot allocate memory";
 const char ErrorString::Enoauth[] = "no authentication required";
 const char ErrorString::Enotimpl[] = "Function not implemented";
 const char ErrorString::Einuse[] = "fid already in use";
 const char ErrorString::Ebadusefid[] = "bad use of fid";
 const char ErrorString::Enotdir[] = "Not a directory";
 const char ErrorString::Etoomanywnames[] = "too many wnames";
 const char ErrorString::Eperm[] = "permission denied";
 const char ErrorString::Etoolarge[] = "i/o count too large";
 const char ErrorString::Ebadoffset[] = "bad offset in directory read";
 const char ErrorString::Edirchange[] = "wstat can't convert between files and directories";
 const char ErrorString::Enotfound[] = "No such file or directory";
 const char ErrorString::Eopen[] = "exclusive use file already open";
 const char ErrorString::Eexist[] = "file or directory already exists";
 const char ErrorString::Enotempty[] = "Directory not empty";
 const char ErrorString::Eunknownuser[] = "unknown user";
 const char ErrorString::Enospace[] = "No space left on device";
 const char ErrorString::Eblocked[] = "Resource temporarily unavailable";
 const char ErrorString::InternalServerError[] = "Internal server error";
 const char ErrorString::Eio[] = "i/o error";


}  // namespace Npfs

