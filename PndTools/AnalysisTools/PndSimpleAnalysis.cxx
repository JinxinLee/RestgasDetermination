/******************************************************
Class PndSimpleAnalysis

Task to perform analysis (combinatorics, simple filetring)
by setting a configuration file

Writes out a TTree 'ntp'

Author: K.Goetzen, GSI, 06/2008

*******************************************************/

//Base headers
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

//FAIR headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

//ROOT headers
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TTree.h"

//Fastsim headers
#include "PndSimpleAnalysis.h"
#include "StrTok.h"
#include "ArgList.h"
#include "PndListDefiner.h"

//RHO headers
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoSelector/RhoMassParticleSelector.h"
#include "RhoSelector/RhoPlusParticleSelector.h"
#include "RhoSelector/RhoMinusParticleSelector.h"
#include "RhoSelector/RhoSimpleElectronSelector.h"
#include "RhoSelector/RhoSimpleKaonSelector.h"
#include "RhoSelector/RhoSimpleMuonSelector.h"
#include "RhoSelector/RhoSimplePionSelector.h"
#include "RhoSelector/RhoSimpleProtonSelector.h"
#include "RhoBase/RhoFactory.h"
//#include "RhoBase/TRho.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"


using std::cout;
using std::endl;
using std::ios;


// -----   Default constructor   -------------------------------------------
PndSimpleAnalysis::PndSimpleAnalysis() :
  FairTask("Panda Analysis Task")
{
  //fdbPdg=TDatabasePDG::Instance();
  fdbPdg = TDatabasePDG::Instance();
}

// -------------------------------------------------------------------------

PndSimpleAnalysis::PndSimpleAnalysis(std::string filename) :
  FairTask("Panda Analysis Task")
{
  //fdbPdg=TDatabasePDG::Instance();
  fdbPdg = TDatabasePDG::Instance();
  SetConfigFile(filename);
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSimpleAnalysis::~PndSimpleAnalysis() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSimpleAnalysis::Init()
{

  //cout << " Inside the Init function****" << endl;

  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndSimpleAnalysis::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  //Prepare all generic Listnames
  InitGenericLists();
  InitColumnNames();

  //ntp=new TTree("ntp","PndSimpleAnalysis NTuple");
  ntp=0;

  // set the initial 4 vector of the pbar p system
  // can be used for missing mass and mom in CMS
  // has to be set in config file for the time being
  fpInit.SetXYZT(0.,0.,0.,0.);

// Get input array
  fChargedArray = (TClonesArray*) ioman->GetObject("PidChargedCand");
  fNeutralArray = (TClonesArray*) ioman->GetObject("PidNeutralCand");

  fChargedProbability = (TClonesArray*) ioman->GetObject("PidChargedProbability");
  fNeutralProbability = (TClonesArray*) ioman->GetObject("PidNeutralProbability");

  fMcArray     = (TClonesArray*) ioman->GetObject("PndMcTracks");
  fMicroArray     = 0;//(TClonesArray*) ioman->GetObject("PndPidCandidates");

  if ( !fChargedArray && !fNeutralArray && !fMcArray && !fMicroArray) {
    cout << "-W- PndSimpleAnalysis::Init: "
         << "None of PidChargedCand, PndNeutralCand, PidMcTracks, PndPidCandidates available!" << endl;
    return kERROR;
  }

  if (!SetupAnalysis()) {
    cout << "-E- PndSimpleAnalysis::Init: "
         << "Error reading config file "<<fCfgFileName<<"." << endl;
    return kFATAL;
  }

  // Create and register output array
  cout << "-I- PndSimpleAnalysis: Intialization successfull" << endl;

  // **** create and configure the selectors/filters we'd like to use later
  //

  plusSel    = new RhoPlusParticleSelector;
  minusSel   = new RhoMinusParticleSelector;


  // **** pid selectors for generic list creation
  //
  eSel   = new RhoSimpleElectronSelector();
  muSel  = new RhoSimpleMuonSelector();
  piSel  = new RhoSimplePionSelector();
  kSel   = new RhoSimpleKaonSelector();
  pSel   = new RhoSimpleProtonSelector();


  evcount=0;

  //fMaxEntries = 200;

  return kSUCCESS;

}
// -------------------------------------------------------------------------

void PndSimpleAnalysis::InitGenericLists()
{

  //very Loose Lists
  fGenericListNames.push_back("ElectronVeryLooseP");
  fGenericListNames.push_back("ElectronVeryLooseM");
  fGenericListNames.push_back("MuonVeryLooseP");
  fGenericListNames.push_back("MuonVeryLooseM");
  fGenericListNames.push_back("PionVeryLooseP");
  fGenericListNames.push_back("PionVeryLooseM");
  fGenericListNames.push_back("KaonVeryLooseP");
  fGenericListNames.push_back("KaonVeryLooseM");
  fGenericListNames.push_back("ProtonVeryLooseP");
  fGenericListNames.push_back("ProtonVeryLooseM");

  //Loose Lists
  fGenericListNames.push_back("ElectronLooseP");
  fGenericListNames.push_back("ElectronLooseM");
  fGenericListNames.push_back("MuonLooseP");
  fGenericListNames.push_back("MuonLooseM");
  fGenericListNames.push_back("PionLooseP");
  fGenericListNames.push_back("PionLooseM");
  fGenericListNames.push_back("KaonLooseP");
  fGenericListNames.push_back("KaonLooseM");
  fGenericListNames.push_back("ProtonLooseP");
  fGenericListNames.push_back("ProtonLooseM");

  //tight Lists
  fGenericListNames.push_back("ElectronTightP");
  fGenericListNames.push_back("ElectronTightM");
  fGenericListNames.push_back("MuonTightP");
  fGenericListNames.push_back("MuonTightM");
  fGenericListNames.push_back("PionTightP");
  fGenericListNames.push_back("PionTightM");
  fGenericListNames.push_back("KaonTightP");
  fGenericListNames.push_back("KaonTightM");
  fGenericListNames.push_back("ProtonTightP");
  fGenericListNames.push_back("ProtonTightM");

  //very tight Lists
  fGenericListNames.push_back("ElectronVeryTightP");
  fGenericListNames.push_back("ElectronVeryTightM");
  fGenericListNames.push_back("MuonVeryTightP");
  fGenericListNames.push_back("MuonVeryTightM");
  fGenericListNames.push_back("PionVeryTightP");
  fGenericListNames.push_back("PionVeryTightM");
  fGenericListNames.push_back("KaonVeryTightP");
  fGenericListNames.push_back("KaonVeryTightM");
  fGenericListNames.push_back("ProtonVeryTightP");
  fGenericListNames.push_back("ProtonVeryTightM");


  // the neutral List
  fGenericListNames.push_back("Neutral");
  fGenericListNames.push_back("McTruth");

  unsigned int i=0;

  for (i=0; i<fGenericListNames.size(); i++) {
    fListMap[fGenericListNames[i]]=i;
  }

  int pcodes[10] = {-11,11,-13,13,211,-211,321,-321,2212,-2212};

  for (i=0; i<40; i++) {
    PndListDefiner* ldef=new PndListDefiner;
    ldef->fName = fGenericListNames[i];
    ldef->fPdgCode = pcodes[i%10];
    ldef->fCharge = (float)(1-(i%2*2));
    ldef->fIsAntiList = (i%2)?true:false;
    ldef->fAntiIdx = (i%2) ? i-1 : i+1;
    ldef->fIsGeneric = true;
    fListDefiners.push_back(ldef);
  }

  //  the neutral list
  PndListDefiner* ldef=new PndListDefiner;
  ldef->fName = fGenericListNames[40];
  ldef->fPdgCode = 22;
  ldef->fCharge = 0.0;
  ldef->fIsAntiList = false;
  ldef->fAntiIdx = 40;
  ldef->fIsGeneric = true;
  fListDefiners.push_back(ldef);

  // mc truth list
  ldef=new PndListDefiner;
  ldef->fName = fGenericListNames[41];
  ldef->fIsAntiList = false;
  ldef->fAntiIdx = 41;
  ldef->fIsGeneric = false;
  fListDefiners.push_back(ldef);

  //for (i=0;i<fListDefiners.size();i++) {cout << i <<" : "; fListDefiners[i]->Print();}
}
// -------------------------------------------------------------------------

void PndSimpleAnalysis::InitColumnNames()
{
  // mapping of column name and index

  fColKeyMap.clear();
  fColShortKeyMap.clear();

  // indices <500 are floats
  fColKeyMap["px"]    = 100;  // px
  fColKeyMap["py"]    = 101;  // py
  fColKeyMap["pz"]    = 102;  // pz
  fColKeyMap["en"]    = 103;  // energy of 4 vector
  fColKeyMap["m"]     = 104;  // mass
  fColKeyMap["ch"]    = 105;  // charge
  fColKeyMap["miss"]  = 113;  // missing mass

  fColKeyMap["vx"]    = 106;  // vertex pos x
  fColKeyMap["vy"]    = 107;  // vertex pos y
  fColKeyMap["vz"]    = 108;  // vertex pos z

  fColKeyMap["phi"]   = 109;  // phi of 3 vector
  fColKeyMap["tht"]   = 110;  // theta
  fColKeyMap["cth"]   = 111;  // cos(theta)
  fColKeyMap["p"]   = 112;  // absolute value of momentum

  fColKeyMap["pxcm"]  = 120;  // px in cms
  fColKeyMap["pycm"]  = 121;  // py in cms
  fColKeyMap["pzcm"]  = 122;  // pz in cms
  fColKeyMap["encm"]  = 123;  // energy in cms

  fColKeyMap["phicm"] = 124;  // phi of 3 vector in cms
  fColKeyMap["thtcm"] = 125;  // theta in cms
  fColKeyMap["cthcm"] = 126;  // cos(theta) in cms
  fColKeyMap["pcm"]   = 127;  // absolute value of momentum in cms

  fColKeyMap["dm"]  = 149;
  fColKeyMap["d1m"] = 150;  // directly store daughter masses
  fColKeyMap["d2m"] = 151;  // directly store daughter masses
  fColKeyMap["d3m"] = 152;  // directly store daughter masses
  fColKeyMap["d4m"] = 153;  // directly store daughter masses
  fColKeyMap["d5m"] = 154;  // directly store daughter masses

  fColKeyMap["tcb"] = 160;  // PID: Barrel DIRC theta_C
  fColKeyMap["tcd"] = 161;  // PID: Disc DIRC theta_C
  fColKeyMap["tcr"] = 162;  // PID: RICH theta_C
  fColKeyMap["tm2"] = 163;  // PID: TOF m^2
  fColKeyMap["dem"] = 164;  // PID: MVD dE/dx
  fColKeyMap["des"] = 165;  // PID: STT dE/dx
  fColKeyMap["det"] = 166;  // PID: TPC dE/dx

  fColKeyMap["elh"] = 170;  // PID: Electron Likelihood
  fColKeyMap["mulh"]  = 171;  // PID: Muon Likelihood
  fColKeyMap["pilh"]  = 172;  // PID: Pion Likelihood
  fColKeyMap["klh"] = 173;  // PID: Kaon Likelihood
  fColKeyMap["plh"] = 174;  // PID: Proton Likelihood

  // indeces >500 are ints
  fColKeyMap["d1"]    = 501;  // index in daughter list 1
  fColKeyMap["d2"]    = 502;  // index in daughter list 2
  fColKeyMap["d3"]    = 503;  // index in daughter list 3
  fColKeyMap["d4"]    = 504;  // index in daughter list 4
  fColKeyMap["d5"]    = 505;  // index in daughter list 5

  fColKeyMap["mci"] = 510;  // index in MC truth list
  fColKeyMap["sel"] = 515;  // PID:selector level accept (1=veryLoose, 2=loose, 3= tight, 4=veryTight)

  fColKeyMap["pdg"] = 520;  // PDG code
  fColKeyMap["mothi"] = 521;  // index of mother particle MCTruth


  // indeces >1000 map to lists of col names

  std::vector<std::string> shortcut;

  fColKeyMap["p4"]  = 1000;
  shortcut.clear();
  shortcut.push_back("px");
  shortcut.push_back("py");
  shortcut.push_back("pz");
  shortcut.push_back("en");
  fColShortKeyMap[1000]=shortcut;

  fColKeyMap["p4cm"]  = 1001;
  shortcut.clear();
  shortcut.push_back("pxcm");
  shortcut.push_back("pycm");
  shortcut.push_back("pzcm");
  shortcut.push_back("encm");
  fColShortKeyMap[1001]=shortcut;

  fColKeyMap["p3"]  = 1002;
  shortcut.clear();
  shortcut.push_back("phi");
  shortcut.push_back("tht");
  shortcut.push_back("cth");
  shortcut.push_back("p");
  fColShortKeyMap[1002]=shortcut;

  fColKeyMap["p3cm"]  = 1003;
  shortcut.clear();
  shortcut.push_back("phicm");
  shortcut.push_back("thtcm");
  shortcut.push_back("cthcm");
  shortcut.push_back("pcm");
  fColShortKeyMap[1003]=shortcut;

  fColKeyMap["pos"] = 1004;
  shortcut.clear();
  shortcut.push_back("vx");
  shortcut.push_back("vy");
  shortcut.push_back("vz");
  fColShortKeyMap[1004]=shortcut;

  fColKeyMap["pid"] = 1005;
  shortcut.clear();
  shortcut.push_back("sel");
  shortcut.push_back("des");
  shortcut.push_back("dem");
  shortcut.push_back("det");
  shortcut.push_back("tcb");
  shortcut.push_back("tcd");
  shortcut.push_back("tcr");
  shortcut.push_back("tm2");
  fColShortKeyMap[1005]=shortcut;

  fColKeyMap["pidlh"] = 1006;
  shortcut.clear();
  shortcut.push_back("elh");
  shortcut.push_back("mulh");
  shortcut.push_back("pilh");
  shortcut.push_back("klh");
  shortcut.push_back("plh");
  fColShortKeyMap[1006]=shortcut;
}

// -------------------------------------------------------------------------

void PndSimpleAnalysis::SetParContainers()
{

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) { Fatal("SetParContainers", "No analysis run"); }

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");


}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSimpleAnalysis::Exec(Option_t* opt)
{
  // set the boost vector
  TVector3 pInitBoost(0,0,0);
  if (fpInit.Z()!=0.0) { pInitBoost=-fpInit.BoostVector(); }

  unsigned int i=0;
  int j=0,k=0,k2=0;

  if (!(++evcount%100)) {
    cout <<"evt "<<evcount<<endl;
  }

  FillGenericLists();

  bool dumpAList=false;

  // cache for boosted 4-vectors

  std::vector<TLorentzVector> lvcache;

  for (i=0; i<fListDefiners.size(); i++) {
    PndListDefiner* cur=fListDefiners[i];

    if (!cur->fIsUsed) { continue; }

    int nd=cur->GetNDau();

    PndListDefiner* ld1,*ld2,*ld3,*ld4,*ld5;

    // *********************
    // do combinatorics and fill list
    // *********************
    switch (nd) {
    case 2:
      ld1=fListDefiners[cur->fDauIdx[0]];
      ld2=fListDefiners[cur->fDauIdx[1]];

      cur->fList.Combine(ld1->fList,ld2->fList);

      break;  // case 2

    case 3:
      ld1=fListDefiners[cur->fDauIdx[0]];
      ld2=fListDefiners[cur->fDauIdx[1]];
      ld3=fListDefiners[cur->fDauIdx[2]];

      cur->fList.Combine(ld1->fList, ld2->fList, ld3->fList);
      break;  // case 3

    case 4:
      ld1=fListDefiners[cur->fDauIdx[0]];
      ld2=fListDefiners[cur->fDauIdx[1]];
      ld3=fListDefiners[cur->fDauIdx[2]];
      ld4=fListDefiners[cur->fDauIdx[3]];

      cur->fList.Combine(ld1->fList, ld2->fList, ld3->fList, ld4->fList);
      break; //case 4

    case 5:
      ld1=fListDefiners[cur->fDauIdx[0]];
      ld2=fListDefiners[cur->fDauIdx[1]];
      ld3=fListDefiners[cur->fDauIdx[2]];
      ld4=fListDefiners[cur->fDauIdx[3]];
      ld5=fListDefiners[cur->fDauIdx[4]];

      cur->fList.Combine(ld1->fList, ld2->fList, ld3->fList, ld4->fList, ld5->fList);
      break; //case 5

    } //switch

    // *********************
    // select candidates
    // *********************
    for (j=0; j<cur->GetNSels(); j++) {
      if (0!=cur->fSelector[j]) {
        cur->fList.Select(cur->fSelector[j]);
      }
    }

    // *********************
    // fill histograms
    // *********************
    if (cur->fHisto.size()>0) {
      for (j=0; j<cur->GetLength(); j++) { cur->fHisto[0]->Fill(cur->fList[j].M()); }
    }

    // *********************
    // fill ntuple
    // *********************
    if (cur->fDumpList) {
      dumpAList|=cur->fDumpList;

      PndListDefiner* curdump=cur;

      // clear the cache of boosted lorentz vectors
      lvcache.clear();

      // if AntiList, then append information to arrays of List
      int off=0;
      if (cur->fIsAntiList) {
        // this is the pointer to the corresponding LIST!
        curdump=fListDefiners[cur->fAntiIdx];
        off=curdump->GetLength();
      }

      // set the number of entries; if list has antilist, take sum of both numbers
      curdump->fNEntries=off+cur->GetLength();

      //  Dump out the float columns
      for (j=0; j<(int)curdump->fNtpFNames.size(); j++) {
        //hmm, unknown name; should not be!
        if (fColKeyMap.find(curdump->fNtpFNames[j])==fColKeyMap.end()) { continue; }

        int key=fColKeyMap[curdump->fNtpFNames[j]];

        bool cachefilled=(lvcache.size()>0);

        for (k=0; k<cur->GetLength(); k++) {
          if ( (off+k)>=fMaxEntries ) { continue; }
          float* theArF=curdump->fNtpFArrays[j];

          PndPidCandidate* mic=(PndPidCandidate*)(&(cur->fList[k].GetRecoCandidate()));

          // set default
          theArF[off+k]=0.0;

          switch (key) {
          case 100:
            theArF[off+k]=cur->fList[k].Px();
            break;
          case 101:
            theArF[off+k]=cur->fList[k].Py();
            break;
          case 102:
            theArF[off+k]=cur->fList[k].Pz();
            break;
          case 103:
            theArF[off+k]=cur->fList[k].E();
            break;

          case 104:
            theArF[off+k]=cur->fList[k].M();
            break;
          case 105:
            theArF[off+k]=cur->fList[k].Charge();
            break;

          case 106:
            theArF[off+k]=cur->fList[k].Pos().X();
            break;
          case 107:
            theArF[off+k]=cur->fList[k].Pos().Y();
            break;
          case 108:
            theArF[off+k]=cur->fList[k].Pos().Z();
            break;

          case 109:
            theArF[off+k]=cur->fList[k].GetVect().Phi();
            break;
          case 110:
            theArF[off+k]=cur->fList[k].GetVect().Theta();
            break;
          case 111:
            theArF[off+k]=cur->fList[k].GetVect().CosTheta();
            break;
          case 112:
            theArF[off+k]=cur->fList[k].GetVect().Mag();
            break;
          case 113:
            theArF[off+k]=( fpInit - (cur->fList[k].P4()) ).M();
            break;

          case 120:  //  Px in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Px();
            break;

          case 121: // Py in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Py();
            break;

          case 122: // Pz in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Pz();
            break;

          case 123: // E in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].E();
            break;

          case 124: // phi in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Vect().Phi();
            break;

          case 125: // theta in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Vect().Theta();
            break;

          case 126: // cos theta in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Vect().CosTheta();
            break;

          case 127: // |p| in CMS
            if (!cachefilled) {
              TLorentzVector lv=cur->fList[k].P4();
              lv.Boost(pInitBoost);
              lvcache.push_back(lv);
            }
            theArF[off+k]=lvcache[k].Vect().Mag();
            break;

          case 150:
            theArF[off+k]=cur->fList[k].Daughter(0)->M();
            break;
          case 151:
            theArF[off+k]=cur->fList[k].Daughter(1)->M();
            break;
          case 152:
            theArF[off+k]=cur->fList[k].Daughter(2)->M();
            break;
          case 153:
            theArF[off+k]=cur->fList[k].Daughter(3)->M();
            break;
          case 154:
            theArF[off+k]=cur->fList[k].Daughter(4)->M();
            break;

            // PID Info based on PndPidCandidate entries
          case 160:
            if (mic) { theArF[off+k]=mic->GetDrcThetaC(); }
            break;  // Barrel DIRC tht_c
          case 161:
            if (mic) { theArF[off+k]=mic->GetDiscThetaC(); }
            break;   // Disc DIRC tht_c
          case 162:
            if (mic) { theArF[off+k]=mic->GetRichThetaC(); }
            break;     // RICH tht_c
          case 163:
            if (mic) { theArF[off+k]=mic->GetTofM2(); }
            break;      // TOF m^2
          case 164:
            if (mic) { theArF[off+k]=mic->GetMvdDEDX(); }
            break;    // MVD dEdx
          case 165:
            if (mic) { theArF[off+k]=mic->GetSttMeanDEDX(); }
            break;    // STT dEdx

          case 170:
            if (mic) { theArF[off+k]=mic->GetElectronPidLH(); }
            break;  // Electron LH
          case 171:
            if (mic) { theArF[off+k]=mic->GetMuonPidLH(); }
            break;    // Muon LH
          case 172:
            if (mic) { theArF[off+k]=mic->GetPionPidLH(); }
            break;    // Pion LH
          case 173:
            if (mic) { theArF[off+k]=mic->GetKaonPidLH(); }
            break;    // Kaon LH
          case 174:
            if (mic) { theArF[off+k]=mic->GetProtonPidLH(); }
            break;    // Proton LH

            // PID Info based in RhoCandidate entries... obsolete
            /*
            case 160: theArF[off+k]=cur->fList[k].GetPidInfo(5); break; // Barrel DIRC tht_c
            case 161: theArF[off+k]=cur->fList[k].GetPidInfo(6); break; // Disc DIRC tht_c
            case 162: theArF[off+k]=cur->fList[k].GetPidInfo(7); break; // RICH tht_c
            case 163: theArF[off+k]=cur->fList[k].GetPidInfo(8); break; // TOF mπ
            case 164: theArF[off+k]=cur->fList[k].GetPidInfo(9); break; // MVD dEdx
            case 165: theArF[off+k]=cur->fList[k].GetPidInfo(10); break; // STT dEdx
            case 166: theArF[off+k]=cur->fList[k].GetPidInfo(11); break; // TPC dEdx

            case 170: theArF[off+k]=cur->fList[k].GetPidInfo(0); break; // Electron LH
            case 171: theArF[off+k]=cur->fList[k].GetPidInfo(1); break; // Muon LH
            case 172: theArF[off+k]=cur->fList[k].GetPidInfo(2); break; // Pion LH
            case 173: theArF[off+k]=cur->fList[k].GetPidInfo(3); break; // Kaon LH
            case 174: theArF[off+k]=cur->fList[k].GetPidInfo(4); break; // Proton LH
            */
          }

        }
      }


      //  Dump out the int columns
      for (j=0; j<(int)curdump->fNtpINames.size(); j++) {
        //hmm, unknown name; should not be!
        if (fColKeyMap.find(curdump->fNtpINames[j])==fColKeyMap.end()) { continue; }

        int key=fColKeyMap[curdump->fNtpINames[j]];

        for (k=0; k<cur->GetLength(); k++) {
          if ( (off+k)>=fMaxEntries ) { continue; }
          int* theArI=curdump->fNtpIArrays[j];

          //set default
          theArI[k+off]=0;

          switch (key) {
            // make the daughter index match
          case 501:
          case 502:
          case 503:
          case 504:
          case 505: {
            int dauindex=-1;
            int dauoff=0;
            int dnum=key-501;  //number of the daughter to find index of

            // the  dnum-th daughter of our particle
            RhoCandidate* dauC=cur->fList[k].Daughter(dnum);
            if (0==dauC) { break; }

            int lidx=cur->fDauIdx[dnum];
            PndListDefiner* dld=fListDefiners[lidx];
            if (dld->fIsAntiList) {
              dauoff=fListDefiners[dld->fAntiIdx]->fList.GetLength();
            }

            for (k2=0; k2<dld->fList.GetLength(); k2++)
              if (dauC->IsCloneOf(dld->fList[k2])) {
                dauindex=k2+dauoff;
                k2=1000;
              }
            theArI[k+off]=dauindex;
          }
          break;

          case 510:
            theArI[k+off]=cur->fList[k].GetMcIdx();
            break;

          case 515: // determine the best pid level
            theArI[k+off]=0;
            if (cur->fIsGeneric) {
              int pidl=0;
              RhoParticleSelectorBase* theSel=0;

              switch (abs(cur->fPdgCode)) {
              case 11:
                theSel=eSel;
                break;
              case 13:
                theSel=muSel;
                break;
              case 211:
                theSel=piSel;
                break;
              case 321:
                theSel=kSel;
                break;
              case 2212:
                theSel=pSel;
                break;
              }
              if (theSel) {
                pidl=1;         //list at least veryLoose
                if (i>=30) { pidl=4; }    //list is already veryTight
                else if (i>=20) { pidl=3; } //list is already tight
                else if (i>=10) { pidl=2; } //list is already loose

                if (pidl<4) {
                  theSel->SetCriterion(veryTight);
                  if (theSel->Accept(cur->fList[k])) { pidl=4; }
                }
                if (pidl<3) {
                  theSel->SetCriterion(tight);
                  if (theSel->Accept(cur->fList[k])) { pidl=3; }
                }
                if (pidl<2) {
                  theSel->SetCriterion(loose);
                  if (theSel->Accept(cur->fList[k])) { pidl=2; }
                }
              }
              theArI[k+off]=pidl;
            }
            break;

          case 520:
            theArI[k+off]=cur->fList[k].PdgCode();
            break;

            //case 521: theArI[k+off]=cur->fList[k].MotherIdx(); break;

          }

        }
      }
    }

  }

  if (dumpAList) { ntp->Fill(); }

//  for (i=0;i<fListDefiners.size();i++)
//    if (fListDefiners[i]->fIsUsed) {cout <<i<<" : "; fListDefiners[i]->Print(); }

}

// -------------------------------------------------------------------------

void PndSimpleAnalysis::PrintTree(RhoCandidate* tc, int level)
{
  int i=0;
  int nd=tc->NDaughters();
  if (nd==0) { return; }
  cout <<tc->Uid()<<"("<<tc->GetPidInfo(29)<<") -> ";
  for (i=0; i<nd; i++) { cout<<tc->Daughter(i)->Uid()<<"("<<tc->Daughter(i)->GetPidInfo(29)<<")  "; }
  cout <<endl;
  for (i=0; i<nd; i++) { PrintTree(tc->Daughter(i),level+1); }
  if (level==0) { cout <<endl; }
}

// -------------------------------------------------------------------------

void PndSimpleAnalysis::SetConfigFile(std::string filename)
{
  fCfgFileName=filename;
}

// -------------------------------------------------------------------------

bool PndSimpleAnalysis::IsGenericListName(std::string n)
{
  if (n=="Charged" || n=="Neutral") { return true; }

  if (n=="ElectronVeryLoose" || n=="MuonVeryLoose" || n=="PionVeryLoose"
      || n=="KaonVeryLoose" || n=="ProtonVeryLoose") { return true; }

  if (n=="ElectronLoose" || n=="MuonLoose" || n=="PionLoose"
      || n=="KaonLoose" || n=="ProtonLoose") { return true; }

  if (n=="ElectronTight" || n=="MuonTight" || n=="PionTight"
      || n=="KaonTight" || n=="ProtonTight") { return true; }

  if (n=="ElectronVeryTight" || n=="MuonVeryTight" || n=="PionVeryTight"
      || n=="KaonVeryTight" || n=="ProtonVeryTight") { return true; }
  else { return false; }
}

// -------------------------------------------------------------------------

void PndSimpleAnalysis::FillGenericLists()
{
  RhoFactory::Instance()->Reset();

  int i, uid=1;

  // **** loop over all Candidates and add them to the list allCands
  //
  chargedCands.Cleanup();
  neutralCands.Cleanup();
  mcCands.Cleanup();

  // when we have a PndPidCandidates Array, take that one
  // ********** DEPRECATED ***************
  /*
  if (fMicroArray)
  {
    for (i=0; i<fMicroArray->GetEntriesFast(); i++)
    {
      PndPidCandidate *mic = (PndPidCandidate *)fMicroArray->At(i);
            RhoCandidate tc(*mic,i);

      if (fabs(tc.Charge())>0.01)
        chargedCands.Add(tc);
      else
        neutralCands.Add(tc);
    }

  }
  else  //otherwise look for the older RhoCandidate arrays
  */
  // read the charged candidates
  if (fChargedArray)
    for (i=0; i<fChargedArray->GetEntriesFast(); i++) {
      FairRecoCandidate* mic = (FairRecoCandidate*)fChargedArray->At(i);
      RhoCandidate tcc(*mic,uid++);

      // are pid data available?
      if (fChargedProbability)
        if (i<fChargedProbability->GetEntriesFast()) {
          PndPidProbability* chProb = (PndPidProbability*)fChargedProbability->At(i);
          // numbering see PndPidListMaker
          tcc.SetPidInfo(0,chProb->GetElectronPidProb());
          tcc.SetPidInfo(1,chProb->GetMuonPidProb());
          tcc.SetPidInfo(2,chProb->GetPionPidProb());
          tcc.SetPidInfo(3,chProb->GetKaonPidProb());
          tcc.SetPidInfo(4,chProb->GetProtonPidProb());
        }
      chargedCands.Add(tcc);
    }

  // read the neutral candidates
  if (fNeutralArray)
    for (i=0; i<fNeutralArray->GetEntriesFast(); i++) {
      FairRecoCandidate* mic = (FairRecoCandidate*)fNeutralArray->At(i);
      RhoCandidate tcn(*mic,uid++);

      // are pid data available?
      if (fNeutralProbability)
        if (i<fNeutralProbability->GetEntriesFast()) {
          PndPidProbability* neuProb = (PndPidProbability*)fNeutralProbability->At(i);
          // numbering see PndPidListMaker
          tcn.SetPidInfo(0,neuProb->GetElectronPidProb());
          tcn.SetPidInfo(1,neuProb->GetMuonPidProb());
          tcn.SetPidInfo(2,neuProb->GetPionPidProb());
          tcn.SetPidInfo(3,neuProb->GetKaonPidProb());
          tcn.SetPidInfo(4,neuProb->GetProtonPidProb());
        }
      neutralCands.Add(tcn);
    }

  // read the mc truth list
  if (fMcArray)
    for (i=0; i<fMcArray->GetEntriesFast(); i++) {
      RhoCandidate* tc = (RhoCandidate*)fMcArray->At(i);
      mcCands.Add(*tc);
    }

  for (i=0; i<40; i++) {
    if (!fListDefiners[i]->fIsUsed) { continue; }
    int crithint=i/10;
    int pidhint=(i%10)/2;
    //VAbsPidSelector *chSel;
    RhoParticleSelectorBase* pidSel=0;

    if (i%2) {
      fListDefiners[i]->fList.Select(chargedCands,minusSel);
    } else {
      fListDefiners[i]->fList.Select(chargedCands,plusSel);
    }

    switch (pidhint) {
    case 0 :
      pidSel = eSel;
      break;
    case 1 :
      pidSel = muSel;
      break;
    case 2 :
      pidSel = piSel;
      break;
    case 3 :
      pidSel = kSel;
      break;
    case 4 :
      pidSel = pSel;
      break;
    }

    switch (crithint) {
    case 0 :
      pidSel->SetCriterion(veryLoose);
      break;
    case 1 :
      pidSel->SetCriterion(loose);
      break;
    case 2 :
      pidSel->SetCriterion(tight);
      break;
    case 3 :
      pidSel->SetCriterion(veryTight);
      break;
    }

    fListDefiners[i]->fList.Select(pidSel);
  }

  fListDefiners[40]->fList=neutralCands;
  fListDefiners[41]->fList=mcCands;


  return;
}

// -------------------------------------------------------------------------

bool PndSimpleAnalysis::SetupAnalysis()
{
  cout<<"setupAnalysis"<<endl;
  ifstream cfgFile(fCfgFileName.c_str(),ios::in);

  int nLists=fListDefiners.size();

  unsigned int i=0,j=0;

  std::vector<int> daupdgs;
  unsigned int daucnt=0;
  unsigned int daulistcnt=0;

  char line[200];
  unsigned int linecnt=0;

  //bool inList=false;
  bool hasAnti=false;

  bool defineOpen=false;
  bool decaySet=false;
  bool dauListsSet=false;

  PndListDefiner* currentList;
  PndListDefiner* currentAntiList;

  // **************************************************
  // the big loop through the .cfg file
  // **************************************************
  while (!cfgFile.eof()) {
    cfgFile.getline(line,200);
    linecnt++;

    ArgVector tokenVec;
    CStrTok tokenizer;

    char* token = tokenizer.GetFirst(line, " \t");

    while(token) {
      tokenVec.push_back(token);
      token=tokenizer.GetNext(" \t");
    }

    if (tokenVec.size()==0) { continue; }


    // **************************************************
    // ***** BEGIN LIST DEFINITION **** Token = DefineList
    // **************************************************

    if (tokenVec[0]=="DefineList") {
      // are we already in a DefineList?
      if (defineOpen) { return ErrorMessage(100,linecnt); }

      // list already defined?
      if (fListMap.find(tokenVec[1])!=fListMap.end()) { return ErrorMessage(101,linecnt,tokenVec[1]); }

      currentList=new PndListDefiner(tokenVec[1]);
      currentList->fIsUsed=true;

      fListMap[tokenVec[1]]=nLists++;

      defineOpen=true;
      decaySet=false;
      dauListsSet=false;
      daupdgs.clear();
      daucnt=0;
      daulistcnt=0;

    } // ******** DefineList *********** END



    // **************************************************
    // ***** READ IN THE DECAY MODE **** Token = decayMode
    // **************************************************

    if (tokenVec[0]=="decayMode") {
      // are we in a DefineList statement?
      if (!defineOpen) { return ErrorMessage(200,linecnt); }

      // decaymode already defined?
      if (decaySet) { return ErrorMessage(201,linecnt); }

      currentList->fPdgCode=GetPdgCode(tokenVec[1]);

      for (i=3; i<tokenVec.size(); i++) {
        daupdgs.push_back(GetPdgCode(tokenVec[i]));
      }

      daucnt=daupdgs.size();
      decaySet=true;

      if (daucnt>5) { return ErrorMessage(203,linecnt); }

    } // ******* decayMode *********** END



    // **************************************************
    // ***** SET DAUGHTER LISTS **** Token = daughterList
    // **************************************************

    if (tokenVec[0]=="daughterList") {
      // are we in a DefineList statement?
      if (!defineOpen) { return ErrorMessage(200,linecnt); }

      // decaymode already defined?
      if (!decaySet) { return ErrorMessage(301,linecnt); }

      // more daughterLists than daughters in decaymode definition?
      if (daulistcnt>daucnt) { return ErrorMessage(302,linecnt); }

      // cannot use McTruth list for combinatorics
      if (tokenVec[1]=="McTruth") { return ErrorMessage(305,linecnt); }

      std::string dName=tokenVec[1];
      bool isgeneric=false;

      // the generic charged lists have a 'P'=plus or 'M'=minus as last letter
      // we have to modifiy accordingly
      if (IsGenericListName(dName)) {
        isgeneric=true;

        // replace 'Charged' with the corresponding particle list
        if (dName=="Charged")
          switch (abs(daupdgs[daulistcnt])) {
          case   11:
            dName="ElectronVeryLoose";
            break;
          case   13:
            dName="MuonVeryLoose";
            break;
          case  211:
            dName="PionVeryLoose";
            break;
          case  321:
            dName="KaonVeryLoose";
            break;
          case 2212:
            dName="ProtonVeryLoose";
            break;
          }

        float charge=fdbPdg->GetParticle(daupdgs[daulistcnt])->Charge();
        if (charge<-0.1) { dName+="M"; }
        else if (charge>0.1) { dName+="P"; }
        currentList->fDauIdx.push_back(fListMap[dName]);
        fListDefiners[fListMap[dName]]->fIsUsed=true;
      }

      // here we have to check whether the list or corresponding anti list is meant
      if (!isgeneric) {
        // does list exist?
        if (fListMap.find(dName)==fListMap.end()) { return ErrorMessage(303,linecnt,dName); }

        int idx=fListMap[dName];

        int decpdg=daupdgs[daulistcnt];
        int listpdg=fListDefiners[idx]->fPdgCode;
        int alistpdg=fListDefiners[fListDefiners[idx]->fAntiIdx]->fPdgCode;

        // do particle types match between decayMode and daughterList definition?
        if (decpdg!=listpdg && decpdg!=alistpdg) { return ErrorMessage(304,linecnt); }

        // decide whether the list or the charged conjugate has to be used
        // be aware, that for pdg==0 (unspecified particles) always the list is used;
        // I don't know how to decide better at the moment
        if (decpdg==listpdg) {
          currentList->fDauIdx.push_back(idx);
        } else if (decpdg==alistpdg) {
          currentList->fDauIdx.push_back(fListDefiners[idx]->fAntiIdx);
        }
      }

      daulistcnt++;
    } // ******* daughterList *********** END



    // **************************************************
    // ***** END LIST DEFINITION **** Token = End
    // **************************************************

    if (tokenVec[0]=="End" || tokenVec[0]=="EndDefine") {
      // are we in a DefineList statement?
      if (!defineOpen) { return ErrorMessage(200,linecnt); }
      // same number of daughterLists as daughters in decaymode definition?
      if (!decaySet || daucnt<2 || daulistcnt!=daucnt) { return ErrorMessage(400,linecnt); }

      // everything seems to be ok, take care whether antilist is needed or not
      // therefore check whether the final state can be distinguished from it's c.c.
      // i.e. the set of list indices from the list of the anti-indices

      //sort the daughter index list
      //std::sort(currentList->fDauIdx.begin(),currentList->fDauIdx.end());

      fListDefiners.push_back(currentList);

      std::vector<int> finstate;
      std::vector<int> ccstate;

      double charge=0.0;

      for (i=0; i<daulistcnt; i++) {
        int idx   = currentList->fDauIdx[i];
        int ccidx = fListDefiners[idx]->fAntiIdx;

        finstate.push_back(idx);
        ccstate.push_back(ccidx);
        charge+=fListDefiners[idx]->fCharge;
      }

      currentList->fCharge=charge;

      // to find out whether the final state and the cc final state are
      // different sort the lists of indices and compare pair-wise
      std::sort(finstate.begin(),finstate.end());
      std::sort(ccstate.begin(),ccstate.end());

      hasAnti=false;

      cout <<currentList->fName<<" : list indices of list and cc : ";
      for (i=0; i<finstate.size(); i++) {
        cout <<"( "<<finstate[i]<<" , "<<ccstate[i]<<" ) ";
        if (finstate[i]!=ccstate[i]) { hasAnti=true; }
      }
      cout <<endl;

      if (!hasAnti) { // set the idx of the antilist to own idx
        currentList->fAntiIdx=fListMap[currentList->fName];
      } else {
        // create the c.c. list
        std::string aName=currentList->fName+"_A";
        currentAntiList=new PndListDefiner(aName);
        fListMap[aName]=nLists++;

        // set the idx for both lists to each other
        currentList->fAntiIdx=fListMap[aName];
        currentAntiList->fAntiIdx=fListMap[currentList->fName];

        charge=0.;
        // set the daughterlist idx
        for (i=0; i<ccstate.size(); i++) {
          int idx   = currentList->fDauIdx[i];
          int ccidx = fListDefiners[idx]->fAntiIdx;

          currentAntiList->fDauIdx.push_back(ccidx);

          //also sum charges of daughters of anti list
          charge+=fListDefiners[ccstate[i]]->fCharge;

          // all the lists used as input for antilist have to be filled as well
          fListDefiners[ccstate[i]]->fIsUsed=true;
        }

        // set the properties of Anti-List according to list properties
        currentAntiList->fPdgCode=GetAntiPdgCode(currentList->fPdgCode);
        currentAntiList->fCharge=charge;
        currentAntiList->fIsAntiList=true;
        currentAntiList->fIsUsed=true;
        currentAntiList->fHisto=currentList->fHisto;
        currentAntiList->fSelector=currentList->fSelector;
        currentAntiList->fDumpList=currentList->fDumpList;

        fListDefiners.push_back(currentAntiList);
      }


      defineOpen=false;

    }// ******* End *********** END


    // **************************************************
    // ***** Misc Command Definitions
    // **************************************************

    // set the initial 4 vector i.e. pbar momentum or E_cms
    if (tokenVec[0]=="pbarMom") {
      if (fpInit.E()!=0) { ErrorMessage(503,linecnt); }

      double p  = atof(tokenVec[1].c_str());
      double mp = 0.938272;
      double E  = sqrt(mp*mp+p*p)+mp;

      fpInit.SetXYZT(0.0,0.0,p,E);
    }

    if (tokenVec[0]=="Ecms") {
      if (fpInit.E()!=0) { ErrorMessage(503,linecnt); }

      double M  = atof(tokenVec[1].c_str());
      double mp = 0.938272;

      double X = (M*M-2*mp*mp)/(2*mp);
      double p = sqrt(X*X-mp*mp);
      double E  = sqrt(mp*mp+p*p)+mp;

      fpInit.SetXYZT(0.0,0.0,p,E);
    }

    // set maximum for entries in column

    /*    if (tokenVec[0]=="MaxEntries")
        {
          int num=atoi(tokenVec[1].c_str());
          if (num<=0 || num>32768)
          {
            ErrorMessage(510,linecnt);
          }
          else
            fMaxEntries=num;
        }*/



    // **************************************************
    // dump the list into nTuple
    // **************************************************

    if (tokenVec[0]=="dumpList") {
      // are we in a DefineList statement? Then we need at least table prefix
      if (defineOpen && tokenVec.size()<2) { return ErrorMessage(501,linecnt); }

      // are we outside of DefineList statement? We need in addition list name!
      if (!defineOpen && tokenVec.size()<3) { return ErrorMessage(502,linecnt); }

      // tokenVec[startcols] is the first column name to dump
      // in DefineList this is position 2; outside it is pos 3
      unsigned int startcols=2;

      std::string dName=tokenVec[1];

      if (!defineOpen) {
        // List has to be defined before
        //for Generic Names like PionVeryLoose add the "P" at the end
        if (dName=="Charged") { dName="PionVeryLoose"; }

        if (IsGenericListName(dName) && dName!="Neutral") { dName+="P"; }

        if (fListMap.find(dName)==fListMap.end()) { return ErrorMessage(303,linecnt,dName); }

        // which list is it?
        int idx=fListMap[dName];

        currentList=fListDefiners[idx];

        // we also want to dump Anti list if existing!
        fListDefiners[currentList->fAntiIdx]->fDumpList=true;
        fListDefiners[currentList->fAntiIdx]->fIsUsed=true;

        startcols=3;
      }

      currentList->fDumpList=true;
      currentList->fIsUsed=true;
      currentList->fColName=tokenVec[startcols-1];

      if (tokenVec.size()==startcols) {
        tokenVec.push_back("p4");
        tokenVec.push_back("ch");
        tokenVec.push_back("m");
      }

      for (i=startcols; i<tokenVec.size(); i++) {
        std::string col=tokenVec[i];

        if (fColKeyMap.find(col)==fColKeyMap.end()) {
          cout << "-W- PndSimpleAnalysis::SetupAnalysis: In '"<< fCfgFileName
               << "', line "<<linecnt<<": ";
          cout << "Unknown column name '"<<col<<"'."<<endl;

          continue;
        }

        int key=fColKeyMap[col];

        // 'mothi' and 'pdg' cannot be added explicit
        if (key==520 || key==521) { continue; }

        // daughter masses cannot be added explicit
        if (key>=150 && key<=154) { continue; }

        // daughter list indices cannot be added explicit
        if (key>=501 && key<=505) { continue; }

        // short cut!
        if (key>=1000) {
          std::vector<std::string> keylist=fColShortKeyMap[key];
          for (j=0; j<keylist.size(); j++) { tokenVec.push_back(keylist[j]); }

        } else if (key<500) { currentList->fNtpFNames.push_back(col); }
        else { currentList->fNtpINames.push_back(col); }

      }

      if (dName=="McTruth") {
        currentList->fNtpINames.push_back("pdg");
        currentList->fNtpINames.push_back("mothi");
      }

    } // dumpList


    // ***** WRITE OUT MASS HISTOGRAM **** Token = histogram

    if (tokenVec[0]=="histogram") {
      // are we in a DefineList statement?
      if (!defineOpen) { return ErrorMessage(200,linecnt); }

      double width=0.1;
      double mass=1.0;

      if (currentList->fPdgCode) {
        mass=fdbPdg->GetParticle(currentList->fPdgCode)->Mass();
      }

      if (tokenVec.size()==2) {
        width=atof(tokenVec[1].c_str());
      }

      double min=mass - width;
      double max=mass + width;

      if (tokenVec.size()==3) {
        double num1=atof(tokenVec[1].c_str());
        double num2=atof(tokenVec[2].c_str());
        if (num1<num2) {min=num1; max=num2;}
        else {min=num2; max=num1;}
      }

      std::string hname=currentList->fName+"_M";
      std::string htitle=currentList->fName+" mass";

      TH1F* h=new TH1F(hname.c_str(),htitle.c_str(),100,min,max);
      currentList->fHisto.push_back(h);

    } // histogram

    // ***** SELECTOR DEFINITIONS **** Token = select

    if (tokenVec[0]=="select") {
      // add a mass selection

      if (tokenVec[1]=="Mass") {
        // particle unknown (pdg==0) and only one argument given?
        if (currentList->fPdgCode==0 && tokenVec.size()<4) {
          return ErrorMessage(500,linecnt, currentList->fName);
        }

        double mean=fdbPdg->GetParticle(currentList->fPdgCode)->Mass();
        double width=0.1;

        // parameter specifies width around nominal mass
        if (tokenVec.size()==3) {
          width=atof(tokenVec[2].c_str());
        }

        if (tokenVec.size()>3) {
          mean=atof(tokenVec[2].c_str());
          width=atof(tokenVec[3].c_str());
        }
        RhoMassParticleSelector* sel=new RhoMassParticleSelector((currentList->fName+"sel").c_str(),mean,width);
        currentList->fSelector.push_back(sel);
      } //Mass

    } //selector



  } // while cfg file

  cfgFile.close();

  std::map<std::string, int> colMap;

  for (j=0; j<fListDefiners.size(); j++) {
    currentList=fListDefiners[j];

    if (!currentList->fIsAntiList && currentList->fDumpList) {
      if (0==ntp) { ntp=new TTree("ntp","PndSimpleAnalysis NTuple"); }
      std::string pre=currentList->fColName;

      int nd=currentList->GetNDau();

      // add the daughter idx branches
      if (nd>0) { currentList->fNtpINames.push_back("d1"); }
      if (nd>1) { currentList->fNtpINames.push_back("d2"); }
      if (nd>2) { currentList->fNtpINames.push_back("d3"); }
      if (nd>3) { currentList->fNtpINames.push_back("d4"); }
      if (nd>4) { currentList->fNtpINames.push_back("d5"); }

      for (i=0; i<currentList->fNtpFNames.size(); i++) {
        if (currentList->fNtpFNames[i]=="dm") {
          currentList->fNtpFNames.erase(currentList->fNtpFNames.begin()+i);

          if (nd>0) { currentList->fNtpFNames.push_back("d1m"); }
          if (nd>1) { currentList->fNtpFNames.push_back("d2m"); }
          if (nd>2) { currentList->fNtpFNames.push_back("d3m"); }
          if (nd>3) { currentList->fNtpFNames.push_back("d4m"); }
          if (nd>4) { currentList->fNtpFNames.push_back("d5m"); }

          i=1000;
        }
      }

      std::string brname="n"+pre;
      if (colMap.find(brname)!=colMap.end()) { return ErrorMessage(504,0,brname); }
      colMap[brname]=1;

      //create the branch with the number of entries
      ntp->Branch(brname.c_str(),&(currentList->fNEntries),(brname+"/I").c_str());

      //create the branches holding the values (float)
      for (i=0; i<currentList->fNtpFNames.size(); i++) {
        brname=pre+currentList->fNtpFNames[i];
        cout <<brname<<" ";
        if (colMap.find(brname)!=colMap.end()) { return ErrorMessage(504,0,brname); }
        colMap[brname]=1;

        float* theAr=new float[fMaxEntries];

        currentList->fNtpFArrays.push_back(theAr);
        ntp->Branch(brname.c_str(),currentList->fNtpFArrays[i],(brname+"[n"+pre+"]/F").c_str());
      }

      //create the branches holding the values (int, at the moment only the daughter indices)
      for (i=0; i<currentList->fNtpINames.size(); i++) {
        brname=pre+currentList->fNtpINames[i];
        cout <<brname<<" ";
        if (colMap.find(brname)!=colMap.end()) { return ErrorMessage(504,0,brname); }
        colMap[brname]=1;

        int* theAr=new int[fMaxEntries];
        currentList->fNtpIArrays.push_back(theAr);
        ntp->Branch(brname.c_str(), currentList->fNtpIArrays[i], (brname+"[n"+pre+"]/I").c_str());
      }
      cout <<endl;


    }  // dump List

  }



  for (i=0; i<fListDefiners.size(); i++)
    if (fListDefiners[i]->fIsUsed) {cout <<i<<" : "; fListDefiners[i]->Print(); }

  //if (ntp) ntp->Print();

  return true;
}


// -------------------------------------------------------------------------

void PndSimpleAnalysis::Finish()
{
  for (unsigned int i=0; i<fListDefiners.size(); i++) {
    PndListDefiner* cur=fListDefiners[i];
    if (cur->fIsAntiList) { continue; }
    for (int j=0; j<cur->GetNHistos(); j++) {
      cur->fHisto[j]->Write();
    }
  }

  if (ntp) {
    ntp->Write();
  }
}

// -------------------------------------------------------------------------

int PndSimpleAnalysis::GetPdgCode(std::string name)
{
  int code=0;
  if (fdbPdg->GetParticle(name.c_str())) {
    code=fdbPdg->GetParticle(name.c_str())->PdgCode();
  }

  //cout <<name<<" "<<code<<endl;

  return code;
}

// -------------------------------------------------------------------------

int PndSimpleAnalysis::GetAntiPdgCode(std::string name)
{
  int code=0, pdgcode=0;
  if (fdbPdg->GetParticle(name.c_str())) {
    pdgcode=fdbPdg->GetParticle(name.c_str())->PdgCode();
    if (fdbPdg->GetParticle(pdgcode)->AntiParticle()) {
      code=-pdgcode;
    } else {
      code=pdgcode;
    }
  }

  //cout <<name<<"("<<pdgcode<<") A="<<code<<endl;

  return code;
}

// -------------------------------------------------------------------------

int PndSimpleAnalysis::GetAntiPdgCode(int pdgcode)
{
  int code=0;
  if (fdbPdg->GetParticle(pdgcode)) {
    if (fdbPdg->GetParticle(pdgcode)->AntiParticle()) {
      code=-pdgcode;
    } else {
      code=pdgcode;
    }
  }

  //cout <<pdgcode<<" A="<<code<<endl;

  return code;
}

// -------------------------------------------------------------------------

bool PndSimpleAnalysis::ErrorMessage(int mid, int line, std::string arg)
{
  cout << "-E- PndSimpleAnalysis::SetupAnalysis: In '"<< fCfgFileName << "', line "<<line<<": ";

  switch (mid) {
  case 100:
    cout << "Missing 'End' statement!";
    break;

  case 101:
    cout << "List with name '"<< arg<< "' already defined";
    break;

  case 200:
    cout << "Missing 'DefineList' statement!";
    break;

  case 201:
    cout << "DecayMode already defined!";
    break;

  case 203:
    cout << "More than 5 daughters not allowed!";
    break;

  case 301:
    cout << "Missing 'decayMode' statement!";
    break;

  case 302:
    cout << "Too many daughterLists defined!";
    break;

  case 303:
    cout << "Unknown List '"<<arg<<"'!";
    break;

  case 304:
    cout << "Particle type mismatch between daughterList and decayMode!";
    break;

  case 305:
    cout << "McTruth cannot be used as daughterList!";
    break;

  case 400:
    cout << "Wrong number of daughters or daughterLists!";
    break;

  case 500:
    cout << "Mass of particle '"<<arg <<"' unknown; must specify selector mean value!";
    break;

  case 501:
    cout << "Must specify name of tuple column!";
    break;

  case 502:
    cout << "Must specify name of list and tuple column!";
    break;

  case 503:
    cout << "Warning: Initial 4 vector already set before!";
    break;

  case 504:
    cout << "Branch '"<<arg<<"' already exists in NTuple!";
    break;

  case 510:
    cout << "Invalid number for parameter 'MaxEntries'.";
    break;

  default:
    cout << "Unexpected error!";
    break;

  }

  cout <<endl;

  return false;
}


// -------------------------------------------------------------------------

ClassImp(PndSimpleAnalysis)


/*
// shortcut 4 vector
if (col=="p4")
{
  currentList->fNtpFNames.push_back("px");
  currentList->fNtpFNames.push_back("py");
  currentList->fNtpFNames.push_back("pz");
  currentList->fNtpFNames.push_back("en");
}
else if (col=="p4cm")
{
  currentList->fNtpFNames.push_back("pxcm");
  currentList->fNtpFNames.push_back("pycm");
  currentList->fNtpFNames.push_back("Cmpzcm");
  currentList->fNtpFNames.push_back("encm");
}
// shortcut position
else if (col=="pos")
{
  currentList->fNtpFNames.push_back("vx");
  currentList->fNtpFNames.push_back("vy");
  currentList->fNtpFNames.push_back("vz");
}
else if (col=="p3")
{
  currentList->fNtpFNames.push_back("phi");
  currentList->fNtpFNames.push_back("tht");
  currentList->fNtpFNames.push_back("cth");
  currentList->fNtpFNames.push_back("p");
}
else if (col=="p3cm")
{
  currentList->fNtpFNames.push_back("phicm");
  currentList->fNtpFNames.push_back("thtcm");
  currentList->fNtpFNames.push_back("cthcm");
  currentList->fNtpFNames.push_back("pcm");
}
// add all known columns
else
{
  // unknown name; skip entry
  if (fColKeyMap.find(col)==fColKeyMap.end())
  {
      cout << "-W- PndSimpleAnalysis::SetupAnalysis: In '"<< fCfgFileName
      << "', line "<<linecnt<<": ";
    cout << "Unknown column name '"<<col<<"'."<<endl;

    continue;
  }

  int key=fColKeyMap[col];

  if (key<200) currentList->fNtpFNames.push_back(col);
  else currentList->fNtpINames.push_back(col);
}
*/

