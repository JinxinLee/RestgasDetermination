#ifndef PNDMDTTRKFINDER_H
#define PNDMDTTRKFINDER_H 1

#include <map>
#include <vector>

#include "FairTask.h"
#include "PndMdtTrk.h"
#include "PndMdtRecoPar.h"

#include "PndTrack.h"
#include "FairGeanePro.h"

class TClonesArray;

using std::map;
using std::vector;

class PndMdtTrkFinder : public FairTask
{

  public:

    /** Default constructor **/  
    PndMdtTrkFinder();


    /** Destructor **/
    ~PndMdtTrkFinder();


    /** Virtual method Init **/
    virtual InitStatus Init();


    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void SetParContainers();


  private: 

    PndMdtTrk* AddTrk(PndMdtTrk* track);

    /** Input array of PndMdtHit **/
    TClonesArray* fHitArray; 

    /** Output array of PndMdtTrk **/
    TClonesArray* fTrkArray; 

    //PndMdtRecoPar *fRecoPar;

    ClassDef(PndMdtTrkFinder,1);


};

#endif
