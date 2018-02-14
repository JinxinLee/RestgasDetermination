#include <iostream>
#include "TStyle.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TMath.h"

//
// Johan Messchendorp - August 2009
// 
// FindTheBin - scan through a 2D spectrum to find the best bin 
//              that fit the value (value_x,value_y);
//              The results are stored in bin_x and bin_y;
//              Function returns 0 if successfull, -1/-2 in case 
//              not found for x/y, respectively.
//

Int_t FindTheBin(TH2 *lookup_table, Float_t value_x, Float_t value_y, Int_t &bin_x, Int_t &bin_y)
{
 bin_x = lookup_table->GetXaxis()->FindBin(value_x);
 bin_y = lookup_table->GetYaxis()->FindBin(value_y);

 if ((bin_x < 1) || (bin_x > lookup_table->GetXaxis()->GetNbins()))
  {
   bin_x = -1; bin_y = -1; 
   return -1;
  }

 if ((bin_y < 1) || (bin_y > lookup_table->GetYaxis()->GetNbins()))
  {
   bin_x = -1; bin_y = -1; 
   return -2;
  }

 return 0; // Succes
} 

// 
// GetValueInZ - Obtains the value in Z given lookup_table 
//               that fit the value (value_x,value_y);
//               Funtion returns the value in Z.
//

Double_t GetValueInZ(TH2 *lookup_table, Float_t value_x, Float_t value_y, Bool_t use_interpolation = kFALSE)
{

 if (use_interpolation)
  {
   //
   // Use the interpolarion routine of ROOT:
   // Interpolate approximates the value via bilinear
   // interpolation based on the four nearest bin centers
   // see Wikipedia, Bilinear Interpolation
   // Andy Mastbaum 10/8/2008
   // vaguely based on R.Raja 6-Sep-2008
   //
   return (lookup_table->Interpolate(value_x,value_y));
  }
 else
  {

   Int_t binx, biny, retval;

   retval = FindTheBin(lookup_table, value_x, value_y, binx, biny);
   if (retval)
    {
     cout << "<E> Error in FindTheBin, check your table and input values!!!!: " << retval << endl;
     return 0;
    }

   return (lookup_table->GetBinContent(binx,biny));
  }
 return 0;
}

void TestIt(Bool_t use_interpolation=kFALSE)
{
 //
 // Create two 2D spectrum, not equal in binning!
 // Try to set the interpolation flag to kTRUE and compare with kFALSE
 //

 gStyle->SetPalette(1);

 Double_t xlowarray_1[] = {0,10,20,30,40,50,60,70,80,90,100};
 Double_t ylowarray_1[] = {0,10,20,30,40,50,60,70,80,90,100};

 TH2D *h1 = new TH2D("h1","Input spectrum",
	             (sizeof(xlowarray_1)/sizeof(Double_t))-1,xlowarray_1,
		     (sizeof(ylowarray_1)/sizeof(Double_t))-1,ylowarray_1);

 Double_t xlowarray_2[] = {0,10,20,30,35,40,45,46,47,48,49,50,51,52,53,54,55,60,65,70,80,90,100};
 Double_t ylowarray_2[] = {0,10,20,30,35,40,45,46,47,48,49,50,51,52,53,54,55,60,65,70,80,90,100};

 TH2D *h2 = new TH2D("h2","Check spectrum",
	             (sizeof(xlowarray_2)/sizeof(Double_t))-1,xlowarray_2,
		     (sizeof(ylowarray_2)/sizeof(Double_t))-1,ylowarray_2);

 //
 // Fill Input spectrum with random numbers
 //

 for (Int_t i = 1; i < (h1->GetXaxis()->GetNbins() + 1); i++)
   {
    for (Int_t j = 1; j < (h1->GetYaxis()->GetNbins() + 1); j++)
     {
       h1->SetBinContent(i,j,
	                 TMath::Gaus(h1->GetXaxis()->GetBinCenter(j),50,20)+
                         TMath::Gaus(h1->GetXaxis()->GetBinCenter(i),50,10));
     } 
   }

 //
 // Fill check spectrum using lookup
 //

 Double_t valx, valy, valz;
 for (Int_t i = 1; i < (h2->GetXaxis()->GetNbins() + 1); i++)
   {
    valx = h2->GetXaxis()->GetBinCenter(i);
    for (Int_t j = 1; j < (h2->GetYaxis()->GetNbins() + 1); j++)
     {
       valy = h2->GetYaxis()->GetBinCenter(j);
       cout << "<I> Checking (" << valx << "," << valy << ")" << endl; 
       valz = GetValueInZ((TH2*) h1, valx, valy, use_interpolation);
       h2->SetBinContent(i,j,valz);
     } 
   }

//
// At this point h1 should be equal to h2, check it yourself!
//

  TCanvas *c = new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  h1->Draw("zcol");
  c->cd(2);
  h2->Draw("zcol");

  cout << "Compare TestIt(kTRUE) with TestIt(kFALSE), interpolation really improves! Cool, heh!" << endl;
} 