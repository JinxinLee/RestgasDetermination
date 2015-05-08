{
// Macro to create Fsc geometry file
// of the FSC detector for PANDA
// in root format
//30.03.2010 Dmitry Morozov

  // Fsc geometry parameters
  // in cm
  //-----------------------------
  Double_t     beamlineangle =  -TMath::DegToRad()*0.0; //in rad
  const Double_t     zpos          =  760.0;
  const Int_t        numcellsx     =  54;
  const Int_t        numcellsy     =  28;
  const Double_t     tyvekbundle   =  0.025;
  const Double_t     paperbundle   =  0.025;
  Double_t     crystalspace  =  tyvekbundle+paperbundle;
  const Double_t     Absorb_xsize  =  5.5;
  const Double_t     Absorb_ysize  =  5.5;
  const Double_t     Air_thickness =  0.0;
  const Double_t     Pb_thickness  =  0.0275;
  const Double_t     Sci_thickness =  0.15;
  const Double_t     Sci_xsize     =  5.5;
  const Double_t     Sci_ysize     =  5.5;
  const Int_t        EMCnumlayers  =  380;
  const Int_t        numfibers     =  6;
  const Double_t     fiber_r       =  0.06;
  const Double_t     fiber_hole_r  =  0.07;



  Double_t  cellxsize = TMath::Max(Absorb_xsize,Sci_xsize)+2.*crystalspace;
  Double_t  cellysize = TMath::Max(Absorb_ysize,Sci_ysize)+2.*crystalspace;
  Double_t  xsize     = cellxsize*numcellsx;
  Double_t  ysize     = cellysize*numcellsy;

  Double_t  cell_thickness = 2*Air_thickness+Pb_thickness+Sci_thickness;
  Double_t  thickness      = cell_thickness*EMCnumlayers + 2.*crystalspace;
  Double_t  holder_thickness = cell_thickness*EMCnumlayers;

  Double_t     hole_xsize    =  6.*cellxsize;
  Double_t     hole_ysize    =  6.*cellysize;
  //position of the hole means - coordinate of its center
  Double_t     hole_xpos     = 2.0*cellxsize; //Must be left to the beam line (check??)
  Double_t     hole_ypos     =  1.0*cellxsize;


  //--------------------------------------------------------------------
 //  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

   // Load this example libraries
   gSystem->Load("libGeoBase");
   gSystem->Load("libParBase");
   gSystem->Load("libBase");
   gSystem->Load("libPndData");
   gSystem->Load("libPassive");

   TString outfile= "emc_module5_fsc.root";
   TFile* fi = new TFile(outfile,"RECREATE");

   FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
   FairGeoInterface *geoFace = geoLoad->getGeoInterface();
//   geoFace->setMediaFile("../../../geometry/media_pnd.geo");
//   geoFace->setMediaFile("$VMCWORKDIR/geometry/media_pnd.geo");
   geoFace->setMediaFile("/home/freez/fairsoft_mar15/pandaroot/geometry/media_pnd.geo");
   geoFace->readMedia();
   geoFace->print();

   FairGeoMedia *Media =  geoFace->getMedia();
   FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

   FairGeoMedium *MediumAir  = Media->getMedium("air");
   FairGeoMedium *MediumTyvek = Media->getMedium("tyvek");
   FairGeoMedium *MediumCarbon  = Media->getMedium("carbon");
   FairGeoMedium *MediumLead = Media->getMedium("lead");
   FairGeoMedium *MediumScint = Media->getMedium("FscScint");
   FairGeoMedium *MediumFiber = Media->getMedium("FscFiber");

   Int_t nmed=geobuild->createMedium(MediumAir);
   nmed=geobuild->createMedium(MediumTyvek);
   nmed=geobuild->createMedium(MediumCarbon);
   nmed=geobuild->createMedium(MediumLead);
   nmed=geobuild->createMedium(MediumScint);
   nmed=geobuild->createMedium(MediumFiber);

   TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

   //building main container

   TGeoVolume *top = new TGeoVolumeAssembly("Emc5");

   gGeoMan->SetTopVolume(top);
//   TSrting name = "FscTopShape";
//
//   TGeoShape* TopShape = new TGeoBBox(name,xsize/2.,ysize/2.,thickness/2.);
//   TGeoVolume *TopVolume = new TGeoVolumeAssembly("FscBox");

   //Building paper volume to place everything inside it
   TString name = "FscModuleShape";
   TGeoShape* ModuleShape = new TGeoBBox(name,cellxsize/2.,cellysize/2.,thickness/2.);
   name = "FscModuleVolume";
   TString medium = "carbon";
   TGeoVolume* ModuleVolume = new TGeoVolume(name, ModuleShape, gGeoMan->GetMedium(medium));
   ModuleVolume->SetLineColor(2);

   //Building tyvek volume inside paper volume
   name = "FscTyvekShape";
   TGeoShape* TyvekShape = new TGeoBBox(name,cellxsize/2. - paperbundle,cellysize/2. - paperbundle,thickness/2. - paperbundle);
   name = "FscTyvekVolume";
   medium = "tyvek";
   TGeoVolume* TyvekVolume = new TGeoVolume(name, TyvekShape, gGeoMan->GetMedium(medium));
   TyvekVolume->SetLineColor(3);
   ModuleVolume->AddNode(TyvekVolume,0);

   //Building and placing holder (for Scint+Lead layer) volumes inside tyvek volume
   name = "FscLayerHolderShape";
   TGeoShape* LayerHolderShape = new TGeoBBox(name, cellxsize/2. - crystalspace, cellysize/2. - crystalspace, cell_thickness/2.);
   name = "FscLayerHolderVolume";
   medium = "Air";
 //  TGeoVolume* LayerHolderVolume = new TGeoVolume(name, LayerHolderShape, gGeoMan->GetMedium(medium));

   TGeoVolume* LayerHolderVolume = new TGeoVolumeAssembly(name);

   TGeoCombiTrans* iCombTrans = NULL;
   TGeoRotation rot;

      //Building and placing Lead + Scint layer inside holder Volume
      name = "FscLeadShape";
      TGeoShape* LeadShape = new TGeoBBox(name, cellxsize/2. - crystalspace, cellysize/2. - crystalspace, Pb_thickness/2.);
      name = "FscLeadVolume";
      medium = "lead";
      TGeoVolume* LeadVolume = new TGeoVolume(name, LeadShape, gGeoMan->GetMedium(medium));
      LeadVolume->SetLineColor(4);
      iCombTrans = new TGeoCombiTrans(0,0,-cell_thickness/2.+Pb_thickness/2., new TGeoRotation(rot));
      LayerHolderVolume->AddNode(LeadVolume,0,iCombTrans);

      name = "FscSciShape";
      TGeoShape* SciShape = new TGeoBBox(name, cellxsize/2. - crystalspace, cellysize/2. - crystalspace, Sci_thickness/2.);
      name = "FscSciVolume";
      medium = "FscScint";
      TGeoVolume* SciVolume = new TGeoVolume(name, SciShape, gGeoMan->GetMedium(medium));
      SciVolume->SetLineColor(5);
      iCombTrans = new TGeoCombiTrans(0,0,cell_thickness/2.-Sci_thickness/2., new TGeoRotation(rot));
      LayerHolderVolume->AddNode(SciVolume,0,iCombTrans);


   Double_t z;
//   TGeoCombiTrans* iCombTrans = NULL;
//   TGeoRotation rot;
   for(int iz = 1; iz <= EMCnumlayers; iz++)
   {
	   z = -holder_thickness/2. + 0.5 * cell_thickness *(2.*iz - 1.);
	   iCombTrans = new TGeoCombiTrans(0,0,z, new TGeoRotation(rot));
	   name = "FscLayerHolder";
	   name += iz-1;
	   TyvekVolume->AddNode(LayerHolderVolume,iz-1,iCombTrans);
   }

   Int_t iCopy = 0;
   Double_t x,y;

   //Building and placing Fibers inside FscModule Volume
   //Holes for fibers Bottom right corner is (ix = 1, iy = 1)
   name = "FscFibHoleSHape";
//   TGeoShape* FiberHoleShape = new TGeoTube(name, 0., fiber_hole_r, (thickness-crystalspace)/2.);
   TGeoShape* FiberHoleShape = new TGeoTube(name, 0., fiber_hole_r, (holder_thickness)/2.);
   name = "FscFibHoleVolume";
   medium = "air";
   TGeoVolume* FiberHoleVolume = new TGeoVolume(name, FiberHoleShape, gGeoMan->GetMedium(medium));
   for(int ix = 1; ix<= numfibers; ix++)
   {
	   for(int iy = 1; iy<= numfibers; iy++)
	   {
		   x = -Absorb_xsize/2. + (Absorb_xsize/(Double_t)numfibers)*(ix - 0.5);
		   y = -Absorb_ysize/2. + (Absorb_ysize/(Double_t)numfibers)*(iy - 0.5);
//		   iCombTrans = new TGeoCombiTrans(x,y,crystalspace, new TGeoRotation(rot));
		   iCombTrans = new TGeoCombiTrans(x,y,0, new TGeoRotation(rot));
		   name = "FscFibHoleVolume";
		   name+=iCopy;
		   iCombTrans->SetName(name);
		   iCombTrans->RegisterYourself();
//		   ModuleVolume->AddNode(FiberHoleVolume,iCopy,iCombTrans);
//		   ModuleVolume->AddNodeOverlap(FiberHoleVolume,iCopy,iCombTrans);
		   TyvekVolume->AddNodeOverlap(FiberHoleVolume,iCopy,iCombTrans);  //!!NO OVERLAPS
//		   TyvekVolume->AddNode(FiberHoleVolume,iCopy,iCombTrans);
		   iCopy++;
	   }
   }
   //FIbers itself inside holes
   name = "FscFiberShape";
//   TGeoShape* FiberShape = new TGeoTube(name, 0., fiber_r, (thickness-crystalspace)/2.);
   TGeoShape* FiberShape = new TGeoTube(name, 0., fiber_r, (holder_thickness)/2.);
   name = "FscFiberVolume";
   medium = "FscFiber";
   TGeoVolume* FiberVolume = new TGeoVolume(name, FiberShape, gGeoMan->GetMedium(medium));
   FiberVolume->SetLineColor(5);
   iCombTrans = new TGeoCombiTrans(0,0,0, new TGeoRotation(rot));
   FiberHoleVolume->AddNode(FiberVolume,0,iCombTrans);


   iCopy =0;
   Int_t CopyCode=0;
   // placing Modules of Carbon inside top FSC
   // Bottom right corner is (ix = 1, iy = 1)
   for(int ix = 1; ix<= numcellsx; ix++)
   {
	   for(int iy = 1; iy<= numcellsy; iy++)
	   {
		   x = 0.5 * cellxsize * (2. * ix - numcellsx - 1.);
		   y = 0.5 * cellysize * (2. * iy - numcellsy - 1.);
		   if(TMath::Abs(x-hole_xpos)<hole_xsize/2. && TMath::Abs(y-hole_ypos)<hole_ysize/2.) continue;
		   iCombTrans = new TGeoCombiTrans(x,y,0., new TGeoRotation(rot));
		   name = "FscModuleVolume";
		   name+=iCopy;
           CopyCode = 100*ix+iy;

		   iCombTrans->SetName(name);
		   iCombTrans->RegisterYourself();
//		   top->AddNode(ModuleVolume,iCopy,iCombTrans);
		   top->AddNode(ModuleVolume,CopyCode,iCombTrans);
		   iCopy++;

	   }
   }


   gGeoMan->CloseGeometry();
   top->Write();
   fi->Close();
//   gGeoManager->Export(outfile);//
   top->Draw("ogl");//

}
