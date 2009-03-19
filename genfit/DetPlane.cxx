#include "DetPlane.h"

#include "assert.h"
#include <iostream>
#include <cmath>
#include "TMath.h"
#include "TRandom3.h"

ClassImp(DetPlane)

DetPlane::DetPlane(const TVector3& o,
		   const TVector3& u,
		   const TVector3& v) 
:_o(o),_u(u),_v(v)
{
  sane();
}
DetPlane::DetPlane()
{
  static TRandom3 r(0);
  _o.SetXYZ(0.,0.,0.);
  _u.SetXYZ(r.Uniform(),r.Uniform(),0.);
  _v.SetXYZ(r.Uniform(),r.Uniform(),0.);
  sane();
}

DetPlane::DetPlane(const TVector3& o,
				   const TVector3& n)
  :_o(o){
  setNormal(n);
}

void 
DetPlane::set(const TVector3& o,
	      const TVector3& u,
	      const TVector3& v)
{
  _o=o;_u=u;_v=v;
  sane();
}

void 
DetPlane::setO(const TVector3& o)
{
  _o=o;
  sane();
}

void 
DetPlane::setU(const TVector3& u)
{
  _u=u;
  sane();
}

void 
DetPlane::setV(const TVector3& v)
{
  _v=v;
  sane();
}

TVector3
DetPlane::getNormal() const
{
  TVector3 result=_u.Cross(_v);
  result.SetMag(1.);
  return result;
}

void
DetPlane::setNormal(TVector3 n){
  n.SetMag(1.);
  if( fabs(n.X()) > 0.1 ){
	_u.SetXYZ(1./n.X()*(-1.*n.Y()-1.*n.Z()),1.,1.);
	_u.SetMag(1.);
  }
  else {
	if(fabs(n.Y()) > 0.1){
	  _u.SetXYZ(1.,1./n.Y()*(-1.*n.X()-1.*n.Z()),1.);
	  _u.SetMag(1.);
	}
	else{
	  _u.SetXYZ(1.,1.,1./n.Z()*(-1.*n.X()-1.*n.Y()));
	  _u.SetMag(1.);
	}
  }
  _v=n.Cross(_u);   
}

void DetPlane::setNormal(const double& theta, const double& phi){
  TVector3 n(TMath::Sin(theta)*TMath::Cos(phi),TMath::Sin(theta)*TMath::Sin(phi),TMath::Cos(theta));
  setNormal(n);
}

TVector2
DetPlane::project(const TVector3& x)const
{
  Double_t x_u=_u*x;
  Double_t x_v=_v*x;
  return TVector2(x_u,x_v);
}

TVector2 
DetPlane::LabToPlane(const TVector3& x)const
{
  TVector3 d=x-_o;
  return project(d);
}

TVector3
DetPlane::toLab(const TVector2& x)const
{
  TVector3 d(_o);
  d+=x.X()*_u;
  d+=x.Y()*_v;
  return d;
}



TVector3 
DetPlane::dist(const TVector3& x)const
{
  TVector2 p=LabToPlane(x);
  TVector3 xplane=toLab(p);
  return xplane-x;
}



void
DetPlane::sane(){
  assert(_u!=_v);
  // ensure unit vectors
  _u.SetMag(1.);
  _v.SetMag(1.);
  // ensure orthogonal system
  // _v should be reached by 
  // rotating _u around _n in a counterclockwise direction

  TVector3 n=getNormal();
  _v=n.Cross(_u);
  
  TVector3 v=_u;
  v.Rotate(TMath::Pi()*0.5,n);
  TVector3 null=v-_v;
  assert(null.Mag()<1E-6);
 
}


void
DetPlane::Print() const
{
  std::cout<<"DetPlane: "
	   <<"O("<<_o.X()<<","<<_o.Y()<<","<<_o.Z()<<") "
	   <<"u("<<_u.X()<<","<<_u.Y()<<","<<_u.Z()<<") "
	   <<"v("<<_v.X()<<","<<_v.Y()<<","<<_v.Z()<<") "
	   <<"n("<<getNormal().X()<<","<<getNormal().Y()<<","<<getNormal().Z()<<") "
		   <<std::endl;
    
}


bool operator== (const DetPlane& lhs, const DetPlane& rhs){
  return lhs._o==rhs._o && lhs._u==rhs._u && lhs._v == rhs._v;

}

bool operator!= (const DetPlane& lhs, const DetPlane& rhs){
  return !(lhs==rhs);
}


void DetPlane::getGraphics(double mesh, double length, TPolyMarker3D **pl,TPolyLine3D** plLine, TPolyLine3D **u, TPolyLine3D **v, TPolyLine3D**n){
  *pl = new TPolyMarker3D(21*21,24);
  (*pl)->SetMarkerSize(0.1);
  (*pl)->SetMarkerColor(kBlue);
  int counter(0);
  int nI=10;
  int nJ=10;
  int linePlCounter=0;
  *plLine = new TPolyLine3D(5);

  {
	TVector3 linevec;
	int i,j;
	i=-1*nI;j=-1*nJ;
	linevec=(_o+(mesh*i)*_u+(mesh*j)*_v);
	(*plLine)->SetPoint(0,linevec.X(),linevec.Y(),linevec.Z());
	i=-1*nI;j=1*nJ;
	linevec=(_o+(mesh*i)*_u+(mesh*j)*_v);
	(*plLine)->SetPoint(0,linevec.X(),linevec.Y(),linevec.Z());
	i=1*nI;j=-1*nJ;
	linevec=(_o+(mesh*i)*_u+(mesh*j)*_v);
	(*plLine)->SetPoint(2,linevec.X(),linevec.Y(),linevec.Z());
	i=1*nI;j=1*nJ;
	linevec=(_o+(mesh*i)*_u+(mesh*j)*_v);
	(*plLine)->SetPoint(1,linevec.X(),linevec.Y(),linevec.Z());
	i=-1*nI;j=-1*nJ;
	linevec=(_o+(mesh*i)*_u+(mesh*j)*_v);
	(*plLine)->SetPoint(4,linevec.X(),linevec.Y(),linevec.Z());

  }
  for (int i=-1*nI;i<=nI;++i){
	for (int j=-1*nJ;j<=nJ;++j){
	  TVector3 vec(_o+(mesh*i)*_u+(mesh*j)*_v);
	  int id=(i+10)*21+j+10;
	  (*pl)->SetPoint(id,vec.X(),vec.Y(),vec.Z());
	}
  }


  *u = new TPolyLine3D(2);
  (*u)->SetPoint(0,_o.X(),_o.Y(),_o.Z());
  (*u)->SetPoint(1,_o.X()+length*_u.X(),_o.Y()+length*_u.Y(),_o.Z()+length*_u.Z());
  (*u)->SetLineWidth(2);
  (*u)->SetLineColor(kGreen);


  *v = new TPolyLine3D(2);
  (*v)->SetPoint(0,_o.X(),_o.Y(),_o.Z());
  (*v)->SetPoint(1,_o.X()+length*_v.X(),_o.Y()+length*_v.Y(),_o.Z()+length*_v.Z());
  (*v)->SetLineWidth(2);
  (*v)->SetLineColor(kRed);

  if(n!=NULL){
    *n = new TPolyLine3D(2);
    TVector3 _n=getNormal();
    (*n)->SetPoint(0,_o.X(),_o.Y(),_o.Z());
    (*n)->SetPoint(1,_o.X()+length*_n.X(),_o.Y()+length*_n.Y(),_o.Z()+length*_n.Z());
    (*n)->SetLineWidth(2);
    (*n)->SetLineColor(kBlue);
  }
}

double DetPlane::distance(TVector3& v) const{
  double s = (v - _o)*_u;
  double t = (v - _o)*_v;
  TVector3 distanceVector = v - _o - (s*_u) - (t*_v); 
  return distanceVector.Mag();
}
