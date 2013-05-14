/** PndTrkClean
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKCLEAN_H
#define PNDTRKCLEAN_H 1


#include "PndGeoHandling.h"


class PndTrkClean : public TObject {
 public:    
  PndTrkClean();
  ~PndTrkClean();    

  int FindMvdLayer(int sensorID);
  
  
 protected:
  PndGeoHandling* fGeoH;

  ClassDef(PndTrkClean,1);
};


#endif
