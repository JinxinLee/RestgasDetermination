#include <iostream>
#include <map>
#include <vector>
#include <fstream>


//#include "runinfo.h"
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsDigi.h"
#include "PndSdsCluster.h"
#include "PndSdsCalcStrip.h"

using namespace std;
ClassImp(PndSdsChargeWeightingAlgorithms);

PndSdsChargeWeightingAlgorithms::PndSdsChargeWeightingAlgorithms(TClonesArray* arr)
{
	fDigiArray = arr;
}
PndSdsChargeWeightingAlgorithms::~PndSdsChargeWeightingAlgorithms()
{
}

/*
 returns the (modified) error function
 this is a local (private) function
 */
double PndSdsChargeWeightingAlgorithms::_erfmod(double x, double p0, double p1, double p2, double p3)
{
  return ((TMath::Erf(p0*(x-p1)))*p2+p3);
}

std::pair<double,double> PndSdsChargeWeightingAlgorithms::center_of_gravity(const PndSdsCluster& Cluster)
{
  std::pair<double,double> result;
  int nrHits = Cluster.GetClusterSize();
  if(nrHits>1)							// minimum of hits in cluster
  {
    double x_g=0., chargesum=0, charge=0, chargemean=0, noise=0, form=0;
    //double error=0.;
    noise = fCalcStrip->GetNoise();
    for(int l=0;l<nrHits;++l)     // loop over all hits
    {
      // we work in cannel numbers, so no pitch used
      //
      //     _ q_i*chan_i
      // x_g=\ --------------
      //     /   Q_sum
      //     -
      //
      //       noise
      // dx=   ------ * a^.5(formfactor)
      //       q_mean
      //
      
      charge = DigiCharge(Cluster.GetDigiIndex(l));
      chargesum+=charge;
      x_g = x_g + ( charge * DigiStripno(Cluster.GetDigiIndex(l)) );
    }
    switch (nrHits) {
      case 2:
        form=sqrt(2/3); //[Bashindzhagyan2005]
        break;
      case 3:
        form=2.12; //[Radeka1980]
        break;
      default:
        form=1.; //TODO: Add more?
        break;
    }
    chargemean=chargesum/nrHits;
    result.first=x_g/chargesum+0.5;
    result.second=form*noise/chargemean;
  }else{
    return binary(Cluster);
  }
  Info("center_of_gravity","Got a cluster with %i digis. Position %d ± %d chn.",nrHits,result.first,result.second);
  return result;
}


std::pair<double,double> PndSdsChargeWeightingAlgorithms::head_tail(const PndSdsCluster& Cluster)
{
  std::pair<double,double> result;
  int nrHits = Cluster.GetClusterSize();
  if(nrHits>1)
  {
    double q_inner=0.;
    //       if(nrHits==2) q_inner=Cluster.GetChargeSum()/2.;
    //       if(nrHits>2) q_inner=(Cluster.GetChargeSum()-Cluster.GetHit(0).GetCharge()-Cluster.GetHit(nrHits-1).GetCharge())*1./nrHits;
    for(int l=0;l<nrHits;++l) q_inner += DigiCharge(Cluster.GetDigiIndex(l));
    q_inner = q_inner/nrHits;
    
    double x_ht=(DigiStripno(Cluster.GetDigiIndex(0))+DigiStripno(Cluster.GetDigiIndex(nrHits-1)))/2.;
    double wert=(DigiCharge(Cluster.GetDigiIndex(0))-DigiCharge(Cluster.GetDigiIndex(nrHits-1)))/(2.*q_inner);
    if(wert<0) wert=-wert;
    result.first=(x_ht+wert+0.5);
  }else{
    return binary(Cluster);
  }
  result.second=0.;
  return result;
}


std::pair<double,double> PndSdsChargeWeightingAlgorithms::binary(const PndSdsCluster& Cluster)
{
  std::pair<double,double> result;
  int nrHits = Cluster.GetClusterSize();
  double charge=0.;
  double channel=0;
  for(int i=0;i<nrHits;++i)
  {
    if(DigiCharge(Cluster.GetDigiIndex(i)) > charge)
    {
      charge=DigiCharge(Cluster.GetDigiIndex(i));
      channel=DigiStripno(Cluster.GetDigiIndex(i));
    }
  }
  result.first=channel+0.5;
  result.second=0.;
  return result;
}

std::pair<double,double> PndSdsChargeWeightingAlgorithms::eta(const PndSdsCluster& Cluster)
{
  //TODO: Make a good MVD par file? Eta Algo needed at all?
  std::pair<double,double> result;				// first calc eta!
  int nrHits = Cluster.GetClusterSize();
  if(nrHits>1)
  {
    double ql=DigiCharge(Cluster.GetDigiIndex(0));
    double qr=DigiCharge(Cluster.GetDigiIndex(nrHits-1));
    
    int f=1, o=nrHits-2;
    double xl=DigiStripno(Cluster.GetDigiIndex(0))*1.;
    
    double p0,pe0,p1,pe1,p2,pe2,p3,pe3;				// f(eta) parameters calc before
    std::ifstream InPar("/home/student/Desktop/repository/calibPar/Eta_para.par");
    InPar >> p0>>pe0>>p1>>pe1>>p2>>pe2>>p3>>pe3;
    InPar.close();
    while(f<o)							// build groups nearly same charge value
    {
      if(ql<=qr) 						// who is smaller
      {
        ql=ql+DigiCharge(Cluster.GetDigiIndex(f));
        ++f;
      }
      if(qr<ql)
      {
        qr=qr+DigiCharge(Cluster.GetDigiIndex(o));
        //             xl=xl+_erfmod(sube, p0, p1, p2, p3);
        --o;
      }
    }
    
    double e = qr/(qr+ql);					// total eta
    
    result.first=xl+f-1+_erfmod(e, p0, p1, p2, p3)+0.5;				// reconstruct
  }else{
    return binary(Cluster);
  }
  result.second=0.;
  return result;
}

std::pair<double,double> PndSdsChargeWeightingAlgorithms::auto_select(const PndSdsCluster& Cluster)
{
  Warning("auto_select", "Do not use, this selection is still wrong!");
  switch(Cluster.GetClusterSize())
  {
    case 1:
      return binary(Cluster);
      break;
    case 2:
      return eta(Cluster);
      break;
    case 3:
      return center_of_gravity(Cluster);
      break;
    case 4:
      return center_of_gravity(Cluster);
      break;
    default:
      return head_tail(Cluster);
      break;
  }
  return head_tail(Cluster);
}
/*
 void PndSdsChargeWeightingAlgorithms::MakedNdEta(const std::vector<PndSdsCluster>& Cluster, RunInfo& info)
 {
 for(int cID=0;cID<Cluster.size();cID++)
 {
 int nrHits = Cluster[cID].GetClusterSize();
 if(nrHits>1)
 {
 double ql=Cluster[cID].GetHit(0).GetCharge()*1.;
 double qr=Cluster[cID].GetHit(nrHits-1).GetCharge()*1.;
 
 int f=1, o=nrHits-2;
 double xl=Cluster[cID].GetHit(0).GetChannel()*1.;
 while(f<o)							// build groups nearly same charge value
 {
 if(ql<=qr) 						// who is smaller
 {
 ql=ql+Cluster[cID].GetHit(f).GetCharge()*1.;
 ++f;
 }
 if(qr<ql)
 {
 qr=qr+Cluster[cID].GetHit(o).GetCharge()*1.;
 //             xl=xl+_erfmod(sube, p0, p1, p2, p3);
 --o;
 }
 }
 double e = qr/(qr+ql);					// total eta
 info.FilldNdEta(e);
 }
 }
 return;
 }
 */


Int_t PndSdsChargeWeightingAlgorithms::DigiStripno(Int_t digiIndex)
{
	Int_t strip;
	SensorSide side;
	PndSdsDigiStrip* myDigi = (PndSdsDigiStrip*)fDigiArray->At(digiIndex);
	fCalcStrip->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
  return strip;
}






