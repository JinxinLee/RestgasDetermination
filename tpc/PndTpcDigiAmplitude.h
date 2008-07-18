#ifndef TPCDIGIAMPLITUDE_HH
#define TPCDIGIAMPLITUDE_HH

class PndTpcDigi;

class PndTpcDigiAmplitude {
public:

  // Constructors/Destructors ---------
  PndTpcDigiAmplitude(){;}
  ~PndTpcDigiAmplitude(){;}

  // Operators
  bool operator()(PndTpcDigi* s1, PndTpcDigi* s2);
  
};

#endif

