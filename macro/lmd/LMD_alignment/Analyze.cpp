/*
Program reading data prodused by KNOSSOS and produce the root file with results
*/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring> 
// // For ROOT

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "/panda/karavdina/myKnossos/constantsLMD.h"
// needed for corditate from\out lumi frame calculation
#include"PndLmdDim.h"
using namespace std;

int main(int __argc,char *__argv[]) {
  std::string inputpar="", outputpar="",outputrootpar="", resnewconst="";
  // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){

    std::cout << "This is script for reading data produced by KNOSSOS\n"
	      <<"-in input [*.par] file, used in simulation \n"
	      <<"-out output [*.txt] file after KNOSSOS \n"
	      <<"-outroot output [*.root] file after KNOSSOS \n"
	      <<"-new [*.par] file to write new constatn \n"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  }
  while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-in") {
      optind++;
      inputpar = __argv[optind];
      found=true;
    }
    if (sw=="-out"){
      optind++;
      outputpar = __argv[optind];
      found=true;
    }
    if (sw=="-outroot"){
      optind++;
      outputrootpar = __argv[optind];
      found=true;
    }
    if (sw=="-new"){
      optind++;
      resnewconst = __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
  }
  cout<<"outputpar = "<<outputpar<<endl;
  const string inputparc = inputpar;
  const string outputparc = outputpar;
  const string outputrootparc = outputrootpar;
  const string resnewconstc = resnewconst;
//  Initialize

//  gROOT->Reset();
 

  PndLmdDim* lmddim = PndLmdDim::Instance();
  lmddim -> Read_transformation_matrices("/PANDA/pandaroot/macro/lmd/matrices_perfect.txt", false);
  lmddim -> Read_transformation_matrices("/PANDA/pandaroot/macro/lmd/matrices_corrected.txt", true);

 double dxin[2][4][5],dyin[2][4][5],dzin[2][4][5],dalphain[2][4][5],dbetain[2][4][5],dgammain[2][4][5];
 int ih=0;
 int ip=0;
 int im=0;
 // char tmptxt[20];
 std::string tmptxt;
 bool stopreadcrap=true;
 char stopletters[3]={'_','s','i'};// search for num_sides

 //  Load files

 double dxout[2][4][5],dyout[2][4][5],dzout[2][4][5],dalphaout[2][4][5],dbetaout[2][4][5],dgammaout[2][4][5];
 double dxerrout[2][4][5],dyerrout[2][4][5],dzerrout[2][4][5],dalphaerrout[2][4][5],dbetaerrout[2][4][5],dgammaerrout[2][4][5];
 double dxpullout[2][4][5],dypullout[2][4][5],dzpullout[2][4][5],dalphapullout[2][4][5],dbetapullout[2][4][5],dgammapullout[2][4][5];

 std::string tmp;
 ifstream output;
 output.open(outputparc.c_str());
 cout<<"File with misalignment constants obtained by KNOSSOS: "<<outputparc<<endl;
 //while(!output.eof() && ih<2) {
 while(!output.eof() && ih<nSides) {
   output>>dxout[ih][ip][im]>>tmp>>dxerrout[ih][ip][im]>>dxpullout[ih][ip][im];
   output>>dyout[ih][ip][im]>>tmp>>dyerrout[ih][ip][im]>>dypullout[ih][ip][im];
   output>>dzout[ih][ip][im]>>tmp>>dzerrout[ih][ip][im]>>dzpullout[ih][ip][im];
   output>>dalphaout[ih][ip][im]>>tmp>>dalphaerrout[ih][ip][im]>>dalphapullout[ih][ip][im];
   output>>dbetaout[ih][ip][im]>>tmp>>dbetaerrout[ih][ip][im]>>dbetapullout[ih][ip][im];
   output>>dgammaout[ih][ip][im]>>tmp>>dgammaerrout[ih][ip][im]>>dgammapullout[ih][ip][im];
   //   output>>tmp;
   cout<<"dxout["<<ih<<"]["<<ip<<"]["<<im<<"] = "<<dxout[ih][ip][im]<<endl;
   //   im++;
   // im++;
   // if(im==nSectors){
   //   im=0;
   //   ip++;
   // }
   // if(ip==nStation){
   //   ip=0;
   //   ih++;
   // }

   ip++;
   if(ip==nStation){
     ip=0;
     //     ih++;
     im++;
   }
   if(im==nSectors){
     im=0;
     ip=0;
     ih++;
   }
   
 }
 output.close();

 // for(int jh=0;jh<nSides;jh++){
 //   for(int jp=0;jp<nStation;jp++){
 //     for(int jm=0;jm<nSensors;jm++){
 //       TVector3 shift_out(dxout[jh][jp][jm],dyout[jh][jp][jm],dzout[jh][jp][jm]);
 //       TVector3 rot_out(dalphaout[jh][jp][jm],dbetaout[jh][jp][jm],dgammaout[jh][jp][jm]);
 //       cout<<"shift_out (LMD) ["<<jh<<"]["<<jp<<"]["<<jm<<"]"<<endl;
 //       shift_out.Print();
 //       // cout<<"BEFORE: rotation"<<endl;
 //       // rotation.Print();

 //       shift_out = lmddim->Transform_lmd_local_to_module_side(shift_out,jh,jp,jm,0,true,false);
 //       rot_out = lmddim->Transform_lmd_local_to_module_side(rot_out,jh,jp,jm,0,true,false);
 //       cout<<"shift_out (local)"<<endl;
 //       shift_out.Print();
 //       cout<<""<<endl;
 //       // cout<<"AFTER: rotation"<<endl;
 //       // rotation.Print();

 //       dxout[jh][jp][jm] = shift_out.X();
 //       dyout[jh][jp][jm] = shift_out.Y();
 //       dzout[jh][jp][jm] = shift_out.Z();
 //       dalphaout[jh][jp][jm] = rot_out.X();
 //       dbetaout[jh][jp][jm] = rot_out.Y();
 //       dgammaout[jh][jp][jm] = rot_out.Z();
 //     }
 //   }
 // }

 //  TVector3& Transform_lmd_local_to_module_side(const TVector3& point, int ihalf, int iplane, int imodule, int iside, bool isvector = false, bool aligned = true);


 ih=0;
 ip=0;
 im=0;
 ifstream input;
 cout<<"File for misalignmet used in simulation: "<<inputparc<<endl;
 input.open(inputparc.c_str());
 // while(stopreadcrap && !input.eof()){
 while(stopreadcrap){
   input>>tmptxt;
   if(tmptxt[3]==stopletters[0] && tmptxt[4]==stopletters[1] && tmptxt[5]==stopletters[2]){
     input>>tmptxt;
     stopreadcrap = false;
   }
   // cout<<"tmptxt = "<<tmptxt<<endl;
 }

 // while(!input.eof() && ih<2){
 while(!input.eof() && ih<nSides){
   input>>tmptxt>>dxin[ih][ip][im]>>tmptxt>>dyin[ih][ip][im]>>tmptxt>>dzin[ih][ip][im]>>tmptxt>>dalphain[ih][ip][im]>>tmptxt>>dbetain[ih][ip][im]>>tmptxt>>dgammain[ih][ip][im];
   cout<<"h,p,m:"<<ih<<","<<ip<<","<<im<<" dx = "<<dxin[ih][ip][im]<<" dy = "<<dyin[ih][ip][im]<<" dz = "<<dzin[ih][ip][im]<<endl;
   im++;
   if(im==nSectors){
     im=0;
     ip++;
   }
   if(ip==nStation){
     ip=0;
     ih++;
   }
 }
 input.close();



 //
 // then we compute global offset and shearing (need to suppress them because 
 // internal alignment is not sensitive to them)
 //

 // double zplanes[40]={0,0,0,0,0,0,0,0,10,10,10,10,10,10,10,10,20,20,20,20,20,20,20,20,30,30,30,30,30,30,30,30};
 // double zplanes[4]={0,10,20,30};
 double z_moy = 0;
 double sigma_z_moy = 0;
 
 // z_average  
 for (unsigned int jp=0; jp<nStation; jp=jp++){
 // for (unsigned int jp=0; jp<(nSides*nStation*nSensors); jp=jp++){
   z_moy += zplanes[jp];
   //   cout<<"z_moy["<<jp<<"]="<<z_moy<<endl;
 }

 z_moy /= float(nStation);
 //z_moy /= float(nSides*nStation*nSensors);
 // Error on z_moy
 for (unsigned int jp=0; jp<nStation; jp=jp++) sigma_z_moy += (zplanes[jp]-z_moy)*(zplanes[jp]-z_moy);
 sigma_z_moy /= float(nStation);
 //for (unsigned int jp=0; jp<(nSides*nStation*nSensors); jp=jp++) sigma_z_moy += (zplanes[jp]-z_moy)*(zplanes[jp]-z_moy);
 // sigma_z_moy /= float(nSides*nStation*nSensors);
 //sigma_z_moy /= 4.;
 cout<<"z_moy = "<<z_moy<<" sigma_z_moy = "<<sigma_z_moy<<endl;
 
 // double off_x = 0.;   // Global offsetl
 // double shear_x = 0.; // Global shearing
 
 // double off_y = 0.;  
 // double shear_y = 0.;
 
 // double off_z = 0.;  
 // double scale_z = 0.;
 
 // double off_a = 0.;   
 // double off_b = 0.;   
 // double off_c = 0.;   
 

 double off_x[nSectors*nSides];   // Global offsetl
 double shear_x[nSectors*nSides]; // Global shearing
 
 double off_y[nSectors*nSides];  
 double shear_y[nSectors*nSides];
 
 double off_z[nSectors*nSides];  
 double scale_z[nSectors*nSides];
 
 double off_a[nSectors*nSides];   
 double off_b[nSectors*nSides];   
 double off_c[nSectors*nSides];   
 
 for(int jm=0;jm<(nSectors*nSides);jm++){
   off_x[jm] = 0;
   shear_x[jm] = 0; 
   
   off_y[jm] = 0;
   shear_y[jm] = 0;  
   
   off_z[jm] = 0;
   scale_z[jm] = 0; 
   
   off_a[jm] = 0;
   off_b[jm] = 0;
   off_c[jm] = 0;
 }

 for(int jh=0;jh<nSides;jh++){
   for(int jp=0;jp<nStation;jp++){
     for(int jm=0;jm<nSectors;jm++){
       //  int glmod = jh*nStation*nSectors+jp*nSectors+jm;
       int glmod = jp;
       int js=jh*nSectors+jm;
       off_x[js] += dxin[jh][jp][jm];
       //       cout<<"dxin["<<jg<<"]["<<ig<<"] = "<<dxin[jg][ig]<<" dyin["<<jg<<"]["<<ig<<"] = "<<dyin[jg][ig]<<endl;
       shear_x[js] +=  dxin[jh][jp][jm]*(zplanes[glmod]-z_moy)/sigma_z_moy;
       //       cout<<"jh:"<<jh<<" jp:"<<jp<<" jm:"<<jm<<" off_x="<<off_x<<" shear_x = "<<shear_x<<" zplanes ["<<glmod<<"]="<<zplanes[glmod]<<endl;
       off_y[js] += dyin[jh][jp][jm];

       shear_y[js] +=  dyin[jh][jp][jm]*(zplanes[glmod]-z_moy)/sigma_z_moy;
       off_z[js] += dzin[jh][jp][jm];
       scale_z[js] +=  dzin[jh][jp][jm]*(zplanes[glmod]-z_moy)/sigma_z_moy;
       
       off_a[js] += dalphain[jh][jp][jm];  
       off_b[js] += dbetain[jh][jp][jm];  
       off_c[js] += dgammain[jh][jp][jm]; 
       

       // off_x += dxin[jh][jp][jm];
       // //       cout<<"dxin["<<jg<<"]["<<ig<<"] = "<<dxin[jg][ig]<<" dyin["<<jg<<"]["<<ig<<"] = "<<dyin[jg][ig]<<endl;
       // shear_x +=  dxin[jh][jp][jm]*(zplanes[glmod]-z_moy)/sigma_z_moy;
       // //       cout<<"jh:"<<jh<<" jp:"<<jp<<" jm:"<<jm<<" off_x="<<off_x<<" shear_x = "<<shear_x<<" zplanes ["<<glmod<<"]="<<zplanes[glmod]<<endl;
       // off_y += dyin[jh][jp][jm];

       // shear_y +=  dyin[jh][jp][jm]*(zplanes[glmod]-z_moy)/sigma_z_moy;
       // off_z += dzin[jh][jp][jm];
       // scale_z +=  dzin[jh][jp][jm]*(zplanes[glmod]-z_moy)/sigma_z_moy;
       
       // off_a += dalphain[jh][jp][jm];  
       // off_b += dbetain[jh][jp][jm];  
       // off_c += dgammain[jh][jp][jm];  
     }
   }
 }

 for(int jm=0;jm<(nSectors*nSides);jm++){
   off_x[jm] /= float(nStation);
   shear_x[jm] /= float(nStation); 
   
   off_y[jm] /= float(nStation);
   shear_y[jm] /= float(nStation);  
   
   off_z[jm] /= float(nStation);
   scale_z[jm] /= float(nStation); 
   
   off_a[jm] /= float(nStation);
   off_b[jm] /= float(nStation);
   off_c[jm] /= float(nStation);
   cout<<jm<<": off_x = "<<off_x[jm]<<" shear_x = "<<shear_x[jm]<<" off_y = "<<off_y[jm]<<" shear_y = "<<shear_y[jm]
     <<" off_z = "<<off_z[jm]<<"  scale_z = "<<scale_z[jm]<<" off_a = "<<off_a[jm]<<" off_b = "<<off_b[jm]<<" off_c = "<<off_c[jm]<<endl;
 }

   // off_x /= float(nSides*nStation*nSectors);
   // shear_x /= float(nSides*nStation*nSectors); 
   
   // off_y /= float(nSides*nStation*nSectors);
   // shear_y /= float(nSides*nStation*nSectors);  
   
   // off_z /= float(nSides*nStation*nSectors);
   // scale_z /= float(nSides*nStation*nSectors); 
   
   // off_a /= float(nSides*nStation*nSectors);
   // off_b /= float(nSides*nStation*nSectors);
   // off_c  /= float(nSides*nStation*nSectors);
   // cout<<": off_x = "<<off_x<<" shear_x = "<<shear_x<<" off_y = "<<off_y<<" shear_y = "<<shear_y
   //     <<" off_z = "<<off_z<<"  scale_z = "<<scale_z<<" off_a = "<<off_a<<" off_b = "<<off_b<<" off_c = "<<off_c<<endl;

 // off_x =0;//TEST
 // shear_x =0;
 
 // off_y =0;//TEST/= float(nSides*nStation*nSectors);
 // shear_y =0;//TEST/= float(nSides*nStation*nSectors);  
 
 // off_z =0;//TEST/= float(nSides*nStation*nSectors);
 // scale_z =0;//TEST/= float(nSides*nStation*nSectors); 
 
 // off_a =0;//TEST/= float(nSides*nStation*nSectors);
 // off_b =0;//TEST/= float(nSides*nStation*nSectors);
 // off_c =0;//TEST/= float(nSides*nStation*nSectors);

 for(int jh=0;jh<nSides;jh++){
 for(int jp=0;jp<nStation;jp++){
   for(int jm=0;jm<nSectors;jm++){
     //     int glmod = jh*nStation*nSectors+jp*nSectors+jm;
     int glmod = jp;
     int js=jh*nSectors+jm;
     // cout<<"["<<jg<<"]["<<ig<<"]"<<endl;
     //    cout<<"IN (x,y,z):"<<dxin[jh][jp][jm]<<" "<<dyin[jh][jp][jm]<<" "<<dzin[jh][jp][jm]<<endl;
     // cout<<"raw: dxin["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dxin[jh][jp][jm]<<endl;
     dxin[jh][jp][jm] = dxin[jh][jp][jm]-off_x[js]-shear_x[js]*(zplanes[glmod]-z_moy);
     dyin[jh][jp][jm] = dyin[jh][jp][jm]-off_y[js]-shear_y[js]*(zplanes[glmod]-z_moy);
     dzin[jh][jp][jm] = dzin[jh][jp][jm]-off_z[js]-scale_z[js]*(zplanes[glmod]-z_moy);
     // dxin[jh][jp][jm] = dxin[jh][jp][jm]-off_x-shear_x*(zplanes[glmod]-z_moy);
     // dyin[jh][jp][jm] = dyin[jh][jp][jm]-off_y-shear_y*(zplanes[glmod]-z_moy);
     // dzin[jh][jp][jm] = dzin[jh][jp][jm]-off_z-scale_z*(zplanes[glmod]-z_moy);
 
     // // cout<<dxin[jh][jp][jm]<<" "<<dyin[jh][jp][jm]<<" "<<dzin[jh][jp][jm]<<endl;
     // // cout<<"---------------------------------------"<<endl;
     // cout<<"BEFORE correction: ";
     // cout<<"dgamma["<<jh<<"]["<<jp<<"]["<<jm<<"]= "<<dgammain[jh][jp][jm]<<endl;
     dalphain[jh][jp][jm] = dalphain[jh][jp][jm] - off_a[js];
     dbetain[jh][jp][jm] = dbetain[jh][jp][jm] - off_b[js];
     dgammain[jh][jp][jm] = dgammain[jh][jp][jm] - off_c[js];
     // dalphain[jh][jp][jm] = dalphain[jh][jp][jm] - off_a;
     // dbetain[jh][jp][jm] = dbetain[jh][jp][jm]   - off_b;
     // dgammain[jh][jp][jm] = dgammain[jh][jp][jm] - off_c;
     // TVector3 shift_in(dxin[jh][jp][jm],dyin[jh][jp][jm],dzin[jh][jp][jm]);
     // TVector3 rot_in(dalphain[jh][jp][jm],dbetain[jh][jp][jm],dgammain[jh][jp][jm]);
     
     // shift_in = lmddim->Transform_lmd_local_to_module_side(shift_in,jh,jp,jm,0,true,true);
     // rot_in = lmddim->Transform_lmd_local_to_module_side(rot_in,jh,jp,jm,0,true,true);
     // dxin[jh][jp][jm] = shift_in.X();
     // dyin[jh][jp][jm] = shift_in.Y();
     // dzin[jh][jp][jm] = shift_in.Z();
     // dalphain[jh][jp][jm] = rot_in.X();
     // dbetain[jh][jp][jm] = rot_in.Y();
     // dgammain[jh][jp][jm] = rot_in.Z();


    
     cout<<"corr: dxin["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dxin[jh][jp][jm];
     cout<<"     dxout["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dxout[jh][jp][jm];
     cout<<" dif="<<dxin[jh][jp][jm]+dxout[jh][jp][jm]<<endl;
     cout<<"corr: dyin["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dyin[jh][jp][jm];
     cout<<"     dyout["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dyout[jh][jp][jm];
     cout<<" dif="<<dyin[jh][jp][jm]+dyout[jh][jp][jm]<<endl;
     cout<<"corr: dgammain["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dgammain[jh][jp][jm];
     cout<<"     dgammaout["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dgammaout[jh][jp][jm];
     cout<<" dif="<<dgammain[jh][jp][jm]+dgammaout[jh][jp][jm]<<endl;
     cout<<"corr: dyin["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dyin[jh][jp][jm];
     cout<<"     dyout["<<jh<<"]["<<jp<<"]["<<jm<<"]="<<dyout[jh][jp][jm]<<endl;
     cout<<""<<endl;
   }
 }
 }


 //TString outfile= "Results.root";
 TString outfile = outputrootparc;
TFile* fi = new TFile(outfile,"RECREATE");  
 //Create and fill histogramms 
 TObjArray *m_mis_bef = new TObjArray;
 TObjArray *m_mis_aft = new TObjArray;
 TObjArray *m_mis_diff = new TObjArray;
 TObjArray *m_pullHistos = new TObjArray;


  double misal_scales[6];
  const double offsetX = 0.15;
  const double offsetY = 0.15;
  const double offsetZ = 0.15;

  const double offsetA = 0.015;
  const double offsetB = 0.015;
  const double offsetC = 0.015;





  (offsetX == 0) ? misal_scales[0] = 1. : misal_scales[0] = 1.*offsetX;
  (offsetY == 0) ? misal_scales[1] = 1. : misal_scales[1] = 1.*offsetY;
  (offsetZ == 0) ? misal_scales[2] = 1. : misal_scales[2] = 1.*offsetZ;
  (offsetA == 0) ? misal_scales[3] = 1. : misal_scales[3] = 1.*offsetA;
  (offsetB == 0) ? misal_scales[4] = 1. : misal_scales[4] = 1.*offsetB;
  (offsetC == 0) ? misal_scales[5] = 1. : misal_scales[5] = 1.*offsetC;

 for (unsigned int histID = 0; histID < 6; ++histID)
 {
    char histoName[100];
    char histoName2[100];
    char histoName3[100];
    char histoName4[100];

    // sprintf(histoName, "mis_before_%d",histID);
    // TH1F* aHisto = new TH1F(histoName, histoName, 100, -misal_scales[histID], misal_scales[histID]);
    // m_mis_bef->Add(aHisto);

    // sprintf(histoName2, "mis_after_%d",histID);
    // TH1F* aHisto2 = new TH1F(histoName2, histoName2, 100, -misal_scales[histID], misal_scales[histID]);
    // m_mis_aft->Add(aHisto2);
  
                                                                                
    // sprintf(histoName3, "pullHisto%d",histID);
    // TH1F* aHisto3 = new TH1F(histoName3, histoName3, 40, -5, 5);
    // m_pullHistos->Add(aHisto3);

    sprintf(histoName, "mis_before_%d",histID);
    TH2F* aHisto = new TH2F(histoName, histoName, nSides*nSectors*nStation,0,nSides*nSectors*nStation,1e2, -misal_scales[histID], misal_scales[histID]);
    m_mis_bef->Add(aHisto);

    sprintf(histoName2, "mis_after_%d",histID);
    // TH2F* aHisto2 = new TH2F(histoName2, histoName2, nSides*nSectors*nStation,0,32, 100, -0.05*misal_scales[histID], 0.05*misal_scales[histID]);
    TH2F* aHisto2 = new TH2F(histoName2, histoName2, nSides*nSectors*nStation,0,nSides*nSectors*nStation, 1e2, -misal_scales[histID], misal_scales[histID]);
    m_mis_aft->Add(aHisto2);
  
    sprintf(histoName4, "mis_diff_%d",histID);
    TH2F* aHisto4 = new TH2F(histoName4, histoName4, nSides*nSectors*nStation,0,nSides*nSectors*nStation, 1e2, -misal_scales[histID], misal_scales[histID]);
    m_mis_diff->Add(aHisto4);
  
                                                                                
    sprintf(histoName3, "pullHisto%d",histID);
    TH1F* aHisto3 = new TH1F(histoName3, histoName3, 40, -5, 5);
    m_pullHistos->Add(aHisto3);
  }


 for(int jh=0;jh<nSides;jh++){
   for(int jp=0;jp<nStation;jp++){
     for(int jm=0;jm<nSectors;jm++){
     int sensorID=jh*nStation*nSectors+jp*nSectors+jm;
     cout<<"for: "<<jh<<","<<jp<<","<<jm<<" glID="<<sensorID<<" dx = "<<dxin[jh][jp][jm]<<endl;
     // if(sensorID!=0 && sensorID!=8 && sensorID!=16 && sensorID!=24) continue;
     ((TH2F*)m_mis_bef->At(0))->Fill(sensorID,dxin[jh][jp][jm]);
     ((TH2F*)m_mis_bef->At(1))->Fill(sensorID,dyin[jh][jp][jm]);
     ((TH2F*)m_mis_bef->At(2))->Fill(sensorID,dzin[jh][jp][jm]);
     ((TH2F*)m_mis_bef->At(3))->Fill(sensorID,dalphain[jh][jp][jm]);
     ((TH2F*)m_mis_bef->At(4))->Fill(sensorID,dbetain[jh][jp][jm]);
     ((TH2F*)m_mis_bef->At(5))->Fill(sensorID,dgammain[jh][jp][jm]);

     ((TH2F*)m_mis_aft->At(0))->Fill(sensorID,dxout[jh][jp][jm]);
     ((TH2F*)m_mis_aft->At(1))->Fill(sensorID,dyout[jh][jp][jm]);
     ((TH2F*)m_mis_aft->At(2))->Fill(sensorID,dzout[jh][jp][jm]);
     ((TH2F*)m_mis_aft->At(3))->Fill(sensorID,dalphaout[jh][jp][jm]);
     ((TH2F*)m_mis_aft->At(4))->Fill(sensorID,dbetaout[jh][jp][jm]);
     ((TH2F*)m_mis_aft->At(5))->Fill(sensorID,dgammaout[jh][jp][jm]);
     
     ((TH2F*)m_mis_diff->At(0))->Fill(sensorID,dxout[jh][jp][jm]+dxin[jh][jp][jm]);
     ((TH2F*)m_mis_diff->At(1))->Fill(sensorID,dyout[jh][jp][jm]+dyin[jh][jp][jm]);
     ((TH2F*)m_mis_diff->At(2))->Fill(sensorID,dzout[jh][jp][jm]+dzin[jh][jp][jm]);
     ((TH2F*)m_mis_diff->At(3))->Fill(sensorID,dalphaout[jh][jp][jm]+dalphain[jh][jp][jm]);
     ((TH2F*)m_mis_diff->At(4))->Fill(sensorID,dbetaout[jh][jp][jm]+dbetain[jh][jp][jm]);
     ((TH2F*)m_mis_diff->At(5))->Fill(sensorID,dgammaout[jh][jp][jm]+dgammain[jh][jp][jm]);

     // ((TH2F*)m_mis_diff->At(0))->Fill(sensorID,dxout[jh][jp][jm]+dxin[jh][jp][jm]);
     // ((TH2F*)m_mis_diff->At(1))->Fill(sensorID,dyout[jh][jp][jm]+dyin[jh][jp][jm]);
     // ((TH2F*)m_mis_diff->At(2))->Fill(sensorID,dzout[jh][jp][jm]+dzin[jh][jp][jm]);
     // ((TH2F*)m_mis_diff->At(3))->Fill(sensorID,dalphaout[jh][jp][jm]+dalphain[jh][jp][jm]);
     // ((TH2F*)m_mis_diff->At(4))->Fill(sensorID,dbetaout[jh][jp][jm]+dbetain[jh][jp][jm]);
     // ((TH2F*)m_mis_diff->At(5))->Fill(sensorID,dgammaout[jh][jp][jm]+dgammain[jh][jp][jm]);
     //     cout<<"dxout["<<jg<<"]["<<ig<<"]="<<dxout[jg][ig]
     //  	 <<" dxin["<<jg<<"]["<<ig<<"]="<<dxin[jg][ig]<<endl;
     
     ((TH1F*)m_pullHistos->At(0))->Fill(dxpullout[jh][jp][jm]);
     ((TH1F*)m_pullHistos->At(1))->Fill(dypullout[jh][jp][jm]);
     ((TH1F*)m_pullHistos->At(2))->Fill(dzpullout[jh][jp][jm]);
     ((TH1F*)m_pullHistos->At(3))->Fill(dalphapullout[jh][jp][jm]);
     ((TH1F*)m_pullHistos->At(4))->Fill(dbetapullout[jh][jp][jm]);
     ((TH1F*)m_pullHistos->At(5))->Fill(dgammapullout[jh][jp][jm]);
   }
 }
 }
for (unsigned int histID = 0; histID < 6; ++histID){
   ((TH2F*)m_mis_bef->At(histID))->Write();
   ((TH2F*)m_mis_aft->At(histID))->Write();
   ((TH2F*)m_mis_diff->At(histID))->Write();
 }

TCanvas* c0 = new TCanvas("c0","Misalignments before/after",200,500,700,800);
c0->SetFillColor(0);
c0->SetBorderMode(0); 
c0->Divide(2,3);     

c0->cd(1);
 ((TH2F*)m_mis_bef->At(0))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{X}");
 ((TH2F*)m_mis_bef->At(0))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_bef->At(0))->SetMarkerStyle(20);
 ((TH2F*)m_mis_bef->At(0))->Draw("BOX");
 ((TH2F*)m_mis_diff->At(0))->SetMarkerColor(2);
 ((TH2F*)m_mis_diff->At(0))->SetLineColor(2);
 ((TH2F*)m_mis_diff->At(0))->SetMarkerStyle(30);
 ((TH2F*)m_mis_diff->At(0))->Draw("BOX;same");
 // ((TH2F*)m_mis_aft->At(0))->SetMarkerColor(3);
 // ((TH2F*)m_mis_aft->At(0))->SetMarkerStyle(28);
 // ((TH2F*)m_mis_aft->At(0))->Draw("same");

c0->cd(2);
 ((TH2F*)m_mis_bef->At(3))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#alpha}");
 ((TH2F*)m_mis_bef->At(3))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_bef->At(3))->SetMarkerStyle(20);
 ((TH2F*)m_mis_bef->At(3))->Draw("BOX");
 ((TH2F*)m_mis_diff->At(3))->SetMarkerColor(2);
 ((TH2F*)m_mis_diff->At(3))->SetLineColor(2);
 ((TH2F*)m_mis_diff->At(3))->SetMarkerStyle(30);
 ((TH2F*)m_mis_diff->At(3))->Draw("BOX;same");
 // ((TH2F*)m_mis_aft->At(3))->SetMarkerColor(3);
 // ((TH2F*)m_mis_aft->At(3))->SetMarkerStyle(28);
 // ((TH2F*)m_mis_aft->At(3))->Draw("same");
c0->cd(3);
((TH2F*)m_mis_bef->At(1))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{Y}");
((TH2F*)m_mis_bef->At(1))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_bef->At(1))->SetMarkerStyle(20);
((TH2F*)m_mis_bef->At(1))->Draw("BOX");
((TH2F*)m_mis_diff->At(1))->SetMarkerColor(2);
 ((TH2F*)m_mis_diff->At(1))->SetLineColor(2);
 ((TH2F*)m_mis_diff->At(1))->SetMarkerStyle(30);
((TH2F*)m_mis_diff->At(1))->Draw("BOX;same");
 //  ((TH2F*)m_mis_aft->At(1))->SetMarkerColor(3);
 // ((TH2F*)m_mis_aft->At(1))->SetMarkerStyle(28);
 // ((TH2F*)m_mis_aft->At(1))->Draw("same");

c0->cd(4);
((TH2F*)m_mis_bef->At(4))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#beta}");
((TH2F*)m_mis_bef->At(4))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_bef->At(4))->SetMarkerStyle(20);
((TH2F*)m_mis_bef->At(4))->Draw("BOX");
((TH2F*)m_mis_diff->At(4))->SetMarkerColor(2);
 ((TH2F*)m_mis_diff->At(4))->SetLineColor(2);
 ((TH2F*)m_mis_diff->At(4))->SetMarkerStyle(30);
((TH2F*)m_mis_diff->At(4))->Draw("BOX;same");
 // ((TH2F*)m_mis_aft->At(4))->SetMarkerColor(3);
 // ((TH2F*)m_mis_aft->At(4))->SetMarkerStyle(28);
 // ((TH2F*)m_mis_aft->At(4))->Draw("same");

c0->cd(5);
((TH2F*)m_mis_bef->At(2))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{Z}");
((TH2F*)m_mis_bef->At(2))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_bef->At(2))->SetMarkerStyle(20);
((TH2F*)m_mis_bef->At(2))->Draw("BOX");
((TH2F*)m_mis_diff->At(2))->SetMarkerColor(2);
 ((TH2F*)m_mis_diff->At(2))->SetLineColor(2);
 ((TH2F*)m_mis_diff->At(2))->SetMarkerStyle(30);
((TH2F*)m_mis_diff->At(2))->Draw("BOX;same");
 // ((TH2F*)m_mis_aft->At(2))->SetMarkerColor(3);
 // ((TH2F*)m_mis_aft->At(2))->SetMarkerStyle(28);
 // ((TH2F*)m_mis_aft->At(2))->Draw("same");

c0->cd(6);
 ((TH2F*)m_mis_bef->At(5))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#gamma}");
 ((TH2F*)m_mis_bef->At(5))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_bef->At(5))->SetMarkerStyle(20);
 ((TH2F*)m_mis_bef->At(5))->Draw("BOX");
 ((TH2F*)m_mis_diff->At(5))->SetMarkerColor(2);
 ((TH2F*)m_mis_diff->At(5))->SetLineColor(2);
 ((TH2F*)m_mis_diff->At(5))->SetMarkerStyle(30);
 ((TH2F*)m_mis_diff->At(5))->Draw("BOX;same");
// ((TH2F*)m_mis_aft->At(5))->SetMarkerColor(3);
//  ((TH2F*)m_mis_aft->At(5))->SetMarkerStyle(28);
//  ((TH2F*)m_mis_aft->At(5))->Draw("same");
c0->Update();


TCanvas* c1 = new TCanvas("c1","Misalignments before correction",200,500,700,800);
c1->SetFillColor(0);
c1->SetBorderMode(0); 
c1->Divide(2,3);     

c1->cd(1);
((TH2F*)m_mis_bef->At(0))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{X}");
((TH2F*)m_mis_bef->At(0))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_bef->At(0))->Draw();

c1->cd(2);
((TH2F*)m_mis_bef->At(3))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#alpha}");
((TH2F*)m_mis_bef->At(3))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_bef->At(3))->Draw();


c1->cd(3);
((TH2F*)m_mis_bef->At(1))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{Y}");
((TH2F*)m_mis_bef->At(1))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_bef->At(1))->Draw();


c1->cd(4);
((TH2F*)m_mis_bef->At(4))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#beta}");
((TH2F*)m_mis_bef->At(4))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_bef->At(4))->Draw();


c1->cd(5);
((TH1F*)m_mis_bef->At(2))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{Z}");
((TH1F*)m_mis_bef->At(2))->GetYaxis()->SetTitleSize(0.05);
((TH1F*)m_mis_bef->At(2))->Draw();


c1->cd(6);
((TH2F*)m_mis_bef->At(5))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#gamma}");
((TH2F*)m_mis_bef->At(5))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_bef->At(5))->Draw();

c1->Update();

TCanvas* c2 = new TCanvas("c2","Misalignments after correction",200,500,700,800);
c2->SetFillColor(0);
c2->SetBorderMode(0); 
c2->Divide(2,3);     

c2->cd(1);
 ((TH2F*)m_mis_aft->At(0))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{X}");
 ((TH2F*)m_mis_aft->At(0))->GetYaxis()->SetTitleSize(0.05);
 ((TH2F*)m_mis_aft->At(0))->Draw();

c2->cd(2);
((TH2F*)m_mis_aft->At(3))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#alpha}");
((TH2F*)m_mis_aft->At(3))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_aft->At(3))->Draw();

c2->cd(3);
((TH2F*)m_mis_aft->At(1))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{Y}");
((TH2F*)m_mis_aft->At(1))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_aft->At(1))->Draw();

c2->cd(4);
((TH2F*)m_mis_aft->At(4))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#beta}");
((TH2F*)m_mis_aft->At(4))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_aft->At(4))->Draw();

c2->cd(5);
((TH2F*)m_mis_aft->At(2))->GetYaxis()->SetTitle("Misalignment (in cm): #Delta_{Z}");
((TH2F*)m_mis_aft->At(2))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_aft->At(2))->Draw();

c2->cd(6);
((TH2F*)m_mis_aft->At(5))->GetYaxis()->SetTitle("Misalignment (in rad): #Delta_{#gamma}");
((TH2F*)m_mis_aft->At(5))->GetYaxis()->SetTitleSize(0.05);
((TH2F*)m_mis_aft->At(5))->Draw();

c2->Update();

TCanvas* c3 = new TCanvas("c3","Pulls",200,500,700,800);
c3->SetFillColor(0);
c3->SetBorderMode(0); 
c3->Divide(2,3);     

c3->cd(1);
 ((TH1F*)m_pullHistos->At(0))->GetXaxis()->SetTitle("Pull value: #Delta_{X}");
((TH1F*)m_pullHistos->At(0))->GetXaxis()->SetTitleSize(0.05);
((TH1F*)m_pullHistos->At(0))->Draw();

c3->cd(2);
((TH1F*)m_pullHistos->At(3))->GetXaxis()->SetTitle("Pull value: #Delta_{#alpha}");
((TH1F*)m_pullHistos->At(3))->GetXaxis()->SetTitleSize(0.05);
((TH1F*)m_pullHistos->At(3))->Draw();

c3->cd(3);
((TH1F*)m_pullHistos->At(1))->GetXaxis()->SetTitle("Pull value: #Delta_{Y}");
((TH1F*)m_pullHistos->At(1))->GetXaxis()->SetTitleSize(0.05);
((TH1F*)m_pullHistos->At(1))->Draw();

c3->cd(4);
((TH1F*)m_pullHistos->At(4))->GetXaxis()->SetTitle("Pull value: #Delta_{#beta}");
((TH1F*)m_pullHistos->At(4))->GetXaxis()->SetTitleSize(0.05);
((TH1F*)m_pullHistos->At(4))->Draw();

c3->cd(5);
((TH1F*)m_pullHistos->At(2))->GetXaxis()->SetTitle("Pull value: #Delta_{Z}");
((TH1F*)m_pullHistos->At(2))->GetXaxis()->SetTitleSize(0.05);
((TH1F*)m_pullHistos->At(2))->Draw();

c3->cd(6);
((TH1F*)m_pullHistos->At(5))->GetXaxis()->SetTitle("Pull value: #Delta_{#gamma}");
((TH1F*)m_pullHistos->At(5))->GetXaxis()->SetTitleSize(0.05);
((TH1F*)m_pullHistos->At(5))->Draw();

c3->Update();
 
 c0->Write();
 c1->Write();
 c2->Write();
 c3->Write();
 fi->Close();

 cout<<"Now we'll write file for LUMI rec"<<endl;
 std::ofstream lumipar;
 lumipar.open(resnewconstc.c_str(),ios::app);
 for(int jh=0;jh<nSides;jh++){
   for(int jp=0;jp<nStation;jp++){
     for(int jm=0;jm<nSectors;jm++){
       lumipar<<"shift_x_"<<jh<<"_"<<jp<<"_"<<jm<<":Double_t "<<dxout[jh][jp][jm]+dxin[jh][jp][jm]<<endl;
       lumipar<<"shift_y_"<<jh<<"_"<<jp<<"_"<<jm<<":Double_t "<<dyout[jh][jp][jm]+dyin[jh][jp][jm]<<endl;
       lumipar<<"shift_z_"<<jh<<"_"<<jp<<"_"<<jm<<":Double_t "<<dzout[jh][jp][jm]+dzin[jh][jp][jm]<<endl;
       lumipar<<"rotate_x_"<<jh<<"_"<<jp<<"_"<<jm<<":Double_t "<<dalphaout[jh][jp][jm]+dalphain[jh][jp][jm]<<endl;
       lumipar<<"rotate_y_"<<jh<<"_"<<jp<<"_"<<jm<<":Double_t "<<dbetaout[jh][jp][jm]+dbetain[jh][jp][jm]<<endl;
       lumipar<<"rotate_z_"<<jh<<"_"<<jp<<"_"<<jm<<":Double_t "<<dgammaout[jh][jp][jm]+dgammain[jh][jp][jm]<<endl;
     }
   }
   }
 lumipar<<"#################################################################"<<endl;
 lumipar.close();
 cout<<"That's it!"<<endl;
 return 1;
}   
