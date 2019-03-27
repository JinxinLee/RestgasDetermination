#ifndef PNDP6DIRECT_H
#define PNDP6DIRECT_H 1

#include "TROOT.h"
#include "FairGenerator.h"

#ifdef __CINT__
#define _DLFCN_H_
#define _DLFCN_H
#endif

#include "TPythia6.h"
#include "TMCParticle.h"
class FairPrimaryGenerator;

class PndPythia6Direct : public FairGenerator
{
  public:

    /** default constructor **/
    PndPythia6Direct();

    /** destructor **/
    virtual ~PndPythia6Direct();

    /** public method ReadEvent **/
    Bool_t ReadEvent(FairPrimaryGenerator*);
    void Print(); //!

    virtual Bool_t Init(); //!

    void SetMom(Double_t mom) {
      fMom = mom;
    };
    void SetStoreTree(Bool_t store=true) {
      fStoreTree=store;
    };
    inline void SetVerbose(int v=1) {
      fVerbose=v;
    };
    void SetPandaDefaults();
    TPythia6* GetTPythia6() {
      return fPythia;
    };

  private:

    TPythia6 *fPythia;             //!
    TClonesArray *fParticleList; //!
    TMCParticle *fParticle;
  protected:

    Double_t fMom;       // pbar momentum
    bool fStoreTree;   // flag if we sore also non-final state particles
    int fVerbose; //!
    ClassDef(PndPythia6Direct,1);
};

#endif /* !PNDP6DIRECT_H */

