Int_t createRootGeoFile(Bool_t builtRICH = kTRUE){

  // dch geometry parameters
  const Double_t kMylarThicknessPerDL = 3.e-3;
  const Double_t kWoThicknessPerDL = 6.28e-6;

  const Double_t  kHoleRadiusFS   = 5.;
  const Double_t  kCiut           = 5.;
  const Int_t      kNumOfChambers  = 8.;
  const Double_t  kPlaneThickness = 1.01;
  const Double_t  kFreeSpace      = 2.5-kPlaneThickness-kMylarThicknessPerDL/2.-kWoThicknessPerDL/2.;
  const Int_t    kNumOfSplanes[kNumOfChambers]   = {0, 0, 8, 8, 8, 8, 8, 8};
  const Double_t  kDchDz[kNumOfChambers]       = {0.,0., 10.,10.,10.,10.,10.,10.};
  const Double_t  kDchDx[kNumOfChambers]       = { 0.,  0., 67., 67., 87.8, 96.8, 164., 190.};
  const Double_t  kDchDy[kNumOfChambers]       = { 0.,  0., 32., 32., 37.5, 41.4, 54.5, 69.5};
//   const Double_t  kDchDx[kNumOfChambers]       = { 0.,  0., 67., 500., 500., 500., 500., 500.};
//   const Double_t  kDchDy[kNumOfChambers]       = { 0.,  0., 32., 300., 300., 300., 300., 300.};
  Double_t  kDchPosition[kNumOfChambers] = {112., 178., 
 						  305., 337.,
 						  418., 462.,
 						  615., 760.};
  const Double_t  kHoleRadius[kNumOfChambers]   = {3.1, 4.2, 4.2, 6.5, 6.5, 6.5, 6.5, 6.5};

  if(builtRICH==kFALSE){
    kDchPosition[kNumOfChambers-1]= kDchPosition[kNumOfChambers-2]+
      kDchDz[kNumOfChambers-2]+kDchDz[kNumOfChambers-1]+5.;
    TString outfile= "../../geometry/dch_woRich.root";
  }
  else{
    TString outfile= "../../geometry/dch.root";
  }
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  // Load this example libraries
//   gSystem->Load("libGeoBase");
//   gSystem->Load("libParBase");
//   gSystem->Load("libBase");
//   gSystem->Load("libMCStack");
//   gSystem->Load("libPassive");

  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *FairMediumAir  = Media->getMedium("air");
  FairGeoMedium *FairMediumGas = Media->getMedium("DCHmixture2bar");
  FairGeoMedium *FairMediumGasP = Media->getMedium("DCHmixturePassive");
  FairGeoMedium *FairMediumVacuum = Media->getMedium("vacuum");
  FairGeoMedium *FairMediumLead = Media->getMedium("lead");
  FairGeoMedium *FairMediumTungsten = Media->getMedium("tungsten");
  FairGeoMedium *FairMediumMylar = Media->getMedium("mylar");
  
  Int_t nmed=geobuild->createMedium(FairMediumAir);
  nmed=geobuild->createMedium(FairMediumGas);
  nmed=geobuild->createMedium(FairMediumGasP);
  nmed=geobuild->createMedium(FairMediumVacuum);
  nmed=geobuild->createMedium(FairMediumLead);
  nmed=geobuild->createMedium(FairMediumTungsten);
  nmed=geobuild->createMedium(FairMediumMylar);
  
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    
  //construct the overall box
  cout<<endl<<endl<<endl<<"&&&&&&&&&&&&&&&&&&&&&&&"<<gGeoMan->GetMedium("air")<<endl<<endl;
  TGeoVolume *top = gGeoMan->MakeBox("topOne",gGeoMan->GetMedium("air"),400,130,800);
  
  gGeoMan->SetTopVolume(top);

  TGeoTranslation* tr[kNumOfChambers];
  TGeoShape* dchFullShape[kNumOfChambers];
  TGeoShape* mylarFullShape[kNumOfChambers];
  TGeoShape* woFullShape[kNumOfChambers];
  TGeoShape* mylarFullShape[kNumOfChambers];
  TGeoShape* woFullShape[kNumOfChambers];
  TGeoShape* planeFullShape[kNumOfChambers];
  TGeoTube* pipeHoleShape[kNumOfChambers];
  TGeoCompositeShape* dchShape[kNumOfChambers];
  TGeoCompositeShape* planeShape[kNumOfChambers];
  TGeoCompositeShape* mylarShape[kNumOfChambers];
  TGeoCompositeShape* woShape[kNumOfChambers];
  TGeoVolume* dchVol[kNumOfChambers];
  TGeoVolume* planeVol[kNumOfChambers];
  TGeoVolume* mylarVol[kNumOfChambers];
  TGeoVolume* woVol[kNumOfChambers];

  TString name, recipe;
  for(Int_t i=2; i<kNumOfChambers; i++){ 
    //for(Int_t i=2; i<kNumOfChambers-2; i++){
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
    dchFullShape[i] = new TGeoBBox(name,kDchDx[i],kDchDy[i],kDchDz[i]); 
    name = "planeFullShape";
    name += (i+1);
    planeFullShape[i] = new TGeoBBox(name,kDchDx[i],kDchDy[i],kPlaneThickness/2.); 
    name = "mylarFullShape";
    name += (i+1);
    mylarFullShape[i] = new TGeoBBox(name,kDchDx[i],kDchDy[i],kMylarThicknessPerDL/4.);
    name = "woFullShape";
    name += (i+1);
    woFullShape[i] = new TGeoBBox(name,kDchDx[i],kDchDy[i],kWoThicknessPerDL/4.); 
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
    //creates shapes of mylar with holes
    name = "mylarShape";
    name += (i+1);
    recipe = mylarFullShape[i]->GetName();
    recipe += "-pipeHoleShape";
    recipe+=(i+1);
    mylarShape[i] = new TGeoCompositeShape(name,recipe);
    //creates shapes of Wo with holes
    name = "woShape";
    name += (i+1);
    recipe = woFullShape[i]->GetName();
    recipe += "-pipeHoleShape";
    recipe+=(i+1);
    woShape[i] = new TGeoCompositeShape(name,recipe);
    // create physical volumes of chambers
    name = "dchVol";
    name+=(i+1);
    TString medium = "air";
    dchVol[i] = new TGeoVolume(name, dchShape[i], gGeoMan->GetMedium(medium));
    cout<<"volume "<<dchVol[i]->GetName()<< " created"<<endl;
    top->AddNode(dchVol[i],0,tr[i]);
    // create physical volumes of planes
    name = "dchSplaneVol";
    name+=(i+1);
    TString  medium = "DCHmixture2bar";
    planeVol[i] = new TGeoVolume(name, planeShape[i], gGeoMan->GetMedium(medium));
    cout<<"volume "<<planeVol[i]->GetName()<< " created"<<endl;
    // create physical volumes of mylar
    name = "mylarVol";
    name+=(i+1);
    TString  medium = "mylar";
    mylarVol[i] = new TGeoVolume(name, mylarShape[i], gGeoMan->GetMedium(medium));
    cout<<"volume "<<mylarVol[i]->GetName()<< " created"<<endl;
    // create physical volumes of tungsten
    name = "woVol";
    name+=(i+1);
    TString  medium = "tungsten";
    woVol[i] = new TGeoVolume(name, woShape[i], gGeoMan->GetMedium(medium));
    cout<<"volume "<<woVol[i]->GetName()<< " created"<<endl;
    
    ////////////////////////////////////////
    // adding sensitive planes to chambers
    ////////////////////////////////////////
    TGeoTranslation* trFS[(kNumOfChambers-2)*(kNumOfSplanes[5])];   
    TGeoTranslation* trFS_mylar[(kNumOfChambers-2)*(kNumOfSplanes[5])];   
    TGeoTranslation* trFS_wo[(kNumOfChambers-2)*(kNumOfSplanes[5])];   
    
    // standard rectangular dch
    Int_t planesSoFar=(i-2)*kNumOfSplanes[5];
    for(Int_t j=0; j<kNumOfSplanes[i]; j++){
      Double_t planeZpos;
      if(j%2==0) planeZpos = -kDchDz[i]+(1+2.*(j/2))*2.5-kPlaneThickness/2.;
      else if(j%2==1) planeZpos = -kDchDz[i]+(1+2.*(j/2))*2.5+kPlaneThickness/2.+
	kMylarThicknessPerDL/2.+kWoThicknessPerDL/2.;
      trFS[j+planesSoFar] = new TGeoTranslation(0., 0., planeZpos);
      trFS_mylar[j+planesSoFar]       = new TGeoTranslation(0., 0.,  planeZpos-kPlaneThickness/2.-
							    kMylarThicknessPerDL/4.-kWoThicknessPerDL/2.);
      trFS_wo[j+planesSoFar]    = new TGeoTranslation(0., 0.,   planeZpos-kPlaneThickness/2.-
						      -kWoThicknessPerDL/4.);

      name="tr";
      name+=(i+1);
      name+="_";
      name+=(j+1);
      trFS[j+planesSoFar]->SetName(name);
      trFS[j+planesSoFar]->RegisterYourself();

      name="tr_mylar";
      name+=(i+1);
      name+="_";
      name+=(j+1);
      trFS_mylar[j+planesSoFar]->SetName(name);
      trFS_mylar[j+planesSoFar]->RegisterYourself();

      name="tr_wo";
      name+=(i+1);
      name+="_";
      name+=(j+1);
      trFS_wo[j+planesSoFar]->SetName(name);
      trFS_wo[j+planesSoFar]->RegisterYourself();

      dchVol[i]->AddNode(planeVol[i],j+1,trFS[j+planesSoFar]);
      dchVol[i]->AddNode(mylarVol[i],j+1,trFS_mylar[j+planesSoFar]);
      dchVol[i]->AddNode(woVol[i],j+1,trFS_wo[j+planesSoFar]);
    }
  }


  for(Int_t i=0; i<10; i++){
    cout<<endl;
    for(Int_t j=0; j<10; j++){
      TString namemylar="tr_mylar";
      TString namewo="tr_wo";
      TString namech="tr";
      TString name;
      namech+=i;
      namemylar+=i;
      namewo+=i;
      name=namech;
      name+="_";
      namemylar+="_";
      namewo+="_";
      name+=j;
      namemylar+=j;
      namewo+=j;
      if(gGeoManager->GetListOfMatrices()->FindObject(name)){
	const Double_t* match = ((TGeoCombiTrans*)gGeoManager->GetListOfMatrices()->FindObject(namech))->GetTranslation();
	const Double_t* matpl = ((TGeoCombiTrans*)gGeoManager->GetListOfMatrices()->FindObject(name))->GetTranslation();
	const Double_t* matmylar = ((TGeoCombiTrans*)gGeoManager->GetListOfMatrices()->FindObject(namemylar))->GetTranslation();
	const Double_t* matwo = ((TGeoCombiTrans*)gGeoManager->GetListOfMatrices()->FindObject(namewo))->GetTranslation();
	Double_t z=match[2]+matpl[2];
	std::cout<<name<<"\t"<<z<<std::endl;
	z=match[2]+matmylar[2];
	std::cout<<namemylar<<"\t"<<z<<std::endl;
	z=match[2]+matwo[2];
	std::cout<<namewo<<"\t"<<z<<std::endl;
      }
    }
  }
  
//&&&&&&&&&&&&&&&&&&&&&&&&&&
  if(builtRICH){  
    FairGeoMedium *FairMediumAl = Media->getMedium("aluminium");
    FairGeoMedium *FairMediumAerogel = Media->getMedium("Aerogel");
    FairGeoMedium *FairMediumC4F10 = Media->getMedium("C4F10"); 
    nmed=geobuild->createMedium(FairMediumAl);
    nmed=geobuild->createMedium(FairMediumAerogel);
    nmed=geobuild->createMedium(FairMediumC4F10);
     
    const Double_t kRichPosition = kDchPosition[kNumOfChambers-2]+
      (kDchPosition[kNumOfChambers-1]-kDchPosition[kNumOfChambers-2])/2.;
    const Double_t kRichDx = kDchDx[kNumOfChambers-1]+20.;
    const Double_t kRichDy = kDchDy[kNumOfChambers-1]+20.;
    const Double_t kRichDz = (kDchPosition[kNumOfChambers-1]-kDchPosition[kNumOfChambers-2])/2.
      -kDchDz[kNumOfChambers-1];

    std::cout<<"Z half size of rich is "<<kRichDz<<std::endl;
    std::cout<<"Z position of rich is "<<kRichPosition<<std::endl;
    
    Double_t aeroHalfThickness = 4.0;
    
    cout<< "********* Forward Rich Stuff ***************************"<<endl;
    //define shape of the hole for beam pipe for RICH
    TGeoTube* pipeHoleShapeRich = new TGeoTube("pipeHoleShapeRich",0.,
					       kHoleRadius[kNumOfChambers-1],2.*kRichDz); 
    // define named geometrical transformations with names
    TGeoTranslation* trRich = new TGeoTranslation(0., 0., kRichPosition);
    trRich->SetName("trRich");
    trRich->RegisterYourself();
    TGeoTranslation* trRichAl1 = new TGeoTranslation(0., 0.,-kRichDz+0.05);
    trRichAl1->SetName("trRichAl1");
    trRichAl1->RegisterYourself();
    TGeoTranslation* trRichAl2 = new TGeoTranslation(0., 0.,kRichDz-0.05);
    trRichAl2->SetName("trRichAl2");
    trRichAl2->RegisterYourself();
    TGeoTranslation* trRichAero = new TGeoTranslation(0., 0.,-kRichDz+0.1+aeroHalfThickness);
    trRichAero->SetName("trRichAero");
    trRichAero->RegisterYourself();
    // define full shape of Rich Box and other shapes
    TGeoShape* RichBoxFullShape = new TGeoBBox("RichBoxFullShape",kRichDx,kRichDy,kRichDz); 
    TGeoShape* RichAlFullShape = new TGeoBBox("RichAlFullShape",kRichDx,kRichDy,0.05);
    TGeoShape* RichAeroFullShape = new TGeoBBox("RichAeroFullShape",kRichDx,kRichDy,aeroHalfThickness);
    // create shapes of with holes
    TGeoCompositeShape* richBoxShape = new TGeoCompositeShape("RichBoxShape","RichBoxFullShape-pipeHoleShapeRich");
    TGeoCompositeShape* richAlShape = new TGeoCompositeShape("RichAlShape","RichAlFullShape-pipeHoleShapeRich");
    TGeoCompositeShape* richAeroShape = new TGeoCompositeShape("RichAeroShape","RichAeroFullShape-pipeHoleShapeRich");
    // create physical volumes of chambers
    TGeoVolume* richBoxVol = new TGeoVolume("RichBoxVol", richBoxShape, gGeoMan->GetMedium("C4F10"));
    TGeoVolume* richAlVol = new TGeoVolume("RichAlVol", richAlShape, gGeoMan->GetMedium("aluminium"));
    TGeoVolume* richAeroVol = new TGeoVolume("RichAeroVol", richAeroShape, gGeoMan->GetMedium("Aerogel"));
    
    richBoxVol->AddNode(richAlVol,0,trRichAl1);
    richBoxVol->AddNode(richAlVol,1,trRichAl2);
    richBoxVol->AddNode(richAeroVol,0,trRichAero);
    
    top->AddNode(richBoxVol,0,trRich);
  }
  //&&&&&&&&&&&&&&&&&&&&&&&&&&

  
  //gGeoMan->CloseGeometry();
  TGeoVolumeAssembly *topA = TGeoVolumeAssembly::MakeAssemblyFromVolume(top);
  topA->SetName("dch");
  topA->Print();
  

  TGeoVolume *freshtop = gGeoMan->MakeBox("top",gGeoMan->GetMedium("air"),400,130,800);
  TGeoTranslation* trTopInCave = new TGeoTranslation(0,0,0);
  trTopInCave->SetName("trTopInCave");
  freshtop->AddNode(topA, 0, trTopInCave);

  TFile* fi = new TFile(outfile,"RECREATE");
  freshtop->Write();
  fi->Close();
  
   //  gGeoManager->SetName("dchGeom");
   //gGeoManager->Export(outfile);

  //top->Draw();
}  

