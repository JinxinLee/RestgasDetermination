// Macro created A.Sanchez
// It creates a geant simulation file for hyp
// Parameters
//1) Generator option 
//    HyPfile set the option for storage of stopping Xi  points
//    1) simple version of a sequential pion decay of Be11_LL
//    2) gam: no pion decay, but emission of single gammas for
//       evaluation of gamma detection effiency of HPGe

// 1.a ) first step Xi m + Xib p --> Decfile, 
//
//       "box", Box generator

// 1.b ) second step , hypernuclei production "hypbup",
//   
//2) detctor geo version
// 2.a) standard--> alicia version 
// 2.b) current --> sebastian version
#include "TString.h"

int QAmacro_hyp_1(Int_t nEvents = 500, TString  SimEngine ="TGeant4", Bool_t HYP_File= false,Bool_t gam = false)
{

  //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="hypqasim.root";
  TString  ParOutputfile  ="hypqapar.root";
  TString  MediaFile      ="media_pnd.geo";
  gDebug                  = 0;
  
  Float_t     BeamMomentum   =3.0; // ** change HERE if you run Box generator
  //------------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  gRandom->SetSeed();
  
  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName(SimEngine.Data() );
  fRun->SetOutputFile(OutputFile.Data());
  //  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
  //fRun->SetUseFairLinks(kTRUE);
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
  
  //---------------------Set Parameter output      ----------
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputfile.Data());
  rtdb->setOutput(output);
  
  // Create and add detectors
  
  //-------------------------  CAVE      -----------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);
  
  //-------------------------  HYP     -----------------
  PndHyp *Hyp = new PndHyp("HYP",kTRUE);
  Hyp->SetAbsorberVol("Absorber"); // absorber layer
    Hyp->SetSensorVol("Sensor");   // silicon sensor
    Hyp->SetGeoVersion("List");
    Hyp->SetListMat("HYPboron");
    Hyp->SetListMat("HYPaluminium");
    Hyp->SetListMat("Al97Mg3");
    Hyp->SetListMat("titanium");
    Hyp->SetListMat("HYPcarbon");
    Hyp->SetListMat("siliconinactive");
    Hyp->SetGeometryFileName("TargetSystem_WindowAbsB_insideAbsB.root");
  
    if(HYP_File){
      
      Hyp->SetHypSDtoFile(true,HYP_File);
      
      if(gam){
	Hyp->SetHypGamFEm(gam);
	Hyp->SetTreeFName("hypBupV1T_Decay_gam.root");
      }else{
	Hyp->SetTreeFName("hypBupV1T_Decay.root");
      }
    }
  
    Hyp->SetMatbud(false); //material budget hit Collection production
    
    fRun->AddModule(Hyp);
    
    //gROOT->LoadMacro("$VMCWORKDIR/gconfig/SetFragments.C");
    //FragConfig(fRun);
    
    //fRun->SetUserDecay(kTRUE);
    
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);
    
    FairBoxGenerator* boxGen = new FairBoxGenerator(3312, 1); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(0.1,0.5); // GeV/c
    boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
    boxGen->SetThetaRange(5., 150.); // Polar angle in lab system range [degree]
    boxGen->SetXYZ(0., 0., -55.); // cm
    primGen->AddGenerator(boxGen);

   
  
    PndMultiField *fField= new PndMultiField("FULL");
    fRun->SetField(fField);
    
    // EMC Hit producer
    //-------------------------------
    
    //-------------------------  Initialize the RUN  -----------------
    fRun->Init();
    //-------------------------  Run the Simulation  -----------------
    fRun->Run(nEvents);
    //-------------------------  Save the parameters -----------------
    rtdb->saveOutput();
    //------------------------Print some info and exit----------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
    
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    delete fRun;  
  return 0;
  
};

