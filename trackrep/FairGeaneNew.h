// Class for the GEANE initialization
//
// Author: M. Al-Turany
//
#ifndef FAIRGeaneNEW_H
#define FAIRGeaneNEW_H

#include "TObject.h"
#include "TString.h"

#include "GeaneMCApplication.h"

//#include "FairField.h"
//#include "PndFieldAdaptor.h"
#include "AbsBField.h"

class FairGeaneNew : public TObject
{
 public:

  /** Default constructor **/
  FairGeaneNew();

  FairGeaneNew(const char* name);



  virtual ~FairGeaneNew();

  private:
  
  GeaneMCApplication* app;
  
  public:

  void SetField(AbsBField* f){app->setField(f);}
//app->setField(new PndFieldAdaptor(f));}

  ClassDef(FairGeaneNew,1)
}; 


#endif
