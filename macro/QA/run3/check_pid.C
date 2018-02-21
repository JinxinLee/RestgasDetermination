#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"
#include <iostream>
#include "TH1F.h"
#include "TRandom.h"

using std::cout;
using std::endl;

bool check_pid(TString fn="pid_plot.root", TString fn2="pid_target_G3.root", double minP = 0.03, int minev = 3, int maxfail=3)
{
        TStopwatch fTimer;
  bool fTest=kFALSE;
        TString templateFile = gSystem->Getenv("VMCWORKDIR");
        templateFile += "/macro/QA/run3/";
        templateFile += fn2;

  TFile *f=new TFile(fn,"READ");
  if (!f->IsZombie())
  {
    TFile *f2=new TFile(templateFile,"READ");

    TKey *key;
    TIter next(f->GetListOfKeys());

    int failcount = 0;

    while ( (key = (TKey*)next()) )
    {
      TObject *obj = key->ReadObj();

      // only check TH1Fs
      if (!obj->InheritsFrom("TH1F")) continue;

      TString name = obj->GetName();
      TH1F* h  = (TH1F*) obj;
      TH1F* h2 = (TH1F*) f2->Get(name);

      if ( h->GetEntries()<minev )
      {
        cout << "Histogram (almost) empty : " << name << " \"" << h2->GetTitle() << "\":  N = " <<  h->GetEntries() << endl;
        failcount++;
      }
      else
      {
        double P = h2->KolmogorovTest(h);
            TString htitile=h2->GetTitle();
            htitile.ReplaceAll(" ","");htitile.ReplaceAll("#","");htitile.ReplaceAll("(","");htitile.ReplaceAll(")","");
            htitile.ReplaceAll("/","");htitile.ReplaceAll("^","");htitile.ReplaceAll("[","");htitile.ReplaceAll("]","");
            htitile.ReplaceAll(":","");htitile.ReplaceAll(".","");htitile.ReplaceAll("{","");htitile.ReplaceAll("}","");
            cout << "<DartMeasurement name=\""<<htitile.Data()<<"\" type=\"numeric/double\">";
            cout << P;
            cout << "</DartMeasurement>" << endl;
        if ( P<minP )
        {
          cout << "Incompatible distribution: " << name << " \"" << h2->GetTitle() << "\":  P = " << P << endl;
          failcount++;
        }
      }

    }

    if (failcount<maxfail) fTest = kTRUE;
      cout << "<DartMeasurement name=\"failcount\" type=\"numeric/int\">"<<failcount<<"</DartMeasurement>"<<endl;
  }

  // Extract the maximal used memory an add is as Dart measurement
  // This line is filtered by CTest and the value send to CDash
  FairSystemInfo sysInfo;
  Float_t maxMemory=sysInfo.GetMaxMemory();
  cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
  cout << maxMemory;
  cout << "</DartMeasurement>" << endl;

  fTimer.Stop();
  Double_t rtime = fTimer.RealTime();
  Double_t ctime = fTimer.CpuTime();

  Float_t cpuUsage=ctime/rtime;
  cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
  cout << cpuUsage;
  cout << "</DartMeasurement>" << endl;

  cout << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
      << "s" << endl;
  cout << "CPU usage " << cpuUsage*100. << "%" << endl;
  cout << "Max Memory " << maxMemory << " MB" << endl;

  if (fTest){
    cout << " Macro finished successfully" << endl;
    cout << " All ok " << endl;
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;
  }

    return fTest;
}
