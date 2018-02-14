#ifndef L1MaterialInfo_h
#define L1MaterialInfo_h

class L1MaterialInfo{
  
//* information about material
  
 public:
   L1MaterialInfo(): thick(0), RL(0),  RadThick(0), logRadThick(0){};
   
  float thick, RL,  RadThick, logRadThick;
};


#endif
