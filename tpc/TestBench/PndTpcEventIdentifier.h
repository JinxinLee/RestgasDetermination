//=======================================================
//
//  Simple holder of event information for the dataReader
//  meh.
//  
//  Written by Felix Boehmer
//  Physics Department E18, TUM
//
//=======================================================


#ifndef PNDTPCEVENTIDENTIFIER_H
#define PNDTPCEVENTIDENTIFIER_H

#include "TNamed.h" 

class PndTpcEventIdentifier : public TNamed { 
  
 public: 
  PndTpcEventIdentifier();  //"sonst geht des nicht"
  PndTpcEventIdentifier(unsigned int event, unsigned int spill);
  virtual ~PndTpcEventIdentifier() {;}
  
  unsigned int getEventInSpill() {return fEventInSpill;}
  unsigned int getSpill() {return fSpill;}

  void setEventInSpill(unsigned int ev) {fEventInSpill=ev;}
  void setSpill(unsigned int sp) {fSpill=sp;}

 private:
  unsigned int fEventInSpill;
  unsigned int fSpill;
  
  ClassDef(PndTpcEventIdentifier, 1)

};

#endif
