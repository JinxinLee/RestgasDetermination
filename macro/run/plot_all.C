#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"
#include <iostream>
#include "TH1F.h"
#include "TRandom.h"

using std::cout;
using std::endl;

bool plot_all(TString fn="QA_histograms.root")
{
  TCanvas *c1 = new TCanvas();
  c1->Divide(6,5);
  
  TFile *f=new TFile(fn,"READ");
  if (!f->IsZombie())
    {
      TKey *key;
      TIter next(f->GetListOfKeys());
      
      Int_t yy = 0;
      while ( (key = (TKey*)next()) )
	{
	  c1->cd(yy+1);
	  TObject *obj = key->ReadObj();
	  
	  // only check TH1Fs
	  if (!obj->InheritsFrom("TH1F")) continue;
	  
	  TString name = obj->GetName();
	  cout << name << endl;
	  TH1F* h  = (TH1F*) obj; 
	  h->SetLineColor(2);
	  h->SetLineWidth(2);
	  h->Draw();
	  yy++;
	}
      
    }
}
  
