/*! 
  \file PndDrcUtil.h
  \brief Utility functions 

*/


#ifndef PNDDRCUTIL_H
#define PNDDRCUTIL_H

#include <string>
#include <fstream>
//using std::fstream;
using std::ostream;
//using std::istream;
using std::string;

#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;
#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;

class TVector3;
//class ostream;



/*! \brief Convert int to string  for only one digit
  \param i int<10
  \return string
*/
string itoa1(int i);
  
/*! \brief Convert number to string
  \param i The integer number to convert.
  \param idigits Fixed number of digits.
  \return string
*/
string itoa(int i, int idigits=0);

/*! \brief Rotate vector
  \param p1 Vector
  \param the rotate by theta
  \param phi rotate by phi
  \return rotated vector 
*/
const XYZVector angle_tra(const XYZVector &p1,
			  const double    &the,
			  const double    &phi);

ostream& operator << (ostream &stream, const TVector3 &t);
ostream& operator << (ostream &stream, const XYZVector &t);
ostream& operator << (ostream &stream, const XYZPoint &t);



#endif

