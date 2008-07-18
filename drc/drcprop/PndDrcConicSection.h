/*! \brief Class for determining the parameters of an conic section with a plane
   which is a ellipse, parabola, or hyperbola. The plane is thought to be a photon
   detector plane. The coordinate system is such that the cone is created at 
   positive z-axis values (downstream) and directed to an photon detector plane 
   upstream. y-axis is up, x-axis such that you get a right handed coordinate system.
   See the red coordinate system in

   \image html PndConicSection.png

   The conic section computation needs 5 x,y coodinates of the plane and the 
   z-coordinate of the cone origin.

   Before that the z-coordinate of the detector plane has to be given otherwise it 
   is assumed to be zero.
*/


#ifndef PNDDRCCONICSECTION_H
#define PNDDRCCONICSECTION_H



#include <vector>
using std::vector;

class PndDrcConicSection
{
 public:

  PndDrcConicSection();                    //!< Empty constructor.

  /*! \brief Copy constructor.
    \param s Object to copy.
  */
  PndDrcConicSection(const PndDrcConicSection& s);

  /*! \brief Assignment operator.
    \param s Object to assign.
  */
  PndDrcConicSection& operator=(const PndDrcConicSection& s);

  virtual ~PndDrcConicSection();          //!< Empty constructor


  /*! \brief Set verbosity
    \param v Verbosity: 0 quiet, ... 5 loud.
  */
  void SetVerbosity(int v) {fVerbosity=v;};
  
  /*! \brief Set z-coordinate of detection plane
    \param z The z-coordinate
  */
  void SetPlane(double z)  {fZ_plane = z;};
  
  /*! \brief Conic section computation
    \param z0 z0 coordinate of cone
    \param x Vector of 5 x coordinates
    \param y Vector of 5 y coordinates
  */
  bool ConicSection(double z0, vector<double>& x, vector<double>& y);
  
  /*! \brief The polar angle of the cone after successfull computation
    \return Polar angle
  */
  double Theta() const {return fTheta;};
  
  /*! \brief The azimuthal angle of the cone after successfull computation
    \return Azimuthal angle
  */
  double Phi() const {return fPhi;};
  
  /*! \brief The opening half-angle of the cone after successfull computation
    \return Opening half-angle
  */
  double Psi() const {return fPsi;};
  
  /*! \brief x-coordinate of the cones origin after successfull computation
    \return coordinate
  */
  double X0() const {return fX0;};
  
  /*! \brief y-coordinate of the cones origin after successfull computation
    \return coordinate
  */
  double Y0() const {return fY0;};
  
  
 private:

  int               fVerbosity;                   //!< Verbosity.
  double            fZ_plane;                     //!< Detector plane.
  double            fTheta;                       //!< Polar angle.
  double            fPhi;                         //!< Azimuthal angle.
  double            fPsi;                         //!< Opening half-angle.
  double            fX0;                          //!< x-ccordinate of cone origin.
  double            fY0;                          //!< y-ccordinate of cone origin.
  


  /*! \brief Auxiliary function for assignment operator and copy constructor..
    \param s The object to copy.
  */
  void Copy(const PndDrcConicSection& s);         

  /*! \brief Rotate coordinates
    \param x Coordinate.
    \param y Coordinate.
    \param phi Rotationangle.
  */
  void Rotate(double& x, double& y, double phi);
  




};


#endif
