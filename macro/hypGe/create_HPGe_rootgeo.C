void HPGe_geo(){
  // Forward tof geometry parameters
  //-----------------------------
  //-- macro created by Alicia S. based on
  //-- Panda TPR and on previous geant3 geometry for forward tof(Vladimir V.)
  const Double_t  kBCentX    = 2.5;   // half length(cm) //from EMC TDR
  const Double_t  kBCentY    = 70.0;   //half length (cm) //?
  const Double_t  kBCentZ    = 0.75;   //half length (cm) //?

  const Double_t  kBBeamX    = 2.5;   // half length(cm) //from EMC TDR
  const Double_t  kBBeamY    = 32.5;  //alf length (cm) //?
  const Double_t  kBBeamZ    = 0.75;   //half length (cm) //?

  const Double_t  kBVertX    = 5.0;   //half length (cm) //from EMC TDR
  const Double_t  kBVertY    = 70.0;   //half length (cm) //?
  const Double_t  kBVertZ    = 0.75;   //half length (cm) //from EMC TDR

 //--------------------------------------------------------------------
  int fcrl; // each cluster consists of several crystals
  int fclr;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  //TString outfile= "../../geometry/HPGeCluster.root";
  TString outfile= "./HPGeCluster.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medGe  = Media->getMedium("germanium");
  Int_t nmedGe=geobuild->createMedium(medGe);
  
  //Scintillator plate
  FairGeoMedium *medsci  = Media->getMedium("polypropylene");
  Int_t nmedsci=geobuild->createMedium(medsci);
  
  // aluminum capsule
  FairGeoMedium *medcap  = Media->getMedium("HYPaluminium");
  Int_t nmedcap=geobuild->createMedium(medcap);

 

  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  
  TGeoVolume *top = new TGeoVolumeAssembly("hpge");
                    
  gGeoMan->SetTopVolume(top);
  Double_t deg = TMath::Pi()/180.;


  TGeoVolume *sphere = new TGeoVolumeAssembly("sphere");
    
  TGeoTube* Crystal_Tube = new TGeoTube("tube",0.,7.0/2.,7.8/2.);

  TGeoPgon* Crystal_Polyhedra = new TGeoPgon("Pgon",0., 360., 6,2);

  Crystal_Polyhedra->DefineSection(0,-3.9,0.,3.50*cos(30.*deg));
  Crystal_Polyhedra->DefineSection(1,3.9,0.,(3.50));

  // scintillator plate
  TGeoPgon* lay_sci = new TGeoPgon("PgLay",0., 360., 6,2);
  //first thickness 0.5 cm,
     //second 1.cm
     lay_sci->DefineSection(0,-0.25,0.,3.50*cos(30.*deg));
     lay_sci->DefineSection(1,0.25,0.,(3.50)*cos(30.*deg));
  //TGeoPgon* lay_sci ;
  
  //aluminum capsule
  TGeoPgon* lay_capl = new TGeoPgon("PgcapL",0., 360., 6,2);
  //first thickness 0.5 cm,
  //second 1.cm
  lay_capl->DefineSection(0,-4.2,0.,3.144);
  lay_capl->DefineSection(1,4.2,0.,(3.627));
   
  TGeoPgon* lay_caps = new TGeoPgon("PgcapS",0., 360., 6,2);
  //first thickness 0.5 cm,
  //second 1.cm
  lay_caps->DefineSection(0,-3.95,0.,3.078);
  lay_caps->DefineSection(1,3.95,0.,(3.553));


  //###real geometry  
  //###TGeoCompositeShape* logicCrystal_test = new TGeoCompositeShape("test","Pgon*tube");
  
  TGeoPgon* logicCrystal_test;
  logicCrystal_test = Crystal_Polyhedra ;
  TGeoCompositeShape* lay_cap = new TGeoCompositeShape("test","(PgcapL)-(PgcapS)");
  
  

  //  GeCluster* HypGe= new GeCluster();
   
   
   CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
		 30.0, 168.0, 90., 2.0,1000,fcrl,fclr); // L
    CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
    				       30.0, 168.0,18, 2.0,1200,fcrl,fclr); // N 
   
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
					30.1, 168.0, 162, 2.0,400,fcrl,fclr); // D
    
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
					30.3, 168.0, 234, 2.0,600,fcrl,fclr); // F
 
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
					30., 168.0, 306., 2.0,1400,fcrl,fclr); // Q
 
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,32.2,
					 148.0, 54.0, 2.0,1100,fcrl,fclr); // M
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,32.9,
				       148.0, 126.0, 2.0,300,fcrl,fclr); // C
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,31.4,
				       148.0,  198.0, 2.0,500,fcrl,fclr); // E
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,32.6,
				       148.0,  270.0, 2.0,1500,fcrl,fclr); // R
     CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,30.3,
				       148.0,  342.0, 2.0,1300,fcrl,fclr); // P
    CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
				      30.0, 140.0, 90., 2.0,700,fcrl,fclr); // G
    CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
    				       30.2, 140.0,18., 2.0,800,fcrl,fclr); // J 
   
    CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
					30.15, 140.0, 162., 2.0,100,fcrl,fclr); // A
    
    CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
					30.15, 140.0, 234., 2.0,200,fcrl,fclr); // B
 
      CreateCluster(logicCrystal_test,lay_sci,lay_cap,*sphere,
					30.2, 140.0, 306., 2.0,900,fcrl,fclr); // K
 
    
  
    top->AddNode(sphere, 0,new TGeoCombiTrans(0., 0., -78., new TGeoRotation (0)));

  
  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
//   gGeoManager->Export(outfile);
  top->Draw("ogl");
}  

void CreateCluster(TGeoPgon* logicCrystal_test,
				TGeoPgon* lay_sci,
				TGeoCompositeShape* lay_cap,
				TGeoVolume& sphere,
				Double_t distance, 
				Double_t theta,   
				Double_t phi, 
				Double_t depth_first_interaction,
		   int id_start, int fnum_crystal,int fnum_cluster)
{
char cluster_name[10];
  if(id_start==800) sprintf(cluster_name,"J");
  if(id_start==700) sprintf(cluster_name,"G");
  if(id_start==100) sprintf(cluster_name,"A");
  if(id_start==200) sprintf(cluster_name,"B");
  if(id_start==900) sprintf(cluster_name,"K");

  if(id_start==1100) sprintf(cluster_name,"M");
  if(id_start==300) sprintf(cluster_name,"C");
  if(id_start==500) sprintf(cluster_name,"E");
  if(id_start==1500) sprintf(cluster_name,"R");
  if(id_start==1300) sprintf(cluster_name,"P");
  
  if(id_start== 400) sprintf(cluster_name,"D");
  if(id_start== 600) sprintf(cluster_name,"F");
  if(id_start==1000) sprintf(cluster_name,"L");
  if(id_start==1200) sprintf(cluster_name,"N");
  
  if(id_start==1400) sprintf(cluster_name,"Q");
  
  Double_t deg = TMath::Pi()/180.;
  Double_t phi1;
  
  fnum_crystal = 0; // each cluster consists of several crystals
  fnum_cluster = 0; // cluster number

  distance = distance + 2.5;

  phi1=phi*deg;

  if(phi1 >360.0*deg) phi1 = phi1 - 360.0*deg;
  if(phi1>=0.0 && phi1 <=180.0*deg) phi1 = 180.0*deg - phi1;
  if(phi1>180.0*deg && phi1 <360.0*deg) phi1 = 540.0*deg - phi1;
  phi1 = phi1+90.0*deg;

  if(phi >360.0) phi = phi - 360.0;
  if(phi>=0.0 && phi <=180.0) phi = 180.0 - phi;
  if(phi>180.0 && phi <360.0) phi = 540.0 - phi;
  phi = phi+90.0;

  TGeoRotation* rot3D = new TGeoRotation();

  TVector3* rotatedPos;
  //Double_t rotatedPos* = new Double_t();
 
  TGeoVolume* crystal_centre[60];
  TGeoVolume* laySci[60];
  TGeoVolume* Cap[60];

  //TGeoCombiTrans *c1;
  
  TGeoRotation* rot3D2 = new TGeoRotation();
  TVector3* rotatedPos2;
  //Double_t rotatedPos2* = new Double_t();
  //TGeoCombiTrans *c2;
  rot3D2->SetAngles(0, 0, 0);
  rot3D2->RotateX(theta);  
  rot3D2->RotateZ(phi);  

  
  rot3D->SetAngles(0, 0, 0);
  rot3D->RotateX(theta);  
  rot3D->RotateZ(phi);  

 
  rotatedPos = new TVector3(rotate(TVector3(0.,0.,62.22-8.05/2.0),rot3D)
    -  rotate(TVector3(0.,0.,62.22-8.05),rot3D2) 
			      +  rotate(TVector3(0.,0.,distance),rot3D2));

  

  rotatedPos2 = new TVector3(rotate(TVector3(0.,0.,62.22-8.05/2.0),rot3D) 
    - rotate(TVector3(0.,0.,62.22-8.05),rot3D2) 
			      + rotate(TVector3(0.,0.,distance-5.0),rot3D2));

  TGeoMedium *Ge = gGeoManager->GetMedium("germanium");
  //scintillator plate
  TGeoMedium *sci = gGeoManager->GetMedium("polypropylene");
  //capsule
  TGeoMedium *cap = gGeoManager->GetMedium("HYPaluminium");

  std::cout<<"material "<<Ge<<" "<<sci<<" "<<cap<<std::endl;
  
  
  char name_crystal[13];
  sprintf (name_crystal,"GeCrystal%02d",id_start+6);
  
 
  char name_lay[13];
  char name_cap[13];
  //scint.
  sprintf (name_lay,"laySci%02d",id_start+6);
  //capsula
  sprintf (name_cap,"Cap%02d",id_start+6);
 

   crystal_centre[fnum_crystal] = new TGeoVolume(name_crystal,
  						(const TGeoShape *)logicCrystal_test,
  						(const TGeoMedium *)Ge );
 
   laySci[fnum_crystal] = new TGeoVolume(name_lay,
  						(const TGeoShape *)lay_sci,
  						(const TGeoMedium *)sci );

   Cap[fnum_crystal] = new TGeoVolume(name_cap,(const TGeoShape *)lay_cap,
  						(const TGeoMedium *)cap );

   //DataG4 data = read.GetData(id_start+6);
  

   //  sphere.AddNode(crystal_centre[fnum_crystal],id_start+6, 
   // 		  new TGeoCombiTrans(rotatedPos.X(),
   // 				     rotatedPos.Y(),
   // 				     rotatedPos.Z(), 
   // 				     new TGeoRotation (rot3D)));

   sphere.AddNode(crystal_centre[fnum_crystal],id_start+6, 
		  new TGeoCombiTrans(rotatedPos->x(),
				     rotatedPos->y(),
				     rotatedPos->z(), 
				     new TGeoRotation (*rot3D)));

   sphere.AddNode(Cap[fnum_crystal],id_start+6, 
		  new TGeoCombiTrans(rotatedPos->x(),
				     rotatedPos->y(),
				     rotatedPos->z(), 
				     new TGeoRotation (*rot3D)));
  
   //scint
   //DataG4 data2 = readsci.GetData(id_start+6);
   ////cout<<id_start+6<<" x "<<data2.posX<<" y "<<data2.posY<<" z "<<data2.posZ<<endl;
   
   
   sphere.AddNode(laySci[fnum_crystal],id_start+6, 
		  new TGeoCombiTrans(rotatedPos2->x(),
				     rotatedPos2->y(),
				     rotatedPos2->z(), 
				     new TGeoRotation (*rot3D)));
   
    
 
  //------------------------------------------------ 
  // Sensitive detectors
  //------------------------------------------------ 
  

  
    
    
  cout  << id_start+6 << " " <<rotatedPos->x()<<" "
	<<rotatedPos->y()<<" "
	<<rotatedPos->z()<<endl;
  
  
  
  //------------------------------------------------ 
  // Crystals surrounding the central one.
  //------------------------------------------------ 

  fnum_crystal = fnum_crystal + 1;
  
  //Float_t v1[6],v2[6],v3[6];

  	
	char tempname[100];
	char periname[13];
	char cappname[13];
	TGeoVolume* crystal[60];
	TGeoVolume* laySci_peri[60];
	TGeoVolume* Cap_peri[60];

	TGeoRotation* rot3D3 = new TGeoRotation();
	TGeoRotation* rot3D4 = new TGeoRotation();
	
	 TVector3* v1;
	 TVector3* v2;

	
	
	for(int iii=0;iii<=1;iii++)
	  {
	    
	    
	    rot3D3->SetAngles(0., 0., 0.);
	    rot3D3->RotateX(-6.66);  
	    rot3D3->RotateZ((-1*iii+1)*60.0);  
	    rot3D3->RotateX(theta);  
	    rot3D3->RotateZ(phi);  
	    
	    // TVector3 v7;
	    // v7 = ((((TVector3(0.,0.,62.22-8.05/2.0)).RotateX(-6.66)).RotateZ((-1*iii+1)*60.0)).RotateX(theta)).RotateZ(phi)-((TVector3(0.,0.,62.22-8.05)).RotateX(theta)).RotateZ(phi)+((TVector3(0.,0.,distance)).RotateX(theta)).RotateZ(phi);
	    
	    v1 = new TVector3(rotate(TVector3(0.,0.,62.22-8.05/2.0),rot3D3)
	      -  rotate(TVector3(0.,0.,62.22-8.05),rot3D2) 
				+  rotate(TVector3(0.,0.,distance),rot3D2));
	    
	    //cout<<" crystal v1 "<<iii<<endl;
	    v1->Print();
	    
	    v2 =  new TVector3(rotate(TVector3(0.,0.,62.22-8.05/2.0),rot3D3) 
	      - rotate(TVector3(0.,0.,62.22-8.05),rot3D2) 
				 + rotate(TVector3(0.,0.,distance-5.0),rot3D2));

	    //cout<<" crystal v2 "<<iii<<endl;
	    v2->Print();
	    
	    sprintf(tempname,"GeCrystal%04d",id_start+iii);
	    //TGeoVolume *crystal[fnum_crystal];
	    
	    //scint 
	    sprintf (periname,"laySci%04d",id_start+iii);
	    //capsula
	    sprintf (cappname,"Cap%04d",id_start+iii);
	    
	    crystal[fnum_crystal] = new TGeoVolume(tempname, 
						   (const TGeoShape *)logicCrystal_test,
						   (const TGeoMedium *)Ge);
	    
	    laySci_peri[fnum_crystal] = new TGeoVolume(periname, 
						       (const TGeoShape *)lay_sci,
						       (const TGeoMedium *)sci);
	    
	    Cap_peri[fnum_crystal] = new TGeoVolume(cappname, 
						    (const TGeoShape *)lay_cap,
						    (const TGeoMedium *)cap);
	    
	    
	    
	    //DataG4 data = read.GetData(id_start+iii);
	    
	    // sphere.AddNode(crystal[fnum_crystal],id_start+iii, 
	    // 		       new TGeoCombiTrans(PosOrb.X(),PosOrb.Y(),
	    // 					  PosOrb.Z(), new TGeoRotation (rot3D)));
	    
	    sphere.AddNode(crystal[fnum_crystal],id_start+iii, 
			   new TGeoCombiTrans(v1->x(),v1->y(),v1->z(), 
					      new TGeoRotation (*rot3D3)));
	    
	    //capsula
	    sphere.AddNode(Cap_peri[fnum_crystal],id_start+iii, 
			   new TGeoCombiTrans(v1->x(),v1->y(),v1->z(), 
					      new TGeoRotation (*rot3D3)));
	    
	    //DataG4 data2 = readsci.GetData(id_start+iii);
	    
	    
	    
	    sphere.AddNode(laySci_peri[fnum_crystal],id_start+iii, 
			   new TGeoCombiTrans(v2->x(),v2->y(),v2->z(), 
					      new TGeoRotation (*rot3D)));
	    
	    
	    //------------------------------------------------ 
	    // Sensitive detectors
	    //------------------------------------------------ 
	    
	    //AddSensitiveVolume(crystal[fnum_crystal]);
	    
	    //  cout  << id_start+iii << " " << v1[0] << " "<< v1[2]<< " "<< v1[2]<< std::endl;
	    cout  << id_start+iii << " " <<v1->x()<< " " <<v1->y()<< " " <<v1->z()<<endl;
	     
	    fnum_crystal = fnum_crystal + 1;
	  
	  }
	
	fnum_cluster = fnum_cluster + 1;
	
	

	
}

TVector3 rotate(TVector3 vec, TGeoRotation* rotma)
{
  TVector3 v;
  v.SetXYZ(0,0,0);
  
  
  const Double_t* b;
 b = rotma->GetRotationMatrix();
  //a = new TRotation(b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7],b[8]);
 
  for (Int_t i=0; i<9; i++) {
      if (TMath::Abs(b[i])<1E-10) b[i] = 0;
      if (TMath::Abs(b[i]-1)<1E-10) b[i] = 1;
      if (TMath::Abs(b[i]+1)<1E-10) b[i] = -1;
  } 
   for(i=0;i<9;i++){
     //cout<<" matrix "<<b[i]<<endl;
  }
  v.SetX(b[0]*vec.X()+b[1]*vec.Y()+b[2]*vec.Z());
  v.SetY(b[3]*vec.X()+b[4]*vec.Y()+b[5]*vec.Z());
  v.SetZ(b[6]*vec.X()+b[7]*vec.Y()+b[8]*vec.Z());
  //cout<<" rotate "<<v.X()<<" "<<v.Y()<<" "<<v.Z()<<endl;
  
  return v;
  

}

 
