//***************************************************************************
//*  For use by things like roguewave where a function taking an X
//*  returning an hash value for X is needed rather than X.hash() or
//*  unsigned int (X::*)(const X&)
//***************************************************************************
#include "IfdKeyHash.h"
#include "IfdKey.h"

//****************************************************************************
unsigned int
ifdKeyHash( const IfdKey& k ) {
//****************************************************************************

  return k._hashVal;
}
