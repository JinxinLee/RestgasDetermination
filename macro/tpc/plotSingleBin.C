
{
#include <fstream>
#include <string>

gROOT->Reset();
gStyle->SetPalette(1);

//define variables

std::string filename = "singleBin6.dat";

std::ifstream infile(filename.c_str(), std::fstream::in);

int n = 89; //TODO: get this from the single bin file

double contents[n];
double x[n]; 


for (int i=0; i<n; i++) 
{
  infile>>contents[i];
  x[i] = i;
}
  
infile.close();

TGraph* graph = new TGraph(n, x, contents);

//draw
TCanvas* c = new TCanvas(filename.c_str(),filename.c_str(),1);
//gStyle->SetOptStat(0);
graph->SetFillColor(30);
graph->Draw("AB");
c->Update();
}

    
