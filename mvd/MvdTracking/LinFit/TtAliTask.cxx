// Simone Bianco 14/07/2010

// This Class' Header ------------------
#include "TtAliTask.h"
#include "TtFitRes.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"

//#include "PndLinTrack.h"
#include "../../../pnddata/TrackData/PndTrackCand.h"
#include "../../../pnddata/SdsData/PndSdsHit.h"
#include "../../../pnddata/MvdData/PndMvdHit.h"
#include "../../../pnddata/TrackData/PndTrackCandHit.h"

#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TVector2.h"

// Fit Classes -----------
#include <TMath.h>
#include <TVector3.h>
#include <TRandom2.h>
//#include <TStyle.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <TVirtualFitter.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

#include <fstream>

using namespace ROOT::Math;
using namespace std;

TtAliTask::TtAliTask()
  : FairTask("Alignment")
{
  fTCandBranchName = "MVDHitsStrip";

  for (Int_t gg = 0 ; gg < 4 ; gg++)
    {
      sX[gg] = 0.;
      sY[gg] = 0.;
      sigX[gg] = 0.;
      sigY[gg] = 0.;
      m_X[gg] = 0.;
      m_Y[gg] = 0.;
    }

  fPrint = 0;
}

TtAliTask::TtAliTask(Int_t ExcludedBox)
  : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = "MVDHitsStrip";
 
  if (ExcludedBox < 1 || ExcludedBox > 6) 
    {
      std::cout << "Excluded box: Wrong value, setting as default 2!" << std::endl; 
      ExcludedBox = 2;
    }

  fExclBox = ExcludedBox;

  for (Int_t gg = 0 ; gg < 4 ; gg++)
    {
      sX[gg] = 0.;
      sY[gg] = 0.;
      sigX[gg] = 0.;
      sigY[gg] = 0.;
      m_X[gg] = 0.;
      m_Y[gg] = 0.;
    }

  fPrint = 0;
}

TtAliTask::TtAliTask(Int_t ExcludedBox,Double_t X1,Double_t Y1,Double_t X2,Double_t Y3,Double_t X4,Double_t Y5,Double_t X6,Double_t Y6)
  : FairTask("Alignment")
{
  fTCandBranchName = "MVDHitsStrip";
 
  if (ExcludedBox < 1 || ExcludedBox > 6) 
    {
      std::cout << "Excluded box: Wrong value, setting as default 2!" << std::endl; 
      ExcludedBox = 2;
    }

  fExclBox = ExcludedBox;

  sX[0] = X1;
  sY[0] = Y1;
  sX[1] = X2;
  sY[1] = Y3;
  sX[2] = X4;
  sY[2] = Y5;
  sX[3] = X6;
  sY[3] = Y6;
  
 for (Int_t gg = 0 ; gg < 4 ; gg++)
    {
      sigX[gg] = 0.;
      sigY[gg] = 0.;
      m_X[gg] = 0.;
      m_Y[gg] = 0.;
    }

  fPrint = 0;
}


TtAliTask::~TtAliTask()
{

}

InitStatus TtAliTask::Init()
{
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("TtAliTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  //Get input collection
  
  fTCandArray=(TClonesArray*) ioman->GetObject(fTCandBranchName);

  if(fTCandArray==0)
    {
      Error("TtAliTask::Init","trackcand-array not found!");
      return kERROR;
    }
  
  std::cout << "-I- TtAliTask: Initialisation successfull" << std::endl;

  fEvent = 0;

  hx = new TH1F("hx","hx",10000,-1.,+1.);
  hy = new TH1F("hy","hy",10000,-1.,+1.);

  return kSUCCESS;
}


void TtAliTask::Exec(Option_t* opt)
{

  //  std::cout << "Event: " << fEvent << std::endl;

  Int_t ntcand=fTCandArray->GetEntriesFast();

  if (ntcand == 6)
    {
      
      //  std::cout << "Entries: " << ntcand << std::endl;

 
      std::map<Double_t,TString> SensorsPos;

       
      //    std::cout<<"TtAliTask::Exec"<<std::endl;
      // Reset output Array
      if(fTCandArray==0) Fatal("TtAliTask::Exec","No Points");
   



      TString name;

    
      // Detailed output
      //   std::cout<<" -I- TtAliTask: contains "<<ntcand<<" TCandidates"<<std::endl;
	
      //std::cout<< " Detailed Debug info on the candidates:"<<std::endl;
      unsigned int detid=12345, index=12345;
      for(Int_t itr=0;itr<ntcand;++itr){

	PndMvdHit *theHit = (PndMvdHit*)   fTCandArray->At(itr);
	
	index = theHit->GetRefIndex();
	detid = theHit->GetDetectorID();

	name = theHit->GetDetectorID();
	

	if (SensorsPos.size() < 6) SensorsPos[theHit->GetZ()] = name;

	  
      }
    

      //  const Int_t sizeMap = Sensors.size();
      
      const Int_t sizeMap = SensorsPos.size();
  
      TString DetNames[sizeMap];
      Double_t Pos[sizeMap];
      Int_t jj = 0;

      if (SensorsPos.size()>=6)
	{
	  //std::cout << "Number of Sensors:" << SensorsPos.size() << std::endl;
	  for (std::map<Double_t,TString>::iterator it=SensorsPos.begin();it!=SensorsPos.end();++it)
	    {
	      if (fEvent < 5) std::cout << "position: " << it->first << " name: " << (it->second) << std::endl;
	  
	      DetNames[jj] = it->second;
	      Pos[jj] = it -> first;
	      jj++;
	    }

	}

  
 
     
      if (fEvent < 5)
	{
      
	  for (Int_t l = 0 ; l < sizeMap ; l++)
	    {
	      std::cout << DetNames[l] << std::endl;
	    }
	}
    

      ///////////////
      
      
      if (SensorsPos.size()==6)
	{
	  Int_t DetRem[sizeMap-1];
	  for (Int_t ss = 0 ; ss < (sizeMap-1) ; ss++)
	    {

	      DetRem[ss] = 0;

	    }
  
  
	  Int_t counter = 0;

	  Double_t BuffZ = -9999.;

	  for (Int_t kk = 0 ; kk < (sizeMap) ; kk++)
	    {

	      if ((fExclBox-1) == kk) 
		{
		  BuffZ = Pos[kk];
		  continue;
		}
	      else
		{
		  DetRem[counter] = kk;
		  counter++;
		}
      
	    }
  

	  Double_t x[6];
	  Double_t y[6];
	  Double_t z[6];

	  Double_t Erx[6];
	  Double_t Ery[6];
	  Double_t Erz[6];

	  Int_t track = 0;

      
	  Double_t RealX = -1999999., RealY = -1999999;
      

	  Double_t buffErrX = -0.099999;
	  Double_t buffErrY = -0.099999;

	
	  Int_t ihit = 0;

	  for (Int_t it1 = 0 ; it1 < ntcand ; it1++)
	    
	    {
	
	      PndMvdHit *theHit = (PndMvdHit*)   fTCandArray->At(it1);
		
	      if ((theHit->GetDetectorID()) == DetNames[0])
		{
		  x[0] = theHit->GetX();
		  y[0] = theHit->GetY();
		  z[0] = theHit->GetZ();
		  Erx[0] = theHit->GetDx();
		  Ery[0] = theHit->GetDy();
		  Erz[0] = theHit->GetDz();
		}

	      if ((theHit->GetDetectorID()) == DetNames[1])
		{
		  x[1] = theHit->GetX();
		  y[1] = theHit->GetY();
		  z[1] = theHit->GetZ();
		  Erx[1] = theHit->GetDx();
		  Ery[1] = theHit->GetDy();
		  Erz[1] = theHit->GetDz();
		}
	      if ((theHit->GetDetectorID()) == DetNames[2])
		{
		  x[2] = theHit->GetX();
		  y[2] = theHit->GetY();
		  z[2] = theHit->GetZ();
		  Erx[2] = theHit->GetDx();
		  Ery[2] = theHit->GetDy();
		  Erz[2] = theHit->GetDz();
		}

	      if ((theHit->GetDetectorID()) == DetNames[3])
		{
		  x[3] = theHit->GetX();
		  y[3] = theHit->GetY();
		  z[3] = theHit->GetZ();
		  Erx[3] = theHit->GetDx();
		  Ery[3] = theHit->GetDy();
		  Erz[3] = theHit->GetDz();
		}
	      if ((theHit->GetDetectorID()) == DetNames[4])
		{
		  x[4] = theHit->GetX();
		  y[4] = theHit->GetY();
		  z[4] = theHit->GetZ();
		  Erx[4] = theHit->GetDx();
		  Ery[4] = theHit->GetDy();
		  Erz[4] = theHit->GetDz();
		}

	      if ((theHit->GetDetectorID()) == DetNames[5])
		{
		  x[5] = theHit->GetX();
		  y[5] = theHit->GetY();
		  z[5] = theHit->GetZ();
		  Erx[5] = theHit->GetDx();
		  Ery[5] = theHit->GetDy();
		  Erz[5] = theHit->GetDz();
		}
	      
	      ihit++;
	      
		
	    } // end loop on points
	  
	  
	  // setting the points

	  
	  
	  Int_t tx = 0, ty = 0;

	  Int_t uu = 0;

	  // to check alignment...

	  for (Int_t ww = 0 ; ww < 6 ; ww++)
	    {
	      if (TMath::Abs(Erx[ww]) < 0.5) 
		{
		  x[ww]=x[ww]+sX[tx];
		  tx++;
		}
	      if (TMath::Abs(Ery[ww]) < 0.5) 
		{
		  y[ww]=y[ww]+sY[ty];
		  ty++;
		}
	    }


	  for (Int_t ww = 0 ; ww < 6 ; ww++)
	    {
	      if ((fExclBox-1)!=ww)
		{
		  if (TMath::Abs(Erx[ww]) > 0.5) Erx[ww] = 1000000;
 		  if (TMath::Abs(Ery[ww]) > 0.5) Ery[ww] = 1000000;
		  uu++;
		}

	      //	      std::cout << "(" << x[ww] << "," <<  y[ww] << "," <<  z[ww] << ")" << std::endl;

	    }

	 
 	  Double_t DX,DY;


 	  MyFit(x,y,z,Erx,Ery,Erz,x[fExclBox-1],y[fExclBox-1],z[fExclBox-1],DX,DY);	

 	  Double_t pointX,pointY,pointZ;
				
 	  hx->Fill(DX);
 	  hy->Fill(DY);
      
	}// if 6 hits
      
      fEvent++;
      
    }
  
  return;
   
}


void TtAliTask::MyFit(Double_t *x,Double_t *y,Double_t *z,Double_t *Erx,Double_t *Ery,Double_t *Erz,Double_t realX, Double_t realY, Double_t realZ, Double_t &DELTAX, Double_t &DELTAY)
{

  TGraphErrors grX;
  TGraphErrors grY;

  Int_t ix=0,iy=0;

  for (Int_t j = 0 ; j < 6 ; j++)
    {
      //std::cout << j << "@(" << Erx[j] << "," <<  Ery[j] << "," <<  Erz[j] << ")" << std::endl;
      if (j == fExclBox-1) continue;

      if (TMath::Abs(Erx[j])<0.5) 
	//if (j==0 || j==1 || j==3 || j==5)
      {
	grX.SetPoint(ix,z[j],x[j]);
	ix++;
      }
      //if(j==0 || j==2 || j==4 || j==5)
      if (TMath::Abs(Ery[j])<0.5) 
      {
	grY.SetPoint(iy,z[j],y[j]);
	iy++;
      }

    }

  //  std::cout << "POINTS  " << ix << "   " << iy << std::endl;


  Double_t mx,nx,my,ny;

  grX.Fit("pol1","Q");
  
  nx = (grX.GetFunction("pol1"))->GetParameter(0);
  mx = (grX.GetFunction("pol1"))->GetParameter(1);


  grY.Fit("pol1","Q");
  
  ny = (grY.GetFunction("pol1"))->GetParameter(0);
  my = (grY.GetFunction("pol1"))->GetParameter(1);

  Double_t buffX, buffY;

  buffX = mx * realZ + nx;
  buffY = my * realZ + ny;

  DELTAX = buffX - realX;
  DELTAY = buffY - realY;



}




void TtAliTask::FinishTask()
{


  Double_t mX,mY,siX,siY;

  TF1 *fun = new TF1("fun","gaus");

  hx->Fit(fun,"Q","",(hx->GetMean() - 2*(hx->GetRMS())),(hx->GetMean() + 2*(hx->GetRMS())));

  mX = fun->GetParameter(1);
  siX = fun->GetParameter(2);

  cout << "RMS: " << hx->GetRMS() << " sigma " << siX << endl;

  hy->Fit(fun,"Q","",(hy->GetMean() - 2*(hy->GetRMS())),(hy->GetMean() + 2*(hy->GetRMS())));

  mY = fun->GetParameter(1);
  siY = fun->GetParameter(2);
  
  switch(fExclBox)
      {
      case 1:
	sX[0] += mX;
	sY[0] += mY;
	m_X[0] = mX;
	m_Y[0] = mY;
	sigX[0] = siX;
	sigY[0] = siY;
	break;
	
      case 2:
	sX[1] += mX;
	sigX[1] = siX;
	m_X[1] = mX;
	
	break;

      case 3:
	sY[1] += mY;	
	sigY[1] = siY;
	m_Y[1] = mY;
	break;

      case 4:
	sX[2] += mX;
	sigX[2] = siX;
	m_X[2] = mX;
	break;

      case 5:
	sY[2] += mY;	
	sigY[2] = siY;
	m_Y[2] = mY;
	break;

      case 6:
	sX[3] += mX;
	sY[3] += mY;
	sigX[3] = siX;
	sigY[3] = siY;
	m_X[3] = mX;
	m_Y[3] = mY;
	break;
      }


  if (fPrint == 1)
    {
      PrintHistos();
    }

  hx->Reset();
  hy->Reset();

  //  fEvent = 0;
}


void TtAliTask::PrintVal()
{

  cout << "SHIFTS" << endl;
  for (Int_t k = 0 ; k < 4 ; k++)
    {
      cout << "X: " << sX[k] << "   Y: " << sY[k] << endl;
    }
}




void TtAliTask::PrintMeanResiduals()
{

  cout << "RESIDUALS" << endl;
  for (Int_t k = 0 ; k < 4 ; k++)
    {
      cout << "X: " << m_X[k] << "   Y: " << m_Y[k] << endl;
    }
}

void TtAliTask::PrintSigmaResiduals()
{

  cout << "SIGMA-RESIDUALS" << endl;
  for (Int_t k = 0 ; k < 4 ; k++)
    {
      cout << "sigX: " << sigX[k] << "   sigY: " << sigY[k] << endl;
    }
}

TVector2  TtAliTask::GetRes()
{
  
  Double_t resx = TMath::Power((sigX[0]*sigX[1]*sigX[2]*sigX[3]),1./4.);
  Double_t resy = TMath::Power((sigY[0]*sigY[1]*sigY[2]*sigY[3]),1./4.);
  
  return TVector2(resx,resy);

}

void TtAliTask::PrintHistos()
{

  TCanvas *can = new TCanvas();

  can->cd();

  hx->Draw();

  hx->GetXaxis()->SetRangeUser((hx->GetMean())-8*(hx->GetRMS()),(hx->GetMean())+8*(hx->GetRMS()));

  std::string nameX = Form("HistResidualsX_%d.png",fExclBox);

  can->Print(nameX.c_str(),"png");


  hy->Draw();

  hy->GetXaxis()->SetRangeUser((hy->GetMean())-8*(hy->GetRMS()),(hy->GetMean())+8*(hy->GetRMS()));
 
  std::string nameY = Form("HistResidualsY_%d.png",fExclBox);

  can->Print(nameY.c_str(),"png");
  

}



ClassImp(TtAliTask);
