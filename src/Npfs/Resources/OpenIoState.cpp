/*
 * OpenIoState.cpp
 *
 *  Created on: 14.08.2012
 *      Author: lode
 */

#include "OpenIoState.h"

namespace Npfs
{

OpenIoState::OpenIoState(IoState* &ioReference, BlockingOpState* &opRef)
: ioState(ioReference), opState(opRef)
{}

//OpenIoState::~OpenIoState()
//{
//  // Auto-generated destructor stub
//}

}
