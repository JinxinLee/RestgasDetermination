// Martin Galuska




void MakeResolutionPlot(TProfile *histo)
{
	// I wrote this to get the error bars from a TProfile plot with name histo and put it into a new plot,
	// but this is not really what I need, maybe it is useful another time
	gStyle->SetOptStat(0);
	TString histotitle = "Relative resolution for ";
	histotitle += histo->GetTitle();
	TH1F *hCoordinateSystem = new TH1F(histotitle, histotitle, 10, 0.5, 5);
	hCoordinateSystem->GetYaxis()->SetTitle("relative p_{z} resolution");
	hCoordinateSystem->GetXaxis()->SetTitle("p_{z} from MC Truth [GeV/c]");


	TGraphErrors *resolution = new TGraphErrors();
	resolution->SetMarkerColor(2);
	resolution->SetMarkerStyle(29);
	resolution->SetMarkerSize(1.4);
	resolution->SetLineColor(2);



	// produce resolution plots from TProfile histo
	const Int_t nBins = histo->GetNbinsX();
	//	std::cout << "nBins = " << nBins << std::endl;
	//	vector<Double_t> mom;
	//	vector<Double_t> stdDev;

	for (Int_t iBin =1; iBin <= nBins; ++iBin)
	{
		//		mom.push_back(histo->GetBinCenter(iBin));
		//		stdDev.push_back(histo->GetBinError(iBin)/histo->GetBinCenter(iBin));
		resolution->SetPoint(iBin, histo->GetBinCenter(iBin), histo->GetBinError(iBin)/histo->GetBinCenter(iBin));
		resolution->SetPointError(iBin, histo->GetBinWidth(iBin)/2.,histo->GetBinError(iBin)/TMath::Sqrt(histo->GetBinEntries(iBin)));

	}

	hCoordinateSystem->Draw();
	resolution->Draw("L,SAME");

}









void plotTrackCands()
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();


	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(10); // entries only
	gStyle->SetOptFit(1);
	//  gStyle->SetOptStat(11); // name and entries
	//  gStyle->SetOptStat(1111110); // with integral


	const Int_t nBins = 90; // 90 very detailed


	// ******************************************************************
	// Get the tree nt which was produced with mctrack.C before
	TString inTreeFile  = "out_test.root";
	TFile *inFile = TFile::Open(inTreeFile,"READ");
	TTree *tree=(TTree *) inFile->Get("nt") ;


	Int_t nEvents = nt->GetEntriesFast();
	// some counting variables
	Int_t skipcount = 0, moreThanTwentyPercentCount = 0, goodEvents = 0;
	Double_t totalDeviation = 0.;


	// ******************************************************************
	// just some histo titles
	TString histotitle = "p_{z} for ";
	histotitle += nEvents;
	histotitle += " #mu^{-} with 0.5 GeV/c < p < 5 GeV/c ";
	TString histotitleinvpz = histotitle + "(from 1/p_{z} HS)";
	TString histotitleinvpzWithB = histotitle + "(from 1/p_{z} HS with mean B field)";
	TString histotitlepz = histotitle + "(from p_{z} HS)";
	TString histotitlepzWithB = histotitle + "(from p_{z} HS with mean B field)";



	// ******************************************************************
	// histograms for pz (several methods can be compared)
	TProfile *hpzFromInvpz  = new TProfile("pzFrominvpz",histotitleinvpz, nBins, 0.5, 5, 0, 8, "s");
	//  TH2F *hpzFromInvpz  = new TH2F("pzFrominvpz",histotitleinvpz, nBins, 0.5, 5, 80, 0, 8);
	hpzFromInvpz->GetYaxis()->SetTitle("reconstructed p_{z} [GeV/c]");
	hpzFromInvpz->GetXaxis()->SetTitle("p_{z} from MC Truth [GeV/c]");

	TProfile* hpzFromInvpzWithBfield = new TProfile("pzFrominvpzWithB", histotitleinvpzWithB, nBins, 0.5, 5, 0, 8, "s");
	hpzFromInvpzWithBfield->GetYaxis()->SetTitle("reconstructed p_{z} [GeV/c]");
	hpzFromInvpzWithBfield->GetXaxis()->SetTitle("p_{z} from MC Truth [GeV/c]");

	TProfile* hpzFrompz = new TProfile("pzFrompz", histotitlepz, nBins, 0.5, 5, 0, 8, "s");
	hpzFrompz->GetYaxis()->SetTitle("reconstructed p_{z} [GeV/c]");
	hpzFrompz->GetXaxis()->SetTitle("p_{z} from MC Truth [GeV/c]");

	TProfile* hpzFrompzWithBfield = new TProfile("pzFrompzWithB", histotitlepzWithB, nBins, 0.5, 5, 0, 8, "s");
	hpzFrompzWithBfield->GetYaxis()->SetTitle("reconstructed p_{z} [GeV/c]");
	hpzFrompzWithBfield->GetXaxis()->SetTitle("p_{z} from MC Truth [GeV/c]");


	// ******************************************************************
	// histogram to visualize what the line Hough transform resulted with
	TH1F* hhitshiftinx = new TH1F("hitshiftinx", "hitshiftinx for single track #mu^{-} p = 3 GeV/c", 200, -50, 50);
	hhitshiftinx->GetXaxis()->SetTitle("hitshiftinx [cm]");
	hhitshiftinx->GetYaxis()->SetTitle("Counts");




	// ******************************************************************


	//	TH1F *hresolution15invpz = new TH1F("Relative resolution for muons with 1.0 < p_{z} < 1.5 GeV/c", "Relative resolution for muons with 1.0 < p_{z} < 1.5 GeV/c", bins, -1.0, 1.0);
	//	hresolution15invpz->GetYaxis()->SetTitle("counts");
	//	hresolution15invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution2invpz = new TH1F("Relative resolution for muons with 1.5 < p_{z} < 2.0 GeV/c", "Relative resolution for muons with 1.5 < p_{z} < 2.0 GeV/c", bins, -1.0, 1.0);
	//	hresolution2invpz->GetYaxis()->SetTitle("counts");
	//	hresolution2invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution25invpz = new TH1F("Relative resolution for muons with 2.0 < p_{z} < 2.5 GeV/c", "Relative resolution for muons with 2.0 < p_{z} < 2.5 GeV/c", bins, -1.0, 1.0);
	//	hresolution25invpz->GetYaxis()->SetTitle("counts");
	//	hresolution25invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution3invpz = new TH1F("Relative resolution for muons with 2.5 < p_{z} < 3.0 GeV/c", "Relative resolution for muons with 2.5 < p_{z} < 3.0 GeV/c", bins, -1.0, 1.0);
	//	hresolution3invpz->GetYaxis()->SetTitle("counts");
	//	hresolution3invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution35invpz = new TH1F("Relative resolution for muons with 3.0 < p_{z} < 3.5 GeV/c", "Relative resolution for muons with 3.0 < p_{z} < 3.5 GeV/c", bins, -1.0, 1.0);
	//	hresolution35invpz->GetYaxis()->SetTitle("counts");
	//	hresolution35invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution4invpz = new TH1F("Relative resolution for muons with 3.5 < p_{z} < 4.0 GeV/c", "Relative resolution for muons with 3.5 < p_{z} < 4.0 GeV/c", bins, -1.0, 1.0);
	//	hresolution4invpz->GetYaxis()->SetTitle("counts");
	//	hresolution4invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution45invpz = new TH1F("Relative resolution for muons with 4.0 < p_{z} < 4.5 GeV/c", "Relative resolution for muons with 4.0 < p_{z} < 4.5 GeV/c", bins, -1.0, 1.0);
	//	hresolution45invpz->GetYaxis()->SetTitle("counts");
	//	hresolution45invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");
	//
	//	TH1F *hresolution5invpz = new TH1F("Relative resolution for muons with 4.5 < p_{z} < 5.0 GeV/c", "Relative resolution for muons with 4.5 < p_{z} < 5.0 GeV/c", bins, -1.0, 1.0);
	//	hresolution5invpz->GetYaxis()->SetTitle("counts");
	//	hresolution5invpz->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");




	// ******************************************************************
	// Variables and their mapping in the tree
	Float_t mc_px = 0., mc_py = 0., mc_pz = 0., mc_theta = 0., mc_theta_xz = 0., mc_phi = 0.;
	Int_t mc_pid = 0;
	Float_t nHitsForHoughSpaceLine = 0, nHitsForHoughSpaceParabola = 0;
	Float_t pzFromInvpz = 0., pzFrompz = 0., pzFromInvpzWithBField = 0., pzFrompzWithBField = 0., hitshiftinx =0.;
	Float_t thetaPeakLine = 0., thetaPeakParabolaFrominvpz = 0., thetaPeakParabolaFrompz = 0.;
	Float_t mc_theta = 0., mc_theta_xz = 0.;

	nt->SetBranchAddress("mc_pz", &mc_pz);
	nt->SetBranchAddress("mc_theta", &mc_theta);
	nt->SetBranchAddress("mc_theta_xz", &mc_theta_xz);
	nt->SetBranchAddress("pzFromInvpz", &pzFromInvpz);
	nt->SetBranchAddress("pzFromInvpzWithBField", &pzFromInvpzWithBField);
	nt->SetBranchAddress("pzFrompz", &pzFrompz);
	nt->SetBranchAddress("pzFrompzWithBField", &pzFrompzWithBField);
	nt->SetBranchAddress("hitshiftinx", &hitshiftinx);
	nt->SetBranchAddress("nHitsForHoughSpaceLine",&nHitsForHoughSpaceLine);
	nt->SetBranchAddress("nHitsForHoughSpaceParabola",&nHitsForHoughSpaceParabola);
	nt->SetBranchAddress("thetaPeakLine", &thetaPeakLine);
	nt->SetBranchAddress("thetaPeakParabolaFrompz", &thetaPeakParabolaFrompz);
	nt->SetBranchAddress("thetaPeakParabolaFrominvpz", &thetaPeakParabolaFrominvpz);


	const Int_t maxMomResPics = 90; // adjust this, 90 is correct for momSpacingForResPlot = 0.05
	const Int_t picsPerCanvas = 12; // keep this the same
	const Int_t maxCanvas = maxMomResPics/picsPerCanvas+1;
	// for momentum resolution plot
	Int_t ResolutionX = 800, ResolutionY = 600; // for plotting
	TCanvas *cresinvpz[maxCanvas];
	Double_t xVal[maxMomResPics];
	Double_t yVal[maxMomResPics];
	Double_t xErr[maxMomResPics];
	Double_t yErr[maxMomResPics];

	// histograms for resolution studies
	const Int_t bins = 200;
	TH1F *hresolution1invpz[maxMomResPics];
	TF1 *func1[maxMomResPics]; // used for fitting histogram above





	for (Int_t iMomRes=0; iMomRes<maxMomResPics; ++iMomRes)
	{
		const Double_t momSpacingForResPlot = 0.05;
		Double_t low = 0.5+momSpacingForResPlot*iMomRes;
		Double_t high = low + momSpacingForResPlot;
		// reset histogram
		hresolution1invpz[iMomRes] = new TH1F("Relative resolution for muons with low*iMomRes < p_{z} < high GeV/c", "Relative resolution for muons with low*iMomRes < p_{z} < high GeV/c", bins, -1.0, 1.0);
		hresolution1invpz[iMomRes]->GetYaxis()->SetTitle("counts");
		hresolution1invpz[iMomRes]->GetXaxis()->SetTitle("(reconstructed p_{z} - p_{z} from MC Truth)/ p_{z} from MC Truth");

		Int_t iCanvas = iMomRes/picsPerCanvas;
		if (0==iMomRes%picsPerCanvas){
			cresinvpz[iCanvas]=new TCanvas("cresinvpz"+iCanvas,"cresinvpz",ResolutionX,ResolutionY);
			cresinvpz[iCanvas]->Divide(4,3);
		}

		// ******************************************************************
		// event loop starts here
		for (Int_t iEvent=0; iEvent< nEvents; ++iEvent)
		{
			nt->GetEntry(iEvent);
			if (0 == iEvent%1000) { cout << "processing event " << iEvent << "\n"; }

			if (2 >= nHitsForHoughSpaceLine) { continue; }
			if (4 >= nHitsForHoughSpaceParabola) { continue; }



			// correct according to angle (try from line or from parabola)
			pzFromInvpz = pzFromInvpz*cos(thetaPeakParabolaFrominvpz/180.*3.14159265);

			// The Following values apply to theta from parabola invpz no B field plot, angle "correction" with pzFromInvpz = pzFromInvpz*cos(thetaPeakParabolaFrominvpz/180.*3.14159265); // momentum resolution is improved, but not much
			const Double_t p0invpz = -0.160255;
			const Double_t p1invpz = 1.11037;
			pzFromInvpz = (pzFromInvpz-p0invpz)/p1invpz;












			if (maxMomResPics==iMomRes+1){
				// make 2d plots
				hpzFromInvpz->Fill(mc_pz,pzFromInvpz);
				hpzFromInvpzWithBfield->Fill(mc_pz,pzFromInvpzWithBField);
				hpzFrompz->Fill(mc_pz,pzFrompz);
				hpzFrompzWithBfield->Fill(mc_pz,pzFrompzWithBField);
				hhitshiftinx->Fill(hitshiftinx);
			}

			// ******************************************************************
			// calculate relative deviation for pz
			Double_t relativeDeviation_pz = (pzFromInvpz-mc_pz)/mc_pz;

			if (maxMomResPics==iMomRes+1){
				// count how many events deviate at least 20% (This number is not represented in the sigma from the Gauss fits)
				if (0.2 <= relativeDeviation_pz)
				{
					++moreThanTwentyPercentCount;
					std::cout << "Event " << iEvent << " MC p_z = " << mc_pz << " FTS p_z = " << pzFromInvpz << " is off by " << relativeDeviation_pz*100 << " per cent." << std::endl;
				}
				totalDeviation += fabs(relativeDeviation_pz);
				goodEvents += 1; // all events that pass the continue criteria are counted as good (this number is needed to calculate the average deviation after the event loop)
			}



			// here comes the plotting stuff of the momentum resolution


			if ((low < mc_pz) && (mc_pz <= high))
			{

				//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
				hresolution1invpz[iMomRes]->Fill(relativeDeviation_pz);
			}

		} // end of event loop
		// ******************************************************************







		// ******************************************************************
		// plotting begins here



		cresinvpz[iCanvas]->cd(iMomRes-iCanvas*picsPerCanvas+1);
		func1[iMomRes] = new TF1("func1","gaus",-0.4,0.4);
		TF1 *funcptr = func1[iMomRes];
		int maxbin = hresolution1invpz[iMomRes]->GetMaximumBin();
		funcptr->SetParameters(hresolution1invpz[iMomRes]->GetBinContent(maxbin), hresolution1invpz[iMomRes]->GetBinCenter(maxbin), 0.1);
		//		TF1 *func1[iMomRes] = new TF1("func1","gaus+gaus(3)",-0.4,0.4);
		//		func1[iMomRes]->SetParameters(hresolution1invpz[iMomRes]->GetBinContent(maxbin), hresolution1invpz[iMomRes]->GetBinCenter(maxbin), 0.05, hresolution1invpz[iMomRes]->GetMaximum(),0.,0.1);
		//		TFitResultPtr fitresult = hresolution1invpz[iMomRes]->Fit(func1[iMomRes],"RS","",-0.4,0.4);
		hresolution1invpz[iMomRes]->Fit(funcptr,"R","",-0.4,0.4);
		hresolution1invpz[iMomRes]->Draw("ep");


		Double_t sigma = funcptr->GetParameter(2);
		Double_t sigmaErr = funcptr->GetParError(2);


		xVal[iMomRes]=(low+high)/2.0;
		yVal[iMomRes]=sigma;
		xErr[iMomRes]=momSpacingForResPlot/2.0;
		yErr[iMomRes]=sigmaErr;

		cout << "sigma = " <<  sigma << endl;
		cout << "sigmaerror = " << sigmaErr << endl;


		cout << "End of momentum loop " << iMomRes << " with events from "<< low << " to " << high << " GeV/c"<< endl;

	} // end of loop for momentum resolution plot





	TCanvas *cMomRes = new TCanvas("c1","A Simple Graph with error bars",800,600);
	cMomRes->SetGrid();
	TGraphErrors *momResPlot = new TGraphErrors(maxMomResPics,xVal,yVal,xErr,yErr);
	momResPlot->SetTitle("Momentum Resolution");
	momResPlot->SetMarkerColor(4);
	momResPlot->SetMarkerStyle(21);
	momResPlot->Draw("AP");
//	momResPlot->GetYaxis()->SetTitle("std. dev. of (p_{z,rec} - p_{z, MC})/ p_{z, MC}");
	momResPlot->GetYaxis()->SetTitle("relative p_{z} resolution");
	momResPlot->SetMinimum(0.0);
	momResPlot->SetMaximum(0.09);
	momResPlot->GetXaxis()->SetTitle("p_{z, MC} [GeV/c]");




	Int_t ResolutionX = 400, ResolutionY = 1200; // for plotting

	TCanvas *cinvpz = new TCanvas("cpzMCvsHoughFromInvpz", "cpzMCvsHoughFromInvpz", ResolutionX, ResolutionY);
	TF1 *p1fit = new TF1("P1","pol1",0.5,4.95);
	hpzFromInvpz->Fit(p1fit,"RS");
	hpzFromInvpz->Draw();

	//
	//  TCanvas *cinvpzWithBfield = new TCanvas("cpzMCvsHoughFromInvpzWithBfield", "cpzMCvsHoughFromInvpzWithBfield", ResolutionX, ResolutionY);
	//  hpzFromInvpzWithBfield->Fit(p1fit,"R");
	//  hpzFromInvpzWithBfield->Draw();
	//
	//  TCanvas *cpz = new TCanvas("cpzMCvsHoughFrompz", "cpzMCvsHoughFrompz", ResolutionX, ResolutionY);
	//  hpzFrompz->Fit(p1fit,"R");
	//  hpzFrompz->Draw();
	//
	//  TCanvas *cpzWithBfield = new TCanvas("cpzMCvsHoughFrompzWithBfield", "cpzMCvsHoughFrompzWithBfield", ResolutionX, ResolutionY);
	//  hpzFrompzWithBfield->Fit(p1fit,"R");
	//  hpzFrompzWithBfield->Draw();

	Int_t ResolutionX = 800, ResolutionY = 600; // for plotting

	TCanvas *chitshiftinx= new TCanvas("chitshiftinx", "chitshiftinx", ResolutionX, ResolutionY);
	hhitshiftinx->Draw();






	std::cout << moreThanTwentyPercentCount << " from " << goodEvents << " events deviate more than 20 per cent from MC truth value! Mean absolute relative deviation for pz is " << totalDeviation/goodEvents*100 << " per cent."  << std::endl;


	// That is probably not what I want

	//
	//  TCanvas *crespzFromInvpz= new TCanvas("crespzFromInvpz", "crespzFromInvpz", ResolutionX, ResolutionY);
	//  MakeResolutionPlot(hpzFromInvpz);
	//
	//  TCanvas *crespzFromInvpz= new TCanvas("cpzFromInvpzWithBfield", "cpzFromInvpzWithBfield", ResolutionX, ResolutionY);
	//  MakeResolutionPlot(hpzFromInvpzWithBfield);
	//
	//  TCanvas *crespzFromInvpz= new TCanvas("cpzFrompz", "cpzFrompz", ResolutionX, ResolutionY);
	//  MakeResolutionPlot(hpzFrompz);
	//
	//  TCanvas *crespzFromInvpz= new TCanvas("cpzFrompzWithBfield", "cpzFrompzWithBfield", ResolutionX, ResolutionY);
	//  MakeResolutionPlot(hpzFrompzWithBfield);


	//	// Covariance Matrix of fit
	//
	//	for (int i=0; i<6; ++i){
	//		for (int j=0; j<6; ++j){
	//			cout << fitresult->CovMatrix(i,j) << " ";
	//		}
	//		cout << "\n";
	//	}



}








// Easier for testing:


//   TNtuple *nt = new TNtuple("nt","nt","evt:mc_px:mc_py:mc_pz:mc_theta_xz:mc_theta:mc_phi:mc_pid:pzFromInvpz:pzFromInvpzWithBField:pzFrompz:pzFrompzWithBField:hitshiftinx");


gROOT->SetStyle("Plain");

TCut nocut("nt.nHitsForHoughSpaceLine>=0");
TCut minhitsforline("nt.nHitsForHoughSpaceLine>1");
TCut minhitsforparabola("nt.nHitsForHoughSpaceParabola>2");

TCut largerthetaxy("nt.mc_theta_xz>0.");
TCut smallhitshift("nt.hitshiftinx<20.");

nt->Draw("nt.mc_pz:nt.pzFromInvpz>>(100,0,5,300,0,15)",minhitsforline&&minhitsforparabola, "");











//if (mc_pz < 0.5)
//{
//	++skipcount;
//	//		std::cout << "Event nr. " << iEvent << " is skipped, because MC truth pz of " << mc_pz << "is below 0.8 GeV/c. p_z is off by " << relativeDeviation_pz*100 << "per cent.  " << skipcount << " events skipped!" << std::endl;
//} // low momenta give weird distribution
//else
//	if (mc_pz < 1.0)
//	{
//		//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
//		hresolution1invpz->Fill(relativeDeviation_pz);
//	}
//	else
//		if (mc_pz < 1.5)
//		{
//			//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
//			hresolution15invpz->Fill(relativeDeviation_pz);
//		}
//		else
//			if (mc_pz < 2.0)
//			{
//				//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
//				hresolution2invpz->Fill(relativeDeviation_pz);
//			}
//			else
//				if (mc_pz < 2.5)
//				{
//					hresolution25invpz->Fill(relativeDeviation_pz);
//				}
//				else
//					if (mc_pz < 3.0)
//					{
//						//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
//						hresolution3invpz->Fill(relativeDeviation_pz);
//					}
//					else
//						if (mc_pz < 3.5)
//						{
//							hresolution35invpz->Fill(relativeDeviation_pz);
//						}
//						else
//							if (mc_pz < 4.0)
//							{
//								//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
//								hresolution4invpz->Fill(relativeDeviation_pz);
//							}
//							else
//								if (mc_pz < 4.5)
//								{
//									//		if ((20. > hitshiftinx)&&(10. < hitshiftinx)) {continue;} // try to figure out why I see a double Gaussian
//									hresolution45invpz->Fill(relativeDeviation_pz);
//								}
//								else
//								{
//									hresolution5invpz->Fill(relativeDeviation_pz);
//								}
//
//} // end of event loop
// ******************************************************************
//
//
//
// This is the old plotting stuff
//	// ******************************************************************
//	// plotting begins here
//
//	Int_t ResolutionX = 400, ResolutionY = 1200; // for plotting
//
//	TCanvas *cinvpz = new TCanvas("cpzMCvsHoughFromInvpz", "cpzMCvsHoughFromInvpz", ResolutionX, ResolutionY);
//	TF1 *p1fit = new TF1("P1","pol1",0.5,4.95);
//	hpzFromInvpz->Fit(p1fit,"RS");
//	hpzFromInvpz->Draw();
//
//	//
//	//  TCanvas *cinvpzWithBfield = new TCanvas("cpzMCvsHoughFromInvpzWithBfield", "cpzMCvsHoughFromInvpzWithBfield", ResolutionX, ResolutionY);
//	//  hpzFromInvpzWithBfield->Fit(p1fit,"R");
//	//  hpzFromInvpzWithBfield->Draw();
//	//
//	//  TCanvas *cpz = new TCanvas("cpzMCvsHoughFrompz", "cpzMCvsHoughFrompz", ResolutionX, ResolutionY);
//	//  hpzFrompz->Fit(p1fit,"R");
//	//  hpzFrompz->Draw();
//	//
//	//  TCanvas *cpzWithBfield = new TCanvas("cpzMCvsHoughFrompzWithBfield", "cpzMCvsHoughFrompzWithBfield", ResolutionX, ResolutionY);
//	//  hpzFrompzWithBfield->Fit(p1fit,"R");
//	//  hpzFrompzWithBfield->Draw();
//
//	Int_t ResolutionX = 800, ResolutionY = 600; // for plotting
//
//	TCanvas *chitshiftinx= new TCanvas("chitshiftinx", "chitshiftinx", ResolutionX, ResolutionY);
//	hhitshiftinx->Draw();
//
//	TCanvas *ctest=new TCanvas("ctest","ctest",ResolutionX,ResolutionY);
//	hresolution1invpz->Draw("ep");
//
//	TCanvas *cresinvpz=new TCanvas("cresinvpz","cresinvpz",ResolutionX,ResolutionY);
//	cresinvpz->Divide(3,3);
//
//
//	cresinvpz->cd(1);
//	TF1 *func1 = new TF1("func1","gaus",-0.4,0.4);
//	int maxbin = hresolution1invpz->GetMaximumBin();
//	func1->SetParameters(hresolution1invpz->GetBinContent(maxbin), hresolution1invpz->GetBinCenter(maxbin), 0.15);
////		TF1 *func1 = new TF1("func1","gaus+gaus(3)",-0.4,0.4);
////		func1->SetParameters(hresolution1invpz->GetBinContent(maxbin), hresolution1invpz->GetBinCenter(maxbin), 0.05, hresolution1invpz->GetMaximum(),0.,0.1);
//	TFitResultPtr fitresult = hresolution1invpz->Fit(func1,"RS","",-0.4,0.4);
//	hresolution1invpz->Draw("ep");
//
//	cresinvpz->cd(2);
//	TF1 *func15 = new TF1("func15","gaus",-0.4,0.4);
//	func15->SetParameters(hresolution15invpz->GetMaximum(), 0., 0.15);
//	hresolution15invpz->Fit(func15,"R","",-0.4,0.4);
//	hresolution15invpz->Draw("ep");
//
//	cresinvpz->cd(3);
//	TF1 *func2 = new TF1("func2","gaus",-0.4,0.4);
//	func2->SetParameters(hresolution2invpz->GetMaximum(), 0., 0.15);
//	hresolution2invpz->Fit(func2,"R","",-0.4,0.4);
//	hresolution2invpz->Draw("ep");
//
//	cresinvpz->cd(4);
//	TF1 *func25 = new TF1("func25","gaus",-0.4,0.4);
//	func25->SetParameters(hresolution25invpz->GetMaximum(), 0., 0.15);
//	hresolution25invpz->Fit(func25,"R","",-0.4,0.4);
//	hresolution25invpz->Draw("ep");
//
//	cresinvpz->cd(5);
//	TF1 *func3 = new TF1("func3","gaus",-0.4,0.4);
//	func3->SetParameters(hresolution3invpz->GetMaximum(), 0., 0.15);
//	hresolution3invpz->Fit(func3,"R","",-0.4,0.4);
//	hresolution3invpz->Draw("ep");
//
//	cresinvpz->cd(6);
//	TF1 *func35 = new TF1("func35","gaus",-0.4,0.4);
//	func35->SetParameters(hresolution35invpz->GetMaximum(), 0., 0.15);
//	hresolution35invpz->Fit(func35,"R","",-0.4,0.4);
//	hresolution35invpz->Draw("ep");
//
//	cresinvpz->cd(7);
//	TF1 *func4 = new TF1("func4","gaus",-0.4,0.4);
//	func4->SetParameters(hresolution4invpz->GetMaximum(), 0., 0.15);
//	hresolution4invpz->Fit(func4,"R","",-0.4,0.4);
//	hresolution4invpz->Draw("ep");
//
//	cresinvpz->cd(8);
//	TF1 *func45 = new TF1("func45","gaus",-0.4,0.4);
//	func45->SetParameters(hresolution45invpz->GetMaximum(), 0., 0.15);
//	hresolution45invpz->Fit(func45,"R","",-0.4,0.4);
//	hresolution45invpz->Draw("ep");
//
//	cresinvpz->cd(9);
//	TF1 *func5 = new TF1("func5","gaus",-0.4,0.4);
//	func5->SetParameters(hresolution5invpz->GetMaximum(), 0., 0.15);
//	hresolution5invpz->Fit(func5,"R","",-0.4,0.4);
//	hresolution5invpz->Draw("ep");





