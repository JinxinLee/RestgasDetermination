#include"TCtrack.h"
#include"StdoutKiller.h"
#include<set>
#include<vector>
#include"TMinuit.h"
#include"TDirectory.h"
#include"TRandom.h"
#include"TCanvas.h"
#include"TGraph.h"
#include"TEllipse.h"
#include"TMatrixT.h"
#include"TVectorT.h"
#include"TAxis.h"
#include"TApplication.h"
#include"TSystem.h"
#include"TF1.h"
#include"TH2.h"




std::set<int> glob_id;
std::vector<TCcluster> glob_clusters;
TRandom globRand(0);

TCtrack::TCtrack(){
  clear();
  canvDraw=NULL;canvDrawClust=NULL;
}

TCtrack::~TCtrack(){
  //std::cout << "dstor " << canvDraw << " " << canvDrawClust << std::endl;
  delete canvDraw;delete canvDrawClust;
}

void glob_fcn(Int_t &npar, Double_t *gin, Double_t &f,
			  Double_t *par, Int_t iflag){
  f=0;
  for(unsigned int i=0;i<glob_clusters.size();++i){
    if(glob_id.count(glob_clusters.at(i).getId())>0
       && glob_clusters.at(i).getFit()){
      f+=glob_clusters.at(i).getChi2(par);  
    }
  }
}

bool TCtrack::fit(int id1,int id2,int id3,int id4,int id5,int id6,int id7,int id8,int id9,int id10){
  if(nClFit()<2) return false;
  glob_id.clear();
  if(id1>=0) glob_id.insert(id1);
  if(id2>=0) glob_id.insert(id2);
  if(id3>=0) glob_id.insert(id3);
  if(id4>=0) glob_id.insert(id4);
  if(id5>=0) glob_id.insert(id5);
  if(id6>=0) glob_id.insert(id6);
  if(id7>=0) glob_id.insert(id7);
  if(id8>=0) glob_id.insert(id8);
  if(id9>=0) glob_id.insert(id9);
  if(id10>=0) glob_id.insert(id10);

  //copy clusters to global variable for static fit function (grrrr)
  glob_clusters.clear();//call destructors (www.cplusplus.com)
  glob_clusters = cl;
  std::sort(glob_clusters.begin(),glob_clusters.end(),TCclusterSortXYZ());
  bool foundFirst = false;
  TVector3 first;
  TVector3 last;
  for(int i=0;i<nCl();++i){
    if((!(foundFirst))&&glob_clusters.at(i).getFit()){
      first = glob_clusters.at(i).posXYZ();
      foundFirst=true;
    }
    if(glob_clusters.at(i).getFit()){
      last = glob_clusters.at(i).posXYZ();
    }
  }
  double startAX,startAY,startBX,startBY;
  TVector3 startDir = last-first;
  startAX=startDir.X()/startDir.Z();
  startAY=startDir.Y()/startDir.Z();
  double startLambda=-1.*first.Z()/startDir.Z();
  startBX=first.X()+startLambda*startDir.X();
  startBY=first.Y()+startLambda*startDir.Y();

  TMinuit *myMinuit = new TMinuit(5);  //initialize TMinuit with a maximum of 4 params
  myMinuit->SetFCN(glob_fcn);
  
  Double_t arglist[10];
  Int_t ierflg = -1;
  
  arglist[0] = 1;
  {
	StdoutKiller killer;
	myMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
  }

  double stepSizeFrac = 0.01;
  // Set starting values and step sizes for parameters
  Double_t vstart[4] = {startAX,startBX,startAY,startBY};//fitArg.ax_s,fitArg.bx_s ,fitArg.ay_s ,fitArg.by_s };
  Double_t step[4] = {0.01,0.01,0.01,0.01};
  {
    StdoutKiller killer;
    myMinuit->mnparm(0, "ax", vstart[0], step[0], 0, 0,ierflg);
    myMinuit->mnparm(1, "bx", vstart[1], step[1], 0,0,ierflg);
    myMinuit->mnparm(2, "ay", vstart[2], step[2], 0,0,ierflg);
    myMinuit->mnparm(3, "by", vstart[3], step[3], 0,0,ierflg);
  }

  // Now ready for minimization step
  arglist[0] = 500;
  arglist[1] = 1.;

  {
    StdoutKiller killer;
    myMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
  }
  
  std::cout << "fit.cxx @@@@@@@@ Minuit fit status (0 is convergence): " << ierflg << std::endl;

  if(ierflg!=0) return false;

  myMinuit->GetParameter(0,ax,dax);
  myMinuit->GetParameter(1,bx,dbx);
  myMinuit->GetParameter(2,ay,day);
  myMinuit->GetParameter(3,by,dby);

  double par[4] = {ax,bx,ay,by};
  chi2=0.;
  NDF=0;

  cl.clear();
  cl = glob_clusters;

  for(unsigned int i=0;i<cl.size();++i){
    if(glob_id.count(glob_clusters.at(i).getId())>0
       && cl.at(i).getFit()){
      cl.at(i).setResid(par);
      chi2+=glob_clusters.at(i).getChi2(par);
      NDF+=glob_clusters.at(i).getNDF();
    }
  }
  NDF-=4;
  delete myMinuit;  
  return true;
}

void TCtrack::draw(bool stop,int _x,int _y,int _w,int _h){
  double x[cl.size()];
  double y[cl.size()];
  double z[cl.size()];
  double xf[cl.size()];
  double yf[cl.size()];
  double zf[cl.size()];
  int index=0;

  TGraph *ampGxz[cl.size()];
  TGraph *ampGyz[cl.size()];
  TEllipse *ellXZ[cl.size()];
  TEllipse *ellYZ[cl.size()];
  double singleX[1];
  double singleY[1];
  double singleZ[1];

  char buf[10];

  for(int i=0;i<cl.size();++i){
    //if(i>0) throw;
    TVector3 point = cl.at(i).posXYZ();
    x[i]=point.X();
    y[i]=point.Y();
    z[i]=point.Z();


    TMatrixT<double> rot(3,3);
    TVector3 dummyV;
    double dummyD;
    TCalign::getInstance()->getConv(cl.at(i).getId(),dummyV,rot,dummyD);
    //rot.Print();
    TMatrixT<double> rotTransp = rot;
    rotTransp.T();
    TVector3 pointErr = cl.at(i).getErr();//still in det coordinates
    //    pointErr.SetXYZ(1.,1.5,2.);
    TMatrixT<double> UVWerrors(3,3);
    UVWerrors[0][0]=pow(pointErr.X(),-2.);
    UVWerrors[1][1]=pow(pointErr.Y(),-2.);
    UVWerrors[2][2]=pow(pointErr.Z(),-2.);
    TMatrixT<double> XYZerrors(3,3);

    XYZerrors = rotTransp*(UVWerrors*rot);

    TMatrixT<double> XZerrors(2,2);
    TMatrixT<double> YZerrors(2,2);
    XZerrors[0][0] = XYZerrors[0][0];
    XZerrors[1][1] = XYZerrors[2][2];
    XZerrors[0][1] = XYZerrors[0][2];
    XZerrors[1][0] = XZerrors[0][1];

    assert(fabs(XZerrors[1][0]-XYZerrors[2][0])<=1.E-7*fabs(XZerrors[1][0]));

    YZerrors[0][0] = XYZerrors[1][1];
    YZerrors[1][1] = XYZerrors[2][2];
    YZerrors[0][1] = XYZerrors[1][2];
    YZerrors[1][0] = YZerrors[0][1];

    assert(fabs(YZerrors[1][0]-XYZerrors[2][1])<=1.E-7*fabs(YZerrors[1][0]));

    //calculate eigenvalues and eigenvectors
    TVectorT<double> EVAXZ(2);
    TMatrixT<double> EVEXZ(2,2);
    EVEXZ=XZerrors.EigenVectors(EVAXZ);
    TVectorT<double> EVAYZ(2);
    TMatrixT<double> EVEYZ(2,2);
    EVEYZ=YZerrors.EigenVectors(EVAYZ);
    double thetaXZ,thetaYZ;

    
    thetaXZ=180./TMath::Pi() * TMath::ATan2(EVEXZ[0][0],EVEXZ[0][1]);

    //calculate ellipse radii (Halbachsen)
    double rE1XZ=pow(EVAXZ[0],-0.5);
    double rE2XZ=pow(EVAXZ[1],-0.5);

    thetaYZ=180./TMath::Pi() * TMath::ATan2(EVEYZ[0][0],EVEYZ[0][1]);


    //calculate ellipse radii
    double rE1YZ=pow(EVAYZ[0],-0.5);
    double rE2YZ=pow(EVAYZ[1],-0.5);
    
    

    ellXZ[i] = new TEllipse(point.Z(),point.X(),rE1XZ,rE2XZ,0.,360.,thetaXZ);
    //sprintf(buf,"c%5.5f",globRand.Uniform());
    //ellXZ[i]->SetName(buf);
    ellXZ[i]->SetLineColor(12);
    ellXZ[i]->SetFillStyle(0);
    ellYZ[i] = new TEllipse(point.Z(),point.Y(),rE1YZ,rE2YZ,0.,360.,thetaYZ);
    //sprintf(buf,"c%5.5f",globRand.Uniform());
    //ellYZ[i]->SetName(buf);
    ellYZ[i]->SetLineColor(12);
    ellYZ[i]->SetFillStyle(0);

    singleX[0]=point.X();
    singleY[0]=point.Y();
    singleZ[0]=point.Z();
    ampGxz[i]=new TGraph(1,singleZ,singleX);
    sprintf(buf,"c%5.5f",globRand.Uniform());
    ampGxz[i]->SetName(buf);
    ampGyz[i]=new TGraph(1,singleZ,singleY);
    sprintf(buf,"c%5.5f",globRand.Uniform());
    ampGyz[i]->SetName(buf);

    if(cl.at(i).getFit()){
      xf[index]=point.X();
      yf[index]=point.Y();
      zf[index]=point.Z();      
      ++index;
    }
  }

  double minAmp=1.E10;
  double maxAmp=-1.E10;
  for(int i=0;i<cl.size();++i){
    if(cl.at(i).getAmp()>maxAmp) maxAmp=cl.at(i).getAmp();
    if(cl.at(i).getAmp()<minAmp) minAmp=cl.at(i).getAmp();
  }
  double maxMarkerSize = 4.;
  double minMarkerSize = 1.5;

  for(int i=0;i<cl.size();++i){
    ampGxz[i]->SetMarkerStyle(24);
    ampGyz[i]->SetMarkerStyle(24);
    ampGxz[i]->SetMarkerSize(minMarkerSize+(maxMarkerSize-minMarkerSize)*(cl.at(i).getAmp()-minAmp)/(maxAmp-minAmp));
    ampGyz[i]->SetMarkerSize(minMarkerSize+(maxMarkerSize-minMarkerSize)*(cl.at(i).getAmp()-minAmp)/(maxAmp-minAmp));
  }

  //TRandom r((UInt_t)((this))+nCl()+(int)(gRandom->Uniform()*1000.));
  char bufFormula[50];
  sprintf(buf,"c%5.5f",globRand.Uniform());
  delete canvDraw;
  canvDraw = new TCanvas(buf,"TCtrack visualization",_x,_y,_w,_h);
  //  new TCanvas(buf,,_x,_y,_w,_h);

  TGraph* gxz = new TGraph(cl.size(),z,x);
  sprintf(buf,"c%5.5f",globRand.Uniform());
  gxz->SetName(buf);
  TGraph* gyz = new TGraph(cl.size(),z,y);
  sprintf(buf,"c%5.5f",globRand.Uniform());
  gyz->SetName(buf);
  TGraph* gfxz = new TGraph(nClFit(),zf,xf);
  sprintf(buf,"c%5.5f",globRand.Uniform());
  gfxz->SetName(buf);
  TGraph* gfyz = new TGraph(nClFit(),zf,yf);
  sprintf(buf,"c%5.5f",globRand.Uniform());
  gfyz->SetName(buf);

  sprintf(buf,"c%5.5f",globRand.Uniform());
  sprintf(bufFormula,"%f*x+%f",ax,bx);
  TF1 *fxz = new TF1(buf,bufFormula,-10000.,10000.);
  TF1 *MCfxz;
  if(isMC){
    sprintf(buf,"c%5.5f",globRand.Uniform());
    sprintf(bufFormula,"%f*x+%f",MCax,MCbx);
    MCfxz = new TF1(buf,bufFormula,-10000.,10000.);
    MCfxz->SetLineColor(kBlue);
    MCfxz->SetLineWidth(2);
    MCfxz->SetLineStyle(3);
  }
  sprintf(buf,"c%5.5f",globRand.Uniform());
  sprintf(bufFormula,"%f*x+%f",ay,by);
  TF1 *fyz = new TF1(buf,bufFormula,-10000.,10000.);
  TF1 *MCfyz;
  if(isMC){
    sprintf(buf,"c%5.5f",globRand.Uniform());
    sprintf(bufFormula,"%f*x+%f",MCay,MCby);
    MCfyz = new TF1(buf,bufFormula,-10000.,10000.);
    MCfyz->SetLineColor(kBlue);
    MCfyz->SetLineWidth(2);
    MCfyz->SetLineStyle(3);
  }

  TH2D *hxz;
  TH2D *hyz;
  if(customRange){
    sprintf(buf,"c%5.5f",globRand.Uniform());
    hxz=new TH2D(buf,"",2,rangeZ1,rangeZ2,2,rangeX1,rangeX2);
    hxz->SetStats(kFALSE);
    sprintf(buf,"c%5.5f",globRand.Uniform());
    hyz=new TH2D(buf,"",2,rangeZ1,rangeZ2,2,rangeY1,rangeY2);
    hyz->SetStats(kFALSE);
  }

  canvDraw->Divide(2,1);
  canvDraw->cd(1);
  gxz->SetTitle("");
  if(customRange){
    hxz->GetYaxis()->SetTitle("x in cm");
    hxz->GetXaxis()->SetTitle("z in cm");
    hxz->Draw();
  }
  else{
    gxz->GetYaxis()->SetTitle("x in cm");
    gxz->GetXaxis()->SetTitle("z in cm");
  }
  gxz->SetMarkerStyle(20);
  gxz->SetMarkerSize(1.);
  gfxz->SetMarkerStyle(20);
  gfxz->SetMarkerSize(.8);
  gfxz->SetMarkerColor(kGreen);
  if(customRange){
    gxz->Draw("P");
  }
  else{
    gxz->Draw("AP");
  }
  
  for(int i=0;i<cl.size();++i){
    ampGxz[i]->Draw("P");
  }
  gfxz->Draw("P");
  fxz->SetLineWidth(1.);
  fxz->Draw("same");
  if(isMC)MCfxz->Draw("same");
  for(int i=0;i<cl.size();++i){
    ellXZ[i]->Draw("same");
  }

  canvDraw->cd(2);
  gyz->SetTitle("");
  if(customRange){
    hyz->GetYaxis()->SetTitle("y in cm");
    hyz->GetXaxis()->SetTitle("z in cm");
    hyz->Draw();
  }
  else{
    gyz->GetYaxis()->SetTitle("y in cm");
    gyz->GetXaxis()->SetTitle("z in cm");
  }
  gyz->SetMarkerStyle(20);
  gyz->SetMarkerSize(1.);
  gfyz->SetMarkerStyle(20);
  gfyz->SetMarkerSize(.8);
  gfyz->SetMarkerColor(kGreen);
  if(customRange){
    gyz->Draw("P");
  }
  else{
    gyz->Draw("AP");
  }
  
  for(int i=0;i<cl.size();++i){
    ampGyz[i]->Draw("P");
  }
  gfyz->Draw("P");
  fyz->SetLineWidth(1.);
  fyz->Draw("same");
  if(isMC)MCfyz->Draw("same");
  for(int i=0;i<cl.size();++i){
    ellYZ[i]->Draw("same");
  }


  gApplication->SetReturnFromRun(kTRUE);
  if(stop){
    gSystem->Run();
  }


}


void TCtrack::drawClustering(bool stop,int _x,int _y,int _w,int _h){
  sort();
  //if customRange wasnt set, define by needed space
  
  if(!customRange){
    double minX=1.E10;
    double minY=1.E10;
    double minZ=1.E10;
    double maxX=-1.E10;
    double maxY=-1.E10;
    double maxZ=-1.E10;
    for(int i=0;i<nCl();++i){
      for(int j=0;j<cl.at(i).nRaw();++j){
	TVector3 raw = cl.at(i).getRaw(j).posXYZ();
	if(raw.X()<minX)minX=raw.X();
	if(raw.Y()<minY)minY=raw.Y();
	if(raw.Z()<minZ)minZ=raw.Z();
	if(raw.X()>maxX)maxX=raw.X();
	if(raw.Y()>maxY)maxY=raw.Y();
	if(raw.Z()>maxZ)maxZ=raw.Z();
      }
    }
    if(fabs(maxX-minX)<1.E-6)minX*=0.95;
    if(fabs(maxY-minY)<1.E-6)minY*=0.95;
    if(fabs(maxZ-minZ)<1.E-6)minZ*=0.95;
    double xL,yL,zL;
    xL=maxX-minX;
    yL=maxY-minY;
    zL=maxZ-minZ;
    static double more=0.1;
    rangeX1=minX-more*xL;rangeX2=maxX+more*xL;
    rangeY1=minY-more*yL;rangeY2=maxY+more*yL;
    rangeZ1=minZ-more*zL;rangeZ2=maxZ+more*zL;
  }

  //TRandom r((UInt_t)((this))+nCl()+(int)(gRandom->Uniform()*1000.));
  char buf[10];
  sprintf(buf,"c%5.5f",globRand.Uniform());
  delete canvDrawClust;
  canvDrawClust = new TCanvas(buf,"TCtrack clustering visualization",_x,_y,_w,_h);
  TH2D *hxz;
  TH2D *hyz;
  sprintf(buf,"c%5.5f",globRand.Uniform());
  hxz=new TH2D(buf,"",2,rangeZ1,rangeZ2,2,rangeX1,rangeX2);
  hxz->SetStats(kFALSE);
  sprintf(buf,"c%5.5f",globRand.Uniform());
  hyz=new TH2D(buf,"",2,rangeZ1,rangeZ2,2,rangeY1,rangeY2);
  hyz->SetStats(kFALSE);
  
  TGraph* gClXZ[nCl()];
  TGraph* gRawXZ[nCl()];
  TGraph* gClYZ[nCl()];
  TGraph* gRawYZ[nCl()];
  
  for(int i=0;i<nCl();++i){
    TCcluster _c = getCl(i);
    double xCl[1];
    double yCl[1];
    double zCl[1];
    xCl[0]=_c.posXYZ().X();
    yCl[0]=_c.posXYZ().Y();
    zCl[0]=_c.posXYZ().Z();
    gClXZ[i] = new TGraph(1,zCl,xCl);
    sprintf(buf,"c%5.5f",globRand.Uniform());
    gClXZ[i]->SetName(buf);
    gClYZ[i] = new TGraph(1,zCl,yCl);
    sprintf(buf,"c%5.5f",globRand.Uniform());   
    gClYZ[i]->SetName(buf);
    double xRaw[_c.nRaw()];
    double yRaw[_c.nRaw()];
    double zRaw[_c.nRaw()];
    for(int j=0;j<_c.nRaw();++j){
      TCcluster _r = _c.getRaw(j);
      xRaw[j]=_r.posXYZ().X();
      yRaw[j]=_r.posXYZ().Y();
      zRaw[j]=_r.posXYZ().Z();
    }
    gRawXZ[i] = new TGraph(_c.nRaw(),zRaw,xRaw);
    sprintf(buf,"c%5.5f",globRand.Uniform());
    gRawXZ[i]->SetName(buf);
    gRawYZ[i] = new TGraph(_c.nRaw(),zRaw,yRaw);
    gRawYZ[i]->SetName(buf);
  }
  
  for(int i=0;i<nCl();++i){
    int rest = i%3;
    gClXZ[i]->SetMarkerSize(1.2);
    gRawXZ[i]->SetMarkerSize(0.8);
    gClYZ[i]->SetMarkerSize(1.2);
    gRawYZ[i]->SetMarkerSize(0.8);
    int R = (i%5)+2;
    int Rgr = (i%5)+12;
    gClXZ[i]->SetMarkerColor(R);
    gRawXZ[i]->SetMarkerColor(R);
    gClYZ[i]->SetMarkerColor(R);
    gRawYZ[i]->SetMarkerColor(R);    
    if(!(getCl(i).getFit())){
      gClXZ[i]->SetMarkerColor(Rgr);
      gRawXZ[i]->SetMarkerColor(Rgr);
      gClYZ[i]->SetMarkerColor(Rgr);
      gRawYZ[i]->SetMarkerColor(Rgr);
    }
    gClXZ[i]->SetMarkerStyle(24);
    gRawXZ[i]->SetMarkerStyle(5);
    gClYZ[i]->SetMarkerStyle(24);
    gRawYZ[i]->SetMarkerStyle(5);

    /*
    if(getCl(i).getFit()){
      gClXZ[i]->SetMarkerColor(2);
      gRawXZ[i]->SetMarkerColor(2);
      gClYZ[i]->SetMarkerColor(2);
      gRawYZ[i]->SetMarkerColor(2);
    }
    else{
      gClXZ[i]->SetMarkerColor(12);
      gRawXZ[i]->SetMarkerColor(12);
      gClYZ[i]->SetMarkerColor(12);
      gRawYZ[i]->SetMarkerColor(12);
    }
    switch(rest){
    case 0:
      gClXZ[i]->SetMarkerStyle(20);
      gRawXZ[i]->SetMarkerStyle(24);
      gClYZ[i]->SetMarkerStyle(20);
      gRawYZ[i]->SetMarkerStyle(24);
      break;
    case 1:
      gClXZ[i]->SetMarkerStyle(21);
      gRawXZ[i]->SetMarkerStyle(25);
      gClYZ[i]->SetMarkerStyle(21);
      gRawYZ[i]->SetMarkerStyle(25);
      break;
    case 2:
      gClXZ[i]->SetMarkerStyle(22);
      gRawXZ[i]->SetMarkerStyle(26);
      gClYZ[i]->SetMarkerStyle(22);
      gRawYZ[i]->SetMarkerStyle(26);
      break;
    }
    */
  }

  canvDrawClust->Divide(2,1);
  canvDrawClust->cd(1);
  hxz->GetYaxis()->SetTitle("x in cm");
  hxz->GetXaxis()->SetTitle("z in cm");
  hxz->Draw();
  for(int i=0;i<nCl();++i){
    gClXZ[i]->Draw("P");
    gRawXZ[i]->Draw("P");
  }

  canvDrawClust->cd(2);
  hyz->GetYaxis()->SetTitle("y in cm");
  hyz->GetXaxis()->SetTitle("z in cm");
  hyz->Draw();
  for(int i=0;i<nCl();++i){
    gClYZ[i]->Draw("P");
    gRawYZ[i]->Draw("P");
  }

  gApplication->SetReturnFromRun(kTRUE);
  if(stop){
    gSystem->Run();
  }
  

}
void TCtrack::useTrivialClustering(){
  std::vector<TCcluster> newCl;
  for(unsigned int i=0;i<nCl();++i){
    TCcluster _c = cl.at(i);
    for(unsigned int j=0;j<_c.nRaw();++j){
      TCcluster __c = _c.getRaw(j);
      if(_c.getFit()){
	__c.setFit(true);
      }
      newCl.push_back(__c);
    }
  }
  clear();
  cl=newCl;
}


ClassImp(TCtrack)
