//
//  PndPidSelector.h
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//

#include <iostream>
#include <float.h>
#include "TNamed.h"
#include "RhoBase/VAbsPidSelector.h"

class TCandidate;
class VAbsMicroCandidate;

class PndPidSelector : public VAbsPidSelector {
  
public:
  //Constructor
  PndPidSelector(const char *name="PndPidSelector", const char* type="");
  //Destructor
  virtual ~PndPidSelector() {}
  
  //operations
  virtual Bool_t Accept(TCandidate& b);
  virtual Bool_t Accept(VAbsMicroCandidate& b);
  
  
public:
  ClassDef(PndPidSelector,1)  // Particle selector
};      





