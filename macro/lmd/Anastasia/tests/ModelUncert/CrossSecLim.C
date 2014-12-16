// macro for calculation el cross-sections according to DPM model in LMD range
#include <LumiFitStructs.h>
#include <model/PndLmdModelFactory.h>
#include <core/Model1D.h>
#include <LumiModelOptions.h>

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
  
    vector<DataStructs::DimensionRange> range;
    DataStructs::DimensionRange th_range(th1,th2);
    range.push_back(th_range);
    //  range.push_back(make_pair(t1,t2));
    double dcs =  model1d->Integral(range,1e-6);
    cs_loc += dcs;
    // cout<<" for "<<th1<<" - "<<th2<<" "<<dcs<<endl;
  }

   vector<DataStructs::DimensionRange> range_sm;
   DataStructs::DimensionRange rangetemp(4e-3,8e-3);
   range_sm.push_back(rangetemp);
   double cs_loc_sm = model1d->Integral(range_sm,1e-3);

   vector<DataStructs::DimensionRange> range_lg;
   rangetemp.range_low = 3e-3;
   rangetemp.range_high = 9e-3;
   double cs_loc_lg = model1d->Integral(range_lg,1e-3);
  cout<<"For Pbeam = "<<mom<<" from "<<th_dw<<" to "<<th_up<<" mrad cross-section = "<<cs_loc<<" mb"<<endl;
  cout<<"in 4-8 mrad "<<cs_loc_sm<<", in 3-9 mrad "<<cs_loc_lg<<endl;


    return 0;
}
