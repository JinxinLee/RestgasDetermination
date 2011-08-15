
//======
// ROOT
//======
#include <TString.h>
#include <TChain.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>



void effiContiPlot( TString var_str = "incidence_theta" )
{
  const int n_files = 4;

  TChain *chain = new TChain( "tree" );

  chain->Add( "prototype_test0_effiConti.root" );
  chain->Add( "prototype_test10_effiConti.root" );
  chain->Add( "prototype_test20_effiConti.root" );
  chain->Add( "prototype_test30_effiConti.root" );


  Double_t min_var = chain->GetMinimum( var_str );
  Double_t max_var = chain->GetMaximum( var_str );
  Double_t max_var_plot = max_var + (max_var - min_var) / (n_files - 1);

  Double_t min_par = chain->GetMinimum( "particle_number" );
  Double_t max_par = chain->GetMaximum( "particle_number" );

  if( min_par != max_par )
    cout << "Files have different particle numbers and hence per track plot is wrong!" << endl;


  const int n_plots = 10;

  TH1F *weight[ n_plots ];
  TH1F *hist[ n_plots ];

  for( int i = 0; i < n_plots; i++ )
  {
    weight[i] = new TH1F();
    hist[i]   = new TH1F();
  }

  TString weight_str[ n_plots ];
  TString hist_str[ n_plots ];


  // total reflection
  weight_str[0] = "";
  hist_str[0]   = "!b_lost_refl && !(b_lost_bar && !b_back)";

  // absorption bar (direct)
  weight_str[1] = hist_str[0];
  hist_str[1]   = hist_str[0] + " && b_abs_bar && !b_abs_limit && !b_back";

  // out of bar
  weight_str[2] = "";
  hist_str[2] = hist_str[0] + " && !(b_abs_bar && !b_back) && !(b_abs_limit && !b_back)";

  // bar to tank
  weight_str[3] = hist_str[2];
  hist_str[3]   = "!b_lost_refl && !b_lost_bar && !b_lost_gap && !b_abs_limit && !b_abs_bar && !b_abs_lens";

  // absorption tank sides
  weight_str[4] = hist_str[3];
  hist_str[4]   = hist_str[3] + " && !b_abs_sides";

  // absorption tank
  weight_str[5] = hist_str[4];
  hist_str[5]   = hist_str[4] + " && b_abs_tank";

  // hits
  weight_str[6] = "";
  hist_str[6]   = "measured";

  // N measured per track
  weight_str[7] = ""; // won't use
  hist_str[7]   = "measured && b_effi";

  // measured (ratio: back reflected - direct)
  weight_str[8] = "measured && !b_back && b_effi";
  hist_str[8]   = "measured && b_back && b_effi";

  // measured
  weight_str[9] = "";
  hist_str[9]   = "measured && b_effi";


  TString n_files_str;
  n_files_str += n_files;
  n_files_str.Remove( TString::kLeading, ' ' );

  TString min_var_str;
  min_var_str += min_var;
  min_var_str.Remove( TString::kLeading, ' ' );

  TString max_var_plot_str;
  max_var_plot_str += max_var_plot;
  max_var_plot_str.Remove( TString::kLeading, ' ' );

  TString weight_var_str = var_str + ">>w(" + n_files_str + "," + min_var_str + "," + max_var_plot_str + ")";
  TString hist_var_str   = var_str + ">>h(" + n_files_str + "," + min_var_str + "," + max_var_plot_str + ")";

  TString max_par_str;
  max_par_str += max_par;
  max_par_str.Remove( TString::kLeading, ' ' );


  TCanvas *c_helper = new TCanvas( "c_helper", "", 1 );
  c_helper->Draw();
  c_helper->cd();


  for( int i = 0; i < n_plots; i++ )
  {
    TH1F *helper;

    TString i_str;
    i_str += i;
    i_str.Remove( TString::kLeading, ' ' );

    TString w_str = "w" + i_str;
    TString h_str = "h" + i_str;

    chain->Draw( weight_var_str, weight_str[i] );
    helper = (TH1F*) gDirectory->Get("w");
    helper->Copy( *weight[i] );
    weight[i]->SetName( w_str );

    chain->Draw( hist_var_str, hist_str[i] );
    helper = (TH1F*) gDirectory->Get("h");;
    helper->Copy( *hist[i] );
    hist[i]->SetName( h_str );
    if( i!= 7 )
      hist[i]->Divide( weight[i] );


    if( i == 7 ) // per track
    {
      TF1 *f1 = new TF1( "f1", max_par_str, min_var, max_var_plot );
      hist[i]->Divide(f1);
    }

    switch( i )
    {
      case 0:  hist[0]->GetYaxis()->SetTitle( "total reflection" );              break;
      case 1:  hist[1]->GetYaxis()->SetTitle( "absorption bar (direct)" );       break;
      case 2:  hist[2]->GetYaxis()->SetTitle( "out of bar" );                    break;
      case 3:  hist[3]->GetYaxis()->SetTitle( "bar to tank" );                   break;
      case 4:  hist[4]->GetYaxis()->SetTitle( "absorption tank sides" );         break;
      case 5:  hist[5]->GetYaxis()->SetTitle( "absorption tank" );               break;
      case 6:  hist[6]->GetYaxis()->SetTitle( "hits" );                          break;
      case 7:  hist[7]->GetYaxis()->SetTitle( "N measured" );                    break;
      case 8:  hist[8]->GetYaxis()->SetTitle( "measured (reflected / direct)" ); break;
      case 9:  hist[9]->GetYaxis()->SetTitle( "measured" );                      break;
    }
  }


  c_helper->Close();
  TCanvas *canvas = new TCanvas( "canvas", "", 200, 10, 700, 990 );
  canvas->Divide( 2, TMath::CeilNint( n_plots / 2. ) );
  canvas->Draw();
  canvas->cd();

  gStyle->SetOptStat(0);

  for( int i = 0; i < n_plots; i++ )
  {

    hist[i]->GetXaxis()->CenterTitle();
    hist[i]->GetXaxis()->SetTitle( "particle incidence angle [deg]" );
    hist[i]->GetYaxis()->CenterTitle();
    hist[i]->GetYaxis()->SetTitleOffset( 1.1 );

    canvas->cd(i+1);
    hist[i]->Draw();
  }
}
