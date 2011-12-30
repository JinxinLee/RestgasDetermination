//
//  PndAnaPidCombiner.h
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//
#ifndef PNDANAPIDCOMBINER_H
#define PNDANAPIDCOMBINER_H

#include <iostream>
#include <float.h>
#include "TNamed.h"
#include "RhoBase/VAbsPidSelector.h"

class TCandidate;
class VAbsMicroCandidate;

class PndAnaPidCombiner : public VAbsPidSelector {
  
public:
  //Constructor
  PndAnaPidCombiner(const char *name="PndAnaPidCombiner", const char* type="");
  //Destructor
  virtual ~PndAnaPidCombiner() {}
  
  //operations
  virtual Bool_t Accept(TCandidate& b);
  virtual Bool_t Accept(VAbsMicroCandidate& b);
  
private:
  
  
public:
  ClassDef(PndAnaPidCombiner,1)  // Particle selector
};    


#endif




