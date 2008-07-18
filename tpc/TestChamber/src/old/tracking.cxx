#include "tracking.h"

#include "assert.h"

#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>

#include "PndTpcCluster.h"

#include "TEllipse.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TApplication.h"

#define DEBUG  std::cout << __FILE__ << " " << __LINE__ << std::endl

tracking::tracking(int _visflag, 
				   double _r1, 
				   double _r2, 
				   int _nHOT,
				   bool _stopROOT,
				   std::string _name,
				   unsigned int _mounting)  : r1Y(_r1), r2Y(_r2), visflag(_visflag), minNhitsOnTrack(_nHOT), counter(0),stopROOT(_stopROOT), dir(_name.c_str(),_name.c_str()), name(_name),mounting(_mounting){
//  cleanup();

  minAX = -1.;
  maxAX = 1.;
  minBX = -100.;
  maxBX = 150.;
  minAY = -1.;
  maxAY = 1.;
  minBY = -100.;
  maxBY = 150.;
  
  nBinsAX = 150;
  nBinsBX = 150;
  nBinsAY = 150;
  nBinsBY = 150;
  
  r1X = 0.03;
  r2X = 2.1;
  /*    static const double r1Y = 0.03; */
  /*    static const double r2Y = 2.1; */


  canvas1=NULL;
  canvas2=NULL;
  xzHits=NULL;
  yzHits=NULL;
  xzErrHits=NULL;
  yzErrHits=NULL;
  xzSelHits=NULL;
  yzSelHits=NULL;
  xzFit = NULL;
  yzFit = NULL;

  assert(mounting == 1 || mounting ==2);

  switch(mounting){
  case 1:
	maxX=99.2;
	maxY=8.;
	maxZ=85.;
	break;
  case 2:
	maxX=8.;
	maxY=85.;
	maxZ=99.2;
	break;
  }


  hits3D=NULL;

  houghHistoXZ=NULL;
  houghHistoYZ=NULL;

  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLinesXZ[i]=NULL;
  }

  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLinesYZ[i]=NULL;
  }
  dir.cd();
  xzHits_H = new TH2D("xzHits_H","",2,0.,maxZ,2,0.,maxX);
  yzHits_H = new TH2D("yzHits_H","",2,0.,maxZ,2,0.,maxY);

  switch(mounting) {
  case 1:
	xzHits_H->GetYaxis()->SetTitle("X in mm");
	xzHits_H->GetXaxis()->SetTitle("Z in mm");
	yzHits_H->GetYaxis()->SetTitle("Y in mm");
	yzHits_H->GetXaxis()->SetTitle("Z in mm");
	break;
  case 2:
	xzHits_H->GetYaxis()->SetTitle("Y in mm");
	xzHits_H->GetXaxis()->SetTitle("X in mm");
	yzHits_H->GetYaxis()->SetTitle("Z in mm");
	yzHits_H->GetXaxis()->SetTitle("X in mm");
	break;
  }

  yzHits_H->GetYaxis()->SetTitleOffset(1.3);
  xzHits_H->GetYaxis()->SetTitleOffset(1.3);
  yzHits_H->GetYaxis()->SetTitleOffset(1.3);
  xzHits_H->SetStats(kFALSE);
  yzHits_H->SetStats(kFALSE);
  biasedResidX = new TH1D("biasedResidX","biasedResidX",50,-4.,4.);
  biasedResidY = new TH1D("biasedResidY","biasedResidY",50,-4.,4.);
  biasedResidZ = new TH1D("biasedResidZ","biasedResidZ",50,-4.,4.);
  chi2reduced = new TH1D("chi2reduced","chi2reduced",100,0.,10.);

}

bool tracking::processEvent(std::vector<PndTpcCluster*>* clusters, int trigger,AnalysisEvent *analysisEvt) {
  bool returnVal = false;
  dir.cd();
  cleanup();
  NumberOfHits = clusters->size();

  analysisEvt->setTrigger(trigger);


  for(int i=0;i<NumberOfHits;i++) {

	switch(mounting){
	case 1:
	  HitCoordinates[i][0] = clusters->at(i)->pos().x()*10.;
	  HitCoordinates[i][1] = clusters->at(i)->pos().y()*10.;
	  HitCoordinates[i][2] = clusters->at(i)->pos().z()*10.;
	  HitCoordinatesError[i][0] = clusters->at(i)->sig().x()*10.;
	  HitCoordinatesError[i][1] = clusters->at(i)->sig().y()*10.;
	  HitCoordinatesError[i][2] = clusters->at(i)->sig().z()*10.;
	  break;
	case 2:
	  HitCoordinates[i][0] = clusters->at(i)->pos().y()*10.;
	  HitCoordinates[i][1] = clusters->at(i)->pos().z()*10.;
	  HitCoordinates[i][2] = clusters->at(i)->pos().x()*10.;
	  HitCoordinatesError[i][0] = clusters->at(i)->sig().y()*10.;
	  HitCoordinatesError[i][1] = clusters->at(i)->sig().z()*10.;
	  HitCoordinatesError[i][2] = clusters->at(i)->sig().x()*10.;
	  break;
	}
	HitCharge[i] = clusters->at(i)->amp();
	clusterSize[i] = clusters->at(i)->size();
	nPad[i] = clusters->at(i)->nPad();
	nPadX[i] = clusters->at(i)->nPadX();
	nPadY[i] = clusters->at(i)->nPadY();
  }

  if(Max_NumberOfHits<NumberOfHits) {
	//std::cerr << "Can't preocess event #" << ientry
	std::cerr << "Can't process event" 
			  << " because NumberOfHits is greater than " 
			  << "Max_NumberOfHits -> increase it"
			  << std::endl;
	return false;
  }

  counter++;

  
  //	  cout << "Trigger: " << Trigger << endl;	  
  double _x1,_x2,_y1,_y2;
  

  makeHitGraph(0);
  makeHoughLines(0);
  makeHoughHisto(0,counter);
  makeHitGraph(1);
  makeHoughLines(1);
  makeHoughHisto(1,counter);

  double axmax,bxmax;
  findMaxInHisto(0,axmax,bxmax);
  
  double aymax,bymax;
  findMaxInHisto(1,aymax,bymax);
  findHitsOnTrack(1,aymax,bymax);

  makeSelHitGraph(0);
  makeSelHitGraph(1);

  if(nSelHits >= minNhitsOnTrack) {
	returnVal = true;
	fitReturn_t fitReturn;
	fitAll(axmax,bxmax,aymax,bymax,fitReturn);
	std::cout << "ax: " << fitReturn.ax << "\n"
			  << "bx: " << fitReturn.bx << "\n"
			  << "ay: " << fitReturn.ay << "\n"
			  << "by: " << fitReturn.by << "\n"
			  << std::endl;
	
	analysisEvt->setTrackParams(fitReturn.ax,
								fitReturn.bx,
								fitReturn.ay,
								fitReturn.by);
	analysisEvt->setAngles();

	analysisEvt->setNselhits(nSelHits);
	analysisEvt->setNhits(NumberOfHits);

	char buf[50];
	sprintf(buf,"%f*x+%f",fitReturn.ax,fitReturn.bx);
	xzFit = new TF1(buf,buf,0.,maxZ);
	xzFit->SetLineColor(kRed);
	xzFit->SetLineWidth(1);
	sprintf(buf,"%f*x+%f",fitReturn.ay,fitReturn.by);
	yzFit = new TF1(buf,buf,0.,maxZ);
	yzFit->SetLineColor(kRed);
	yzFit->SetLineWidth(1);

	//fill biased resid histo
	for(int i=0;i<nSelHits;i++) {
	  double dx,dy,dz;
	  residuals(fitReturn.ax,fitReturn.bx,
			    fitReturn.ay,fitReturn.by,
			    HitCoordinates[ selHits[i] ][0],
			    HitCoordinates[ selHits[i] ][1],
			    HitCoordinates[ selHits[i] ][2],
			    dx,dy,dz);

	  AnalysisCluster cluster;

	  switch(mounting){
	  case 1:
		cluster.pos(HitCoordinates[ selHits[i] ][0],
					HitCoordinates[ selHits[i] ][1],
					HitCoordinates[ selHits[i] ][2]);
		cluster.err(HitCoordinatesError[ selHits[i] ][0],
					HitCoordinatesError[ selHits[i] ][1],
					HitCoordinatesError[ selHits[i] ][2]);
		cluster.resid(dx,dy,dz);
		break;
	  case 2:
		cluster.pos(HitCoordinates[ selHits[i] ][2],
					HitCoordinates[ selHits[i] ][0],
					HitCoordinates[ selHits[i] ][1]);
		cluster.err(HitCoordinatesError[ selHits[i] ][2],
					HitCoordinatesError[ selHits[i] ][0],
					HitCoordinatesError[ selHits[i] ][1]);
		cluster.resid(dz,dx,dy);
		break;
	  }
	  cluster.amp(HitCharge[ selHits[i] ]);
	  cluster.setClustersizes(clusterSize[selHits[i] ],
							  nPad[selHits[i] ],
							  nPadX[selHits[i] ],
							  nPadY[selHits[i] ]);
	  analysisEvt->addCluster(cluster);

	  if( (HitCoordinates[ selHits[i] ][2] >= Z_START) && (HitCoordinates[ selHits[i] ][2] < Z_STOP) ){
		biasedResidX->Fill(dx);
		biasedResidY->Fill(dy);
		biasedResidZ->Fill(dz);
	  }

	}
	
	if(analysisEvt->getMinYresid()>1.) returnVal = false;
	chi2reduced->Fill(fitReturn.chi2/(nSelHits*2. - 4.));
	
  }
  

  //fit subset 
  
  if(visflag==1 && NumberOfHits>2) {


	char bufa[50];
	sprintf(bufa,"canvas_a_%s",name.c_str());
	char bufb[50];
	sprintf(bufb,"canvas_b_%s",name.c_str());
	char buf2[50];
	sprintf(buf2,"HTA - %s - trigger: %d",name.c_str(),trigger);
	if(stopROOT) {
	  canvas1=new TCanvas(bufa,buf2,700,0  ,600,430);
	  canvas2=new TCanvas(bufb,buf2,700,500,600,430);
	}
	else {
	  canvas1=new TCanvas(bufa,buf2,0,0  ,600,430);
	  canvas2=new TCanvas(bufb,buf2,0,500,600,430);
	}

	//	TCanvas* myCanvas2=new TCanvas("myCanvas2","Hough Transform Analysis",400,400);
	//	TCanvas* myCanvas3=new TCanvas("myCanvas3","Hough Transform Analysis",400,400);
	//	TCanvas* myCanvas4=new TCanvas("myCanvas4","Hough Transform Analysis",400,400);
	canvas1->Divide(2,1);
	canvas2->Divide(2,1);

	canvas1->cd(1);

	xzHits_H->Draw();


	for(int i=0;i<NumberOfHits;++i) {
	  xzHitsVec.at(i)->Draw("P");
	}

	xzSelHits->Draw("P");
	xzErrHits->Draw("P");

	if(nSelHits >= minNhitsOnTrack) xzFit->Draw("same");

	canvas1->cd(2);
	yzHits_H->Draw();

	for(int i=0;i<NumberOfHits;++i) {
	  yzHitsVec.at(i)->Draw("P");
	}
	yzSelHits->Draw("P");
	yzErrHits->Draw("P");
	
	if(nSelHits >= minNhitsOnTrack) yzFit->Draw("same");
	
	canvas2->cd(1);
	//	  houghLinesXZ_H->Draw();
	houghHistoXZ->Draw("colz");
	for(int i=0;i<NumberOfHits;i++) {
	  houghLinesXZ[i]->Draw("same");
	}
	
	canvas2->cd(2);
	//	  houghLinesYZ_H->Draw();
	houghHistoYZ->Draw("colz");
	for(int i=0;i<NumberOfHits;i++) {
	  houghLinesYZ[i]->Draw("same");
	}
	
	TEllipse* ell = new TEllipse(aymax,bymax,r1Y,r2Y);
	ell->SetLineWidth(4);
	ell->Draw("same");

	//	dir.ls();	
	if(stopROOT) {
	  gApplication->SetReturnFromRun(kTRUE);
	  gSystem->Run();
	}

	//delete myCanvas1;
// 	delete myCanvas2;
// 	delete myCanvas3;
// 	delete myCanvas4;
  }

  return returnVal;
  
}


tracking::~tracking(){
  //  cleanup();
}

void tracking::findMaxInHisto(int xyswitch, double& amax,double& bmax) {

  double minA,maxA,minB,maxB,nBinsA,nBinsB;
  TH2D* hist;

  switch(xyswitch) {
  case 0:
	hist = houghHistoXZ;
	minA = minAX;
	maxA = maxAX;
	minB = minBX;
	maxB = maxBX;
	nBinsA = nBinsAX;
	nBinsB = nBinsBX;
	break;
  case 1:
	hist = houghHistoYZ;
	minA = minAY;
	maxA = maxAY;
	minB = minBY;
	maxB = maxBY;
	nBinsA = nBinsAY;
	nBinsB = nBinsBY;
	break;
  }



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

void tracking::makeHoughLines(int xyswitch){
  char buf[50];
  for(int i=0;i<NumberOfHits;i++) {
	switch(xyswitch) {
	case 0:
	  sprintf(buf,"%f-%f*x",HitCoordinates[i][0],HitCoordinates[i][2]);
			  //gRandom->Gaus(HitCoordinates[i][0],HitCoordinatesError[i][0]),HitCoordinates[i][2]);
	  houghLinesXZ[i] = new TF1(buf,buf,minAX,maxAX);
	  //	  houghLinesXZ[i]->SetLineColor(kBlue);
	  houghLinesXZ[i]->SetLineStyle(2);
	  break;
	case 1:
	  sprintf(buf,"%f-%f*x",HitCoordinates[i][1],HitCoordinates[i][2]);
	  houghLinesYZ[i] = new TF1(buf,buf,minAY,maxAY);
	  //	  houghLinesYZ[i]->SetLineColor(kBlue);
	  houghLinesYZ[i]->SetLineStyle(2);
	  break;
	}

  }
}

void tracking::findHitsOnTrack(int xyswitch, double a0,double b0){
  double z,x_or_y;
  double discr;

  for(int i=0;i<Max_NumberOfHits;i++) {
	selHits[i] = -1;
  }
	nSelHits = 0;
  

  for(int ihit=0;ihit<NumberOfHits;ihit++) {
	z = HitCoordinates[ihit][2];
	switch(xyswitch) {
	case 0:
	  x_or_y = HitCoordinates[ihit][0];
	  break;
	case 1:
	  x_or_y = HitCoordinates[ihit][1];
	  break;
	}

	//b=(-z)*a+x_or_y;
	double slope = (-z);
	double offset = x_or_y;
	//geradengleichung so transformieren, dass sie einer ellipse im ursprung entspricht
	offset -= b0;
	offset += slope*a0;
	//	cout << a0 << " " << b0 <<  endl;

	switch(xyswitch) {
	case 0:
	  discr = r1X*r1X*slope*slope + r2X*r2X - offset*offset;
	  //if(discr>=0) houghLinesXZ[ihit]->SetLineStyle(1);
	  break;
	case 1:
	  discr = r1Y*r1Y*slope*slope + r2Y*r2Y - offset*offset;
	  if(discr>=0) {
		selHits[nSelHits] = ihit;
		nSelHits++;
	  }
	  if(discr>=0) houghLinesYZ[ihit]->SetLineStyle(1);
	  if(discr>=0) houghLinesYZ[ihit]->SetLineWidth(1);
	  if(discr>=0) houghLinesYZ[ihit]->SetLineColor(kGreen);
	  break;
	}

	

  }

}

void tracking::makeHoughHisto(int xyswitch,int nevt){
  //char buf[50];


  switch(xyswitch) {
  case 0:
	//sprintf(buf,"houghHistoXZ%i",nevt);
	houghHistoXZ = new TH2D("","",nBinsAX,minAX,maxAX,nBinsBX,minBX,maxBX);
	houghHistoXZ->SetStats(kFALSE);
	houghHistoXZ->GetYaxis()->SetTitle("BX");
	houghHistoXZ->GetXaxis()->SetTitle("AX");
	
	break;
  case 1:
	//sprintf(buf,"houghHistoYZ%i",nevt);
	houghHistoYZ = new TH2D("","",nBinsAY,minAY,maxAY,nBinsBY,minBY,maxBY);
	houghHistoYZ->SetStats(kFALSE);
	houghHistoYZ->GetYaxis()->SetTitle("BY");
	houghHistoYZ->GetXaxis()->SetTitle("AY");

	break;
  }


  for(int ihit=0;ihit<NumberOfHits;ihit++) {///NumberOfHits;ihit++) {
	switch(xyswitch) {
	case 0:
	  for(int ia=0;ia<nBinsAX;ia++) {
		for(int ib=0;ib<nBinsBX;ib++) {
		  int weight = binWeight(ia,ib,ihit,xyswitch);
		  if(weight > 0) {
			houghHistoXZ->SetBinContent(ia+1,ib+1,houghHistoXZ->GetBinContent(ia+1,ib+1) + weight);
		  }
		}
	  }
	  break;
	case 1:
	  for(int ia=0;ia<nBinsAY;ia++) {
		for(int ib=0;ib<nBinsBY;ib++) {
		  int weight = binWeight(ia,ib,ihit,xyswitch);
		  if(weight > 0) {
			houghHistoYZ->SetBinContent(ia+1,ib+1,houghHistoYZ->GetBinContent(ia+1,ib+1) + weight);
		  }
		}
	  }
	  break;
	}

  }
}

int tracking::binWeight(int ia, int ib, int ihit , int xyswitch) {

  double minA,maxA,minB,maxB,nBinsA,nBinsB;

  double x_or_y;
  double z = HitCoordinates[ihit][2];

	switch(xyswitch) {
	case 0:
	  x_or_y = HitCoordinates[ihit][0];
	  minA = minAX;
	  maxA = maxAX;
	  minB = minBX;
	  maxB = maxBX;
	  nBinsA = nBinsAX;
	  nBinsB = nBinsBX;
	  break;
	case 1:
	  x_or_y = HitCoordinates[ihit][1];
	  minA = minAY;
	  maxA = maxAY;
	  minB = minBY;
	  maxB = maxBY;
	  nBinsA = nBinsAY;
	  nBinsB = nBinsBY;
	  break;
	}



  double binA = (maxA - minA)/nBinsA;
  double binB = (maxB - minB)/nBinsB;



  
  double a0 = minA + ia*binA;
  double b0 = minB + ib*binB;
  double a1 = a0 + binA;
  double b1 = b0 + binB;
  
  
  double a_at_b0 = (x_or_y-b0)/z;
  double a_at_b1 = (x_or_y-b1)/z;
  double b_at_a0 = -1.0*a0*z+x_or_y;
  double b_at_a1 = -1.0*a1*z+x_or_y;


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

  /*
  if(b_at_a0>b0 && b_at_a0<b1) {//linke kante
	if(b_at_a1>b0 && b_at_a1<b1) {//rechte kante
	  distance = sqrt( pow(b_at_a0-b_at_a1,2.) + pow(binA,2.));
	}
	else{
	  if(a_at_b0>a0 && a_at_b0<a1) {//untere kante
		distance = sqrt( pow(a_at_b0-a0,2.) +pow(b_at_a0-b0,2.) );
	  }
	  else {
		if(a_at_b1>a0 && a_at_b1<a1) {//obere kante
		  distance = sqrt( pow(a_at_b1-a0,2.) +pow(b_at_a0-b1,2.) );
		}
		else {
		  std::cerr << "Error 1: geometrically inconsistent" << std::endl;
		}
	  }
	}
  }
  
  else {//nicht durch linke kante
	if(a_at_b0>a0 && a_at_b0<a1) {//untere kante
	  if(a_at_b1>a0 && a_at_b1<a1) {//obere kante
		distance = sqrt( pow(a_at_b0 - a_at_b1,2.)+pow(binB,2.));
	  }
	  else {
		if(b_at_a1>b0 && b_at_a1<b1) {//rechte kante
		  distance = sqrt(pow(a1-a_at_b0,2.)+pow(b0-b_at_a1,2.));
		}
		else {
		  std::cerr << "Error 2: geometrically inconsistent" << std::endl;
		}
	  }
	}
	else {//hier gibt es nur noch die moeglichkeit, dass rechte und obere getroffen
		  //oder aber gar keine
	  if(b_at_a1>b0 && b_at_a1<b1) {//rechte kante
		if(a_at_b1>a0 && a_at_b1<a1) {//obere kante
		  distance = sqrt(pow(b1-b_at_a1,2.) + pow(a1-a_at_b1,2.)   );
		}
		else {
		  std::cerr << "Error 3: geometrically inconsistent" << std::endl;
		}
	  }
	  
	}
  }
  */
  //habe jetzt distance oder aber -1 fuer nicht getroffen
  double maxDistance = sqrt(pow(binA,2.)+pow(binB,2.));
  int weight = (int)((distance * 100) / maxDistance);
  return weight;
}


void tracking::makeHitGraph(int xyswitch) {
  double xvals[Max_NumberOfHits];
  double yvals[Max_NumberOfHits];
  double xerrs[Max_NumberOfHits];
  double yerrs[Max_NumberOfHits];

  double minAmp=1.E10;
  double maxAmp=-1.E10;
  
  for(int i=0;i<NumberOfHits;i++) {
	
	if(HitCharge[i]>maxAmp) maxAmp=HitCharge[i];
	if(HitCharge[i]<minAmp) minAmp=HitCharge[i];
  }
  double maxMarkerSize = 4.;
  double minMarkerSize = 1.5;

  double singleX[1];
  double singleY[1];
  double singleXerr[1];
  double singleYerr[1];

  for(int i=0;i<NumberOfHits;i++) {
	switch(xyswitch) {
	case 0:
	  yvals[i] = HitCoordinates[i][0];
	  yerrs[i] = HitCoordinatesError[i][0];
	  singleY[0] = HitCoordinates[i][0];
	  singleYerr[0] = HitCoordinatesError[i][0];
	  break;
	case 1:
	  yvals[i] = HitCoordinates[i][1];
	  yerrs[i] = HitCoordinatesError[i][1];
	  singleY[0] = HitCoordinates[i][1];
	  singleYerr[0] = HitCoordinatesError[i][1];
	  break;
	}
	xvals[i] = HitCoordinates[i][2];
	xerrs[i] = HitCoordinatesError[i][2];
	singleX[0] = HitCoordinates[i][2];
	singleXerr[0] = HitCoordinatesError[i][2];

	TGraph* _gr_;
	switch(xyswitch) {
	case 0:
	  _gr_ = new TGraph(1,singleX,singleY);
	  _gr_->SetMarkerStyle(24);
	  _gr_->SetMarkerSize(minMarkerSize+(maxMarkerSize-minMarkerSize)*(HitCharge[i]-minAmp)/(maxAmp-minAmp));
	  xzHitsVec.push_back(_gr_);
	  break;
	case 1:
	  _gr_ = new TGraph(1,singleX,singleY);
	  _gr_->SetMarkerStyle(24);
	  _gr_->SetMarkerSize(minMarkerSize+(maxMarkerSize-minMarkerSize)*(HitCharge[i]-minAmp)/(maxAmp-minAmp));
	  yzHitsVec.push_back(_gr_);	  
	  break;
	}
  }


  switch(xyswitch) {
  case 0:
	xzHits = new TGraphErrors(NumberOfHits,xvals,yvals);
	xzHits->SetMarkerStyle(24);
	xzErrHits = new TGraphErrors(NumberOfHits,xvals,yvals,xerrs,yerrs);
	xzErrHits->SetMarkerStyle(1);

	break;
  case 1:
	yzHits = new TGraphErrors(NumberOfHits,xvals,yvals);
	yzHits->SetMarkerStyle(24);
	yzErrHits = new TGraphErrors(NumberOfHits,xvals,yvals,xerrs,yerrs);
	yzErrHits->SetMarkerStyle(1);

	break;
  }
	
  

}

void tracking::makeSelHitGraph(int xyswitch) {
  double xvals[Max_NumberOfHits];
  double yvals[Max_NumberOfHits];

  for(int i=0;i<nSelHits;i++) {
	switch(xyswitch) {
	case 0:
	  yvals[i] = HitCoordinates[ selHits[i] ][0];
	  break;
	case 1:
	  yvals[i] = HitCoordinates[ selHits[i] ][1];
	  break;
	}
	xvals[i] = HitCoordinates[ selHits[i] ][2];

  }


  switch(xyswitch) {
  case 0:
	xzSelHits = new TGraph(nSelHits,xvals,yvals);
	xzSelHits->SetMarkerSize(1.);
	xzSelHits->SetMarkerColor(kGreen);
	xzSelHits->SetMarkerStyle(20);
	break;
  case 1:
	yzSelHits = new TGraph(nSelHits,xvals,yvals);
	yzSelHits->SetMarkerSize(1.);
	yzSelHits->SetMarkerColor(kGreen);
	yzSelHits->SetMarkerStyle(20);
	break;
  }
}

int tracking::fitAll(double ax_s, double bx_s, double ay_s, double by_s,
		   fitReturn_t &fitReturn) {
  if(nSelHits<minNhitsOnTrack) {
	std::cout << "nSelHits<minNhitsOnTrack for this event -> skipped" << std::endl;
	return 0;
  }

  if(nSelHits>MAXHITS) {
	std::cout << "nSelHits>MAXHITS for this event -> skipped" << std::endl;
	return 0;
  }

  fitArg_t fitArg;

  for(int i=0;i<nSelHits;i++) {

	fitArg.x[i] = HitCoordinates[ selHits[i] ][0];
	fitArg.y[i] = HitCoordinates[ selHits[i] ][1];
	fitArg.z[i] = HitCoordinates[ selHits[i] ][2];
	fitArg.xerr[i] = HitCoordinatesError[ selHits[i] ][0];
	fitArg.yerr[i] = HitCoordinatesError[ selHits[i] ][1];
	fitArg.zerr[i] = HitCoordinatesError[ selHits[i] ][2];
  }
  fitArg.nHits = nSelHits;
  fitArg.ax_s = ax_s;
  fitArg.bx_s = bx_s;
  fitArg.ay_s = ay_s;
  fitArg.by_s = by_s;
  fitReturn = doFit(fitArg);
  return 1;
}


void tracking::cleanup() {
  //  std::cout << "Cleaning up..." << std::endl;

  for(int i=0;i<xzHitsVec.size();++i) {
	delete xzHitsVec.at(i);
  }
  xzHitsVec.clear();
  for(int i=0;i<yzHitsVec.size();++i) {
	delete yzHitsVec.at(i);
  }
  yzHitsVec.clear();

  delete canvas1;
  delete canvas2;

  delete xzHits;
  delete yzHits;
  delete xzErrHits;
  delete yzErrHits;
  delete xzSelHits;
  delete yzSelHits;
  delete xzFit;
  delete yzFit;
  delete hits3D;

  delete houghHistoXZ;
  delete houghHistoYZ;

  for(int i=0;i<Max_NumberOfHits;i++) {
	delete houghLinesXZ[i];
  }

  for(int i=0;i<Max_NumberOfHits;i++) {
	delete houghLinesYZ[i];
  }

  for(int i=0;i<Max_NumberOfHits;i++) {
	selHits[i] = -1;
	nSelHits = 0;
  }

  canvas1 = NULL;
  canvas2 = NULL;

  xzHits=NULL;
  yzHits=NULL;
  xzErrHits=NULL;
  yzErrHits=NULL;
  xzSelHits=NULL;
  yzSelHits=NULL;
  xzFit = NULL;
  yzFit = NULL;


  hits3D=NULL;

  houghHistoXZ=NULL;
  houghHistoYZ=NULL;

  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLinesXZ[i]=NULL;
  }

  for(int i=0;i<Max_NumberOfHits;i++) {
	houghLinesYZ[i]=NULL;
  }


  //  std::cout << "done cleaning up..." << std::endl;

}

 
 
