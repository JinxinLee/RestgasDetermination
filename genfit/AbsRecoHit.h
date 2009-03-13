/** @addtogroup genfit */
/* @{ */


#ifndef ABSRECOHIT_H
#define ABSRECOHIT_H

#include<iostream>

#include "TMatrixT.h"
#include "TObject.h"

#include "AbsTrackRep.h"
#include "DetPlane.h"

/** @brief Base Class for representing a Hit in GENFIT
 *
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 * A hit is defined as a single measurement of a detector. Each detector can 
 * define it's own hit representation (geometry) by inherting from AbsRecoHit. 
 * We call such a child object a "RecoHit" to make clear that
 * it inherits from AbsRecoHit.
 * All detector specific information is handled inside the RecoHit objects.
 * The idea behind this is that the fitting algorithms can work on different
 * detectors simultanously. 
 *
 * AbsRecoHit defines the basic interface that is used by all genfit algorithms
 * to access hit-measurement information. It provides:
 *  - the matrices to store hit coordinates and covariances
 *  - defines methods to access these data
 *  - the interface to access a hit's detector plane object
 *  - method to calculate the residual of a hit to a track representation.
 * 
 * All hits have to inherit from this base class. 
 * Inheritance can be direct or through template class 
 * RecoHitIfc<GeometryPolicy>.
 * These interface classes (defined with a specific policy)
 * provide additional functionality for specific hit geometries, 
 * such as spec points, wires, etc. For details look 
 * at the RecoHitIfc documentation.
 * 
 * A simple example is given in VirtSpacePointRecoHit
 *
 * Background information: The main feature here is
 * that coordinates and covariances are available as general 
 * TMatrixT<double> objects. By using these general algebraic 
 * objects it is possible to abstract from the concrete measurement and 
 * thus define a unified framework to handle the data. This is a key ingredient
 * for the separation of data and algorithms which in turn enables us 
 * to elegantly combine information from different detectors.
 */

class AbsRecoHit : public TObject{
protected:
  /// Transformation from TrackRep frame to Hit coordinates. @sa setHMatrix
  TMatrixT<double> _HMatrix; 

  /// Vector of raw coordinates of hit
  TMatrixT<double> _hitCoord;

  /// Covariance of raw hit coordinates
  TMatrixT<double> _hitCov;

public:
  virtual ~AbsRecoHit();

  /** @brief Constructor specifying dimension of coordinate vector
   *
   * One hit is generally represented by a vector of coordinates. 
   * @param NparHit specifies the dimension of this vector. 
   * (e.g. 3 for a spacepoint, 2 for a pixel, 1 for a strip)
   */
  AbsRecoHit(int NparHit);

  /** @brief Default constructor needed for compatibility with ROOT
   */
  AbsRecoHit();

  /** @brief Set transformation matrix. Transformation between hit 
   * coordinates and track representation coordinates.
   *
   * This is a virtual abstract method which has to be implemented in the child
   * classes.
   *
   * In general there is a linear transformation between the coordinate system
   * of the hit (which is defined by the detector plane) and the coordinates of
   * the track representation in that plane. In the most simple case the track
   * representation has 5 parameters (space + momentum) 
   * while a hit usually has less (one to three space coordinates). 
   *
   * The transformation matrix is then simply projecting out the 
   * space-components of the track representation.
   *
   * Its dimensions are NxM. Where N is the number of dimensions of the 
   * hit in the detector plane (usually 2 or 1) and M is the dimension of the
   * track representation.
   *
   * In this method a hit has to define with which track representations it can
   * work together. It should be the only point where this explicit 
   * coordination is necessary. 
   *
   * For example code see implementing classes below:
   */
  virtual void setHMatrix(const AbsTrackRep* stateVector,
			  const TMatrixT<double>& state)=0;
  
  
  /** @brief Get transformation matrix. Transformation between hit 
   * coordinates and track representation coordinates.
   *
   * @sa setHMatrix
   */
  inline TMatrixT<double> getHMatrix() const {return _HMatrix;}
  

  /** @brief Calculate residual with respect to a track representation.
   *
   * Returns the N-dimensional residual of this vector to a given 
   * track representation.
   *
   * This method is not doing any extrapolation. But it
   * creates the necessary detector plane object. See AbsRecoHit::getDetPlane
   *
   * @param stateVector pointer to track representation - used to synchronize
   * with the track repesentation
   * @param state parameter vector of the track representation 
   *
   * @sa setHMatrix
   * @sa getDetPlane
   */
  TMatrixT<double> residualVector(AbsTrackRep* stateVector,
				  const TMatrixT<double>& state,
				  const DetPlane& d) {
    setHMatrix(stateVector,state);
    return ( getHitCoord(d) - (_HMatrix*state ));
  }

  
  /** @brief Get raw hit covariances. 
   *
   */
  TMatrixT<double> getRawHitCov() const {return _hitCov;}

  /** @brief Get raw hit coordinates. 
   *
   */
  TMatrixT<double> getRawHitCoord() const {return _hitCoord;}

  /** @brief Get hit covariances in a specific detector plane
   *
   * Virtual abstract method has to be implemented by inherting classes.
   * Implementation involves transformation from raw coordinates in detector
   * coordinate system to detector plane coordinate system. 
   * @sa getDetPlane
   */
  virtual TMatrixT<double> getHitCov(const DetPlane&)=0;

  /** @brief Get hit coordinates in a specific detector plane
   *
   * Virtual abstract method has to be implemented by inherting classes.
   * Implementation involves transformation from raw coordinates in detector
   * coordinate system to detector plane coordinate system. 
   * @sa getDetPlane
   */
  virtual TMatrixT<double> getHitCoord(const DetPlane&)=0;
  
  
  /** @brief Get detector plane for a given track representation.
   *
   * Virtual abstract method has to be implemented by inherting classes.
   *
   * In general the detector plane can depend both on the detector/hit geometry
   * as well as the track geometry. E.g. for a space point one usually chooses 
   * a plane that is perpendicular to the current track, since in that case no 
   * other plane is predefined.
   *
   * There are several implementations for this method in the HitPolicies. 
   * In the most simple case (a planar detector) the method just returns a
   * fixed (detector module specific) plane. This behaviour for example is 
   * implemented in PlanarHitPolicy.
   */
  virtual const DetPlane& getDetPlane(AbsTrackRep*)=0; 
   

  
  /** @brief Get clone of this object.
   *
   * Virtual abstract method. Has to be implemented by inherting classes.
   * Creates a deep copy of this object. 
   * Ownership is trandsferred to the caller!
   */
  virtual AbsRecoHit* clone() = 0;

  /** @brief Print raw hit coordinates.
   */
  virtual void Print() {_hitCoord.Print();}
  
  ClassDef(AbsRecoHit,1)

};
  

#endif //FITTER_ABSHIT_H

/** @} */ 
