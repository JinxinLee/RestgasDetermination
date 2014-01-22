// macro for calculation el cross-sections according to DPM model in LMD range
//include <PndLmdDPMAngModel1D.h>
#include <PndLmdLumiFitOptions.h>
#include <PndLmdModelFactory.h>
#include <Model1D.h>
#include <PndLmdLumiHelper.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <iostream>

using namespace std;
int main(){
  double mom = 15;
  //  std::vector<PndLmdLumiFitOptions*> fit_options_vec;
  PndLmdLumiFitOptions *temp_fit_opt = new PndLmdLumiFitOptions();
  LumiFit::LmdBinaryFitOptions bit_fit_opt(0);
  bit_fit_opt.setFitAsRaw(false);// will calc cross-section as a func from (theta), theta in rad
  //  bit_fit_opt.setFitAsRaw(true);// will calc cross-section as a func from (t)
  temp_fit_opt->setModelBinaryOptions(bit_fit_opt);

  // double fit_range_low = 2.0;
  // double fit_range_high = 10.0;
  // double lower_shift=0;
  // temp_fit_opt = new PndLmdLumiFitOptions(bit_fit_opt, 0, 0, 1, mom, fit_range_low+lower_shift,
  // 					  fit_range_high);

  PndLmdModelFactory model_factory;
  shared_ptr<Model1D> model1d = model_factory.generate1DModel(temp_fit_opt, mom);

  // double th_dw = 4e-3;
  // double th_up = 8e-3;

  // double th_dw = 3e-3;
  // double th_up = 9e-3;

  //  double th_dw = 2.09439510239319525e-03; //calc from 0.12 degree
  //  double th_dw = 2.09439988e-03;//DPM printout
  double th_dw = 1.04719994E-03;//0.06 DPM
  double th_up = TMath::Pi();
  // double  Tmin=  -9.86963369E-06;
  // double  Tmax=  -1.55898321;
  // vector<pair<double, double> > ranget;
  // ranget.push_back(make_pair(Tmax,Tmin));
  // double cs_loc_t = model1d->Integral(ranget,1e-6);
  // cout<<" CS = "<<cs_loc_t <<endl;
  // double t_min =  PndLmdLumiHelper::getMomentumTransferFromTheta(mom, th_dw);
  // double t_max =  PndLmdLumiHelper::getMomentumTransferFromTheta(mom, TMath::Pi());
  // cout<<"t_min = "<<t_min<<" t_MIN_DPM = "<<Tmin<<" |t_fromTh| - |t_DPM| = "<<fabs(t_min)-fabs(Tmin)<<endl;
  // vector<pair<double, double> > rangetl;
  // rangetl.push_back(make_pair(Tmin,Tmax));

  // double th_dw = 1e-3;
  // double th_up = 1e-2;
  const int nst = 1e3;
  //  double vt[nst];
  //  double vcs[nst];
  // vector<pair<double, double> > range;
  // range.push_back(make_pair(th_dw,th_up));
  // double cs_loc = model1d->Integral(range,1e-1);
  double cs_loc = 0;
  for(int i=0;i<nst;i++){
    double dth = (th_up-th_dw)/nst;
    double theta =  th_dw +i*dth;
    double th1 = theta;
    double th2 = theta + dth;
    //double t1 =  PndLmdLumiHelper::getMomentumTransferFromTheta(mom,th1);
    //    double t2 =  PndLmdLumiHelper::getMomentumTransferFromTheta(mom,th2);
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


  //   // Integral(std::vector<std::pair<double, double> > &ranges, double precision);
   
  //   //  double theta = 1e-3;
  //   //  double theta = 0.008;
  //   //  double t =  PndLmdLumiHelper::getMomentumTransferFromTheta(mom,theta);

  //   //    double cs_loc = model1d->evaluate(&t);
  //   vt[i] = t;
  //   //    vt[i] = theta;
  //   vcs[i] = cs_loc;
  //   //double cs_loc = model1d->evaluate(&theta);
  //   //    std::cout<<"cs_loc =  "<<cs_loc<<std::endl;
  //   //    std::cout<<t<<" "<<cs_loc<<std::endl;
  // }
  // TGraph *cs_t = new TGraph(nst,vt,vcs);
  // // TCanvas c1;
  // // cs_t->Draw("A*");
  // // c1.SaveAs("cs_test.pdf");
  // double integ = cs_t->Integral();
  cout<<"For Pbeam = "<<mom<<" from "<<th_dw<<" to "<<th_up<<" mrad cross-section = "<<cs_loc<<" mb"<<endl;
  cout<<"in 4-8 mrad "<<cs_loc_sm<<", in 3-9 mrad "<<cs_loc_lg<<endl;


    return 0;
}
