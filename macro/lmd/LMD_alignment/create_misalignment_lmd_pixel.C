void create_misalignment_lmd_pixel(double dr=0, double da=0, string filename){
  dr*=1e-4;//mkm --> cm
  // da*=1e-5;// 10e-5rad --> rad
  da*=1e-3;// 10e-3rad --> rad
  std::ofstream output;//misalignment params of each sensor
  //output.open("/home/karavdin/pandaRoot12/macro/lmd/lumi.misalign.par");
  output.open(filename.c_str());
  output<<"#################################################################"<<endl;
  output<<"# Alignment parameters for LMD"<<endl;
  output<<"# Format:"<<endl;
  output<<"# parameter_name:parameter_type(i-integer, d-double) parameter_value"<<endl;
  output<<"# PndLmdAlignPar"<<endl;
  output<<"# Description of parameters:"<<endl;
  output<<"#################################################################"<<endl;
  output<<"#################################################################"<<endl;
  output<<"# Alignment parameters for Lmd pixel sensors"<<endl;
  output<<"#################################################################"<<endl;
  output<<"[LMDAlignPar]"<<endl;
  output<<"num_halfs:Int_t 2"<<endl;
  output<<"num_planes:Int_t 4"<<endl;
  output<<"num_modules:Int_t 5"<<endl;
  output<<"num_sensors:Int_t 5"<<endl;
  output<<"num_sides:Int_t 2"<<endl;
  //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  //misalignment
  double dx[2][4][5];
  double dy[2][4][5];
  double dz[2][4][5];
  double dalpha[2][4][5];
  double dbeta[2][4][5];
  double dgamma[2][4][5];

  ///TEST for one sensor only!
  TRandom1 seedgen;
  for(Int_t j=0; j<2; ++j){
    for(Int_t i=0; i<4; ++i){
      for(Int_t m=0; m<5; ++m){
      //      for(Int_t m=0; m<1; ++m){
      //TRandom3 the recommended one, since it has good random proprieties (period of about 10**6000 )and it is fast.
	TRandom3 grand(0);//uniq seed generation
      // int seedgrand = seedgen.Uniform(1e9);
      // grand.SetSeed(seedgrand);
      //      if(m==0){
      dx[j][i][m] = grand.Gaus(0,dr);
      //      seedgrand = seedgen.Uniform(1e9);
      //      grand.SetSeed(seedgrand);
      dy[j][i][m] = grand.Gaus(0,dr);
      //      seedgrand = seedgen.Uniform(1e9);
      //      grand.SetSeed(seedgrand);
      dz[j][i][m] = grand.Gaus(0,dr);
      // seedgrand = seedgen.Uniform(1e7);
      // grand.SetSeed(seedgrand);
      dalpha[j][i][m] = grand.Gaus(0,da);
      // seedgrand = seedgen.Uniform(1e9);
      //      grand.SetSeed(seedgrand);
      dbeta[j][i][m] = grand.Gaus(0,da);
      // seedgrand = seedgen.Uniform(1e9);
      //      grand.SetSeed(seedgrand);
      dgamma[j][i][m] = grand.Gaus(0,da);
      //if(dgamma[j][i][m]<0) dgamma[j][i][m]=-dgamma[j][i][m];

      // dalpha[j][i][m]=0;//TEST
      // dbeta[j][i][m] =0;//TEST
      //      if(i==0) dgamma[j][i][m]=0;
      //      if(i==0) dgamma[j][i][m]=0;
      // } 
      //      if(i==0){
      // if(i!=3){
      // 	dx[j][i][m] = 0;
      // 	dy[j][i][m] = 0;
      // 	dz[j][i][m] = 0;
      // 	dalpha[j][i][m] = 0;
      // 	dbeta[j][i][m] = 0;
      // 	dgamma[j][i][m] = 0;
      // }
      // // else{
      
      // // }
      // cout<<" dx["<<j<<"]["<<i<<"]="<<dx[j][i]<<" ";
      // cout<<" dy["<<j<<"]["<<i<<"]="<<dy[j][i]<<" ";
      // cout<<" dgamma["<<j<<"]["<<i<<"]="<<dgamma[j][i]<<endl;
      output<<"shift_x_"<<j<<"_"<<i<<"_"<<m<<":Double_t "<<dx[j][i][m]<<endl;
      output<<"shift_y_"<<j<<"_"<<i<<"_"<<m<<":Double_t "<<dy[j][i][m]<<endl;
      output<<"shift_z_"<<j<<"_"<<i<<"_"<<m<<":Double_t "<<dz[j][i][m]<<endl;
      output<<"rotate_x_"<<j<<"_"<<i<<"_"<<m<<":Double_t "<<dalpha[j][i][m]<<endl;
      output<<"rotate_y_"<<j<<"_"<<i<<"_"<<m<<":Double_t "<<dbeta[j][i][m]<<endl;
      output<<"rotate_z_"<<j<<"_"<<i<<"_"<<m<<":Double_t "<<dgamma[j][i][m]<<endl;
    }
    // output<<" "<<endl;
    }
  }
  output<<"#################################################################"<<endl;
  output.close();
}  
