Bool_t create_stof(TGeoManager *gGeoMan, TGeoVolume *top,TGeoVolume *unitVol){
	// all position parameters in cm
	// side tof plate geometry parameters
	//-----------------------------
	//-- macro created by Alicia S. based on 3d pdf drawing of the PANDA detector (Panda_v912.pdf)
	//--

	// PANDA Coordinate System
	//	y    x
	//	^  /
	//	|/
	//	|----> z

	// Detector sketch


	//            ____
	//           /|
	//		    / |           y
	//		   /  |           ^
	//	   ___/   |           |
	//		 |    |           |
	//	 dy1 |    | dy2       |
	//		 |    |        ----------------> z
	//		 |    |           |
	//	  ___|-dz-|           |
	//		  \   |           |
	//		   \  |           |
	//		    \ |
	//           \|___


	// Positions in cm (PANDA coordinate system)
	const Double_t zDistStof = 500;
	const Double_t xInnerDistStof = 279.6;
	const Double_t yMiddleStof = 0.;

	// Dimensions
	const Double_t  kdX    = 2.0;   // half length (cm) = dx/2. //from 3d pdf
	const Double_t  kdY1    = 45.0;   // half length (cm) = dy1/2. closer to int. point //from 3d pdf
	const Double_t  kdY2    = 53.0;   //half length (cm) = dy2/2. further from int. point //from 3d pdf
	const Double_t  kdZ    = 48.0;   //half length (cm) = dz/2. //from 3d pdf

	TGeoVolumeAssembly* subunitVol = new TGeoVolumeAssembly("Stof_plates");


	char nameShape[13];
	char nameVol[13];

	TGeoTrd1 *detShape[2];
	TGeoVolume* detVol[2];



	for(int i=0;i<2;i++){



		sprintf (nameShape,"PlateShape%d",i);
		sprintf (nameVol,"Stof_plate%d",i);

		detShape[i] = new TGeoTrd1(nameShape, kdY1, kdY2, kdX, kdZ); // for stof plate y has to vary with z
		detVol[i] = new TGeoVolume(nameVol,detShape[i],gGeoMan->GetMedium("polyvinyltoluene"));

		Int_t sign = pow(-1,i); // plus/minus 1
		TGeoRotation rot;
		rot.RotateZ(90.);
		Double_t xPos = xInnerDistStof/2.+kdX;
		TGeoCombiTrans* transMat= new TGeoCombiTrans(sign*xPos,0.,0.,new TGeoRotation (rot));

		transMat->SetName(nameShape);
		transMat->RegisterYourself();
		subunitVol->AddNode(detVol[i],i,transMat);
	}



	unitVol->AddNode(subunitVol,0,new TGeoCombiTrans(0.,yMiddleStof,zDistStof+kdZ,new TGeoRotation ()));
	top->AddNode(unitVol,0,new TGeoCombiTrans(0.,0.,0.,new TGeoRotation ()));
	return kTRUE;
}






Bool_t create_ftofwall(Double_t zDistFtofWall, TGeoManager *gGeoMan, TGeoVolume *top, TGeoVolume *unitVol)
{
	// Forward tof wall geometry parameters
	//-----------------------------
	//-- macro created by Alicia S. based on
	//-- Panda TPR and on previous geant3 geometry for forward tof (Vladimir V.)
	const Double_t  kBCentX    = 2.5;   // half length (cm) //from EMC TDR
	const Double_t  kBCentY    = 70.0;   // half length (cm) //?
	const Double_t  kBCentZ    = 0.75;   // half length (cm) //?

	const Double_t  kBBeamX    = 2.5;   // half length (cm) //from EMC TDR
	const Double_t  kBBeamY    = 32.5;  // half length (cm) //?
	const Double_t  kBBeamZ    = 0.75;   // half length (cm) //?

	const Double_t  kBVertX    = 5.0;   // half length (cm) //from EMC TDR
	const Double_t  kBVertY    = 70.0;   // half length (cm) //?
	const Double_t  kBVertZ    = 0.75;   // half length (cm) //from EMC TDR

	TGeoBBox *CentShape[22];
	TGeoBBox *BeamShape[22];
	TGeoBBox *VertShape[46];

	TGeoVolume* CentVol[22];
	TGeoVolume* BeamVol[22];
	TGeoVolume* VertVol[46];


	Double_t bx[22],by[22],bz[22];
	Double_t bvx[46],bvy[46],bvz[46];
	TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Ftof_strips");
        

	char name[13];
	char namB[13];
	char namV[13];
	char name1[13];
	char namB1[13];
	char namV1[20];
	char name2[13];
	char namB2[13];
	char namV2[13];


	for(int i=0;i<22;i++){
		bx[i]=-40+kBCentX*((2*i)+1);
	}


	Double_t offset;
	for(int i=0;i<22;i++){
		if(i<9||i>12){

			sprintf (name,"Centshape%d",i);
			sprintf (name1,"Ftof_Central_Strip%d",i);

			CentShape[i] = new TGeoBBox(name,kBCentX,kBCentY,kBCentZ);
			CentVol[i] = new TGeoVolume(name1,CentShape[i],gGeoMan->GetMedium("polyvinyltoluene"));
			TGeoCombiTrans* trc1=0;
			if(i<9) trc1= new TGeoCombiTrans(bx[i],0,0.,new TGeoRotation ());
			else if (i>12) trc1= new TGeoCombiTrans(bx[i-2],0,0.,new TGeoRotation ());

			trc1->SetName(name);
			trc1->RegisterYourself();
			SubunitVol->AddNode(CentVol[i],i,trc1);
		}
		else{

			sprintf (namB,"Beamshape%d",i);
			sprintf (namB1,"Ftof_Beam_Strip%02d",i);
			BeamShape[i] = new TGeoBBox(namB,kBBeamX,kBBeamY,kBBeamZ);
			BeamVol[i] = new TGeoVolume(namB1,BeamShape[i],gGeoMan->GetMedium("polyvinyltoluene"));

			if(i==9||i==10){


				(i==9 ? (offset = -70+kBBeamY) : (offset = 70-kBBeamY));
				TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[9],offset,0.,new TGeoRotation ());

				trc1->SetName(namB);
				trc1->RegisterYourself();
				SubunitVol->AddNode(BeamVol[i],i,trc1);

			}
			if(i==11||i==12){

				(i==11 ?  (offset = -70+kBBeamY) : (offset = 70-kBBeamY));

				TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[10],offset,0.,new TGeoRotation ());

				trc1->SetName(namB);
				trc1->RegisterYourself();
				SubunitVol->AddNode(BeamVol[i],i,trc1);

			}
		}

	}

	for(int i=0;i<24;i++){
		bvx[i]=-280+kBVertX*((2*i)+1);
	}

	for(int i=0;i<22;i++){

		bvx[i+24]=60+kBVertX*((2*i)+1);
	}

	for(int i=0;i<46;i++){


		sprintf (namV,"VertShape%d",i);
		sprintf (namV1,"Ftof_Vertical_Strip%d",i);

		VertShape[i] = new TGeoBBox(namV,kBVertX,kBVertY,kBVertZ);
		VertVol[i] = new TGeoVolume(namV1,VertShape[i],gGeoMan->GetMedium("polyvinyltoluene"));
		TGeoCombiTrans* trc1= new TGeoCombiTrans(bvx[i],0,0.,new TGeoRotation ());

		trc1->SetName(namV);
		trc1->RegisterYourself();
		SubunitVol->AddNode(VertVol[i],i,trc1);
	}

	unitVol->AddNode(SubunitVol,0,new TGeoCombiTrans(0.,0.,zDistFtofWall,new TGeoRotation ()));
	top->AddNode(unitVol,0,new TGeoCombiTrans(0.,0.,0.,new TGeoRotation ()));
	return kTRUE;
}







void create_ftof_stof_rootgeo(Double_t zDistFtofWall = 775){


	//--------------------------------------------------------------------
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	// Load this libraries
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libPndData");
	gSystem->Load("libPassive");

	TString outfile= "../../geometry/ftofwall+stof.root";
	TFile* fi = new TFile(outfile,"RECREATE");

	FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile("../../geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();

	FairGeoMedia *Media =  geoFace->getMedia();
	FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

	FairGeoMedium *MediumSci  = Media->getMedium("polyvinyltoluene");


	Int_t nmed=geobuild->createMedium(MediumSci);


	TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

	TGeoVolume *top = new TGeoVolumeAssembly("Ftof");

	gGeoMan->SetTopVolume(top);


	TGeoVolume *merge_fstof = new TGeoVolumeAssembly("FStof");
	//merging the two assemblies volumes ftof and stof as unit

	create_ftofwall(zDistFtofWall, gGeoMan, top,merge_fstof);
	std::cout << "\nftof wall was created.\n";

	create_stof(gGeoMan, top,merge_fstof);
	std::cout << "stof side plates were created in dipole region.\n\n";












	gGeoMan->CloseGeometry();
	top->Write();
	fi->Close();
	//   gGeoManager->Export(outfile);
	top->Draw("ogl");
}  
