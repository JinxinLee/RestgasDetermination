#include "AliceHelixTrackRep.h"
#include "AbsRecoHit.h"

#include<cmath>

AliceHelixTrackRep::AliceHelixTrackRep() : AbsTrackRep(5) {
}

AliceHelixTrackRep::AliceHelixTrackRep(double initialR,const TMatrixT<double>& initialState, const TMatrixT<double>& initialCov) : AbsTrackRep(5) {
  state = initialState;
  cov = initialCov;

  s = initialR;
  startS = initialR;
  startState = initialState;
  startCov = initialCov;
}


AliceHelixTrackRep::AliceHelixTrackRep(double initialR, 
											   double phi,double z,
											   double lambda,double C,
											   double D0,double dphi,
											   double dz,double dlambda,
											   double dC,
											   double dD0) : AbsTrackRep(5) {
  state[0][0]=phi;
  state[1][0]=z;
  state[2][0]=lambda;
  state[3][0]=C;
  state[4][0]=D0;
  cov[0][0]=dphi*dphi;
  cov[1][1]=dz*dz;
  cov[2][2]=dlambda*dlambda;
  cov[3][3]=dC*dC;
  cov[4][4]=dD0*dD0;

  s=initialR;
  startS = initialR;
  startState = state;
  startCov = cov;


}

void 
AliceHelixTrackRep::init(const TVector3& startpoint, 
						 const TVector3& startmomentum, 
						 const TVector3& locBField,
						 const int charge)
{

  TMatrixT<double> MCtruth(5,1);
  double MCtruth_s;
  double B=locBField.Z();

  double R = startmomentum.Perp()/(0.00299792*B);// in cm!
  if(startpoint.Perp()<1.e-9) {
	MCtruth[0][0] = startmomentum.Phi();
	MCtruth[1][0] = startpoint.Z();
	MCtruth[2][0] = 0.5*TMath::Pi()-startmomentum.Theta();
	MCtruth[3][0] = -1.*charge/(2.*R);
	MCtruth[4][0] = 0.;
	MCtruth_s = 1.e-9;
  }
  else {
	MCtruth[0][0] = startpoint.Phi();
	MCtruth[1][0] = startpoint.Z();
	MCtruth[2][0] = 0.5*TMath::Pi()-startmomentum.Theta();
	MCtruth[3][0] = -1.*charge/(2.*R);
	double x=startpoint.X();
	double y=startpoint.Y();;
	double px=startmomentum.X();
	double py=startmomentum.Y();
	double r,u,v;
	if ( TMath::Abs(py/px)>1.e-5 ) {
	  u = 1.;
	  v = -px/py;
	}
	else {
	  v = 1.;
	  u = -py/px;
	}
	r = TMath::Sqrt(x*x+y*y);
	double cos_alpha = (x*u+y*v) / ( r + TMath::Sqrt(u*u+v*v) );
	//maybe + not -
	double D0 = R - TMath::Sqrt( r*r+R*R-2.*r*R*cos_alpha  );
	MCtruth[4][0] = D0;
	MCtruth_s = startpoint.Perp();
  }
  // calculate curvature

  /*
  //std::cout<<"BField="<<B<<std::endl;
  double C=0.0015*B/startmomentum.Perp(); // C=1/2R
  state[0][0]=startpoint.Phi()*1.3;
  state[1][0]=startpoint.Z()*3;
  state[2][0]=0.5*TMath::Pi()-startmomentum.Theta()+TMath::Pi()*0.1;
  state[3][0]=C*3.;
  state[4][0]=1E-6;
  */


  params.setMCtruth(MCtruth);
  params.setMCtruth_s(MCtruth_s);

  state = MCtruth;
  s=MCtruth_s;//startpoint.Perp();
  startS = s;
  startState = MCtruth;//state;

  for(int i=0;i<5;++i)cov[i][i]=state[i][0]*state[i][0]*0.25; // 20% error
  cov[4][4]=1e-2;
  /*  cov[0][0]=pow(5./180.*TMath::Pi(),2.);
  cov[1][1]=pow(1.,2.);
  cov[2][2]=pow(5./180.*TMath::Pi(),2.);
  cov[3][3]=pow(1./(2.*10.),2.);
  cov[4][4]=pow(0.01,2.);
  cov.Print();
  */
  startCov = cov;

  //  std::cout<<"******* Init: State & Cov ***************"<<std::endl;
  //  state.Print();
  //  cov.Print();
  modStartValues();
}

void AliceHelixTrackRep::modStartValues() {
  state[0][0] = state[0][0]*1.3;
  state[1][0] = state[1][0]*1.3;
  state[2][0] = state[2][0]*1.3;
  state[3][0] = state[3][0]*1.3;
  state[4][0] = state[4][0]*1.3;
  startState = state;
  params.setStartState(state);
  params.setStartState_s(params.getMCtruth_s());  
}


void AliceHelixTrackRep::predict(AbsRecoHit* hit, 
								 TMatrixT<double>& statePred,
								 TMatrixT<double>& covPred, 
								 TMatrixT<double>& jacobian, 
								 double& sPred){
  sPred = hit->getS();
  extrapolate(sPred,statePred,covPred,jacobian);
}



AliceHelixTrackRep::~AliceHelixTrackRep() {
}

void AliceHelixTrackRep::extrapolate(double sExtrapolateTo, TMatrixT<double>& statePred){
  statePred = state;
  statePred[0][0] = extrapolatePhi(state,sExtrapolateTo,s);
  statePred[1][0] = extrapolateZ(state,sExtrapolateTo,s);
}

void AliceHelixTrackRep::extrapolate(double sExtrapolateFrom, 
									 double sExtrapolateTo, 
									 const TMatrixT<double>& stateFrom, 
									 TMatrixT<double>& stateResult){
  stateResult = stateFrom;
  stateResult[0][0] = extrapolatePhi(stateFrom,sExtrapolateTo,sExtrapolateFrom);
  stateResult[1][0] = extrapolateZ(stateFrom,sExtrapolateTo,sExtrapolateFrom);
}


void AliceHelixTrackRep::extrapolate(double sExtrapolateTo, 
									 TMatrixT<double>& statePred,
									 TMatrixT<double>& covPred,
									 TMatrixT<double>& jacobian){

  statePred = state;
  statePred[0][0] = extrapolatePhi(state,sExtrapolateTo,s);
  statePred[1][0] = extrapolateZ(state,sExtrapolateTo,s);

  jacobian.ResizeTo(5,5);
  makeJacobianAnalytical(sExtrapolateTo,s,state,jacobian);

  TMatrixT<double> covPredTemp(cov,TMatrixT<double>::kMultTranspose,jacobian);
  covPred = jacobian * covPredTemp;

}


double AliceHelixTrackRep::extrapolatePhi(const TMatrixT<double>& S, double rNew, double rOld) const {
  double phi   = S[0][0];
  double z     = S[1][0];
  double lambda= S[2][0];
  double C     = S[3][0];
  double D0    = S[4][0];
  double gamma = phi - asin( (C*rOld + D0/rOld * (1+C*D0)) / (1. + 2*C*D0 ) );
  return (gamma + asin( (C*rNew + D0/rNew * (1+C*D0)) / (1. + 2*C*D0 ) ));
}

double AliceHelixTrackRep::extrapolateZ(const TMatrixT<double>& S, double rNew, double rOld) const {
  double phi   = S[0][0];
  double z     = S[1][0];
  double lambda= S[2][0];
  double C     = S[3][0];
  double D0    = S[4][0];
  double z_0  = z   - tan(lambda)/C * asin( C * TMath::Sqrt( (rOld*rOld-D0*D0)/(1.+2*C*D0) ) );
  return (z_0   + tan(lambda)/C * asin( C * TMath::Sqrt( (rNew*rNew-D0*D0)/(1.+2*C*D0) ) ));
}
         
double AliceHelixTrackRep::functionForNumerical(int i, int j, double x, const TMatrixT<double>& oldState, double rNew, double rOld) const {
  TMatrixT<double> S(5,1);
  S = oldState;

  S[j][0] = x;
  switch(i) {
  case 0:
    return extrapolatePhi(S,rNew, rOld);
  case 1:
    return extrapolateZ(S,rNew, rOld);
  default:
    return S[i][0];
  }
}


void AliceHelixTrackRep::makeJacobianNumerical(double newR,
						       double oldR,
						       const TMatrixT<double>& S,
						       TMatrixT<double>& theJacobian,
						       double h_relative) const {
  double h,x;
  for(int i=0;i<5;i++) {
    for(int j=0;j<5;j++) {
      x = state[j][0];
      h = TMath::Abs(h_relative * x);
	  if(h<1.e-12) h=1.e-6;
      //      h = TMath::Abs(h_relative * functionForNumerical( i,j, x  ,oldState,newR,oldR  ));
      theJacobian[i][j] = (8.0 * (functionForNumerical(i,j,x+h/2.,state,newR,oldR)-
                                  functionForNumerical(i,j,x-h/2.0,state,newR,oldR))
                           - (functionForNumerical(i,j,x+h,state,newR,oldR)-
                              functionForNumerical(i,j,x-h,state,newR,oldR)) ) / (6.0*h);
    }
  }
}



void AliceHelixTrackRep::makeJacobianAnalytical(double rNew,
							double rOld,
							const TMatrixT<double>& S,
							TMatrixT<double>& cg)const {
  double phiOld   = S[0][0];
  double zOld     = S[1][0];
  double lambdaOld= S[2][0];
  double COld     = S[3][0];
  double DOld     = S[4][0];


  cg[0][0] = 1;
  cg[0][3] = ((rNew + pow(DOld, 0.2e1) / rNew) / (0.2e1 * COld * DOld + 0.1e1) - 0.2e1 * (COld * rNew + DOld / rNew * (COld * DOld + 0.1e1)) * pow(0.2e1 * COld
* DOld + 0.1e1, -0.2e1) * DOld) * pow(0.1e1 - pow(COld * rNew + DOld / rNew * (COld * DOld + 0.1e1), 0.2e1) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1), -0.1e1 / 0.2e1) - ((rOld + pow(DOld, 0.2e1) / rOld) / (0.2e1 * COld * DOld + 0.1e1) - 0.2e1 * (COld * rOld + DOld / rOld * (COld * DOld + 0.1e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * DOld) * pow(0.1e1 - pow(COld * rOld + DOld / rOld * (COld * DOld + 0.1e1), 0.2e1) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1), -0.1e1 / 0.2e1);
  cg[0][4] = ((0.1e1 / rNew * (COld * DOld + 0.1e1) + DOld / rNew * COld) / ( 0.2e1 * COld * DOld + 0.1e1) - 0.2e1 * (COld * rNew + DOld / rNew * (COld * DOld + 0.1e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * COld) * pow(0.1e1 - pow(COld * rNew + DOld / rNew * (COld * DOld + 0.1e1), 0.2e1) * pow( 0.2e1 * COld *
DOld + 0.1e1, -0.2e1), -0.1e1 / 0.2e1) - ((0.1e1 / rOld * (COld * DOld + 0.1e1) + DOld / rOld * COld) / (0.2e1 * COld * DOld + 0.1e1) - 0.2e1 * (COld * rOld + DOld / rOld * (COld * DOld + 0.1e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * COld) * pow(0.1e1 - pow(COld * rOld + DOld / rOld * (COld * DOld + 0.1e1),
0.2e1) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1), -0.1e1 / 0.2e1);
  cg[1][1] = 1;
  cg[1][2] = (0.1e1 + pow(tan(lambdaOld), 0.2e1)) / COld * asin(COld * TMath::Sqrt((pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1))) - (0.1e1 + pow(tan(lambdaOld), 0.2e1)) / COld * asin(COld * TMath::Sqrt((pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1)));
  cg[1][3] = -tan(lambdaOld) * pow(COld, -0.2e1) * asin(COld * TMath::Sqrt((pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1))) + tan(lambdaOld) / COld * (TMath::Sqrt((pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / ( 0.2e1 * COld * DOld + 0.1e1)) - COld * pow((pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld
+ 0.1e1), -0.1e1 / 0.2e1) * (pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * DOld) * pow( 0.1e1 - pow(COld, 0.2e1) * (pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1) + tan(lambdaOld) * pow(COld, -0.2e1) * asin(COld * TMath::Sqrt((pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1))) - tan(lambdaOld) / COld * (TMath::Sqrt((pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1)) - COld * pow((pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1) * (pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * DOld) * pow(0.1e1 - pow(COld, 0.2e1) * (pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1);
  cg[1][4] = tan(lambdaOld) * pow((pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1) * (-0.2e1 * DOld / (0.2e1 * COld * DOld + 0.1e1) - 0.2e1 * (pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * COld) * pow( 0.1e1 - pow(COld, 0.2e1) * (pow(rNew, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1) / 0.2e1 - tan(lambdaOld) * pow((pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1) * (-0.2e1 * DOld / (0.2e1 * COld * DOld + 0.1e1) - 0.2e1 * (pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) * pow(0.2e1 * COld * DOld + 0.1e1, -0.2e1) * COld) * pow(0.1e1 - pow(COld, 0.2e1) * (pow(rOld, 0.2e1) - pow(DOld, 0.2e1)) / (0.2e1 * COld * DOld + 0.1e1), -0.1e1 / 0.2e1) / 0.2e1;
  cg[2][2] = 1;
  cg[3][3] = 1;
  cg[4][4] = 1;
}


ClassImp(AliceHelixTrackRep)
