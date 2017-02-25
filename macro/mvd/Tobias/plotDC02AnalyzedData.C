double FindMax(TFile* f, TString select)
{
	TList* l = f->GetListOfKeys();
	TIter iter(l);
	TObject *ob;
	TString maxName;

	double max, oldMax = 0;
	while (ob = iter())	{
		if (TString(ob->GetName()).Contains(select))	{
			TH2D* histo = (TH2D*) (f->Get(ob->GetName()));
			max = histo->GetMaximum();
			//			std::cout << "Maxmium: " << max << std::endl;
			if (max > oldMax){
				oldMax = max;
				maxName = histo->GetName();
			}
		}
	}
	std::cout << "Maximum counts in: " << maxName << " " << oldMax << std::endl;
	return oldMax;
}

int plotDC02AnalyzedData(TString fileName)
{
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libEve");
	gROOT->LoadMacro("DrawEveHisto.C");

	TFile* f = new TFile(fileName);
	TGeoManager* fGeoMan = new TGeoManager("geoMan","geoMan");
	fGeoMan->Import("Mvd_GridUrqmdSim.root");
	PndGeoHandling gH(gGeoManager);

	TEveManager::Create();

//	TIter iter2(l);

	TEveCompound* comp = new TEveCompound("RadDamHistos");
	TEveCompound* pixelBL1 = new TEveCompound("PixelBL1");
	TEveCompound* pixelBL2 = new TEveCompound("PixelBL2");
	TEveCompound* pixelSDK = new TEveCompound("PixelSDK");
	TEveCompound* pixelMDK = new TEveCompound("PixelMDK");
	TEveCompound* strip		= new TEveCompound("Strip");
	TEveCompound* stripBL3  = new TEveCompound("StripBL3");
	TEveCompound* stripBL4  = new TEveCompound("StripBL4");
	TEveCompound* stripLDK  = new TEveCompound("StripLDK");
	comp->AddElement(pixelBL1);
	comp->AddElement(pixelBL2);
	comp->AddElement(pixelSDK);
	comp->AddElement(pixelMDK);
	comp->AddElement(strip);
	strip->AddElement(stripBL3);
	strip->AddElement(stripBL4);
	strip->AddElement(stripLDK);

	double pixMax = FindMax(f, "1_1");
	double stripMax = FindMax(f, gH->GetVolumeID("StripActiveoRectL"));

	stripMax = 60;

//	std::cout << "max: " << max << " stripMax: " << stripMax << std::endl;

//	std::cout << "OldMax: " << oldMax << std::endl;
	TList* l = f->GetListOfKeys();
	TIter iter(l);
	TObject* ob;

	while(ob = iter()){
		if (TString(ob->GetName()).Contains("1_1")){
			TEveCompound* tempComp;
			double tempMax;
			if (TString(ob->GetName()).Contains(gH->GetVolumeID("PixeloBl1ov2-NEW"))){
				tempComp = pixelBL1;
				tempMax = pixMax;
			}
			else if (TString(ob->GetName()).Contains(gH->GetVolumeID("PixeloBl2ov3-NEW"))){
				tempComp = pixelBL2;
				tempMax = pixMax;
			}
			else if (TString(ob->GetName()).Contains(gH->GetVolumeID("PixeloSdk-v2-NEW"))){
				tempComp = pixelSDK;
				tempMax = pixMax;
			}
			else if (TString(ob->GetName()).Contains(gH->GetVolumeID("PixeloMdkov1-NEW"))){
				tempComp = pixelMDK;
				tempMax = pixMax;
			}
			else if (TString(ob->GetName()).Contains(gH->GetVolumeID("StripoBl3"))){
				tempComp = stripBL3;
				tempMax = stripMax;
			}
			else if (TString(ob->GetName()).Contains(gH->GetVolumeID("StripoBL4")) &&
					 !TString(ob->GetName()).Contains(gH->GetVolumeID("StripActiveoTrapL"))){
				tempComp = stripBL4;
				tempMax = stripMax;
			}
			else{
				tempComp = stripLDK;
				tempMax = stripMax;
			}
			TH2D* histo = (TH2D*)(f->Get(ob->GetName()));
			PndStringVector sVec(histo->GetName());
			TGeoHMatrix* matrix = gH.GetMatrixId(sVec.Replace("o","/"));
//			std::cout << "OldMax2: " << oldMax << std::endl;
			DrawEveHisto(histo, matrix, tempMax, tempComp);
		}
	}

	TGeoNode *N=  gGeoManager->GetTopNode();
	TEveGeoTopNode *TNod=new  TEveGeoTopNode(gGeoManager, N);
	gEve->AddGlobalElement(TNod);
	gEve->AddGlobalElement(comp);
	gEve->Redraw3D(kTRUE);
  return 0;
}
