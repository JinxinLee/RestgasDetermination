#include "G4UserRunAction.hh"
#include "globals.hh"
#include "TFile.h"
#include "TTree.h"
#include "PWO25SimEvent.hh"

class G4Run;


class PWO25RunAction : public G4UserRunAction
{
public:

  PWO25RunAction();
  ~PWO25RunAction();

  /** Returns a pointer to the (single) instance of this class */
  static inline PWO25RunAction* GetInstance() { return fInstance; };

  void BeginOfRunAction(const G4Run* aRun);
  void EndOfRunAction(const G4Run* aRun);


  static inline PWO25SimEvent* GetPWO25Event() { return fPWO25Event; };
  inline TTree* GetRootTree() { return rootDataTree; };

private:

  static PWO25RunAction* fInstance;
  /** This is the object linked with the Root tree */
  static PWO25SimEvent* fPWO25Event;
  TFile *rootFile;
  TTree*  rootDataTree;

};
