// Fast Digitization

#ifndef PndSttRootGeometryInitTask_H
#define PndSttRootGeometryInitTask_H 1


#include "PndPersistencyTask.h"
#include "PndGeoSttPar.h"
#include "PndGeoHandling.h"


class PndSttRootGeometryInitTask : public PndPersistencyTask
{

 public:

  /** Default constructor **/  
  PndSttRootGeometryInitTask();


  /** Destructor **/
  virtual ~PndSttRootGeometryInitTask();


  /** Virtual method Init **/
  virtual InitStatus Init();

  virtual InitStatus ReInit();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

 protected:
  void SetParContainers();

 private:
  bool fRunOnce;
  PndGeoSttPar*  fSttParameters;
  PndGeoHandling* fGeoH;


 ClassDef(PndSttRootGeometryInitTask,1);

};

#endif
