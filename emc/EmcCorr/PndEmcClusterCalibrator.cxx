//--------------------------------------------------------------------------
// Description:
//	Class PndEmcClusterCalibrator
//      Do an energy and position corrections for EMC cluster
//      PndEmcClusterCalibrator is a factory which call specific method depending on the input parameter "method"
//      method=1 - PndEmcClusterHistCalibrator is called, which perform correction from the interpolation on 2-dimensional histogram on (Energy, theta)
//      method=2 - PndEmcClusterSimpleCalibrator is called, where energy correction is parametrised as a function of (Energy, theta)
//      PndEmcAbsClusterCalibrator - abstract interface class
//
// Versions of the correction: 
// 1 - "emc_module12.dat", "emc_module3new.root", "emc_module4_StraightGeo24.4.root", "emc_module5_fsc.root" (EMC only)
// 2 - "emc_module12.dat", "emc_module3new.root", "emc_module4_StraightGeo24.4.root", "emc_module5_fsc.root" (+ full PANDA geometry)
//
// Author List:
//      D.Melnychuk
//      PndEmcClusterHistCalibrator class is based on code PndEmcMakeCorr.cxx
//      (A. Biegun, M. Babai)
//      PndEmcClusterSimpleCalibrator class is based on EmcPhotonSimpleCalib class in Babar framework (Jan Zhong)
//------------------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcClusterCalibrator.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PndEmcStructure.h"
#include "PndEmcDataTypes.h"

#include "PndEmcMapper.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndEmcClusterCalibrationPar.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------
PndEmcClusterCalibrator::PndEmcClusterCalibrator()
{
}

//--------------
// Destructor --
//--------------
PndEmcClusterCalibrator::~PndEmcClusterCalibrator()
{
}

PndEmcAbsClusterCalibrator *PndEmcClusterCalibrator::MakeEmcClusterCalibrator(Int_t method, Int_t version, TString transportModel)
{
	switch (method) {
		case 1:
			return new PndEmcClusterHistCalibrator(version, transportModel);
			break;
		case 2:
			return new PndEmcClusterSimpleCalibrator(version, transportModel);
			break;
		default :
			cout<<"PndEmcClusterCalibrator:: Method "<<method<<" is not defined. Default PndEmcClusterHistCalibrator is used"<<endl;
			return new PndEmcClusterHistCalibrator(version, transportModel);
	}
	
}


/////////////////////////////////////////////////////////////////
//////////// PndEmcAbsClusterCalibrator /////////////////////////
/////////////////////////////////////////////////////////////////
PndEmcAbsClusterCalibrator::PndEmcAbsClusterCalibrator(Int_t version, TString transportModel)
{
	fVersion = version; 
	fModel = transportModel;
}
		
PndEmcAbsClusterCalibrator::~PndEmcAbsClusterCalibrator()
{
}


/////////////////////////////////////////////////////////////////
//////////// PndEmcClusterHistCalibrator ////////////////////////
/////////////////////////////////////////////////////////////////

PndEmcClusterHistCalibrator::PndEmcClusterHistCalibrator(Int_t version, TString transportModel)
{
	
	fPath = getenv("VMCWORKDIR");
	fPath += "/macro/params/";
 
	TString fileNamePhoton;
	TString fileNameElectron;

	fileNamePhoton.Form("gamma_en_th_corr_%s_%i.root",fModel.Data(),version);
	fileNameElectron.Form("electron_en_th_corr_%s_%i.root",fModel.Data(),version);
	
	fileNamePhoton=fPath+fileNamePhoton;
	fileNameElectron=fPath+fileNameElectron;
	
	fPhoton = new TFile(fileNamePhoton,"READ");
	if (fPhoton->IsZombie())
	{ 
		std::cout<<"EMC cluster correction file "<<fileNamePhoton<<" for photons does not exist for given geometry"<<std::endl;
		abort();
	}
	fElectron = new TFile(fileNameElectron,"READ");
	if (fElectron->IsZombie())
	{
		std::cout<<"EMC cluster correction file "<<fileNameElectron<<" for electrons does not exist for given geometry"<<std::endl;
		abort();
	}
	
	// Names of the histograms in root files
	// Target EMC 
	TString nameEn = "hisEnergyDelta"; 
	TString nameTh = "hisThetaDiff";
	// Shashlyk
	TString nameEn5 = "hisEnergy5Delta";
	TString nameTh5 = "hisTheta5Diff";
	
	// *** GetMean() from: E_cluster/E_MC & Theta_MC-Theta_Cluster
	// Target EMC 
	fHEnPhoton = (TH2F*) fPhoton->Get(nameEn);
	fHThPhoton = (TH2F*) fPhoton->Get(nameTh);
	// Shashlyk
	fHEn5Photon = (TH2F*) fPhoton->Get(nameEn5);
	fHTh5Photon = (TH2F*) fPhoton->Get(nameTh5);
 
	// Target EMC 
	fHEnElectron = (TH2F*) fPhoton->Get(nameEn);
	fHThElectron = (TH2F*) fPhoton->Get(nameTh);
	// Shashlyk
	fHEn5Electron = (TH2F*) fElectron->Get(nameEn5);
	fHTh5Electron = (TH2F*) fElectron->Get(nameTh5);
 	
	
}
		
PndEmcClusterHistCalibrator::~PndEmcClusterHistCalibrator()
{
	fPhoton->Close();
	fElectron->Close();
	delete fPhoton;
	delete fElectron;
}

Double_t PndEmcClusterHistCalibrator::Energy(PndEmcCluster *theCluster, Int_t pid)
{
	Double_t valzEn;
	Bool_t use_interpolation=kTRUE;
	
	Double_t energy=theCluster->energy();
	TVector3 position=theCluster->where();
	Double_t theta=position.Theta();
	Double_t thetaRad=theta*(180./TMath::Pi());
	
	Int_t module=theCluster->GetModule();
 
	switch (pid)
	{
		case 22: // photon
			if (module==5) // shashlyk
				valzEn = GetValueInZ( fHEn5Photon, energy, thetaRad, use_interpolation);
			else 
				valzEn = GetValueInZ( fHEnPhoton, energy, thetaRad, use_interpolation);
			break;
		case 11: // electron
		case -11: // positron
			if (module==5) // shashlyk
				valzEn = GetValueInZ( fHEn5Electron, energy, thetaRad, use_interpolation);
			else 
				valzEn = GetValueInZ( fHEnElectron, energy, thetaRad, use_interpolation);
			break;
		default:
			std::cout<<"Emc cluster correction for pid= "<<pid<<" is not defined. Photon values is used."<<std::endl;
			if (module==5) // shashlyk
				valzEn = GetValueInZ( fHEn5Photon, energy, thetaRad, use_interpolation);
			else 
				valzEn = GetValueInZ( fHEnPhoton, energy, thetaRad, use_interpolation);
	}
	
	Double_t energyCorrected  = energy/valzEn;
	
	return energyCorrected;
}

TVector3 PndEmcClusterHistCalibrator::Where(PndEmcCluster *theCluster, Int_t pid)
{
	Double_t valzTh;
	Bool_t use_interpolation=kTRUE;
	
	Double_t energy=theCluster->energy();
	TVector3 position=theCluster->where();
	Double_t theta=position.Theta()*(180./TMath::Pi());
	Double_t phiRad=position.Phi();
	Double_t mag=position.Mag();
	
	Int_t module=theCluster->GetModule();
		
	switch (pid)
	{
		case 22: // photon
			if (module==5) // shashlyk
				valzTh = GetValueInZ( fHTh5Photon, energy, theta, use_interpolation);
			else 
				valzTh = GetValueInZ( fHThPhoton, energy, theta, use_interpolation);
			break;
			case 11: // electron
			case -11: // positron
				if (module==5) // shashlyk
					valzTh = GetValueInZ( fHTh5Electron, energy, theta, use_interpolation);
				else 
					valzTh = GetValueInZ( fHThElectron, energy, theta, use_interpolation);
			break;
		default:
			std::cout<<"Emc cluster correction for pid= "<<pid<<" is not defined. Photon values is used."<<std::endl;
			if (module==5) // shashlyk
				valzTh = GetValueInZ( fHTh5Photon, energy, theta, use_interpolation);
			else 
				valzTh = GetValueInZ( fHThPhoton, energy, theta, use_interpolation);
	}
	
	Double_t thetaCorrected = valzTh+theta;
	Double_t thetaCorrectedRad = thetaCorrected*(TMath::Pi()/180.);
	
	TVector3 correctedPos;
	correctedPos.SetMagThetaPhi(mag,thetaCorrectedRad,phiRad);

	return correctedPos;
}

//-------------
// Methods   --
//-------------
Int_t 
PndEmcClusterHistCalibrator::FindTheBin(TH2* lookup_table, Float_t value_x, Float_t value_y, Int_t &bin_x, Int_t &bin_y)
{
  bin_x = lookup_table->GetXaxis()->FindBin(value_x);
  bin_y = lookup_table->GetYaxis()->FindBin(value_y);

  if ((bin_x < 1) || (bin_x > lookup_table->GetXaxis()->GetNbins()))
    {
      bin_x = -1; bin_y = -1; 
      return -1;
    }

  if ((bin_y < 1) || (bin_y > lookup_table->GetYaxis()->GetNbins()))
    {
      bin_x = -1; bin_y = -1; 
      return -2;
    }

  return 0; // Success
} 

Double_t 
PndEmcClusterHistCalibrator::GetValueInZ(TH2 *lookup_table, Float_t value_x, Float_t value_y, Bool_t use_interpolation)
{
  // We own the EmcLocMaxInfo objects.  Delete from last time.
  // Clean-up res, We need an empty set to store the results.

  if (use_interpolation)
    {
      //cout<<"use_interpolation = kTRUE "<<endl;
      //
      // Use the interpolarion routine of ROOT:
      // Interpolate approximates the value via bilinear
      // interpolation based on the four nearest bin centers
      // see Wikipedia, Bilinear Interpolation
      // Andy Mastbaum 10/8/2008
      // vaguely based on R.Raja 6-Sep-2008
      //
      //cout<<"value_x = "<< value_x <<", value_y = "<<value_y <<endl;
      
      return (lookup_table->Interpolate(value_x,value_y));
    }
  else
    {
      //cout<<"use_interpolation = kFALSE "<<endl;
      Int_t binx, biny, retval;

      retval = FindTheBin(lookup_table, value_x, value_y, binx, biny);
      if (retval)
	{
	  cout << "<E> Error in FindTheBin, check your table and input values!!!!: " << retval << endl;
	  return 0;
	}

      return (lookup_table->GetBinContent(binx,biny));
    }
  return 0;
}

/////////////////////////////////////////////////////////////////
//////////// PndEmcClusterSimpleCalibrator //////////////////////
/////////////////////////////////////////////////////////////////
PndEmcClusterSimpleCalibrator::PndEmcClusterSimpleCalibrator(Int_t version, TString transportModel)
{
	fPath = getenv("VMCWORKDIR");
	fPath += "/macro/params/";
 
	TString fileName;

	if (version==0)
	{
		fileName="emc_cluster_correction_par_default.root";
	} else
	{	
		fileName.Form("emc_cluster_correction_par_%s_%i.root",fModel.Data(),version);
	}
	
	fileName=fPath+fileName;

	TFile *clusterCalibrationFile = new TFile(fileName.Data());
	if(clusterCalibrationFile->IsZombie()){
		std::cout << "-E- PndEmcClusterSimpleCalibrator: Could not open file " << fileName << " for Emc cluster calibration parameters" << std::endl;
	} else {
		clusterCalibrationFile->GetObject("PndEmcClusterCalibrationParObject",fParObject);
		if(fParObject == NULL){
			std::cout << "-E- PndEmcClusterSimpleCalibrator: Could not get Emc cluster calibration information from file " << fileName<< std::endl;
		}
	}
}

TVector3 PndEmcClusterSimpleCalibrator::Where(PndEmcCluster *clust, Int_t pid)
{
	return clust->where();
}

Double_t PndEmcClusterSimpleCalibrator::Energy(PndEmcCluster *clust, Int_t pid)
{
	if (pid!=22)
	{
		std::cout<<"PndEmcClusterSimpleCalibrator:: EMC Cluster energy correction is not defined for pid="<<pid<<". Photon correction is used"<<std::endl;
	}
	
	Double_t e=clust->energy();
	TVector3 clusterPosition= clust->where();
	Double_t theta_cluster=clusterPosition.Theta();
	
	Double_t e1=e;
	Double_t theta1=theta_cluster;
	
	if ( (clusterPosition.Z() < 180.0)&&(theta_cluster<140.*TMath::Pi()/180.))
	{
		if (e<0.03) e1 = 0.03;
		if (e>8.0)  e1 = 8.0 ;
	}
	
	if ( (clusterPosition.Z() < 180.0)&&(theta_cluster>140.*TMath::Pi()/180.))
	{
		if (e<0.03) e1 = 0.03;
		if (e>2.0)  e1 = 2.0 ;
	}
	
	if (clusterPosition.Z() > 180.0)
	{
		if (e<0.01) e1 = 0.01;
		if (e>16.0)  e1 = 16.0 ;
	}

	Int_t iParSet; // EMC component and energy range (1 - barrel (below 1 GeV, 2 -barrel (above 1 geV, 3 -forward endcap, 4 -backward endcap, 5 -shashlyk) )
	
	if ( clusterPosition.Z() > 500.0)
	{
		iParSet=5;
	}
	else if ( (clusterPosition.Z() < 180.0)&&(theta_cluster>140.*TMath::Pi()/180.))
	{
		iParSet=4;
	}
	else  if ( (clusterPosition.Z() < 180.0)&&(theta_cluster<140.*TMath::Pi()/180.))
	{
		if (e1<1.0)
			iParSet=1;
		else
			iParSet=2;
	}
	else
	{ 
		iParSet=3;
	}
	
	Double_t pars[10];
	fParObject->GetCalibrationPar(iParSet, pars);
	
	double p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;
	double eout;
	
	if (iParSet==5)
	{
		p0=pars[0];
		p1=pars[1];
		p2=pars[2];
		p3=pars[3];
		p4=pars[4];
		eout=(p0-p1/sqrt(e1)+p2/e1+p3/(e1*e1)-p4/(e1*sqrt(e1)))*e;
	} else
	{ 
		p0=pars[0];
		p1=pars[1];
		p2=pars[2];
		p3=pars[3];
		p4=pars[4];
		p5=pars[5];
		p6=pars[6];
		p7=pars[7];
		p8=pars[8];
		p9=pars[9];
		
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
		eout=e* exp(factor1);
	}
	
	return eout;
}

PndEmcClusterSimpleCalibrator::~PndEmcClusterSimpleCalibrator()
{
}
