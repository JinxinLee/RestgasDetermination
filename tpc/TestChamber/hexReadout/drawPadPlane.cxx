#include "Mapping.h"
#include<string>
#include<iostream>
#include<fstream>
#include<map>
#include<vector>

#include"TROOT.h"
#include"TGraph.h"
#include"TH2.h"
#include"TCanvas.h"
#include"TLegend.h"
#include "TApplication.h"

std::map<int,int> read(std::string filename){
  std::map<int,int> ret;
  std::ifstream input(filename.c_str());
  char line[200];
  std::vector<Channel_t> channels;
  int lines=0;
  while(input.getline(line,199)) {
    std::istringstream istr(line);
    std::string s;
    istr >> s;
    if(s.substr(0,1)=="#" || s.substr(0,1)=="") continue;
    unsigned int chip,pin;
    std::istringstream istr1(s.c_str());
    istr1 >> pin;
    istr >> chip;
    ret[pin]=chip;
  }
  return ret;
}

int main(int argc, char* argv[]) {
  TApplication theApp("myApp",NULL,NULL);
 
  

  if(argc != 3) {
    std::cerr << "NumArgs is not 2. Args: infile chipMapFile" << std::endl;
	throw;
  }
  std::string infile(argv[1]);
  std::string chipMapFile(argv[2]);

  std::map<int,int> chipMap = read(chipMapFile);

  Mapping m;
  m.read(infile.c_str());
  
  //  double x,y;
  // m.map(4,48,x,y);
  //std::cout << x << " " << y << std::endl;


  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
  double x[6][16][16];
  double y[6][16][16];
  int marker[6][16][16];
  int color[6][16][16];
  int counter[6][16];

  for(int i=0;i<6;++i){
    for(int j=0;j<16;++j){
      counter[i][j]=0;
      for(int k=0;k<16;++k){
	x[i][j][k]=-60.;
	y[i][j][k]=-60.;
	marker[i][j][k]=1;
	color[i][j][k]=1;
      }
    }
  }

  for(int i=0;i<6;++i){
    for(int j=0;j<256;++j){
      double X,Y;
      std::cout << "mapping " << i << " " << j+Mapping::PINOFFSET << std::endl;


      m.map(i,j+Mapping::PINOFFSET,X,Y);
      std::cout << i << " " << j/16 << " " << counter[i][j/16] << std::endl;
      x[i][j/16][counter[i][j/16]]=X;
      y[i][j/16][counter[i][j/16]++]=Y;
    }
  }

  TGraph *g[6][16];

  for(int i=0;i<6;++i){
    for(int j=0;j<16;++j){
      g[i][j]=new TGraph(counter[i][j],x[i][j],y[i][j]);
    }
  }

  for(int i=0;i<6;++i){
    for(int j=0;j<16;++j){
      g[i][j]->SetMarkerColor(j);
      g[i][j]->SetMarkerStyle(20+i);
      char buf[50];
      sprintf(buf,"g_%d_%d",i,j);
      g[i][j]->SetName(buf);
    }
  }

  TCanvas *c = new TCanvas("c","",600,600);

  TH2D *h = new TH2D("h","",2,-50,50,2,-50,50);
  h->SetStats(kFALSE);
  h->Draw();
  for(int i=0;i<6;++i){
    for(int j=0;j<16;++j){
      g[i][j]->Draw("P");
    }
  }  

  TLegend *l = new TLegend(0.6,0.6,0.9,0.9);

  for(int j=0;j<16;++j){
    int i=0;
    char buf[50];
    sprintf(buf,"g_%d_%d",i,j);
    char buf2[50];
    sprintf(buf2,"Pad%d",j+1);
    l->AddEntry(buf,buf2,"p");
  }
  l->Draw();
  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
  double x1[6][4][64];
  double y1[6][4][64];
  int marker1[6][4][64];
  int color1[6][4][64];
  int counter1[6][4];

  for(int i=0;i<6;++i){
    for(int j=0;j<4;++j){
      counter1[i][j]=0;
      for(int k=0;k<64;++k){
	x1[i][j][k]=-60.;
	y1[i][j][k]=-60.;
	marker1[i][j][k]=1;
	color1[i][j][k]=1;
      }
    }
  }

  for(int i=0;i<6;++i){
    for(int j=0;j<256;++j){
      double X,Y;
      m.map(i,j+Mapping::PINOFFSET,X,Y);

      int chipNum = chipMap[j+Mapping::PINOFFSET]-1;

      x1[i][chipNum][counter1[i][chipNum]]=X;
      y1[i][chipNum][counter1[i][chipNum]++]=Y;
    }
  }

  TGraph *g1[6][4];

  for(int i=0;i<6;++i){
    for(int j=0;j<4;++j){
      g1[i][j]=new TGraph(counter1[i][j],x1[i][j],y1[i][j]);
    }
  }

  for(int i=0;i<6;++i){
    for(int j=0;j<4;++j){
      g1[i][j]->SetMarkerColor(j+1);
      g1[i][j]->SetMarkerStyle(20+i);
      char buf[50];
      sprintf(buf,"g1_%d_%d",i,j);
      g1[i][j]->SetName(buf);
    }
  }

  TCanvas *c1 = new TCanvas("c1","",600,600);

  TH2D *h1 = new TH2D("h","",2,-50,50,2,-50,50);
  h1->SetStats(kFALSE);
  h1->Draw();
  for(int i=0;i<6;++i){
    for(int j=0;j<4;++j){
      g1[i][j]->Draw("P");
    }
  }  

  TLegend *l1 = new TLegend(0.6,0.6,0.9,0.9);

  for(int j=0;j<4;++j){
    int i=0;
    char buf[50];
    sprintf(buf,"g1_%d_%d",i,j);
    char buf2[50];
    sprintf(buf2,"Chip%d",j+1);
    l1->AddEntry(buf,buf2,"p");
  }
  l1->Draw();
  /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


  theApp.Run();

}
