
//======
// ROOT
//======
#include <TCanvas.h>
#include <TFile.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TString.h>
#include <TStyle.h>
#include <TTree.h>
#include <TChain.h>
#include <TMath.h>
#include <TRandom3.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void screenPix( TString inFilename = "", Double_t resolution = 100, Bool_t effiMode = true, Bool_t poskBarZ = false ) // resolution in mm (default: 6.375)
{

  if( inFilename == "" )
  {
    cout << "Usage: screenPix( filename, resolution, effiMode )" << endl;
    return;
  }


//==============================================================================
// Access to the input ROOT-file
//==============================================================================
  TFile *inFile = new TFile( inFilename );
  TTree *info   = (TTree*) inFile->Get("info");
  TTree *photon = (TTree*) inFile->Get("photon");


  Double_t fishtank_width, fishtank_height;
  Double_t slab_width, slab_height;
  Double_t parDirX, parDirY, parDirZ;

  info->SetBranchAddress( "fishtank_width" , &fishtank_width );
  info->SetBranchAddress( "fishtank_height", &fishtank_height );
  info->SetBranchAddress( "particle_dirX", &parDirX );
  info->SetBranchAddress( "particle_dirY", &parDirY );
  info->SetBranchAddress( "particle_dirZ", &parDirZ );
  info->GetEntry( 0 );


  Double_t hitPosX, hitPosY;
  Double_t kBarZ;
  Bool_t measured;
  Double_t thetaC, wavelength;
  Double_t phot_parDirX, phot_parDirY, phot_parDirZ;

  photon->SetBranchAddress( "hitPosX"   , &hitPosX );
  photon->SetBranchAddress( "hitPosY"   , &hitPosY );
  photon->SetBranchAddress( "kBarZ"     , &kBarZ );
  photon->SetBranchAddress( "measured"  , &measured );
  photon->SetBranchAddress( "wavelength", &wavelength );
  photon->SetBranchAddress( "thetaC"    , &thetaC );
  if( parDirX == -666 )
  {
    photon->SetBranchAddress( "particle_dirX", &phot_parDirX );
    photon->SetBranchAddress( "particle_dirY", &phot_parDirY );
    photon->SetBranchAddress( "particle_dirZ", &phot_parDirZ );
  }

  Int_t nEntries = photon->GetEntries();


  TString res_str;
  res_str += resolution;
  res_str.Remove( TString::kLeading, ' ' );
  if( res_str.Length() == 5 )
    res_str.Resize(5); // max. 3 digits after comma

  TString effiMode_str = "";
  if( effiMode )
    effiMode_str = "effi_";

  TString poskBarZ_str = "";
  if( poskBarZ )
    poskBarZ_str = "poskBarZ_";

  TString outFilename = inFilename;
  outFilename.Resize( inFilename.Length() - 5 ); // remove file extension .root
  outFilename = outFilename + "_" + res_str + "mm_" + effiMode_str + poskBarZ_str + "screenPix.root";

  TFile *outFile = new TFile( outFilename, "RECREATE" );
  TTree *pixelTree = new TTree( "pixel", outFilename ); // starts at (0,0), (0,1) ...
  TTree *infoTree  = new TTree( "info", outFilename );


  Double_t x_dimMin = -fishtank_width/2;
  Double_t x_dimMax = fishtank_width/2;
  Double_t y_dimMin = -fishtank_height/2;
  Double_t y_dimMax = fishtank_height/2;

  Int_t x_binsH =  TMath::CeilNint(fishtank_width / resolution);
  Int_t y_binsH =  TMath::CeilNint(fishtank_height / resolution);

  const Int_t x_bins = x_binsH;
  const Int_t y_bins = y_binsH;

  if( (Int_t) (fishtank_width*1000) % (Int_t) (resolution*1000) !=0) // max. 3 digits after comma
    x_dimMax = x_bins*resolution - fishtank_width/2;
  if( (Int_t) (fishtank_height*1000) % (Int_t) (resolution*1000) !=0)
    y_dimMax = y_bins*resolution - fishtank_height/2;


  Int_t freq[x_bins][y_bins] = { 0 };
  Int_t px_freq  = 0;

  const Int_t hitsPerPixel = 150; // default: 50 hits per pixel
  Int_t parDir_size = -666;
  if( parDirX == -666 )
    parDir_size = hitsPerPixel;

  TString parDir_size_str;
  parDir_size_str += hitsPerPixel;
  parDir_size_str.Remove( TString::kLeading, ' ' );
  TString parDirX_str = "parDirX[" + parDir_size_str + "]";
  TString parDirY_str = "parDirY[" + parDir_size_str + "]";
  TString parDirZ_str = "parDirZ[" + parDir_size_str + "]";
  TString parDirX_str2 = parDirX_str + "/D";
  TString parDirY_str2 = parDirY_str + "/D";
  TString parDirZ_str2 = parDirZ_str + "/D";

  Double_t parDirX_px[x_bins][y_bins][hitsPerPixel] ={0};
  Double_t parDirY_px[x_bins][y_bins][hitsPerPixel] ={0};
  Double_t parDirZ_px[x_bins][y_bins][hitsPerPixel] ={0};
  Double_t parDirXH[hitsPerPixel]={0};
  Double_t parDirYH[hitsPerPixel]={0};
  Double_t parDirZH[hitsPerPixel]={0};

  pixelTree->Branch( "freq", &px_freq, "freq/I" );
  if( parDirX == -666 )
  {
    pixelTree->Branch( parDirX_str, parDirXH, parDirX_str2 );
    pixelTree->Branch( parDirY_str, parDirYH, parDirY_str2 );
    pixelTree->Branch( parDirZ_str, parDirZH, parDirZ_str2 );
  }
  infoTree->Branch( "parDirX", &parDirX, "parDirX/D" );
  infoTree->Branch( "parDirY", &parDirY, "parDirY/D" );
  infoTree->Branch( "parDirZ", &parDirZ, "parDirZ/D" );
  infoTree->Branch( "thetaC" , &thetaC , "thetaC/D" );
  infoTree->Branch( "fishtank_width" , &fishtank_width , "fishtank_width/D" );
  infoTree->Branch( "fishtank_height", &fishtank_height, "fishtank_height/D" );
  infoTree->Branch( "parDir_size", &parDir_size, "parDir_size/I" );


//==============================================================================
// Canvas settings
//==============================================================================
  TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
  canvas->Draw();

  canvas->SetLeftMargin( 0.13 );
  canvas->SetRightMargin( 0.13 );

  TH2F *screen = new TH2F( "screen","", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );


//==============================================================================
// Photon detector efficiency
//==============================================================================
// old
//     Double_t fEfficiency[70];
//
//     for (int i=0; i<70; i++) fEfficiency[i]=0;
//
//   // the argument is the wavelenght (nm) divided by 10.
//     fEfficiency[29] = 0.06;
//     fEfficiency[30] = 0.10;
//     fEfficiency[31] = 0.16;
//     fEfficiency[32] = 0.22;
//     fEfficiency[33] = 0.27;
//     fEfficiency[34] = 0.28;
//     fEfficiency[35] = 0.285;
//     fEfficiency[36] = 0.31;
//     fEfficiency[37] = 0.31;
//     fEfficiency[38] = 0.30;
//     fEfficiency[39] = 0.29;
//     fEfficiency[40] = 0.28;
//     fEfficiency[41] = 0.27;
//     fEfficiency[42] = 0.26;
//     fEfficiency[43] = 0.25;
//     fEfficiency[44] = 0.24;
//     fEfficiency[45] = 0.23;
//     fEfficiency[46] = 0.21;
//     fEfficiency[47] = 0.19;
//     fEfficiency[48] = 0.18;
//     fEfficiency[49] = 0.17;
//     fEfficiency[50] = 0.16;
//     fEfficiency[51] = 0.14;
//     fEfficiency[52] = 0.12;
//     fEfficiency[53] = 0.10;
//     fEfficiency[54] = 0.07;
//     fEfficiency[55] = 0.05;
//     fEfficiency[56] = 0.04;
//     fEfficiency[57] = 0.03;
//     fEfficiency[58] = 0.03;
//     fEfficiency[59] = 0.02;
//     fEfficiency[60] = 0.01;
//     fEfficiency[61] = 0.0075;
//     fEfficiency[62] = 0.005;
//     fEfficiency[63] = 0.0025;
//
//     for (int i=0; i<70; i++) fEfficiency[i]=fEfficiency[i]*3; // to avoid the removing of too much photons


// quantum efficiency data from Alex Britting, Jan 25, 2011
// copied from PndDrcHitProducerReal.cxx
// unit is percent


  double fEfficiency[501]; // index is the wavelength in nm + 200

  const double min_wave = 200;

  for(int i=0; i<501; i++) fEfficiency[i] = 0;

  fEfficiency[0]=  231.84;
  fEfficiency[1]=  615.36;
  fEfficiency[2]=  657.4;
  fEfficiency[3]=  258.78;
  fEfficiency[4]=  9839.92;
  fEfficiency[5]=  44.67;
  fEfficiency[6]=  67.87;
  fEfficiency[7]=  51.01;
  fEfficiency[8]=  41.49;
  fEfficiency[9]=  5.36;
  fEfficiency[10]= 49.4;
  fEfficiency[11]= 2.13;
  fEfficiency[12]= 35.49;
  fEfficiency[13]= 8.66;
  fEfficiency[14]= 5.03;
  fEfficiency[15]= 7.51;
  fEfficiency[16]= 13.27;
  fEfficiency[17]= 18.71;
  fEfficiency[18]= 3.92;
  fEfficiency[19]= 3.66;
  fEfficiency[20]= 8.2;
  fEfficiency[21]= 0.56;
  fEfficiency[22]= 7.68;
  fEfficiency[23]= 2.87;
  fEfficiency[24]= 10.06;
  fEfficiency[25]= 3.47;
  fEfficiency[26]= 3.39;
  fEfficiency[27]= 6.99;
  fEfficiency[28]= 6.01;
  fEfficiency[29]= 4.92;
  fEfficiency[30]= 6.25;
  fEfficiency[31]= 5.97;
  fEfficiency[32]= 6.92;
  fEfficiency[33]= 8.29;
  fEfficiency[34]= 10.45;
  fEfficiency[35]= 8.68;
  fEfficiency[36]= 8.6;
  fEfficiency[37]= 9.79;
  fEfficiency[38]= 11.76;
  fEfficiency[39]= 9.53;
  fEfficiency[40]= 10.98;
  fEfficiency[41]= 9.9;
  fEfficiency[42]= 10.97;
  fEfficiency[43]= 11.31;
  fEfficiency[44]= 10.88;
  fEfficiency[45]= 10.78;
  fEfficiency[46]= 12.16;
  fEfficiency[47]= 12.38;
  fEfficiency[48]= 12.37;
  fEfficiency[49]= 13.04;
  fEfficiency[50]= 12.36;
  fEfficiency[51]= 13.18;
  fEfficiency[52]= 13.7;
  fEfficiency[53]= 13.85;
  fEfficiency[54]= 13.66;
  fEfficiency[55]= 13.98;
  fEfficiency[56]= 14.55;
  fEfficiency[57]= 14.93;
  fEfficiency[58]= 14.82;
  fEfficiency[59]= 14.97;
  fEfficiency[60]= 14.98;
  fEfficiency[61]= 15.14;
  fEfficiency[62]= 15.35;
  fEfficiency[63]= 15.37;
  fEfficiency[64]= 15.43;
  fEfficiency[65]= 15.49;
  fEfficiency[66]= 15.59;
  fEfficiency[67]= 15.84;
  fEfficiency[68]= 15.84;
  fEfficiency[69]= 15.92;
  fEfficiency[70]= 16.01;
  fEfficiency[71]= 16.22;
  fEfficiency[72]= 16.41;
  fEfficiency[73]= 16.42;
  fEfficiency[74]= 16.52;
  fEfficiency[75]= 16.86;
  fEfficiency[76]= 17.1;
  fEfficiency[77]= 17.17;
  fEfficiency[78]= 17.22;
  fEfficiency[79]= 17.46;
  fEfficiency[80]= 17.79;
  fEfficiency[81]= 17.99;
  fEfficiency[82]= 18.13;
  fEfficiency[83]= 18.33;
  fEfficiency[84]= 18.34;
  fEfficiency[85]= 18.53;
  fEfficiency[86]= 18.72;
  fEfficiency[87]= 18.95;
  fEfficiency[88]= 19.02;
  fEfficiency[89]= 19.15;
  fEfficiency[90]= 19.28;
  fEfficiency[91]= 19.45;
  fEfficiency[92]= 19.66;
  fEfficiency[93]= 19.69;
  fEfficiency[94]= 19.77;
  fEfficiency[95]= 19.73;
  fEfficiency[96]= 19.95;
  fEfficiency[97]= 19.98;
  fEfficiency[98]= 20.17;
  fEfficiency[99]= 20.29;
  fEfficiency[100]=20.33;
  fEfficiency[101]=20.37;
  fEfficiency[102]=20.47;
  fEfficiency[103]=20.48;
  fEfficiency[104]=20.57;
  fEfficiency[105]=20.75;
  fEfficiency[106]=20.8;
  fEfficiency[107]=20.84;
  fEfficiency[108]=20.86;
  fEfficiency[109]=20.88;
  fEfficiency[110]=21.0;
  fEfficiency[111]=21.06;
  fEfficiency[112]=21.0;
  fEfficiency[113]=21.06;
  fEfficiency[114]=21.06;
  fEfficiency[115]=21.04;
  fEfficiency[116]=21.1;
  fEfficiency[117]=21.14;
  fEfficiency[118]=21.08;
  fEfficiency[119]=21.17;
  fEfficiency[120]=21.3;
  fEfficiency[121]=21.38;
  fEfficiency[122]=21.49;
  fEfficiency[123]=21.58;
  fEfficiency[124]=21.69;
  fEfficiency[125]=21.77;
  fEfficiency[126]=21.87;
  fEfficiency[127]=22.02;
  fEfficiency[128]=22.13;
  fEfficiency[129]=22.29;
  fEfficiency[130]=22.35;
  fEfficiency[131]=22.45;
  fEfficiency[132]=22.53;
  fEfficiency[133]=22.55;
  fEfficiency[134]=22.64;
  fEfficiency[135]=22.67;
  fEfficiency[136]=22.73;
  fEfficiency[137]=22.74;
  fEfficiency[138]=22.71;
  fEfficiency[139]=22.79;
  fEfficiency[140]=22.76;
  fEfficiency[141]=22.77;
  fEfficiency[142]=22.76;
  fEfficiency[143]=22.75;
  fEfficiency[144]=22.78;
  fEfficiency[145]=22.7;
  fEfficiency[146]=22.68;
  fEfficiency[147]=22.72;
  fEfficiency[148]=22.66;
  fEfficiency[149]=22.64;
  fEfficiency[150]=22.7;
  fEfficiency[151]=22.67;
  fEfficiency[152]=22.71;
  fEfficiency[153]=22.67;
  fEfficiency[154]=22.75;
  fEfficiency[155]=22.77;
  fEfficiency[156]=22.83;
  fEfficiency[157]=22.84;
  fEfficiency[158]=22.93;
  fEfficiency[159]=22.97;
  fEfficiency[160]=23.0;
  fEfficiency[161]=23.08;
  fEfficiency[162]=23.16;
  fEfficiency[163]=23.27;
  fEfficiency[164]=23.25;
  fEfficiency[165]=23.37;
  fEfficiency[166]=23.44;
  fEfficiency[167]=23.49;
  fEfficiency[168]=23.55;
  fEfficiency[169]=23.52;
  fEfficiency[170]=23.58;
  fEfficiency[171]=23.64;
  fEfficiency[172]=23.63;
  fEfficiency[173]=23.58;
  fEfficiency[174]=23.64;
  fEfficiency[175]=23.63;
  fEfficiency[176]=23.62;
  fEfficiency[177]=23.64;
  fEfficiency[178]=23.63;
  fEfficiency[179]=23.66;
  fEfficiency[180]=23.59;
  fEfficiency[181]=23.59;
  fEfficiency[182]=23.56;
  fEfficiency[183]=23.58;
  fEfficiency[184]=23.63;
  fEfficiency[185]=23.57;
  fEfficiency[186]=23.66;
  fEfficiency[187]=23.62;
  fEfficiency[188]=23.67;
  fEfficiency[189]=23.64;
  fEfficiency[190]=23.54;
  fEfficiency[191]=23.57;
  fEfficiency[192]=23.51;
  fEfficiency[193]=23.53;
  fEfficiency[194]=23.45;
  fEfficiency[195]=23.3;
  fEfficiency[196]=23.41;
  fEfficiency[197]=23.25;
  fEfficiency[198]=23.21;
  fEfficiency[199]=23.08;
  fEfficiency[200]=23.01;
  fEfficiency[201]=22.92;
  fEfficiency[202]=22.9;
  fEfficiency[203]=22.76;
  fEfficiency[204]=22.76;
  fEfficiency[205]=22.61;
  fEfficiency[206]=22.53;
  fEfficiency[207]=22.48;
  fEfficiency[208]=22.39;
  fEfficiency[209]=22.29;
  fEfficiency[210]=22.24;
  fEfficiency[211]=22.2;
  fEfficiency[212]=22.12;
  fEfficiency[213]=22.07;
  fEfficiency[214]=21.96;
  fEfficiency[215]=21.89;
  fEfficiency[216]=21.87;
  fEfficiency[217]=21.76;
  fEfficiency[218]=21.74;
  fEfficiency[219]=21.58;
  fEfficiency[220]=21.49;
  fEfficiency[221]=21.48;
  fEfficiency[222]=21.37;
  fEfficiency[223]=21.29;
  fEfficiency[224]=21.2;
  fEfficiency[225]=21.17;
  fEfficiency[226]=21.03;
  fEfficiency[227]=20.98;
  fEfficiency[228]=20.92;
  fEfficiency[229]=20.85;
  fEfficiency[230]=20.76;
  fEfficiency[231]=20.69;
  fEfficiency[232]=20.58;
  fEfficiency[233]=20.56;
  fEfficiency[234]=20.47;
  fEfficiency[235]=20.37;
  fEfficiency[236]=20.32;
  fEfficiency[237]=20.24;
  fEfficiency[238]=20.13;
  fEfficiency[239]=20.08;
  fEfficiency[240]=19.9;
  fEfficiency[241]=19.84;
  fEfficiency[242]=19.77;
  fEfficiency[243]=19.69;
  fEfficiency[244]=19.63;
  fEfficiency[245]=19.51;
  fEfficiency[246]=19.41;
  fEfficiency[247]=19.27;
  fEfficiency[248]=19.06;
  fEfficiency[249]=19.01;
  fEfficiency[250]=18.87;
  fEfficiency[251]=18.7;
  fEfficiency[252]=18.49;
  fEfficiency[253]=18.41;
  fEfficiency[254]=18.17;
  fEfficiency[255]=17.98;
  fEfficiency[256]=17.84;
  fEfficiency[257]=17.69;
  fEfficiency[258]=17.5;
  fEfficiency[259]=17.25;
  fEfficiency[260]=17.15;
  fEfficiency[261]=16.98;
  fEfficiency[262]=16.79;
  fEfficiency[263]=16.66;
  fEfficiency[264]=16.48;
  fEfficiency[265]=16.32;
  fEfficiency[266]=16.19;
  fEfficiency[267]=16.02;
  fEfficiency[268]=15.88;
  fEfficiency[269]=15.77;
  fEfficiency[270]=15.67;
  fEfficiency[271]=15.5;
  fEfficiency[272]=15.39;
  fEfficiency[273]=15.23;
  fEfficiency[274]=15.09;
  fEfficiency[275]=15.04;
  fEfficiency[276]=14.92;
  fEfficiency[277]=14.75;
  fEfficiency[278]=14.7;
  fEfficiency[279]=14.5;
  fEfficiency[280]=14.45;
  fEfficiency[281]=14.34;
  fEfficiency[282]=14.25;
  fEfficiency[283]=14.16;
  fEfficiency[284]=14.13;
  fEfficiency[285]=14.0;
  fEfficiency[286]=13.92;
  fEfficiency[287]=13.84;
  fEfficiency[288]=13.76;
  fEfficiency[289]=13.73;
  fEfficiency[290]=13.61;
  fEfficiency[291]=13.54;
  fEfficiency[292]=13.52;
  fEfficiency[293]=13.45;
  fEfficiency[294]=13.41;
  fEfficiency[295]=13.39;
  fEfficiency[296]=13.31;
  fEfficiency[297]=13.22;
  fEfficiency[298]=13.17;
  fEfficiency[299]=13.13;
  fEfficiency[300]=13.06;
  fEfficiency[301]=13.2;
  fEfficiency[302]=13.09;
  fEfficiency[303]=12.97;
  fEfficiency[304]=12.92;
  fEfficiency[305]=12.73;
  fEfficiency[306]=12.65;
  fEfficiency[307]=12.4;
  fEfficiency[308]=12.22;
  fEfficiency[309]=12.02;
  fEfficiency[310]=11.79;
  fEfficiency[311]=11.59;
  fEfficiency[312]=11.33;
  fEfficiency[313]=11.03;
  fEfficiency[314]=10.68;
  fEfficiency[315]=10.46;
  fEfficiency[316]=10.14;
  fEfficiency[317]=9.88;
  fEfficiency[318]=9.62;
  fEfficiency[319]=9.36;
  fEfficiency[320]=9.14;
  fEfficiency[321]=8.87;
  fEfficiency[322]=8.63;
  fEfficiency[323]=8.51;
  fEfficiency[324]=8.24;
  fEfficiency[325]=8.07;
  fEfficiency[326]=7.88;
  fEfficiency[327]=7.77;
  fEfficiency[328]=7.65;
  fEfficiency[329]=7.52;
  fEfficiency[330]=7.35;
  fEfficiency[331]=7.27;
  fEfficiency[332]=7.21;
  fEfficiency[333]=7.1;
  fEfficiency[334]=6.92;
  fEfficiency[335]=6.89;
  fEfficiency[336]=6.79;
  fEfficiency[337]=6.74;
  fEfficiency[338]=6.56;
  fEfficiency[339]=6.54;
  fEfficiency[340]=6.5;
  fEfficiency[341]=6.39;
  fEfficiency[342]=6.33;
  fEfficiency[343]=6.25;
  fEfficiency[344]=6.27;
  fEfficiency[345]=6.14;
  fEfficiency[346]=6.06;
  fEfficiency[347]=6.04;
  fEfficiency[348]=6.01;
  fEfficiency[349]=5.91;
  fEfficiency[350]=5.89;
  fEfficiency[351]=5.79;
  fEfficiency[352]=5.75;
  fEfficiency[353]=5.75;
  fEfficiency[354]=5.67;
  fEfficiency[355]=5.61;
  fEfficiency[356]=5.51;
  fEfficiency[357]=5.52;
  fEfficiency[358]=5.43;
  fEfficiency[359]=5.43;
  fEfficiency[360]=5.34;
  fEfficiency[361]=5.31;
  fEfficiency[362]=5.35;
  fEfficiency[363]=5.23;
  fEfficiency[364]=5.2;
  fEfficiency[365]=5.14;
  fEfficiency[366]=5.11;
  fEfficiency[367]=5.11;
  fEfficiency[368]=5.01;
  fEfficiency[369]=4.98;
  fEfficiency[370]=4.93;
  fEfficiency[371]=4.99;
  fEfficiency[372]=4.89;
  fEfficiency[373]=4.82;
  fEfficiency[374]=4.87;
  fEfficiency[375]=4.8;
  fEfficiency[376]=4.7;
  fEfficiency[377]=4.65;
  fEfficiency[378]=4.65;
  fEfficiency[379]=4.61;
  fEfficiency[380]=4.49;
  fEfficiency[381]=4.56;
  fEfficiency[382]=4.44;
  fEfficiency[383]=4.42;
  fEfficiency[384]=4.44;
  fEfficiency[385]=4.35;
  fEfficiency[386]=4.35;
  fEfficiency[387]=4.27;
  fEfficiency[388]=4.29;
  fEfficiency[389]=4.19;
  fEfficiency[390]=4.13;
  fEfficiency[391]=4.08;
  fEfficiency[392]=4.02;
  fEfficiency[393]=4.07;
  fEfficiency[394]=3.92;
  fEfficiency[395]=3.95;
  fEfficiency[396]=3.88;
  fEfficiency[397]=3.82;
  fEfficiency[398]=3.86;
  fEfficiency[399]=3.74;
  fEfficiency[400]=3.71;
  fEfficiency[401]=3.66;
  fEfficiency[402]=3.72;
  fEfficiency[403]=3.62;
  fEfficiency[404]=3.55;
  fEfficiency[405]=3.56;
  fEfficiency[406]=3.57;
  fEfficiency[407]=3.45;
  fEfficiency[408]=3.38;
  fEfficiency[409]=3.36;
  fEfficiency[410]=3.36;
  fEfficiency[411]=3.28;
  fEfficiency[412]=3.25;
  fEfficiency[413]=3.19;
  fEfficiency[414]=3.26;
  fEfficiency[415]=3.13;
  fEfficiency[416]=3.17;
  fEfficiency[417]=3.15;
  fEfficiency[418]=3.04;
  fEfficiency[419]=2.98;
  fEfficiency[420]=2.93;
  fEfficiency[421]=2.98;
  fEfficiency[422]=2.9;
  fEfficiency[423]=2.89;
  fEfficiency[424]=2.9;
  fEfficiency[425]=2.81;
  fEfficiency[426]=2.74;
  fEfficiency[427]=2.81;
  fEfficiency[428]=2.68;
  fEfficiency[429]=2.73;
  fEfficiency[430]=2.7;
  fEfficiency[431]=2.57;
  fEfficiency[432]=2.58;
  fEfficiency[433]=2.55;
  fEfficiency[434]=2.55;
  fEfficiency[435]=2.37;
  fEfficiency[436]=2.39;
  fEfficiency[437]=2.39;
  fEfficiency[438]=2.44;
  fEfficiency[439]=2.37;
  fEfficiency[440]=2.26;
  fEfficiency[441]=2.27;
  fEfficiency[442]=2.27;
  fEfficiency[443]=2.23;
  fEfficiency[444]=2.26;
  fEfficiency[445]=2.14;
  fEfficiency[446]=2.08;
  fEfficiency[447]=2.15;
  fEfficiency[448]=2.06;
  fEfficiency[449]=2.09;
  fEfficiency[450]=2.04;
  fEfficiency[451]=2.0;
  fEfficiency[452]=1.95;
  fEfficiency[453]=2.02;
  fEfficiency[454]=1.87;
  fEfficiency[455]=1.9;
  fEfficiency[456]=1.8;
  fEfficiency[457]=1.87;
  fEfficiency[458]=1.85;
  fEfficiency[459]=1.87;
  fEfficiency[460]=1.81;
  fEfficiency[461]=1.86;
  fEfficiency[462]=1.74;
  fEfficiency[463]=1.74;
  fEfficiency[464]=1.63;
  fEfficiency[465]=1.59;
  fEfficiency[466]=1.5;
  fEfficiency[467]=1.5;
  fEfficiency[468]=1.44;
  fEfficiency[469]=1.47;
  fEfficiency[470]=1.32;
  fEfficiency[471]=1.24;
  fEfficiency[472]=1.28;
  fEfficiency[473]=1.19;
  fEfficiency[474]=1.21;
  fEfficiency[475]=1.21;
  fEfficiency[476]=1.1;
  fEfficiency[477]=1.1;
  fEfficiency[478]=1.05;
  fEfficiency[479]=1.06;
  fEfficiency[480]=0.94;
  fEfficiency[481]=0.92;
  fEfficiency[482]=0.87;
  fEfficiency[483]=0.92;
  fEfficiency[484]=0.81;
  fEfficiency[485]=0.86;
  fEfficiency[486]=0.78;
  fEfficiency[487]=0.77;
  fEfficiency[488]=0.8;
  fEfficiency[489]=0.67;
  fEfficiency[490]=0.7;
  fEfficiency[491]=0.81;
  fEfficiency[492]=0.61;
  fEfficiency[493]=0.64;
  fEfficiency[494]=0.71;
  fEfficiency[495]=0.66;
  fEfficiency[496]=0.67;
  fEfficiency[497]=0.68;
  fEfficiency[498]=0.69;
  fEfficiency[499]=0.68;
  fEfficiency[500]=0.73;


  for(int i=0; i<501; i++)
  {
    fEfficiency[i] = fEfficiency[i]/100. * 4;

    if( i < 100 && fEfficiency[i] > 1 )
      fEfficiency[i] = 0;
  }

  TRandom3 rand;


//==============================================================================
// Event loop
//==============================================================================
  for( int i = 0; i < nEntries; i++ )
  {
    photon->GetEntry( i );

//         if( i%10000 == 0)
//             cout << "Photon: " << i << " / " << nEntries << endl;

    if( measured == true)
    {
      Int_t pxX = -666;
      Int_t pxY = -666;

//             if( TMath::Abs(hitPosY) > 50)
//             {
//             if( !effiMode || ( wavelength < 700 && (rand.Uniform() < fEfficiency[(int)(wavelength/10+0.5)]) ) )
      if( !effiMode || ( wavelength < 700 && (rand.Uniform() < fEfficiency[(int) (wavelength - min_wave)]) ) )
      {
        if( (poskBarZ && kBarZ > 0) || !poskBarZ )
        {
          screen->Fill( hitPosX, hitPosY );

          pxX = TMath::FloorNint( (hitPosX + fishtank_width/2) / resolution );
          pxY = TMath::FloorNint( (hitPosY + fishtank_height/2) / resolution );

          freq[ pxX ][ pxY ]++;

          if( parDirX == -666 )
          {
            Int_t k = 0;
            while( parDirX_px[pxX][pxY][k] != 0) // was initialized with 0
            {
              if( k > hitsPerPixel )
                break;
              else
                k++;
            }
            if( k > hitsPerPixel )
              continue;

            parDirX_px[pxX][pxY][k] = phot_parDirX;
            parDirY_px[pxX][pxY][k] = phot_parDirY;
            parDirZ_px[pxX][pxY][k] = phot_parDirZ;
          }
        }
      }
//             }
    }
  }


//==============================================================================
// Plot histograms
//==============================================================================
//     screen->SetStats(false);
  screen->Draw("colz");
  canvas->Write( "screen");
  canvas->Close();


//     cout << parDir_size << endl;
  infoTree->Fill();

  for( int i = 0; i < x_bins; i++ )
  {
    for( int j = 0; j < y_bins; j++ )
    {
      px_freq = freq[i][j];
      if( parDirX == -666 )
      {
        if( hitsPerPixel < freq[i][j] )
          cout << "More than " << hitsPerPixel << " hits for pixel (" << i << ", " << j << ")  (hits: " << freq[i][j] << ")" << endl;

        for( int k = 0; k < hitsPerPixel; k++)
        {
          parDirXH[k] = parDirX_px[i][j][k];
          parDirYH[k] = parDirY_px[i][j][k];
          parDirZH[k] = parDirZ_px[i][j][k];
        }
      }
      pixelTree->Fill();
    }
  }


  outFile->Write();
  outFile->Close();

  cout << "Root-file " << outFilename << " was written" << endl;
}

