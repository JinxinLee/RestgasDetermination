

TVector3 ComputeTransformation(const int copyNo, Double_t *par) 
{//{ComputeTransformation(const int copyNo, TVector3* origin,Double_t *par) const
  TVector3 origin;
  
 
 Double_t    display= par[4]*( 20-copyNo);  
 Double_t    position= par[0]+(copyNo)*par[2];

 origin.SetXYZ(0, display,position);
       
  return origin;
  
}

TVector3 ComputeDimensions( const int copyNo,Double_t *par) 
{//ComputeDimensions(TVector3& Layer, const int copyNo,Double_t *par) const
  TVector3 Layer;
  
  //Double_t fStartPosition = firstPosition;
  //Double_t fHalfLayThick = 0.5*LayThick;
  //Double_t fSpacing  = LayThick;
  //Double_t fHalfLengthFirst = 0.5 * firstLength;
  //Double_t fHalfLengthIncr =  0.5 *(lastLength-firstLength)/20;
  //Double_t fHalfLastLength = 0.5*lastLength;

      Double_t  halfLength = par[3] + copyNo * par[4];
      Layer.SetXYZ(par[5],halfLength,par[1]);
      //Layer.SetX(par[5]);
      //Layer.SetY(par[1]);
      return Layer;
      
      //}
}

TVector3 rotate(TVector3 vec, TGeoRotation* rotma)
{
  TVector3 v;
  v.SetXYZ(0,0,0);
  
  vec.Print();
  const Double_t* b;
 b = rotma->GetRotationMatrix();
  //a = new TRotation(b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7],b[8]);
 
  for (Int_t i=0; i<9; i++) {
      if (TMath::Abs(b[i])<1E-10) b[i] = 0;
      if (TMath::Abs(b[i]-1)<1E-10) b[i] = 1;
      if (TMath::Abs(b[i]+1)<1E-10) b[i] = -1;
  } 
   for(i=0;i<9;i++){
     cout<<" matrix "<<b[i]<<endl;
  }
  v.SetX(b[0]*vec.X()+b[1]*vec.Y()+b[2]*vec.Z());
  v.SetY(b[3]*vec.X()+b[4]*vec.Y()+b[5]*vec.Z());
  v.SetZ(b[6]*vec.X()+b[7]*vec.Y()+b[8]*vec.Z());
  //cout<<" rotate "<<v.X()<<" "<<v.Y()<<" "<<v.Z()<<endl;
  
  return v;
  

}

int create_HypSTxy3Cmic_rootgeo3()
{

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");


   TString outfile= "../../geometry/HYPST_assexy3C.root";
   TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *medab  = Media->getMedium("HYPdiamond");
  Int_t nmedGe=geobuild->createMedium(medab);
  
  //Scintillator plate
  FairGeoMedium *medsci  = Media->getMedium("HYPsilicon");
  Int_t nmedsci=geobuild->createMedium(medsci);
  
  FairGeoMedium *medcar  = Media->getMedium("HYPcarbon");
  Int_t nmedC=geobuild->createMedium(medcar);

 

  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  
  TGeoVolume *top = new TGeoVolumeAssembly("hypst");
                    
  gGeoMan->SetTopVolume(top);

  TGeoVolume *complex = new TGeoVolumeAssembly("complex");

    //936.4335456774155
    const Int_t max = 80;   // max. number of stations
    Double_t xt[4];         // detector thickness [mm]
    Double_t yt[4];        // inner radius       [mm]
    Double_t zt[4];        // outer radius       [mm]
    Double_t z[max];         // z position from target [mm]
    Double_t dlt[max],dl[max];        // inner acceptance [deg]
    Double_t hlt[max],hl[max];        // outer acceptance [deg]
    Double_t zll[max],zlt[max];        // outer acceptance [deg]
    Double_t xl[max];         // detector thickness [mm]
    Double_t yl[max];        // inner radius       [mm]
    Double_t zl[max];        // outer radius       [mm]
  
    Double_t da[max];        // inner acceptance [deg]
    Double_t ha[max];        // outer acceptance [deg]
    Double_t zla[max];        // outer acceptance [deg]
    Double_t xa[max];         // detector thickness [mm]
    Double_t ya[max];        // inner radius       [mm]
    Double_t za[max];        // outer radius       [mm]
  
    Double_t dg[max];        // inner acceptance [deg]
    Double_t hg[max];        // outer acceptance [deg]
    Double_t zlg[max];        // outer acceptance [deg]
    Double_t xg[max];         // detector thickness [mm]
    Double_t yg[max];        // inner radius       [mm]
    Double_t zg[max];        // outer radius       [mm]
    Int_t n,nn,na,ng;


    //ifstream geoin;
    //geoin.open("secontarget.dat");
    //ofstream geoout("nada.geo");

    Double_t fAbsorberThickness = 0.1;  //Material mm
    Double_t fGapThickness      = 0.03;  //Silicon  mm
    Int_t  NbOfLayers        = 20;       //21
  
    Double_t lt = fAbsorberThickness + fGapThickness;
    Double_t stt = NbOfLayers*lt; 
    Double_t stsyz       =  stt + 1.5;
    Double_t hole = 0.75; //mm the radius of the hole(pipe)
    Double_t SecTargThick = stt;
    Double_t LayThick= lt;
    Double_t SecTargSizeYZ = stsyz;
    
    TVector3* v2;
    TGeoRotation* rot = new TGeoRotation();
    TGeoRotation* rot2 = new TGeoRotation();
    char tempname[100];
    char name4[100];

    TGeoVolume *BoxAsmb[4];
    

    TGeoVolume *LayAsmb[80];
    TGeoBBox *LayB;

    TGeoBBox *BoxSi[80];
    TGeoBBox *BoxAb[80];
    TGeoVolume *LaySi[80];
    TGeoVolume *LayAb[80];
    TGeoTube *pipe;
    TGeoVolume *hyppipe;
    int d;
    d=0;
    for(int i=0;i<4;i++){
    
      //int i =0;
      rot->SetAngles(0,0,0);
      rot2->SetAngles(0,0,0);
      rot->RotateX(90);
      rot->RotateZ((-1*i)*90.0);
      rot2->RotateZ(-1*i*90.0);
      //cout<<"  matr "<<-1*i*90.0<<endl;

      v2 = new TVector3(rotate(TVector3((stsyz/2)-hole,stsyz/2,0.),rot2));
      sprintf(tempname,"stgQuad%d",i+1);
      sprintf(name4,"transQuad%d",i+1);

      TGeoCombiTrans* trc5 = new TGeoCombiTrans(v2->x(), v2->y(), v2->z(), 
						new TGeoRotation (*rot));
      trc5->SetName(name4);
      trc5->RegisterYourself();

      BoxAsmb[d]= new TGeoVolumeAssembly(tempname);
      complex->AddNode(BoxAsmb[d],0,trc5);
      
      d = d+1;
      }
   
    string tmaterial[4]={"HYPdiamond","HYPdiamond","beryllium","aluminium"};
    char tmp[100];
    char tmplay[100],tmplayb[100],tmpAbs[100], tmpGap[100],tmpVAbs[100], tmpVGap[100];
    char name[100], name1[100],name2[100];
   
    Double_t firstPosition = -0.5*SecTargThick + 0.5*LayThick;
    Double_t theta = (40*TMath::Pi())/180.;
    Double_t lengthpar = (hole+0.5*LayThick)/TMath::Tan(theta);
    Double_t firstLength = 0.5*SecTargSizeYZ + lengthpar;
    //Double_t firstLength = SecTargSizeYZ - ((SecTargThick)/TMath::Tan(theta));
    //Double_t firstLength = SecTargSizeYZ - ((SecTargThick-0.5*LayThick)/TMath::Tan(theta));
    Double_t lastLength = SecTargSizeYZ;
 
    TVector3 Lay;
    TVector3 v_Pos;
    TVector3 vecAbs,vecGap;
   
    
    
    //fNbOfLayers = NbOfLayers;
   
    Double_t fStartPosition = firstPosition;
    Double_t fHalfLayThick = 0.5*LayThick;
    Double_t fSpacing  = LayThick;
    Double_t fHalfLengthFirst = 0.5 * firstLength;
    Double_t fHalfLengthIncr =  0.5 *(lastLength-firstLength)/NbOfLayers;
    Double_t fHalfLastLength = 0.5*lastLength;
   
    Double_t param[6]={fStartPosition,fHalfLayThick,fSpacing,fHalfLengthFirst,
		       fHalfLengthIncr,fHalfLastLength};

    //for(Int_t k=0; k<4; k++)
    //{
	//geoin>>n>>dlt[k]>>hlt[k]>>zlt[k];
	//cout<<"thick"<<dlt[k]<<endl;

	for(Int_t i=0; i<20; i++)
	  {

	    //Lay.SetXYZ(0.,0.,0.);
	    //v_Pos.SetXYZ(0.,0.,0.);
	    
	    
	    v_Pos= ComputeTransformation(i,param);
	    
	    Lay = ComputeDimensions(i,param);
	    
	     v_Pos.SetY(-v_Pos.y()); 
	     //(k==0 ? v_Pos.SetZ(-v_Pos.z()): v_Pos.SetZ(v_Pos.z()));
	     v_Pos.SetZ(-v_Pos.z());
	      
	     dl[i] = Lay.X(); //length
	      hl[i] = Lay.Y(); //parametrised length
	      zll[i] = Lay.Z(); //thickness
	      
	      xl[i] = v_Pos.x();
	      yl[i] = v_Pos.y();
	      zl[i] = v_Pos.z();
	      
	      //absorver layer dimension//
	      
	      da[i]= dl[i];
	      ha[i]= hl[i];
	      zla[i]= fAbsorberThickness/2;
	      
	      //absorver layer placement 

	      //(k==0 ? vecAbs.SetZ(-fGapThickness/2.0) : vecAbs.SetZ(fGapThickness/2.0));
	      vecAbs.SetZ(-fGapThickness/2.0);

	      xa[i] = vecAbs.x();
	      ya[i] = vecAbs.y();
	      za[i] = vecAbs.z();

	      //Gap layer dimension//
	      
	      dg[i]= dl[i];
	     hg[i]= hl[i]; 
	     zlg[i]= fGapThickness/2.0;

	      //Gap layer placement 

	     //(k==0 ? vecGap.SetZ(fAbsorberThickness/2) : vecGap.SetZ(-fAbsorberThickness/2));
	     vecGap.SetZ(fAbsorberThickness/2);

	      xg[i] = vecGap.x();
	      yg[i] = vecGap.y();
	      zg[i] = vecGap.z();
	     

	     	 
	  }
	//} 
   

    int t,j;

   

    for(Int_t j=0; j<4;j++)
      {
	
	if(j==0){
	  t=0;
	  for(Int_t k=0; k<20; k++)
	    {	
	      
	      //sprintf(tmp,"stgQuad#0%d",j);
	      //sprintf(tmpAbs,"stglA%d%d%d#01",j,x,k);//target
	      //sprintf(tmpGap,"stglS%d%d%d#01",j,x,k);//silicon
	      
	      //sprintf(tmplayb,"stgLayB_%d",k);
	      sprintf(tmplay,"stgLay%d",k);
	      sprintf(tmpAbs,"stgBlAb%d",k);
	      sprintf(tmpGap,"stgBlSi%d",k);
	      sprintf(tmpVAbs,"stglAb%d",k);
	      sprintf(tmpVGap,"stglSi%d",k);

	      sprintf(name,"transAb%d",k);
	      sprintf(name1,"transSi%d",k);
	      sprintf(name2,"transLay%d",k);
	   
	      LayAsmb[t]= new TGeoVolumeAssembly(tmplay);
	      // LayB=new TGeoBBox("stgLayB",dl[j][k],hl[j][k],zll[j][k]);
	      // LayAsmb= new TGeoVolume("stgLay",LayB,gGeoMan->GetMedium("vacuum"));
	      
	      
	      
	      BoxAb[t] = new TGeoBBox(tmpAbs,da[k],ha[k],zla[k]);
	      LayAb[t] = new TGeoVolume(tmpVAbs,BoxAb[t],gGeoMan->GetMedium("HYPdiamond"));
	      BoxSi[t] = new TGeoBBox(tmpGap,dg[k],hg[k],zlg[k]);
	      LaySi[t] = new TGeoVolume(tmpVGap,BoxSi[t],gGeoMan->GetMedium("HYPsilicon"));
	      LaySi[t]->SetLineColor(kPink);
	      LayAb[t]->SetLineColor(kYellow);
	      TGeoCombiTrans* trc1 = new TGeoCombiTrans(xa[k],ya[k],za[k],new TGeoRotation ());
	      trc1->SetName(name);
	      trc1->RegisterYourself();
	      LayAsmb[t]->AddNode(LayAb[t],k, trc1);
	      TGeoCombiTrans* trc2 = new TGeoCombiTrans(xg[k],yg[k],zg[k],new TGeoRotation ());
	      trc2->SetName(name1);
	      trc2->RegisterYourself();
	      LayAsmb[t]->AddNode(LaySi[t],k, trc2);
	      TGeoCombiTrans* trc3 = new TGeoCombiTrans(xl[k],yl[k],zl[k], new TGeoRotation ());
	      trc3->SetName(name2);
	      trc3->RegisterYourself();
	      BoxAsmb[j]->AddNode(LayAsmb[t],k,trc3);
	      t = t+1;
	    }
	}
	
	if(j==1){
	  t=20;
	  for(Int_t k=20; k<40; k++)
	    {	
	      
	      //sprintf(tmp,"stgQuad#0%d",j);
	      //sprintf(tmpAbs,"stglA%d%d%d#01",j,x,k);//target
	      //sprintf(tmpGap,"stglS%d%d%d#01",j,x,k);//silicon
	      
	      // sprintf(tmplayb,"stgLayB_%d",k);
	      sprintf(tmplay,"stgLay%d",k);
	      sprintf(tmpAbs,"stgBlAb%d",k);
	      sprintf(tmpGap,"stgBlSi%d",k);
	      sprintf(tmpVAbs,"stglAb%d",k);
	      sprintf(tmpVGap,"stglSi%d",k);
	      
	      sprintf(name,"transAb%d",k);
	      sprintf(name1,"transSi%d",k);
	      sprintf(name2,"transLay%d",k);

	      LayAsmb[t]= new TGeoVolumeAssembly(tmplay);
	      // LayB=new TGeoBBox("stgLayB",dl[j][k],hl[j][k],zll[j][k]);
	      // LayAsmb= new TGeoVolume("stgLay",LayB,gGeoMan->GetMedium("vacuum"));
	      
	      
	      
	      BoxAb[t] = new TGeoBBox(tmpAbs,da[k-20],ha[k-20],zla[k-20]);
	      LayAb[t] = new TGeoVolume(tmpVAbs,BoxAb[k],gGeoMan->GetMedium("HYPdiamond"));
	      BoxSi[t] = new TGeoBBox(tmpGap,dg[k-20],hg[k-20],zlg[k-20]);
	      LaySi[t] = new TGeoVolume(tmpVGap,BoxSi[t],gGeoMan->GetMedium("HYPsilicon"));
	      LaySi[t]->SetLineColor(kPink);
	      LayAb[t]->SetLineColor(kYellow);
	      TGeoCombiTrans* trc1 = new TGeoCombiTrans(xa[k-20],ya[k-20],za[k-20],new TGeoRotation ());
	      trc1->SetName(name);
	      trc1->RegisterYourself();
	      LayAsmb[t]->AddNode(LayAb[t],k, trc1);

	      TGeoCombiTrans* trc2 = new TGeoCombiTrans(xg[k-20],yg[k-20],zg[k-20],new TGeoRotation ());
	      trc2->SetName(name1);
	      trc2->RegisterYourself();
	      LayAsmb[t]->AddNode(LaySi[t],k, trc2);

	      TGeoCombiTrans* trc3 = new TGeoCombiTrans(xl[k-20],yl[k-20],zl[k-20], new TGeoRotation ());
	      trc3->SetName(name2);
	      trc3->RegisterYourself();
	      BoxAsmb[j]->AddNode(LayAsmb[t],k,trc3);

	      t = t +1;
	    }
	}
	
	if(j==2){
	  t=40;
	  for(Int_t k=40; k<60; k++)
	    {	
		
	      //sprintf(tmp,"stgQuad#0%d",j);
	      //sprintf(tmpAbs,"stglA%d%d%d#01",j,x,k);//target
	      //sprintf(tmpGap,"stglS%d%d%d#01",j,x,k);//silicon
	      
	      // sprintf(tmplayb,"stgLayB_%d",k);
	      sprintf(tmplay,"stgLay%d",k);
	      sprintf(tmpAbs,"stgBlAb%d",k);
	      sprintf(tmpGap,"stgBlSi%d",k);
	      sprintf(tmpVAbs,"stglAb%d",k);
	      sprintf(tmpVGap,"stglSi%d",k);
	      
	      sprintf(name,"transAb%d",k);
	      sprintf(name1,"transSi%d",k);
	      sprintf(name2,"transLay%d",k);

	      LayAsmb[t]= new TGeoVolumeAssembly(tmplay);
	      // LayB=new TGeoBBox("stgLayB",dl[j][k],hl[j][k],zll[j][k]);
	      // LayAsmb= new TGeoVolume("stgLay",LayB,gGeoMan->GetMedium("vacuum"));
	      
	      
	      
	      BoxAb[t] = new TGeoBBox(tmpAbs,da[k-40],ha[k-40],zla[k-40]);
	      LayAb[t] = new TGeoVolume(tmpVAbs,BoxAb[t],gGeoMan->GetMedium("HYPdiamond"));
	      BoxSi[t] = new TGeoBBox(tmpGap,dg[k-40],hg[k-40],zlg[k-40]);
	      LaySi[t] = new TGeoVolume(tmpVGap,BoxSi[t],gGeoMan->GetMedium("HYPsilicon"));
	      LaySi[t]->SetLineColor(kPink);
	      LayAb[t]->SetLineColor(kYellow);

	      TGeoCombiTrans* trc1 = new TGeoCombiTrans(xa[k-40],ya[k-40],za[k-40],new TGeoRotation ());
	      trc1->SetName(name);
	      trc1->RegisterYourself();
	      LayAsmb[t]->AddNode(LayAb[t],k, trc1);
	      
	      TGeoCombiTrans* trc2 = new TGeoCombiTrans(xg[k-40],yg[k-40],zg[k-40],new TGeoRotation ());
	      trc2->SetName(name1);
	      trc2->RegisterYourself();
	      LayAsmb[t]->AddNode(LaySi[t],k, trc2);

	      TGeoCombiTrans* trc3 = new TGeoCombiTrans(xl[k-40],yl[k-40],zl[k-40], new TGeoRotation ());
	      trc3->SetName(name2);
	      trc3->RegisterYourself();
	      BoxAsmb[j]->AddNode(LayAsmb[t],k,trc3);
	      t = t +1;
	    }
	}
	
	if(j==3){
	  t = 60;
	  for(Int_t k=60; k<80; k++)
	    {	
	      
	      //sprintf(tmp,"stgQuad#0%d",j);
	      //sprintf(tmpAbs,"stglA%d%d%d#01",j,x,k);//target
	      //sprintf(tmpGap,"stglS%d%d%d#01",j,x,k);//silicon
	      
	      // sprintf(tmplayb,"stgLayB_%d",k);
	      sprintf(tmplay,"stgLay%d",k);
	      sprintf(tmpAbs,"stgBlAb%d",k);
	      sprintf(tmpGap,"stgBlSi%d",k);
	      sprintf(tmpVAbs,"stglAb%d",k);
	      sprintf(tmpVGap,"stglSi%d",k);
	      
	      sprintf(name,"transAb%d",k);
	      sprintf(name1,"transSi%d",k);
	      sprintf(name2,"transLay%d",k);
	      
	      LayAsmb[t]= new TGeoVolumeAssembly(tmplay);
	      // LayB=new TGeoBBox("stgLayB",dl[j][k],hl[j][k],zll[j][k]);
	      // LayAsmb= new TGeoVolume("stgLay",LayB,gGeoMan->GetMedium("vacuum"));
	      
	      
	      
	      BoxAb[t] = new TGeoBBox(tmpAbs,da[k-60],ha[k-60],zla[k-60]);
	      LayAb[t] = new TGeoVolume(tmpVAbs,BoxAb[t],gGeoMan->GetMedium("HYPdiamond"));
	      BoxSi[t] = new TGeoBBox(tmpGap,dg[k-60],hg[k-60],zlg[k-60]);
	      LaySi[t] = new TGeoVolume(tmpVGap,BoxSi[t],gGeoMan->GetMedium("HYPsilicon"));
	      LaySi[t]->SetLineColor(kPink);
	      LayAb[t]->SetLineColor(kYellow);

	      TGeoCombiTrans* trc1 = new TGeoCombiTrans(xa[k-60],ya[k-60],za[k-60],new TGeoRotation ());
	      trc1->SetName(name);
	      trc1->RegisterYourself();
	      LayAsmb[t]->AddNode(LayAb[t],k, trc1);
	      TGeoCombiTrans* trc2 = new TGeoCombiTrans(xg[k-60],yg[k-60],zg[k-60],new TGeoRotation ());
	      trc2->SetName(name1);
	      trc2->RegisterYourself();
	      LayAsmb[t]->AddNode(LaySi[t],k, trc2);
	      TGeoCombiTrans* trc3 = new TGeoCombiTrans(xl[k-60],yl[k-60],zl[k-60], new TGeoRotation ());
	      trc3->SetName(name2);
	      trc3->RegisterYourself();
	      BoxAsmb[j]->AddNode(LayAsmb[t],k,trc3);
	      
	      t = t+1;
	    }
	  
	}
	
      }
    
    
    pipe = new TGeoTube("stghypPipe",0.69,0.7,10.);
    hyppipe = new TGeoVolume("stglpipe",pipe,gGeoMan->GetMedium("HYPcarbon"));
    
    TGeoCombiTrans* trc4 = new TGeoCombiTrans(0., 0., 0., new TGeoRotation ());

    trc4->SetName("transpipe");
    trc4->RegisterYourself();
    complex->AddNode(hyppipe, 0,trc4);

    top->AddNode(complex,0,new TGeoCombiTrans(0., 0., -76.5, new TGeoRotation ()));
    
    gGeoMan->CloseGeometry();
    top->Write();
    fi->Close();
    //top->Draw("ogl");				 
  return 0;
}
	
