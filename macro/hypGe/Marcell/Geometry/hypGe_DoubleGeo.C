#include "TGeoManager.h"
double Pi = TMath::Pi();

void ClusterPlacer(double x, double y, double z, double GlobalZOffset , int ObjectNo, TGeoVolume *top,TGeoVolume *Clusterr,double phi, double theta, double psi)
{
TGeoRotation *Clusterrot = new TGeoRotation("Clusterrot",phi, theta,psi-phi);
		Clusterrot->RegisterYourself();
top->AddNode(Clusterr,ObjectNo,new TGeoCombiTrans(x,y,GlobalZOffset+z,Clusterrot));
}

void hypGe_DoubleGeo()
{

gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
	TString outfile= "../../../../geometry/hypGe_GeoMarcell_2er.root";

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



  TGeoManager *geom = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TGeoVolume *top = new TGeoVolumeAssembly("hpGe");
	TGeoVolume *strange = new TGeoVolumeAssembly("strange");
	   
          
  geom->SetTopVolume(top);
	
	Double_t GlobalZOffset = -55;
	int ObjektNr = 0;
	//Strahlrohr
	TGeoVolume *Strahlrohr =geom->MakeTube("Strahlrohr",Al_sol,0,7.5,150);
	Strahlrohr->SetLineColor(kBlue);
	//strange->AddNode(Strahlrohr,ObjektNr);
	ObjektNr++;
	//Außenhülle
	TGeoVolume *Außenhülle =geom->MakeTube("Außenhülle",Al_sol,45,45.5,100);
	Außenhülle->SetLineColor(kBlue);
	//strange->AddNode(Außenhülle,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;

//Halteplatte
	TGeoVolume *HoldingStruc =geom->MakeBox("HoldingStruc",Al_sol,1,45,50);
	HoldingStruc->SetLineColor(kGreen);
	//strange->AddNode(HoldingStruc,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;
//Hilfskugel
	TGeoSphere *Kugel = new TGeoSphere("Kugel",1,30,0,180,0,360);
	TGeoVolume *Kugell = new TGeoVolume("Kugell",Kugel,Al_sol);
	Kugell->SetLineColor(kBlue);
	Kugell->SetTransparency(99);
	//strange->AddNode(Kugell,ObjektNr,new TGeoTranslation(0,0,GlobalZOffset));
	ObjektNr++;
	
//Nullpunktmarker
	TGeoSphere *Origin = new TGeoSphere("Origin",0,1,0,180,0,360);
	TGeoVolume *Originn = new TGeoVolume("Originn",Origin,Al_sol);
	Originn->SetLineColor(kGreen);
	//strange->AddNode(Originn,ObjektNr);
	ObjektNr++;
//Kappe
	//Außen
	double orangexy[16]={-2.0065,6.5042 , -3.9982,3.0203 , -2.2317,0 , 2.0065,6.5042 , -3.0594,8.2379 , -5.5748,3.9280 , -3.2774,0 , 3.0594,8.3279};
	double gelbxy[16]={-2.2317,0 , -3.9982,-3.0203 , -2.0065,-6.5042 , 2.0065,-6.5042 , -3.2774,0 , -5.5748,-3.9280 ,  -3.0594,-8.3279 , 3.0594,-8.3279};	
	double braunxy[16]={2.0065,-6.5042 , 3.9982,-3.0203 , 2.2317,0 , -2.2317,0 , 3.0594,-8.3279 , 5.5748,-3.9580 , 3.2774,0 , -3.2774,0};
	double grauxy[16]={2.2317,0 , 3.9982,3.0203 , 2.0065,6.5042 , -2.2317,0 , 3.2774,0 , 5.5748,3.9250 , 3.0594,8.3279 , -3.2774,0};

	
	TGeoArb8 *orange = new TGeoArb8 ("orange",12.95/2, orangexy  );
	TGeoArb8 *gelb = new TGeoArb8 ("gelb",12.95/2, gelbxy  );
	TGeoArb8 *braun = new TGeoArb8 ("braun",12.95/2, braunxy  );
	TGeoArb8 *grau = new TGeoArb8 ("grau",12.95/2, grauxy  );


	//Innen
	double orangeInxy[16]={-1.9198,6.3539 , -3.8251,3.0212 , -2.0581,0 , 1.9198,6.3539 , -2.9726,8.1776 , -5.4016,3.9289 , -3.1037,0 , 2.9726,8.1776};
 
	double gelbInxy[16]={-2.0581,0 , -3.8251,-3.0212 , -1.9198,-6.3539 , 1.9198,-6.3539 , -3.1037,0 , -5.4016,-3.9289 , -2.9726,-8.1776 , 2.9726,-8.1776};
	double braunInxy[16]={1.9198,-6.3539 , 3.8251,-3.0212 , 2.0581,0 , -2.0581,0 , 2.9726,-8.1776 , 5.4016,-3.9289 , 3.1037,0 , -3.1057,0};
	double grauInxy[16]={2.0581,0, 3.8251,3.0212 , 1.9198,6.3539 , -2.0581,0 , 3.1037,0 , 5.4016,3.9289 , 2.9726,8.1776 , -3.1037,0};


	TGeoTranslation *In_Z = new TGeoTranslation("In_Z",0,0,0.15);
	In_Z->RegisterYourself();
	TGeoArb8 *orangeIn = new TGeoArb8 ("orangeIn",(12.95-0.15)/2, orangeInxy  );
	TGeoArb8 *gelbIn = new TGeoArb8 ("gelbIn",(12.95-0.15)/2, gelbInxy  );
	TGeoArb8 *braunIn = new TGeoArb8 ("braunIn",(12.95-0.15)/2, braunInxy  );
	TGeoArb8 *grauIn = new TGeoArb8 ("grauIn",(12.95-0.15)/2, grauInxy  );

		

//UpperPart
	double up1_2xy[16]={-3.0594,8.3279 , -5.5748,3.9280 , -5.4016,3.9289 , -2.9726,8.1776 ,
 -3.2618,8.6784 , -5.9787,3.9260 , -5.4016,3.9289 , -2.9726,8.1776};
	double up2_3xy[16]={-5.5748,3.9280 , -3.2774,0 , -3.1037,0 , -5.4016,3.9289 ,
 -5.9787,3.9260 , -3.6824,0 , -3.1037,0 , -5.4016,3.9289};
	double up3_4xy[16]={-3.2774,0 , -5.5748,-3.9280 , -5.4016,-3.9289 ,  -3.1037,0 ,
-3.6824,0 , -5.9787,-3.9260 , -5.4016,-3.9289 ,  -3.1037,0};
	double up4_5xy[16]={-5.5748,-3.9280 , -3.0594,-8.3279 , -2.9726,-8.1776 , 
-5.4016,-3.9289 ,
-5.9789,-3.9260 , -3.2618,-8.6784 , -2.9726,-8.1776 , -5.4016,-3.9289};
	double up5_6xy[16]={-3.0594,-8.3279 , 3.0594,-8.3279 , 2.9726,-8.1776 ,  -2.9726,-8.1776 , -3.2618,-8.6784 , 3.2618,-8.6784 , 2.9726,-8.1776 , -2.9726,-8.1776};
	double up6_7xy[16]={5.5748,-3.9280 , 3.0594,-8.3279 , 2.9726,-8.1776 , 
5.4016,-3.9289 , 5.9789,-3.9260 , 3.2618,-8.6784 , 2.9726,-8.1776 , 5.4016,-3.9289};
	double up7_8xy[16]={3.2774,0 , 5.5748,-3.9280 , 5.4016,-3.9289 ,  3.1037,0 ,
3.6824,0 , 5.9787,-3.9260 , 5.4016,-3.9289 ,  3.1037,0};

	double up8_9xy[16]={5.5748,3.9280 , 3.2774,0 , 3.1037,0 , 5.4016,3.9289 ,
 5.9787,3.9260 , 3.6824,0 , 3.1037,0 , 5.4016,3.9289};

	double up9_10xy[16]={3.0594,8.3279 , 5.5748,3.9280 , 5.4016,3.9289 , 2.9726,8.1776 ,
 3.2618,8.6784 , 5.9787,3.9260 , 5.4016,3.9289 , 2.9726,8.1776}; 
	double up10_1xy[16]={-3.0594,8.3279 , 3.0594,8.3279 , 2.9726,8.1776 ,  -2.9726,8.1776 , -3.2618,8.6784 , 3.2618,8.6784 , 2.9726,8.1776 , -2.9726,8.1776};
	TGeoArb8 *up1_2 = new TGeoArb8 ("up1_2",2.5, up1_2xy  );
	TGeoArb8 *up2_3 = new TGeoArb8 ("up2_3",2.5, up2_3xy  );
	TGeoArb8 *up3_4 = new TGeoArb8 ("up3_4",2.5, up3_4xy  );
	TGeoArb8 *up4_5 = new TGeoArb8 ("up4_5",2.5, up4_5xy  );
	TGeoArb8 *up5_6 = new TGeoArb8 ("up5_6",2.5, up5_6xy  );
	TGeoArb8 *up6_7 = new TGeoArb8 ("up6_7",2.5, up6_7xy  );
	TGeoArb8 *up7_8 = new TGeoArb8 ("up7_8",2.5, up7_8xy  );
	TGeoArb8 *up8_9 = new TGeoArb8 ("up8_9",2.5, up8_9xy  );
	TGeoArb8 *up9_10 = new TGeoArb8 ("up9_10",2.5, up9_10xy  );
	
	TGeoArb8 *up10_1 = new TGeoArb8 ("up10_1",2.5, up10_1xy  );
	TGeoTranslation *up_Z = new TGeoTranslation("up_Z",0,0,12.95/2+5./2);
	up_Z->RegisterYourself();



	//Deckel
double xDeckel[10] = {-3.2618,-5.9787,-3.6824,-5.9787,-3.2618,3.2618,5.9787,3.6824,5.9787,3.2618};

	double yDeckel[10] ={8.6784,3.9260,0,-3.9260,-8.6784,-8.6784,-3.9260,0,3.9260,8.6784}; 

TGeoXtru *Deckel = new TGeoXtru (2);
		Deckel->DefinePolygon(10,xDeckel,yDeckel);
		Deckel->DefineSection(0,12.95/2+5,0,0,1);
		Deckel->DefineSection(1,12.95/2+5+0.5,0,0,1);
TGeoVolume *DeckelVol = new TGeoVolume("Deckel",Deckel,Al_sol);
	TGeoCompositeShape *KappenForm = new TGeoCompositeShape("KappenForm","orange+gelb+braun+grau-orangeIn:In_Z-gelbIn:In_Z-braunIn:In_Z-grauIn:In_Z+up1_2:up_Z+up2_3:up_Z+up3_4:up_Z+up4_5:up_Z+up5_6:up_Z+up6_7:up_Z+up7_8:up_Z+up8_9:up_Z+up9_10:up_Z+up10_1:up_Z");
	TGeoVolume *Kappe = new TGeoVolume("Kappe",KappenForm,Al_sol);
	Kappe->SetLineColor(kBlue);
	DeckelVol->SetLineColor(kBlue);
	//Kappe->SetTransparency(50);
	//DeckelVol->SetTransparency(50);
	/*TGeoRotation *rightPosRot = new TGeoRotation("rightPosRot", 0,0,180+130.2);
	rightPosRot->RegisterYourself();
	TGeoCombiTrans *rightPos = new TGeoCombiTrans(0,0,6.475,rightPosRot);
	rightPos->RegisterYourself();*/
	

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
double TripleXOffset = 0;//-3.53/2-0.2;
double TripleYOffset = 3.0693;//-6.1386/2; 
TGeoRotation *CryCap1rot = new TGeoRotation("CryCap1rot", 0,+4,-0);
	CryCap1rot->RegisterYourself();
	TGeoCombiTrans *CryCap1Combitrans = new TGeoCombiTrans(0+TripleXOffset,-TripleYOffset,0.45+0.2843,CryCap1rot);
	CryCap1Combitrans->RegisterYourself();

TGeoRotation *CryCap2rot = new TGeoRotation("CryCap2rot", 0,-4,-0);
	CryCap2rot->RegisterYourself();
	TGeoCombiTrans *CryCap2Combitrans = new TGeoCombiTrans(0,TripleYOffset,0.45+0.2843,CryCap2rot);
	CryCap2Combitrans->RegisterYourself();

TGeoRotation *CryCap3rot = new TGeoRotation("CryCap3rot", 0,-4,0);
	CryCap3rot->RegisterYourself();
	TGeoCombiTrans *CryCap3Combitrans = new TGeoCombiTrans(0+TripleXOffset,6.1386+TripleYOffset,0.45+0.2843,CryCap3rot);
	CryCap3Combitrans->RegisterYourself();

cout << "test0" << endl;

//Building 1 ClusterTemp
TGeoVolume *Pointingtube = geom->MakeTube("Pointer",Al_sol,0,0.2,150);

TGeoVolume *Cluster[24];
TGeoVolume *ClusterTemp[24];
for(int i = 0;i < 24; i ++)
{
	char buffer[20];
	sprintf(buffer, "Cluster_%d",i);
	Cluster[i]= new TGeoVolumeAssembly(buffer);
	ClusterTemp[i]= new TGeoVolumeAssembly("ClusterTemp");
	//ClusterTemp[i]->AddNode(Pointingtube,5);
	ClusterTemp[i]->AddNode(Kappe,200+i+1,new TGeoTranslation(0,0,6.475));
	ClusterTemp[i]->AddNode(DeckelVol,300+i+1,new TGeoTranslation(0,0,6.475));
	ClusterTemp[i]->AddNode(CryCap[2*i],1000+i+1,CryCap1Combitrans);

	ClusterTemp[i]->AddNode(CryCap[2*i+1],2000+i+1,CryCap2Combitrans);

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

//First Quarter

ClusterPlacer(-5.5,12.8525, -26.5436, GlobalZOffset , 1000, strange,Cluster[0],23.1678, 27.7747, -20.74+23.1678);

ClusterPlacer(-12.3507,8.4346, -26.0061, GlobalZOffset , 2000, strange,Cluster[1],55.6699, 29.9031, -50.21+55.6699);
 ClusterPlacer(-5.5,23.0778, -18.3621, GlobalZOffset , 3000, strange,Cluster[2],13.4046, 52.2607, -8.3+13.4046);

 ClusterPlacer(-12.0846,19.6444, -19.1849, GlobalZOffset , 4000, strange,Cluster[3],31.5985, 50.2457, -21.67+31.5985);

 ClusterPlacer(-18.1658,14.9679, -18.6003, GlobalZOffset , 5000, strange,Cluster[4],50.5126, 51.6831, -35.43+50.5126);

 ClusterPlacer(-20.9778,5.5762, -20.7083, GlobalZOffset , 6000, strange,Cluster[5],75.1143, 46.348, 9.44+75.1143);

//Second Quarter

ClusterPlacer(-5.5,-12.8525, -26.5436, GlobalZOffset , 7000, strange,Cluster[6],180-23.1678, 27.7747, 180-(-20.74+23.1678));

ClusterPlacer(-12.3507,-8.4346, -26.0061, GlobalZOffset , 8000, strange,Cluster[7],180-55.6699, 29.9031, 180-(-50.21+55.6699));
 ClusterPlacer(-5.5,-23.0778, -18.3621, GlobalZOffset , 9000, strange,Cluster[8],180-13.4046, 52.2607, 180-(-8.3+13.4046));

 ClusterPlacer(-12.0846,-19.6444, -19.1849, GlobalZOffset , 10000, strange,Cluster[9],180-31.5985, 50.2457, 180-(-21.67+31.5985));

 ClusterPlacer(-18.1658,-14.9679, -18.6003, GlobalZOffset , 11000, strange,Cluster[10],180-50.5126, 51.6831, 180-(-35.43+50.5126));

 ClusterPlacer(-20.9778,-5.5762, -20.7083, GlobalZOffset , 12000, strange,Cluster[11],180-75.1143, 46.348, 180-(9.44+75.1143));

//Third Quarter

ClusterPlacer(5.5,-12.8525, -26.5436, GlobalZOffset , 13000, strange,Cluster[12],180+23.1678, 27.7747, 180+(-20.74+23.1678));

ClusterPlacer(12.3507,-8.4346, -26.0061, GlobalZOffset , 14000, strange,Cluster[13],180+55.6699, 29.9031, 180+(-50.21+55.6699));
 ClusterPlacer(5.5,-23.0778, -18.3621, GlobalZOffset , 15000, strange,Cluster[14],180+13.4046, 52.2607, 180+(-8.3+13.4046));

 ClusterPlacer(12.0846,-19.6444, -19.1849, GlobalZOffset , 160000, strange,Cluster[15],180+31.5985, 50.2457, 180+(-21.67+31.5985));

 ClusterPlacer(18.1658,-14.9679, -18.6003, GlobalZOffset , 17000, strange,Cluster[16],180+50.5126, 51.6831, 180+(-35.43+50.5126));

 ClusterPlacer(20.9778,-5.5762, -20.7083, GlobalZOffset , 18000, strange,Cluster[17],180+75.1143, 46.348, 180+(9.44+75.1143));

//Fourth Quarter

ClusterPlacer(5.5,12.8525, -26.5436, GlobalZOffset , 19000, strange,Cluster[18],-23.1678, 27.7747, -(-20.74+23.1678));

ClusterPlacer(12.3507,8.4346, -26.0061, GlobalZOffset , 20000, strange,Cluster[19],-55.6699, 29.9031, -(-50.21+55.6699));
 ClusterPlacer(5.5,23.0778, -18.3621, GlobalZOffset , 21000, strange,Cluster[20],-13.4046, 52.2607, -(-8.3+13.4046));

 ClusterPlacer(12.0846,19.6444, -19.1849, GlobalZOffset , 22000, strange,Cluster[21],-31.5985, 50.2457, -(-21.67+31.5985));

 ClusterPlacer(18.1658,14.9679, -18.6003, GlobalZOffset , 23000, strange,Cluster[22],-50.5126, 51.6831, -(-35.43+50.5126));

 ClusterPlacer(20.9778,5.5762, -20.7083, GlobalZOffset , 24000, strange,Cluster[23],-75.1143, 46.348, -(9.44+75.1143));


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


