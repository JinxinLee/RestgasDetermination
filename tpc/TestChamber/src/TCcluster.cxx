#include"TCcluster.h"
#include"Pedestals.h"
#include<cmath>

#include"PndTpcDigiMapper.h"

#include"TMath.h"
#include"TMatrixD.h"

TCcluster cogTCcluster(std::vector<TCcluster>& _raw){
  TCcluster rclust;
  int rdetId = -1;
  TVector3 rpos(0.,0.,0.);
  TVector3 rerr(0.,0.,0.);
  double ramp=0.;
  unsigned int firstId=_raw.at(0).getId();
  unsigned int nraw=_raw.size();
  for(unsigned int i=0;i<nraw;++i){
    assert(firstId == _raw.at(i).getId());
    double a=_raw.at(i).getAmp();
    TVector3 thispos=_raw.at(i).posUVW();
    TVector3 thissig=_raw.at(i).getErr();
    thissig.SetX(pow(thissig.X(),2.));
    thissig.SetY(pow(thissig.Y(),2.));
    thissig.SetZ(pow(thissig.Z(),2.));
    rerr+=a*a*thissig;
    rpos+=a*thispos;
    ramp+=a;
    rclust.addRaw(_raw.at(i));
  }
  rpos*=1./ramp;
  rdetId=firstId;
  
  for(unsigned int i=0;i<nraw;++i){
    TVector3 thispos=_raw.at(i).posUVW();
    double sigmaAi = _raw.at(i).getPedestalRMS();
    TVector3 temp(pow(thispos.X()-rpos.X(),2.),
		  pow(thispos.Y()-rpos.Y(),2.),
		  pow(thispos.Z()-rpos.Z(),2.));
    temp *= sigmaAi*sigmaAi;
    rerr += temp;
  }


  rerr.SetX(sqrt(rerr.X())/ramp);
  rerr.SetY(sqrt(rerr.Y())/ramp);
  rerr.SetZ(sqrt(rerr.Z())/ramp);
  
  rclust.posUVW(rpos);
  rclust.setErr(rerr);
  rclust.setAmp(ramp);
  rclust.setId(rdetId);
  return rclust;

}


TCcluster::TCcluster(TVector3 p,TVector3 e,double a,int id):pos(p),err(e),amp(a),detId(id),fit(false){}
TCcluster::TCcluster(const PndTpcCluster& _c,int id) : detId(id),fit(false){
  pos=_c.pos();
  err=_c.sig();
  for(unsigned int i=0;i<_c.nDigi();++i){
    PndTpcDigi _d = _c.getDigi(i);
    TCcluster cRaw(_d,id);
    raw.push_back(cRaw);
  }
  amp=_c.amp();
  pedestalRMS=-1.E10;
}
TCcluster::TCcluster():fit(false){
  TVector3 def(-1.E10,-1.E10,-1.E10);
  pos=def;
  err=def;
  detId=-1;
  amp=-1.E10;
  pedestalRMS=-1.E10;
}

TCcluster::TCcluster(const PndTpcDigi& _d,int id) : detId(id),fit(false){//for raw clusters
  PndTpcDigiMapper::getInstance()->map(&_d,pos);
  amp=_d.amp();

  McId dummyID(1,1);
  McIdCollection dummyColl;
  dummyColl.AddID(dummyID);

  static float dx(0.62);
  static float dy(0.1);

  //this block is to define the z jitter
  TVector3 zDiff1,zDiff2;
  PndTpcDigi zDiffDigi1(1,1,1,dummyColl),zDiffDigi2(1,2,1,dummyColl);
  PndTpcDigiMapper::getInstance()->map(&zDiffDigi1,zDiff1);
  PndTpcDigiMapper::getInstance()->map(&zDiffDigi2,zDiff2);
  double zDiff = zDiff2.z() - zDiff1.z();
  //end of z jitter
  
  double Dl = PndTpcDigiMapper::getInstance()->getGas()->Dl();
  double Dt = PndTpcDigiMapper::getInstance()->getGas()->Dt();
  
  double diffSigmaL = Dl * sqrt(pos.z());
  double diffSigmaT = Dt * sqrt(pos.z());
  err.SetX(TMath::Sqrt(dx*dx/12. + diffSigmaT*diffSigmaT));
  err.SetY(TMath::Sqrt(dy*dy/12. + diffSigmaT*diffSigmaT));
  err.SetZ(TMath::Sqrt(zDiff*zDiff/12. + diffSigmaL*diffSigmaL));

  double dummy;
  Pedestals::getPedestal(_d.padId(),dummy,pedestalRMS);
}



void TCcluster::print(){
  std::cout << "========== TCcluster::print()" << std::endl;
  printf("pos:   (%10.10f,%10.10f,%10.10f)\n",pos.X(),pos.Y(),pos.Z());
  printf("err:   (%10.10f,%10.10f,%10.10f)\n",err.X(),err.Y(),err.Z());
  printf("resid: (%10.10f,%10.10f,%10.10f)\n",res.X(),res.Y(),res.Z());
  std::cout << "========== ++++++++++++++++++" << std::endl;
}

double TCcluster::getChi2(double *par){
  TVector3 r = getResid(par);
  return pow(r.X()/err.X(),2.)+pow(r.Y()/err.Y(),2.)+pow(r.Z()/err.Z(),2.);
}

TVector3 TCcluster::getResid(double *par){
  setResid(par);
  return res;
}

int TCcluster::getNDF(){
  if(detId<0) {
    std::cerr << "TCcluster::getNDF(...) detId<0 not implemented ->abort" << std::endl;
    throw;
  }
  else if(detId<100) return 1;
  else if(detId<200) {
    std::cerr << "TCcluster::getNDF(...) Pixel detectors (id=100-199) currently not implemented ->abort" << std::endl;
    throw;
  }
  else if(detId<300) return 3;
  else {
    std::cerr << "TCcluster::getNDF(...) detId>=300 currently not implemented ->abort" << std::endl;
    throw;
  }  
}

void TCcluster::setResid(double *par){
  TVector3 p,d;
  convertPar(par,p,d);
  if(detId<0) {
	std::cerr << "TCcluster::getResid(...) detId<0 not implemented ->abort" << std::endl;
	throw;
  }
  else if(detId<100) res = residStrip(p,d);
  else if(detId<200) res = residPixel(p,d);
  else if(detId<300) res = residPoint(p,d);
  else {
	std::cerr << "TCcluster::getResid(...) detId>=300 currently not implemented ->abort" << std::endl;
	throw;
  }
}

TVector3 TCcluster::residPoint(const TVector3& p,const TVector3& d){//these track parameters are in detector coordinates
  /*
	residual vector R=pos-Q;
	Q is POCA
	R*d=0 in POCA
	Q=p+t*d
	R=pos-p-t*d
	pos*d-p*d=t*d^2
	t=1/d^2 * (pos*d-p*d)
  */
  double t = 1/pow(d.Mag(),2.) * (pos*d-p*d);
  TVector3 ret = pos-p-t*d;
  return ret;//points from track to pos
}

TVector3 TCcluster::residPixel(const TVector3& p,const TVector3& d){//these track parameters are in detector coordinates
  std::cerr << "TCcluster::residPixel not yet implemented ->abort" << std::endl;
  throw;
}

TVector3 TCcluster::residStrip(const TVector3& p,const TVector3& d){//these track parameters are in detector coordinates
  /*
	in strip detectors, the coordinate sysytem is such 
	that the detector plane is w=0
	The strips always run along the u coordinate.
	This means that the residual vector is just:
	(b_u-pos_u,0,0)
	The calculation of bu is based on:(bu,bv,0)=p+t*d
  */
  //double t = -1.*p.Z()/d.Z();
  //double b_u = p.X()+t*d.X();
  TVector3 ret((p.X()-(p.Z()/d.Z())*d.X()) - pos.X() ,0.,0.);
  return ret;
}

  unsigned int TCcluster::nPadX(){
   unsigned int nRows;

    raw.push_back( raw.at(0) );
      for(int iraw=1;iraw<raw.size();++iraw){
	if(fabs(raw.at(0).posUVW().X()-raw.at(iraw).posUVW().X())<1.E-5){//same U pad row
        nRows == nRows;
        raw.push_back(raw.at(iraw));
	}
	else{//next U pad row
	  nRows == nRows + 1;
	  raw.push_back(raw.at(iraw));
	}
      }
   return nRows;
  }

  unsigned int TCcluster::nPadY(){
   unsigned int nRows;

    raw.push_back( raw.at(0) );
      for(int iraw=1;iraw<raw.size();++iraw){
	if(fabs(raw.at(0).posUVW().Y()-raw.at(iraw).posUVW().Y())<1.E-5){//same U pad row
        nRows == nRows;
        raw.push_back(raw.at(iraw));
	}
	else{//next U pad row
	  nRows == nRows + 1;
	  raw.push_back(raw.at(iraw));
	}
      }
   return nRows;
  }
void TCcluster::convertPar(double *par,TVector3& p,TVector3& d){

  //extract point and direction vector from par
  TVector3 dir(par[0],par[2],1.);
  dir.SetMag(1.);
  TVector3 point(par[1],par[3],0.);

  //convert them to detector coordinates
  d = TCalign::getInstance()->notranslXYZtoUVW(detId,dir);
  p = TCalign::getInstance()->XYZtoUVW(detId,point);
}

ClassImp(TCcluster)
