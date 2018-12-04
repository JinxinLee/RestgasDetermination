// root macro to analyze the radiation length of sub-detector components
// requires a simulation file with activated RadLenRegister and RadLenPoints
// To test different sub-detectors or components of them only modify the marked parts of the anaRadLength() macro

struct histos {

    histos(TString compName, std::function<bool(TString&)> selector, int thetaRes = 181, int phiRes = 361,
            float thetaStart = -0.5, float thetaEnd = 180.5, float phiStart = -180.5, float phiEnd = 180.5):
    fComponentName(compName), fSelector(selector), fRadLengthOnTrack(10, 0.0)
    {
        TString radLen ="RadiationLength_";
        radLen+=compName;
        hisRadLen = new TH1D(radLen.Data(), radLen.Data(), 1000,0,100);
        hisRadLen->GetXaxis()->SetTitle("Radiation Length [%]");

        TString radLen2D = "RadiationLength2D_";
        radLen2D+=compName;
        hisRadLen2D = new TH2D(radLen2D.Data(), radLen2D.Data(), thetaRes, thetaStart, thetaEnd, phiRes, phiStart, phiEnd);
        hisRadLen2D->GetXaxis()->SetTitle("Theta [/°]");
        hisRadLen2D->GetYaxis()->SetTitle("Phi [/°]");
        TString radLenCount = "RadiationLengthCount_";
        radLenCount+=compName;
        hisRadLenCount = new TH2D(radLenCount.Data(), radLenCount.Data(), thetaRes, thetaStart, thetaEnd, phiRes, phiStart, phiEnd);

    }

    void Write(){
        hisRadLen->Write();
        hisRadLen2D->Write();
        hisRadLenCount->Write();
    }

    std::function<bool(TString&)> fSelector;
    TString fComponentName;

    TH1D* hisRadLen;
    TH2D* hisRadLen2D;
    TH2D* hisRadLenCount;

    std::vector<double> fRadLengthOnTrack;

};

void anaRadLength()
{

  gStyle->SetPalette(1);
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  std::vector<histos*> histosVector;

  PndFileNameCreator namecreator("radlength_geantinos"); //Set file prefix here
  std::string inFile = namecreator.GetSimFileName();
  std::string parFile = namecreator.GetParFileName();
  std::string outFile = namecreator.GetCustomFileName("radLenHistos");

  TFile* out = new TFile(outFile.c_str(),"RECREATE");

  TFile* f = new TFile(inFile.c_str());
  TTree *t=(TTree *) f->Get("pndsim") ;


  TFile* par = new TFile(parFile.c_str());
  (TGeoManager*)par->Get("FairGeoParSet");

  // The part below is to be modified by the user

  int  nEvents = 1000000; //Set it to the number of events to be analyzed. If the value is bigger than the number of events in the sim file it is ignored
  int startEvent = 0;
  bool verbose = false;

  //Add your sub detector here: Give an individual name and a selector which returns true if a unique part of the component name is in the geo path of the object.
  //If the steps for theta and phi are not one degree, the number of steps has to be adopted
  histosVector.push_back(new histos("stt", [](TString& path){ return path.Contains("stt"); }, 181, 361));
  histosVector.push_back(new histos("stt_support", [](TString& path){ return (path.Contains("stt") && !path.Contains("tubestt")); }));
  histosVector.push_back(new histos("stt_straw", [](TString& path){ return (path.Contains("stt") && path.Contains("tubestt")); }));
  histosVector.push_back(new histos("mvd", [](TString& path){ return (path.Contains("Mvd")); }));
  histosVector.push_back(new histos("gem", [](TString& path){ return (path.Contains("Gem")); }));
  //End of the part which should be modified by the user


  TClonesArray* radlen_array = new TClonesArray("FairRadLenPoint");
  t->SetBranchAddress("RadLen", &radlen_array);

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TVector3 vecs,veco;
  std::map<int,int> trackHitMap;

  for (Int_t j=startEvent; j<(nEvents+startEvent) && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    //if (verbose)
    if (j % 100 == 0)
        cout<<">>>> Event No "<<j<<endl;
    for (auto detector : histosVector){
        detector->fRadLengthOnTrack.clear();
        detector->fRadLengthOnTrack.resize(10,0.0);
    }

    for (Int_t i=0; i<radlen_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
       FairRadLenPoint *point=(FairRadLenPoint*)radlen_array->At(i);
      if (verbose) cout << "Track ID: " << point->GetTrackID() << std::endl;
      if (point->GetTrackID() > 10) continue;           //larger trackIDs are usually from secondaries one could even think to reduce it to == 0
      TVector3 pos, posOut, res;
      pos = point->GetPosition();
      posOut = point->GetPositionOut();
      res = posOut - pos;
      PndMCTrack* myTrack = (PndMCTrack*)(mc_array->At(point->GetTrackID()));
      if (myTrack == nullptr) {
         // std::cout << "MyTrack Nullptr" << std::endl;
          continue;
      }
      TGeoNode* node = gGeoManager->FindNode(point->GetX(),point->GetY(),point->GetZ());
      if( 0==node)
      {
        std::cout<<"Warning: There is a node not defined properly!"<<std::endl;
        continue;
      }
      node->cd();
      TString detname = gGeoManager->GetPath();
      if (myTrack->GetMotherID() < 0){
	      if (verbose){
	    	  std::cout << "Time: " << point->GetTime() << " Length: " << point->GetLength() << std::endl;
	    	  std::cout << "Pos: " << pos.x() << "/" << pos.y() << "/" << pos.z();
	    	  std::cout << " OutPos: " << posOut.x() << "/" << posOut.y() << "/" << posOut.z() << std::endl;
	    	  std::cout << "TrackLength: " << res.Mag() << " RadLength: " << point->GetRadLength() << " Res: " << res.Mag() / point->GetRadLength() << std::endl;
	    	  std::cout << "Detname: " << detname.Data() << std::endl;
	      }
	      for(auto detector : histosVector) {
	          //std::cout << "if Statement " << point->GetTrackID() << " selector " << detector->fSelector(detname) << std::endl;
	          if (point->GetTrackID() > -1 && point->GetTrackID() < 10 && detector->fSelector(detname)){
	              detector->fRadLengthOnTrack[point->GetTrackID()] += res.Mag()/point->GetRadLength()*100;
	              //std::cout << detector->fRadLengthOnTrack[point->GetTrackID()] << std::endl;
	          }
	      }
      }
    } // end of radLength Array
    //for (int k = 0; k < RadLengthOnTrack.size(); k++){
    for(auto detector : histosVector) {
        int k = 0;
          if ( detector->fRadLengthOnTrack.size() > 0 && detector->fRadLengthOnTrack[k] > 0){
              PndMCTrack* mcTrack = (PndMCTrack*)(mc_array->At(k));

              detector->hisRadLen->Fill(detector->fRadLengthOnTrack[k]);
              detector->hisRadLen2D->Fill(mcTrack->GetMomentum().Theta()*TMath::RadToDeg(), mcTrack->GetMomentum().Phi()*TMath::RadToDeg(), detector->fRadLengthOnTrack[k]);
              detector->hisRadLenCount->Fill(mcTrack->GetMomentum().Theta()*TMath::RadToDeg(), mcTrack->GetMomentum().Phi()*TMath::RadToDeg());
          }
        }
  }

    for(auto detector : histosVector) {
        detector->hisRadLen2D->Divide(detector->hisRadLenCount);

      TCanvas* can1 = new TCanvas();
      can1->Divide(2,1);

      gStyle->SetPalette(1);
      can1->cd(1);
      detector->hisRadLen->DrawCopy();
      can1->cd(2);
      detector->hisRadLen2D->DrawCopy("colz");

      out->cd();
      detector->Write();
    }

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
