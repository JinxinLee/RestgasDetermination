#include <iostream>
#include <map>
#include <vector>
#include <fstream>


//#include "runinfo.h"
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsDigi.h"
#include "PndSdsCluster.h"
#include "PndSdsCalcStrip.h"

//using namespace std;
ClassImp(PndSdsChargeWeightingAlgorithms);

PndSdsChargeWeightingAlgorithms::PndSdsChargeWeightingAlgorithms(TClonesArray* arr)
{
	fDigiArray = arr;
  fVerbose=0;
}
PndSdsChargeWeightingAlgorithms::~PndSdsChargeWeightingAlgorithms()
{
}

/*
 returns the (modified) error function
 this is a local (private) function
 */
Double_t PndSdsChargeWeightingAlgorithms::Erfmod(Double_t x, Double_t p0, Double_t p1, Double_t p2, Double_t p3)
{
  return ((TMath::Erf(p0*(x-p1)))*p2+p3);
}

std::pair<Double_t,Double_t> PndSdsChargeWeightingAlgorithms::CenterOfGravity(const PndSdsCluster* Cluster)
{
  std::pair<Double_t,Double_t> result;
  Int_t nrHits = Cluster->GetClusterSize();
  if(nrHits>1)							// minimum of hits in cluster
  {
    Double_t x_g=0., chargesum=0, charge=0, noise=0, stripno=0;
    Double_t xerror=0.,xerrtmp=0.,cherr=0.;
    noise = fCalcStrip->GetNoise();
    for(Int_t l=0;l<nrHits;++l)     // loop over all hits
    {
      // we work in cannel numbers, so no pitch used
      //
      //           ( q_i*chan_i )
      // x_g= SUM ( ------------ )
      //           (   Q_sum    )
      //     
      //
      
      charge = DigiCharge(Cluster->GetDigiIndex(l));
      stripno = DigiStripno(Cluster->GetDigiIndex(l));
      chargesum+=charge;
      x_g += charge * stripno ;
      if(fVerbose>2) Info("center_of_gravity","adding digi values (stripno,charge) = (%f,%f)",stripno,charge);
    }
    x_g = x_g/chargesum;
    result.first=x_g;
    
    // error estimation propagate dq:
    //      Sqrt (SUM{ ((x_i - x)dq_i)^2 })
    // dx=  -------------------------------
    //                    Q
    //
    for(Int_t l=0;l<nrHits;++l)     // loop over all hits again (errors)
    {
      cherr = DigiChargeError(Cluster->GetDigiIndex(l));
      cherr = sqrt(noise*noise+cherr*cherr);
      xerrtmp = ( x_g - DigiStripno(Cluster->GetDigiIndex(l)) ) * cherr;
      xerror += xerrtmp*xerrtmp;
    }
    xerror = sqrt(xerror)/(chargesum);
    
    if (xerror < 1e-15) 
      Warning("center_of_gravity","Got bad error value: Cluster with %i digis. Position %f ± %f chn.",nrHits,x_g,xerror);
    result.second = xerror;
    
  }else{
    result=Binary(Cluster);
  }
  if(fVerbose>1) Info("center_of_gravity","Got a cluster with %i digis. Position %f ± %f chn.",nrHits,result.first,result.second);
  return result;
}


std::pair<Double_t,Double_t> PndSdsChargeWeightingAlgorithms::HeadTail(const PndSdsCluster* Cluster)
{
  // We assume that the digis in each cluster are sorted.
  std::pair<Double_t,Double_t> result;
  Int_t nrHits = Cluster->GetClusterSize();
  if(nrHits>1)
  {
    Double_t q_inner=0.;
    //       if(nrHits==2) q_inner=Cluster->GetChargeSum()/2.;
    //       if(nrHits>2) q_inner=(Cluster->GetChargeSum()-Cluster->GetHit(0).GetCharge()-Cluster->GetHit(nrHits-1).GetCharge())*1./nrHits;
    for(Int_t l=0;l<nrHits;++l) q_inner += DigiCharge(Cluster->GetDigiIndex(l));
    q_inner = q_inner/nrHits;
    
    Double_t x_ht=(DigiStripno(Cluster->GetDigiIndex(0))+DigiStripno(Cluster->GetDigiIndex(nrHits-1)))/2.;
    Double_t wert=(DigiCharge(Cluster->GetDigiIndex(0))-DigiCharge(Cluster->GetDigiIndex(nrHits-1)))/(2.*q_inner);
    if(wert<0) wert=-wert;
    result.first=(x_ht+wert);
  }else{
    return Binary(Cluster);
  }
  result.second=0.;
  return result;
}


std::pair<Double_t,Double_t> PndSdsChargeWeightingAlgorithms::Binary(const PndSdsCluster* Cluster)
{
  std::pair<Double_t,Double_t> result;
  Int_t nrHits = Cluster->GetClusterSize();
  Double_t charge=0.,chargemax=0;
  Double_t channel=0;
  for(Int_t i=0;i<nrHits;++i)
  {
    charge=DigiCharge(Cluster->GetDigiIndex(i));
    if( charge > chargemax)
    { // choose highest signal
      chargemax=charge;
      channel=DigiStripno(Cluster->GetDigiIndex(i));
    }
  }
  result.first=channel;
  result.second=1./sqrt(12.);
  if(fVerbose>1) Info("binary","Got a cluster with %i digis. Position %f ± %f chn.",nrHits,result.first,result.second);
  return result;
}

std::pair<Double_t,Double_t> PndSdsChargeWeightingAlgorithms::Eta(const PndSdsCluster* Cluster)
{
  //TODO: Make a good MVD eta-par file. Eta Algo needed at all?
  std::pair<Double_t,Double_t> result;				// first calc eta!
  Int_t nrHits = Cluster->GetClusterSize();
  if(nrHits>1)
  {
    Double_t ql=DigiCharge(Cluster->GetDigiIndex(0));
    Double_t qr=DigiCharge(Cluster->GetDigiIndex(nrHits-1));
    
    Int_t f=1, o=nrHits-2;
    Double_t xl=DigiStripno(Cluster->GetDigiIndex(0))*1.;
    
    Double_t p0,pe0,p1,pe1,p2,pe2,p3,pe3;				// f(eta) parameters calc before
    std::ifstream InPar("/home/student/Desktop/repository/calibPar/Eta_para.par");
    InPar >> p0>>pe0>>p1>>pe1>>p2>>pe2>>p3>>pe3;
    InPar.close();
    while(f<o)							// build groups nearly same charge value
    {
      if(ql<=qr) 						// who is smaller
      {
        ql=ql+DigiCharge(Cluster->GetDigiIndex(f));
        ++f;
      }
      if(qr<ql)
      {
        qr=qr+DigiCharge(Cluster->GetDigiIndex(o));
        //             xl=xl+Erfmod(sube, p0, p1, p2, p3);
        --o;
      }
    }
    
    Double_t e = qr/(qr+ql);					// total eta
    
    result.first=xl+f-1+Erfmod(e, p0, p1, p2, p3);				// reconstruct
  }else{
    return Binary(Cluster);
  }
  result.second=0.;
  return result;
}

std::pair<Double_t,Double_t> PndSdsChargeWeightingAlgorithms::AutoSelect(const PndSdsCluster* Cluster)
{
  Warning("auto_select", "Do not use, this selection is still wrong!");
  switch(Cluster->GetClusterSize())
  {
    case 1:
      return Binary(Cluster);
      break;
    case 2:
      //return eta(Cluster); // switched off (no par file)
      return CenterOfGravity(Cluster);
      break;
    case 3:
      return CenterOfGravity(Cluster);
      break;
    case 4:
      return CenterOfGravity(Cluster);
      break;
    default:
      return HeadTail(Cluster);
      break;
  }
  return HeadTail(Cluster);
}
/*
 void PndSdsChargeWeightingAlgorithms::MakedNdEta(const std::vector<PndSdsCluster>& Cluster, RunInfo& info)
 {
 for(Int_t cID=0;cID<Cluster->size();cID++)
 {
 Int_t nrHits = Cluster[cID].GetClusterSize();
 if(nrHits>1)
 {
 Double_t ql=Cluster[cID].GetHit(0).GetCharge()*1.;
 Double_t qr=Cluster[cID].GetHit(nrHits-1).GetCharge()*1.;
 
 Int_t f=1, o=nrHits-2;
 Double_t xl=Cluster[cID].GetHit(0).GetChannel()*1.;
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
 //             xl=xl+Erfmod(sube, p0, p1, p2, p3);
 --o;
 }
 }
 Double_t e = qr/(qr+ql);					// total eta
 info.FilldNdEta(e);
 }
 }
 return;
 }
 */

Double_t PndSdsChargeWeightingAlgorithms::DigiCharge(Int_t digiIndex)
{
  PndSdsDigiStrip* digi = (PndSdsDigiStrip*)(fDigiArray->At(digiIndex));
  Double_t charge = fChargeConverter->DigiValueToCharge(digi->GetCharge());
  //Info("DigiCharge","digi=%p chargedigi=%f charge=%f",digi,digi->GetCharge(), charge);
  //digi->Print();
  return charge;
}

Double_t PndSdsChargeWeightingAlgorithms::DigiChargeError(Int_t digiIndex)
{
  Double_t cherr = DigiCharge(digiIndex);
  cherr *= fChargeConverter->GetRelativeError(cherr); 
  return cherr;
}


Int_t PndSdsChargeWeightingAlgorithms::DigiStripno(Int_t digiIndex)
{
	Int_t strip;
	SensorSide side;
	PndSdsDigiStrip* myDigi = (PndSdsDigiStrip*)fDigiArray->At(digiIndex);
	fCalcStrip->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
  return strip;
}





