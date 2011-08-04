#define KBarAnalysis_cxx
// The class definition in KBarAnalysis.h has been generated automatically
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
// Root > T->Process("KBarAnalysis.C")
// Root > T->Process("KBarAnalysis.C","some options")
// Root > T->Process("KBarAnalysis.C+")
//

#include <iostream>
#include <cstdlib>
using namespace std;

#include "KBarAnalysis.hh"
#include "TStyle.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"


// KBarAnalysis::KBarAnalysis(TTree * /*tree*/)
KBarAnalysis::KBarAnalysis( Int_t n_mcp )//: _n_mcp(n_mcp) // initialize const variable
{
  _n_mcp = n_mcp;

  if( _n_mcp < 0 )
  {
    cout << "Negative number of MCPs!" << endl;
    abort();
  }


  _notify = false;
  _mcpMode = false;
  _effiMode = false;


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


  _fishtank_width  = unknown;
  _fishtank_height = unknown;
  _airgap          = unknown;

  _resolution = unknown;

  _mcpLabel_str.resize( _n_mcp );

  _mcp_dim.resize( _n_mcp );
  _mcp_act.resize( _n_mcp );
  _mcp_det.resize( _n_mcp );

  _minX_dim.resize( _n_mcp );
  _minY_dim.resize( _n_mcp );
  _maxX_dim.resize( _n_mcp );
  _maxY_dim.resize( _n_mcp );

  _minX_det.resize( _n_mcp );
  _minY_det.resize( _n_mcp );
  _maxX_det.resize( _n_mcp );
  _maxY_det.resize( _n_mcp );

  _shiftX.resize( _n_mcp );
  _shiftY.resize( _n_mcp );

  _x_bins.resize( _n_mcp );
  _y_bins.resize( _n_mcp );

  for( int i = 0; i < _n_mcp; i++ )
  {
    _mcp_dim[i] = unknown;
    _mcp_det[i] = unknown;

    _minX_dim[i] = unknown;
    _minY_dim[i] = unknown;
    _maxX_dim[i] = unknown;
    _maxY_dim[i] = unknown;

    _minX_det[i] = unknown;
    _minY_det[i] = unknown;
    _maxX_det[i] = unknown;
    _maxY_det[i] = unknown;

    _x_bins[i] = unknown;
    _y_bins[i] = unknown;
  }

  _type_str[ all ]    = "";
  _type_str[ direct ] = "direct";
  _type_str[ left ]   = "left";
  _type_str[ right ]  = "right";
  _type_str[ up ]     = "up";
  _type_str[ down ]   = "down";

  ClearOutTree();
}


void KBarAnalysis::ClearOutTree()
{
  for( int i = 0; i < size; i++ )
  {
    _px_kBarX[i]    = unknown;
    _px_kBarY[i]    = unknown;
    _px_kBarZ[i]    = unknown;

    _px_kBarXerr[i] = unknown;
    _px_kBarYerr[i] = unknown;
    _px_kBarZerr[i] = unknown;

    _px_freq[i]     = unknown;

    _px_mcp = unknown;
    _px_col = unknown;
    _px_row = unknown;
  }
}


void KBarAnalysis::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();


  if( ( !_mcpMode && _resolution == unknown ) || _fishtank_width == unknown || _fishtank_height == unknown || _airgap == unknown )
  {
    cout << "Some parameters were not set!" << endl;
    abort();
  }

  if( _mcpMode )
  {
    for( int i = 0; i < _n_mcp; i++ )
    {
      if( _mcp_dim[i] == unknown || _mcp_det[i] == unknown || _minX_dim[i] == unknown || _minY_dim[i] == unknown
          || _x_bins[i] == unknown || _y_bins[i] == unknown )
      {
        cout << "MCP parameters were not set!" << endl;
        abort();
      }

      if( _x_bins[i] < 0 || _y_bins[i] < 0 )
      {
        cout << "Negative number of bins!" << endl;
        abort();
      }
    }
  }

  if( !_mcpMode && _n_mcp != 1 )
    _n_mcp = 1; // screen is one big MCP

  if( _fishtank_width/2 > TMath::Abs(unknown) || _fishtank_height/2 > TMath::Abs(unknown) )
  {
    cout << "Change magic number \"unkown\"!" << endl;
    abort();
  }

  if( !_mcpMode && _resolution < 0 )
  {
    cout << "Negative resolution!" << endl;
    abort();
  }


}


void KBarAnalysis::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();


  cout << "*** Initialize a lot of histograms" << endl;


  Double_t x_dimMin = -_fishtank_width/2;
  Double_t x_dimMax = _fishtank_width/2;
  Double_t y_dimMin = -_fishtank_height/2;
  Double_t y_dimMax = _fishtank_height/2;


  if( _mcpMode )
  {
    _mcpLabel.resize( _n_mcp );

    _line_mcpCase.resize( _n_mcp );
    _line_mcpArea.resize( _n_mcp );

    for( int i = 0; i < _n_mcp; i++ )
    {
      _line_mcpCase[i].resize( n_lines );
      _line_mcpArea[i].resize( n_lines );
    }


    for( int i = 0; i < _n_mcp; i++ )
    {
      _maxX_dim[i] = _minX_dim[i] + _mcp_dim[i];
      _maxY_dim[i] = _minY_dim[i] + _mcp_dim[i];

      _minX_det[i] = _minX_dim[i] + _shiftX[i];
      _minY_det[i] = _minY_dim[i] + _shiftY[i];
      _maxX_det[i] = _minX_dim[i] + _shiftX[i] + _mcp_det[i];
      _maxY_det[i] = _minY_dim[i] + _shiftY[i] + _mcp_det[i];


      _line_mcpCase[i][0] = new TLine( _minX_dim[i], _minY_dim[i], _minX_dim[i], _maxY_dim[i] );
      _line_mcpCase[i][1] = new TLine( _minX_dim[i], _maxY_dim[i], _maxX_dim[i], _maxY_dim[i] );
      _line_mcpCase[i][2] = new TLine( _maxX_dim[i], _maxY_dim[i], _maxX_dim[i], _minY_dim[i] );
      _line_mcpCase[i][3] = new TLine( _maxX_dim[i], _minY_dim[i], _minX_dim[i], _minY_dim[i] );

      _line_mcpArea[i][0] = new TLine( _minX_det[i], _minY_det[i], _minX_det[i], _maxY_det[i] );
      _line_mcpArea[i][1] = new TLine( _minX_det[i], _maxY_det[i], _maxX_det[i], _maxY_det[i] );
      _line_mcpArea[i][2] = new TLine( _maxX_det[i], _maxY_det[i], _maxX_det[i], _minY_det[i] );
      _line_mcpArea[i][3] = new TLine( _maxX_det[i], _minY_det[i], _minX_det[i], _minY_det[i] );

      for( int j = 0; j < n_lines; j++ )
      {
        _line_mcpCase[i][j]->SetLineWidth(3);
        _line_mcpArea[i][j]->SetLineWidth(3);
      }


      _mcpLabel[i] = new TPaveText( _minX_det[i], _maxY_det[i], _maxX_det[i], _maxY_dim[i] );
      _mcpLabel[i]->SetFillColor( 0 );
      _mcpLabel[i]->AddText( _mcpLabel_str[i] );
    }
  }
  else
  {
    _x_bins[0] = TMath::CeilNint(_fishtank_width / _resolution);
    _y_bins[0] = TMath::CeilNint(_fishtank_height / _resolution);

    if( (Int_t) (_fishtank_width*1000) % (Int_t) (_resolution*1000) !=0)
      x_dimMax = _x_bins[0]*_resolution - _fishtank_width/2;
    if( (Int_t) (_fishtank_height*1000) % (Int_t) (_resolution*1000) !=0)
      y_dimMax = _y_bins[0]*_resolution - _fishtank_height/2;
  }


  _pixelX.resize( _n_mcp );
  _pixelY.resize( _n_mcp );
  _pixelZ.resize( _n_mcp );

  _pixelX_str.resize( _n_mcp );
  _pixelY_str.resize( _n_mcp );
  _pixelZ_str.resize( _n_mcp );

  _kBarXsum.resize( _n_mcp );
  _kBarYsum.resize( _n_mcp );
  _kBarZsum.resize( _n_mcp );

  _freq.resize( _n_mcp );


  Int_t resizeop    = 0;

  for( int i = 0; i < _n_mcp; i++ )
    _pixeltot += ( _x_bins[i] * _y_bins[i] );


  for( int i = 0; i < _n_mcp; i++ )
  {
    _pixelX[i].resize( _x_bins[i] );
    _pixelY[i].resize( _x_bins[i] );
    _pixelZ[i].resize( _x_bins[i] );

    _pixelX_str[i].resize( _x_bins[i] );
    _pixelY_str[i].resize( _x_bins[i] );
    _pixelZ_str[i].resize( _x_bins[i] );

    _kBarXsum[i].resize( _x_bins[i] );
    _kBarYsum[i].resize( _x_bins[i] );
    _kBarZsum[i].resize( _x_bins[i] );

    _freq[i].resize( _x_bins[i] );

    for( int j = 0; j < _x_bins[i]; j++ )
    {
      _pixelX[i][j].resize( _y_bins[i] );
      _pixelY[i][j].resize( _y_bins[i] );
      _pixelZ[i][j].resize( _y_bins[i] );

      _pixelX_str[i][j].resize( _y_bins[i] );
      _pixelY_str[i][j].resize( _y_bins[i] );
      _pixelZ_str[i][j].resize( _y_bins[i] );

      _kBarXsum[i][j].resize( _y_bins[i] );
      _kBarYsum[i][j].resize( _y_bins[i] );
      _kBarZsum[i][j].resize( _y_bins[i] );

      _freq[i][j].resize( _y_bins[i] );

      for( int k = 0; k < _y_bins[i]; k++ )
      {
        _pixelX[i][j][k].resize( size );
        _pixelY[i][j][k].resize( size );
        _pixelZ[i][j][k].resize( size );

        _pixelX_str[i][j][k].resize( size );
        _pixelY_str[i][j][k].resize( size );
        _pixelZ_str[i][j][k].resize( size );

        _kBarXsum[i][j][k].resize( size );
        _kBarYsum[i][j][k].resize( size );
        _kBarZsum[i][j][k].resize( size );

        _freq[i][j][k].resize( size );

      }

      resizeop +=  _y_bins[i];

      cout << resizeop << " / " << _pixeltot << " resize operations" << endl;
    }
  }


  _kBarX.resize( size );
  _kBarY.resize( size );
  _kBarZ.resize( size );

  _kBarX_str.resize( size );
  _kBarY_str.resize( size );
  _kBarZ_str.resize( size );

  if( _mcpMode )
  {
    _kBarX_screen.resize( size );
    _kBarY_screen.resize( size );
    _kBarZ_screen.resize( size );

    _kBarX_screen_str.resize( size );
    _kBarY_screen_str.resize( size );
    _kBarZ_screen_str.resize( size );
  }

  for( UInt_t i = 0; i < size; i++ )
  {
    _kBarX[i].resize( _n_mcp );
    _kBarY[i].resize( _n_mcp );
    _kBarZ[i].resize( _n_mcp );

    _kBarX_str[i].resize( _n_mcp );
    _kBarY_str[i].resize( _n_mcp );
    _kBarZ_str[i].resize( _n_mcp );

    if( _mcpMode )
    {
      _kBarX_screen[i].resize( _n_mcp );
      _kBarY_screen[i].resize( _n_mcp );
      _kBarZ_screen[i].resize( _n_mcp );

      _kBarX_screen_str[i].resize( _n_mcp );
      _kBarY_screen_str[i].resize( _n_mcp );
      _kBarZ_screen_str[i].resize( _n_mcp );
    }
  }


    _screen = new TH2F( "screen", "", _x_bins[0], x_dimMin, x_dimMax, _y_bins[0], y_dimMin, y_dimMax ); // both modes


  for( int i = 0; i < size; i++ )
  {
    for( int j = 0; j < _n_mcp; j++ )
    {
      if( _mcpMode )
      {
        TString j_str;
        j_str += j;
        j_str.Remove( TString::kLeading, ' ' );

        _kBarX_str[i][j] = new TString( "kBarX_mcp" + j_str );
        _kBarY_str[i][j] = new TString( "kBarY_mcp" + j_str );
        _kBarZ_str[i][j] = new TString( "kBarZ_mcp" + j_str );

        _kBarX_screen_str[i][j] = new TString( "kBarX_screen" + j_str );
        _kBarY_screen_str[i][j] = new TString( "kBarY_screen" + j_str );
        _kBarZ_screen_str[i][j] = new TString( "kBarZ_screen" + j_str );
      }
      else
      {
        _kBarX_str[i][j] = new TString( "kBarX" );
        _kBarY_str[i][j] = new TString( "kBarY" );
        _kBarZ_str[i][j] = new TString( "kBarZ" );
      }

      if( i > 0 )
      {
        _kBarX_str[i][j]->Append( "_" + _type_str[i] );
        _kBarY_str[i][j]->Append( "_" + _type_str[i] );
        _kBarZ_str[i][j]->Append( "_" + _type_str[i] );

        if( _mcpMode )
        {
          _kBarX_screen_str[i][j]->Append( "_" + _type_str[i] );
          _kBarY_screen_str[i][j]->Append( "_" + _type_str[i] );
          _kBarZ_screen_str[i][j]->Append( "_" + _type_str[i] );
        }
      }

      if( _mcpMode )
      {
        _kBarX[i][j] = new TH2F( *_kBarX_str[i][j], "", _y_bins[j], 0, _y_bins[j], _y_bins[j], 0, _y_bins[j] );
        _kBarY[i][j] = new TH2F( *_kBarY_str[i][j], "", _y_bins[j], 0, _y_bins[j], _y_bins[j], 0, _y_bins[j] );
        _kBarZ[i][j] = new TH2F( *_kBarZ_str[i][j], "", _y_bins[j], 0, _y_bins[j], _y_bins[j], 0, _y_bins[j] );

        _kBarX_screen[i][j] = new TH2F( *_kBarX_screen_str[i][j], "", _y_bins[j], _minX_det[j], _maxX_det[j],
                                         _y_bins[j],_minY_det[j],_maxY_det[j] );
        _kBarY_screen[i][j] = new TH2F( *_kBarY_screen_str[i][j], "", _y_bins[j], _minX_det[j], _maxX_det[j],
                                         _y_bins[j],_minY_det[j],_maxY_det[j] );
        _kBarZ_screen[i][j] = new TH2F( *_kBarZ_screen_str[i][j], "", _y_bins[j], _minX_det[j], _maxX_det[j],
                                         _y_bins[j],_minY_det[j],_maxY_det[j] );
      }
      else
      {
        _kBarX[i][j] = new TH2F( *_kBarX_str[i][j], "", _x_bins[j], x_dimMin, x_dimMax, _y_bins[j], y_dimMin, y_dimMax );
        _kBarY[i][j] = new TH2F( *_kBarY_str[i][j], "", _x_bins[j], x_dimMin, x_dimMax, _y_bins[j], y_dimMin, y_dimMax );
        _kBarZ[i][j] = new TH2F( *_kBarZ_str[i][j], "", _x_bins[j], x_dimMin, x_dimMax, _y_bins[j], y_dimMin, y_dimMax );
      }
    }
  }


  Int_t plotdecop    = 0;
  Int_t plotdecopoff = 0;
  Int_t plotdecoptot = _pixeltot*size*3;

  if( _mcpMode )
    plotdecopoff = size*_n_mcp*6;
  else
    plotdecopoff = size*3;

  for( int i = 0; i < _n_mcp; i++ )
  {
    for( int j = 0; j < _x_bins[i]; j++ )
    {
      for( int k = 0; k < _y_bins[i]; k++ )
      {
        for( int l = 0; l < size; l++ )
        {
          TString i_str;
          TString j_str;
          TString k_str;
          i_str += i;
          j_str += j;
          k_str += k;
          i_str.Remove( TString::kLeading, ' ' );
          j_str.Remove( TString::kLeading, ' ' );
          k_str.Remove( TString::kLeading, ' ' );

          _pixelX_str[i][j][k][l] = new TString( "kBarX_" + _type_str[l] + "_" + i_str + "_" + j_str + "_" + k_str );
          _pixelY_str[i][j][k][l] = new TString( "kBarY_" + _type_str[l] + "_" + i_str + "_" + j_str + "_" + k_str );
          _pixelZ_str[i][j][k][l] = new TString( "kBarZ_" + _type_str[l] + "_" + i_str + "_" + j_str + "_" + k_str );

          _pixelX[i][j][k][l] = new TH1F( *_pixelX_str[i][j][k][l], "", 200, -1, 1 );
          _pixelY[i][j][k][l] = new TH1F( *_pixelY_str[i][j][k][l], "", 200, -1, 1 );
          _pixelZ[i][j][k][l] = new TH1F( *_pixelZ_str[i][j][k][l], "", 200, 0, 1 );


          _kBarXsum[i][j][k][l] = 0;
          _kBarYsum[i][j][k][l] = 0;
          _kBarZsum[i][j][k][l] = 0;

          _freq[i][j][k][l] = 0;
        }
      }

      plotdecop += ( (_y_bins[i] * size) * 3 );

      cout << plotdecopoff + plotdecop << " / " << plotdecopoff + plotdecoptot << " initialized histograms" << endl;
    }
  }
}


Bool_t KBarAnalysis::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either KBarAnalysis::GetEntry() or TBranch::GetEntry()
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

//     if( entry%100000 == 0 )
//         cout << entry << " / " << fChain->GetTree()->GetEntries() << endl;
  //
//     kBarY = kBarY + 500/kBarZ * 1.47/300; // for time difference (here: kBarY = time), start position z=-500
//     kBarY = kBarY + 500/kBarZ * 1.58/300; // with group refrdet index


  const int n_mcp_helper = _n_mcp;
  Int_t pxX[n_mcp_helper];
  Int_t pxY[n_mcp_helper];

  for( int i = 0; i < _n_mcp; i++ )
  {
    pxX[i] = unknown;
    pxY[i] = unknown;
  }

  Int_t n_test = 0;
  Int_t mcpID_helper = unknown;

  if( !_effiMode || ( wavelength > 200 && wavelength < 700 && (_rand.Uniform() < _effi[(int)(wavelength - min_wave)]) ) )
  {
    if( _mcpMode )
    {
      for( int i = 0; i < _n_mcp; i++)
      {
        if( hitPosDetX >= _minX_det[i] && hitPosDetX < _maxX_det[i]
            && hitPosDetY >= _minY_det[i] && hitPosDetY < _maxY_det[i] )
        {
          pxX[i] = TMath::FloorNint( (hitPosDetX - _minX_det[i])*_x_bins[i] / _mcp_det[i] );
          pxY[i] = TMath::FloorNint( (hitPosDetY - _minY_det[i])*_y_bins[i] / _mcp_det[i] );

          n_test++;
          mcpID_helper = i;
        }
      }

      if( n_test > 1 )
      {
        cout << "Something is wrong (hit for different MCPs)." << endl;
        abort();
      }
    }
    else
    {
      pxX[0] = TMath::FloorNint( (hitPosDetX + _fishtank_width/2) / _resolution );
      pxY[0] = TMath::FloorNint( (hitPosDetY + _fishtank_height/2) / _resolution );

      n_test++;
      mcpID_helper = 0;
    }
  }
  else
    return kTRUE;

  if( n_test == 0 ) // no valid hit
    return kTRUE;


  const int mcpID = mcpID_helper;

  _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]->Fill( kBarX );
  _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]->Fill( kBarY );
  _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]->Fill( kBarZ );


  _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ] += kBarX;
  _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ] += kBarY;
  _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ] += kBarZ;

  _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]++;



  Int_t nBoxRefl = 0;

  for( int j = 0; j < index_pos; j++ )
  {
    if( j > 0 && posZ[j] > _airgap + 0.0001
        && ( posZ[j] < hitPosZ || posZ[j+1] == hitPosZ ) ) // need tiny shift to check the reflections at the fishtank sides
    {
      cout << "Reflection at fishtank side for photonID: " << entry << " ; posZ: " << posZ[j]
          << " ; pixel: (" << pxX << "," << pxY << ")" << endl;

      nBoxRefl++;
      if( nBoxRefl > 2 )
        cout << "Photon hits more than one time the fishtank side walls" << endl;


      TVector3 dirBefore( posX[j] - posX[j-1], posY[j] - posY[j-1], posZ[j] - posZ[j-1] );
      TVector3 dirAfter(  posX[j+1] - posX[j], posY[j+1] - posY[j], posZ[j+1] - posZ[j] );
      dirBefore = dirBefore.Unit();
      dirAfter = dirAfter.Unit();

      cout << "before: " << dirBefore.X() << " " << dirBefore.Y() << " " << dirBefore.Z() <<
          "  after: " << dirAfter.X() << " " << dirAfter.Y() << " " << dirAfter.Z() << endl;

      if( posX[j] > 0 && ( (dirBefore.X() > 0 && dirAfter.X() < 0) || (dirBefore.X() < 0 && dirAfter.X() > 0) ) )
      {
        cout << "left-reflection at fishtank" << endl;

        _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ]->Fill( kBarX );
        _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ]->Fill( kBarY );
        _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ]->Fill( kBarZ );

        _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ] += kBarX;
        _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ] += kBarY;
        _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ] += kBarZ;

        _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ left ]++;

      }
      else if( posX[j] < 0 && ( (dirBefore.X() > 0 && dirAfter.X() < 0) || (dirBefore.X() < 0 && dirAfter.X() > 0) ) )
      {
        cout << "right-reflection at fishtank" << endl;

        _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ]->Fill( kBarX );
        _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ]->Fill( kBarY );
        _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ]->Fill( kBarZ );

        _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ] += kBarX;
        _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ] += kBarY;
        _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ] += kBarZ;

        _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ right ]++;
      }
      else if( posY[j] > 0 && ( (dirBefore.Y() > 0 && dirAfter.Y() < 0) || (dirBefore.Y() < 0 && dirAfter.Y() > 0) ) )
      {
        cout << "up-reflection at fishtank" << endl;

        _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ]->Fill( kBarX );
        _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ]->Fill( kBarY );
        _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ]->Fill( kBarZ );

        _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ] += kBarX;
        _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ] += kBarY;
        _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ] += kBarZ;

        _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ up ]++;
      }
      else if( posY[j] < 0 && ( (dirBefore.Y() > 0 && dirAfter.Y() < 0) || (dirBefore.Y() < 0 && dirAfter.Y() > 0) ) )
      {
        cout << "down-reflection at fishtank" << endl;

        _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ]->Fill( kBarX );
        _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ]->Fill( kBarY );
        _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ]->Fill( kBarZ );

        _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ] += kBarX;
        _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ] += kBarY;
        _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ] += kBarZ;

        _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ down ]++;
      }
    }
  }

  if( nBoxRefl == 0)
  {
    _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ]->Fill( kBarX );
    _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ]->Fill( kBarY );
    _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ]->Fill( kBarZ );

    _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ] += kBarX;
    _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ] += kBarY;
    _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ] += kBarZ;

    _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ direct ]++;
  }

//             if( pxX == 27 && pxY == 8)
//                 cout << "photonID: " << entry << " ; kBarX: " << kBarX << endl;

  return kTRUE;
}


void KBarAnalysis::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}


void KBarAnalysis::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.


  TString modeExtension;
  if( _mcpMode )
    modeExtension = "_mcpPlots_gap" + _gap_str + ".root";
  else
    modeExtension = "_screenPlots.root";

  TString outFilename;
  outFilename = _outDirectory + _outFileCore + modeExtension;
  TFile *outFile = new TFile( outFilename, "RECREATE" );
  TTree *pixelTree = new TTree( "pixel", outFilename ); // for example: MCP1 is pixel 0 - 63; starts at (0,0), (0,1) ...

  pixelTree->Branch( "kBarX[6]"   , _px_kBarX   , "kBarX[6]/D" );
  pixelTree->Branch( "kBarY[6]"   , _px_kBarY   , "kBarY[6]/D" );
  pixelTree->Branch( "kBarZ[6]"   , _px_kBarZ   , "kBarZ[6]/D" );
  pixelTree->Branch( "kBarXerr[6]", _px_kBarXerr, "kBarXerr[6]/D" );
  pixelTree->Branch( "kBarYerr[6]", _px_kBarYerr, "kBarYerr[6]/D" );
  pixelTree->Branch( "kBarZerr[6]", _px_kBarZerr, "kBarZerr[6]/D" );

  pixelTree->Branch( "mcp", &_px_mcp, "mcp/I" );
  pixelTree->Branch( "col", &_px_col, "col/I" );
  pixelTree->Branch( "row", &_px_row, "row/I" );

  pixelTree->Branch( "freq[6]", _px_freq, "freq[6]/I" );


  TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
  canvas->SetLeftMargin(0.11);
  canvas->SetRightMargin(0.13);
  canvas->Draw();

  TCanvas *canvasX = new TCanvas( "canvasX", "" ,200, 10, 700, 510 );
  TCanvas *canvasY = new TCanvas( "canvasY", "" ,200, 10, 700, 510 );
  TCanvas *canvasZ = new TCanvas( "canvasZ", "" ,200, 10, 700, 510 );
  canvasX->SetLeftMargin(0.11);
  canvasX->SetRightMargin(0.11);
  canvasX->Draw();
  canvasY->SetLeftMargin(0.11);
  canvasY->SetRightMargin(0.11);
  canvasY->Draw();
  canvasZ->SetLeftMargin(0.11);
  canvasZ->SetRightMargin(0.11);
  canvasZ->Draw();

  canvas->cd();


  cout << "*** Draw histograms" << endl;


  Int_t plotdrawop    = 0;
  Int_t plotdrawopoff = 0;
  Int_t plotdrawoptot = _pixeltot*size*3;

  if( _mcpMode )
    plotdrawopoff = size*_n_mcp*6;
  else
    plotdrawopoff = size*3;

  for( int i = 0; i < _n_mcp; i++ )
  {
    for( int j = 0; j < _x_bins[i]; j++)
    {
      for( int k = 0; k < _y_bins[i]; k++)
      {
        for( int l = 0; l < size; l++ )
        {
          if( _freq[i][j][k][l] != 0 )
          {
            _pixelX[i][j][k][l]->Draw();
            canvas->Write( *_pixelX_str[i][j][k][l] );
            canvas->Clear();

            _pixelY[i][j][k][l]->Draw();
            canvas->Write( *_pixelY_str[i][j][k][l] );
            canvas->Clear();

            _pixelZ[i][j][k][l]->Draw();
            canvas->Write( *_pixelZ_str[i][j][k][l] );
            canvas->Clear();
          }

          if( _freq[i][j][k][l] == 0 )
          {
            _kBarXsum[i][j][k][l] = nokbar;
            _kBarYsum[i][j][k][l] = nokbar;
            _kBarZsum[i][j][k][l] = nokbar;
            _freq[i][j][k][l] = nofreq;
          }

          if( _mcpMode )
          {
            Double_t hitX = j * _mcp_det[i] / _y_bins[i] + _minX_det[i] + _mcp_det[i] / ( 2 * _y_bins[i] );
            Double_t hitY = k * _mcp_det[i] / _y_bins[i] + _minY_det[i] + _mcp_det[i] / ( 2 * _y_bins[i] );

            _kBarX[l][i]->Fill( j, k, _kBarXsum[i][j][k][l] / _freq[i][j][k][l] );
            _kBarY[l][i]->Fill( j, k, _kBarYsum[i][j][k][l] / _freq[i][j][k][l] );
            _kBarZ[l][i]->Fill( j, k, _kBarZsum[i][j][k][l] / _freq[i][j][k][l] );

            _kBarX_screen[l][i]->Fill( hitX, hitY, _kBarXsum[i][j][k][l] / _freq[i][j][k][l] );
            _kBarY_screen[l][i]->Fill( hitX, hitY, _kBarYsum[i][j][k][l] / _freq[i][j][k][l] );
            _kBarZ_screen[l][i]->Fill( hitX, hitY, _kBarZsum[i][j][k][l] / _freq[i][j][k][l] );
          }
          else
          {
            Double_t hitX = j * _resolution - _fishtank_width/2 + _resolution/2;
            Double_t hitY = k * _resolution - _fishtank_height/2 + _resolution/2;

            _kBarX[l][0]->Fill( hitX, hitY, _kBarXsum[i][j][k][l] / _freq[i][j][k][l] );
            _kBarY[l][0]->Fill( hitX, hitY, _kBarYsum[i][j][k][l] / _freq[i][j][k][l] );
            _kBarZ[l][0]->Fill( hitX, hitY, _kBarZsum[i][j][k][l] / _freq[i][j][k][l] );

            if( l == all )
              _screen->Fill( hitX, hitY, _freq[i][j][k][ all ] );
          }

          _px_kBarX[l]    =  _kBarXsum[i][j][k][l] / _freq[i][j][k][l];
          _px_kBarY[l]    =  _kBarYsum[i][j][k][l] / _freq[i][j][k][l];
          _px_kBarZ[l]    =  _kBarZsum[i][j][k][l] / _freq[i][j][k][l];

          _px_kBarXerr[l] =  _pixelX[i][j][k][l]->GetRMS();
          _px_kBarYerr[l] =  _pixelY[i][j][k][l]->GetRMS();
          _px_kBarZerr[l] =  _pixelZ[i][j][k][l]->GetRMS();

          _px_freq[l]     =  _freq[i][j][k][l];
        }

        _px_mcp = i;
        _px_col = j;
        _px_row = k;

        pixelTree->Fill();
        ClearOutTree();
      }

      plotdrawop += ( (_y_bins[i] * size) * 3 );

      cout << plotdrawopoff + plotdrawop << " / " << plotdrawopoff + plotdrawoptot << " drawn histograms" << endl;
    }
  }


  if( !_mcpMode )
  {
    canvas->cd();
    _screen->SetTitle( "screen" );
    _screen->SetContour(50);
    _screen->GetXaxis()->SetTitle( "x [mm]" );
    _screen->GetXaxis()->CenterTitle();
    _screen->GetYaxis()->SetTitle( "y [mm]" );
    _screen->GetYaxis()->CenterTitle();
    _screen->GetYaxis()->SetTitleOffset( 0.9 );
    _screen->SetStats( false );
    _screen->Draw("colz");
    canvas->Write("screen");
    canvas->Clear();
  }

  for( int i = 0; i < size; i++ )
  {
    TString kBarX_screenTitle;
    TString kBarY_screenTitle;
    TString kBarZ_screenTitle;

    if( i == 0 )
    {
      kBarX_screenTitle = "kBarX_screen";
      kBarY_screenTitle = "kBarY_screen";
      kBarZ_screenTitle = "kBarZ_screen";
    }
    else
    {
      kBarX_screenTitle = "kBarX_screen_" + _type_str[i];
      kBarY_screenTitle = "kBarY_screen_" + _type_str[i];
      kBarZ_screenTitle = "kBarZ_screen_" + _type_str[i];
    }

    // draw empty screen to get full detector area for mcpMode; no mcpMode this plot isn't saved
    canvasX->cd();
    _screen->SetContour(50);
    _screen->SetMinimum(-1);
    _screen->SetMaximum(+1);
    _screen->SetTitle( kBarX_screenTitle );
    _screen->SetStats( false );
    _screen->Draw("col");

    canvasY->cd();
    _screen->SetTitle( kBarY_screenTitle );
    _screen->Draw("col");

    canvasZ->cd();
    _screen->SetMinimum(0);
    _screen->SetTitle( kBarZ_screenTitle );
    _screen->Draw("col");

    for( int j = 0; j < _n_mcp; j++ )
    {
      canvas->cd();

      _kBarX[i][j]->SetTitle( *_kBarX_str[i][j] );
      _kBarX[i][j]->SetContour(50);
      _kBarX[i][j]->GetXaxis()->SetTitle( "x [mm]" );
      _kBarX[i][j]->GetXaxis()->CenterTitle();
      _kBarX[i][j]->GetYaxis()->SetTitle( "y [mm]" );
      _kBarX[i][j]->GetYaxis()->CenterTitle();
      _kBarX[i][j]->GetYaxis()->SetTitleOffset( 0.9 );
      _kBarX[i][j]->SetStats( false );
      _kBarX[i][j]->Draw("colz");
      canvas->Write( *_kBarX_str[i][j] );
      canvas->Clear();

      _kBarY[i][j]->SetTitle( *_kBarY_str[i][j] );
      _kBarY[i][j]->SetContour(50);
      _kBarY[i][j]->GetXaxis()->SetTitle( "x [mm]" );
      _kBarY[i][j]->GetXaxis()->CenterTitle();
      _kBarY[i][j]->GetYaxis()->SetTitle( "y [mm]" );
      _kBarY[i][j]->GetYaxis()->CenterTitle();
      _kBarY[i][j]->GetYaxis()->SetTitleOffset( 0.9 );
      _kBarY[i][j]->SetStats( false );
      _kBarY[i][j]->Draw("colz");
      canvas->Write( *_kBarY_str[i][j] );
      canvas->Clear();

      _kBarZ[i][j]->SetTitle( *_kBarZ_str[i][j] );
      _kBarZ[i][j]->SetContour(50);
      _kBarZ[i][j]->GetXaxis()->SetTitle( "x [mm]" );
      _kBarZ[i][j]->GetXaxis()->CenterTitle();
      _kBarZ[i][j]->GetYaxis()->SetTitle( "y [mm]" );
      _kBarZ[i][j]->GetYaxis()->CenterTitle();
      _kBarZ[i][j]->GetYaxis()->SetTitleOffset( 0.9 );
      _kBarZ[i][j]->SetStats( false );
      _kBarZ[i][j]->Draw("colz");
      canvas->Write( *_kBarZ_str[i][j] );
      canvas->Clear();


      if( _mcpMode )
      {
        canvasX->cd();

        _kBarX_screen[i][j]->SetContour(50);
        _kBarX_screen[i][j]->SetMinimum(-1);
        _kBarX_screen[i][j]->SetMaximum(+1);
        _kBarX_screen[i][j]->SetStats( false );
        _kBarX_screen[i][j]->Draw("same colz");

        _mcpLabel[j]->Draw("same");

        for( int k = 0; k < n_lines; k++ )
        {
          _line_mcpCase[j][k]->Draw("same");
          _line_mcpArea[j][k]->Draw("same");
        }


        canvasY->cd();

        _kBarY_screen[i][j]->SetContour(50);
        _kBarY_screen[i][j]->SetMinimum(-1);
        _kBarY_screen[i][j]->SetMaximum(+1);
        _kBarY_screen[i][j]->SetStats( false );
        _kBarY_screen[i][j]->Draw("same colz");

        _mcpLabel[j]->Draw("same");

        for( int k = 0; k < n_lines; k++ )
        {
          _line_mcpCase[j][k]->Draw("same");
          _line_mcpArea[j][k]->Draw("same");
        }


        canvasZ->cd();

        _kBarZ_screen[i][j]->SetContour(50);
        _kBarZ_screen[i][j]->SetMinimum(0);
        _kBarZ_screen[i][j]->SetMaximum(+1);
        _kBarZ_screen[i][j]->SetStats( false );
        _kBarZ_screen[i][j]->Draw("same colz");

        _mcpLabel[j]->Draw("same");

        for( int k = 0; k < n_lines; k++ )
        {
          _line_mcpCase[j][k]->Draw("same");
          _line_mcpArea[j][k]->Draw("same");
        }
      }
    }

    if( _mcpMode )
    {
      _screen->GetXaxis()->SetTitle( "x [mm]" );
      _screen->GetXaxis()->CenterTitle();
      _screen->GetYaxis()->SetTitle( "y [mm]" );
      _screen->GetYaxis()->CenterTitle();
      _screen->GetYaxis()->SetTitleOffset( 0.9 );

      canvasX->cd();
      _screen->SetTitle( kBarX_screenTitle );
      canvasX->Write( kBarX_screenTitle );
      canvasY->cd();
      _screen->SetTitle( kBarY_screenTitle );
      canvasY->Write( kBarY_screenTitle );
      canvasZ->cd();
      _screen->SetTitle( kBarZ_screenTitle );
      canvasZ->Write( kBarZ_screenTitle );
    }
  }


  canvas->Close();
  canvasX->Close();
  canvasY->Close();
  canvasZ->Close();


  outFile->Write();
  outFile->Close();
  cout << "Root-file " << outFilename << " was written" << endl;
}
