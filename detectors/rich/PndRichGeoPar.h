#ifndef PNDRICHGEOPAR_H
#define PNDRICHGEOPAR_H

#include "FairParGenericSet.h"

class TObjArray;
class FairParamList;

class PndRichGeoPar       : public FairParGenericSet
{
  public:

    /** List of FairGeoNodes for sensitive  volumes */
    TObjArray*      fGeoSensNodes;

    /** List of FairGeoNodes for sensitive  volumes */
    TObjArray*      fGeoPassNodes;

    PndRichGeoPar(const char* name="PndRichGeoPar",
                      const char* title="PndRich Geometry Parameters",
                      const char* context="TestDefaultContext");
    ~PndRichGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() {return fGeoSensNodes;}
    TObjArray* GetGeoPassiveNodes()   {return fGeoPassNodes;}

  private:
    PndRichGeoPar(const PndRichGeoPar&);
    PndRichGeoPar& operator=(const PndRichGeoPar&);

    ClassDef(PndRichGeoPar,1)
};

#endif
