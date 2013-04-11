#include "TString.h"
#include "TStopwatch.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TFile.h"
using namespace std;
void MisAlignmentStatStudy()
{
  double dN[10]={100,200,500,1000,2000,5000,10000,20000,50000,100000};
  //translation misalignment only
  // double derrth[10]={4e-4,4e-4,4e-4,4e-4,4e-4,4e-4,4e-4,4e-4,4e-4,4e-4};
  double derrN[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  double dX200[10]={18.2,19.6,9.6,9.5,8.4,12.2,3.4,14.6,1.9,2.1};
  double dY200[10]={85.6,25.6,20.8,21,20.1,3.1,11,2.2,3.7,3.1};
  double dthres200[10]={0.2015,0.2236,0.2077,0.2115,0.2034,0.158,0.1699,0.1572,0.1387,0.1381};
  double derrth200[10]={6e-4,6e-4,6e-4,5e-4,5e-4,5e-4,5e-4,4e-4,3e-4,3e-4};

  double dX100[10]={19.6,15.4,8.3,5.1,4,3.7,2.9,1.8,1.2,0.85};
  double dY100[10]={17.7,12.3,8.3,5.2,3.4,2,1.6,1.4,1.4,0.75};
  double dthres100[10]={0.1517,0.1406,0.1458,0.1422,0.1401,0.1393,0.1381,0.1372,0.1374,0.1376};
  double derrth100[10]={4e-4,3e-4,4e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4};

  double dX50[10]={12,11,7.3,4.8,3.6,2.4,2.8,1.9,1.3,0.89};
  double dY50[10]={12,10,7.8,5.3,3,1.9,1.5,1.5,1.4,0.88};
  double dthres50[10]={0.1502, 0.1407,0.1457,0.1424,0.1386,0.139,0.1386,0.1373,0.137, 0.1377};
  double derrth50[10]={4e-4,3e-4,4e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4};

  double dX20[10]={9.7,8.6,5.3,4.9,3.8,3.8,2.7,1.3,1.3,0.89};
  double dY20[10]={10.8,9.1,4.8,5.2,3.1,3.1,1.7,1.3,1.3,0.88};
  double dthres20[10]={0.1464,0.141,0.1383,0.1424,0.1397,0.139,0.1385,0.1373,0.1372,0.1377 };
  double derrth20[10]={4e-4,4e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4,3e-4};
  
  TFile *fout = new TFile("MisAlignmentStatSudy.root","RECREATE");
  TMultiGraph *mgAC = new TMultiGraph();//misalignment constans
  TMultiGraph *mgTR = new TMultiGraph();//theta resolution

  TGraph *grACX[4];
  TGraph *grACY[4];
  TGraphErrors *grTR[4];
  for(int j=0;j<4;j++){
  // for(int j=0;j<3;j++){
    double *xvar,*yvar,*rthvar,*derrth;
    if(j==0){ 
      xvar = dX200;
      yvar = dY200;
      rthvar = dthres200;
      derrth = derrth200;
    }
    if(j==1){ 
      xvar = dX100;
      yvar = dY100;
      rthvar = dthres100;
      derrth = derrth100;
    }
    if(j==2){ 
      xvar = dX50;
      yvar = dY50;
      rthvar = dthres50;
      derrth = derrth50;
    }
    if(j==3){ 
      xvar = dX20;
      yvar = dY20;
      rthvar = dthres20;
      derrth = derrth20;
    }
    grACX[j] = new TGraph(10,dN,xvar);
    grACX[j]->SetMarkerStyle(21);
    grACX[j]->SetMarkerColor(j+1);
    grACX[j]->SetMarkerSize(1.2);
    mgAC->Add(grACX[j]);

    grACY[j] = new TGraph(10,dN,yvar);
    grACY[j]->SetMarkerStyle(29);
    grACY[j]->SetMarkerColor(j+1);
    grACY[j]->SetMarkerSize(1.2);
    mgAC->Add(grACY[j]);

    grTR[j] = new TGraphErrors(10,dN,rthvar,derrN,derrth);
    grTR[j]->SetMarkerStyle(23);
    grTR[j]->SetMarkerColor(j+1);
    grTR[j]->SetMarkerSize(1.2);
    mgTR->Add(grTR[j]);
  }
  
  //  TCanvas *c1 = new TCanvas("Theta","Theta",900,900);
  TCanvas *c1 = new TCanvas("alignConst","alignment constants");
  mgAC->Draw("AP");
  mgAC->GetXaxis()->SetTitle("Number of events");
  mgAC->GetYaxis()->SetTitle("#Delta, #mum");
  c1->Update();
  c1->Write();

  TCanvas *c2 = new TCanvas("ResTh","#theta resolution");
  mgTR->Draw("AP");
  mgTR->GetXaxis()->SetTitle("Number of events");
  mgTR->GetYaxis()->SetTitle("#theta_{res}, mrad");
  c2->Update();
  c2->Write();

  mgAC->Write();
  mgTR->Write();
  fout->Write();
}
