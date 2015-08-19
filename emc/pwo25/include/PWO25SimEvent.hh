#include "TObject.h"
#include "TClonesArray.h"

// Representing one event in the root tree.

class PWO25SimHit;

class PWO25SimEvent : public TObject {

 private:

  /** Number of hits in the fHits TClonesArray.
   *  In principle this information is also available directly within
   *  the TClonesArray. This is for easier handling.
   */

  int	fEntries;

  /** Array containing objects of class PWO25SimHit */
  TClonesArray*	fHits;

 public:
  PWO25SimEvent();
  ~PWO25SimEvent();

  /** Add one hit.
   *  \param module Number of the detector module
   *  \param energy Energy deposit
  */

  void Add(int module, double energy,int trackId);
  /** Clears event, i.e. reset array and set fEntries=0. */
  virtual void Clear(const Option_t* opt = 0);

  inline int GetEntries() { return fEntries; }
  inline TClonesArray* GetHits() { return fHits; }

  ClassDef(PWO25SimEvent,2)
};
