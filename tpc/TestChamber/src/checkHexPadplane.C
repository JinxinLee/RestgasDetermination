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
struct pad_t{
  int id;
  double x;
  double y;
};
void checkPadplane(){
  
  gROOT->Reset();
  TApplication app("app",NULL,NULL);
 
  ifstream input("hex2.dat");
  
  vector<pad_t> pads;
  char line[200];
  int j=0;
  pad_t pad;
  int dummy;
  double maxX=0;
  double minX=0;
  double maxY=0;
  double minY=0;
  while(input.getline(line,199)) {
	std::istringstream istr(line);
	istr>>pad.id>>dummy>>dummy>>pad.x>>pad.y;
	if(pad.x>maxX){
	  maxX=pad.x;
	}
	if(pad.x<minX){
	  minX=pad.x;
	}
	if(pad.y>maxY){
	  maxY=pad.y;
	}
	if(pad.y<minY){
	  minY=pad.y;
	}
	pads.push_back(pad);
	std::cout << "# " << j << std::endl;
	j++;
  }

  TCanvas c1("c1","",1100,1100);
  c1.Draw();
  cout<<"after canvas"<<endl;
  
  TPaveText t1(0.1,0.1,0.13,0.13);

  t1.AddText("mhmmm");
  t1.Draw();
  
  int nPads = pads.size();
  double offset =0.008;
  double zeroX=-minX;
  double zeroY=-minY;
  double rangeX=maxX-minX+10;
  double rangeY=maxY-minY+10;
  double factor = 1;
  vector<TPaveText *> padText;
  for(vector<pad_t>::iterator it=pads.begin();it!=pads.end();++it){

    double x = (*it).x;
    double y = (*it).y;
    int id = (*it).id;
    cout<<"iterating "<<x<<endl;
    padText.push_back(new TPaveText(((x+zeroX)/rangeX-offset)/factor+0.05,((y+zeroY)/rangeY-offset)/factor+0.05,((x+zeroX)/rangeX+offset)/factor+0.05,((y+zeroY)/rangeY+offset)/factor+0.05));
    ostringstream ostr;
    ostr << id;
    padText.back()->AddText(ostr.str().c_str());
    padText.back()->Draw();
    
    
  }

  cout<<"after first pave"<<endl;
  cout<<"rangeX "<<rangeX<<endl;
  cout<<"rangeY "<<rangeY<<endl;
  cout<<"minX "<<minX<<endl;
  cout<<"maxX "<<maxX<<endl;
  cout<<"minY "<<minY<<endl;
  cout<<"maxY "<<maxY<<endl;
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
}
