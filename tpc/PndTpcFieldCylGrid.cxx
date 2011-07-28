//-----------------------------------------------------------
// File and Version Information:
// 
//
// Description:
//      Implementation of class FieldCylGrid
//      see FieldCylGrid.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM            (original author)
//      Felix Boehmer           TUM
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcFieldCylGrid.h"


// Collaborating Class Headers --------
#include <cmath>
#include "TError.h"

// Class Member definitions -----------

template <class t>
PndTpcFieldCylGrid<t>::PndTpcFieldCylGrid(const t& nominalValue,
				       const double MinR, const double MinZ,
				       const double SpacingR, 
				       const double SpacingZ,
				       const TVector3& RelPosition,
				       std::vector<std::vector<t*>*>* pGrid)
  : PndTpcAbsField<t>(nominalValue, RelPosition),
    fpGrid(pGrid), fminX(MinR), fminZ(MinZ),
    fspacingX(SpacingR), fspacingZ(SpacingZ) 
{
  evalMaxPoint();
}

template <class t>
PndTpcFieldCylGrid<t>::PndTpcFieldCylGrid(const PndTpcFieldCylGrid& aFCG):
  PndTpcAbsField<t>(aFCG.nominal(), aFCG.relPosition())
{
  fpGrid = new std::vector<std::vector<t*>*>(*aFCG.fpGrid);  
  fminX = aFCG.minR();        
  fminZ = aFCG.minZ();
  fspacingX = aFCG.spacingR();
  fspacingZ = aFCG.spacingZ();
  evalMaxPoint();
}

template <class t>
PndTpcFieldCylGrid<t>::~PndTpcFieldCylGrid()
{
  deleteGrid(); //PndTpcFieldCylGrid has ownership of this pointer
}


// Operators    ----------------------------------------------------------------


// template <class t>
// PndTpcFieldCylGrid<t>::PndTpcFieldCylGrid<t>& 
// PndTpcFieldCylGrid<t>::operator=(const PndTpcFieldCylGrid<t>& aFCG)
// {
//   delete fpGrid;
//   PndTpcAbsField<t>::operator=(aFCG);

//   // like the copy constructor
//   fpGrid = new std::vector<std::vector<t*>*>(*aFCG.fpGrid);
//   fminX = aFCG.minR();
//   fminZ = aFCG.minZ();
//   fspacingX = aFCG.spacingR();
//   fspacingZ = aFCG.spacingZ();
//   evalMaxPoint();
//   return(*this);
// }

template <class t>
void
PndTpcFieldCylGrid<t>::print(std::ostream& s) const
{
  s << "PndTpcFieldCylGrid\n"
    << "nominal="<<this->nominal()<<"\n"
    << "scale=" <<this->scale()<< "\n"
    << "minX="<<fminX << "; minZ="<<fminZ <<"\n"
    << "maxX="<<fmaxX << "; maxZ="<<fmaxZ;
}

template <>
void
PndTpcFieldCylGrid<TVector3>::print(std::ostream& s) const
{
  s << "PndTpcFieldCylGrid\n"
    << "nominal=("<<this->nominal().X()<<", "<<this->nominal().Y()
    <<", "<<this->nominal().Z()
    << ")\n"
    << "scale=" <<this->scale()<< "\n"
    << "minX="<<fminX << "; minZ="<<fminZ <<"\n"
    << "maxX="<<fmaxX << "; maxZ="<<fmaxZ;
}


// Accessors --------------------------------------------------------------------

template <class t>
t
PndTpcFieldCylGrid<t>::value(const TVector3& point) const
{
  return (evalValue(point)) * (PndTpcAbsField<t>::scale()) ;
}

template <class t>
bool 
PndTpcFieldCylGrid<t>::pointOk(const TVector3& point) const
{
  TVector3 aPoint = point - this-> relPosition();
  double r2 =(aPoint.X()*aPoint.X())+(aPoint.Y()*aPoint.Y());
  if (r2 < fminX*fminX ||
      aPoint.Z() < fminZ)
    return(false);
  if (r2  >= fmaxX*fmaxX ||
      aPoint.Z() >= fmaxZ)
    return(false);
  return(true);
}

// Protected Methods --------------------------------------------------------------

//default
template <class t>          //this implementation is ugly, but works
t
PndTpcFieldCylGrid<t>::rotate(const t& valRZ, const double phi) const
{
  t instValRZ = valRZ;
  return(instValRZ);
}
//specialization for TVector3 / bad as well, needs a change
template <>
TVector3
PndTpcFieldCylGrid<TVector3>::rotate(const TVector3& valRZ, const double phi) const
{
  TVector3 rotValRZ = valRZ;
  rotValRZ.RotateZ(phi);
  return rotValRZ;
}

//returns max coordinates evaluated from fieldmap-size
template <class t>
void
PndTpcFieldCylGrid<t>::evalMaxPoint()   
{
  if (fpGrid == 0)
    return;
   
  fmaxX = fminX + fspacingX * fpGrid->size();
  fmaxZ = fminZ + fspacingZ * fpGrid->at(0)->size();   
}


// Private Methods ----------------------------------------------------------


template <class t>
t 
PndTpcFieldCylGrid<t>::evalValue(const TVector3& point) const
{
  //check if fpGrid exists
  if (fpGrid == 0)
    Fatal("PndTpcFieldCylGrid::evalDerivOrValue", "Field Data does not exist!");
  //check if we are in our volume
  if (pointOk(point) == false )
  { 
    Error("PndTpcFieldCylGrid::evalDerivOrValue", "Point outside the volume");
    return(this->nominal());
  }

  TVector3 aPoint = point - this->relPosition();
  
  // find grid point next to us
  double phi = aPoint.Phi();
  aPoint.RotateZ(-phi);
  double difX = aPoint.X()-fminX;
  double difZ = aPoint.Z()-fminZ;
  int xIndex = (int)std::floor( difX / fspacingX );
  int zIndex = (int)std::floor( difZ / fspacingZ );
  
  //actual deviations from the grid-field's "center of mass"
  double dx = difX - (xIndex + 0.5)*fspacingX;
  double dz = difZ - (zIndex + 0.5)*fspacingZ;
  
  const t Value = startInterpolation(xIndex, zIndex, dx, dz);
  return (rotate(Value, phi));
}

template <class t>
t 
PndTpcFieldCylGrid<t>::startInterpolation(int& xBin, int& zBin, double dx, double dz) const
{
  //determine the grid rectangle aPoint lies in and define 
  //the surrounding 4 rectangles counter-clockwise
  if(dx < 0)
    xBin--;
  if(dz < 0)
    zBin--;
  
  if(dx>=0 && dz>=0)
    return (interpolate(xBin, zBin, dx, dz));
  if(dx>=0 && dz<0)
    return (interpolate(xBin, zBin, dx, dz+fspacingZ));
  if(dx<0 && dz<0)
    return (interpolate(xBin, zBin, dx+fspacingX, dz+fspacingZ));
  if(dx<0 && dz>=0)
    return (interpolate(xBin, zBin, dx+fspacingX, dz));

}

template <class t>
t 
PndTpcFieldCylGrid<t>::interpolate(int& xBin, int& zBin, double fdx, double fdz) const
{
  double dx = fdx;
  double dz = fdz;
  
  if (zBin<0)              //catch the cases we are out of bound  
  {   
    zBin++;                                      
    return (interpolate(xBin, zBin, dx, 0));
  }
  if (zBin==(int)fpGrid->at(0)->size()-1)
    dz = 0;
  if (xBin<0)
  {
    xBin++;
    return (interpolate(xBin, zBin, 0, dz));
  }
  if (xBin==(int)fpGrid->size()-1)
    dx = 0;

  t b1, b2, b3, b4;      //the 4 surrounding fpGrid values 

  double U = dx/fspacingX;  //normalize to binWidth
  double T = dz/fspacingZ;
  
  b1 = *(fpGrid->at(xBin)->at(zBin));       //starting grid-bin, ok in any case

  if(dx==0 && dz==0)
    return b1;                              //no interpolation at all.
 
  if(dx==0)
  {
    b2 = *(fpGrid->at(xBin)->at(zBin+1));
    return (1-T)*b1 + T*b2;
  }
  
  if(dz==0)
  {
    b4 = *(fpGrid->at(xBin+1)->at(zBin));
    return (1-U)*b1 + U*b4;
  }    
  
  //the general case, we are not at the boundaries of the grid
  b2 = *(fpGrid->at(xBin)->at(zBin+1));
  b3 = *(fpGrid->at(xBin+1)->at(zBin+1));
  b4 = *(fpGrid->at(xBin+1)->at(zBin));

  return ((1-T)*(1-U)*b1 + T*(1-U)*b2 + T*U*b3 + (1-T)*U*b4);
}

template <class t>
void
PndTpcFieldCylGrid<t>::deleteGrid()
{
  if(fpGrid == 0)
    return;
  
  for(unsigned int i=0; i<fpGrid->size(); i++)
    for(unsigned int j=0; j<fpGrid->at(0)->size(); j++)
      delete fpGrid->at(i)->at(j);

  for(unsigned int i=0; i<fpGrid->size(); i++)
    delete fpGrid->at(i);

  delete fpGrid;
}


template class PndTpcFieldCylGrid<double>;
template class PndTpcFieldCylGrid<TVector3>;


//Update 29.06.07, fboehmer: Greatly improved interpolation 
