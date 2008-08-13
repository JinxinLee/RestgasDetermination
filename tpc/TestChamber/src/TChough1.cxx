#include "TChough1.h"
#include<cmath>

#include"TGraph.h"
#include"TCanvas.h"
#include"TApplication.h"
#include"TSystem.h"
#include"TRandom.h"
#include"TEllipse.h"

TChough1::TChough1(const TVector3& _yp,const TVector3& _zp,double _r1,double _r2) : TCabsHough(_yp,_zp){
  r1Y=_r1;
  r2Y=_r2;


  minAY = -5.;
  maxAY = 5.;
  minBY = -1.;
  maxBY = 1.;
  
  nBinsAY = 50;
  nBinsBY = 50;

  canvas1 = NULL;
  houghHistoYZ=NULL;
  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLinesYZ[i]=NULL;
  }


}

TChough1::~TChough1(){
}

void TChough1::draw(bool stop,int _x,int _y,int _w,int _h){
  double x[ypHit.size()];
  double y[ypHit.size()];
  for(int i=0;i<ypHit.size();++i){
    x[i]=zpHit.at(i);
    y[i]=ypHit.at(i);
  }

  static TRandom r(0);
  char buf[10];
  sprintf(buf,"c%5.5f",r.Uniform());
  TCanvas *c = new TCanvas(buf,"Hough Transformation",_x,_y,_w,_h);
  TGraph* g = new TGraph(ypHit.size(),x,y);

  c->Divide(1,2);
  c->cd(1);
  g->SetTitle("");
  g->GetXaxis()->SetTitle("z");
  g->SetMarkerStyle(24);
  g->SetMarkerSize(2.);
  g->Draw("AP");

  std::vector<int> selHits;
  for(int i=0;i<ypHit.size();++i){
    if(hot(i)){
      selHits.push_back(i);
      houghLinesYZ[i]->SetLineStyle(1);
      houghLinesYZ[i]->SetLineWidth(1);
      houghLinesYZ[i]->SetLineColor(kGreen);
    }
  }

  double xsel[selHits.size()];
  double ysel[selHits.size()];
  for(int i=0;i<selHits.size();++i){
    xsel[i]=zpHit.at(selHits.at(i));
    ysel[i]=ypHit.at(selHits.at(i));
  }
  TGraph* gsel = new TGraph(selHits.size(),xsel,ysel);
  gsel->SetMarkerStyle(20);
  gsel->SetMarkerColor(kGreen);
  gsel->Draw("P");


  c->cd(2);
  houghHistoYZ->Draw("colz");

  double r1=getScaleA()*r1Y;
  double r2=getScaleB()*r2Y;


  TEllipse* ell = new TEllipse(aymax,bymax,r1,r2);
  ell->SetFillStyle(0);
  ell->SetLineWidth(4);
  ell->Draw("same");
  for(int i=0;i<NumberOfHits;++i){
    houghLinesYZ[i]->Draw("same");
  }

  if(stop){
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
  }

}

void TChough1::doHough(){
  cleanup();
  NumberOfHits = nHits();
  assert(NumberOfHits<=Max_NumberOfHits);
  for(int i=0;i<NumberOfHits;i++) {

    HitCoordinates[i][0] = ypHit.at(i);
    HitCoordinates[i][1] = zpHit.at(i);

  }

  makeHoughLines();
  makeHoughHisto();

  findMaxInHisto(aymax,bymax);

}

bool TChough1::hot(int index){
  assert(index<nHits());

  double z,y;
  double discr;

  z = HitCoordinates[index][1];
  y = HitCoordinates[index][0];

  //b=(-z)*a+x_or_y;
  double slope = (-z);
  double offset = y;
  //geradengleichung so transformieren, dass sie einer ellipse im ursprung entspricht
  offset -= bymax;
  offset += slope*aymax;
  //	cout << a0 << " " << b0 <<  endl;

  double r1=getScaleA()*r1Y;
  double r2=getScaleB()*r2Y;

  
  discr = r1*r1*slope*slope + r2*r2 - offset*offset;
  if(discr>=0) {
	return true;
  }
  return false;
}


void TChough1::findMaxInHisto(double& amax,double& bmax) {

  double minA,maxA,minB,maxB,nBinsA,nBinsB;
  TH2D* hist;

  hist = houghHistoYZ;
  minA = minAY;
  maxA = maxAY;
  minB = minBY;
  maxB = maxBY;
  nBinsA = nBinsAY;
  nBinsB = nBinsBY;


  double binA = (maxA - minA)/nBinsA;
  double binB = (maxB - minB)/nBinsB;

  double maxEntry = -1;

  for(int ia=0;ia<nBinsA;ia++) {
	for(int ib=0;ib<nBinsB;ib++) {
	  if(hist->GetBinContent(ia+1,ib+1) > maxEntry) {
		maxEntry = hist->GetBinContent(ia+1,ib+1);
		amax = minA + ia*binA + 0.5*binA;
		bmax = minB + ib*binB + 0.5*binB;
	  }
	}
  }
}



void TChough1::cleanup() {
  //  std::cout << "Cleaning up..." << std::endl;

  delete canvas1;



  delete houghHistoYZ;

  for(int i=0;i<Max_NumberOfHits;i++) {
	delete houghLinesYZ[i];
  }

  for(int i=0;i<Max_NumberOfHits;i++) {
	selHits[i] = -1;
	nSelHits = 0;
  }

  canvas1 = NULL;



  houghHistoYZ=NULL;

  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLinesYZ[i]=NULL;
  }
  //  std::cout << "done cleaning up..." << std::endl;

}

 
 


void TChough1::makeHoughLines(){
  char buf[50];
  char bufName[50];
  for(int i=0;i<NumberOfHits;i++) {
	sprintf(buf,"%f-%f*x",HitCoordinates[i][0],HitCoordinates[i][1]);
	sprintf(bufName,"copy%f-%f*x",HitCoordinates[i][0],HitCoordinates[i][1]);
	houghLinesYZ[i] = new TF1(bufName,buf,minAY,maxAY);
	//	  houghLinesYZ[i]->SetLineColor(kBlue);
	houghLinesYZ[i]->SetLineStyle(2);
  }
}


void TChough1::findHitsOnTrack(){
  double z,y;
  double discr;

  for(int i=0;i<Max_NumberOfHits;i++) {
	selHits[i] = -1;
  }
  nSelHits = 0;
  

  for(int ihit=0;ihit<NumberOfHits;ihit++) {
	z = HitCoordinates[ihit][1];
	y = HitCoordinates[ihit][0];

	//b=(-z)*a+x_or_y;
	double slope = (-z);
	double offset = y;
	//geradengleichung so transformieren, dass sie einer ellipse im ursprung entspricht
	offset -= bymax;
	offset += slope*aymax;

	discr = r1Y*r1Y*slope*slope + r2Y*r2Y - offset*offset;
	if(discr>=0) {
	  selHits[nSelHits] = ihit;
	  nSelHits++;
	}
	if(discr>=0) houghLinesYZ[ihit]->SetLineStyle(1);
	if(discr>=0) houghLinesYZ[ihit]->SetLineWidth(1);
	if(discr>=0) houghLinesYZ[ihit]->SetLineColor(kGreen);
  }

}


void TChough1::makeHoughHisto(){
  char buf[50];
  static TRandom r(0);
  sprintf(buf,"houghHistoYZ%5.5f",r.Uniform());
  houghHistoYZ = new TH2D(buf,"",nBinsAY,minAY,maxAY,nBinsBY,minBY,maxBY);
  houghHistoYZ->SetStats(kFALSE);
  houghHistoYZ->GetYaxis()->SetTitle("B");
  houghHistoYZ->GetXaxis()->SetTitle("A");
  

  for(int ihit=0;ihit<NumberOfHits;ihit++) {///NumberOfHits;ihit++) {
	for(int ia=0;ia<nBinsAY;ia++) {
	  for(int ib=0;ib<nBinsBY;ib++) {
		int weight = binWeight(ia,ib,ihit);
		if(weight > 0) {
		  houghHistoYZ->SetBinContent(ia+1,ib+1,houghHistoYZ->GetBinContent(ia+1,ib+1) + weight);
		}
	  }
	}
  }
}


int TChough1::binWeight(int ia, int ib, int ihit) {

  double minA,maxA,minB,maxB,nBinsA,nBinsB;

  double y;
  double z = HitCoordinates[ihit][1];

  y = HitCoordinates[ihit][0];
  minA = minAY;
  maxA = maxAY;
  minB = minBY;
  maxB = maxBY;
  nBinsA = nBinsAY;
  nBinsB = nBinsBY;

  double binA = (maxA - minA)/nBinsA;
  double binB = (maxB - minB)/nBinsB;

  
  double a0 = minA + ia*binA;
  double b0 = minB + ib*binB;
  double a1 = a0 + binA;
  double b1 = b0 + binB;
  
  
  double a_at_b0 = (y-b0)/z;
  double a_at_b1 = (y-b1)/z;
  double b_at_a0 = -1.0*a0*z+y;
  double b_at_a1 = -1.0*a1*z+y;


  bool left=false;
  bool right=false;
  bool top=false;
  bool bottom=false;
  
  if(b_at_a0>b0 && b_at_a0<b1) left=true;
  if(b_at_a1>b0 && b_at_a1<b1) right=true;
  if(a_at_b0>a0 && a_at_b0<a1) bottom=true;
  if(a_at_b1>a0 && a_at_b1<a1) top=true;


  if(top || bottom || left || right) {
// 	std::cout << ihit << " " << ia << " " << ib << std::endl;
// 	cout << "top: " << top << endl;
// 	cout << "bottom: " << bottom << endl;
// 	cout << "right: " << right << endl;
// 	cout << "left: " << left << endl;
//	distance = 1.;
  }


  /*
  if(!top&&bottom&&left&&right) cout << "bottom and left and right" << endl;
  if(top&&!bottom&&left&&right) cout << "top and left and right" << endl;
  if(top&&bottom&&!left&&right) cout << "top and bottom and right" << endl;
  if(top&&bottom&&left&&!right) cout << "top and bottom and left" << endl;
  if(top&&bottom&&!left&&!right) cout << "top and bottom" << endl;
  if(top&&!bottom&&left&&!right) cout << "top and left" << endl;
  if(top&&!bottom&&!left&&right) cout << "top and right" << endl;
  if(!top&&bottom&&left&&!right) cout << "bottom and left" << endl;
  if(!top&&bottom&&!left&&right) cout << "bottom and right" << endl;
  if(!top&&!bottom&&left&&right) cout << "left and right" << endl;
  if(top&&!bottom&&!left&&!right) cout << "top only" << endl;
  if(!top&&bottom&&!left&&!right) cout << "bottom only" << endl;
  if(!top&&!bottom&&left&&!right) cout << "left only" << endl;
  if(!top&&!bottom&&!left&&right) cout << "right only" << endl;
  */

  double distance=-1;
  if(left&&right) distance = sqrt( pow(b_at_a0-b_at_a1,2.) + pow(binA,2.));
  if(left&&bottom) distance = sqrt( pow(a_at_b0-a0,2.) +pow(b_at_a0-b0,2.) );
  if(left&&top) distance = sqrt( pow(a_at_b1-a0,2.) +pow(b_at_a0-b1,2.) );
  if(bottom&&top) distance = sqrt( pow(a_at_b0 - a_at_b1,2.)+pow(binB,2.));
  if(bottom&&right) distance = sqrt(pow(a1-a_at_b0,2.)+pow(b0-b_at_a1,2.));
  if(right&&top) distance = sqrt(pow(b1-b_at_a1,2.) + pow(a1-a_at_b1,2.)   );

  //habe jetzt distance oder aber -1 fuer nicht getroffen
  double maxDistance = sqrt(pow(binA,2.)+pow(binB,2.));
  int weight = (int)((distance * 100) / maxDistance);
  return weight;
}
