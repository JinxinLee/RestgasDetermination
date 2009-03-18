/** @addtogroup genfit
 * @{
 */

#ifndef ABSTRACKREP_H
#define ABSTRACKREP_H

#include<vector>
#include<list>

#include "TMatrixT.h"
#include "TVector3.h"

#include "DetPlane.h"

class AbsRecoHit;    

/** @brief Base Class for genfit track representations. 
 * Defines interface for track parameterizations.
 *
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 * It is important to understand the difference between a track and a 
 * track representation in genfit:
 * - A track representation is a specific parameterization of a trajectory. 
 * It contains the parameters that describe the track at some point and 
 * code for the extrapolation of the track parameters through space. 
 * The actual extrapolation code is not part of genfit but has to be supplied
 * in some additional package (e.g. GEANE). LSLTrackRep is a very basic example
 * of a track representation.
 * - A Track is a collection of RecoHits (see AbsRecoHit) plus a collection
 * of track representation objects. The hits can be from different detectors.
 * There can be several representations of the same track. This makes it 
 * possible to perform several fits in parallel, for example to compare 
 * different parameterizations or to fit different particle hypotheses.
 *
 * All track tepresentations must inherit AbsTrackRep to be available 
 * in genfit. Algorithms in genfit use this class as interface to 
 * access track parameters
 *
 * Provides:
 *  - Matrix objects to store track parameters
 *  - ... and covariances
 *  - interface to track extrapolation code
 *
 * The track extrapolation engine can be exchanged in genfit. 
 * Or one can even use more than one engine in parallel!
 * In order to use a track extrapolation engine (like e.g. GEANE) with genfit
 * one has to write a TrackRep class that inherits from AbsTrackRep. This makes
 * it possible to uses different track extrapolation codes within a unified
 * framework without major changes in the detector code.
 *
 * There is only one thing one has to do to use a specific track 
 * representation together with the hits from a detector: 
 * add the respective code in the AbsRecoHit::setHMatrix method implementation
 * of the RecoHit in question.
 */

class AbsTrackRep : public TObject{

  /*----- Data mambers -----*/
 protected:
  //! Dimensionality of track representation
  unsigned int dimension;

  //! The vector of track parameters
  TMatrixT<double> state;
  
  //! The covariance matrix
  TMatrixT<double> cov;   
  
  TMatrixT<double> startState; 
  TMatrixT<double> startCov;
  DetPlane startPlane;  

  // detector plane where the track parameters are given
  DetPlane         _refPlane;
  
  //! chiSqu of the track fit
  double           chiSqu;
  unsigned int     ndf;

  //! status of track representation: 0 means everything's OK
  int statusFlag; 
  //! specifies the direction of flight of the particle
  bool inverted; 

 public:
  virtual AbsTrackRep* clone() const = 0;

  virtual AbsTrackRep* prototype() const = 0;
  
  //! returns the tracklength spanned in this extrapolation
  /* There is a default implementation in AbsTrackRep.cxx which just drops
     the predicted covaraiance. If your trackrep has a way to extrapolate
     without giving a correct cov (that would be faster probably), please
     overwrite it.
  */
  virtual double extrapolate(const DetPlane& plane, TMatrixT<double>& statePred);


 public:

  AbsTrackRep();
  AbsTrackRep(int);
  virtual ~AbsTrackRep();


  //! This method is to extrapolate track to point of closest approach to a point in space
  /*! There is an empty implementation of this method in AbsTrackRep.cxx,
      which will just abort with an error message. One can overwrite this
      method if one wishes to implement a track representation, which should
      have this feature. An example of an experiment in which you would not
      need this feature would be track fitting (not so much vertexing) in
      an experiment with only planar trackers like silicons or planar wire
      chambers and such. An example where you would need it, would be a TPC
      where you have to fit the track to space points, or other drift chambers
      with complicated hit topology.
   */
  virtual void extrapolateToPoca(const TVector3& point,
				 TVector3& poca,
				 TVector3& dirInPoca); 

  //! 
  /** @brief This method extrapolates to the point of closest approach to a line
   * 
   * This method extrapolates to the POCA to a line, i.e. a wire. There
   * is a default implementation just like for the extrapolateToPoca for
   * trackReps which do not need this feature, which will abort the
   * execution if it is ever called.
   */
  virtual void extrapolateToLine(const TVector3& point1, 
				 const TVector3& point2,
				 TVector3& poca,
				 TVector3& dirInPoca);
  
  
  //! make step of h cm along the track
  /*! There is an emply implementation in AbsTrackRep.cxx which will abort
      (see one of the extrapolate methods above). This can be overwritten,
      if this feature is needed.
  */
  virtual void stepalong(double h); 

  //! Extrapolates the track to the given detetorplane
  /*! Results are put into statePred and covPred
      This method does NOT alter the state of the object!
   */ 
  virtual double extrapolate(const DetPlane& plane,
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred)=0;

  //! This changes the state and cov and plane of the rep
  /*! This method extrapolates to to the plane and sets the results of state,
      cov and also plane in itself.
  */
  double extrapolate(const DetPlane& plane);

  //! returns dimension of state vector
  int getDim() const {return dimension;}  
  
  virtual void Print() const;

  //! Accessors:
  inline TMatrixT<double> getState() const {
    return state;
  }
  inline TMatrixT<double> getCov() const {
    return cov;
  }

  double getStateElem(int i) const {return state(i,0);}
  double getCovElem(int i, int j) const {return cov(i,j);}

  

  virtual TVector3 getPos(const DetPlane& pl)=0; 
  virtual TVector3 getMom(const DetPlane& pl)=0; 
  virtual void getPosMom(const DetPlane& pl,TVector3& pos,TVector3& mom)=0;
  virtual double getCharge()const =0;

  TVector3 getPos() {return getPos(_refPlane);}
  TVector3 getMom() {return getMom(_refPlane);}

  inline TMatrixT<double> getStartState() const {
    return startState;
  }
  inline TMatrixT<double> getStartCov() const {
    return startCov;
  }
  inline DetPlane getStartPlane() const {
    return startPlane;
  }
  inline double getChiSqu() const {
    return chiSqu;
  }
  //! returns chi2/ndf
  inline double getRedChiSqu() const {
    if(ndf>0) return chiSqu/ndf;
    return 0;
  }
  inline unsigned int getNDF() const {
    return ndf;
  }

  inline void setState(const TMatrixT<double>& aState) {
    state = aState;
  }
  inline void setCov(const TMatrixT<double>& aCov) {
    cov = aCov;
  }
  inline void setStartState(const TMatrixT<double>& aState) {
    startState = aState;
  }
  inline void setStartCov(const TMatrixT<double>& aCov) {
    startCov = aCov;
  }
  inline void setStartPlane(const DetPlane& aPlane) {
    startPlane = aPlane;;
  }

  //! sets the referene plane
  /* There is a default implementation in AbsTrackRep.cxx which just puts
     pl as the reference plane. This method if virtual (so can be over-
     written) because some track reps want to use this place to define
     their free parameter. For example if a track has the z-axis as its free
     parameter it will update is from the detplane given here.
  */
  virtual void setReferencePlane(const DetPlane& pl);

  const DetPlane& getReferencePlane() const {return _refPlane;}

  inline void setChiSqu(double aChiSqu) {
    chiSqu = aChiSqu;
  }
  inline void setNDF(unsigned int n) {
    ndf = n;
  }
  inline void addChiSqu(double aChiSqu) {
    chiSqu += aChiSqu;
  }
  inline void addNDF(unsigned int n) {
    ndf += n;
  }
  inline void setStatusFlag(int _val) {
	statusFlag = _val;
  }
  
  virtual void switchDirection() = 0;
  bool setInverted(bool f=true){inverted=f;}
  
  inline bool getStatusFlag() {
	return statusFlag;
  }

  virtual void reset();

 private:
  void Abort(std::string method);


  ClassDef(AbsTrackRep,2)

};

/* @} **/


#endif
