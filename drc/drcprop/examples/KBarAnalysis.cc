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
using namespace std;

#include "KBarAnalysis.hh"
#include "TStyle.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"


KBarAnalysis::KBarAnalysis(TTree * /*tree*/)
{
    _notify = false;
    _mcpMode = false;
    _effiMode = false;

    for (int i=0; i<70; i++) _effi[i]=0;

    // the argument is the wavelenght (nm) divided by 10.
    _effi[29] = 0.06;
    _effi[30] = 0.10;
    _effi[31] = 0.16;
    _effi[32] = 0.22;
    _effi[33] = 0.27;
    _effi[34] = 0.28;
    _effi[35] = 0.285;
    _effi[36] = 0.31;
    _effi[37] = 0.31;
    _effi[38] = 0.30;
    _effi[39] = 0.29;
    _effi[40] = 0.28;
    _effi[41] = 0.27;
    _effi[42] = 0.26;
    _effi[43] = 0.25;
    _effi[44] = 0.24;
    _effi[45] = 0.23;
    _effi[46] = 0.21;
    _effi[47] = 0.19;
    _effi[48] = 0.18;
    _effi[49] = 0.17;
    _effi[50] = 0.16;
    _effi[51] = 0.14;
    _effi[52] = 0.12;
    _effi[53] = 0.10;
    _effi[54] = 0.07;
    _effi[55] = 0.05;
    _effi[56] = 0.04;
    _effi[57] = 0.03;
    _effi[58] = 0.03;
    _effi[59] = 0.02;
    _effi[60] = 0.01;
    _effi[61] = 0.0075;
    _effi[62] = 0.005;
    _effi[63] = 0.0025;

    for (int i=0; i<70; i++) _effi[i]=_effi[i]*3; // to avoid the removing of too much photons


    _fishtank_width  = -666;
    _fishtank_height = -666;
    _airgap          = -666;

    _resolution = -666;

    _mcp_dim = -666;
    _mcp_active = -666;

    for( int i = 0; i < 4; i++ )
    {
        _minX_dim[i] = -666;
        _minY_dim[i] = -666;
    }

    type_str[ all ]    = "";
    type_str[ direct ] = "direct";
    type_str[ left ]   = "left";
    type_str[ right ]  = "right";
    type_str[ up ]     = "up";
    type_str[ down ]   = "down";

    ClearOutTree();
}

void KBarAnalysis::ClearOutTree()
{
    for( int i = 0; i < size; i++ )
    {
        _px_kBarX[i]    = -666;
        _px_kBarY[i]    = -666;
        _px_kBarZ[i]    = -666;

        _px_kBarXerr[i] = -666;
        _px_kBarYerr[i] = -666;
        _px_kBarZerr[i] = -666;

        _px_freq[i]    = -666;
    }
}

void KBarAnalysis::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

    TString option = GetOption();


    if( ( !_mcpMode && _resolution == -666 )
           || _fishtank_width == -666 || _fishtank_height == -666 || _airgap == -666
           || ( _mcpMode && ( _mcp_dim == -666 || _mcp_active == -666 || _minX_dim[0] == -666 ) )
      )
    {
        cout << "Class members were not set!" << endl;
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
        _n_mcp = 4; // number of MCPs
        _x_bins = _n_mcp*8;
        _y_bins = 8;


        for( int i = 0; i < _n_mcp; i++ )
        {
            Double_t shift = ( _mcp_dim - _mcp_active ) / 2; // to shift active area in the center of MCP

            _maxX_dim[i] = _minX_dim[i] + _mcp_dim;
            _maxY_dim[i] = _minY_dim[i] + _mcp_dim;

            _minX_active[i] = _minX_dim[i] + shift;
            _minY_active[i] = _minY_dim[i] + shift;
            _maxX_active[i] = _minX_dim[i] + shift + _mcp_active;
            _maxY_active[i] = _minY_dim[i] + shift + _mcp_active;


            _line_mcpCase[i][0] = TLine( _minX_dim[i], _minY_dim[i], _minX_dim[i], _maxY_dim[i] );
            _line_mcpCase[i][1] = TLine( _minX_dim[i], _maxY_dim[i], _maxX_dim[i], _maxY_dim[i] );
            _line_mcpCase[i][2] = TLine( _maxX_dim[i], _maxY_dim[i], _maxX_dim[i], _minY_dim[i] );
            _line_mcpCase[i][3] = TLine( _maxX_dim[i], _minY_dim[i], _minX_dim[i], _minY_dim[i] );

            _line_mcpArea[i][0] = TLine( _minX_active[i], _minY_active[i], _minX_active[i], _maxY_active[i] );
            _line_mcpArea[i][1] = TLine( _minX_active[i], _maxY_active[i], _maxX_active[i], _maxY_active[i] );
            _line_mcpArea[i][2] = TLine( _maxX_active[i], _maxY_active[i], _maxX_active[i], _minY_active[i] );
            _line_mcpArea[i][3] = TLine( _maxX_active[i], _minY_active[i], _minX_active[i], _minY_active[i] );

            for( int j = 0; j < 4; j++ )
            {
                _line_mcpCase[i][j].SetLineWidth(3);
                _line_mcpArea[i][j].SetLineWidth(3);
            }


            TString i_str;
            i_str += (i + 1);
            i_str.Remove( TString::kLeading, ' ' );
            TString mcpLabel_str = "MCP" +i_str;

            _mcpLabel[i] = new TPaveText( _minX_active[i], _maxY_active[i], _maxX_active[i], _maxY_dim[i] );
            _mcpLabel[i]->SetFillColor( 0 );
            _mcpLabel[i]->AddText( mcpLabel_str );
        }
    }
    else
    {
        _n_mcp = 1; // screen is a big single MCP

        _x_bins = TMath::CeilNint(_fishtank_width / _resolution);
        _y_bins = TMath::CeilNint(_fishtank_height / _resolution);

        if( (Int_t) (_fishtank_width*1000) % (Int_t) (_resolution*1000) !=0)
            x_dimMax = _x_bins*_resolution - _fishtank_width/2;
        if( (Int_t) (_fishtank_height*1000) % (Int_t) (_resolution*1000) !=0)
            y_dimMax = _y_bins*_resolution - _fishtank_height/2;
    }


    _pixelX.resize(_x_bins);
    _pixelY.resize(_x_bins);
    _pixelZ.resize(_x_bins);

    _pixelX_str.resize(_x_bins); // resize doesn't work with 2nd argument to set a default value (CINT issue ?)
    _pixelY_str.resize(_x_bins);
    _pixelZ_str.resize(_x_bins);

    _kBarXsum.resize(_x_bins);
    _kBarYsum.resize(_x_bins);
    _kBarZsum.resize(_x_bins);

    _freq.resize( _x_bins );

    for( UInt_t i = 0; i < _kBarXsum.size(); i++ )
    {
        _pixelX[i].resize(_y_bins);
        _pixelY[i].resize(_y_bins);
        _pixelZ[i].resize(_y_bins);

        _pixelX_str[i].resize(_y_bins);
        _pixelY_str[i].resize(_y_bins);
        _pixelZ_str[i].resize(_y_bins);

        _kBarXsum[i].resize(_y_bins);
        _kBarYsum[i].resize(_y_bins);
        _kBarZsum[i].resize(_y_bins);

        _freq[i].resize(_y_bins );

        for( UInt_t j = 0; j < _kBarXsum[i].size(); j++ )
        {
            _pixelX[i][j].resize(size);
            _pixelY[i][j].resize(size);
            _pixelZ[i][j].resize(size);

            _pixelX_str[i][j].resize(size);
            _pixelY_str[i][j].resize(size);
            _pixelZ_str[i][j].resize(size);

            _kBarXsum[i][j].resize(size);
            _kBarYsum[i][j].resize(size);
            _kBarZsum[i][j].resize(size);

            _freq[i][j].resize(size);
        }
        cout << (i+1)*_kBarXsum[i].size() << " / " << _kBarXsum.size()*_kBarXsum[i].size() << " resize operations" << endl;
    }


    _kBarX.resize(size);
    _kBarY.resize(size);
    _kBarZ.resize(size);

    _kBarX_str.resize(size);
    _kBarY_str.resize(size);
    _kBarZ_str.resize(size);

    if( _mcpMode )
    {
        _kBarX_screen.resize(size);
        _kBarY_screen.resize(size);
        _kBarZ_screen.resize(size);

        _kBarX_screen_str.resize(size);
        _kBarY_screen_str.resize(size);
        _kBarZ_screen_str.resize(size);
    }

    for( UInt_t i = 0; i < _kBarX.size(); i++ )
    {
        _kBarX[i].resize(_n_mcp);
        _kBarY[i].resize(_n_mcp);
        _kBarZ[i].resize(_n_mcp);

        _kBarX_str[i].resize(_n_mcp);
        _kBarY_str[i].resize(_n_mcp);
        _kBarZ_str[i].resize(_n_mcp);

        if( _mcpMode )
        {
            _kBarX_screen[i].resize(_n_mcp);
            _kBarY_screen[i].resize(_n_mcp);
            _kBarZ_screen[i].resize(_n_mcp);

            _kBarX_screen_str[i].resize(_n_mcp);
            _kBarY_screen_str[i].resize(_n_mcp);
            _kBarZ_screen_str[i].resize(_n_mcp);
        }
    }


    _screen = new TH2F( "screen", "", _x_bins, x_dimMin, x_dimMax, _y_bins, y_dimMin, y_dimMax );


    for( int i = 0; i < size; i++ )
    {
        for( int j = 0; j < _n_mcp; j++ )
        {
            if( _mcpMode )
            {
                TString j_str;
                j_str += (j + 1);
                j_str.Remove( TString::kLeading, ' ' );

                _kBarX_str[i][j] = "kBarX_mcp" + j_str;
                _kBarY_str[i][j] = "kBarY_mcp" + j_str;
                _kBarZ_str[i][j] = "kBarZ_mcp" + j_str;

                _kBarX_screen_str[i][j] = "kBarX_screen" + j_str;
                _kBarY_screen_str[i][j] = "kBarY_screen" + j_str;
                _kBarZ_screen_str[i][j] = "kBarZ_screen" + j_str;
            }
            else
            {
                _kBarX_str[i][j] = "kBarX";
                _kBarY_str[i][j] = "kBarY";
                _kBarZ_str[i][j] = "kBarZ";
            }

            if( i > 0 )
            {
                _kBarX_str[i][j] = _kBarX_str[i][j] + "_" + type_str[i];
                _kBarY_str[i][j] = _kBarY_str[i][j] + "_" + type_str[i];
                _kBarZ_str[i][j] = _kBarZ_str[i][j] + "_" + type_str[i];

                if( _mcpMode )
                {
                    _kBarX_screen_str[i][j] = _kBarX_str[i][j] + "_" + type_str[i];
                    _kBarY_screen_str[i][j] = _kBarY_str[i][j] + "_" + type_str[i];
                    _kBarZ_screen_str[i][j] = _kBarZ_str[i][j] + "_" + type_str[i];
                }
            }

            if( _mcpMode )
            {
                _kBarX[i][j] = TH2F( _kBarX_str[i][j], "", _y_bins, 0, _y_bins, _y_bins, 0, _y_bins );
                _kBarY[i][j] = TH2F( _kBarY_str[i][j], "", _y_bins, 0, _y_bins, _y_bins, 0, _y_bins );
                _kBarZ[i][j] = TH2F( _kBarZ_str[i][j], "", _y_bins, 0, _y_bins, _y_bins, 0, _y_bins );

                _kBarX_screen[i][j] = TH2F( _kBarX_screen_str[i][j], "", _y_bins, _minX_active[j], _maxX_active[j],
                                            _y_bins,_minY_active[j],_maxY_active[j] );
                _kBarY_screen[i][j] = TH2F( _kBarY_screen_str[i][j], "", _y_bins, _minX_active[j], _maxX_active[j],
                                            _y_bins,_minY_active[j],_maxY_active[j] );
                _kBarZ_screen[i][j] = TH2F( _kBarZ_screen_str[i][j], "", _y_bins, _minX_active[j], _maxX_active[j],
                                            _y_bins,_minY_active[j],_maxY_active[j] );
            }
            else
            {
                _kBarX[i][j] = TH2F( _kBarX_str[i][j], "", _x_bins, x_dimMin, x_dimMax, _y_bins, y_dimMin, y_dimMax );
                _kBarY[i][j] = TH2F( _kBarY_str[i][j], "", _x_bins, x_dimMin, x_dimMax, _y_bins, y_dimMin, y_dimMax );
                _kBarZ[i][j] = TH2F( _kBarZ_str[i][j], "", _x_bins, x_dimMin, x_dimMax, _y_bins, y_dimMin, y_dimMax );
            }
        }
    }


    for( int i = 0; i < _x_bins; i++ )
    {
        for( int j = 0; j < _y_bins; j++ )
        {
            for( int k = 0; k < size; k++ )
            {
                TString i_str;
                TString j_str;
                i_str += i;
                j_str += j;
                i_str.Remove( TString::kLeading, ' ' );
                j_str.Remove( TString::kLeading, ' ' );

                _pixelX_str[i][j][k] = "kBarX_" + type_str[k] + "_" + i_str + "_" + j_str;
                _pixelY_str[i][j][k] = "kBarY_" + type_str[k] + "_" + i_str + "_" + j_str;
                _pixelZ_str[i][j][k] = "kBarZ_" + type_str[k] + "_" + i_str + "_" + j_str;

                _pixelX[i][j][k] = TH1F( _pixelX_str[i][j][k], "", 200, -1, 1 );
                _pixelY[i][j][k] = TH1F( _pixelY_str[i][j][k], "", 200, -1, 1 );
                _pixelZ[i][j][k] = TH1F( _pixelZ_str[i][j][k], "", 200, 0, 1 );


                _kBarXsum[i][j][k] = 0;
                _kBarYsum[i][j][k] = 0;
                _kBarZsum[i][j][k] = 0;

                _freq[i][j][k] = 0;
            }
        }

        if( _mcpMode )
            cout << 145 + (i+1)*18*_y_bins << " / " << _x_bins*_y_bins*size*3+145 << " initialized histograms" << endl;
        else
            cout << 19 + (i+1)*18*_y_bins << " / " << _x_bins*_y_bins*size*3+19 << " initialized histograms" << endl;
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

//     kBarY = kBarY + 500/kBarZ * 1.47/300; // for time difference (here: kBarY = time), start position z=-500
//     kBarY = kBarY + 500/kBarZ * 1.58/300; // with group refractive index

    Int_t pxX = -666;
    Int_t pxY = -666;

    if( !_effiMode || ( wavelength < 700 && (_rand.Uniform() < _effi[(int)(wavelength/10+0.5)]) ) )
    {
        if( _mcpMode )
        {
            Int_t n_test = 0;

            for( int i = 0; i < _n_mcp; i++)
            {
                if( hitPosX >= _minX_active[i] && hitPosX < _maxX_active[i] && hitPosY >= _minY_active[i] && hitPosY < _maxY_active[i] )
                {
                    pxX = TMath::FloorNint( (hitPosX - _minX_active[i])*_y_bins / _mcp_active ) + _y_bins*i;
                    pxY = TMath::FloorNint( (hitPosY - _minY_active[i])*_y_bins / _mcp_active );

                    n_test++;
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
            pxX = TMath::FloorNint( (hitPosX + _fishtank_width/2) / _resolution );
            pxY = TMath::FloorNint( (hitPosY + _fishtank_height/2) / _resolution );
        }
    }
    else
        return kTRUE;

    if( pxX == -666 )
        return kTRUE;


    _pixelX[ pxX ][ pxY ][ all ].Fill( kBarX );
    _pixelY[ pxX ][ pxY ][ all ].Fill( kBarY );
    _pixelZ[ pxX ][ pxY ][ all ].Fill( kBarZ );


    _kBarXsum[ pxX ][ pxY ][ all ] += kBarX;
    _kBarYsum[ pxX ][ pxY ][ all ] += kBarY;
    _kBarZsum[ pxX ][ pxY ][ all ] += kBarZ;

    _freq[ pxX ][ pxY ][ all ]++;


    Int_t nBoxRefl = 0;

    for( int j = 0; j < index_pos; j++ )
    {
        if( j > 0 && posZ[j] > _airgap + 0.0001 && posZ[j] < hitPosZ ) // need tiny shift to check the reflections at the fishtank sides
        {
//                 cout << "Reflection at fishtank side for photonID: " << n_ph << " ; posZ: " << posZ[j]
//                         << " ; pixel: (" << pxX << "," << pxY << ")" << endl;

            nBoxRefl++;
            if( nBoxRefl > 2 )
                cout << "Photon hits more than one time the fishtank side walls" << endl;


            TVector3 dirBefore( posX[j] - posX[j-1], posY[j] - posY[j-1], posZ[j] - posZ[j-1] );
            TVector3 dirAfter(  posX[j+1] - posX[j], posY[j+1] - posY[j], posZ[j+1] - posZ[j] );
            dirBefore = dirBefore.Unit();
            dirAfter = dirAfter.Unit();

//                 cout << "before: " << dirBefore.X() << " " << dirBefore.Y() << " " << dirBefore.Z() <<
//                         "  after: " << dirAfter.X() << " " << dirAfter.Y() << " " << dirAfter.Z() << endl;

            if( posX[j] > 0 && ( (dirBefore.X() > 0 && dirAfter.X() < 0) || (dirBefore.X() < 0 && dirAfter.X() > 0) ) )
            {
//                     cout << "left" << endl;

                _pixelX[ pxX ][ pxY ][ left ].Fill( kBarX );
                _pixelY[ pxX ][ pxY ][ left ].Fill( kBarY );
                _pixelZ[ pxX ][ pxY ][ left ].Fill( kBarZ );

                _kBarXsum[ pxX ][ pxY ][ left ] += kBarX;
                _kBarYsum[ pxX ][ pxY ][ left ] += kBarY;
                _kBarZsum[ pxX ][ pxY ][ left ] += kBarZ;

                _freq[ pxX ][ pxY ][ left ]++;

            }
            else if( posX[j] < 0 && ( (dirBefore.X() > 0 && dirAfter.X() < 0) || (dirBefore.X() < 0 && dirAfter.X() > 0) ) )
            {
//                     cout << "right" << endl;

                _pixelX[ pxX ][ pxY ][ right ].Fill( kBarX );
                _pixelY[ pxX ][ pxY ][ right ].Fill( kBarY );
                _pixelZ[ pxX ][ pxY ][ right ].Fill( kBarZ );

                _kBarXsum[ pxX ][ pxY ][ right ] += kBarX;
                _kBarYsum[ pxX ][ pxY ][ right ] += kBarY;
                _kBarZsum[ pxX ][ pxY ][ right ] += kBarZ;

                _freq[ pxX ][ pxY ][ right ]++;
            }
            else if( posY[j] > 0 && ( (dirBefore.Y() > 0 && dirAfter.Y() < 0) || (dirBefore.Y() < 0 && dirAfter.Y() > 0) ) )
            {
//                     cout << "up" << endl;

                _pixelX[ pxX ][ pxY ][ up ].Fill( kBarX );
                _pixelY[ pxX ][ pxY ][ up ].Fill( kBarY );
                _pixelZ[ pxX ][ pxY ][ up ].Fill( kBarZ );

                _kBarXsum[ pxX ][ pxY ][ up ] += kBarX;
                _kBarYsum[ pxX ][ pxY ][ up ] += kBarY;
                _kBarZsum[ pxX ][ pxY ][ up ] += kBarZ;

                _freq[ pxX ][ pxY ][ up ]++;
            }
            else if( posY[j] < 0 && ( (dirBefore.Y() > 0 && dirAfter.Y() < 0) || (dirBefore.Y() < 0 && dirAfter.Y() > 0) ) )
            {
//                     cout << "down" << endl;

                _pixelX[ pxX ][ pxY ][ down ].Fill( kBarX );
                _pixelY[ pxX ][ pxY ][ down ].Fill( kBarY );
                _pixelZ[ pxX ][ pxY ][ down ].Fill( kBarZ );

                _kBarXsum[ pxX ][ pxY ][ down ] += kBarX;
                _kBarYsum[ pxX ][ pxY ][ down ] += kBarY;
                _kBarZsum[ pxX ][ pxY ][ down ] += kBarZ;

                _freq[ pxX ][ pxY ][ down ]++;
            }
        }
    }

    if( nBoxRefl == 0)
    {
        _pixelX[ pxX ][ pxY ][ direct ].Fill( kBarX );
        _pixelY[ pxX ][ pxY ][ direct ].Fill( kBarY );
        _pixelZ[ pxX ][ pxY ][ direct ].Fill( kBarZ );

        _kBarXsum[ pxX ][ pxY ][ direct ] += kBarX;
        _kBarYsum[ pxX ][ pxY ][ direct ] += kBarY;
        _kBarZsum[ pxX ][ pxY ][ direct ] += kBarZ;

        _freq[ pxX ][ pxY ][ direct ]++;
    }

//             if( pxX == 27 && pxY == 8)
//                 cout << "photonID: " << n_ph << " ; kBarX: " << kBarX << endl;


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

    pixelTree->Branch( "freq[6]", _px_freq, "freq[6]/I" );


    TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
    canvas->SetLeftMargin(0.11);
    canvas->SetRightMargin(0.11);
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


    for( int i = 0; i < _x_bins; i++)
    {
        for( int j = 0; j < _y_bins; j++)
        {
            for( int k = 0; k < size; k++ )
            {
                if( _freq[i][j][k] != 0 )
                {
                    _pixelX[i][j][k].Draw();
                    canvas->Write( _pixelX_str[i][j][k] );
                    canvas->Clear();

                    _pixelY[i][j][k].Draw();
                    canvas->Write( _pixelY_str[i][j][k] );
                    canvas->Clear();

                    _pixelZ[i][j][k].Draw();
                    canvas->Write( _pixelZ_str[i][j][k] );
                    canvas->Clear();
                }

                if( _mcpMode )
                {
                    Int_t l = TMath::CeilNint( i / 8 );
                    Double_t hitX = (i - l * _y_bins) * _mcp_active / _y_bins + _minX_active[l] + _mcp_active/(2*_y_bins);
                    Double_t hitY = j * _mcp_active / _y_bins + _minY_active[l] + _mcp_active/(2*_y_bins);;

                    if( _freq[i][j][k] == 0 )
                    {
                        _kBarXsum[i][j][k] = 666;
                        _kBarYsum[i][j][k] = 666;
                        _kBarZsum[i][j][k] = 666;
                        _freq[i][j][k] = -1;
                    }

                    _kBarX[k][l].Fill( i-l*_y_bins, j, _kBarXsum[i][j][k] / _freq[i][j][k] );
                    _kBarY[k][l].Fill( i-l*_y_bins, j, _kBarYsum[i][j][k] / _freq[i][j][k] );
                    _kBarZ[k][l].Fill( i-l*_y_bins, j, _kBarZsum[i][j][k] / _freq[i][j][k] );

                    _kBarX_screen[k][l].Fill( hitX, hitY, _kBarXsum[i][j][k] / _freq[i][j][k] );
                    _kBarY_screen[k][l].Fill( hitX, hitY, _kBarYsum[i][j][k] / _freq[i][j][k] );
                    _kBarZ_screen[k][l].Fill( hitX, hitY, _kBarZsum[i][j][k] / _freq[i][j][k] );
                }
                else
                {
                    if( _freq[i][j][k] == 0 )
                    {
                        _kBarXsum[i][j][k] = 666;
                        _kBarYsum[i][j][k] = 666;
                        _kBarZsum[i][j][k] = 666;
                        _freq[i][j][k] = -1;
                    }

                    Double_t hitX = i * _resolution - _fishtank_width/2 + _resolution/2;
                    Double_t hitY = j * _resolution - _fishtank_height/2 + _resolution/2;

                    _kBarX[k][0].Fill( hitX, hitY, _kBarXsum[i][j][k] / _freq[i][j][k] );
                    _kBarY[k][0].Fill( hitX, hitY, _kBarYsum[i][j][k] / _freq[i][j][k] );
                    _kBarZ[k][0].Fill( hitX, hitY, _kBarZsum[i][j][k] / _freq[i][j][k] );

                    _screen->Fill( hitX, hitY, _freq[i][j][ all ] );
                }

                _px_kBarX[k]    =  _kBarXsum[i][j][k] / _freq[i][j][k];
                _px_kBarY[k]    =  _kBarYsum[i][j][k] / _freq[i][j][k];
                _px_kBarZ[k]    =  _kBarZsum[i][j][k] / _freq[i][j][k];

                _px_kBarXerr[k] =  _pixelX[i][j][k].GetRMS();
                _px_kBarYerr[k] =  _pixelY[i][j][k].GetRMS();
                _px_kBarZerr[k] =  _pixelZ[i][j][k].GetRMS();

                _px_freq[k]     =  _freq[i][j][k];
            }

            pixelTree->Fill();
            ClearOutTree();
        }

        cout << (i+1)*18*_y_bins << " drawn histograms" << endl;
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
//         _screen->GetYaxis()->SetTitleOffset( 1.3 );
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
            kBarX_screenTitle = "kBarX_screen_" + type_str[i];
            kBarY_screenTitle = "kBarY_screen_" + type_str[i];
            kBarZ_screenTitle = "kBarZ_screen_" + type_str[i];
        }

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

            _kBarX[i][j].SetTitle( _kBarX_str[i][j] );
            _kBarX[i][j].SetContour(50);
            _kBarX[i][j].GetXaxis()->SetTitle( "x [mm]" );
            _kBarX[i][j].GetXaxis()->CenterTitle();
            _kBarX[i][j].GetYaxis()->SetTitle( "y [mm]" );
            _kBarX[i][j].GetYaxis()->CenterTitle();
//             _kBarX[i][j].GetYaxis()->SetTitleOffset( 1.3 );
            _kBarX[i][j].SetStats( false );
            _kBarX[i][j].Draw("colz");
            canvas->Write( _kBarX_str[i][j] );
            canvas->Clear();

            _kBarY[i][j].SetTitle( _kBarY_str[i][j] );
            _kBarY[i][j].SetContour(50);
            _kBarY[i][j].GetXaxis()->SetTitle( "x [mm]" );
            _kBarY[i][j].GetXaxis()->CenterTitle();
            _kBarY[i][j].GetYaxis()->SetTitle( "y [mm]" );
            _kBarY[i][j].GetYaxis()->CenterTitle();
//             _kBarY[i][j].GetYaxis()->SetTitleOffset( 1.3 );
            _kBarY[i][j].SetStats( false );
            _kBarY[i][j].Draw("colz");
            canvas->Write( _kBarY_str[i][j] );
            canvas->Clear();

            _kBarZ[i][j].SetTitle( _kBarZ_str[i][j] );
            _kBarZ[i][j].SetContour(50);
            _kBarZ[i][j].GetXaxis()->SetTitle( "x [mm]" );
            _kBarZ[i][j].GetXaxis()->CenterTitle();
            _kBarZ[i][j].GetYaxis()->SetTitle( "y [mm]" );
            _kBarZ[i][j].GetYaxis()->CenterTitle();
//             _kBarZ[i][j].GetYaxis()->SetTitleOffset( 1.3 );
            _kBarZ[i][j].SetStats( false );
            _kBarZ[i][j].Draw("colz");
            canvas->Write( _kBarZ_str[i][j] );
            canvas->Clear();


            if( _mcpMode )
            {
                canvasX->cd();

                _kBarX_screen[i][j].SetContour(50);
                _kBarX_screen[i][j].SetMinimum(-1);
                _kBarX_screen[i][j].SetMaximum(+1);
                _kBarX_screen[i][j].SetStats( false );
                _kBarX_screen[i][j].Draw("same colz");

                _mcpLabel[j]->Draw("same");

                for( int k = 0; k < 4; k++ )
                {
                    _line_mcpCase[j][k].Draw("same");
                    _line_mcpArea[j][k].Draw("same");
                }


                canvasY->cd();

                _kBarY_screen[i][j].SetContour(50);
                _kBarY_screen[i][j].SetMinimum(-1);
                _kBarY_screen[i][j].SetMaximum(+1);
                _kBarY_screen[i][j].SetStats( false );
                _kBarY_screen[i][j].Draw("same colz");

                _mcpLabel[j]->Draw("same");

                for( int k = 0; k < 4; k++ )
                {
                    _line_mcpCase[j][k].Draw("same");
                    _line_mcpArea[j][k].Draw("same");
                }


                canvasZ->cd();

                _kBarZ_screen[i][j].SetContour(50);
                _kBarZ_screen[i][j].SetMinimum(-1);
                _kBarZ_screen[i][j].SetMaximum(+1);
                _kBarZ_screen[i][j].SetStats( false );
                _kBarZ_screen[i][j].Draw("same colz");

                _mcpLabel[j]->Draw("same");

                for( int k = 0; k < 4; k++ )
                {
                    _line_mcpCase[j][k].Draw("same");
                    _line_mcpArea[j][k].Draw("same");
                }
            }
        }

        if( _mcpMode )
        {
            _screen->GetXaxis()->SetTitle( "x [mm]" );
            _screen->GetXaxis()->CenterTitle();
            _screen->GetYaxis()->SetTitle( "y [mm]" );
            _screen->GetYaxis()->CenterTitle();
//             _screen->GetYaxis()->SetTitleOffset( 1.3 );

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
