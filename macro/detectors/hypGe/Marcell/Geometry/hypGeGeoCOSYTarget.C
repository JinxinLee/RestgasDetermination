#include "TGeoManager.h"
double Pi = TMath::Pi();


TString BuildOutputNameTarget(Double_t TargetThickness_Ext, Double_t Angle)
{
	TString outfileWithoutPath = "hypGeGeoCOSYTarget_";
	//char buf[10];
	//sprintf(buf,"%.2f",TargetThickness_Ext);
	//outfileWithoutPath += buf;
	outfileWithoutPath +=TargetThickness_Ext;
	//outfileWithoutPath +=
	outfileWithoutPath += "cm_";
	outfileWithoutPath += Angle;
	outfileWithoutPath += "_Degree.root";
	return outfileWithoutPath;
}

TGeoShape* makeMaytec(Double_t length,TString Name)
{
	char buf[100];
	sprintf(buf, "%sRaw",Name.Data());
	TGeoBBox *Raw = new TGeoBBox(buf,1.5,1.5,length/2);
	sprintf(buf, "%sHole",Name.Data());
	TGeoTube *Hole = new TGeoTube(buf,0,0.6, length);
	sprintf(buf, "%sTrapez",Name.Data());
	TGeoTrd1 *Trapez =  new TGeoTrd1(buf,0.45,0.75,length,0.25);
	sprintf(buf, "%sRectangle",Name.Data());
	TGeoBBox *Rectangle = new TGeoBBox(buf,0.4,0.4,length);
	sprintf(buf, "%sRectangle2",Name.Data());
	TGeoBBox *Rectangle2 = new TGeoBBox(buf,0.15,0.5,length);
	sprintf(buf, "%sRectangle3",Name.Data());
	TGeoBBox *Rectangle3 = new TGeoBBox(buf,0.1,0.1,length);
	sprintf(buf, "%sTube",Name.Data());
	TGeoTubeSeg *Tube =  new TGeoTubeSeg(buf, 0.2 , 1 ,length,0,90);
	
	TGeoCombiTrans *TrapezCombiTrans[4];
	TGeoCombiTrans *RectangleCombiTrans[4];
	TGeoCombiTrans *Rectangle2CombiTrans[4];
	TGeoCombiTrans *Rectangle3CombiTrans[4];
	TGeoCombiTrans *Rectangle3CombiTrans2[4];
	TGeoCombiTrans *TubeCombiTrans[4];
	
	sprintf(buf,"%sRaw - %sHole",Name.Data(),Name.Data());
	TString ShapeString = buf;
	
	// position and remove all the fine structure of a maytec profile
	for (int i = 0; i< 4; i++)
	{
		sprintf(buf,"TrapezCombiTrans%d",i);
		TrapezCombiTrans[i] = new TGeoCombiTrans(buf,1.05*TMath::Sin((i+1)*(TMath::Pi()/2)),1.05*TMath::Cos((i+1)*(TMath::Pi()/2)),0, new TGeoRotation("",90*(1-i),90,0) );			// sin for 4 fold symmetry 
		TrapezCombiTrans[i] ->RegisterYourself();
		sprintf(buf,"-%sTrapez :TrapezCombiTrans%d",Name.Data(),i);
		ShapeString += buf;
		sprintf(buf,"RectangleCombiTrans%d",i);
		RectangleCombiTrans[i] = new TGeoCombiTrans(buf,1.4*TMath::Sin((i+1)*(TMath::Pi()/2)),1.4*TMath::Cos((i+1)*(TMath::Pi()/2)),0, new TGeoRotation("",0,0,0) );
		RectangleCombiTrans[i]->RegisterYourself();
		sprintf(buf,"-%sRectangle :RectangleCombiTrans%d",Name.Data(),i);
		ShapeString += buf;
		sprintf(buf,"Rectangle2CombiTrans%d",i);
		Rectangle2CombiTrans[i] = new TGeoCombiTrans(buf,0.9*pow(-1,i/2),0.9*pow(-1,(i+1)/2),0, new TGeoRotation("",-45+i*90,0,0) );
		Rectangle2CombiTrans[i]->RegisterYourself();
		sprintf(buf,"-%sRectangle2 :Rectangle2CombiTrans%d",Name.Data(),i);
		ShapeString += buf;
		sprintf(buf,"Rectangle3CombiTrans%d",i);
		Rectangle3CombiTrans[i] = new TGeoCombiTrans(buf,0.9*pow(-1,i/2),1.58*pow(-1,(i+1)/2),0, new TGeoRotation("",0,0,0) );
		Rectangle3CombiTrans[i]->RegisterYourself();
		sprintf(buf,"-%sRectangle3 :Rectangle3CombiTrans%d",Name.Data(),i);
		ShapeString += buf;
		sprintf(buf,"Rectangle3CombiTrans2%d",i);
		Rectangle3CombiTrans2[i] = new TGeoCombiTrans(buf,1.58*pow(-1,(i+1)/2),0.9*pow(-1,i/2),0, new TGeoRotation("",0,0,0) );
		Rectangle3CombiTrans2[i]->RegisterYourself();
		sprintf(buf,"-%sRectangle3 :Rectangle3CombiTrans2%d",Name.Data(),i);
		ShapeString += buf;
		sprintf(buf,"TubeCombiTrans%d",i);
		TubeCombiTrans[i] = new TGeoCombiTrans(buf,1.3*pow(-1,(i+1)/2),1.3*pow(-1,i/2),0, new TGeoRotation("",i*90,0,0) );
		TubeCombiTrans[i]->RegisterYourself();
		sprintf(buf,"-%sTube :TubeCombiTrans%d",Name.Data(),i);
		ShapeString += buf;
	}
		
	TGeoCompositeShape *Profile = new TGeoCompositeShape(Name.Data(), ShapeString.Data());
	cout << ShapeString.Data() << endl;
	gDirectory->ls();
	
	
	return Profile;
}

TGeoVolume * makeMaytecFrame(TGeoMedium *Med_ext)
{
	cout << "frame start" << endl;
	TGeoVolumeAssembly *FrameAssembly= new TGeoVolumeAssembly("FrameAssembly");
	
		Double_t Height = 21.5;
		Double_t Depth = 18.1;
		Double_t Width = 24;
		Double_t KermitWidth = 5;
		
		TGeoVolume *Pillar = new TGeoVolume ("Pillar", makeMaytec(Height,"Pillar"),Med_ext);
		TGeoVolume *Pillar2 = new TGeoVolume ("Pillar2", makeMaytec(Height,"Pillar1"),Med_ext);
		TGeoVolume *Pillar3 = new TGeoVolume ("Pillar3", makeMaytec(Height,"Pillar2"),Med_ext);
		TGeoVolume *Pillar4 = new TGeoVolume ("Pillar4", makeMaytec(Height,"Pillar3"),Med_ext);
		Pillar ->SetFillColor(kBlue);
		Pillar ->SetFillColor(kBlue);
		Pillar2 ->SetFillColor(kBlue);
		Pillar2 ->SetFillColor(kBlue);
		Pillar3 ->SetFillColor(kBlue);
		Pillar3 ->SetFillColor(kBlue);
		Pillar4 ->SetFillColor(kBlue);
		Pillar4 ->SetFillColor(kBlue);
		TGeoVolume *Beam1 = new TGeoVolume ("Beam1", makeMaytec(Width,"Beam1"),Med_ext);
		TGeoVolume *Beam12 = new TGeoVolume ("Beam12", makeMaytec(Width,"Beam12"),Med_ext);
		Beam1->SetFillColor(14);
		Beam1 ->SetFillColor(14);
		Beam12->SetFillColor(14);
		Beam12 ->SetFillColor(14);
		TGeoVolume *Beam2 = new TGeoVolume ("Beam2", makeMaytec(Depth,"Beam2"),Med_ext);
		TGeoVolume *Beam22 = new TGeoVolume ("Beam22", makeMaytec(Depth,"Beam22"),Med_ext);
		Beam2->SetFillColor(14);
		Beam2 ->SetFillColor(14);
		Beam22->SetFillColor(14);
		Beam22 ->SetFillColor(14);
	FrameAssembly ->AddNode(Pillar,1, new TGeoCombiTrans(Height/2,1.5,Width/2-1.5, new TGeoRotation("",90,90,0)));
	FrameAssembly ->AddNode(Pillar2,2, new TGeoCombiTrans(Height/2,1.5,-(Width/2-1.5), new TGeoRotation("",90,90,0)));
	FrameAssembly ->AddNode(Pillar3,3, new TGeoCombiTrans(Height/2,1.5+Depth+3,-(Width/2-1.5), new TGeoRotation("",90,90,0)));
	FrameAssembly ->AddNode(Pillar4,4, new TGeoCombiTrans(Height/2,1.5+Depth+3,(Width/2-1.5), new TGeoRotation("",90,90,0)));
	FrameAssembly ->AddNode(Beam1,1, new TGeoCombiTrans(Height+1.5,1.5,0, new TGeoRotation("",0,0,0)));
	FrameAssembly ->AddNode(Beam12,2, new TGeoCombiTrans(Height+1.5,1.5+Depth+3,0, new TGeoRotation("",0,0,0)));
	FrameAssembly ->AddNode(Beam2,1, new TGeoCombiTrans(Height+1.5,1.5+Depth/2+1.5,(KermitWidth/2+1.5), new TGeoRotation("",0,90,0)));
	FrameAssembly ->AddNode(Beam22,2, new TGeoCombiTrans(Height+1.5,1.5+Depth/2+1.5,-(KermitWidth/2+1.5), new TGeoRotation("",0,90,0)));

	
	cout << "frame built" << endl;
	return FrameAssembly;
}
TGeoVolume *makeGlueStackFrame(TGeoMedium *Al_ext,TGeoMedium *Steel_ext)
{
	// (0,0,0) on the front of the front plate of the stack!
	Double_t BarLength = 6;
	
	TGeoBBox *GlueSampleFrameFrontBox = new TGeoBBox("GlueSampleFrameFrontBox",4,4,0.2);
	//TGeoVolume *GlueSampleFrameFront = new TGeoVolume("GlueSampleFrameFront", GlueSampleFrontBox,Med_ext);
	
	TGeoTranslation *GlueSampleFrontTrans = new TGeoTranslation("GlueSampleFrontTrans",0,0,-0.2);
	GlueSampleFrontTrans->RegisterYourself();
	TGeoTranslation *GlueSampleBackTrans = new TGeoTranslation("GlueSampleBackTrans",0,0,-(0.4+BarLength+0.2));
	GlueSampleBackTrans->RegisterYourself();
	
	TGeoBBox *GlueSampleFrameBarBox = new TGeoBBox("GlueSampleFrameBarBox",.4,.4,BarLength/2);
	//TGeoVolume *GlueSampleFrameBar = new TGeoVolume("GlueSampleFrameBar", GlueSampleFrontBox,Med_ext);
	
	TGeoTranslation *GlueSampleFrameBarTrans1 = new TGeoTranslation("GlueSampleFrameBarTrans1",1.4+0.4,1.4+0.4,-(BarLength/2+0.4));
	GlueSampleFrameBarTrans1->RegisterYourself();
	TGeoTranslation *GlueSampleFrameBarTrans2 = new TGeoTranslation("GlueSampleFrameBarTrans2",-(1.4+0.4),1.4+0.4,-(BarLength/2+0.4));
	GlueSampleFrameBarTrans2->RegisterYourself();
	TGeoTranslation *GlueSampleFrameBarTrans3 = new TGeoTranslation("GlueSampleFrameBarTrans3",-(1.4+0.4),-(1.4+0.4),-(BarLength/2+0.4));
	GlueSampleFrameBarTrans3->RegisterYourself();
	TGeoTranslation *GlueSampleFrameBarTrans4 = new TGeoTranslation("GlueSampleFrameBarTrans4",1.4+0.4,-(1.4+0.4),-(BarLength/2+0.4));
	GlueSampleFrameBarTrans4->RegisterYourself();
	
	TString ShapeString = "GlueSampleFrameFrontBox : GlueSampleFrontTrans + GlueSampleFrameFrontBox : GlueSampleBackTrans + GlueSampleFrameBarBox : GlueSampleFrameBarTrans1 + GlueSampleFrameBarBox : GlueSampleFrameBarTrans2 + GlueSampleFrameBarBox : GlueSampleFrameBarTrans3 + GlueSampleFrameBarBox : GlueSampleFrameBarTrans4";
	TGeoCompositeShape *GlueStackFrameShape = new TGeoCompositeShape("GlueStackFrameShape", ShapeString.Data());
	TGeoVolume *GlueSampleFrame = new TGeoVolume("GlueSampleFrame", GlueStackFrameShape);
	return GlueSampleFrame;
}

TGeoVolume *makeGlueSample(TGeoMedium *Al_ext,TGeoMedium *Glue_ext, Int_t SampleNumber )
{
	// one glue sample sandwich is constructed in this function
	
	Double_t HolderThickness = 0.1; // 1 mm
	Double_t GlueThickness = 0.003; // 40 µm
	char buf[100];
	
	sprintf (buf,"GlueSampleAssembly%d", SampleNumber);
	TGeoVolume *SampleAssembly = new TGeoVolumeAssembly (buf);
	
	sprintf (buf,"GlueHolderBox%d", SampleNumber);
	TGeoBBox *GlueHolderBox = new TGeoBBox(buf,7.8/2,2.6/2,HolderThickness/2);
	sprintf (buf,"GlueSampleBox%d", SampleNumber);
	TGeoBBox *GlueBox = new TGeoBBox(buf,7.8/2,2.6/2,GlueThickness/2);
	
	sprintf (buf,"GlueHolder%d", SampleNumber);
	TGeoVolume *GlueHolder = new TGeoVolume (buf, GlueHolderBox, Al_ext);
	
	sprintf (buf,"GlueSampleCrystal%d", SampleNumber);
	TGeoVolume *GlueSampleCrystal = new TGeoVolume (buf, GlueBox, Glue_ext);
	
	sprintf (buf,"GlueHolderTrans1%d", SampleNumber);
	TGeoTranslation *GlueHolderTrans1 = new TGeoTranslation(buf,0,0,-HolderThickness/2);
	GlueHolderTrans1->RegisterYourself();
	sprintf (buf,"GlueHolderTrans2%d", SampleNumber);
	TGeoTranslation *GlueHolderTrans2 = new TGeoTranslation(buf,0,0,-(HolderThickness+GlueThickness +HolderThickness/2));
	GlueHolderTrans2->RegisterYourself();
	sprintf (buf,"GlueTransd", SampleNumber);
	TGeoTranslation *GlueTrans = new TGeoTranslation(buf,0,0,-(HolderThickness+GlueThickness/2));
	GlueTrans->RegisterYourself();
	
	SampleAssembly->AddNode(GlueHolder,0,GlueHolderTrans1);
	SampleAssembly->AddNode(GlueHolder,1,GlueHolderTrans2);
	SampleAssembly->AddNode(GlueSampleCrystal,0,GlueTrans);
	
	return SampleAssembly;
	
}

TString hypGeGeoCOSYTarget(Double_t TargetThickness_Ext = 5, Double_t Angle = 0)
{
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  gSystem->Load("libHypGe");
	
	cout << TargetThickness_Ext;
	TString outfileWithoutPath =BuildOutputNameTarget(TargetThickness_Ext, Angle); //"hypGeGeoCOSYTarget_";
	////char buf[10];
	////sprintf(buf,"%.2f",TargetThickness_Ext);
	////outfileWithoutPath += buf;
	//outfileWithoutPath +=TargetThickness_Ext;
	////outfileWithoutPath +=
	//outfileWithoutPath += "cm_";
	//outfileWithoutPath += Angle;
	//outfileWithoutPath += "_Degree.root";
	TString outfile= "../../../../geometry/";
	outfile += outfileWithoutPath;

	cout << outfile << endl;

  TFile* fi = new TFile(outfile,"RECREATE");

 FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  //geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medcarbon  = Media->getMedium("carbon");
  Int_t nmedcap=geobuild->createMedium(medcarbon);
	TGeoMedium *Carbon = gGeoManager->GetMedium("carbon");

	FairGeoMedium *medcap  = Media->getMedium("HYPaluminium");
  Int_t nmedcap=geobuild->createMedium(medcap);
  TGeoMedium *Al = gGeoManager->GetMedium("HYPaluminium");

	FairGeoMedium *medGe  = Media->getMedium("germanium");
  Int_t nmedGe=geobuild->createMedium(medGe);
  TGeoMedium *Ge = gGeoManager->GetMedium("germanium");

  FairGeoMedium *medsteel  = Media->getMedium("steel");
  Int_t nmedcap=geobuild->createMedium(medsteel);
	TGeoMedium *Steel = gGeoManager->GetMedium("steel");

	FairGeoMedium *medcu  = Media->getMedium("copper");
  Int_t nmedcap=geobuild->createMedium(medcu);
	TGeoMedium *Cu = gGeoManager->GetMedium("copper");
	
	FairGeoMedium *medpe_bor5  = Media->getMedium("pe_bor5");
  Int_t nmedcap=geobuild->createMedium(medcu);
	TGeoMedium *pe_bor5 = gGeoManager->GetMedium("pe_bor5");	// polyethylen with 5 % boronoxide
	
  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *Realtop = new TGeoVolumeAssembly("Realtop");
	   
//cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);

	// carbon target
	Double_t TargetThickness = TargetThickness_Ext;
	TGeoVolume* Target = geom->MakeBox("Target",Carbon,2.4,2.4,TargetThickness/2);
	Target->SetLineColor(kRed);
	Target->SetFillColor(kRed);
	TGeoRotation *TargetRot = new TGeoRotation("TargetRot",0,Angle,0);
			//Realtop->AddNode(Target,1,new TGeoCombiTrans(0,0,TargetThickness/2/TMath::Cos(Angle*Pi/180),TargetRot));
	cout << TargetThickness/2/TMath::Cos(Angle*Pi/180) << endl;
	TGeoVolume* TargetAddition = geom->MakeBox("TargetAddition",Carbon,0.5,2.4,2.4);
	TargetAddition->SetLineColor(kRed);
	TargetAddition->SetFillColor(kRed);
			//Realtop->AddNode(TargetAddition,1,new TGeoCombiTrans(-2.9,0,-0.1+TargetThickness/2/TMath::Cos(Angle*Pi/180),TargetRot));
			//Realtop->AddNode(TargetAddition,2,new TGeoCombiTrans(-3.9,0,-0.1+TargetThickness/2/TMath::Cos(Angle*Pi/180),TargetRot));
	
	// Al support
	TGeoVolume *AlSupportAssembly = new TGeoVolumeAssembly("AlSupportAssembly"); 
		// plate under target 
	TGeoTube *AlPlateTube = new TGeoTube("AlPlateTube",0,6,0.2);
		TGeoRotation *AlPlateTubeRotation = new TGeoRotation ("AlPlateTubeRotation",90,90,0);
		AlPlateTubeRotation->RegisterYourself();
	TGeoBBox *AlPlateBox = new TGeoBBox("AlPlateBox",0.2,6,11.6/2);
		TGeoTranslation *AlPlateBoxTranslation = new TGeoTranslation ("AlPlateBoxTranslation",0,0,+5.8);
		AlPlateBoxTranslation->RegisterYourself();
	TGeoCompositeShape *AlPlateShape = new TGeoCompositeShape("AlPlateShape", "AlPlateTube : AlPlateTubeRotation +  AlPlateBox : AlPlateBoxTranslation");
	TGeoVolume *AlPlate = new TGeoVolume("AlPlate", AlPlateShape, Al);
	AlPlate->SetLineColor(14);
	AlPlate->SetFillColor(14);
	
		// angle bracket in front of target
	TGeoBBox *AlFrontAngleTop = new TGeoBBox("AlFrontAngleTop",0.5,2.5,0.1);
		TGeoTranslation *AlFrontAngleTopTranslation = new TGeoTranslation ("AlFrontAngleTopTranslation",+0.2+0.5,0,-0.1);
		AlFrontAngleTopTranslation->RegisterYourself();
	TGeoBBox *AlFrontAngleBottom = new TGeoBBox("AlFrontAngleBottom",0.1,2.5,1.5);
		TGeoTranslation *AlFrontAngleBottomTranslation = new TGeoTranslation ("AlFrontAngleBottomTranslation",+0.3,0,-1.5);
		AlFrontAngleBottomTranslation->RegisterYourself();
	TGeoBBox *AlFrontAngleBottomCutout = new TGeoBBox("AlFrontAngleBottomCutout",1,0.25,1.5);
		TGeoTranslation *AlFrontAngleBottomCutoutTranslation = new TGeoTranslation ("AlFrontAngleBottomCutoutTranslation",+0.3,0,-2.5);
		AlFrontAngleBottomCutoutTranslation->RegisterYourself();
	TGeoCompositeShape *AlFrontAngleShape = new TGeoCompositeShape("AlFrontAngleShape", "(AlFrontAngleTop : AlFrontAngleTopTranslation +  AlFrontAngleBottom : AlFrontAngleBottomTranslation ) - ( AlFrontAngleBottomCutout : AlFrontAngleBottomCutoutTranslation)");
	TGeoVolume *AlFrontAngle = new TGeoVolume("AlFrontAngle", AlFrontAngleShape, Al);
	AlFrontAngle ->SetLineColor(14);
	AlFrontAngle ->SetFillColor(14);
	
		// angle bracket in the back of target
	TGeoBBox *AlBackAngleTop = new TGeoBBox("AlBackAngleTop",0.5,2.5,0.1);
		TGeoTranslation *AlBackAngleTopTranslation = new TGeoTranslation ("AlBackAngleTopTranslation",+0.2+0.5,0,4.9);
		AlBackAngleTopTranslation->RegisterYourself();
	TGeoBBox *AlBackAngleBottom = new TGeoBBox("AlBackAngleBottom",0.1,2.5,5);
		TGeoTranslation *AlBackAngleBottomTranslation = new TGeoTranslation ("AlBackAngleBottomTranslation",+0.3,0,4.8+5);
		AlBackAngleBottomTranslation->RegisterYourself();
	TGeoBBox *AlBackAngleBottomCutout = new TGeoBBox("AlBackAngleBottomCutout",1,0.25,5);
		TGeoTranslation *AlBackAngleBottomCutoutTranslation = new TGeoTranslation ("AlBackAngleBottomCutoutTranslation",+0.3,0,4.8+7);
		AlBackAngleBottomCutoutTranslation->RegisterYourself();
	TGeoCompositeShape *AlBackAngleShape = new TGeoCompositeShape("AlBackAngleShape", "(AlBackAngleTop : AlBackAngleTopTranslation +  AlBackAngleBottom : AlBackAngleBottomTranslation ) - ( AlBackAngleBottomCutout : AlBackAngleBottomCutoutTranslation)");
	TGeoVolume *AlBackAngle = new TGeoVolume("AlBackAngle", AlBackAngleShape, Al);
	AlBackAngle ->SetLineColor(14);
	AlBackAngle ->SetFillColor(14);
	
		// add Al goniometer
	TGeoTube *AlGoniometerTube = new TGeoTube("AlGoniometerTube",5,6,2);
		TGeoCombiTrans *AlGoniometerTubeCombiTrans = new TGeoCombiTrans ("AlGoniometerTubeCombiTrans",-2.2,0,0, new TGeoRotation("",90,90,0));
		AlGoniometerTubeCombiTrans->RegisterYourself();
	TGeoVolume *AlGoniometer = new TGeoVolume("AlGoniometer",AlGoniometerTube,Al);
	AlGoniometer ->SetLineColor(14);
	AlGoniometer ->SetFillColor(14);
	
	//screw head close to the beam
	
	TGeoTube *M5ScrewHeadOut = new TGeoTube("M5ScrewHeadOut",0.45/2,0.4,0.25);
	TGeoTube *M5ScrewHeadIn = new TGeoTube("M5ScrewHeadIn",0,0.45/2,0.25/2);
	TGeoTranslation *M5ScrewHeadInTrans = new TGeoTranslation("M5ScrewHeadInTrans",0,0,0.25/2);
	M5ScrewHeadInTrans -> RegisterYourself();
	TGeoCompositeShape *M5ScrewHeadShape = new TGeoCompositeShape("M5SrewHeadShape","M5ScrewHeadOut + M5ScrewHeadIn : M5ScrewHeadInTrans");
	TGeoVolume *M5ScrewHead = new TGeoVolume("M5ScrewHead",M5ScrewHeadShape,Steel);
	TGeoCombiTrans *M5ScrewHeadCombiTrans = new TGeoCombiTrans ("M5ScrewHeadCombiTrans",+0.4+0.25,0,8, new TGeoRotation("",90,270,0));
		M5ScrewHeadCombiTrans->RegisterYourself();
	TGeoCombiTrans *M5ScrewHead2CombiTrans = new TGeoCombiTrans ("M5ScrewHead2CombiTrans",+0.4+0.25,0,-2, new TGeoRotation("",90,270,0));
		M5ScrewHead2CombiTrans->RegisterYourself();	
	M5ScrewHead->SetLineColor(36);
	M5ScrewHead->SetFillColor(36);
	
	AlSupportAssembly->AddNode(M5ScrewHead,1, M5ScrewHeadCombiTrans);
	AlSupportAssembly->AddNode(M5ScrewHead,2, M5ScrewHead2CombiTrans);
	
	// add Al base plate
	
	TGeoBBox *AlBasePlateBox = new TGeoBBox("AlBasePlateBox",0.4,30,40);
		TGeoCombiTrans *AlBasePlateCombiTrans = new TGeoCombiTrans ("AlBasePlateCombiTrans",-4.6,25,-25, new TGeoRotation("",0,60,0));
		AlBasePlateCombiTrans->RegisterYourself();
	TGeoVolume *AlBasePlate = new TGeoVolume("AlBasePlate",AlBasePlateBox,Al);
	AlBasePlate ->SetLineColor(14);
	AlBasePlate ->SetFillColor(14);
	
	// add all Al parts to assembly
	AlSupportAssembly->AddNode(AlBasePlate,1, AlBasePlateCombiTrans);
	AlSupportAssembly->AddNode(AlGoniometer,1, AlGoniometerTubeCombiTrans);
	AlSupportAssembly->AddNode(AlFrontAngle,1);
	AlSupportAssembly->AddNode(AlBackAngle,1);
	AlSupportAssembly->AddNode(AlPlate,1);
	
	TGeoVolume *Frame = makeMaytecFrame(Al);
	AlSupportAssembly ->AddNode(Frame,1, new TGeoCombiTrans(-4.2,15*TMath::Sin(TMath::Pi()/180*60),-15*TMath::Cos(TMath::Pi()/180*60), new TGeoRotation("",0,-30,0)));
	
			//Realtop->AddNode(AlSupportAssembly,1, new TGeoTranslation (-4.6,0,0));	
	
	
	
	 
	
	
	
	
	// neutron detectors
		
	TGeoVolume *NeutronDetectorAssembly = new TGeoVolumeAssembly("NeutronDetectorAssembly");
		// electronic neutron dosimeter (Kermit)
	TGeoTube *KermitTube= new TGeoTube("KermitTube",0,23.5/2,26.4/2);
	TGeoVolume *KermitVolume = new TGeoVolume("KermitCrystal",KermitTube,pe_bor5);
	TGeoCombiTrans *KermitCombiTrans = new TGeoCombiTrans ("KermitHeadCombiTrans",24.5+23.5/2,15+(26.4/2)*TMath::Sin(TMath::Pi()/180*60),-(15+26.4/2)*TMath::Cos(TMath::Pi()/180*60), new TGeoRotation("",0,60,0));
		KermitCombiTrans->RegisterYourself();
	NeutronDetectorAssembly ->AddNode(KermitVolume,1, KermitCombiTrans);		// check x position of kermit, seems too high, but need better connection for this...
	
		// passive neutron ball
	TGeoSphere *NeutronBallSphere = new TGeoSphere("NeutronBallSphere",0,32/2,0,180,0,360);
	TGeoVolume *NeutronBall = new TGeoVolume ("NeutronBallCrystal", NeutronBallSphere,pe_bor5);
	TGeoCombiTrans *NeutronBallCombiTrans = new TGeoCombiTrans ("NeutronBallHeadCombiTrans",20,21.5+32/2,-54, new TGeoRotation("",0,0,0));
		NeutronBallCombiTrans->RegisterYourself();
	NeutronDetectorAssembly ->AddNode(NeutronBall,1, NeutronBallCombiTrans);
	
			//Realtop->AddNode(NeutronDetectorAssembly,1, new TGeoTranslation (-4.6,0,0));
	
	//germanium
	
	PndGeoHypGeSingleCluster *SingleCluster = new PndGeoHypGeSingleCluster(Ge,Al,Cu,1);
	Int_t CrystalNumber = 1;
			//SingleCluster ->PlaceCluster(Realtop,  0,15, 0,60,0,&CrystalNumber);
	
	// stack of glue samples
		// first the frame
	
	TGeoVolume *GlueStackAssembly = new TGeoVolumeAssembly("GlueStackAssembly");
	TGeoVolume *GlueStackFrame = makeGlueStackFrame(Al,Steel);
	
	GlueStackAssembly->AddNode(GlueStackFrame,1);
	
	TGeoVolume *GlueSamples[23];
	//TGeoCombiTrans GlueSampleTrans[23];
	
	for (int i= 0; i < 1; i++)
	{
		GlueSamples[i] = makeGlueSample(Al, Steel,i);
		//GlueSampleTrans[i] = new TGeoCombiTrans (0,0,0.4 + i*(1), new TGeoRotation(0,0,0));
		GlueStackAssembly->AddNode(GlueSamples[i],i, new TGeoCombiTrans (0,0,-(0.4 + (i+0.5)*(0.203)), new TGeoRotation("",0,0,0)));
	}
	Realtop->AddNode(GlueStackAssembly,1, new TGeoTranslation (0,0,-100));
	top->AddNode(Realtop,0);
	
	
	geom->CloseGeometry();	
	cout << "# of Nodes: " << Realtop->CountNodes(10,1) << endl;
	

	top->Write();
	fi->Close();
	cout << "Wrote target geometry to file: " << outfileWithoutPath << endl;
	Realtop->Draw("ogl");
	


	
	
	
	return outfileWithoutPath; // returns the name of the created geometry file
}


