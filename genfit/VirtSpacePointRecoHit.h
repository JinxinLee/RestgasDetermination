/** @addtogroup genfit
 *  @{ */

/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */
#ifndef VIRTSPACEPOINTRECOHIT_H
#define VIRTSPACEPOINTRECOHIT_H

#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

/** @brief Define a hit policy template specialization */
typedef RecoHitIfc<SpacepointHitPolicy> SpacepointRecoHit;

/** @brief A basic space point reco hit. Uses a TVector3 for initialization
 *
 * This class can be used to calculate the distance (residual) of a track
 * to a point in space.
 *
 * It is a very basic example of a RecoHit. VirtSpacePointRecoHit is 
 * initialized by an ordinary 3-vector and has unity error matrix. 
 * It is also a good example which can be used to develop more 
 * realistic RecoHits.
 */
class VirtSpacePointRecoHit : public SpacepointRecoHit {
public:
 
  // Constructors/Destructors ---------
  VirtSpacePointRecoHit();
  VirtSpacePointRecoHit(double x, double y, double z);
  /** @brief Initializing constructor
   *
   * With a RecoHitProducer that works on a TClonseArray containing 
   * TVector3 objects this constructor allows the automatic conversion
   * from a cluster class (here the TVector3) to a RecoHit 
   * (the VirtSpacePointRecoHit). 
   */
  VirtSpacePointRecoHit(const TVector3& pos);

  virtual ~VirtSpacePointRecoHit();

  virtual AbsRecoHit* clone();

  // Operations ----------------------
  /** @brief currently works only with LSLTrackRep
   *
   * Modifications needed here to use VirtSpacePointRecoHit with different
   * track representations.
   *
   * @sa AbsRecoHit::setHMatrix
   */
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


private:

  // Private Data Members ------------
  static const int NparHitRep = 3;

  // Private Methods -----------------

public:
  ClassDef(VirtSpacePointRecoHit,1)


};

#endif

/** @} */
