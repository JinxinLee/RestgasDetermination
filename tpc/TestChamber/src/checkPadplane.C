
#include<iostream>
#include<fstream>
#include<sstream>

#include "TROOT.h"
#include "TH1.h"
#include "TPaveText.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TCanvas.h"

using namespace std;

class data_t{
private:
  static const int  NUMCOLS = 20;
public:
  double cols[NUMCOLS];
  data_t(){reset();}
  void reset(){
	for(int i=0;i<NUMCOLS;++i) cols[i]=-1.E10;
  } 
  void print() {
	cout << "---------------" << endl;
	cout << "data object at: " << this << endl;
	for(int i=0;i<NUMCOLS;++i) cout << cols[i] << endl;
	cout << "+++++++++++++++" << endl;
  }
  int ncols() {return NUMCOLS;}
};


void checkPadplane(){
  
  gROOT->Reset();
  TApplication app("app",NULL,NULL);
 
  ifstream input("padplane.dat");
  
  vector<data_t> datavec;
  char line[200];
  int j=0;
  data_t data;
  while(input.getline(line,199)) {
	std::istringstream istr(line);
	data.reset();
	for(int i=0;i<data.ncols();++i) istr >> data.cols[i];
	datavec.push_back(data);
	std::cout << "# " << j << std::endl;
	j++;
  }

  for(unsigned int i=0;i<datavec.size();++i) datavec.at(i).print();

  TCanvas c1("c1","",1000,900);
  c1.Draw();

  TPaveText t1(0.1,0.1,0.13,0.13);
  t1.AddText("dreggn");
  t1.Draw();

  double maxX=9.61;
  double maxY=0.75;
  int nX = 16;
  int nY = 8;

  int nPads = datavec.size();
  TPaveText *pads[nPads];

  double offsetX = (1./nX)/2.2;
  double offsetY = (1./nY)/2.2;

  double factor = 1.1;

  for(int i=0;i<nPads;++i) {
	double x = datavec[i].cols[3];
	double y = datavec[i].cols[4];
	
	pads[i] = new TPaveText((x/maxX-offsetX)/factor,(y/maxY-offsetY)/factor,(x/maxX+offsetX)/factor,(y/maxY+offsetY)/factor);
	ostringstream ostr;
	ostr << datavec[i].cols[0];
	pads[i]->AddText(ostr.str().c_str());
	ostr.str(" ");
	pads[i]->AddText(ostr.str().c_str());
	for(int j=0;j<8;j++) {
	  if(datavec[i].cols[6+j]>-1) {
		ostr.str("");
		ostr << datavec[i].cols[6+j];
		pads[i]->AddText(ostr.str().c_str());
	  }
	}
	pads[i]->Draw();
  }

  //TH1D h4("h4","h4",100,0,5);
  //h4.Draw();
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();

  
}

 
 
