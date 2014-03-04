/*
 * SharedPtr.h
 *
 *  Created on: Feb 27, 2014
 *      Author: steve
 */

#ifndef SHAREDPTR_H_
#define SHAREDPTR_H_

#ifdef HAS_SHAREDPOINTER
#include <memory>
using std::shared_ptr;
#else
#include <tr1/memory>
using std::tr1::shared_ptr;
#endif

#endif /* SHAREDPTR_H_ */
