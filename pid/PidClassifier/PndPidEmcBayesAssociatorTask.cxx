#include "PndPidEmcBayesAssociatorTask.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "FairRootManager.h"
#include "TMath.h"
#include "TF1.h"
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

  SetupEnvironment((char *)"ProbaEMC110407.dat");   // prepare the pdf files
  std::cout << "-I- PndPidEmcBayesAssociatorTask::Init: Success!" << std::endl;
  
  return kSUCCESS;
}

//______________________________________________________
void PndPidEmcBayesAssociatorTask::SetupEnvironment(char *filename) {
// read in all pdf files and store
  TString fPath = getenv("VMCWORKDIR");
  fPath += "/macro/params/";
  TString fullName = fPath + filename;
  printf("-I- PndPidEmcBayesAssociatorTask::SetupEnvironment:File used=%s",filename);
  fDirectory[0]= "elec_pos";
  fDirectory[1]= "muon_pos";
  fDirectory[2]= "pion_pos"; 
  fDirectory[3]= "kaon_pos";
  fDirectory[4]= "prot_pos";
  fDirectory[5]= "elec_neg";
  fDirectory[6]= "muon_neg";
  fDirectory[7]= "pion_neg";  
  fDirectory[8]= "kaon_neg";
  fDirectory[9]= "prot_neg";  

  FILE *in;
  in = fopen(fullName,"r");

// Loop over particles
  int Nbins,dum0,dum1,dum2,dum3,dum4,kip;
  float fdum0,fdum1,fdum2,fdum3,fdum4,fdum5;
  double ddum0,ddum1,ddum2,ddum3,ddum4,ddum5;

  fscanf(in,"%d %d %d %d %d %d \n",&Nbins,&dum0,&dum1,&dum2,&dum3,&dum4);
  fNbins=Nbins;
  fNvar[0]=dum0;
  fNvar[1]=dum1;
  fNvar[2]=dum2;
  fNvar[3]=dum3;
  fNvar[4]=dum4;
  fNvar[5]=1;
  
  if(fVerbose>1) {
    printf("********************************************\n");
    printf("%d %d %d %d %d %d %d\n",fNbins,fNvar[0],fNvar[1],fNvar[2],
                                           fNvar[3],fNvar[4],fNvar[5]
         );
  }

  char histfunction[40];

// loop over particle sets
  for (Int_t ip=0; ip<10 ; ip++){
    fscanf(in,"%d\n",&kip);

// loop over functions
    for (Int_t k=0; k<fNbins ; k++){
      fscanf(in,"%d %f %f %f %f %f\n",&kip,&fdum0,&fdum1,&fdum2,&fdum3,&fdum4);
      fitCenter[ip][0][k]=fdum0;
      fitCenter[ip][1][k]=fdum1;
      fitCenter[ip][2][k]=fdum2;
      fitCenter[ip][3][k]=fdum3;
      fitCenter[ip][4][k]=fdum4;

      fscanf(in,"%e %f %f %f\n",&fdum0,&fdum1,&fdum2,&fdum3);
      fitChi2[ip][k]=fdum0;
      fitScale[ip][k]=fdum1;
      fitRange[ip][0][k]=fdum2;
      fitRange[ip][1][k]=fdum3;

      fscanf(in,"%e %f %f %f %f %f\n",&fdum0,&fdum1,&fdum2,&fdum3,&fdum4,&fdum5);
      fitParam[ip][0][k]=fdum0;
      fitParam[ip][1][k]=fdum1;
      fitParam[ip][2][k]=fdum2;
      fitParam[ip][3][k]=fdum3;
      fitParam[ip][4][k]=fdum4;
      fitParam[ip][5][k]=fdum5;

      fscanf(in,"%e %f %f %f %f %f\n",&fdum0,&fdum1,&fdum2,&fdum3,&fdum4,&fdum5);
      fitParam[ip][6][k]=fdum0;
      fitParam[ip][7][k]=fdum1;
      fitParam[ip][8][k]=fdum2;
      fitParam[ip][9][k]=fdum3;
      fitParam[ip][10][k]=fdum4;
      fitParam[ip][11][k]=fdum5;

      if(fVerbose>1) {
        printf("%d %10.4f %10.4f %10.4f %10.4f %10.4f\n",
         k,fitCenter[ip][0][k],fitCenter[ip][1][k],
         fitCenter[ip][2][k],fitCenter[ip][3][k],fitCenter[ip][4][k]
        );
        printf("%10.4e %10.4f %10.4f %10.4f\n",
         fitChi2[ip][k],fitScale[ip][k],fitRange[ip][0][k],fitRange[ip][1][k]
        );
        printf("%10.4e %10.4e %10.4e %10.4e %10.4e %10.4e\n",
         fitParam[ip][0][k],fitParam[ip][1][k],fitParam[ip][2][k],
         fitParam[ip][3][k],fitParam[ip][4][k],fitParam[ip][5][k]
        );
        printf("%10.4e %10.4e %10.4e %10.4e %10.4e %10.4e\n",
         fitParam[ip][6][k],fitParam[ip][7][k],fitParam[ip][8][k],
         fitParam[ip][9][k],fitParam[ip][10][k],fitParam[ip][11][k]
        );
      }
      
      TString aline="fPDF_"+fDirectory[ip];
      aline+="_";
      aline+=k;
      fPDF[ip][k]= new TF1(aline,"pol5(0)+gaus(6)+gaus(9)",fitRange[ip][0][k],fitRange[ip][1][k]);
      if(fVerbose>1)  std::cout << "defined fPDF: " << aline << std::endl;
      for (Int_t ib=0; ib<12 ; ib++){
          Double_t paramk = fitParam[ip][ib][k];
          fPDF[ip][k]->SetParameter(ib,paramk);
      }
    }
  }
  
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
      if (pidcand->GetDrcIndex()==-1) continue;
      DoPidMatch(pidcand,prob);
    }
 
}

void PndPidEmcBayesAssociatorTask::DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob)
{
//   Float_t mom          = pidcand->GetMomentum().Mag(); 
   Float_t radeg=57.29578;

   Int_t Charge         = pidcand-> GetCharge();
   Float_t emc          = pidcand->GetEmcRawEnergy();
   Float_t z20          = pidcand->GetEmcClusterZ20();
   Float_t z53          = pidcand->GetEmcClusterZ53();

   TLorentzVector pidTrack = pidcand->GetLorentzVector();
  	 Float_t pidx   = pidTrack.Px(); 
  	 Float_t pidy   = pidTrack.Py(); 
  	 Float_t pidz   = pidTrack.Pz(); 
   Float_t pidth  = radeg*pidTrack.Theta(); 
   Float_t pidph  = radeg*pidTrack.Phi(); 
    
   Float_t pidp   = TMath::Sqrt(pidx*pidx+pidy*pidy+pidz*pidz);
   Float_t EP     = emc/pidp;

// Get the probabilities
   Float_t proba[5];
   GetPdf(pidp,pidth,pidph,z20,z53,EP,Charge,proba);
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

void PndPidEmcBayesAssociatorTask::GetPdf(Float_t ppin, Float_t thin, Float_t phin, 
                                          Float_t z20in, Float_t z53in, Float_t EPin, 
                                          Int_t charge, Float_t *proba)
{
   Float_t lRange[6]={0.2,  5,-180, 0, 0.4, 0};
   Float_t uRange[6]={5  ,140, 180, 3, 4.8, 2};
   Int_t nRange[6]  ={14 ,  7,   1, 1,   1,120};
   
   Float_t pp  = ppin;
   Float_t th  = thin;
   Float_t ph  = phin;
   Float_t Z20 = z20in;
   Float_t Z53 = z53in;
   Float_t EP  = EPin;

// trafo that spreads out the parameters
   Z20 = -TMath::Log10(1-z20in);
   Z53 = -TMath::Log10(z53in);
   pp  = 2.6 + 3.4336 * TMath::Log10(ppin);  
   
   Float_t sum=0;  // used for normalisation

// cuts used in the tables reproduced here
   if(pp>=lRange[0] && pp<=uRange[0] && 
      th>=lRange[1] && th<=uRange[1] &&
      ph>=lRange[2] && ph<=uRange[2] &&
      Z20>=lRange[3] && Z20<=uRange[3] &&
      Z53>=lRange[4] && Z53<=uRange[4] &&
      EP>=lRange[5] && EP<=uRange[5]) {

      Int_t ipp   = (int) (fNvar[0]*(pp  -lRange[0])/(uRange[0]-lRange[0]));
      Int_t ith   = (int) (fNvar[1]*(th  -lRange[1])/(uRange[1]-lRange[1]));
//   Int_t iph   = (int) (fNvar[2]*(ph  -lRange[2])/(uRange[2]-lRange[2]));

      Int_t ifinal = fNvar[2]*(ith+fNvar[1]*ipp);

      for (Int_t k=0; k<5 ; k++){
        proba[k]=0;
        if(charge>0) {
         if(EP>fitRange[k][0][ifinal] && EP<fitRange[k][1][ifinal]) {
          proba[k]=fPDF[k][ifinal]->Eval(EP,0,0);
          proba[k]/=fitScale[k][ifinal]; 
          if(proba[k]<0)  proba[k]=0;
         }
        } else {
         if(EP>fitRange[k+5][0][ifinal] && EP<fitRange[k+5][1][ifinal]) {
          proba[k]=fPDF[k+5][ifinal]->Eval(EP,0,0);
          proba[k]/=fitScale[k+5][ifinal]; 
          if(proba[k]<0)  proba[k]=0;
         }
        }
        sum+=proba[k];
      }
   }

// Normalise the probabilities to one
   for (Int_t k=0; k<5 ; k++){
     if(sum>0) {
       proba[k]/=sum;
     } else {
       proba[k]=0.2;
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
