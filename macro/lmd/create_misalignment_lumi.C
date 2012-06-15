void create_misalignment_lumi(double dr=0, double da=0){
  dr*=1e-4;//mkm --> cm
  da*=1e-4;// 10e-4rad --> rad
  std::ofstream output;//misalignment params of each sensor
  output.open("/home/akaravdina/lmdMacro/lumi.misalign.par");
  output<<"#################################################################"<<endl;
 output<<"# Alignment parameters for LMD"<<endl;
 output<<"# Format:"<<endl;
 output<<"# parameter_name:parameter_type(i-integer, d-double) parameter_value"<<endl;
 output<<"# PndLmdAlignPar"<<endl;
 output<<"# Description of parameters:"<<endl;
 output<<"#################################################################"<<endl;
 output<<"#################################################################"<<endl;
 output<<"# Alignment parameters for TRAPEZOID Lmd strip sensors"<<endl;
 output<<"#################################################################"<<endl;
 output<<"[LMDAlignPar]"<<endl;
 output<<"num_sensors:Int_t 8"<<endl;
 output<<"num_sides:Int_t 1"<<endl;
 output<<"num_planes:Int_t 4"<<endl;
  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  //misalignment
  double dx[4][8];
  double dy[4][8];
  double dz[4][8];
  double dalpha[4][8];
  double dbeta[4][8];
  double dgamma[4][8];

 //  for(Int_t j=0; j<4; ++j){
 //    for(Int_t i=0; i<8; ++i){
 //      TRandom1 grand;
 //      dx[j][i] = grand.Gaus(0,dr);
 //      dy[j][i] = grand.Gaus(0,dr);
 //      dz[j][i] = grand.Gaus(0,dr);
 //      dalpha[j][i] = grand.Gaus(0,da);
 //      dbeta[j][i] = grand.Gaus(0,da);
 //      dgamma[j][i] = grand.Gaus(0,da);
 //      // dz[j][i] = 0.;
 //      // dalpha[j][i] = 0.;
 //      // dbeta[j][i] = 0.;
 //      // dgamma[j][i] = 0.;

 //      cout<<" dx["<<j<<"]["<<i<<"]="<<dx[j][i]<<" ";
 //      cout<<" dy["<<j<<"]["<<i<<"]="<<dy[j][i]<<" ";
 //      cout<<" dgamma["<<j<<"]["<<i<<"]="<<dgamma[j][i]<<endl;
 //      output<<"shift_x_"<<j<<"_"<<i<<":Double_t "<<dx[j][i]<<endl;
 //      output<<"shift_y_"<<j<<"_"<<i<<":Double_t "<<dy[j][i]<<endl;
 //      output<<"shift_z_"<<j<<"_"<<i<<":Double_t "<<dz[j][i]<<endl;
 //      output<<"rotate_x_"<<j<<"_"<<i<<":Double_t "<<dalpha[j][i]<<endl;
 //      output<<"rotate_y_"<<j<<"_"<<i<<":Double_t "<<dbeta[j][i]<<endl;
 //      output<<"rotate_z_"<<j<<"_"<<i<<":Double_t "<<dgamma[j][i]<<endl;
 //    }
 //    // output<<" "<<endl;
 // }

  ///TEST for one sensor only!
  for(Int_t j=0; j<4; ++j){
    for(Int_t i=0; i<8; ++i){
      TRandom1 grand;
      // //  if(j==0){
      // if(j>-1){
	dx[j][i] = grand.Gaus(0,dr);
	dy[j][i] = grand.Gaus(0,dr);
      // }
      // else{
      // 	dx[j][i] = 0;
      // 	dy[j][i] = 0;
      // }
      dz[j][i] = grand.Gaus(0,dr);
      dalpha[j][i] = grand.Gaus(0,da);
      dbeta[j][i] = grand.Gaus(0,da);
      dgamma[j][i] = grand.Gaus(0,da);
      // dz[j][i] = 0.;
      // dalpha[j][i] = 0.;
      // dbeta[j][i] = 0.;
      // dgamma[j][i] = 0.;

      cout<<" dx["<<j<<"]["<<i<<"]="<<dx[j][i]<<" ";
      cout<<" dy["<<j<<"]["<<i<<"]="<<dy[j][i]<<" ";
      cout<<" dgamma["<<j<<"]["<<i<<"]="<<dgamma[j][i]<<endl;
      output<<"shift_x_"<<j<<"_"<<i<<":Double_t "<<dx[j][i]<<endl;
      output<<"shift_y_"<<j<<"_"<<i<<":Double_t "<<dy[j][i]<<endl;
      output<<"shift_z_"<<j<<"_"<<i<<":Double_t "<<dz[j][i]<<endl;
      output<<"rotate_x_"<<j<<"_"<<i<<":Double_t "<<dalpha[j][i]<<endl;
      output<<"rotate_y_"<<j<<"_"<<i<<":Double_t "<<dbeta[j][i]<<endl;
      output<<"rotate_z_"<<j<<"_"<<i<<":Double_t "<<dgamma[j][i]<<endl;
    }
    // output<<" "<<endl;
 }
  output<<"#################################################################"<<endl;
  output.close();
}  
