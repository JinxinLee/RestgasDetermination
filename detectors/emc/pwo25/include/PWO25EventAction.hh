#include "G4UserEventAction.hh"

class G4Event;

class PWO25EventAction : public G4UserEventAction {
public:
  PWO25EventAction();
  ~PWO25EventAction();
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

};
