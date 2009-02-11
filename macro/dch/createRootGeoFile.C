{

  // dch geometry parameters
  const Double_t  kHoleRadiusFS   = 5.;
  const Double_t  kCiut           = 5.;
  const Int_t      kNumOfChambers  = 8.;
  const Double_t  kFreeSpace      = 0.5;
  const Double_t  kPlaneThickness = 1.;
  const Int_t      kNumOfSplanes[kNumOfChambers]   = {2, 8, 6, 6, 6, 6, 6, 6};
  const Double_t  kDchDz[kNumOfChambers]       = {0.016,                         
						  0.5*(kNumOfSplanes[1]*kPlaneThickness+5.*kFreeSpace), 
						  0.5*(kNumOfSplanes[2]*kPlaneThickness+4.*kFreeSpace),  
						  0.5*(kNumOfSplanes[3]*kPlaneThickness+4.*kFreeSpace), 
						  0.5*(kNumOfSplanes[4]*kPlaneThickness+4.*kFreeSpace),  
						  0.5*(kNumOfSplanes[5]*kPlaneThickness+4.*kFreeSpace), 
						  0.5*(kNumOfSplanes[6]*kPlaneThickness+4.*kFreeSpace),  
						  0.5*(kNumOfSplanes[7]*kPlaneThickness+4.*kFreeSpace)};
  const Double_t  kDchDx[kNumOfChambers]       = {54., 80., 62., 75., 120., 139., 220., 320.};
  const Double_t  kDchDy[kNumOfChambers]       = { 0.,  0., 39., 45., 38., 42.5, 79., 94.};
  const Double_t  kDchPosition[kNumOfChambers] = {112., 178., 
 						  284.5, 330.,
 						  402., 452.,
 						  612.5, 742.5};
  const Double_t  kHoleRadius[kNumOfChambers]   = {3.1, 4.2, 4.2, 6.5, 6.5, 6.5, 6.5, 6.5};
  // const Double_t  kDchPosition[kNumOfChambers] = {50., 60., 
// 						  75, 90.,
// 						  110., 140.,
// 						  160., 190};
  //const Double_t  kHoleRadius[kNumOfChambers]   = {6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5};
  
  
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libPassive");

  TString outfile= "../../geometry/dch.root";
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumAir  = Media->getMedium("air");
  FairGeoMedium *CbmMediumSilicon = Media->getMedium("silicon");
  FairGeoMedium *CbmMediumGas = Media->getMedium("DCHmixture");
  FairGeoMedium *CbmMediumGasP = Media->getMedium("DCHmixturePassive");
  FairGeoMedium *CbmMediumVacuum = Media->getMedium("vacuum");
  FairGeoMedium *CbmMediumLead = Media->getMedium("lead");
  
  Int_t nmed=geobuild->createMedium(CbmMediumAir);
  nmed=geobuild->createMedium(CbmMediumSilicon);
  nmed=geobuild->createMedium(CbmMediumGas);
  nmed=geobuild->createMedium(CbmMediumGasP);
  nmed=geobuild->createMedium(CbmMediumVacuum);
  nmed=geobuild->createMedium(CbmMediumLead);
  
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    
  //construct the overall box
  cout<<endl<<endl<<endl<<"&&&&&&&&&&&&&&&&&&&&&&&"<<gGeoMan->GetMedium("air")<<endl<<endl;
  TGeoVolume *top = gGeoMan->MakeBox("top",gGeoMan->GetMedium("air"),350,100,800);
  gGeoMan->SetTopVolume(top);

  TGeoTranslation* tr[kNumOfChambers];
  TGeoShape* dchFullShape[kNumOfChambers];
  TGeoShape* planeFullShape[kNumOfChambers];
  TGeoTube* pipeHoleShape[kNumOfChambers];
  TGeoCompositeShape* dchShape[kNumOfChambers];
  TGeoCompositeShape* planeShape[kNumOfChambers];
  TGeoVolume* dchVol[kNumOfChambers];
  TGeoVolume* planeVol[kNumOfChambers];
  TString name, recipe;
  for(Int_t i=2; i<kNumOfChambers; i++){
    cout<< "********* Next chamber ***************************"<<endl;
    //define shape of the hole for beam pipe for the current chamber
    name = "pipeHoleShape";
    name += (i+1);
    pipeHoleShape[i] = new TGeoTube(name,0.,kHoleRadius[i],20.); 
    // define named geometrical transformations with names
    tr[i] = new TGeoTranslation(0., 0., kDchPosition[i]);
    name="tr";
    name+=(i+1);
    tr[i]->SetName(name);
    tr[i]->RegisterYourself();
    // define full shapes of chambers and planes (without the hole)
    name = "dchFullShape";
    name+=(i+1);
    if(i<2){
      dchFullShape[i] = new TGeoTube(name,0.,kDchDx[i],kDchDz[i]);
      if(i==0) planeFullShape[i] = new TGeoTube("planeFullShape1",0.,kDchDx[i],kDchDz[i]/2.);
      else     planeFullShape[i] = new TGeoTube("planeFullShape2",0.,kDchDx[i],kPlaneThickness/2.);
    }
    else{
      dchFullShape[i] = new TGeoBBox(name,kDchDx[i],kDchDy[i],kDchDz[i]); 
      name = "planeFullShape";
      name += (i+1);
      planeFullShape[i] = new TGeoBBox(name,kDchDx[i],kDchDy[i],kPlaneThickness/2.); 
    }
    // create shapes of chambers with holes
    name = "dchShape";
    name += (i+1);
    recipe = dchFullShape[i]->GetName();
    recipe += "-pipeHoleShape";
    recipe+=(i+1);
    dchShape[i] = new TGeoCompositeShape(name,recipe);
    //creates shapes of planes with holes
    name = "planeShape";
    name += (i+1);
    recipe = planeFullShape[i]->GetName();
    recipe += "-pipeHoleShape";
    recipe+=(i+1);
    planeShape[i] = new TGeoCompositeShape(name,recipe);
    // create physical volumes of chambers
    name = "dchVol";
    name+=(i+1);
    TString medium;
    i>0 ? medium = "DCHmixturePassive" : medium = "air";
    dchVol[i] = new TGeoVolume(name, dchShape[i], gGeoMan->GetMedium(medium));
    cout<<"volume "<<dchVol[i]->GetName()<< " created"<<endl;
    top->AddNode(dchVol[i],0,tr[i]);
    // create physical volumes of planes
    name = "dchSplaneVol";
    name+=(i+1);
    TString medium;
    i>0 ? medium = "DCHmixture" : medium = "silicon";
    planeVol[i] = new TGeoVolume(name, planeShape[i], gGeoMan->GetMedium(medium));
    cout<<"volume "<<planeVol[i]->GetName()<< " created"<<endl;
    
    ////////////////////////////////////////
    // adding sensitive planes to chambers
    ////////////////////////////////////////
    TGeoTranslation* trFS[(kNumOfChambers-2)*(kNumOfSplanes[5])];   
    // gem
    if(i==0){
      TGeoTranslation* tr1[2];
      tr1[0] = new TGeoTranslation(0., 0., -kDchDz[0]/2.);
      name="tr1_1";
      tr1[0]->SetName(name);
      tr1[0]->RegisterYourself();
      tr1[1] = new TGeoTranslation(0., 0., kDchDz[0]/2.);
      name="tr1_2";
      tr1[1]->SetName(name);
      tr1[1]->RegisterYourself();
      dchVol[0]->AddNode(planeVol[0],1,tr1[0]);
      dchVol[0]->AddNode(planeVol[0],2,tr1[1]);
    }
    // round TS dch
    else if (i==1) {
      TGeoTranslation* tr2[8];
      for(Int_t j=0; j<kNumOfSplanes[1]; j++){
	tr2[j] = new TGeoTranslation(0., 0., 
				     kPlaneThickness*(j+0.5)+
				     kFreeSpace*(1+((Int_t)(j/2)))-
				     kDchDz[1]);
	name="tr2_";
	name+=(j+1);
	tr2[j]->SetName(name);
	tr2[j]->RegisterYourself();
	dchVol[1]->AddNode(planeVol[1],j+1,tr2[j]);
      }
    }
    // standard rectangular dch
    else {
      Int_t planesSoFar=(i-2)*kNumOfSplanes[5];
      for(Int_t j=0; j<kNumOfSplanes[i]; j++){
	trFS[j+planesSoFar] = new TGeoTranslation(0., 0., 
						   kPlaneThickness*(j+0.5)+
						  kFreeSpace*(1+((Int_t)(j/2)))-
						  kDchDz[i]);
	name="tr";
	name+=(i+1);
	name+="_";
	name+=(j+1);
	trFS[j+planesSoFar]->SetName(name);
	trFS[j+planesSoFar]->RegisterYourself();
	dchVol[i]->AddNode(planeVol[i],j+1,trFS[j+planesSoFar]);
      }
    }
  }


  for(Int_t i=0; i<10; i++){
    cout<<endl;
    for(Int_t j=0; j<10; j++){
      TString namech="tr";
      TString name;
      namech+=i;
      name=namech;
      name+="_";
      name+=j;
      if(gGeoManager->GetListOfMatrices()->FindObject(name)){
	const Double_t* match = ((TGeoCombiTrans*)gGeoManager->GetListOfMatrices()->FindObject(namech))->GetTranslation();
	const Double_t* matpl = ((TGeoCombiTrans*)gGeoManager->GetListOfMatrices()->FindObject(name))->GetTranslation();
	Double_t z=match[2]+matpl[2];
	std::cout<<name<<"\t"<<z<<std::endl;
      }
    }
  }
  
  gGeoMan->CloseGeometry();
  
   TFile* fi = new TFile(outfile,"RECREATE");
   top->Write();
   fi->Close();

   //  gGeoManager->SetName("dchGeom");
   //gGeoManager->Export(outfile);

  //top->Draw();
}  

