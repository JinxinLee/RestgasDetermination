#include "TObject.h"

// Holding information of one hit.

class PWO25SimHit : public TObject {
 private:
  int		fModule;       ///< Module number
  double	fE;		       ///< Energy in module
  int       track_;        ///< GTrack ID

 public:

  PWO25SimHit() {fModule=-1; fE=0.;};

  PWO25SimHit(int module, double energy, int trackId);
  ~PWO25SimHit();

  inline int GetModule() const {return fModule;}
  inline double GetE() const {return fE;}
  inline double GetTrackId() const {return track_;}

  ClassDef(PWO25SimHit,2)
};
