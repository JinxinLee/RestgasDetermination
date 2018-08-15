#include "RhoEventShapes.h"
#include "RhoCandidate.h"

ClassImp(RhoEventShapes);


RhoEventShapes::RhoEventShapes(RhoCandList &l, TLorentzVector cms) : 
  fnChrg(0), fnNeut(0), fN(0), fpmaxlab(0.), fpmaxcms(0.), fptmax(0.),
  fptsumlab(0.),fneutetsumlab(0.),fneutesumlab(0.),fchrgptsumlab(0.),fchrgpsumlab(0.),
  fptsumcms(0.),fneutetsumcms(0.),fneutesumcms(0.),fchrgptsumcms(0.),fchrgpsumcms(0.),
  fsph(-1.), fapl(-1.), fpla(-1.), fFWready(false), fthr(-1.)
{
  int i;

  // initialize more complex things
  fThrVect.SetXYZ(0.,0.,0.);
  fBoost=cms.BoostVector();  
  for (i=0;i<=FWMAX;++i) fFWmom[i]=0.;
  
  
  double pmax=0., ptmax=0., pmaxcms=0.;
  
  fLabList.clear();
  fCmsList.clear();
  
  for (i=0;i<l.GetLength();++i)
  {
    RhoCandidate* cand = l[i];
    TLorentzVector lv(cand->P4());
    int chrg(cand->Charge());

    fN++;
    
    // cache multiplicities
    if (chrg==0) fnNeut++;
    else fnChrg++;
    
    // cache unboosted 4-vectors
    fLabList.push_back(lv);
    // cache charges
    fCharge.push_back(chrg);
    
    // sum momentum variables (lab)
    fptsumlab += lv.Pt();
    if (chrg==0)
    {
      fneutetsumlab += lv.Pt();  
      fneutesumlab  += lv.E();
    }
    else
    {
      fchrgptsumlab += lv.Pt();
      fchrgpsumlab  += lv.P();
    }
    
    // cache maximum momenta in lab
    if (lv.P()>pmax) pmax=lv.P();
    if (lv.Pt()>ptmax) ptmax=lv.Pt();
    
    // cache boosted vectors
    lv.Boost(-fBoost);
    fCmsList.push_back(lv);

    // sum momentum variables (cms)
    fptsumcms += lv.Pt();
    if (chrg==0)
    {
      fneutetsumcms += lv.Pt();  
      fneutesumcms  += lv.E();
    }
    else
    {
      fchrgptsumcms += lv.Pt();
      fchrgpsumcms  += lv.P();
    }
    
    // cache maximum momenta in cms
    if (lv.P()>pmaxcms) pmaxcms=lv.P();
  }
  
  fpmaxlab = pmax;
  fptmax   = ptmax;
  fpmaxcms = pmaxcms;
}

// ----------------------------

void RhoEventShapes::ComputeSphericity()
{
  if( fN==0 ) return;
  
  double stot=0, sxx=0, sxy=0, sxz=0, syy=0, syz=0, szz=0;  
  int i;
  
  for (i=0;i<fN;++i)
  {
    TVector3 v(fCmsList[i].Vect());
    
    sxx += v.X()*v.X(); sxy += v.X()*v.Y(); sxz += v.X()*v.Z();
    syy += v.Y()*v.Y(); syz += v.Y()*v.Z();
    szz += v.Z()*v.Z();
    
    stot += v.Mag2();
  }
  
  TMatrixD sm(3,3);
  sm(0,0) = sxx/stot; sm(0,1) = sxy/stot; sm(0,2) = sxz/stot; 
  sm(1,0) = sxy/stot; sm(1,1) = syy/stot; sm(1,2) = syz/stot; 
  sm(2,0) = sxz/stot; sm(2,1) = syz/stot; sm(2,2) = szz/stot; 
  
  TMatrixDEigen ei(sm);
  TMatrixD eiv=ei.GetEigenValues();
  
  fsph = 1.5 * (eiv(1,1) + eiv(2,2));
  fapl = 1.5 * eiv(2,2);
  fpla = eiv(1,1) - eiv(2,2);  
}

// ---------------------------------

double RhoEventShapes::Sphericity()
{
  if (fsph<0.) ComputeSphericity();
  
  return fsph;
}

// ---------------------------------

double RhoEventShapes::Planarity()
{
  if (fpla<0.) ComputeSphericity();
  
  return fpla;
}

// ---------------------------------

double RhoEventShapes::Aplanarity()
{
  if (fapl<0.) ComputeSphericity();
  
  return fapl;
}

// ---------------------------------------
// use iterative formula for thrust vector
//
//             sum eps( n(j)*p_i) * p_i
//   n(j+1) = --------------------------
//            |        ''             | 
//
// with eps(x)=-1 for x<0 and =+1 for x>0

double RhoEventShapes::Thrust()
{
  // did we already compute?
  if (fthr>-1.) return fthr; 
  
  TVector3 n0(0,0,0);
  if( fN==0 ) return -1.;
  
  int i,j;
  double pmax=0;
  
  //find starting vector as maximum momentum vector
  for (i=0;i<fN;++i)
  {
    if (fCmsList[i].Vect().Mag()>pmax)
    {
      n0=fCmsList[i].Vect();
      pmax=fCmsList[i].Vect().Mag();
    }
  }
  
  TVector3 nnew(0,0,0);

  // find thrust axis (10 iterations)
  for (i=0;i<10;++i)
  {
    for (j=0;j<fN;++j) 
      nnew += Eps(n0,fCmsList[j].Vect()) * fCmsList[j].Vect();
    
    n0=nnew.Unit();
  }
  
  fThrVect = n0;
  
  double thr=0, sum=0;
  for (i=0;i<fN;++i)
  {
    thr += fabs(fThrVect.Dot(fCmsList[i].Vect()));
    sum += fCmsList[i].Vect().Mag();
  }
  
  fthr = thr/sum;
  
  return fthr;
}

// ---------------------------------------

TVector3 RhoEventShapes::ThrustVector()
{
  if (fthr<0.) Thrust();
  
  return fThrVect;
}

// ---------------------------------------

double RhoEventShapes::Legendre( int l, double x )
{
    if (fabs(x)>1.) return -999.;

    if (l==0) return 1.;
    
    double pmm = 1.;
    double pmmp1 = x;

    if (l>1)
    {
        for(int ll=2; ll<=l; ll++)
        {
          double pll = (x * (2 * ll - 1) * pmmp1 - (ll - 1) * pmm) / (ll);
          pmm = pmmp1;
          pmmp1 = pll;
        }
    }
   return pmmp1;
}

// ---------------------------------------

double RhoEventShapes::FoxWolfMomH(int order)
{
  if (order>FWMAX) return -1.;
 
  // already computed FW moments
  if (fFWready) return fFWmom[order];
  
  if( fN==0 ) return -1.;

  double s = 0.;
  int i,j,l;

  for (i=0; i<fN-1; ++i)
  {
     // this candidate's 3-momentum
      TVector3 p1(fCmsList[i].Vect());
      double pmag1 = p1.Mag();

      // loop over other candidates, starting at the next one in the list
      
      for (j=i+1; j<fN; ++j)
      {
        // this candidate's 3-momentum
        TVector3 p2(fCmsList[j].Vect());
        double pmag2 = p2.Mag();

        // the cosine of the angle between the two candidates
        double cosPhi =  cos ( p1.Angle(p2) );

        // the contribution of this pair of track 
        // (note the factor 2 : the pair enters the sum twice)
        for( l=0; l<=FWMAX; l++ )
          fFWmom[l] += 2 * pmag1 * pmag2 * Legendre( l, cosPhi );
          
      }
      // contribution of this track
      for( l=0; l<=FWMAX; l++ )
        fFWmom[l] += pmag1 * pmag1 * Legendre( l, 1. );
      
      // total energy
      s += fCmsList[i].Energy();
   }
  
  // well ...
  if( s<=0. ) return -1.;
  double s2=s*s;
  
  // normalize Fox Wolfram Moments
  for( i=0; i<=FWMAX; i++) fFWmom[i]/=s2 ;  
  
  fFWready = true;
  
  return fFWmom[order];
}


// ---------------------------------------

double RhoEventShapes::FoxWolfMomR(int order)
{
  return FoxWolfMomH(order)/FoxWolfMomH(0);
}

// ---------------------------------------

int RhoEventShapes::MultPminLab(double pmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
    if (fLabList[i].P()>pmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int RhoEventShapes::MultPmaxLab(double pmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
    if (fLabList[i].P()<pmax) cnt++;
 
  return cnt;
}

// ---------------------------------------

int RhoEventShapes::MultPminCms(double pmin)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
    if (fCmsList[i].P()>pmin) cnt++;
 
  return cnt;
}

// ---------------------------------------

int RhoEventShapes::MultPmaxCms(double pmax)
{
  int cnt=0;
  for (int i=0;i<fN;++i) 
    if (fCmsList[i].P()<pmax) cnt++;
 
  return cnt;
}
