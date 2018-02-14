#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"


/** Storing crystal set hit information within Geant4 */
class PWO25Hit : public G4VHit
{
  public:

  PWO25Hit(G4Step* aStep);
  ~PWO25Hit();
  PWO25Hit(const PWO25Hit &right);
  const PWO25Hit& operator=(const PWO25Hit &right);

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();


  public:

  inline void SetDE(G4double de) { fDE  = de; }
  inline void AddDE(G4double de) { fDE += de; }
  inline void SetModule(G4int module) { fModule = module; }
  inline void SetPVolume(G4VPhysicalVolume* v) {  fPVolume = v; }
  inline void SetTrackId(G4int id) {  gtrackId_ = id; }


  /** \name Retrieving information */
  inline G4double GetDE() const { return fDE; }
  inline G4int GetModule() const { return fModule; }
  inline G4int TrackId() const { return gtrackId_; }
  inline G4VPhysicalVolume* GetPVolume() const { return fPVolume; }


  private:

  G4double 		fDE;		///< Energy deposit
  G4int 		fModule;	///< Module number
  G4VPhysicalVolume* 	fPVolume;	///< Pointer to physical volume
  G4Step* step;
  G4int                 gtrackId_;

};

/** Use G4 template for hits collection */
typedef G4THitsCollection<PWO25Hit> PWO25HitsCollection;

/** Allocation is done using a G4 template.
 *  see PWO25Hit::operator new() and PWO25Hit::operator delete() */
extern G4Allocator<PWO25Hit> PWO25HitAllocator;

inline void* PWO25Hit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) PWO25HitAllocator.MallocSingle();
  return aHit;
}

inline void PWO25Hit::operator delete(void *aHit)
{
  PWO25HitAllocator.FreeSingle((PWO25Hit*) aHit);
}
