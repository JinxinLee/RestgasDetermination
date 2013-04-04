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



//==========
// STD C/C++
//==========
#include <iostream>
#include <cstdlib>

using namespace std;



//======
// ROOT
//======
#include "KBarAnalysis.h"
#include "TStyle.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"


//==========
// my stuff
//==========
#include "DetEffi.h"



Bool_t KBarAnalysis::_notify = false;


KBarAnalysis::KBarAnalysis( Int_t n_mcp )//: _n_mcp(n_mcp) // initialize const variable
{
  _n_mcp = n_mcp;

  if( _n_mcp < 0 )
  {
    cout << "Negative number of MCPs!" << endl;
    abort();
  }


  _mcpMode = false;
  _effiMode = false;


  //_effi = new DetEffi::DetEffi();
  _effi = new DetEffi();


  _fishtank_width  = unknown;
  _fishtank_height = unknown;
  _airgap          = unknown;

  _resolution = unknown;

  _mcp_dim.resize( _n_mcp );
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

    _shiftX[i]   = unknown;
    _shiftY[i]   = unknown;

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

  if( !_mcpMode && _n_mcp != 1 )
    _n_mcp = 1; // screen is one big MCP

  if( _fishtank_width/2 > TMath::Abs(unknown) || _fishtank_height/2 > TMath::Abs(unknown) )
  {
    cout << "Change magic number \"unknown\"!" << endl;
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
    _line_mcpCase.resize( _n_mcp );
    _line_mcpArea.resize( _n_mcp );

    for( int i = 0; i < _n_mcp; i++ )
    {
      _line_mcpCase[i].resize( n_lines );
      _line_mcpArea[i].resize( n_lines );
    }


    for( int i = 0; i < _n_mcp; i++ )
    {
      if( _mcp_dim[i] == unknown )
        continue;

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


  Int_t resizeop = 0;
  _pixeltot = 0;

  for( int i = 0; i < _n_mcp; i++ )
  {
    if( _mcpMode && _mcp_dim[i] == unknown )
      continue;

    _pixeltot += ( _x_bins[i] * _y_bins[i] );
  }


  for( int i = 0; i < _n_mcp; i++ )
  {
    if( _mcpMode && _mcp_dim[i] == unknown )
      continue;

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
      if( _mcpMode && _mcp_dim[j] == unknown )
        continue;

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
    if( _mcpMode && _mcp_dim[i] == unknown )
      continue;

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

  if( !_effiMode || _effi->DetEffi::Effi( (int) wavelength ) )
  {
    if( _mcpMode )
    {
      for( int i = 0; i < _n_mcp; i++)
      {
        if( _mcpMode && _mcp_dim[i] == unknown )
          continue;

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

  _pixelX[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]->Fill( kBarX ); // 1 fill per event (photon)
  _pixelY[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]->Fill( kBarY );
  _pixelZ[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]->Fill( kBarZ );


  _kBarXsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ] += kBarX;
  _kBarYsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ] += kBarY;
  _kBarZsum[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ] += kBarZ;

  _freq[mcpID][ pxX[mcpID] ][ pxY[mcpID] ][ all ]++;



  Int_t nBoxRefl = 0;

  for( int j = 0; j < index_pos; j++ )
  {
    if( j > 0 && posZ[j] > _airgap + 0.0001 && ( posZ[j] < hitPosZ ||
        (posZ[j+1] == hitPosZ && j+1<index_pos ) ) ) // need tiny shift to check the reflections at the fishtank sides
    {
      cout << "Reflection at fishtank side for photonID: " << entry << " ; posZ: " << posZ[j] << endl;

      nBoxRefl++;
      if( nBoxRefl > 1 )
        cout << "Photon hits more than one time the fishtank side walls" << endl;


      TVector3 dirBefore( posX[j] - posX[j-1], posY[j] - posY[j-1], posZ[j] - posZ[j-1] );
      TVector3 dirAfter(  posX[j+1] - posX[j], posY[j+1] - posY[j], posZ[j+1] - posZ[j] );
      dirBefore = dirBefore.Unit();
      dirAfter = dirAfter.Unit();

      cout << "before: " << dirBefore.X() << " " << dirBefore.Y() << " " << dirBefore.Z() <<
          "  after: " << dirAfter.X() << " " << dirAfter.Y() << " " << dirAfter.Z() << endl;

      // included cases with nBoxRefl > 1
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
  {
    TString resolution_str;
    resolution_str += _resolution;
    resolution_str.Remove( TString::kLeading, ' ' );

    modeExtension = "_screenPlots_res" + resolution_str + ".root";
  }


  TString outDirectory_lastChar = _outDirectory;
  outDirectory_lastChar.Remove( 0, outDirectory_lastChar.Length()-1 );
  if( outDirectory_lastChar != "/" )
    _outDirectory = _outDirectory + "/";

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
    if( _mcpMode && _mcp_dim[i] == unknown )
      continue;

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
      if( _mcpMode && _mcp_dim[j] == unknown )
        continue;

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
