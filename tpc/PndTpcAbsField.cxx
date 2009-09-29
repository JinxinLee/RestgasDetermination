//-----------------------------------------------------------
// File and Version Information:
// 
//
// Description:
//      Implementation of class PndTpcAbsField
//      see PndTpcAbsField.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Cristoforo Simonetto    TUM         (original author)
//      Felix Boehmer           TUM
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcAbsField.h"


// Class Member definitions -----------

template <class t>
PndTpcAbsField<t>::PndTpcAbsField(const t& nominalValue,
			       const TVector3& relPosition)
      :frelPosition(relPosition), fnominal(nominalValue), fscale(1)
{;}

template <class t>
t
PndTpcAbsField<t>::derivative(const TVector3& point, TVector3& direction) const
{
  int i;
  for(i=0;i<=10;i++)
  {
    if(pointOk(point+direction)==true
       && pointOk(point-direction)==true)
      break;
    direction=direction*0.5;
  }
  double f=2.0*direction.Mag();
  if(i==10 || f == 0.)                        //given up
  {
    direction=TVector3(0,0,0);
    return(fnominal-fnominal);                //return "0"
  }
  return (value(point+direction)-value(point-direction)) * (1/f);
}

template <class t>
void
PndTpcAbsField<t>::print(std::ostream& s) const
{
  s << "Field\n"
    << "nominal = "<<fnominal<<"\n"
    << "relPosition = ("<<frelPosition.X()<<", "<<frelPosition.Y()<<", "
    << frelPosition.Z()<<")";
}

template <>
void
PndTpcAbsField<TVector3>::print(std::ostream& s) const
{
  s << "Field\n"
    << "nominal = ("<<fnominal.X()<<", "<<fnominal.Y()<<", "<<fnominal.Z()<<")\n"
    << "relPosition = ("<<frelPosition.X()<<", "<<frelPosition.Y()<<", "
    << frelPosition.Z()<<")";
}

template <>
TVector3
PndTpcAbsField<double>::grad(const TVector3& point, const double step) const
{
  TVector3 Vx(step,0,0);
  TVector3 Vy(0,step,0);
  TVector3 Vz(0,0,step);
  double dx = derivative(point, Vx);
  double dy = derivative(point, Vy);
  double dz = derivative(point, Vz);
  return(TVector3(dx,dy,dz));
}

template <>
double
PndTpcAbsField<TVector3>::divergence(const TVector3& point, const double step) const
{
  TVector3 Vx(step,0,0);
  TVector3 Vy(0,step,0);
  TVector3 Vz(0,0,step);
  double dx = (derivative(point, Vx)).X();
  double dy = (derivative(point, Vy)).Y();
  double dz = (derivative(point, Vz)).Z();
  return(dx+dy+dz);
}

template <>
TVector3
PndTpcAbsField<TVector3>::curl(const TVector3& point, const double step) const
{
  TVector3 Vx(step,0,0);
  TVector3 Vy(0,step,0);
  TVector3 Vz(0,0,step);
  TVector3 dx = (derivative(point, Vx));
  TVector3 dy = (derivative(point, Vy));
  TVector3 dz = (derivative(point, Vz));
  return(TVector3(dy.Z()-dz.Y(),
		    dz.X()-dx.Z(),
		    dx.Y()-dy.X()));
}

template class PndTpcAbsField<double>;         
template class PndTpcAbsField<TVector3>;

template <class t>
std::ostream& operator<<(std::ostream& s, const PndTpcAbsField<t>& me)
{
  me.print(s);
  return(s);
}

template std::ostream& operator<<<double>(std::ostream&,
					  const PndTpcAbsField<double>&);
template std::ostream& operator<<<TVector3>(std::ostream&,
					      const PndTpcAbsField<TVector3>&);
