#include "G4VSensitiveDetector.hh"
#include "PWO25Hit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/** Sensitive detector for the set 5x5 crystals **/

class PWO25SD : public G4VSensitiveDetector
{
public:
  PWO25SD(G4String name,G4int modules);
  ~PWO25SD();
  /** A new hit collection with the sensitive detector name and the
   *  collection name is built, a collection id is retrieved and
   *  the collection is added to the HCofThisEvent.
   */
  void Initialize(G4HCofThisEvent*HCE);
  /** If a particle hits this detector, process this single hit.
   *  dE, module number and a pointer to the physical volume
   *  are stored.
   */
  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  /** set pointer in fHitList to NULL.
   *  (the PWO25Hit objects are deleted via fHitCollection)
   */
  void EndOfEvent(G4HCofThisEvent *HCE);
  void clear();
  void DrawAll();
  void PrintAll();

  private:
      PWO25HitsCollection* fHitCollection;
      PWO25Hit** 	 fHitList;
      G4int 		 fModules;
      G4int 		 fCollID;
};
