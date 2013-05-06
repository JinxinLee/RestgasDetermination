#ifndef PNDMCLISTCONVERTER_H
#define PNDMCLISTCONVERTER_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TDatabasePDG;

class PndMcListConverter : public FairTask
{

  public:
    /** Default constructor **/
    PndMcListConverter();


    /** Destructor **/
    ~PndMcListConverter();


    /** Virtual method Init **/
    virtual InitStatus Init();


    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void FinishEvent();
    //void CreateStructure();

    void SetVerbose(Bool_t verb) { fVerbose = verb  ;};

  private:

    /** Input array of TpcLheTrack **/
    TClonesArray* fMcCandidates;
    TClonesArray* fMcTracks;

    TDatabasePDG* fPdg;

    /** Geo file to use **/

    /** Get parameter containers **/
    virtual void SetParContainers();


    ClassDef(PndMcListConverter,1);

};

#endif
