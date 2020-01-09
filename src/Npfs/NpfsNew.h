/*
 * NpfsNew.h
 *
 *  Created on: 07.12.2012
 *      Author: jlode
 */

#ifndef NPFSNEW_H_
#define NPFSNEW_H_

#include "MemoryManager.h"
#include <stddef.h>

namespace Npfs
{

void* npfsNew(size_t size, MemoryManager* mm);
void npfsDelete(void * mem);

}  // namespace Npfs

#endif /* NPFSNEW_H_ */
