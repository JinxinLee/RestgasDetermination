
#include <iostream>
#include "PndVtxPoca.h"
#include "PndAnalysisCalcTools.h"
#include "RhoCandidate.h"
#include "RhoCandList.h"

using namespace std;

ClassImp(PndVtxPoca)

TBuffer& operator>>(TBuffer& buf, PndVtxPoca *&obj)
{
  obj = (PndVtxPoca*) buf.ReadObject(PndVtxPoca::Class());
  return buf;
}

//Include only those constraint which need vertex Info....
PndVtxPoca::PndVtxPoca()
{
}

PndVtxPoca::~PndVtxPoca()
{
}

Double_t PndVtxPoca::GetPocaVtx(TVector3& vertex, RhoCandidate* composite)
{
	RhoCandList cands;

	for (int i=0;i<composite->NDaughters();++i) cands.Put(composite->Daughter(i));

	return GetPocaVtx(vertex, cands); 
}

Double_t PndVtxPoca::GetPocaVtx(TVector3& vertex, RhoCandList  &cands)
{
	vertex.SetXYZ(0.,0.,0.);
	if ( cands.GetLength() <  2 ) { return -99999.; }
	if ( cands.GetLength() == 2 ) { return GetPoca(vertex, cands[0], cands[1]); }

	std::vector<Double_t> distances;
	std::vector<TVector3> results;
	// loop over daughters, take the mean value of all "best" positions
	// TODO do this smarter by using already found vertices ?
	TVector3 theVertex(0.,0.,0.);
	Double_t actualDoca=0.;
	
	for(Int_t daug1 =0; daug1<cands.GetLength(); daug1++) 
	{
		RhoCandidate* a=cands[daug1];
		
		for(Int_t daug2=daug1+1; daug2<cands.GetLength(); daug2++) 
		{
			RhoCandidate* b=cands[daug2];
			
			actualDoca = GetPoca(theVertex,a,b);
			if(actualDoca < 0) 
			{
				printf("PndVtxPoca - Error with getting a POCA. \"Distance\" is %g. SKIPPING Candidate pair now!",actualDoca);
				continue;
			}
			distances.push_back(actualDoca);
			results.push_back(theVertex);
		}//daug2
	}//daug1
	
	// Averaging vertex results from each track pair, weighted with 1/distance
	std::vector<Double_t>::iterator iterDoca;
	std::vector<TVector3>::iterator iterVtx;
	Double_t docaweight=0,sumdocaweigts=0;
	TVector3 vertexK;
	for(iterVtx=results.begin(), iterDoca=distances.begin(); iterVtx!=results.end()&&iterDoca!=distances.end(); ++iterVtx,++iterDoca) 
	{
		docaweight=1/(*iterDoca);
		//docaweight *= docaweight;
		vertexK=*iterVtx;
		if (docaweight == 0) { docaweight = 1; } // right so?
		vertexK *= docaweight;
		vertex+=vertexK;
		sumdocaweigts+=docaweight;
	}
	if (sumdocaweigts == 0) { sumdocaweigts=1; }
	vertex*=1./sumdocaweigts;
	//sumdocaweigts = sqrt(sumdocaweigts);
	return cands.GetLength()/sumdocaweigts;
		
}

Double_t PndVtxPoca::GetPoca(TVector3& vertex,RhoCandidate* a, RhoCandidate* b)
{
  // Decide which POCA algorithm to use
  
  if      (fabs(a->Charge())>1e-6 && fabs(b->Charge())>1e-6) return GetPocaTwoCharged(vertex, a, b);
  else if (fabs(a->Charge())<1e-6 && fabs(b->Charge())<1e-6) return GetPocaTwoNeutral(vertex, a, b);
  else if (fabs(a->Charge())<1e-6 || fabs(b->Charge())<1e-6) return GetPocaChargedToNeutral(vertex, a, b);
  else return -99999.;
}

Double_t PndVtxPoca::GetPocaTwoCharged(TVector3& vertex,RhoCandidate* a, RhoCandidate* b)
{
  // Calculate an approximate POCA for two helices.
  // First in 2D (x-y projection), then select best solution by minimum z distance
  
  vertex.SetXYZ(0.,0.,0.);
  Double_t bField = 0.1*RhoCalculationTools::GetBz(vertex); // T, assume field in z only
  Double_t bc = 0.0029979246*bField;
  TVector3 dB(0,0,1.0);
  TVector3 position1 = a->GetPosition();
  // Momentum vectors
  TVector3 ap3 = a->P3();
  Double_t pPerp1 = ap3.Perp();
  TVector3 d1 = ap3;
  d1.SetZ(0);
  d1*=1.0/pPerp1;

  // Radius and center
  Double_t rho1 = pPerp1/bc; // Radius in cm
  TVector3 r1=d1.Cross(dB);
  r1 *= -a->Charge()*rho1;
  TVector3 center1 = position1 - r1;
  center1.SetZ(0);

  TVector3 position2 = b->GetPosition();

  // Momentum vectors
  TVector3 bp3 = b->P3();
  Double_t pPerp2 = bp3.Perp();
  TVector3 d2 = bp3;
  d2.SetZ(0);
  d2*=1.0/pPerp2;

  // Radius and center
  Double_t rho2 =  pPerp2/bc; // Radius in cm
  TVector3 r2=d2.Cross(dB);
  r2 *= -b->Charge()*rho2;
  TVector3 center2 = position2 - r2;
  center2.SetZ(0);

  // distance and angle of the axis between the two centers
  TVector3 ab = center2 - center1;
  Double_t dab = ab.Perp();
  Double_t cosTheAB = ab.X()/dab;
  Double_t sinTheAB = ab.Y()/dab;

  Double_t darr = dab;
  darr -= rho1;
  darr -= rho2;

  // both circles do not intersect (only one solution)
  Int_t nSolMax=1;
  Double_t x=0;
  Double_t y=0;
  if (darr < 0) {
    // sum of radii is smaller than the two centers distance, circles intersect at two points
    nSolMax=2;
    // x value of intersect at reduced system
    x = 0.5*dab + ( rho1*rho1 - rho2*rho2 )/(2*dab);
    // y*y value of intersect at reduced system for helix A
    Double_t y2 = (rho1+x)*(rho1-x);
    if (y2 > 0) { y = sqrt(y2); }
  } else {
    // no intersecting circles, take the mid point between both circles
    x = 0.5*(dab + rho1 - rho2);
  }
  // now we compute the solution(s)
  TVector3 newapos[2];
  TVector3 newbpos[2];
  Int_t best=0;
  Double_t actualDoca=1.E8;
  for (Int_t ns=0; ns<nSolMax; ns++) {     // loop on the solutions
    // radius vector of intersection point
    Double_t sign = ns ? 1.0 : -1.0;
    TVector3 rs1( cosTheAB*x - sinTheAB*y * sign, sinTheAB*x + cosTheAB*y * sign, 0);
    TVector3 rs2( rs1-ab );

    // are we moving forward or backward?
    Double_t adir=(rs1-r1).Dot(ap3)>0 ? 1.0 : -1.0;
    Double_t aangle=adir * r1.Angle(rs1);
    // intersection point
    Double_t newaz=position1.Z() + rho1*aangle/pPerp1 * ap3.Z();
    newapos[ns].SetX( center1.X() + rs1.X() );
    newapos[ns].SetY( center1.Y() + rs1.Y() );
    newapos[ns].SetZ( newaz );

    // same for b
    Double_t bdir=(rs2-r2).Dot(bp3)>0 ? 1.0 : -1.0;
    Double_t bangle=bdir * r2.Angle(rs2);
    Double_t newbz=position2.Z() + rho2*bangle/pPerp2 * bp3.Z();
    newbpos[ns].SetX( center2.X() + rs2.X());   // ==newapos[ns].X()
    newbpos[ns].SetY( center2.Y() + rs2.Y());   // ==newapos[ns].Y()
    newbpos[ns].SetZ( newbz );

    Double_t delta = (newapos[ns]-newbpos[ns]).Mag();

    // take the solution of minimal deltaZ
    if ( delta < actualDoca ) {
      best=ns;
      actualDoca  = delta;
    }
  }
  vertex=0.5*(newapos[best]+newbpos[best]);
  return actualDoca;
}

Double_t PndVtxPoca::GetPocaChargedToNeutral(TVector3& vertex,RhoCandidate* a, RhoCandidate* b)
{
  // POCA Approxiamtion for a helix with a line
  // Fist mtching in x-y projection, then choose one solution in z
  // by the smallest distance.
  
  RhoCandidate* charged;
  RhoCandidate* neutral;
  if (fabs(a->Charge())<1e-6 && fabs(b->Charge())>1e-6){
    charged=b; 
    neutral=a;
  } else if (fabs(a->Charge())>1e-6 && fabs(b->Charge())<1e-6){
    charged=a; 
    neutral=b;
  } else return -99999.;

  vertex.SetXYZ(0.,0.,0.);
  Double_t bField = 0.1*RhoCalculationTools::GetBz(vertex); // T, assume field in z only
  Double_t bc = 0.0029979246*bField;
  TVector3 dB(0,0,1.0);
  TVector3 position1 = charged->GetPosition();
  // Momentum vectors
  TVector3 ap3 = charged->P3();
  Double_t pPerp1 = ap3.Perp();
  TVector3 d1 = ap3;
  d1.SetZ(0);
  d1*=1.0/pPerp1;

  // Radius and center
  Double_t rho1 = pPerp1/bc; // Radius in cm
  TVector3 r1=d1.Cross(dB);
  r1 *= -charged->Charge()*rho1;
  TVector3 center1 = position1 - r1;
  center1.SetZ(0);

  TVector3 position2 = neutral->GetPosition();

  // Momentum vectors
  TVector3 bp3 = neutral->P3();
  Double_t pPerp2 = bp3.Perp();
  TVector3 d2 = bp3;
  d2.SetZ(0);
  d2*=1.0/pPerp2;                                                          	//direction of neutral
  TVector3 g_p = position2;
  g_p.SetZ(0);
  TVector3 c_to_g = (center1.Dot(d2) - g_p.Dot(d2))*d2 + g_p - center1;		//vector pointing from center1 to the neutral
  
  Int_t nSolMax=1;
  TVector3 newapos[2];
  TVector3 newbpos[2];
  if(c_to_g.Mag()<rho1)
  {
    newapos[0].SetZ(0.);
    newapos[1].SetZ(0.);
    nSolMax=2;

    newapos[0] = center1 + TMath::Sqrt(rho1*rho1 - (c_to_g.Dot(c_to_g)))*d2 + c_to_g;
    newapos[1] = center1 - TMath::Sqrt(rho1*rho1 - (c_to_g.Dot(c_to_g)))*d2 + c_to_g;
    newbpos[0] = center1 + TMath::Sqrt(rho1*rho1 - (c_to_g.Dot(c_to_g)))*d2 + c_to_g;
    newbpos[1] = center1 - TMath::Sqrt(rho1*rho1 - (c_to_g.Dot(c_to_g)))*d2 + c_to_g;

    TVector3 rs1 = newapos[0] - center1;
    // are we moving forward or backward?
    Double_t adir=(rs1-r1).Dot(ap3)>0 ? 1.0 : -1.0;
    Double_t aangle=adir * r1.Angle(rs1);
    newapos[0].SetZ(position1.Z() + rho1*aangle/pPerp1 * ap3.Z());
    
    rs1 = newapos[1] - center1;
    // are we moving forward or backward?
    adir=(rs1-r1).Dot(ap3)>0 ? 1.0 : -1.0;
    aangle=adir * r1.Angle(rs1);
    newapos[1].SetZ(position1.Z() + rho1*aangle/pPerp1 * ap3.Z());

    // are we moving forward or backward?
    adir = bp3.Dot(newbpos[0] - position2)>0 ? 1.0 : -1.0;    
    TVector3 length = newbpos[0] - g_p;
    newbpos[0].SetZ(position2.Z() + length.Mag() * adir * bp3.Z()/pPerp2);
    adir = bp3.Dot(newbpos[1] - position2)>0 ? 1.0 : -1.0;    
    length = newbpos[1] - g_p;
    newbpos[1].SetZ(position2.Z() + length.Mag() * adir * bp3.Z()/pPerp2);
  }
  else
  {
    nSolMax=1;
    newapos[0] = 0.5*(c_to_g*(rho1/c_to_g.Mag()) + center1 + g_p); 
    newbpos[0] = 0.5*(c_to_g*(rho1/c_to_g.Mag()) + center1 + g_p); 
    
    TVector3 rs1 = newapos[0] - center1;
    // are we moving forward or backward?
    Double_t adir=(rs1-r1).Dot(ap3)>0 ? 1.0 : -1.0;
    Double_t aangle=adir * r1.Angle(rs1);
    newapos[0].SetZ(position1.Z() + rho1*aangle/pPerp1 * ap3.Z());
    
    // are we moving forward or backward?
    adir = bp3.Dot(newbpos[0] - position2)>0 ? 1.0 : -1.0;    
    TVector3 length = newbpos[0] - g_p;
    newbpos[0].SetZ(position2.Z() + length.Mag() * adir * bp3.Z()/pPerp2);
  }


  Int_t best=0;
  Double_t actualDoca=1.E8;
  for(int ns=0; ns<nSolMax; ns++){
    Double_t delta = (newapos[ns]-newbpos[ns]).Mag();
    // take the solution of minimal deltaZ
    if ( delta < actualDoca ) {
      best=ns;
      actualDoca  = delta;
    }
  }
  
  vertex=0.5*(newapos[best]+newbpos[best]);
  return actualDoca;
}

Double_t PndVtxPoca::GetPocaTwoNeutral(TVector3& vertex,RhoCandidate* canda, RhoCandidate* candb)
 {
   // This is the exact(!) skewed line POCA
   //
   
   vertex.SetXYZ(0.,0.,0.);
   
   TVector3 av = canda->GetPosition();
   TVector3 ap = canda->P3();
   
   TVector3 bv = candb->GetPosition();
   TVector3 bp = candb->P3();
   
   TVector3   u = ap.Unit();
   TVector3   v = bp.Unit();
   TVector3   w = av - bv;
   double     a = u.Mag(); //dot(u,u);         // always >= 0
   double     b = u.Dot(v); //dot(u,v);
   double     c = v.Mag(); //dot(v,v);         // always >= 0
   double     d = u.Dot(w); //dot(u,w);
   double     e = v.Dot(w); //dot(v,w);
   double     D = a*c - b*b;        // always >= 0
   double     sc, tc;
   
   // compute the line parameters of the two closest points
   if (D < 1e-9) {          // the lines are almost parallel
     sc = 0.0;
     tc = (b>c ? d/b : e/c);    // use the largest denominator
   }
   else {
       sc = (b*e - c*d) / D; //determination of s in g: x1 = av + s*ap
       tc = (a*e - b*d) / D; //determination of t in h: x2 = bv + t*bp
       	  	  	  	  	  	//using (x1-x2)*u = 0 and (x1-x2)*v = 0
   }
   
   // get the difference of the two closest points
   //TVector3   dP = w + (sc * u) - (tc * v);  // =  L1(sc) - L2(tc)
   TVector3   pocaa = av + sc*u;
   TVector3   pocab = bv + tc*v;
   vertex = 0.5*(pocaa+pocab);
   TVector3   diff = pocaa-pocab;
   
   return diff.Mag();   // return the closest distance
   
 }

