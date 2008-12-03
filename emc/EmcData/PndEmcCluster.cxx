//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//           energy()            returns energy sum of digis in cluster.
//           where()             returns a TVector3 at the centre of the
//                                 cluster.
//           x()                 returns the x element of this.
//           y()                 returns the y element of this.
//           z()                 returns the z element of this.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//	Helmut Marsiske         SLAC

//---------------------------------------------------------------------

#include "PndEmcCluster.h"

#include "PndEmcClusterLiloPos.h"
#include "PndEmcClusterLinearPos.h"
#include "PndEmcDigi.h"
#include "PndEmcXtal.h"
#include "PndEmcStructure.h"
#include "PndEmcXClMoments.h"
#include "PndEmcClusterMoments.h"
#include "PndEmcClusterEnergySums.h"
#include "PndEmcClusterDistances.h"	
#include "TVector3.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <vector>
#include "assert.h"

using std::endl;
using std::vector;

					
//----------------
// Constructors --
//----------------

PndEmcCluster::PndEmcCluster()  : 
	fMemberDigiMap( new PndEmcDigiPtrDict ),
	fEnergyValid( false ),
	fEnergy( 0 ),
	fWhereValid( false ),
	fWhere( 0 ),
	fTheClusLiloPos( 0 ),
	fTheClusLinearPos( 0 ),
	fTheClusEnergySums( 0 ),
	fTheClusMoments( 0 ),
	fTheClusXClMoments( 0 ),
	fTheClusDistances( 0 )
{
}

PndEmcCluster::PndEmcCluster(Int_t digiList, Int_t localMaxList) :
	fMemberDigiMap( new PndEmcDigiPtrDict ),
	fEnergyValid( false ),
	fEnergy( 0 ),
	fWhereValid( false ),
	fWhere( 0 ),
	fTheClusLiloPos( 0 ),
	fTheClusLinearPos( 0 ),
	fTheClusEnergySums( 0 ),
	fTheClusMoments( 0 ),
	fTheClusXClMoments( 0 ),
	fTheClusDistances( 0 )
{
	fDigiList.reserve(digiList);
	fLocalMaxList.reserve(localMaxList);
}

//--------------
// Destructor --
//--------------

PndEmcCluster::~PndEmcCluster()
{
  delete fWhere;
  fMemberDigiMap->clear();
  delete fMemberDigiMap;

  if ( fTheClusLiloPos != 0) delete fTheClusLiloPos;
  if ( fTheClusLinearPos != 0) delete fTheClusLinearPos;
  
  if ( fTheClusEnergySums != 0) delete fTheClusEnergySums;
  if ( fTheClusMoments != 0) delete fTheClusMoments;
  if ( fTheClusXClMoments != 0) delete fTheClusXClMoments;
  if ( fTheClusDistances != 0) delete fTheClusDistances;

}

Double_t
PndEmcCluster::energy() const
{
	if ( ! fEnergyValid )
	{
		Double_t sum=0;
      std::vector<PndEmcDigi*>::const_iterator digi_iter;

      for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
		{
			sum+=(*digi_iter)->GetEnergy();
		}
		
      fEnergy = sum;
      fEnergyValid = true;
    }
  
  return fEnergy;
}


Double_t
PndEmcCluster::theta() const
{
  return where().Theta();
}

Double_t
PndEmcCluster::phi() const
{
  return where().Phi();
}

Double_t
PndEmcCluster::thetaIndex() const
{
  return 0;
}

Double_t
PndEmcCluster::phiIndex() const
{
  return 0;
}

TVector3
PndEmcCluster::position() const
{
  return this->where();
}


TVector3
PndEmcCluster::where() const
{
  if ( !fWhereValid )
    {
      delete fWhere;
      fWhere = new TVector3( algWhere( this ) );
      fWhereValid = true;
    }

  return *fWhere;
}

Double_t
PndEmcCluster::x() const
{
  return where().x();
}

Double_t
PndEmcCluster::y() const
{
  return where().y();
}

Double_t
PndEmcCluster::z() const
{
  return where().z();
}


//-------------
// Modifiers --
//-------------

void
PndEmcCluster::addDigi( PndEmcDigi* theDigi )
{
  fDigiList.push_back(theDigi);
  PndEmcTwoCoordIndex *theTCI = theDigi->GetTCI();
  fMemberDigiMap->insert(PndEmcDigiPtrDict::value_type(theTCI, theDigi));

  invalidateCache();
}


Int_t
PndEmcCluster::NumberOfDigis() const
{
  return fDigiList.size();
}

Int_t PndEmcCluster::NBumps() const
{
	return fNbumps;
}

Double_t PndEmcCluster::Mass() const
{
	Double_t mass;
	TVector3 clusterMomentum(0,0,0);
	vector<PndEmcDigi*>::const_iterator digi_iter;
	TVector3 digiDirection;
	Double_t digiEnergy;
	for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
	{
		digiDirection=(*digi_iter)->where().Unit();
		digiEnergy=(*digi_iter)->GetEnergy();
		clusterMomentum=clusterMomentum+digiDirection*digiEnergy;
	}
	
	Double_t clEnergy=energy();
	
	mass=sqrt(clEnergy*clEnergy-clusterMomentum.Mag2());

	return mass;
}

void
PndEmcCluster::SetNBumps(unsigned nbumps) {
  fNbumps = nbumps;
}

void
PndEmcCluster::Print(const Option_t* opt) const
{
    std::cout<<"*********************************"<< endl;
    std::cout<<"total energy of cluster: "<< energy() << endl;
}


const PndEmcClusterLiloPos&
PndEmcCluster::liloPositions() const
{
  if (fTheClusLiloPos == 0) 
    fTheClusLiloPos = new PndEmcClusterLiloPos( *this );
  return *fTheClusLiloPos;
}

const PndEmcClusterLinearPos&
PndEmcCluster::linearPositions() const
{
  if (fTheClusLinearPos == 0) 
    fTheClusLinearPos = new PndEmcClusterLinearPos( *this );
  return *fTheClusLinearPos;
}


void
PndEmcCluster::addCluster(PndEmcCluster* cluster)
{
	const vector<PndEmcDigi*> tmpList = cluster->DigiList();
	vector<PndEmcDigi*>::const_iterator digi_iter;
	for (digi_iter=tmpList.begin();digi_iter!=tmpList.end();++digi_iter)
	{
		addDigi(*digi_iter);
	}
}

void
PndEmcCluster::selectCentroidMethod( CentroidMethod alg, PndEmcClusterLiloPosData ClusterPositionParameters )
{
  TVector3 (*algorithm)(const PndEmcCluster*) = 0;
  switch (alg)
    {

    case lilo:
      algorithm = PndEmcClusterLiloPos::liloWhere;
		PndEmcClusterLiloPos::SetParameters(ClusterPositionParameters);
      break;
		
	case linear:
		algorithm = PndEmcClusterLinearPos::linearWhere;
		break;

    default:
      std::cout << "PndEmcCluster::selectCentroidMethod. "
		      << "Attmpted to select unknown cluster position method." 
		      << endl;
    }

  // Now actually set the poInt_ter
  algPointer() = algorithm;
}


TVector3
PndEmcCluster::algWhere( const PndEmcCluster* me )
{
  return algPointer()( me );
}

// Set the default cluster position method

TVector3 (*&PndEmcCluster::algPointer())( const PndEmcCluster* )
{
   static TVector3 (*pointer)( const PndEmcCluster* ) = PndEmcClusterLiloPos::liloWhere;

  return pointer;
}

void
PndEmcCluster::invalidateCache()
{
  fEnergyValid = false;
  fWhereValid = false;

  if ( fTheClusLiloPos != 0) {
    delete fTheClusLiloPos;
    fTheClusLiloPos = 0;
  }
}

//check if a digi belong to this cluster
bool 
PndEmcCluster::isInCluster( PndEmcDigi* theDigi )
{
	vector<PndEmcDigi*>::iterator digi_iter;
	for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
	{
		if(theDigi->isNeighbour(*digi_iter)) return true;
	}

	return false;
}

// Returnd digi with the highest energy in cluster
const PndEmcDigi*
PndEmcCluster::Maxima() const
{
	Double_t max=0;
	PndEmcDigi *biggest=0;

	std::vector<PndEmcDigi*>::const_iterator digipos;
	
	for (digipos=fDigiList.begin();digipos!=fDigiList.end();++digipos){
		if ( max < (*digipos)->GetEnergy() ) {
			max=(*digipos)->GetEnergy();
			biggest=*digipos;
      }
	}

	return( biggest );
}

PndEmcDigi*
PndEmcCluster::Maxima()
{
	Double_t max=0;
	PndEmcDigi *biggest=0;

	std::vector<PndEmcDigi*>::iterator digipos;
	
	for (digipos=fDigiList.begin();digipos!=fDigiList.end();++digipos){
		if ( max < (*digipos)->GetEnergy() ) {
			max=(*digipos)->GetEnergy();
			biggest=*digipos;
      }
	}

	return( biggest );
}

Double_t
PndEmcCluster::Major_axis() const
{
	Double_t maj=0.,st2=0.;
	Double_t n=fDigiList.size();
	
	if ( n==1 ) return( -999. );
	
	Double_t phi_wtd=Moments().Phi1();
	Double_t phi_clu=phi();
	Double_t theta_clu=theta();
	Double_t theta_wtd = Moments().Theta1();
	
	std::vector<PndEmcDigi*>::const_iterator current;

	for (current=fDigiList.begin();current!=fDigiList.end();++current)
	{
		Double_t x=0.,y=0.,e=0.,t=0.;
		x=FindPhiDiff((*current)->GetPhi(),phi_clu);
		y=(*current)->GetTheta()-theta_clu;
		e=(*current)->GetEnergy();
		e*=e;                     // Will use e squared
		t=(x-phi_wtd)*e;
		st2+=t*t;
		maj+=t*(y-theta_wtd)*e;
	}
	
	// The major axis is found by the slope of a line through 
	// the coordinates or the Digis with respect to the centre
	// of the cluster. (Or just the coordinates, but I'll be consistent.)
	
	if ( !st2 ) return( TMath::Pi()/2.0 );
	
	maj/=st2;
	
	return( atan( maj ) );
}


Double_t
PndEmcCluster::FindPhiDiff( Double_t phi1, Double_t phi2)
{
  Double_t diff;
  diff = phi1 - phi2;

  while( diff>  TMath::Pi()  ) diff -= 2*TMath::Pi();
  while( diff< -TMath::Pi()  ) diff += 2*TMath::Pi();

  return diff;
}

TVector3
PndEmcCluster::GravWhere( const PndEmcCluster* me )
{
	TVector3 aVector(0,0,0);
	
	Int_t length = me->fDigiList.size();
	for (Int_t i=0; i<length; i++ ) {
		const PndEmcDigi* digi = (me->fDigiList)[i];
		aVector += digi->where()* digi->GetEnergy();
	}
	
	aVector *= 1./me->energy();
	
	PndEmcTwoCoordIndex *theTCI=PndEmcStructure::Instance()->locateIndex(aVector.Theta(),aVector.Phi());

	assert(theTCI != 0);  
	
	std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	
	const PndEmcXtal *theGeom=tciXtalMap[theTCI];
	
	const TVector3 normal = theGeom->normalToFrontFace();
	
	TVector3 centre(theGeom->frontCentre() - TVector3(0.0,0.0,0.0));
	
	double distanceOfPlane = normal.Dot(centre);
	
	double amplitude = distanceOfPlane / normal.Dot(aVector.Unit());
	
	aVector.SetMag(amplitude);
	
	return TVector3( aVector.x(), aVector.y(), aVector.z() );
}


Double_t
PndEmcCluster::DistanceToCentre( const TVector3& aPoint ) const
{
  return  ( where() - aPoint ).Mag();
}

Double_t
PndEmcCluster::DistanceToCentre( const PndEmcDigi* aDigi ) const
{
  return  ( where() - aDigi->where() ).Mag();
}

const PndEmcClusterEnergySums&
PndEmcCluster::Esums() const
{
  if (fTheClusEnergySums == 0) 
    fTheClusEnergySums = new PndEmcClusterEnergySums( *this );
  return *fTheClusEnergySums;
}

const PndEmcClusterMoments&
PndEmcCluster::Moments() const
{
  if (fTheClusMoments == 0) 
    fTheClusMoments = new PndEmcClusterMoments( *this );
  return *fTheClusMoments;
}

const PndEmcClusterDistances&
PndEmcCluster::Distances() const
{
  if (fTheClusDistances == 0) 
    fTheClusDistances = new PndEmcClusterDistances( *this );
  return * fTheClusDistances;
}

const PndEmcXClMoments&
PndEmcCluster::Xmoments() const
{
  if (fTheClusXClMoments == 0) 
    fTheClusXClMoments = new PndEmcXClMoments( *this );
  return *fTheClusXClMoments;
}

Double_t
PndEmcCluster::GetEnergyCorrected() const
{
	Double_t e=this->energy();
	TVector3 clusterPosition= this->where();
	Double_t theta=clusterPosition.Theta();
		
	Double_t e1=e;
	Double_t theta1=theta;
	
	if ( (clusterPosition.Z() < 180.0)&&(theta<140.*TMath::Pi()/180.))
	{
		if (e<0.03) e1 = 0.03;
		if (e>8.0)  e1 = 8.0 ;
	}
	
	if ( (clusterPosition.Z() < 180.0)&&(theta>140.*TMath::Pi()/180.))
	{
		if (e<0.03) e1 = 0.03;
		if (e>2.0)  e1 = 2.0 ;
	}
	
	if (clusterPosition.Z() > 180.0)
	{
		if (e<0.01) e1 = 0.01;
		if (e>16.0)  e1 = 16.0 ;
	}

  double b0 = 1.45312;
  double b1 = 2.79086e-02;
  double b2 = 3.91932e-04;
  double b3 =-1.23117e-03;
  double b4 = 2.72270e-01;
  double b5 =-1.31540;
  double b6 = 1.44447;
  double b7 =-4.05724e-01;
  double b8 =-2.07396;
  double b9 = 4.80507e-02;

  double p0=0;
  double p1=0;
  double p2=0;
  double p3=0;
  double p4=0;
  double p5=0;
  double p6=0;
  double p7=0;
  double p8=0;
  double p9=0;

  if(e1<1.0)
  {
     p0 =  4.13189e-02;   
     p1 = -2.03834e-02;  
     p2 = -2.58086e-03;   
     p3 = -1.77821e-03;   
     p4 = -1.73738e-02;   
     p5 =  7.40362e-02;   
     p6 = -6.41892e-02;   
     p7 = -9.85564e-02;   
     p8 =  1.50123e-01;   
     p9 = -7.87742e-04;   
  }
  else
  {
     p0 =  5.05003e-02;  
     p1 = -3.47672e-02;   
     p2 =  3.72767e-02;   
     p3 = -1.26492e-02;   
     p4 = -2.16876e-02;   
     p5 =  1.02682e-01;   
     p6 = -9.85242e-02;   
     p7 = -1.39872e-01;   
     p8 =  2.02309e-01;   
     p9 =  1.11696e-03;   
  }


  double t0 = 1.81631;
  double t1 =-1.71202e-02;
  double t2 = 3.59161e-03;
  double t3 =-3.46712e-04;
  double t4 =-3.73691e-01;
  double t5 =-1.56688;
  double t6 =-1.62618;
  double t7 =-4.10972e-01;
  double t8 = 2.2222;
  double t9 = 4.60908e-03;


  double factor1= p0
		+p1*log(e1)
		+p2*log(e1)*log(e1)
		+p3*log(e1)*log(e1)*log(e1)
		+p4*cos(theta1)
		+p5*cos(theta1)*cos(theta1)
		+p6*cos(theta1)*cos(theta1)*cos(theta1)
		+p7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
		+p8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
		+p9*log(e1)*cos(theta1);

  double factor2= t0
                +t1*log(e1)
                +t2*log(e1)*log(e1)
                +t3*log(e1)*log(e1)*log(e1)
                +t4*cos(theta1)
                +t5*cos(theta1)*cos(theta1)
                +t6*cos(theta1)*cos(theta1)*cos(theta1)
                +t7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +t8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +t9*log(e1)*cos(theta1);


  double factor3= b0
                +b1*log(e1)
                +b2*log(e1)*log(e1)
                +b3*log(e1)*log(e1)*log(e1)
                +b4*cos(theta1)
                +b5*cos(theta1)*cos(theta1)
                +b6*cos(theta1)*cos(theta1)*cos(theta1)
                +b7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +b8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
                +b9*log(e1)*cos(theta1);


  double eout1=e* exp(factor1); 
  double eout2=e* exp(factor2); 
  double eout3=e* exp(factor3); 

  double eout4=(3.31694-0.0183379/sqrt(e1)+0.0327113/e1+0.00040156/(e1*e1)-0.00641305/(e1*sqrt(e1)))*e;
  
  

	if ( clusterPosition.Z() > 500.0)
		return eout4;
	else if ( (clusterPosition.Z() < 180.0)&&(theta>140.*TMath::Pi()/180.))
		return eout3;
	else  if ( (clusterPosition.Z() < 180.0)&&(theta<140.*TMath::Pi()/180.))
		return eout1;
	else 
		return eout2;

}

void PndEmcCluster::ValidateDigiMap()
{
	fMemberDigiMap->clear();
	vector<PndEmcDigi*>::iterator digi_iter;
	for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
	{
		(*digi_iter)->ValidateTCI();
		PndEmcTwoCoordIndex *theTCI = (*digi_iter)->GetTCI();
		fMemberDigiMap->insert(PndEmcDigiPtrDict::value_type(theTCI, (*digi_iter)));
	}
}

TMatrixD PndEmcCluster::GetErrorMatrix() const{
	
	Double_t clEnergy=energy();
	Double_t clTheta=theta();
	Double_t clPhi=phi();
	
	enum {bwcap, barrel, fwcap, fsc};
	Int_t clusterInComponent=-1; //in which detector component is the cluster
	
	if( clTheta<10*M_PI/180.0 ){
		clusterInComponent = fsc;
	}else if( clTheta<22*M_PI/180.0 ){
		clusterInComponent = fwcap;
	}else if( clTheta<145*M_PI/180.0 ){
		clusterInComponent = barrel;
	}else{
		clusterInComponent = bwcap;
	}

	//errors on position are estimated w/ respect to 
	//nominal z=100. (for fwcap, bwcap, fsc) ---> dx(z) and dy(z) 
	//and 
	//nominal R=100. (for barrel)   ---> dz(r) and dphi(r)
	//calculated errors are scaled with the following factors
	//
	Double_t barrelRadius=54.;
	Double_t bwCapPosZ = 56.;
	Double_t fwCapPosZ = 208.2;
	Double_t fscPosZ =   780.;

	Double_t scaleFactor[4];
	scaleFactor[bwcap]=bwCapPosZ/100.;
	scaleFactor[barrel]=barrelRadius/100.;
	scaleFactor[fwcap]=fwCapPosZ/100.;
	scaleFactor[fsc]=fscPosZ/100.;
	
	Double_t energyMinCutOff[4]={ 0.1, 0.1, 0.1, 0.5 };
	Double_t energyMaxCutOff[4]={ 1.5, 5., 7., 7. };
	Double_t eng=clEnergy;
	if(clEnergy>energyMaxCutOff[clusterInComponent]) eng=energyMaxCutOff[clusterInComponent];
	if(clEnergy<energyMinCutOff[clusterInComponent]) eng=energyMinCutOff[clusterInComponent];
	
	//functions used for parameterization
	//Energy: Delta(E)/E = (a^2/E^power) + const^2 + (quadr/E)^2
	//position: Delta(x)=(a*a/E^power) + const^2
	Double_t engParA[4], engPower[4], engConst[4],engQuadr[4];
	Double_t pos1ParA[4], pos1Power[4], pos1Const[4];
	Double_t pos2ParA[4], pos2Power[4], pos2Const[4];
	
	engParA[bwcap]=0.073138;
	engPower[bwcap]=0.874890;
	engConst[bwcap]=0.119693;
	engQuadr[bwcap]=0.000000;
	pos1ParA[bwcap]=0.455331;
	pos1Power[bwcap]=0.457373;
	pos1Const[bwcap]=0.604822;
	pos2ParA[bwcap]=0.558651;
	pos2Power[bwcap]=0.337999;
	pos2Const[bwcap]=0.528809;
	engParA[barrel]=0.087543;
	engPower[barrel]=0.806983;
	engConst[barrel]=0.084801;
	engQuadr[barrel]=0.000000;
	pos1ParA[barrel]=0.669595;
	pos1Power[barrel]=0.336361;
	pos1Const[barrel]=0.000000;
	pos2ParA[barrel]=0.067536;
	pos2Power[barrel]=0.348951;
	pos2Const[barrel]=0.000000;
	engParA[fwcap]=0.091336;
	engPower[fwcap]=0.763612;
	engConst[fwcap]=0.067230;
	engQuadr[fwcap]=0.000000;
	pos1ParA[fwcap]=0.293973;
	pos1Power[fwcap]=0.451571;
	pos1Const[fwcap]=0.234385;
	pos2ParA[fwcap]=0.294116;
	pos2Power[fwcap]=0.459566;
	pos2Const[fwcap]=0.237317;
	engParA[fsc]=0.202292;
	engPower[fsc]=0.717711;
	engConst[fsc]=0.083642;
	engQuadr[fsc]=0.000000;
	pos1ParA[fsc]=0.333771;
	pos1Power[fsc]=0.715637;
	pos1Const[fsc]=0.341344;
	pos2ParA[fsc]=0.393149;
	pos2Power[fsc]=0.397055;
	pos2Const[fsc]=0.321306;
	/////
	
	
	Int_t i = clusterInComponent; //for shortness
	//std::cout << "+++++++++ Cluster in " << i << std::endl;
	//enrgy error
	Double_t errEnergy = engParA[i]*engParA[i]/pow(eng,engPower[i]) 
		+ pow(engConst[i], 2.0) 
		+ pow(engQuadr[i]/eng,2.0) ;
	//std::cout << "Error on energy for " << clEnergy << " GeV is " << errEnergy << std::endl;
	errEnergy*=clEnergy;
	
	//position coordinate errors
	Double_t pos1Err= pow(pos1ParA[i],2.)/pow(eng,pos1Power[i])+pow(pos1Const[i],2.);
	//std::cout << "Error on pos1 for " << clEnergy << " GeV is " << pos1Err << std::endl;
	pos1Err*=scaleFactor[i];
	
	Double_t pos2Err= pow(pos2ParA[i],2.)/pow(eng,pos2Power[i])+pow(pos2Const[i],2.);
	//std::cout << "Error on pos2 for " << clEnergy << " GeV is " << pos2Err << std::endl;
	if(clusterInComponent!=barrel)
		pos2Err*=scaleFactor[i];
	
	
	//error matrix for E, pos1, pos2 and arbitrary pos3
	TMatrixD theError(3,3);
	theError(0,0)=errEnergy*errEnergy;
	theError(1,1)=pos1Err*pos1Err;
	theError(2,2)=pos2Err*pos2Err;
	
	//std::cout << "theError before transformation: " << theError << std::endl;
	
	Double_t sin_theta=sin(clTheta);
	Double_t sin_phi  =sin(clPhi);
	Double_t cos_theta=cos(clTheta);
	Double_t cos_phi  =cos(clPhi);
	
	
	TMatrixD trans(4,3);
	if(clusterInComponent==barrel){
		Double_t barrelRadius=scaleFactor[barrel]*100.;
		
		trans(0,0)=1.;                       //dE/dE
		trans(0,1)=0.;                       //dE/dz
		trans(0,2)=0.;                       //dE/dphi
		
		trans(1,0)=0.;                       //dTheta/dE
		trans(1,1)=-sin_theta*sin_theta/barrelRadius;  //dTheta/dz
		trans(1,2)=0.;                       //dTheta/dphi
		
		trans(2,0)=0.;                       //dPhi/dE
		trans(2,1)=0.;                       //dPhi/dz
		trans(2,2)=1.;                       //dPhi/dphi
		
		trans(3,0)=0.;                       //dR/dE
		trans(3,1)=0.;                       //dR/dz
		trans(3,2)=0.;                       //dR/dphi
		
	}else{
		Double_t R=fabs(scaleFactor[i]*100./cos_theta); //a bit ugly, but theta is never 90 deg for these components 
		//trans(row, col)
		trans(0,0)=1.;                       //dE/dE
		trans(0,1)=0.;                       //dE/dx
		trans(0,2)=0.;                       //dE/dy
		
		trans(1,0)=0.;                       //dTheta/dE
		trans(1,1)= cos_theta * cos_phi /R;  //dTheta/dx
		trans(1,2)= cos_theta * sin_phi /R;  //dTheta/dy
		
		trans(2,0)=0.;//dPhi/dE
		trans(2,1)= -sin_phi /(R*sin_theta); //dPhi/dx
		trans(2,2)= cos_phi/(R*sin_theta);   //dPhi/dy
		
		trans(3,0)=0.;                       //dR/dE
		//trans(4,2)= sin_theta * cos_phi /R;  //dR/dx
		//trans(4,3)= sin_theta * sin_phi /R;  //dR/dy
		trans(3,1)= 0.0;  //dR/dx
		trans(3,2)= 0.0;  //dR/dy
	
	}
	
	// Error matrix in (E, theta, phi, R)
	TMatrixD errorMatrix=similarityWith(theError,trans);
	
	return errorMatrix;
	
}

TMatrixD PndEmcCluster::Get4MomentumErrorMatrix() const {
	// Conversion from (E, theta, phi, r) to ( px, py, pz, E )
	
	double z = where().Z();
	double perp = where().Perp();
	double cos_theta = z;
	double sin_theta = perp;
	double sin_phi = where().Y() / perp;
	double cos_phi = where().X() / perp;
	double e=energy();
	double p = e;

	// Create a matrix to transform the error matrix
	TMatrixD toComp( 4, 4 );
	toComp(0,0) = sin_theta * cos_phi * e/p;
	toComp(0,1) = p * cos_theta * cos_phi;
	toComp(0,2) = -p * sin_theta * sin_phi;
	toComp(0,3) = 0;
	toComp(1,0) = sin_theta * sin_phi * e/p;
	toComp(1,1) = p * cos_theta * sin_phi;
	toComp(1,2) = p * sin_theta * cos_phi;
	toComp(1,3) = 0;
	toComp(2,0) = cos_theta * e/p;
	toComp(2,1) = -p * sin_theta;
	toComp(2,2) = 0;
	toComp(2,3) = 0;
	toComp(3,0) = 1;
	toComp(3,1) = 0;
	toComp(3,2) = 0;
	toComp(3,3) = 0;

	TMatrixD errorMatrix=similarityWith(GetErrorMatrix(),toComp);
	
	return errorMatrix;
}


// Function is copied from BbrGeom/BbrError.cc
// It does the same as m1*mat*m1^T
// but with assumption that mat, and output matrix are symmetric

TMatrixD similarityWith(const TMatrixD& mat,
                                           const TMatrixD& m1)
{
	TMatrixD result(m1.GetNrows(),m1.GetNrows());
	
	TMatrixD temp = m1*mat;
	double tmp;
	
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c <= r; c++) {
			tmp = 0.;
			for (int k = 0; k < m1.GetNcols(); k++) {
				tmp += temp(r,k)*m1(c,k);
			}
			result(r,c) = tmp;
			// Modification from original code, to make output matrix explicitly symmetric
			// In original babar code symmetric matrix were stored as lower triangular.
			if (r!=c)
				result(c,r) = tmp;
		}
	}
	return result;
}


ClassImp(PndEmcCluster)
