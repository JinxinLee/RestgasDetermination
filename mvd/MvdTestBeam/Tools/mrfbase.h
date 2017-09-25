/*============================================================*/
/* mrfbase.h                                                  */
/* MVD Readout Framework Base Functions                       */
/*                                               M.C. Mertens */
/*============================================================*/


#ifndef __MRFBASE_H__
#define __MRFBASE_H__

//#include <machine/types.h>
#include "Rtypes.h"


//#include <stdint.h>

//! Basic register address and content types used within MRF Suite.
namespace mrf {
	//! Register content data type.
	typedef UInt_t registertype;
	//! Register address data type.
	typedef UInt_t addresstype;
//	typedef UInt_t devicetype;
}

#endif // __MRFBASE_H__

