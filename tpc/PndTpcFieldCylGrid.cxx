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
				       const double minR, const double minZ,
				       const double spacingR, 
				       const double spacingZ,
				       const TVector3& relPosition,
				       std::vector<std::vector<t*>*>* pGrid)
  : PndTpcAbsField<t>(nominalValue, relPosition),
    _pGrid(pGrid), _minX(minR), _minZ(minZ),
    _spacingX(spacingR), _spacingZ(spacingZ) 
{
  evalMaxPoint();
}

template <class t>
PndTpcFieldCylGrid<t>::PndTpcFieldCylGrid(const PndTpcFieldCylGrid& aFCG):
  PndTpcAbsField<t>(aFCG.nominal(), aFCG.relPosition())
{
  _pGrid = new std::vector<std::vector<t*>*>(*aFCG._pGrid);  
  _minX = aFCG.minR();        
  _minZ = aFCG.minZ();
  _spacingX = aFCG.spacingR();
  _spacingZ = aFCG.spacingZ();
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
//   delete _pGrid;
//   PndTpcAbsField<t>::operator=(aFCG);

//   // like the copy constructor
//   _pGrid = new std::vector<std::vector<t*>*>(*aFCG._pGrid);
//   _minX = aFCG.minR();
//   _minZ = aFCG.minZ();
//   _spacingX = aFCG.spacingR();
//   _spacingZ = aFCG.spacingZ();
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
    << "minX="<<_minX << "; minZ="<<_minZ <<"\n"
    << "maxX="<<_maxX << "; maxZ="<<_maxZ;
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
    << "minX="<<_minX << "; minZ="<<_minZ <<"\n"
    << "maxX="<<_maxX << "; maxZ="<<_maxZ;
}


// Accessors --------------------------------------------------------------------

template <class t>
t
PndTpcFieldCylGrid<t>::value(const TVector3& point) const
{
  return( evalValue(point) );
}

template <class t>
bool 
PndTpcFieldCylGrid<t>::pointOk(const TVector3& point) const
{
  TVector3 aPoint = point - this-> relPosition();
  double r2 =(aPoint.X()*aPoint.X())+(aPoint.Y()*aPoint.Y());
  if (r2 < _minX*_minX ||
      aPoint.Z() < _minZ)
    return(false);
  if (r2  >= _maxX*_maxX ||
      aPoint.Z() >= _maxZ)
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
  if (_pGrid == 0)
    return;
   
  _maxX = _minX + _spacingX * _pGrid->size();
  _maxZ = _minZ + _spacingZ * _pGrid->at(0)->size();   
}


// Private Methods ----------------------------------------------------------


template <class t>
t 
PndTpcFieldCylGrid<t>::evalValue(const TVector3& point) const
{
  //check if _pGrid exists
  if (_pGrid == 0)
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
  double difX = aPoint.X()-_minX;
  double difZ = aPoint.Z()-_minZ;
  int xIndex = (int)std::floor( difX / _spacingX );
  int zIndex = (int)std::floor( difZ / _spacingZ );
  
  //actual deviations from the grid-field's "center of mass"
  double dx = difX - (xIndex + 0.5)*_spacingX;
  double dz = difZ - (zIndex + 0.5)*_spacingZ;
  
  const t value = startInterpolation(xIndex, zIndex, dx, dz);
  return (rotate(value, phi));
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
    return (interpolate(xBin, zBin, dx, dz+_spacingZ));
  if(dx<0 && dz<0)
    return (interpolate(xBin, zBin, dx+_spacingX, dz+_spacingZ));
  if(dx<0 && dz>=0)
    return (interpolate(xBin, zBin, dx+_spacingX, dz));

}

template <class t>
t 
PndTpcFieldCylGrid<t>::interpolate(int& xBin, int& zBin, double _dx, double _dz) const
{
  double dx = _dx;
  double dz = _dz;
  
  if (zBin<0)              //catch the cases we are out of bound  
  {   
    zBin++;                                      
    return (interpolate(xBin, zBin, dx, 0));
  }
  if (zBin==(int)_pGrid->at(0)->size()-1)
    dz = 0;
  if (xBin<0)
  {
    xBin++;
    return (interpolate(xBin, zBin, 0, dz));
  }
  if (xBin==(int)_pGrid->size()-1)
    dx = 0;

  t b1, b2, b3, b4;      //the 4 surrounding _pGrid values 

  double U = dx/_spacingX;  //normalize to binWidth
  double T = dz/_spacingZ;
  
  b1 = *(_pGrid->at(xBin)->at(zBin));       //starting grid-bin, ok in any case

  if(dx==0 && dz==0)
    return b1;                              //no interpolation at all.
 
  if(dx==0)
  {
    b2 = *(_pGrid->at(xBin)->at(zBin+1));
    return (1-T)*b1 + T*b2;
  }
  
  if(dz==0)
  {
    b4 = *(_pGrid->at(xBin+1)->at(zBin));
    return (1-U)*b1 + U*b4;
  }    
  
  //the general case, we are not at the boundaries of the grid
  b2 = *(_pGrid->at(xBin)->at(zBin+1));
  b3 = *(_pGrid->at(xBin+1)->at(zBin+1));
  b4 = *(_pGrid->at(xBin+1)->at(zBin));

  return ((1-T)*(1-U)*b1 + T*(1-U)*b2 + T*U*b3 + (1-T)*U*b4);
}

template <class t>
void
PndTpcFieldCylGrid<t>::deleteGrid()
{
  if(_pGrid == 0)
    return;
  
  for(unsigned int i=0; i<_pGrid->size(); i++)
    for(unsigned int j=0; j<_pGrid->at(0)->size(); j++)
      delete _pGrid->at(i)->at(j);

  for(unsigned int i=0; i<_pGrid->size(); i++)
    delete _pGrid->at(i);

  delete _pGrid;
}


template class PndTpcFieldCylGrid<double>;
template class PndTpcFieldCylGrid<TVector3>;


//Update 29.06.07, fboehmer: Greatly improved interpolation 
