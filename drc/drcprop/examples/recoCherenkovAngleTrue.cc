
//======
// ROOT
//======
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TMath.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGaxis.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void recoCherenkovAngleTrue(Int_t bining = 100, Bool_t b_effiNEW = true)
{
    Double_t todegree = 180./TMath::Pi();
    Double_t indegree = TMath::Pi()/180.;
    Double_t deg2mrad = TMath::Pi()*1000./180.;
    Double_t mrad2deg = 180./(TMath::Pi()*1000.);

    gStyle->SetStatX(0.97);
    gStyle->SetStatY(0.8);
//     gStyle->SetOptStat("");
    gStyle->SetOptStat("mr");
    gStyle->SetOptFit(111);
    gStyle->SetStatFontSize(0.05);


//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
//     TString beamtestFilename = "forDiss/beamtest0909_angleAcceptance.root";
    TString beamtestFilename = "/d/panda02/rhohler/beamtest1106/test/prototype_angleAcceptance_pion_mom1.7GeV_onlyInBar.root";
//     TString beamtestFilename = "beamtest_theta30_1gev.root";
//     TString beamtestFilename = "/u/rhohler/src/examples/forDiss/beamtest0909_theta27.0_1000par.root";
//     TString beamtestFilename = "/u/rhohler/src/examples/forDiss/beamtest0909_theta21.8_1000par.root";
//     TString beamtestFilename = "/u/rhohler/src/examples/forDiss/beamtest0909_theta45_1000par.root";
//     TString beamtestFilename = "prototype_f250_gap20_theta30.root";

    TFile *beamtestFile = new TFile( beamtestFilename );
    TTree *photon = (TTree*) beamtestFile->Get("photon");
    TTree *info = (TTree*) beamtestFile->Get("info");


    Double_t parDirX, parDirY, parDirZ;
    Double_t lambda_min, lambda_max;

    info->SetBranchAddress( "particle_dirX", &parDirX );
    info->SetBranchAddress( "particle_dirY", &parDirY );
    info->SetBranchAddress( "particle_dirZ", &parDirZ );
    info->SetBranchAddress( "lambda_min"   , &lambda_min );
    info->SetBranchAddress( "lambda_max"   , &lambda_max );
    info->GetEntry( 0 );


    Bool_t mcp_effi = false;
    if( lambda_min != lambda_max )
    {
        mcp_effi = true;
        cout << "MCP efficiency was enabled" << endl;
    }


    Double_t kBarX, kBarY, kBarZ;
    Double_t thetaC;
    Bool_t measured;
    Double_t wavelength, time;
    Double_t hitPosX, hitPosY;

    if( parDirX == -666 )
    {
        photon->SetBranchAddress( "particle_dirX", &parDirX );
        photon->SetBranchAddress( "particle_dirY", &parDirY );
        photon->SetBranchAddress( "particle_dirZ", &parDirZ );
    }
    photon->SetBranchAddress( "kBarX"     , &kBarX );
    photon->SetBranchAddress( "kBarY"     , &kBarY );
    photon->SetBranchAddress( "kBarZ"     , &kBarZ );
    photon->SetBranchAddress( "thetaC"    , &thetaC );
    photon->SetBranchAddress( "measured"  , &measured );
    photon->SetBranchAddress( "wavelength", &wavelength );
    photon->SetBranchAddress( "time"      , &time );
    photon->SetBranchAddress( "hitPosX"   , &hitPosX );
    photon->SetBranchAddress( "hitPosY"   , &hitPosY );

    Int_t nEntries = photon->GetEntries();


//==============================================================================
// Photon detector efficiency
//==============================================================================
    Double_t effiOLD[70];

    for (int i=0; i<70; i++) effiOLD[i]=0;

  // the argument is the wavelenght (nm) divided by 10.
    effiOLD[29] = 0.06;
    effiOLD[30] = 0.10;
    effiOLD[31] = 0.16;
    effiOLD[32] = 0.22;
    effiOLD[33] = 0.27;
    effiOLD[34] = 0.28;
    effiOLD[35] = 0.285;
    effiOLD[36] = 0.31;
    effiOLD[37] = 0.31;
    effiOLD[38] = 0.30;
    effiOLD[39] = 0.29;
    effiOLD[40] = 0.28;
    effiOLD[41] = 0.27;
    effiOLD[42] = 0.26;
    effiOLD[43] = 0.25;
    effiOLD[44] = 0.24;
    effiOLD[45] = 0.23;
    effiOLD[46] = 0.21;
    effiOLD[47] = 0.19;
    effiOLD[48] = 0.18;
    effiOLD[49] = 0.17;
    effiOLD[50] = 0.16;
    effiOLD[51] = 0.14;
    effiOLD[52] = 0.12;
    effiOLD[53] = 0.10;
    effiOLD[54] = 0.07;
    effiOLD[55] = 0.05;
    effiOLD[56] = 0.04;
    effiOLD[57] = 0.03;
    effiOLD[58] = 0.03;
    effiOLD[59] = 0.02;
    effiOLD[60] = 0.01;
    effiOLD[61] = 0.0075;
    effiOLD[62] = 0.005;
    effiOLD[63] = 0.0025;

    for (int i=0; i<70; i++) effiOLD[i]=effiOLD[i]*3; // to avoid the removing of too much photons

    Double_t effi[501];
    const double min_wave = 200;

    for(int i=0; i<501; i++) effi[i] = 0; // index is the wavelength in nm + 200 (min_wave)

    effi[0]=  231.84;
    effi[1]=  615.36;
    effi[2]=  657.4;
    effi[3]=  258.78;
    effi[4]=  9839.92;
    effi[5]=  44.67;
    effi[6]=  67.87;
    effi[7]=  51.01;
    effi[8]=  41.49;
    effi[9]=  5.36;
    effi[10]= 49.4;
    effi[11]= 2.13;
    effi[12]= 35.49;
    effi[13]= 8.66;
    effi[14]= 5.03;
    effi[15]= 7.51;
    effi[16]= 13.27;
    effi[17]= 18.71;
    effi[18]= 3.92;
    effi[19]= 3.66;
    effi[20]= 8.2;
    effi[21]= 0.56;
    effi[22]= 7.68;
    effi[23]= 2.87;
    effi[24]= 10.06;
    effi[25]= 3.47;
    effi[26]= 3.39;
    effi[27]= 6.99;
    effi[28]= 6.01;
    effi[29]= 4.92;
    effi[30]= 6.25;
    effi[31]= 5.97;
    effi[32]= 6.92;
    effi[33]= 8.29;
    effi[34]= 10.45;
    effi[35]= 8.68;
    effi[36]= 8.6;
    effi[37]= 9.79;
    effi[38]= 11.76;
    effi[39]= 9.53;
    effi[40]= 10.98;
    effi[41]= 9.9;
    effi[42]= 10.97;
    effi[43]= 11.31;
    effi[44]= 10.88;
    effi[45]= 10.78;
    effi[46]= 12.16;
    effi[47]= 12.38;
    effi[48]= 12.37;
    effi[49]= 13.04;
    effi[50]= 12.36;
    effi[51]= 13.18;
    effi[52]= 13.7;
    effi[53]= 13.85;
    effi[54]= 13.66;
    effi[55]= 13.98;
    effi[56]= 14.55;
    effi[57]= 14.93;
    effi[58]= 14.82;
    effi[59]= 14.97;
    effi[60]= 14.98;
    effi[61]= 15.14;
    effi[62]= 15.35;
    effi[63]= 15.37;
    effi[64]= 15.43;
    effi[65]= 15.49;
    effi[66]= 15.59;
    effi[67]= 15.84;
    effi[68]= 15.84;
    effi[69]= 15.92;
    effi[70]= 16.01;
    effi[71]= 16.22;
    effi[72]= 16.41;
    effi[73]= 16.42;
    effi[74]= 16.52;
    effi[75]= 16.86;
    effi[76]= 17.1;
    effi[77]= 17.17;
    effi[78]= 17.22;
    effi[79]= 17.46;
    effi[80]= 17.79;
    effi[81]= 17.99;
    effi[82]= 18.13;
    effi[83]= 18.33;
    effi[84]= 18.34;
    effi[85]= 18.53;
    effi[86]= 18.72;
    effi[87]= 18.95;
    effi[88]= 19.02;
    effi[89]= 19.15;
    effi[90]= 19.28;
    effi[91]= 19.45;
    effi[92]= 19.66;
    effi[93]= 19.69;
    effi[94]= 19.77;
    effi[95]= 19.73;
    effi[96]= 19.95;
    effi[97]= 19.98;
    effi[98]= 20.17;
    effi[99]= 20.29;
    effi[100]=20.33;
    effi[101]=20.37;
    effi[102]=20.47;
    effi[103]=20.48;
    effi[104]=20.57;
    effi[105]=20.75;
    effi[106]=20.8;
    effi[107]=20.84;
    effi[108]=20.86;
    effi[109]=20.88;
    effi[110]=21.0;
    effi[111]=21.06;
    effi[112]=21.0;
    effi[113]=21.06;
    effi[114]=21.06;
    effi[115]=21.04;
    effi[116]=21.1;
    effi[117]=21.14;
    effi[118]=21.08;
    effi[119]=21.17;
    effi[120]=21.3;
    effi[121]=21.38;
    effi[122]=21.49;
    effi[123]=21.58;
    effi[124]=21.69;
    effi[125]=21.77;
    effi[126]=21.87;
    effi[127]=22.02;
    effi[128]=22.13;
    effi[129]=22.29;
    effi[130]=22.35;
    effi[131]=22.45;
    effi[132]=22.53;
    effi[133]=22.55;
    effi[134]=22.64;
    effi[135]=22.67;
    effi[136]=22.73;
    effi[137]=22.74;
    effi[138]=22.71;
    effi[139]=22.79;
    effi[140]=22.76;
    effi[141]=22.77;
    effi[142]=22.76;
    effi[143]=22.75;
    effi[144]=22.78;
    effi[145]=22.7;
    effi[146]=22.68;
    effi[147]=22.72;
    effi[148]=22.66;
    effi[149]=22.64;
    effi[150]=22.7;
    effi[151]=22.67;
    effi[152]=22.71;
    effi[153]=22.67;
    effi[154]=22.75;
    effi[155]=22.77;
    effi[156]=22.83;
    effi[157]=22.84;
    effi[158]=22.93;
    effi[159]=22.97;
    effi[160]=23.0;
    effi[161]=23.08;
    effi[162]=23.16;
    effi[163]=23.27;
    effi[164]=23.25;
    effi[165]=23.37;
    effi[166]=23.44;
    effi[167]=23.49;
    effi[168]=23.55;
    effi[169]=23.52;
    effi[170]=23.58;
    effi[171]=23.64;
    effi[172]=23.63;
    effi[173]=23.58;
    effi[174]=23.64;
    effi[175]=23.63;
    effi[176]=23.62;
    effi[177]=23.64;
    effi[178]=23.63;
    effi[179]=23.66;
    effi[180]=23.59;
    effi[181]=23.59;
    effi[182]=23.56;
    effi[183]=23.58;
    effi[184]=23.63;
    effi[185]=23.57;
    effi[186]=23.66;
    effi[187]=23.62;
    effi[188]=23.67;
    effi[189]=23.64;
    effi[190]=23.54;
    effi[191]=23.57;
    effi[192]=23.51;
    effi[193]=23.53;
    effi[194]=23.45;
    effi[195]=23.3;
    effi[196]=23.41;
    effi[197]=23.25;
    effi[198]=23.21;
    effi[199]=23.08;
    effi[200]=23.01;
    effi[201]=22.92;
    effi[202]=22.9;
    effi[203]=22.76;
    effi[204]=22.76;
    effi[205]=22.61;
    effi[206]=22.53;
    effi[207]=22.48;
    effi[208]=22.39;
    effi[209]=22.29;
    effi[210]=22.24;
    effi[211]=22.2;
    effi[212]=22.12;
    effi[213]=22.07;
    effi[214]=21.96;
    effi[215]=21.89;
    effi[216]=21.87;
    effi[217]=21.76;
    effi[218]=21.74;
    effi[219]=21.58;
    effi[220]=21.49;
    effi[221]=21.48;
    effi[222]=21.37;
    effi[223]=21.29;
    effi[224]=21.2;
    effi[225]=21.17;
    effi[226]=21.03;
    effi[227]=20.98;
    effi[228]=20.92;
    effi[229]=20.85;
    effi[230]=20.76;
    effi[231]=20.69;
    effi[232]=20.58;
    effi[233]=20.56;
    effi[234]=20.47;
    effi[235]=20.37;
    effi[236]=20.32;
    effi[237]=20.24;
    effi[238]=20.13;
    effi[239]=20.08;
    effi[240]=19.9;
    effi[241]=19.84;
    effi[242]=19.77;
    effi[243]=19.69;
    effi[244]=19.63;
    effi[245]=19.51;
    effi[246]=19.41;
    effi[247]=19.27;
    effi[248]=19.06;
    effi[249]=19.01;
    effi[250]=18.87;
    effi[251]=18.7;
    effi[252]=18.49;
    effi[253]=18.41;
    effi[254]=18.17;
    effi[255]=17.98;
    effi[256]=17.84;
    effi[257]=17.69;
    effi[258]=17.5;
    effi[259]=17.25;
    effi[260]=17.15;
    effi[261]=16.98;
    effi[262]=16.79;
    effi[263]=16.66;
    effi[264]=16.48;
    effi[265]=16.32;
    effi[266]=16.19;
    effi[267]=16.02;
    effi[268]=15.88;
    effi[269]=15.77;
    effi[270]=15.67;
    effi[271]=15.5;
    effi[272]=15.39;
    effi[273]=15.23;
    effi[274]=15.09;
    effi[275]=15.04;
    effi[276]=14.92;
    effi[277]=14.75;
    effi[278]=14.7;
    effi[279]=14.5;
    effi[280]=14.45;
    effi[281]=14.34;
    effi[282]=14.25;
    effi[283]=14.16;
    effi[284]=14.13;
    effi[285]=14.0;
    effi[286]=13.92;
    effi[287]=13.84;
    effi[288]=13.76;
    effi[289]=13.73;
    effi[290]=13.61;
    effi[291]=13.54;
    effi[292]=13.52;
    effi[293]=13.45;
    effi[294]=13.41;
    effi[295]=13.39;
    effi[296]=13.31;
    effi[297]=13.22;
    effi[298]=13.17;
    effi[299]=13.13;
    effi[300]=13.06;
    effi[301]=13.2;
    effi[302]=13.09;
    effi[303]=12.97;
    effi[304]=12.92;
    effi[305]=12.73;
    effi[306]=12.65;
    effi[307]=12.4;
    effi[308]=12.22;
    effi[309]=12.02;
    effi[310]=11.79;
    effi[311]=11.59;
    effi[312]=11.33;
    effi[313]=11.03;
    effi[314]=10.68;
    effi[315]=10.46;
    effi[316]=10.14;
    effi[317]=9.88;
    effi[318]=9.62;
    effi[319]=9.36;
    effi[320]=9.14;
    effi[321]=8.87;
    effi[322]=8.63;
    effi[323]=8.51;
    effi[324]=8.24;
    effi[325]=8.07;
    effi[326]=7.88;
    effi[327]=7.77;
    effi[328]=7.65;
    effi[329]=7.52;
    effi[330]=7.35;
    effi[331]=7.27;
    effi[332]=7.21;
    effi[333]=7.1;
    effi[334]=6.92;
    effi[335]=6.89;
    effi[336]=6.79;
    effi[337]=6.74;
    effi[338]=6.56;
    effi[339]=6.54;
    effi[340]=6.5;
    effi[341]=6.39;
    effi[342]=6.33;
    effi[343]=6.25;
    effi[344]=6.27;
    effi[345]=6.14;
    effi[346]=6.06;
    effi[347]=6.04;
    effi[348]=6.01;
    effi[349]=5.91;
    effi[350]=5.89;
    effi[351]=5.79;
    effi[352]=5.75;
    effi[353]=5.75;
    effi[354]=5.67;
    effi[355]=5.61;
    effi[356]=5.51;
    effi[357]=5.52;
    effi[358]=5.43;
    effi[359]=5.43;
    effi[360]=5.34;
    effi[361]=5.31;
    effi[362]=5.35;
    effi[363]=5.23;
    effi[364]=5.2;
    effi[365]=5.14;
    effi[366]=5.11;
    effi[367]=5.11;
    effi[368]=5.01;
    effi[369]=4.98;
    effi[370]=4.93;
    effi[371]=4.99;
    effi[372]=4.89;
    effi[373]=4.82;
    effi[374]=4.87;
    effi[375]=4.8;
    effi[376]=4.7;
    effi[377]=4.65;
    effi[378]=4.65;
    effi[379]=4.61;
    effi[380]=4.49;
    effi[381]=4.56;
    effi[382]=4.44;
    effi[383]=4.42;
    effi[384]=4.44;
    effi[385]=4.35;
    effi[386]=4.35;
    effi[387]=4.27;
    effi[388]=4.29;
    effi[389]=4.19;
    effi[390]=4.13;
    effi[391]=4.08;
    effi[392]=4.02;
    effi[393]=4.07;
    effi[394]=3.92;
    effi[395]=3.95;
    effi[396]=3.88;
    effi[397]=3.82;
    effi[398]=3.86;
    effi[399]=3.74;
    effi[400]=3.71;
    effi[401]=3.66;
    effi[402]=3.72;
    effi[403]=3.62;
    effi[404]=3.55;
    effi[405]=3.56;
    effi[406]=3.57;
    effi[407]=3.45;
    effi[408]=3.38;
    effi[409]=3.36;
    effi[410]=3.36;
    effi[411]=3.28;
    effi[412]=3.25;
    effi[413]=3.19;
    effi[414]=3.26;
    effi[415]=3.13;
    effi[416]=3.17;
    effi[417]=3.15;
    effi[418]=3.04;
    effi[419]=2.98;
    effi[420]=2.93;
    effi[421]=2.98;
    effi[422]=2.9;
    effi[423]=2.89;
    effi[424]=2.9;
    effi[425]=2.81;
    effi[426]=2.74;
    effi[427]=2.81;
    effi[428]=2.68;
    effi[429]=2.73;
    effi[430]=2.7;
    effi[431]=2.57;
    effi[432]=2.58;
    effi[433]=2.55;
    effi[434]=2.55;
    effi[435]=2.37;
    effi[436]=2.39;
    effi[437]=2.39;
    effi[438]=2.44;
    effi[439]=2.37;
    effi[440]=2.26;
    effi[441]=2.27;
    effi[442]=2.27;
    effi[443]=2.23;
    effi[444]=2.26;
    effi[445]=2.14;
    effi[446]=2.08;
    effi[447]=2.15;
    effi[448]=2.06;
    effi[449]=2.09;
    effi[450]=2.04;
    effi[451]=2.0;
    effi[452]=1.95;
    effi[453]=2.02;
    effi[454]=1.87;
    effi[455]=1.9;
    effi[456]=1.8;
    effi[457]=1.87;
    effi[458]=1.85;
    effi[459]=1.87;
    effi[460]=1.81;
    effi[461]=1.86;
    effi[462]=1.74;
    effi[463]=1.74;
    effi[464]=1.63;
    effi[465]=1.59;
    effi[466]=1.5;
    effi[467]=1.5;
    effi[468]=1.44;
    effi[469]=1.47;
    effi[470]=1.32;
    effi[471]=1.24;
    effi[472]=1.28;
    effi[473]=1.19;
    effi[474]=1.21;
    effi[475]=1.21;
    effi[476]=1.1;
    effi[477]=1.1;
    effi[478]=1.05;
    effi[479]=1.06;
    effi[480]=0.94;
    effi[481]=0.92;
    effi[482]=0.87;
    effi[483]=0.92;
    effi[484]=0.81;
    effi[485]=0.86;
    effi[486]=0.78;
    effi[487]=0.77;
    effi[488]=0.8;
    effi[489]=0.67;
    effi[490]=0.7;
    effi[491]=0.81;
    effi[492]=0.61;
    effi[493]=0.64;
    effi[494]=0.71;
    effi[495]=0.66;
    effi[496]=0.67;
    effi[497]=0.68;
    effi[498]=0.69;
    effi[499]=0.68;
    effi[500]=0.73;


    for(int i=0; i<501; i++)
    {
      effi[i] = effi[i]/100. * 4; // to avoid the removing of too much photons

      if( i < 25 )
        effi[i] = 0;

      if( effi[i] > 1 )
      {
        cout << "Effiency greater than 1!" << endl;
        abort();
      }
    }

    TRandom3 rand;


//==============================================================================
// Canvas & plot settings
//==============================================================================

    TCanvas *canvas = new TCanvas( "canvas", "" ,1 );
    canvas->Draw();
    canvas->SetTopMargin( 0.14 ); // for 2 x-axis

    TString title = "Cherenkov angle  (par: #Theta = [0,180[#circ, #phi = [0,360[#circ)";
    Double_t xmin_c = 43. *deg2mrad; // number in degree
    Double_t xmax_c = 51. *deg2mrad;
    TH1F *cherenkov = new TH1F( "cherenkov_angle", "", bining, xmin_c, xmax_c); // 40, 48

    cherenkov->GetXaxis()->SetTitle( "#Theta_{c} [mrad]" );
    cherenkov->GetXaxis()->CenterTitle();
    cherenkov->GetYaxis()->SetTitle( "Häufigkeit");
    cherenkov->GetYaxis()->CenterTitle();
    cherenkov->GetYaxis()->SetTitleOffset(1.1);
    cherenkov->SetMinimum(0);


//==============================================================================
// Event loop
//==============================================================================
    Int_t cnt = 0;

    for( int i = 0; i < nEntries; i++ )
    {
        photon->GetEntry( i );

        if( i%100000 == 0 )
            cout << i+1 << " / " << nEntries << endl; // " " << parDirX << " " << parDirY << " " << parDirZ << endl;


        if ( measured )
        {
            cnt++;


            TVector3 parDir( parDirX, parDirY, parDirZ);
            TVector3 kBar( kBarX, kBarY, kBarZ);

            Double_t mag = parDir.Mag() * kBar.Mag();

            if( mag == 0 )
                cout << "?" << endl;


            Double_t angleC[8]; // up, down, left, right, forward, backward; total 2*2*2 possibilities in bar


            kBar.SetX( kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( kBarZ );
            angleC[0] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( -kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( kBarZ );
            angleC[1] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( kBarZ );
            angleC[2] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( -kBarZ );
            angleC[3] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[3]=0;

            kBar.SetX( -kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( kBarZ );
            angleC[4] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( -kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( -kBarZ );
            angleC[5] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[5]=0;

            kBar.SetX( kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( -kBarZ );
            angleC[6] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[6]=0;

            kBar.SetX( -kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( -kBarZ );
            angleC[7] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[7]=0;

//             angleC[3] = 0; // no mirrored photons
//             angleC[5] = 0;
//             angleC[6] = 0;
//             angleC[7] = 0;

            if( mcp_effi )
            {
              if( ( !b_effiNEW && wavelength < 700 && (rand.Uniform() < effiOLD[(int)(wavelength/10+0.5)]) ) ||
                ( b_effiNEW && wavelength > 200 && wavelength < 700 && (rand.Uniform() < effi[(int)(wavelength - min_wave)]) ) )
                {
                    for( int k = 0; k < 8; k++ )
                    {
                        if (angleC[k] !=0)
                            cherenkov->Fill( angleC[k]*1000 );
                    }
                }
                else
                    continue;
            }
            else
            {
                for( int k = 0; k < 8; k++ )
                {
                    if (angleC[k] !=0)
                    cherenkov->Fill( angleC[k]*1000 );

//                     if( angleC[k]*todegree > 45 && angleC[k]*todegree < 45.5 )
//                       cout << k << " " << angleC[k]*todegree << " " << thetaC*todegree << " " << hitPosX << " " << hitPosY << endl;
                }
            }
        }
    }

    cout << "measured photons: " << cnt << endl;


//==============================================================================
// Plot
//==============================================================================
    cherenkov->Draw();

    cout << cherenkov->GetRMS() << " mrad" << endl;

    // Fit
    TF1 f1("f1","gaus(0)+pol0(3)");
//     TF1 f1("f1","gaus(0)");
    f1.SetParNames("A","#mu","#sigma","c");
//     f1.SetParameters(1000,44,0.5,1);
//     f1.SetParLimits(2, 0, 2);
    f1.SetParameters(1000,770,10,1);
    f1.SetLineColor(2);
//     cherenkov->Fit("f1","","",42*deg2mrad,46*deg2mrad);// 42, 46
    cherenkov->Fit("f1","","",45*deg2mrad,49*deg2mrad);// 45, 49
//     cherenkov->Draw("e");

    cout << (f1.GetParameter(2)) << " +- " << (f1.GetParError(2)) << " mrad" << endl;


    TF1 *degreefunc=new TF1("degreefunc","x",xmin_c *mrad2deg, xmax_c *mrad2deg);
    TGaxis *degreeAxis = new TGaxis(xmin_c,26000,xmax_c,26000,"degreefunc",510,"-");
    degreeAxis->SetLabelFont(132);
    degreeAxis->SetLabelSize(0.05);
    degreeAxis->SetTitle("#Theta_{c} [#circ]");
    degreeAxis->SetTitleFont(132);
    degreeAxis->SetTitleSize(0.06);
    degreeAxis->CenterTitle(true);
    degreeAxis->Draw();

//     TH1F *test = new TH1F("test","",100,200,700);
//     for(int i =0; i<501;i++)
//       test->Fill(200+i,effi[i]);
//     for(int i =29; i<64;i++)
//       test->Fill(10*i,effiOLD[i]);
//     test->Draw();
}
