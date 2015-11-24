// ******************************************************
// Psi' -> J/psi (-> e+ e-) pi+ pi- Analysis Example Task
//
// K.Goetzen 7/2012
// *******************************************************

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStopwatch.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include <string>
#include <iostream>

// RHO stuff
#include "RhoCandidate.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"

// Analysis Tools
#include "PndMyAnalysisTask.h"
#include "PndAnalysis.h"

// Fitters
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndKinFitter.h"


using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndMyAnalysisTask::PndMyAnalysisTask() :
  FairTask("Panda Analysis Task")
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMyAnalysisTask::~PndMyAnalysisTask() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMyAnalysisTask::Init()
{
  //cout << " Inside the Init function****" << endl;


  // initialize analysis object
  theAnalysis=new PndAnalysis();

  // Create and register output array
  /*  hjpsimass = new TH1F("hjpsimass","jpsi cands",100,0.0,4.5);
    hpsimass = new TH1F("hpsimass","psi cands",100,0.,4.5);*/

  hjpsim_nopid = new TH1F("hjpsim_nopid","J/#psi mass (no pid)",200,0,4);
  hpsim_nopid  = new TH1F("hpsim_nopid","#psi(2S) mass (no pid)",200,0,5);
  hjpsim_lpid = new TH1F("hjpsim_lpid","J/#psi mass (loose pid)",200,0,4);
  hpsim_lpid  = new TH1F("hpsim_lpid","#psi(2S) mass (loose pid)",200,0,5);
  hjpsim_tpid = new TH1F("hjpsim_tpid","J/#psi mass (true pid)",200,0,4);
  hpsim_tpid  = new TH1F("hpsim_tpid","#psi(2S) mass (true pid)",200,0,5);

  hjpsim_ftm = new TH1F("hjpsim_ftm","J/#psi mass (full truth match)",200,0,4);
  hpsim_ftm  = new TH1F("hpsim_ftm","#psi(2S) mass ( truth match)",200,0,5);
  hjpsim_nm = new TH1F("hjpsim_nm","J/#psi mass (no truth match)",200,0,4);
  hpsim_nm  = new TH1F("hpsim_nm","#psi(2S) mass (no truth match)",200,0,5);

  hjpsim_diff = new TH1F("hjpsim_diff","J/#psi mass diff to truth",100,-2,2);
  hpsim_diff  = new TH1F("hpsim_diff","#psi(2S) mass diff to truth",100,-2,2);

  hjpsim_vf   = new TH1F("hjpsim_vf","J/#psi mass vertex fit",200,0,4);
  hjpsim_4cf  = new TH1F("hjpsim_4cf","J/#psi mass (4C fit)",200,0,4);
  hjpsim_mcf  = new TH1F("hjpsim_mcf","J/#psi mass (mass constraint fit)",200,0,4);

  hjpsi_chi2_vf  = new TH1F("hjpsi_chi2_vf","J/#psi, #chi^{2} vertex fit",100,0,10);
  hpsi_chi2_4c   = new TH1F("hpsi_chi2_4c","#psi, #chi^{2} 4C fit",100,0,250);
  hjpsi_chi2_mf  = new TH1F("hjpsi_chi2_mf","J/#psi, #chi^{2} mass fit",100,0,10);

  hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-2,2,100,-2,2);

  ntp=new RhoTuple("ntp","the J/psi ntuple");
  ntp2=new RhoTuple("ntp2","the psi(2S) ntuple");

  // **** mass selector and McTruthMatcher
  //
  jpsiMassSel = new RhoMassParticleSelector("jpsiSelector" , 3.097, 0.6);

  evcount=0;
  epmax=0;
  emmax=0;
  pipmax=0;
  pimmax=0;
  mcmax=0;

  cout << "-I- PndMyAnalysisTask: Intialization successfull" << endl;

  timer=new TStopwatch();
  timer->Start();

  return kSUCCESS;
}

void PndMyAnalysisTask::SetParContainers()
{

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) { Fatal("SetParContainers", "No analysis run"); }

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");


}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMyAnalysisTask::Exec(Option_t* opt)
{
  Int_t j=0;

  theAnalysis->GetEvent();

  if (!(++evcount%100)) {
    cout <<"evt "<<evcount;
    timer->Stop();
    cout <<"  t="<< timer->CpuTime();
    timer->Start();
    cout <<"  Cand Watermark = "<<RhoFactory::Instance()->GetCandidateWatermark();
    cout <<" epmax:"<<epmax;
    cout <<" emmax:"<<emmax;
    cout <<" pipmax:"<<pipmax;
    cout <<" pimmax:"<<pimmax;
    cout <<" mcmax:"<<mcmax;
    cout <<endl;
  }


  // **** create all the particle lists we'll need for rebuilding the decay tree
  //
  //RhoCandList eplus, eminus, piplus, piminus, jpsi, psi, mctrk;

  RhoCandList all, chrg, el, eplus, eminus, piplus, piminus, jpsi, jpsi2, psi2s;
  RhoCandList epsel, emsel;

  TLorentzVector ini(0, 0, 6.231552, 7.240065);


//  cout <<"    #### mct="<<mctrk.GetLength()<<endl;

  // *** Select with no PID info ('All'); type and mass are set
  theAnalysis->FillList(eplus,   "ElectronAllPlus","PidAlgoEmcBayes");
  theAnalysis->FillList(eminus,  "ElectronAllMinus","PidAlgoEmcBayes");
  theAnalysis->FillList(piplus,  "PionAllPlus","PidAlgoEmcBayes");
  theAnalysis->FillList(piminus, "PionAllMinus","PidAlgoEmcBayes");


  int nep = eplus.GetLength();
  int nem = eminus.GetLength();
  int npip = piplus.GetLength();
  int npim = piminus.GetLength();

  if (nep>epmax) { epmax=nep; }
  if (nem>emmax) { emmax=nem; }
  if (npip>pipmax) { pipmax=npip; }
  if (npim>pimmax) { pimmax=npim; }

//  jpsi.Combine(eplus, eminus);
//  FillMassHisto(hjpsimass, jpsi);
//  jpsi.Select(jpsiMSel);
//
//  psi.Combine(jpsi, piplus, piminus);
//  FillMassHisto(hpsimass, psi);


  jpsi.Combine(eplus, eminus);

  // *** do the truth match for jpsi
  jpsi.SetType("J/psi");
  for (j=0; j<jpsi.GetLength(); ++j) {
    hjpsim_nopid->Fill( jpsi[j]->M() );
    if (theAnalysis->McTruthMatch(jpsi[j])) { hjpsim_ftm->Fill( jpsi[j]->M() ); }
    else { hjpsim_nm->Fill( jpsi[j]->M() ); }
  }

  // *** do vertex fitting (J/psi)
  for (j=0; j<jpsi.GetLength(); ++j) {
    PndKinVtxFitter vtxfitter(jpsi[j]);        // instantiate a vertex fitter

    vtxfitter.Fit();
    const RhoCandidate* jfit = jpsi[j]->GetFit();  // access the fitted cand
    TVector3 jVtx=jfit->Pos();                         // and the decay vertex position

    double chi2_vtx=vtxfitter.GetChi2();            // access chi2 of fit
    hjpsi_chi2_vf->Fill(chi2_vtx);

    if (chi2_vtx<2) {                                  // when good enough, fill some histos
      hjpsim_vf->Fill(jfit->M());
      hvpos->Fill(jVtx.X(),jVtx.Y());
    }

    bool match = theAnalysis->McTruthMatch(jpsi[j]);

    //ntp->Fill(jpsi[j]->M(),jfit->M(),chi2_vtx,jVtx.X(),jVtx.Y(),jVtx.Z(),match);

    RhoCandidate* ep = jpsi[j]->Daughter(0);
    RhoCandidate* em = jpsi[j]->Daughter(1);

    ntp->Column("jpsim",  jpsi[j]->M(),      -1.);
    ntp->Column("jpsitht",  jpsi[j]->P4().Theta(), -999.);
    ntp->Column("jpsip",  jpsi[j]->P(),      -1.);
    ntp->Column("jpsiphi",  jpsi[j]->P4().Phi(),   -999.);

    ntp->Column("eptht",  ep->P4().Theta(),   -999.);
    ntp->Column("epp",    ep->P(),        -1.);
    ntp->Column("epphi",  ep->P4().Phi(),     -999.);
    ntp->Column("eppid0",   ep->GetPidInfo(0),    -1.);
    ntp->Column("eppid1",   ep->GetPidInfo(1),    -1.);
    ntp->Column("eppid2",   ep->GetPidInfo(2),    -1.);
    ntp->Column("eppid3",   ep->GetPidInfo(3),    -1.);
    ntp->Column("eppid4",   ep->GetPidInfo(4),    -1.);

    ntp->Column("emtht",  em->P4().Theta(),   -999.);
    ntp->Column("emp",    em->P(),        -1.);
    ntp->Column("emphi",  em->P4().Phi(),     -999.);
    ntp->Column("empid0",   em->GetPidInfo(0),    -1.);
    ntp->Column("empid1",   em->GetPidInfo(1),    -1.);
    ntp->Column("empid2",   em->GetPidInfo(2),    -1.);
    ntp->Column("empid3",   em->GetPidInfo(3),    -1.);
    ntp->Column("empid4",   em->GetPidInfo(4),    -1.);

    ntp->Column("jpsifitm", jfit->M(),        -1.);
    ntp->Column("vchi2",  chi2_vtx,       9999.);
    ntp->Column("vx",   jVtx.X(),       9999.);
    ntp->Column("vy",   jVtx.Y(),       9999.);
    ntp->Column("vz",   jVtx.Z(),       9999.);
    ntp->Column("jmct",     match,          kFALSE);

    ntp->DumpData();
  }

  // *** some rough mass selection
  //jpsi.Select(jpsiMassSel);

  psi2s.Combine(jpsi, piplus, piminus);

  // *** do the truth match for psi2s
  psi2s.SetType("psi(2S)0");

  for (j=0; j<psi2s.GetLength(); ++j) {
    hpsim_nopid->Fill( psi2s[j]->M() );
    if (theAnalysis->McTruthMatch(psi2s[j])) {
      hpsim_ftm->Fill( psi2s[j]->M() );
       
      ///TODO  recheck the MC truth access! Is the truth list available?
      const RhoCandidate* truePsi = psi2s[j]->GetMcTruth();
      const RhoCandidate* trueJ   = psi2s[j]->Daughter(0)->GetMcTruth();

      hjpsim_diff->Fill(trueJ->M()-psi2s[j]->Daughter(0)->M());
      hpsim_diff->Fill(truePsi->M()-psi2s[j]->M());
    } else { hpsim_nm->Fill( psi2s[j]->M() ); }
  }

  // *** do 4c fit (initial psi(2S) system)
  for (j=0; j<psi2s.GetLength(); ++j) {
    Pnd4CFitter fitter(psi2s[j],ini);
    fitter.FitConserveMasses();

    double chi2_4c=fitter.GetChi2();
    hpsi_chi2_4c->Fill(chi2_4c);

    //const RhoCandidate* jfit = (psi2s[j]->Daughter(0))->GetFit();

    const RhoCandidate* epfit = (psi2s[j]->Daughter(0)->Daughter(0))->GetFit();
    const RhoCandidate* emfit = (psi2s[j]->Daughter(0)->Daughter(1))->GetFit();

    TLorentzVector tlvepf = epfit->P4();
    TLorentzVector tlvemf = emfit->P4();

    hjpsim_4cf->Fill((tlvepf+tlvemf).M());



    RhoCandidate* ep = psi2s[j]->Daughter(0)->Daughter(0);
    RhoCandidate* em = psi2s[j]->Daughter(0)->Daughter(1);
    RhoCandidate* pip = psi2s[j]->Daughter(1);
    RhoCandidate* pim = psi2s[j]->Daughter(2);
    RhoCandidate* jpsic = psi2s[j]->Daughter(0);

    bool match = theAnalysis->McTruthMatch(psi2s[j]);
    bool matchj = theAnalysis->McTruthMatch(jpsic);

    ntp2->Column("psim",    psi2s[j]->M(),   -1.);
    ntp2->Column("fcchi2",  chi2_4c,    9999.);
    ntp2->Column("psimct",   match,     kFALSE);

    ntp2->Column("jmct",   matchj,      kFALSE);
    ntp2->Column("jfitm", (tlvepf+tlvemf).M(),    -1.);
    ntp2->Column("jpsim", jpsic->M(), -1.);
    ntp->Column("jpsitht",  jpsic->P4().Theta(),  -999.);
    ntp->Column("jpsip",  jpsic->P(),     -1.);
    ntp->Column("jpsiphi",  jpsic->P4().Phi(),    -999.);

    ntp2->Column("eptht", ep->P4().Theta(),   -999.);
    ntp2->Column("epp",   ep->P(),        -1.);
    ntp2->Column("epphi", ep->P4().Phi(),     -999.);
    ntp2->Column("eppid0",   ep->GetPidInfo(0),   -1.);
    ntp2->Column("eppid1",   ep->GetPidInfo(1),   -1.);
    ntp2->Column("eppid2",   ep->GetPidInfo(2),   -1.);
    ntp2->Column("eppid3",   ep->GetPidInfo(3),   -1.);
    ntp2->Column("eppid4",   ep->GetPidInfo(4),   -1.);

    ntp2->Column("emtht", em->P4().Theta(),   -999.);
    ntp2->Column("emp",   em->P(),        -1.);
    ntp2->Column("emphi", em->P4().Phi(),     -999.);
    ntp2->Column("empid0",   em->GetPidInfo(0),   -1.);
    ntp2->Column("empid1",   em->GetPidInfo(1),   -1.);
    ntp2->Column("empid2",   em->GetPidInfo(2),   -1.);
    ntp2->Column("empid3",   em->GetPidInfo(3),   -1.);
    ntp2->Column("empid4",   em->GetPidInfo(4),   -1.);

    ntp2->Column("piptht",  pip->P4().Theta(),    -999.);
    ntp2->Column("pipp",    pip->P(),       -1.);
    ntp2->Column("pipphi",  pip->P4().Phi(),      -999.);
    ntp2->Column("pippid0",   pip->GetPidInfo(0),   -1.);
    ntp2->Column("pippid1",   pip->GetPidInfo(1),   -1.);
    ntp2->Column("pippid2",   pip->GetPidInfo(2),   -1.);
    ntp2->Column("pippid3",   pip->GetPidInfo(3),   -1.);
    ntp2->Column("pippid4",   pip->GetPidInfo(4),   -1.);

    ntp2->Column("pimtht",  pim->P4().Theta(),    -999.);
    ntp2->Column("pimp",    pim->P(),       -1.);
    ntp2->Column("pimphi",  pim->P4().Phi(),      -999.);
    ntp2->Column("pimpid0",   pim->GetPidInfo(0),   -1.);
    ntp2->Column("pimpid1",   pim->GetPidInfo(1),   -1.);
    ntp2->Column("pimpid2",   pim->GetPidInfo(2),   -1.);
    ntp2->Column("pimpid3",   pim->GetPidInfo(3),   -1.);
    ntp2->Column("pimpid4",   pim->GetPidInfo(4),   -1.);

    ntp2->DumpData();

  }

  // do mass constraint fit
  for (j=0; j<jpsi.GetLength(); ++j) {
    PndKinFitter mfitter(jpsi[j]);
    mfitter.AddMassConstraint(3.0965);
    mfitter.Fit();
    double chi2_m = mfitter.GetChi2();
    hjpsi_chi2_mf->Fill(chi2_m);

    if (chi2_m<2) { hjpsim_mcf->Fill(jpsi[j]->M()); }
  }


  // *** all combinatorics again with true PID
  jpsi.Combine(eplus, eminus);
  for (j=0; j<jpsi.GetLength(); ++j) { hjpsim_tpid->Fill( jpsi[j]->M() ); }

  //jpsi.Select(jpsiMassSel);

  psi2s.Combine(jpsi, piplus, piminus);
  for (j=0; j<psi2s.GetLength(); ++j) { hpsim_tpid->Fill( psi2s[j]->M() ); }

  // *** and again with PidAlgoEmcBayes and loose selection
  theAnalysis->FillList(eplus, "ElectronLoosePlus","PidAlgoEmcBayes");
  theAnalysis->FillList(eminus, "ElectronLooseMinus","PidAlgoEmcBayes");
  theAnalysis->FillList(piplus, "PionLoosePlus","PidAlgoEmcBayes");
  theAnalysis->FillList(piminus, "PionLooseMinus","PidAlgoEmcBayes");

  jpsi.Combine(eplus, eminus);
  for (j=0; j<jpsi.GetLength(); ++j) { hjpsim_lpid->Fill( jpsi[j]->M() ); }

  //jpsi.Select(jpsiMassSel);
  //SelectMass(jpsi,3.096,1.0);

  psi2s.Combine(jpsi, piplus, piminus);
  for (j=0; j<psi2s.GetLength(); ++j) { hpsim_lpid->Fill( psi2s[j]->M() ); }

}
// -------------------------------------------------------------------------

void PndMyAnalysisTask::FillMassHisto(TH1F* h, RhoCandList& l)
{
  Int_t i=0;
  for (i=0; i<l.GetLength(); ++i) {
    h->Fill(l[i]->M());
  }
}

// -------------------------------------------------------------------------

int PndMyAnalysisTask::SelectPdgCode(RhoCandList& mct, RhoCandList& l)
{
  int removed = 0;
  int pdgcode=0;
  //int nmct = mct.GetLength();

  //PndMcTruthMatch mcm;

  if (l.GetLength()>0) { pdgcode = l[0]->PdgCode(); }

  for (int ii=0; ii<l.GetLength(); ++ii) {
    RhoCandidate* mccnd = l[ii]->GetMcTruth();
    if (mccnd)
      if(mccnd->PdgCode()==pdgcode)
        continue; // we skip if things are allright and remove if not
    l.Remove(l[ii]);
    removed++;
  }
  return removed;
}

// -------------------------------------------------------------------------

void PndMyAnalysisTask::Finish()
{
  hjpsim_nopid->Write();
  hpsim_nopid->Write();
  hjpsim_lpid->Write();
  hpsim_lpid->Write();
  hjpsim_tpid->Write();
  hpsim_tpid->Write();
  hjpsim_ftm->Write();
  hpsim_ftm->Write();
  hjpsim_nm->Write();
  hpsim_nm->Write();

  hpsim_diff->Write();
  hjpsim_diff->Write();

  hjpsim_vf->Write();
  hjpsim_4cf->Write();
  hjpsim_mcf->Write();

  hjpsi_chi2_vf->Write();
  hpsi_chi2_4c->Write();
  hjpsi_chi2_mf->Write();

  hvpos->Write();

  ntp->GetInternalTree()->Write();
  ntp2->GetInternalTree()->Write();
  //ntp->WriteToFile("outtask_ntp.root");
  //ntp2->AddToFile("outtask_ntp.root");

//   hjpsimass->Write();
//   hpsimass->Write();

}

ClassImp(PndMyAnalysisTask)
