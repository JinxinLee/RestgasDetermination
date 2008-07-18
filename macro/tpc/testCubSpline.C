//testing script for the spline stuff - needs to be compiled
//Felix

#include "tpc/spacecharge/CubSpline.h"
#include "tpc/spacecharge/CubSplineFitter.h"
#include "tpc/spacecharge/BSpline.h"
#include "tpc/spacecharge/SplineTF1Interface.h"
#include <iostream>
#include <vector>
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TGraph.h"
#include "TArrayD.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TStopwatch.h"

// // helper class ------------------------------------------------
// class Function {
// public:
  
//   void setCSP(CubSpline* csp) {
//     _csp = csp;
//     _knots = _csp->getKnots();
//     _length = _knots->size();
//     _coeffs = _csp->getCoeffs();
//     std::cout<<"ncoeffs="<<_coeffs->size()<<std::endl;
//   }
  
//   CubSpline* getCSP() {return _csp;}
   
//   double eval(double *x, double *p) {
//     //user is responsible for proper length of parameter array!
//     _csp->setCoeffsByArray(p);   // WATCH n_par of TF1 !!!
//     double result = _csp->eval(x[0]);
//     return result;
//   }

// private:
//   CubSpline* _csp;
//   const std::vector<double>* _knots;
//   std::vector<double>* _coeffs;
//   int _length;

// };
// // -------------------------------------------------------------


void testCubSpline() {

int Nknots = 18;
int Nlamda = Nknots+8;
std::vector<double>* knots = new std::vector<double>;
std::vector<double>* coeff = new std::vector<double>;
double lL = 0;
double uL = 15;
double step = (uL - lL)/Nknots;

double k;

for(int i=0; i<Nlamda; ++i){
  k = lL + (i-4)*step;
  knots->push_back(k);
  if(i<Nlamda-4)
    coeff->push_back(i%4);  //alternating 1's and 0's
}

std::cout<<"\n\nThe knots:"<<std::endl;
for(int k=0; k<knots->size(); k++)
  std::cout<<knots->at(k)<<"  ";
std::cout<<std::endl;

// B-Spline testing --------------------------------------------

BSpline* Spline = new BSpline(knots, 10);
Spline->print();
for(int j=0; j<5; j++) {
  std::cout<<"\nValue at x = "<<(knots->at(10)) - j*step<<": "
	   <<Spline->eval(knots->at(10) - j*step);
}

std::cout<<"\nInitialized BSpline\n"<<std::endl;

// -------------------------------------------------------------


TCanvas* c1 = new TCanvas();


// Cub-Spline testing ------------------------------------------
CubSpline* csp = new CubSpline(knots, coeff);
CubSpline* csp2 = new CubSpline(knots);
CubSpline* csp3 = new CubSpline(knots);

SplineTF1Interface* func = new SplineTF1Interface(csp);
SplineTF1Interface* func2 = new SplineTF1Interface(csp2);

TArrayD c_A = TArrayD(Nlamda-4);          //coefficient array

for(int i=0; i<Nlamda-4; i++)
  c_A[i] = coeff->at(i);



// build the histogram ------------------------------------------
TH1D* hist = new TH1D("blub","blub",100,0,14);
for ( Int_t i = 0; i<1000; i++ ) hist->Fill(gRandom->Gaus(6,1));
for ( Int_t i = 0; i<1000; i++ ) hist->Fill(gRandom->Gaus(9,1));

for (int i = 0; i<100; i++) hist->AddBinContent(i);



//WATCH NUMBER OF COEFFICIENTS!!! 
TF1* f1 =new TF1("f1",func,&SplineTF1Interface::eval,
		 0,14,Nlamda-4);
f1->SetNpx(1000);
f1->SetParameters(c_A.GetArray());

std::cout<<"\nInitialized TF1 f1"<<std::endl;

c1->Divide(1,2);
c1->cd(1);

f1->Draw();
c1->cd(2);
 
TF1* f2 =new TF1("f2",func2,&SplineTF1Interface::eval,
		 0,14,Nlamda-4);
 TArrayD c_A2 = TArrayD(Nlamda-4);
 for(int i=0; i<Nlamda-4; i++)
   c_A2[i] = 0;
 f2->SetNpx(1000);
 f2->SetParameters(c_A2.fArray);  //starting params don't matter


//build data -----------------------------------------------------------
std::cout<<"\nBuilding data vector...."<<std::endl;
std::vector<std::vector<double>*> data;
int nbin = hist->GetNbinsX();
TVectorD data_vec = TVectorD(nbin);

 for(int i=0; i<nbin; i++) {
   //if(j) {
     data.push_back(new std::vector<double>(2));
     data[i]->at(0) = (double) i * 14/nbin;  //x coordinate
     data[i]->at(1) = hist->GetBinContent(i+1);
     data_vec[i] = hist->GetBinContent(i+1);
     //}
 }
 std::cout<<"\nData point coordinates: "<<std::endl;
 for(int i=0; i<nbin/5; i++)
   std::cout<<data.at(i)->at(0)<<"  ";
// ---------------------------------------------------------------------

// std::cout<<"\nfirst entries of the data vector... "<<std::endl;
// for(int i=0; i<10; i++)
//   std::cout<<"x: "<<data[i]->at(0)<<"  val: "<<data[i]->at(1)<<std::endl;
      

TStopwatch timer;
timer.Start();

CubSplineFitter* fitter = new CubSplineFitter(csp3, &data);
//TMatrixD* matrix = fitter->getSpMatrix();
//matrix->Print();

fitter->decompose();
TVectorD solution = fitter->solve(data_vec);

timer.Stop();

//TO DO: expand solution by zeros for each removed column 
//(see CubSplineFitter....)

TArrayD sol_arr = TArrayD(Nknots+4);

std::cout<<"\n\nNew solution coefficient vector:"<<std::endl;
for(int i=0; i<Nknots+4; i++){
  sol_arr[i] = solution[i];
  std::cout<<sol_arr[i]<<"  ";
}
std::cout<<std::endl;
std::cout.flush();

double* temp = sol_arr.GetArray();

f2->SetParameters(temp);


hist->Draw();
//  hist->Fit("f2");
//c1->cd(3);
f2->Draw("same");

Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);


}

