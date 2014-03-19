// macro for calculation el cross-sections according to DPM model in LMD range
#include <LumiFitStructs.h>
#include <PndLmdModelFactory.h>
#include <Model1D.h>

#include <TMath.h>
#include <iostream>

using namespace std;
int main(){
  double mom = 15;
  //  std::vector<PndLmdLumiFitOptions*> fit_options_vec;
  LumiFit::PndLmdFitModelOptions model_options;
  model_options.momentum_transfer_active = true; // will calc cross-section as a func from (t)
  //model_options.momentum_transfer_active = false; // will calc cross-section as a func from (theta), theta in rad

  PndLmdModelFactory model_factory;
  shared_ptr<Model1D> model1d = model_factory.generate1DModel(model_options, mom);

  double th_dw = 1.04719994E-03;//0.06 DPM
  double th_up = TMath::Pi();

  const int nst = 1e3;
  
  double cs_loc = 0;
  for(int i=0;i<nst;i++){
    double dth = (th_up-th_dw)/nst;
    double theta =  th_dw +i*dth;
    double th1 = theta;
    double th2 = theta + dth;
  
    vector<pair<double, double> > range;
    range.push_back(make_pair(th1,th2));
    //  range.push_back(make_pair(t1,t2));
    double dcs =  model1d->Integral(range,1e-6);
    cs_loc += dcs;
    // cout<<" for "<<th1<<" - "<<th2<<" "<<dcs<<endl;
  }

   vector<pair<double, double> > range_sm;
   range_sm.push_back(make_pair(4e-3,8e-3));
   double cs_loc_sm = model1d->Integral(range_sm,1e-3);

   vector<pair<double, double> > range_lg;
   range_lg.push_back(make_pair(3e-3,9e-3));
   double cs_loc_lg = model1d->Integral(range_lg,1e-3);
  cout<<"For Pbeam = "<<mom<<" from "<<th_dw<<" to "<<th_up<<" mrad cross-section = "<<cs_loc<<" mb"<<endl;
  cout<<"in 4-8 mrad "<<cs_loc_sm<<", in 3-9 mrad "<<cs_loc_lg<<endl;


    return 0;
}
