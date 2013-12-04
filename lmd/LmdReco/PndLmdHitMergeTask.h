// -------------------------------------------------------------------------
// -----                 PndLmdHitMergeTask header file             -----
// -------------------------------------------------------------------------



#ifndef PNDLMDHITMERGETASK_H
#define PNDLMDHITMERGETASK_H

#include "FairTask.h"
#include "PndSdsHit.h"
#include "PndLmdDim.h"

#include <string>
#include <vector>

#include "TH2D.h"
class TClonesArray;

class PndLmdHitMergeTask : public FairTask
  {
  public:
    
    /** Default constructor **/
    
    PndLmdHitMergeTask(); 
    PndLmdHitMergeTask(const char* name); 
    
    /** Destructor **/
    virtual ~PndLmdHitMergeTask();

    
    /** Virtual method Init **/
    virtual InitStatus Init();
    

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    
    /** Set path to geometry matricies [explicitly needed only in alignment studies!] **/
    void SetMtxPath(TString _path){mtxpath = _path;};
    /** Set "alignment consts are known" flag **/
    void SetAlignFlag(bool fflag){readAlign = fflag;}
  protected:
    
    TClonesArray* fHitArray;  // Input array of PndSdsHits
    TClonesArray* fMergedHitArray;  // Output array of PndSdsHits
    
    TString fHitBranchName;

    PndLmdDim* lmddim;
    TH2* hdxdy;
    TH1* hdz;
    bool readAlign;
    TString mtxpath;//path to file(s) with geometry matrix
    ClassDef(PndLmdHitMergeTask,0);
    
  };

#endif /* PNDLMDHITMERGETASK_H */

