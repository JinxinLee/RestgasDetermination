#ifndef CbmL1Def_h
#define CbmL1Def_h 1

#define HAVE_SSE

#ifdef HAVE_SSE
  #include "vectors/P4_F32vec4.h"
#else
  #include "vectors/PSEUDO_F32vec4.h"
  #error NoSseFound
#endif // HAVE_SSE

#endif // CbmL1Def_h
