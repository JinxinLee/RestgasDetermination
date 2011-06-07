#define EffiContiAll_cxx
// The class definition in EffiContiAll.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("EffiContiAll.C")
// Root > T->Process("EffiContiAll.C","some options")
// Root > T->Process("EffiContiAll.C+")
//

#include <iostream>
#include <cstdlib>
using namespace std;

#include "EffiContiAll.hh"
#include <TH2.h>
#include <TStyle.h>
#include "TMath.h"
using TMath::Abs;
#include "TString.h"
#include "TFile.h"
#include "TTree.h"


EffiContiAll::EffiContiAll()
{
  _notify = false;


// quantum efficiency data from Alex Britting, Jan 25, 2011
// copied from PndDrcHitProducerReal.cxx
// unit is percent

  for(int i=0; i<501; i++) _effi[i] = 0; // index is the wavelength in nm + 200 (min_wave)

  _effi[0]=  231.84;
  _effi[1]=  615.36;
  _effi[2]=  657.4;
  _effi[3]=  258.78;
  _effi[4]=  9839.92;
  _effi[5]=  44.67;
  _effi[6]=  67.87;
  _effi[7]=  51.01;
  _effi[8]=  41.49;
  _effi[9]=  5.36;
  _effi[10]= 49.4;
  _effi[11]= 2.13;
  _effi[12]= 35.49;
  _effi[13]= 8.66;
  _effi[14]= 5.03;
  _effi[15]= 7.51;
  _effi[16]= 13.27;
  _effi[17]= 18.71;
  _effi[18]= 3.92;
  _effi[19]= 3.66;
  _effi[20]= 8.2;
  _effi[21]= 0.56;
  _effi[22]= 7.68;
  _effi[23]= 2.87;
  _effi[24]= 10.06;
  _effi[25]= 3.47;
  _effi[26]= 3.39;
  _effi[27]= 6.99;
  _effi[28]= 6.01;
  _effi[29]= 4.92;
  _effi[30]= 6.25;
  _effi[31]= 5.97;
  _effi[32]= 6.92;
  _effi[33]= 8.29;
  _effi[34]= 10.45;
  _effi[35]= 8.68;
  _effi[36]= 8.6;
  _effi[37]= 9.79;
  _effi[38]= 11.76;
  _effi[39]= 9.53;
  _effi[40]= 10.98;
  _effi[41]= 9.9;
  _effi[42]= 10.97;
  _effi[43]= 11.31;
  _effi[44]= 10.88;
  _effi[45]= 10.78;
  _effi[46]= 12.16;
  _effi[47]= 12.38;
  _effi[48]= 12.37;
  _effi[49]= 13.04;
  _effi[50]= 12.36;
  _effi[51]= 13.18;
  _effi[52]= 13.7;
  _effi[53]= 13.85;
  _effi[54]= 13.66;
  _effi[55]= 13.98;
  _effi[56]= 14.55;
  _effi[57]= 14.93;
  _effi[58]= 14.82;
  _effi[59]= 14.97;
  _effi[60]= 14.98;
  _effi[61]= 15.14;
  _effi[62]= 15.35;
  _effi[63]= 15.37;
  _effi[64]= 15.43;
  _effi[65]= 15.49;
  _effi[66]= 15.59;
  _effi[67]= 15.84;
  _effi[68]= 15.84;
  _effi[69]= 15.92;
  _effi[70]= 16.01;
  _effi[71]= 16.22;
  _effi[72]= 16.41;
  _effi[73]= 16.42;
  _effi[74]= 16.52;
  _effi[75]= 16.86;
  _effi[76]= 17.1;
  _effi[77]= 17.17;
  _effi[78]= 17.22;
  _effi[79]= 17.46;
  _effi[80]= 17.79;
  _effi[81]= 17.99;
  _effi[82]= 18.13;
  _effi[83]= 18.33;
  _effi[84]= 18.34;
  _effi[85]= 18.53;
  _effi[86]= 18.72;
  _effi[87]= 18.95;
  _effi[88]= 19.02;
  _effi[89]= 19.15;
  _effi[90]= 19.28;
  _effi[91]= 19.45;
  _effi[92]= 19.66;
  _effi[93]= 19.69;
  _effi[94]= 19.77;
  _effi[95]= 19.73;
  _effi[96]= 19.95;
  _effi[97]= 19.98;
  _effi[98]= 20.17;
  _effi[99]= 20.29;
  _effi[100]=20.33;
  _effi[101]=20.37;
  _effi[102]=20.47;
  _effi[103]=20.48;
  _effi[104]=20.57;
  _effi[105]=20.75;
  _effi[106]=20.8;
  _effi[107]=20.84;
  _effi[108]=20.86;
  _effi[109]=20.88;
  _effi[110]=21.0;
  _effi[111]=21.06;
  _effi[112]=21.0;
  _effi[113]=21.06;
  _effi[114]=21.06;
  _effi[115]=21.04;
  _effi[116]=21.1;
  _effi[117]=21.14;
  _effi[118]=21.08;
  _effi[119]=21.17;
  _effi[120]=21.3;
  _effi[121]=21.38;
  _effi[122]=21.49;
  _effi[123]=21.58;
  _effi[124]=21.69;
  _effi[125]=21.77;
  _effi[126]=21.87;
  _effi[127]=22.02;
  _effi[128]=22.13;
  _effi[129]=22.29;
  _effi[130]=22.35;
  _effi[131]=22.45;
  _effi[132]=22.53;
  _effi[133]=22.55;
  _effi[134]=22.64;
  _effi[135]=22.67;
  _effi[136]=22.73;
  _effi[137]=22.74;
  _effi[138]=22.71;
  _effi[139]=22.79;
  _effi[140]=22.76;
  _effi[141]=22.77;
  _effi[142]=22.76;
  _effi[143]=22.75;
  _effi[144]=22.78;
  _effi[145]=22.7;
  _effi[146]=22.68;
  _effi[147]=22.72;
  _effi[148]=22.66;
  _effi[149]=22.64;
  _effi[150]=22.7;
  _effi[151]=22.67;
  _effi[152]=22.71;
  _effi[153]=22.67;
  _effi[154]=22.75;
  _effi[155]=22.77;
  _effi[156]=22.83;
  _effi[157]=22.84;
  _effi[158]=22.93;
  _effi[159]=22.97;
  _effi[160]=23.0;
  _effi[161]=23.08;
  _effi[162]=23.16;
  _effi[163]=23.27;
  _effi[164]=23.25;
  _effi[165]=23.37;
  _effi[166]=23.44;
  _effi[167]=23.49;
  _effi[168]=23.55;
  _effi[169]=23.52;
  _effi[170]=23.58;
  _effi[171]=23.64;
  _effi[172]=23.63;
  _effi[173]=23.58;
  _effi[174]=23.64;
  _effi[175]=23.63;
  _effi[176]=23.62;
  _effi[177]=23.64;
  _effi[178]=23.63;
  _effi[179]=23.66;
  _effi[180]=23.59;
  _effi[181]=23.59;
  _effi[182]=23.56;
  _effi[183]=23.58;
  _effi[184]=23.63;
  _effi[185]=23.57;
  _effi[186]=23.66;
  _effi[187]=23.62;
  _effi[188]=23.67;
  _effi[189]=23.64;
  _effi[190]=23.54;
  _effi[191]=23.57;
  _effi[192]=23.51;
  _effi[193]=23.53;
  _effi[194]=23.45;
  _effi[195]=23.3;
  _effi[196]=23.41;
  _effi[197]=23.25;
  _effi[198]=23.21;
  _effi[199]=23.08;
  _effi[200]=23.01;
  _effi[201]=22.92;
  _effi[202]=22.9;
  _effi[203]=22.76;
  _effi[204]=22.76;
  _effi[205]=22.61;
  _effi[206]=22.53;
  _effi[207]=22.48;
  _effi[208]=22.39;
  _effi[209]=22.29;
  _effi[210]=22.24;
  _effi[211]=22.2;
  _effi[212]=22.12;
  _effi[213]=22.07;
  _effi[214]=21.96;
  _effi[215]=21.89;
  _effi[216]=21.87;
  _effi[217]=21.76;
  _effi[218]=21.74;
  _effi[219]=21.58;
  _effi[220]=21.49;
  _effi[221]=21.48;
  _effi[222]=21.37;
  _effi[223]=21.29;
  _effi[224]=21.2;
  _effi[225]=21.17;
  _effi[226]=21.03;
  _effi[227]=20.98;
  _effi[228]=20.92;
  _effi[229]=20.85;
  _effi[230]=20.76;
  _effi[231]=20.69;
  _effi[232]=20.58;
  _effi[233]=20.56;
  _effi[234]=20.47;
  _effi[235]=20.37;
  _effi[236]=20.32;
  _effi[237]=20.24;
  _effi[238]=20.13;
  _effi[239]=20.08;
  _effi[240]=19.9;
  _effi[241]=19.84;
  _effi[242]=19.77;
  _effi[243]=19.69;
  _effi[244]=19.63;
  _effi[245]=19.51;
  _effi[246]=19.41;
  _effi[247]=19.27;
  _effi[248]=19.06;
  _effi[249]=19.01;
  _effi[250]=18.87;
  _effi[251]=18.7;
  _effi[252]=18.49;
  _effi[253]=18.41;
  _effi[254]=18.17;
  _effi[255]=17.98;
  _effi[256]=17.84;
  _effi[257]=17.69;
  _effi[258]=17.5;
  _effi[259]=17.25;
  _effi[260]=17.15;
  _effi[261]=16.98;
  _effi[262]=16.79;
  _effi[263]=16.66;
  _effi[264]=16.48;
  _effi[265]=16.32;
  _effi[266]=16.19;
  _effi[267]=16.02;
  _effi[268]=15.88;
  _effi[269]=15.77;
  _effi[270]=15.67;
  _effi[271]=15.5;
  _effi[272]=15.39;
  _effi[273]=15.23;
  _effi[274]=15.09;
  _effi[275]=15.04;
  _effi[276]=14.92;
  _effi[277]=14.75;
  _effi[278]=14.7;
  _effi[279]=14.5;
  _effi[280]=14.45;
  _effi[281]=14.34;
  _effi[282]=14.25;
  _effi[283]=14.16;
  _effi[284]=14.13;
  _effi[285]=14.0;
  _effi[286]=13.92;
  _effi[287]=13.84;
  _effi[288]=13.76;
  _effi[289]=13.73;
  _effi[290]=13.61;
  _effi[291]=13.54;
  _effi[292]=13.52;
  _effi[293]=13.45;
  _effi[294]=13.41;
  _effi[295]=13.39;
  _effi[296]=13.31;
  _effi[297]=13.22;
  _effi[298]=13.17;
  _effi[299]=13.13;
  _effi[300]=13.06;
  _effi[301]=13.2;
  _effi[302]=13.09;
  _effi[303]=12.97;
  _effi[304]=12.92;
  _effi[305]=12.73;
  _effi[306]=12.65;
  _effi[307]=12.4;
  _effi[308]=12.22;
  _effi[309]=12.02;
  _effi[310]=11.79;
  _effi[311]=11.59;
  _effi[312]=11.33;
  _effi[313]=11.03;
  _effi[314]=10.68;
  _effi[315]=10.46;
  _effi[316]=10.14;
  _effi[317]=9.88;
  _effi[318]=9.62;
  _effi[319]=9.36;
  _effi[320]=9.14;
  _effi[321]=8.87;
  _effi[322]=8.63;
  _effi[323]=8.51;
  _effi[324]=8.24;
  _effi[325]=8.07;
  _effi[326]=7.88;
  _effi[327]=7.77;
  _effi[328]=7.65;
  _effi[329]=7.52;
  _effi[330]=7.35;
  _effi[331]=7.27;
  _effi[332]=7.21;
  _effi[333]=7.1;
  _effi[334]=6.92;
  _effi[335]=6.89;
  _effi[336]=6.79;
  _effi[337]=6.74;
  _effi[338]=6.56;
  _effi[339]=6.54;
  _effi[340]=6.5;
  _effi[341]=6.39;
  _effi[342]=6.33;
  _effi[343]=6.25;
  _effi[344]=6.27;
  _effi[345]=6.14;
  _effi[346]=6.06;
  _effi[347]=6.04;
  _effi[348]=6.01;
  _effi[349]=5.91;
  _effi[350]=5.89;
  _effi[351]=5.79;
  _effi[352]=5.75;
  _effi[353]=5.75;
  _effi[354]=5.67;
  _effi[355]=5.61;
  _effi[356]=5.51;
  _effi[357]=5.52;
  _effi[358]=5.43;
  _effi[359]=5.43;
  _effi[360]=5.34;
  _effi[361]=5.31;
  _effi[362]=5.35;
  _effi[363]=5.23;
  _effi[364]=5.2;
  _effi[365]=5.14;
  _effi[366]=5.11;
  _effi[367]=5.11;
  _effi[368]=5.01;
  _effi[369]=4.98;
  _effi[370]=4.93;
  _effi[371]=4.99;
  _effi[372]=4.89;
  _effi[373]=4.82;
  _effi[374]=4.87;
  _effi[375]=4.8;
  _effi[376]=4.7;
  _effi[377]=4.65;
  _effi[378]=4.65;
  _effi[379]=4.61;
  _effi[380]=4.49;
  _effi[381]=4.56;
  _effi[382]=4.44;
  _effi[383]=4.42;
  _effi[384]=4.44;
  _effi[385]=4.35;
  _effi[386]=4.35;
  _effi[387]=4.27;
  _effi[388]=4.29;
  _effi[389]=4.19;
  _effi[390]=4.13;
  _effi[391]=4.08;
  _effi[392]=4.02;
  _effi[393]=4.07;
  _effi[394]=3.92;
  _effi[395]=3.95;
  _effi[396]=3.88;
  _effi[397]=3.82;
  _effi[398]=3.86;
  _effi[399]=3.74;
  _effi[400]=3.71;
  _effi[401]=3.66;
  _effi[402]=3.72;
  _effi[403]=3.62;
  _effi[404]=3.55;
  _effi[405]=3.56;
  _effi[406]=3.57;
  _effi[407]=3.45;
  _effi[408]=3.38;
  _effi[409]=3.36;
  _effi[410]=3.36;
  _effi[411]=3.28;
  _effi[412]=3.25;
  _effi[413]=3.19;
  _effi[414]=3.26;
  _effi[415]=3.13;
  _effi[416]=3.17;
  _effi[417]=3.15;
  _effi[418]=3.04;
  _effi[419]=2.98;
  _effi[420]=2.93;
  _effi[421]=2.98;
  _effi[422]=2.9;
  _effi[423]=2.89;
  _effi[424]=2.9;
  _effi[425]=2.81;
  _effi[426]=2.74;
  _effi[427]=2.81;
  _effi[428]=2.68;
  _effi[429]=2.73;
  _effi[430]=2.7;
  _effi[431]=2.57;
  _effi[432]=2.58;
  _effi[433]=2.55;
  _effi[434]=2.55;
  _effi[435]=2.37;
  _effi[436]=2.39;
  _effi[437]=2.39;
  _effi[438]=2.44;
  _effi[439]=2.37;
  _effi[440]=2.26;
  _effi[441]=2.27;
  _effi[442]=2.27;
  _effi[443]=2.23;
  _effi[444]=2.26;
  _effi[445]=2.14;
  _effi[446]=2.08;
  _effi[447]=2.15;
  _effi[448]=2.06;
  _effi[449]=2.09;
  _effi[450]=2.04;
  _effi[451]=2.0;
  _effi[452]=1.95;
  _effi[453]=2.02;
  _effi[454]=1.87;
  _effi[455]=1.9;
  _effi[456]=1.8;
  _effi[457]=1.87;
  _effi[458]=1.85;
  _effi[459]=1.87;
  _effi[460]=1.81;
  _effi[461]=1.86;
  _effi[462]=1.74;
  _effi[463]=1.74;
  _effi[464]=1.63;
  _effi[465]=1.59;
  _effi[466]=1.5;
  _effi[467]=1.5;
  _effi[468]=1.44;
  _effi[469]=1.47;
  _effi[470]=1.32;
  _effi[471]=1.24;
  _effi[472]=1.28;
  _effi[473]=1.19;
  _effi[474]=1.21;
  _effi[475]=1.21;
  _effi[476]=1.1;
  _effi[477]=1.1;
  _effi[478]=1.05;
  _effi[479]=1.06;
  _effi[480]=0.94;
  _effi[481]=0.92;
  _effi[482]=0.87;
  _effi[483]=0.92;
  _effi[484]=0.81;
  _effi[485]=0.86;
  _effi[486]=0.78;
  _effi[487]=0.77;
  _effi[488]=0.8;
  _effi[489]=0.67;
  _effi[490]=0.7;
  _effi[491]=0.81;
  _effi[492]=0.61;
  _effi[493]=0.64;
  _effi[494]=0.71;
  _effi[495]=0.66;
  _effi[496]=0.67;
  _effi[497]=0.68;
  _effi[498]=0.69;
  _effi[499]=0.68;
  _effi[500]=0.73;


  for(int i=0; i<501; i++)
  {
    _effi[i] = _effi[i]/100. * 4; // to avoid the removing of too much photons

    if( i < 25 )
      _effi[i] = 0;

    if( _effi[i] > 1 )
    {
      cout << "Effiency greater than 1!" << endl;
      abort();
    }
  }


  _frontLens_thickness = unknown;
  _frontLens_diameter  = unknown;
  _airgap              = unknown;
  _slab_width          = unknown;
  _slab_height         = unknown;
  _fishtank_width      = unknown;
  _fishtank_height     = unknown;
  _refl_limit          = unknown;

  _b_frontLens = false;
  _b_airgap    = false;


  ClearOutTree();
}


void EffiContiAll::ClearOutTree()
{
  _b_effi = false;

  _b_lost_refl   = false;
  _b_lost_bar    = false;
  _b_lost_gap    = false;
  _b_lost_lens   = false;
  _b_lost_airBox = false;
  _b_lost_tank   = false;

  _b_abs_limit = false;
  _b_abs_bar   = false;
  _b_abs_lens  = false;
  _b_abs_tank  = false;
  _b_abs_sides = false;

  _b_back      = false;
  _b_back_lens = false;
  _b_back_tank = false;
}



void EffiContiAll::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();


   _outFilename = _outDirectory + _outFileCore + ".root";
   _outFile = new TFile( _outFilename, "RECREATE" );
   _outTree = new TTree( "tree", _outFilename );

   _outTree->Branch("wavelength", &wavelength, "wavelength/D");
   _outTree->Branch("color"     , &color     , "color/I");
   _outTree->Branch("kBarX"     , &kBarX     , "kBarX/D");
   _outTree->Branch("kBarY"     , &kBarY     , "kBarY/D");
   _outTree->Branch("kBarZ"     , &kBarZ     , "kBarZ/D");
   _outTree->Branch("hitPosX"   , &hitPosX   , "hitPosX/D");
   _outTree->Branch("hitPosY"   , &hitPosY   , "hitPosY/D");
   _outTree->Branch("hitPosZ"   , &hitPosZ   , "hitPosZ/D");
   _outTree->Branch("hitPosDetX", &hitPosDetX, "hitPosDetX/D");
   _outTree->Branch("hitPosDetY", &hitPosDetY, "hitPosDetY/D");
   _outTree->Branch("hitPosDetZ", &hitPosDetZ, "hitPosDetZ/D");
   _outTree->Branch("hitDirX"   , &hitDirX   , "hitDirX/D");
   _outTree->Branch("hitDirY"   , &hitDirY   , "hitDirY/D");
   _outTree->Branch("hitDirZ"   , &hitDirZ   , "hitDirY/D");
   _outTree->Branch("index_pos" , &index_pos , "index_pos/I");
   _outTree->Branch("particleID", &particleID, "particleID/I");
   _outTree->Branch("thetaC"    , &thetaC    , "thetaC/D");
   _outTree->Branch("phiC"      , &phiC      , "phiC/D");
   _outTree->Branch("measured"  , &measured  , "measured/O");
   _outTree->Branch("absorbed"  , &absorbed  , "absorbed/O");
   _outTree->Branch("lost"      , &lost      , "lost/O");
   _outTree->Branch("time"      , &time      , "time/D");
   _outTree->Branch("nRefl"     , &nRefl     , "nRefl/I");

   _outTree->Branch("b_effi"       , &_b_effi       , "b_effi/O");
   _outTree->Branch("b_lost_refl"  , &_b_lost_refl  , "b_lost_refl/O");
   _outTree->Branch("b_lost_bar"   , &_b_lost_bar   , "b_lost_bar/O");
   _outTree->Branch("b_lost_gap"   , &_b_lost_gap   , "b_lost_gap/O");
   _outTree->Branch("b_lost_lens"  , &_b_lost_lens  , "b_lost_lens/O");
   _outTree->Branch("b_lost_airBox", &_b_lost_airBox, "b_lost_airBox/O");
   _outTree->Branch("b_lost_tank"  , &_b_lost_tank  , "b_lost_tank/O");
   _outTree->Branch("b_abs_limit"  , &_b_abs_limit  , "b_abs_limit/O");
   _outTree->Branch("b_abs_bar"    , &_b_abs_bar    , "b_abs_bar/O");
   _outTree->Branch("b_abs_lens"   , &_b_abs_lens   , "b_abs_lens/O");
   _outTree->Branch("b_abs_tank"   , &_b_abs_tank   , "b_abs_tank/O");
   _outTree->Branch("b_abs_sides"  , &_b_abs_sides  , "b_abs_sides/O");
   _outTree->Branch("b_back"       , &_b_back       , "b_back/O");
   _outTree->Branch("b_back_lens"  , &_b_back_lens  , "b_back_lens/O");
   _outTree->Branch("b_back_tank"  , &_b_back_tank  , "b_back_tank/O");

   _outTree->Branch("theta", &_theta, "theta/D");
}

void EffiContiAll::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t EffiContiAll::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either EffiContiAll::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.


  fChain->GetTree()->GetEntry( entry ); // get current event


  for( int j = 0; j < index_pos; j++ )
  {
    if( _b_frontLens && posZ[j] > 0 && posZ[j] <= _frontLens_thickness && posZ[j+1] < posZ[j] && j != index_pos-1)
      _b_back_lens = true;

    if( _b_airgap && posZ[j] == _airgap  && posZ[j+1] < _airgap && j != index_pos-1 )
      _b_back_tank = true;
  }


  if( hitPosZ != posZ[index_pos-1] )
    cout << "Something is wrong!" << endl;


  if( lost )
  {
    if( (Abs(posX[1]) == _slab_width/2 || Abs(posY[1]) == _slab_height/2) && nRefl == 0 && posZ[1] < 0 )
      _b_lost_refl = true;

    if( hitPosZ < 0 && Abs(hitPosX) < (_slab_width/2)+_tiny && Abs(hitPosY) < (_slab_height/2)+_tiny )
      _b_lost_bar = true;
    else if( hitPosZ < _frontLens_thickness && _b_frontLens
             && Abs(hitPosX) <= (_frontLens_diameter/2)+_tiny && Abs(hitPosY) <= (_frontLens_diameter/2)+_tiny )
      _b_lost_lens = true;
    else if( hitPosZ < _airgap && _b_airgap )
      _b_lost_airBox = true;
    else
      _b_lost_tank = true;
  }


  if( absorbed )
  {

    if( hitPosZ < 0 || (hitPosZ == 0 && Abs(hitPosX) <= _slab_width/2 && Abs(hitPosY) <= _slab_height/2) )
      _b_abs_bar = true;
    else if( _b_frontLens && hitPosZ <= _frontLens_thickness )
      _b_abs_lens = true;
    else if( !( Abs(hitPosX) == _fishtank_width/2 || Abs(hitPosY) == _fishtank_height/2 ) )
      _b_abs_tank = true;
    else
      _b_abs_sides = true;
  }


  if( absorbed && nRefl == (_refl_limit + 1)  )
    _b_abs_limit = true;

  if( _b_back_lens || _b_back_tank )
    _b_back = true;

  if( _b_lost_lens || _b_lost_airBox )
    _b_lost_gap = true;


  if( wavelength > 200 && wavelength < 700 && (_rand.Uniform() < _effi[(int)(wavelength - min_wave)]) )
    _b_effi = true;


  _outTree->Fill();
  ClearOutTree();

   return kTRUE;
}

void EffiContiAll::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void EffiContiAll::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

  _outFile->Write();
  _outFile->Close();
  cout << "Root-file " << _outFilename << " was written" << endl;
}
