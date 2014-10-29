#include "TGeoManager.h"
double Pi = TMath::Pi();

void ClusterPlacer(double x, double y, double z, double GlobalZOffset , int ObjectNo, TGeoVolume *top,TGeoVolume *Clusterr,double phi, double theta, double psi)
{
TGeoRotation *Clusterrot = new TGeoRotation("Clusterrot",phi, theta,psi-phi);
		Clusterrot->RegisterYourself();
top->AddNode(Clusterr,ObjectNo,new TGeoCombiTrans(x,y,GlobalZOffset+z,Clusterrot));
}

void hypGe_TripleGeo()
{

gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
	TString outfile= "../../../../geometry/hypGe_GeoMarcell.root";

  TFile* fi = new TFile(outfile,"RECREATE");

	

 FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medGe  = Media->getMedium("germanium");
  Int_t nmedGe=geobuild->createMedium(medGe);
    
  // aluminum capsule
  FairGeoMedium *medcap  = Media->getMedium("HYPaluminium");
  Int_t nmedcap=geobuild->createMedium(medcap);

	TGeoMedium *Ge = gGeoManager->GetMedium("germanium");
  
  //capsule
  TGeoMedium *Al_sol = gGeoManager->GetMedium("HYPaluminium");


/*TGeoElementTable *table = gGeoManager->GetElementTable();
TGeoElement *Alum = table->GetElement(13);
TGeoElement *Germ = table->GetElement(32);
	//Materials
	TGeoMaterial *mat = new TGeoMaterial("Vacuum",0,0,0);
   TGeoMaterial *Ger = new TGeoMaterial("Ger",Germ,5.3);
	TGeoMaterial *Alu = new TGeoMaterial("Alu",Alum,2.7);
	TGeoMaterial *Alusol = new TGeoMaterial("Alu",Alum,2.7);	
	//Media
	TGeoMedium *vac = new TGeoMedium("Vacuum",0,mat);
	TGeoMedium *Ge = new TGeoMedium("Ge",1,Ger);
	TGeoMedium *Al = new TGeoMedium("Al",2,Alu);
	TGeoMedium *Al_sol = new TGeoMedium("Alsol",2,Alusol);

//Germ->Print();*/

  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *strange = new TGeoVolumeAssembly("strange");
	   
cout<<" geom "<<geom<<endl;           
  geom->SetTopVolume(top);
	
	Double_t GlobalZOffset = -55;
	int ObjektNr = 0;
	//Strahlrohr
	TGeoVolume *Strahlrohr =geom->MakeTube("Strahlrohr",Al_sol,0,7.5,150);
	Strahlrohr->SetLineColor(kBlue);
	//top->AddNode(Strahlrohr,ObjektNr);
	ObjektNr++;
	//Außenhülle
	TGeoVolume *Außenhülle =geom->MakeTube("Außenhülle",Al_sol,45,45.5,100);
	Außenhülle->SetLineColor(kBlue);
	//top->AddNode(Außenhülle,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;

//Halteplatte
	TGeoVolume *HoldingStruc =geom->MakeBox("HoldingStruc",Al_sol,1,45,50);
	HoldingStruc->SetLineColor(kGreen);
	//top->AddNode(HoldingStruc,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;
//Hilfskugel
	TGeoSphere *Kugel = new TGeoSphere("Kugel",1,30,0,180,0,360);
	TGeoVolume *Kugell = new TGeoVolume("Kugell",Kugel,Al_sol);
	Kugell->SetLineColor(kBlue);
	Kugell->SetTransparency(50);
	//top->AddNode(Kugell,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;
	
//Nullpunktmarker
	TGeoSphere *Origin = new TGeoSphere("Origin",0,1,0,180,0,360);
	TGeoVolume *Originn = new TGeoVolume("Originn",Origin,Al_sol);
	Originn->SetLineColor(kGreen);
	//top->AddNode(Originn,ObjektNr);
	ObjektNr++;
//Kappe
	//Außen
	double orangexy[16]={5.1211,-3.9906 , 2.5436,-7.0666 , -1.40290,-6.3389 , -2.5693,-3.0401 , 6.5119,-5.1633 , 3.2569,-9.0480 , -1.7273,-8.1289 , -3.2443,-3.8387};
	double gelbxy[16]={-2.5693,-3.0401 , -6.0164,-2.4697 , -7.3916,1.3304 , -4.7881,4.3843 , -3.2443,-3.8387 , -7.7274,-3.0578 ,  -9.4641,1.7035 , -6.1761,5.5903};	
	double braunxy[16]={5.1211,-3.9906 , -2.5693,-3.0401 , -4.7881,4.3843 , -1.3481,3.7451 , 6.5119,-5.1633 , -3.2443,-3.8387 , -6.1761,5.5603 , -1.7021,4.7290};
	double grauxy[16]={5.1211,-3.9906 , -1.3481,3.7451 , 0.8954,6.4301 , 3.9175,-0.7051 , 6.5119,-5.1633 , -1.7021,4.7290 , 1.2156,8.2210 , 4.9466,-0.8905};
	double gruenxy[16]={0.8954,6.4301 , 4.8480,5.7361 , 6.1610,1.9544 ,  3.9175,-0.7051 , 1.2156,8.2210 , 6.2074,7.3444 , 7.9036,2.5685 , 4.9466,-0.8905};
	
	TGeoArb8 *orange = new TGeoArb8 ("orange",12.95/2, orangexy  );
	TGeoArb8 *gelb = new TGeoArb8 ("gelb",12.95/2, gelbxy  );
	TGeoArb8 *braun = new TGeoArb8 ("braun",12.95/2, braunxy  );
	TGeoArb8 *grau = new TGeoArb8 ("grau",12.95/2, grauxy  );
	TGeoArb8 *gruen = new TGeoArb8 ("gruen",12.95/2, gruenxy  );

	//Innen
	double orangeInxy[16]={-2.4572,-2.9075 , -1.2905,-6.2072 , 2.4849,-6.9033 , 4.9505,-3.9607 , -3.1322,-3.7060 , -1.6148,-7.9972 , 3.1981,-8.8846 , 6.3412,-5.1334}; 
	double gelbInxy[16]={-2.4572,-2.9075 , -4.7303,4.2211 , -7.2208,1.2997 , -5.9053,-2.3069 , -3.1322,-3.7060 , -6.1183,5.3971 , -9.2933,1.6727 , -7.6162,-2.9250};
	double braunInxy[16]={-2.4572,-2.9075 , 4.9505,-3.9607 , -1.2893,3.5817 , -4.7303,4.2211 , -3.1322,-3.7060 , 6.3412,-5.1334 , -1.6433,4.5655 , -6.1183,5.3971};
	double grauInxy[16]={4.9505,-3.9607 , 3.7466,-0.4744 , 0.9549,6.2675 , -1.2893,3.5817 , 6.3412,-5.1334 , 4.7756,-0.8597 , 1.2751,8.0583 , -1.6433,4.5655};
	double gruenInxy[16]={3.7466,-0.4744 , 6.0208,1.9859 , 4.7360,5.6035 , 0.9549,6.2675 , 4.7756,-0.8597 , 7.7332,2.6000 , 6.0953,7.2118 , 1.2751,8.0583};

	TGeoTranslation *In_Z = new TGeoTranslation("In_Z",0,0,0.15);
	In_Z->RegisterYourself();
	TGeoArb8 *orangeIn = new TGeoArb8 ("orangeIn",(12.95-0.15)/2, orangeInxy  );
	TGeoArb8 *gelbIn = new TGeoArb8 ("gelbIn",(12.95-0.15)/2, gelbInxy  );
	TGeoArb8 *braunIn = new TGeoArb8 ("braunIn",(12.95-0.15)/2, braunInxy  );
	TGeoArb8 *grauIn = new TGeoArb8 ("grauIn",(12.95-0.15)/2, grauInxy  );
	TGeoArb8 *gruenIn = new TGeoArb8 ("gruenIn",(12.95-0.15)/2, gruenInxy  );
		

//UpperPart
	double up1_2xy[16]={6.5119,-5.1633 , 3.2569,-9.0480 , 3.1981,-8.8846 , 6.3412,-5.1334 ,
 6.9097,-5.2329 , 3.3939,-9.4288 , 3.1981,-8.8846 , 6.3412,-5.1334};
	double up2_3xy[16]={3.2569,-9.0480 , -1.7273,-8.1289 , -1.6148,-7.9972 , 3.1981,-8.8846 ,
 3.3939,-9.4288 , -1.9896,-8.4361 , -1.6148,-7.9972 , 3.1981,-8.8846};
	double up3_4xy[16]={-1.7273,-8.1289 , -3.2443,-3.8387 , -3.1322,-3.7060 ,  -1.6148,-7.9972 ,
-1.9896,-8.4361 , -3.5050,-4.1481 , -3.1322,-3.7060 ,  -1.6148,-7.9972};
	double up4_5xy[16]={-3.2443,-3.8387 , -7.7274,-3.0578 , -7.6162,-2.9250 , 
-3.1322,-3.7060 ,
-3.5050,-4.1481 , -7.9866,-3.3866 , -7.6162,-2.9250 , -3.1322,-3.7060};
	double up5_6xy[16]={-7.7274,-3.0578 , -9.4641,1.7035 , -9.2933,1.6727 ,  -7.6162,-2.9250 , -7.9866,-3.3866 , -9.8624,1.7752 , -9.2933,1.6727 ,-7.6162,-2.9250 };
	double up6_7xy[16]={-9.4641,1.7035 , -6.1761,5.5603 , -6.1183,5.3971 , -9.2933,1.6727 , -9.8624,1.7752 , -6.3111,5.9410 , -6.1183,5.3971 , -9.2933,1.6727};
	double up7_8xy[16]={-6.1761,5.5603 , -1.7021,4.7290 , -1.6433,4.5655 , -6.1183,5.3971 , -6.3111,5.9410 , -1.8393,5.1101 , -1.6433,4.5655 , -6.1183,5.3971};
	double up8_9xy[16]={-1.7021,4.7290 , 1.2156,8.2210 , 1.2751,8.0583 ,  -1.6433,4.5655 , -1.8393,5.1101 , 1.0770,8.6003 , 1.2751,8.0583 , -1.6433,4.5655};
	double up9_10xy[16]={1.2156,8.2210 , 6.2074,7.3444 , 6.0953,7.2118 ,  1.2751,8.0583 , 1.0770,8.6003 , 6.4686,7.6535 , 6.0953,7.2118 , 1.2751,8.0583};
	double up10_11xy[16]={6.2074,7.3444 , 7.9036,2.5685 , 7.7332,2.6000 ,  6.0953,7.2118 , 6.4686,7.6535 , 8.3007,2.4950 , 7.7332,2.6000 , 6.0953,7.2118};
	double up11_12xy[16]={7.9036,2.5685 , 4.9466,-0.8905 , 4.7756,-0.8597 ,  7.7332,2.6000 , 8.3007,2.4950 , 5.3453,-0.9622 , 4.7756,-0.8597 ,  7.7332,2.6000};

	double up12_1xy[16]={4.9466,-0.8905 , 6.5119,-5.1633 , 6.3412,-5.1334 ,  4.7756,-0.8597 , 5.3453,-0.9622 , 6.9097,-5.2329 ,6.3412,-5.1334 , 4.7756,-0.8597};
	TGeoArb8 *up1_2 = new TGeoArb8 ("up1_2",2.5, up1_2xy  );
	TGeoArb8 *up2_3 = new TGeoArb8 ("up2_3",2.5, up2_3xy  );
	TGeoArb8 *up3_4 = new TGeoArb8 ("up3_4",2.5, up3_4xy  );
	TGeoArb8 *up4_5 = new TGeoArb8 ("up4_5",2.5, up4_5xy  );
	TGeoArb8 *up5_6 = new TGeoArb8 ("up5_6",2.5, up5_6xy  );
	TGeoArb8 *up6_7 = new TGeoArb8 ("up6_7",2.5, up6_7xy  );
	TGeoArb8 *up7_8 = new TGeoArb8 ("up7_8",2.5, up7_8xy  );
	TGeoArb8 *up8_9 = new TGeoArb8 ("up8_9",2.5, up8_9xy  );
	TGeoArb8 *up9_10 = new TGeoArb8 ("up9_10",2.5, up9_10xy  );
	TGeoArb8 *up10_11 = new TGeoArb8 ("up10_11",2.5, up10_11xy  );
	TGeoArb8 *up11_12 = new TGeoArb8 ("up11_12",2.5, up11_12xy  );
	TGeoArb8 *up12_1 = new TGeoArb8 ("up12_1",2.5, up12_1xy  );
	TGeoTranslation *up_Z = new TGeoTranslation("up_Z",0,0,12.95/2+5./2);
	up_Z->RegisterYourself();



	//Deckel
double xDeckel[12] = {6.9097,3.3939,-1.9896,-3.5058,-7.9866,-9.8624,-6.3111,-1.8393,1.0770,6.4686,8.3007,5.3453};

	double yDeckel[12] ={-5.2329,-9.4288,-8.4361,-4.1481,-3.3866,1.7752,5.9410,5.1101,8.6003,7.6535,2.4950,-0.9622}; 

TGeoXtru *Deckel = new TGeoXtru (2);
		Deckel->DefinePolygon(12,xDeckel,yDeckel);
		Deckel->DefineSection(0,12.95/2+5,0,0,1);
		Deckel->DefineSection(1,12.95/2+5+0.5,0,0,1);
TGeoVolume *DeckelVol = new TGeoVolume("Deckel",Deckel,Al_sol);
	TGeoCompositeShape *KappenForm = new TGeoCompositeShape("KappenForm","(orange+gelb+braun+grau+gruen)-orangeIn:In_Z-gelbIn:In_Z-braunIn:In_Z-grauIn:In_Z-gruenIn:In_Z+(up1_2:up_Z+up2_3:up_Z+up3_4:up_Z+up4_5:up_Z+up5_6:up_Z+up6_7:up_Z+up7_8:up_Z+up8_9:up_Z+up9_10:up_Z+up10_11:up_Z+up11_12:up_Z+up12_1:up_Z)");
	TGeoVolume *Kappe = new TGeoVolume("Kappe",KappenForm,Al_sol);
	//Kappe->SetTransparency(50);
	//DeckelVol->SetTransparency(50);
	TGeoRotation *rightPosRot = new TGeoRotation("rightPosRot", 0,0,180+130.2);
	rightPosRot->RegisterYourself();
	TGeoCombiTrans *rightPos = new TGeoCombiTrans(0,0,6.475,rightPosRot);
	rightPos->RegisterYourself();
	//top->AddNode(Kappe,0,rightPos);
	//top->AddNode(DeckelVol,1,rightPos);

	//TGeoVolume *Kappe2 = new TGeoVolume("Kappe2",KappenForm,Al_sol);
	Kappe->SetLineColor(kBlue);
	DeckelVol->SetLineColor(kBlue);
	//top->AddNode(Kappe2,2);

	//Kapsel
	TGeoTube *CapTube = new TGeoTube("CapTube",0,3.75,5);
	TGeoPgon *CapPgon = new TGeoPgon("CapPgon",0.,360.,6,2);
		CapPgon->DefineSection(0,-5,3.0571,3.8);
		CapPgon->DefineSection(1,4.9094,3.75,3.8);
double CapThickness = 0.08;
double Scale = (3.0571-CapThickness)/3.0571;
	TGeoTube *CapInTube = new TGeoTube("CapInTube",0,3.75*Scale,5);
	TGeoPgon *CapInPgon = new TGeoPgon("CapInPgon",0.,360.,6,2);
		CapInPgon->DefineSection(0,-5,3.0571-CapThickness,3.8);
		CapInPgon->DefineSection(1,4.9094,3.75-CapThickness,3.8);
	TGeoTube *CapTop = new TGeoTube("CapTop",0,3.75,CapThickness/2);
		TGeoTranslation *CapTop_Z = new TGeoTranslation("CapTop_Z",0,0,5-CapThickness/2);
		CapTop_Z->RegisterYourself();		
	TGeoCompositeShape *CapOutForm= new TGeoCompositeShape("CapOutForm","(CapTube-CapPgon)");
	TGeoCompositeShape *CapInForm= new TGeoCompositeShape("CapInForm","(CapInTube-CapInPgon)");
	TGeoTranslation *Cap_Z = new TGeoTranslation("Cap_Z",0,0,CapThickness);
	Cap_Z->RegisterYourself();
	TGeoCompositeShape *CapForm = new TGeoCompositeShape("CapForm","CapOutForm-CapInForm:Cap_Z+CapTop:CapTop_Z");
	
	TGeoVolume *Capsule = new TGeoVolume("Capsule",CapForm,Al_sol);	
	Capsule->SetTransparency(10);
	Capsule->SetLineColor(10);
	//top->AddNode(Capsule,5);
	
//Kristall
double VacThickness = 0.08;
double Scale2 =(3.0571-CapThickness-VacThickness)/3.0571;
cout << Scale2 << endl;
	TGeoTube *CryTube = new TGeoTube("CryTube",0,3.75,3.5);
	TGeoPgon *CryPgon = new TGeoPgon("CryPgon",0.,360.,6,2);
		CryPgon->DefineSection(0,-3.5,3.0571-CapThickness-VacThickness,3.8);
		CryPgon->DefineSection(1,1.5+4.9094,3.75-CapThickness-VacThickness,3.8);
	TGeoTube *CryHole = new TGeoTube("CryHole",0,0.5,3);
	TGeoTranslation *CryHoleTrans = new TGeoTranslation("CryHoleTrans",0,0,0.5);
CryHoleTrans->RegisterYourself();
TGeoCompositeShape *CryForm= new TGeoCompositeShape("CryForm","((CryTube-CryPgon)-CryHole:CryHoleTrans)");

TGeoVolume *Crystal = new TGeoVolume("Crystal",CryForm,Ge);
	Crystal->SetLineColor(kGreen);
	Crystal->SetFillColor(kGreen);
TGeoTranslation *CryTrans = new TGeoTranslation(0,0,5-1.5+CapThickness+VacThickness);
CryTrans->RegisterYourself();
//top->AddNode(Crystal,6,CryTrans); 	//top->AddNode(Kappe,0,rightPos);
	//top->AddNode(DeckelVol,1,rightPos);


// Capsule+Crystal
TGeoVolume *CryCap[48];
for(int i = 0; i < 48;i++)
{
	CryCap[i]= new TGeoVolumeAssembly("CryCap");
	CryCap[i]->AddNode(Crystal,i+1,CryTrans);
	CryCap[i]->AddNode(Capsule,100+i+1,new TGeoTranslation(0,0,5));
}
double TripleXOffset = -3.53/2-0.2;
double TripleYOffset = -6.1386/2; 
TGeoRotation *CryCap1rot = new TGeoRotation("CryCap1rot", -0,+4,0);
	CryCap1rot->RegisterYourself();
	TGeoCombiTrans *CryCap1Combitrans = new TGeoCombiTrans(0+TripleXOffset,0+TripleYOffset,+0.45+0.2843,CryCap1rot);
	CryCap1Combitrans->RegisterYourself();

TGeoRotation *CryCap2rot = new TGeoRotation("CryCap2rot", -90,-6.93,90);
	CryCap2rot->RegisterYourself();
	TGeoCombiTrans *CryCap2Combitrans = new TGeoCombiTrans(5.3162+TripleXOffset,6.1386/2+TripleYOffset,0.45+0.2843,CryCap2rot);
	CryCap2Combitrans->RegisterYourself();

TGeoRotation *CryCap3rot = new TGeoRotation("CryCap3rot", 0,-4,0);
	CryCap3rot->RegisterYourself();
	TGeoCombiTrans *CryCap3Combitrans = new TGeoCombiTrans(0+TripleXOffset,6.1386+TripleYOffset,0.45+0.2843,CryCap3rot);
	CryCap3Combitrans->RegisterYourself();

cout << "test0" << endl;

//Building 1 ClusterTemp
TGeoVolume *Pointingtube = geom->MakeTube("Pointer",Al_sol,0,0.2,150);

TGeoVolume *Cluster[16];
TGeoVolume *ClusterTemp[16];
for(int i = 0;i < 16; i ++)
{
	char buffer[20];
	sprintf(buffer, "Cluster_%d",i);
	Cluster[i]= new TGeoVolumeAssembly(buffer);
	ClusterTemp[i]= new TGeoVolumeAssembly("ClusterTemp");
//	ClusterTemp->AddNode(Pointingtube,5);
	ClusterTemp[i]->AddNode(Kappe,200+i+1,rightPos);
	ClusterTemp[i]->AddNode(DeckelVol,300+i+1,rightPos);
	ClusterTemp[i]->AddNode(CryCap[3*i],1000+i+1,CryCap1Combitrans);

	ClusterTemp[i]->AddNode(CryCap[3*i+1],2000+i+1,CryCap2Combitrans);
	ClusterTemp[i]->AddNode(CryCap[3*i+2],3000+i+1,CryCap3Combitrans);
	cout <<"winkel "<< 180/Pi*TMath::ATan(12.5751/-26.3009) << endl;
	cout <<"winkel2 "<< 180/Pi*TMath::ATan(7.0801/-26.3009) << endl;
	TGeoRotation *ClusterTemprot = new TGeoRotation;
		ClusterTemprot-> RotateZ(0);
		ClusterTemprot->RotateY(180);
		ClusterTemprot->RotateX(0);
		ClusterTemprot->RegisterYourself();

Cluster[i]->AddNode(ClusterTemp[i],400+i,new TGeoCombiTrans(0,0,0,ClusterTemprot));
}
//top->AddNode(ClusterTemp,100,new TGeoCombiTrans(-7.5713,12.0363,GlobalZOffset-26.416,ClusterTemprot2));

cout << "test1" << endl;

/*TGeoRotation *Clusterrot = new TGeoRotation;
	Clusterrot-> RotateZ(0);
		Clusterrot->RotateX(-180/Pi*TMath::ATan(12.5751/-26.3009));
		Clusterrot->RotateY(180/Pi*TMath::ATan(7.0801/-26.3009));
		Clusterrot->RegisterYourself();
top->AddNode(Cluster,1000,new TGeoCombiTrans(7.0801,12.5751,GlobalZOffset-26.3009,Clusterrot));*/

//Inner Ring
ClusterPlacer(7.0801,12.5751, -26.3009, GlobalZOffset , 1000, strange,Cluster[0],180+150.62, 28.73,180-4.95);
cout << "test2" << endl;
ClusterPlacer(12.8422,-0, -27.1123, GlobalZOffset , 2000, strange,Cluster[1],180+90,25.34,180);
 ClusterPlacer(7.0801,-12.5751, -26.3009, GlobalZOffset , 3000, strange,Cluster[2],180+29.38,28.73,180+4.95);

 ClusterPlacer(-7.0801,-12.5751, -26.3009, GlobalZOffset , 4000, strange,Cluster[3],150.62,28.73,4.95);
 ClusterPlacer(-12.8422,0, -27.1123, GlobalZOffset , 5000, strange,Cluster[4],90,25.34,0);
 ClusterPlacer(-7.0801,12.5751, -26.3009, GlobalZOffset , 6000, strange,Cluster[5],29.38,28.73,4.95);

//Outer Ring
ClusterPlacer(8.421,20.8241, -19.8857, GlobalZOffset , 7000, strange,Cluster[6],180+157.98,48.5,0); //Patient
ClusterPlacer(17.9414,10.7963, -21.4836, GlobalZOffset , 8000, strange,Cluster[7],180+121.04,44.29,180-4.95);
ClusterPlacer(24.3173,0, -17.5689, GlobalZOffset , 9000, strange,Cluster[8],180+90,54.51,0);
ClusterPlacer(17.9414,-10.7963, -21.4836, GlobalZOffset , 10000, strange,Cluster[9],180+180-121.04,44.29,180+4.95);
ClusterPlacer(8.421,-20.8241, -19.8857, GlobalZOffset , 11000, strange,Cluster[10],180+180-157.98,48.5,0); //Patient

ClusterPlacer(-8.421,-20.8241, -19.8857, GlobalZOffset , 12000, strange,Cluster[11],+157.98,48.5,180); //Patient
ClusterPlacer(-17.9414,-10.7963, -21.4836, GlobalZOffset , 13000, strange,Cluster[12],+121.04,44.29,-4.95);
ClusterPlacer(-24.3173,0, -17.5689, GlobalZOffset , 14000, strange,Cluster[13],+90,54.51,180);
ClusterPlacer(-17.9414,10.7963, -21.4836, GlobalZOffset , 15000, strange,Cluster[14],+180-121.04,44.29,4.95);
ClusterPlacer(-8.421,20.8241, -19.8857, GlobalZOffset , 16000, strange,Cluster[15],+180-157.98,48.5,180); //Patient

	top->AddNode(strange,0);
	geom->CloseGeometry();
	top->Write();
	fi->Close();
	//strange->Draw("");	strange->Raytrace();
	strange->Draw("ogl");
	geom->CheckOverlaps(0.0000000001,option="d"); //1 Overlap von Strahlrohr + Kugel ist normal!!!!!!!!!!!!!!!!!
	geom->PrintOverlaps();
	cout <<"Anzahl der Objekte: "<< ObjektNr << endl;
	//Paket[1]->Print();
}


