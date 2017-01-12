#include "PndPidEmcBayesAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
#include "TH2.h"
#include "Riostream.h"


//___________________________________________________________
PndPidEmcBayesAssociatorTask::~PndPidEmcBayesAssociatorTask() {
  //
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();
}

//___________________________________________________________
PndPidEmcBayesAssociatorTask::PndPidEmcBayesAssociatorTask() {
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
PndPidEmcBayesAssociatorTask::PndPidEmcBayesAssociatorTask(const char *name, const char *title):FairTask(name) 
{
  //---
  fPidChargedProb = new TClonesArray("PndPidProbability");
}

//___________________________________________________________
InitStatus PndPidEmcBayesAssociatorTask::Init() {
  
  std::cout << "InitStatus PndPidEmcBayesAssociatorTask::Init()" << std::endl;
  
  FairRootManager *fManager =FairRootManager::Instance();        

  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  if ( ! fPidChargedCand) {
    std::cout << "-I- PndPidEmcBayesAssociatorTask::Init: No PndPidCandidate array PidChargedCand there!" << std::endl;
    return kERROR;
  }
    
  Register();

  fevcounter=0;

  SetupEnvironment((char *)"BayesEMC.root");   // read back of the files
  std::cout << "-I- PndPidEmcBayesAssociatorTask::Init: Success!!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidEmcBayesAssociatorTask::SetupEnvironment(char *filename) {
// read in all pdf files and store
  TString fPath = getenv("VMCWORKDIR");
  fPath += "/macro/params/";
  TString fullName = fPath + filename;
  printf("-I- PndPidEmcBayesAssociatorTask::SetupEnvironment:File used=%s",filename);
      TFile* hfile1 = new TFile (fullName);

      fBayesEP[0] = (TH2D*)hfile1->Get("p.v.EP_proba_elecSpos");
      fBayesTH[0] = (TH2D*)hfile1->Get("p.v.LAT_proba_elecSpos");
      fBayesZZ[0] = (TH2D*)hfile1->Get("p.v.Z53_proba_elecSpos");

      fBayesEP[1] = (TH2D*)hfile1->Get("p.v.EP_proba_muonSpos");
      fBayesTH[1] = (TH2D*)hfile1->Get("p.v.LAT_proba_muonSpos");
      fBayesZZ[1] = (TH2D*)hfile1->Get("p.v.Z53_proba_muonSpos");

      fBayesEP[2] = (TH2D*)hfile1->Get("p.v.EP_proba_pionSpos");
      fBayesTH[2] = (TH2D*)hfile1->Get("p.v.LAT_proba_pionSpos");
      fBayesZZ[2] = (TH2D*)hfile1->Get("p.v.Z53_proba_pionSpos");

      fBayesEP[3] = (TH2D*)hfile1->Get("p.v.EP_proba_kaonSpos");
      fBayesTH[3] = (TH2D*)hfile1->Get("p.v.LAT_proba_kaonSpos");
      fBayesZZ[3] = (TH2D*)hfile1->Get("p.v.Z53_proba_kaonSpos");

      fBayesEP[4] = (TH2D*)hfile1->Get("p.v.EP_proba_protSpos");
      fBayesTH[4] = (TH2D*)hfile1->Get("p.v.LAT_proba_protSpos");
      fBayesZZ[4] = (TH2D*)hfile1->Get("p.v.Z53_proba_protSpos");

      fBayesEP[5] = (TH2D*)hfile1->Get("p.v.EP_proba_elecSneg");
      fBayesTH[5] = (TH2D*)hfile1->Get("p.v.LAT_proba_elecSneg");
      fBayesZZ[5] = (TH2D*)hfile1->Get("p.v.Z53_proba_elecSneg");

      fBayesEP[6] = (TH2D*)hfile1->Get("p.v.EP_proba_muonSneg");
      fBayesTH[6] = (TH2D*)hfile1->Get("p.v.LAT_proba_muonSneg");
      fBayesZZ[6] = (TH2D*)hfile1->Get("p.v.Z53_proba_muonSneg");

      fBayesEP[7] = (TH2D*)hfile1->Get("p.v.EP_proba_pionSneg");
      fBayesTH[7] = (TH2D*)hfile1->Get("p.v.LAT_proba_pionSneg");
      fBayesZZ[7] = (TH2D*)hfile1->Get("p.v.Z53_proba_pionSneg");

      fBayesEP[8] = (TH2D*)hfile1->Get("p.v.EP_proba_kaonSneg");
      fBayesTH[8] = (TH2D*)hfile1->Get("p.v.LAT_proba_kaonSneg");
      fBayesZZ[8] = (TH2D*)hfile1->Get("p.v.Z53_proba_kaonSneg");

      fBayesEP[9] = (TH2D*)hfile1->Get("p.v.EP_proba_protSneg");
      fBayesTH[9] = (TH2D*)hfile1->Get("p.v.LAT_proba_protSneg");
      fBayesZZ[9] = (TH2D*)hfile1->Get("p.v.Z53_proba_protSneg");


}
//______________________________________________________
void PndPidEmcBayesAssociatorTask::SetParContainers() {
  //--
}
//______________________________________________________
void PndPidEmcBayesAssociatorTask::Exec(Option_t * option) {
  if (fPidChargedProb->GetEntriesFast() != 0) fPidChargedProb->Clear();
  if(fVerbose>1) std::cout << "-I- Start PndPidEmcBayesAssociatorTask. "<<std::endl;
  if(fVerbose>1) std::cout << "-I- counter: "<< fevcounter <<std::endl;
  fevcounter++;

  // Get the Candidates
  for(Int_t i=0; i<fPidChargedCand->GetEntriesFast(); i++)
    {
      PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
      TClonesArray& pidRef = *fPidChargedProb;
      PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
      prob->SetIndex(i);
      if (pidcand->GetEmcIndex()==-1) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidEmcBayesAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
//   Double_t mom          = pidcand->GetMomentum().Mag(); 
   Double_t radeg=57.29578;

   Int_t Charge         = pidcand-> GetCharge();
   Double_t emc          = pidcand->GetEmcRawEnergy();
   Double_t z20          = pidcand->GetEmcClusterZ20();
   Double_t z53          = pidcand->GetEmcClusterZ53();
   Double_t lat          = pidcand->GetEmcClusterLat();
   TLorentzVector pidTrack = pidcand->GetLorentzVector();
           Double_t pidx   = pidTrack.Px(); 
           Double_t pidy   = pidTrack.Py(); 
           Double_t pidz   = pidTrack.Pz(); 
   Double_t pidth  = radeg*pidTrack.Theta(); 
   Double_t pidph  = radeg*pidTrack.Phi(); 
    
   Double_t pidp   = TMath::Sqrt(pidx*pidx+pidy*pidy+pidz*pidz);
   Double_t EP     = emc/pidp;

// Get the probabilities
   Double_t proba[5];
   GetPdf(pidp,pidth,pidph,z20,z53,lat,EP,Charge,proba);
   if(fVerbose>1) {
         std::cout <<" proba in Pidmatch: " << proba[0] << " ";
         std::cout << proba[1] << " " << proba[2] << " ";
         std::cout << proba[3] << " " << proba[4] << std::endl;
   }
   
// And store them
  // electron
  {
    prob->SetElectronPdf(proba[0]);
  }

  // muon
  {
    prob->SetMuonPdf(proba[1]);
  }
  
  // pion
  {
    prob->SetPionPdf(proba[2]);
  }
  
  // kaon
  {
    prob->SetKaonPdf(proba[3]);
  }
  
  // proton
  {
    prob->SetProtonPdf(proba[4]);
  }
}  

void PndPidEmcBayesAssociatorTask::GetPdf(Double_t ppin, Double_t thin, Double_t phin, 
                                          Double_t z20in, Double_t z53in, Double_t LATin, Double_t EPin,
                                          Int_t charge, Double_t *proba)
{
// variables: pp,th,ph, z20, z53, LAT, E/P
   Double_t lRange[7]={0.2,  5,-180, 0, 0, 0, 0};
   Double_t uRange[7]={10  ,140, 180, 4, 5, 6, 2};
   //Int_t nRange[7]  ={14 ,  7,   1, 1,   1,120}; //[R.K. 01/2017] unused variable
   Double_t rangePconst0= 4.2318;  // Two constants to ajust the momenta 
   Double_t rangePconst1= 5.7682;  // calculated from the nominal range
    
   Double_t pp  = ppin;
   Double_t th  = thin;
   Double_t ph  = phin;
   Double_t Z20 = z20in;
   Double_t Z53 = z53in;
   Double_t LAT = LATin;
   Double_t EP  = EPin;
   if(fVerbose>1) std::cout << "ppin: " << pp  << std::endl;

// trafo that spreads out the parameters
   if( ppin<0.000001)  ppin=0.000001;
   if(z20in>0.999999) z20in=0.999999;
   if(z53in<0.000001) z53in=0.000001;
   if(LATin<0.000001) LATin=0.000001;

   pp  = rangePconst0 + rangePconst1 * TMath::Log10(ppin);
   Z20 = -TMath::Log10(1-z20in);
   Z53 = -TMath::Log10(z53in);
   LAT = -TMath::Log10(LATin);
   if(fVerbose>1) std::cout << "pp: " << pp  << std::endl;

// if the values are outside the range, get them back in
       Double_t pplook=ppin;

       if(pplook<lRange[0]) pplook=1.0001*lRange[0]; 
       if(pplook>uRange[0]) pplook=0.9999*uRange[0]; 
       if(th    <lRange[1]) th    =1.0001*lRange[1]; 
       if(th    >uRange[1]) th    =0.9999*uRange[1]; 
       if(ph    <lRange[2]) ph    =1.0001*lRange[2]; 
       if(ph    >uRange[2]) ph    =0.9999*uRange[2]; 
       if(Z20   <lRange[3]) Z20   =1.0001*lRange[3]; 
       if(Z20   >uRange[3]) Z20   =0.9999*uRange[3]; 
       if(Z53   <lRange[4]) Z53   =1.0001*lRange[4]; 
       if(Z53   >uRange[4]) Z53   =0.9999*uRange[4]; 
       if(LAT   <lRange[5]) LAT   =1.0001*lRange[5]; 
       if(LAT   >uRange[5]) LAT   =0.9999*uRange[5]; 
       if(EP    <lRange[6]) EP    =1.0001*lRange[6]; 
       if(EP    >uRange[6]) EP    =0.9999*uRange[6]; 
   if(fVerbose>1) std::cout << "pplook: " << pplook  << std::endl;

// Get the bins in the histograms
       Int_t binxP = (fBayesEP[0]->GetXaxis())->FindBin(pplook);
       Int_t binyP = (fBayesEP[0]->GetYaxis())->FindBin(EP);
       Int_t binxT = (fBayesTH[0]->GetXaxis())->FindBin(pplook);
       Int_t binyT = (fBayesTH[0]->GetYaxis())->FindBin(LAT);
       Int_t binxZ = (fBayesZZ[0]->GetXaxis())->FindBin(pplook);
       Int_t binyZ = (fBayesZZ[0]->GetYaxis())->FindBin(Z53);
       Double_t PBayesE[5];
       Double_t PBayesL[5];
       Double_t PBayesZ[5];
       Double_t PBayesB[5];

       if(fVerbose>1) {
           std::cout << "pp: " << pp << " EP " << EP << std::endl;
           std::cout << "probas: ";
       }


       Double_t probasum=0; // used for normalisation
   
       for (Int_t k=0; k<5; k++){
         if(charge>0) {
           PBayesE[k]=fBayesEP[k]->GetBinContent(binxP,binyP);
           PBayesL[k]=fBayesTH[k]->GetBinContent(binxT,binyT);
           PBayesZ[k]=fBayesZZ[k]->GetBinContent(binxZ,binyZ);
           PBayesB[k]=PBayesE[k]*PBayesL[k]*PBayesZ[k];
           probasum+=PBayesB[k];
         } else {
           PBayesE[k]=fBayesEP[k+5]->GetBinContent(binxP,binyP);
           PBayesL[k]=fBayesTH[k+5]->GetBinContent(binxT,binyT);
           PBayesZ[k]=fBayesZZ[k+5]->GetBinContent(binxZ,binyZ);
           PBayesB[k]=PBayesE[k]*PBayesL[k]*PBayesZ[k];
           probasum+=PBayesB[k];
         }
         PBayesB[k]=PBayesE[k]*PBayesL[k]*PBayesZ[k]/
                 (1-PBayesE[k])*(1-PBayesL[k])*(1-PBayesZ[k]);
         if(fVerbose>1)  std::cout << PBayesZ[k] << " ";
      }


// Normalise the probabilities to one
   for (Int_t k=0; k<5 ; k++){
     if(probasum>0) {
       proba[k]=PBayesB[k]/(1+PBayesB[k]);
     } else {
       proba[k]=1.;
     }
   }
}

//_________________________________________________________________
void PndPidEmcBayesAssociatorTask::Register() {
  //---
  FairRootManager::Instance()->
    Register("PidAlgoEmcBayes","Pid", fPidChargedProb, kTRUE); 
}

//_________________________________________________________________
void PndPidEmcBayesAssociatorTask::Finish() {
}

//_________________________________________________________________
void PndPidEmcBayesAssociatorTask::Reset() {
  //---
}


ClassImp(PndPidEmcBayesAssociatorTask)
