// Macros used to visualize the event by plotting
// B field
// MC points
// hits
// found track candidates


#ifndef HOUGH_TEST
#define HOUGH_TEST


#include <math.h>


#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLorentzVector.h"
#include "TStyle.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TGraph.h"
#include "TSpectrum2.h"


#include <stdio.h>
#include <iostream>
#include <iomanip>
using namespace std;

// Panda Libs
//#include "RhoBase/TCandidate.h"
//#include "RhoBase/TCandList.h"
//#include "RhoBase/TFitParams.h"
//#include "RhoBase/VAbsMicroCandidate.h"
//#include "RhoSelector/TPidSelector.h"

#include "FairLogger.h"
//#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"

#include "PndFtsHit.h"
#include "FairMCPoint.h"
#include "FairHit.h"


//#include "AnalysisTools/PndEventReader.h"
#include "AnalysisTools/PndMcTruthMatch.h"
#include "AnalysisTools/PndAnalysis.h"

#include "AnalysisTools/Fitter/Pnd4CFitter.h"
#include "AnalysisTools/Fitter/PndKinVtxFitter.h"
#include "AnalysisTools/Fitter/PndKinFitter.h"

#include "AnalysisTools/PndAnaPidCombiner.h"
#include "AnalysisTools/PndAnaPidSelector.h"
#include "FairRunSim.h"
#include "PndMultiField.h"
#include "PndMCTrack.h"










void PlotMCTracks(int iEvent, TString branchName, TString tcaName, TGraph *mcPoints)
{
	// File that you want to access needs to be opened first (and not other file after it), because the file is accessed via gFile
	// TODO I could also pass in the handle instead of using gFile

	// Uncomment if you want to test the routine without calling it in a macro (by copy pasting into the interpreter)
	// TString branchName="FTSPoint"; TString tcaName="PndFtsPoint";
	// TString branchName="STTPoint"; TString tcaName="PndSttPoint";

	int verbose = 0;


	TTree* tSim = (TTree*) (gFile->Get("cbmsim"));
	//tSim->StartViewer();
	if (verbose > 5)
	{
		std::cout << "Got the tree" << std::endl;
	}

	TClonesArray* Points = new TClonesArray(tcaName);
	if (verbose > 5)
	{
		std::cout << "Created new TCA" << std::endl;
	}

	tSim->SetBranchAddress(branchName, &Points);
	if (verbose > 5)
	{
		std::cout << "Set the branchName" << std::endl;
	}

	TBranch* br = tSim->FindBranch(branchName);
	if (verbose > 5)
	{
		std::cout << "Found the branch" << std::endl;
	}

	if (br == 0 || br->GetEntries() == 0 || iEvent >= br->GetEntries())
		return;
	if (verbose > 3)
	{
		std::cout << "Passed the test" << std::endl;
	}

	tSim->GetEntry(iEvent); // Look at event iEvent
	const Int_t nPoints = Points->GetEntriesFast();
	// store coordinates of points for plotting in TGraph
	Double_t X[nPoints];
	Double_t Y[nPoints]; // not used for plotting
	Double_t Z[nPoints];
	if (verbose > 2)
	{
		std::cout << "Number of points: " << nPoints << std::endl;
	}

	for (int iPoint = 0; iPoint < nPoints; iPoint++)
	{
		TObject* obj = Points->At(iPoint);
		if (iPoint < 1 && verbose > 5)
		{
			std::cout << obj->ClassName() << std::endl;
			std::cout << "obj->InheritsFrom(FairMCPoint) = "
					<< obj->InheritsFrom("FairMCPoint") << std::endl;
			std::cout << "obj->InheritsFrom(FairHit) = "
					<< obj->InheritsFrom("FairHit") << std::endl;
		}
		TVector3 point;
		if (obj->InheritsFrom("FairMCPoint"))
		{
			FairMCPoint* myPoint = (FairMCPoint*) obj;
			myPoint->Position(point);
		}
		else if (obj->InheritsFrom("FairHit"))
		{
			FairHit* myHit = (FairHit*) obj;
			myHit->Position(point);
		}
		else
		{
			std::cout
			<< "Error, the hit or point does neither inherit from FairHit nor from FairMCPoint \n";
		}

		// Hit coordinates
		X[iPoint] = point.X();
		Y[iPoint] = point.Y();
		Z[iPoint] = point.Z();

		//mcPoints->Fill(Z, X);

		if (verbose > 2)
		{
			std::cout << "Point " << iPoint << " : ";
			//myPoint->Print();
			std::cout << "Z=" << Z[iPoint] << "cm X=" << X[iPoint] << "cm Y=" << Y[iPoint] << "cm"
					<< std::endl;
		}

	}

	if (0<nPoints)
	{
		mcPoints->DrawGraph(nPoints, Z, X, "LP,SAME");
		//		DrawGraph(Int_t n, const Double_t *x=0, const Double_t *y=0, Option_t *option="");
	}


	tSim->ResetBranchAddresses();
	Points->Delete(); // maybe Points->Clear(); would be enough...
}


void PlotMCTracksPrintBField(PndMultiField *fField, int iEvent,
		TString branchName, TString tcaName, TH2D* mcPoints)
{
	// File that you want to access needs to be opened first (and not other file after it), because the file is accessed via gFile
	// TODO I could also pass in the handle instead of using gFile

	// Uncomment if you want to test the routine without calling it in a macro (by copy pasting into the interpreter)
	// TString branchName="FTSPoint"; TString tcaName="PndFtsPoint";
	// TString branchName="STTPoint"; TString tcaName="PndSttPoint";

	int verbose = 6;

	TTree* tSim = (TTree*) (gFile->Get("cbmsim"));
	//tSim->StartViewer();
	if (verbose > 5)
	{
		std::cout << "Got the tree" << std::endl;
	}

	TClonesArray* Points = new TClonesArray(tcaName);
	if (verbose > 5)
	{
		std::cout << "Created new TCA" << std::endl;
	}
	tSim->SetBranchAddress(branchName, &Points);
	if (verbose > 5)
	{
		std::cout << "Set the branchName" << std::endl;
	}
	TBranch* br = tSim->FindBranch(branchName);
	if (verbose > 5)
	{
		std::cout << "Found the branch" << std::endl;
	}
	if (br == 0 || br->GetEntries() == 0 || iEvent >= br->GetEntries())
		return;
	if (verbose > 3)
	{
		std::cout << "Passed the test" << std::endl;
	}
	tSim->GetEntry(iEvent); // Look at event iEvent
	if (verbose > 2)
	{
		std::cout << "Number of points: " << Points->GetEntriesFast()
						<< std::endl;
	}
	for (int iPoint = 0; iPoint < Points->GetEntriesFast(); iPoint++)
	{
		TObject* obj = Points->At(iPoint);
		if (iPoint < 1 && verbose > 5)
		{
			std::cout << obj->ClassName() << std::endl;
			std::cout << "obj->InheritsFrom(FairMCPoint) = "
					<< obj->InheritsFrom("FairMCPoint") << std::endl;
			std::cout << "obj->InheritsFrom(FairHit) = "
					<< obj->InheritsFrom("FairHit") << std::endl;
		}
		TVector3 point;
		if (obj->InheritsFrom("FairMCPoint"))
		{
			FairMCPoint* myPoint = (FairMCPoint*) obj;
			myPoint->Position(point);
		}
		else if (obj->InheritsFrom("FairHit"))
		{
			FairHit* myHit = (FairHit*) obj;
			myHit->Position(point);
		}
		else
		{
			std::cout
			<< "Error, the hit or point does neither inherit from FairHit nor from FairMCPoint \n";
		}

		// Hit coordinates
		Double_t X = point.X();
		Double_t Y = point.Y();
		Double_t Z = point.Z();

		mcPoints->Fill(Z, X);

		// Get the magnetic field at the point location
		// This draws the magnetic field in pointer fField into histogram Bfield

		Double_t po[3], BB[3];
		Double_t Btot = 0;

		po[0] = X;
		po[1] = Y;
		po[2] = Z;

		fField->GetFieldValue(po, BB); //return value in KG (G3)

		if (verbose > 2)
		{
			std::cout << "Point " << iPoint << " : ";
			//myPoint->Print();
			std::cout << "Z=" << Z << "cm X=" << X << "cm Y=" << Y << "cm  BX="
					<< BB[0] / 10. << "T BY=" << BB[1] / 10. << "T BZ="
					<< BB[2] / 10. << "T" << std::endl;
		}

	}

	mcPoints->SetMarkerColor(2);
	mcPoints->SetMarkerStyle(29);
	mcPoints->SetMarkerSize(1);

	tSim->ResetBranchAddresses();
	Points->Delete(); // maybe Points->Clear(); would be enough...

}









void MakeHoughLineFit(Double_t b, Double_t theta,
		Double_t weight, const Double_t zOffset, TGraph* returnTGraph)
{
	// calculate two points and return result in TGraph*
	const Double_t meinpi = 3.14159265;
	Double_t realtheta = theta / 360. * 2. * meinpi;
	const Double_t tantheta = tan(realtheta);

	const Int_t nPoints = 2;
	Double_t z[nPoints] = { -200., 300. }; // z is in local coordinate system
	Double_t x[nPoints];
	for (Int_t iPoint = 0; iPoint < nPoints; ++iPoint)
	{
		x[iPoint] = tantheta*z[iPoint]+b;
		z[iPoint]+=zOffset; // z has to be shifted into laboratory system before plotting
	}

	returnTGraph->DrawGraph(nPoints,z,x,"LP,SAME") ;
}






Double_t MakeHoughParabolaFitwithBfield(PndMultiField *fField, Double_t pzinv, Double_t theta,
		Double_t weight, Double_t zOffset, Double_t hitshiftinx, Double_t zscaling, const Double_t c,
		const Bool_t keepBConstant, TGraph* ParabolaFitInXZPlane)
{

	int verbose = 0;
	// (zreal=zOffset, xreal=-hitshiftinx) = (zshifted = 0, xshifted = 0)
	// zshifted = zreal - zOffset
	// xshifted = xreal + hitshiftinx
	// zreal = zshifted + zOffset
	// xreal = xshifted - hitshiftinx



	// There is a bug for theta == 0
	// calculate constants
	// There is a bug for By == 0

	// 0 == theta is a special case which I did not want to implement. if statement is a workaround
	if (0 == theta)
	{
		theta = 1. / 100.;
	}

	const Int_t iterationsForBfield = 10; // 10 should be enough

	// for B field access
	Double_t Byminus = 0., Byplus = 0.;
	Double_t BMeanPlus = 0., BMeanMinus;
	Double_t po[3], BB[3];
	Double_t xplusshifted = 0., xminusshifted = 0., lastxminusshifted = 0., lastxplusshifted = 0., lastxminusreal= 0., lastxplusreal = 0., zshifted = 0.;
	po[1] = 0.; // always use B-field for y == 0

	const Double_t meinpi = 3.14159265;
	Double_t realtheta = theta / 360. * 2. * meinpi;
	const Double_t tantheta = tan(realtheta);
	const Double_t pzstuff = 1. / pzinv / 2. / c / tantheta / sin(realtheta);
	const Int_t upperlimit = 500, lowerlimit = -200;
	const Int_t nPoints = ceil((upperlimit-lowerlimit)/zscaling);
	Double_t xplusreal[nPoints];
	Double_t xminusreal[nPoints];
	Double_t zreal[nPoints];
	Int_t howManyPointsInBField = 0;



	if (2<verbose) { std::cout << "hitshiftinx is " << hitshiftinx << std::endl; }

	for (Int_t indexForArrays = 0, zshifted = lowerlimit; indexForArrays < nPoints; ++indexForArrays, zshifted = zshifted + 1. / zscaling)
	{
		// (zreal=zOffset, xreal=-hitshiftinx) = (zshifted = 0, xshifted = 0)
		// zshifted = zreal - zOffset
		// xshifted = xreal + hitshiftinx
		// zreal = zshifted + zOffset
		// xreal = xshifted - hitshiftinx


		zreal[indexForArrays] = zshifted + zOffset;
		// iterate to get the correct B field
		for (Int_t iTry = 0; iTry<iterationsForBfield; ++iTry)
		{

			if (kTRUE == keepBConstant)
			{
				// do not take B field into account for plotting
				Byplus = 1.;
				Byminus = 1.;
				// do not iterate
				iTry = iterationsForBfield+10;
			}
			else
			{
				// try to plot the track taking the B-field in y direction into account

				po[2] = zreal[indexForArrays];

				lastxminusreal = lastxminusshifted-hitshiftinx;
				lastxminusreal = lastxminusshifted-hitshiftinx;


				po[0] = lastxminusreal;
				fField->GetFieldValue(po, BB); //return value in KG (G3)
				Byminus = BB[1] / 10.; // y-component of magnetic field in Tesla

				po[0] = lastxplusreal;
				fField->GetFieldValue(po, BB); //return value in KG (G3)
				Byplus = BB[1] / 10.; // y-component of magnetic field in Tesla

				if (6<verbose)
				{
					std::cout << iTry << ". try: z+zOffset = " << zreal[indexForArrays] << " cm  Byminus = "
							<< Byminus << " T  Byplus = " << Byplus << " T"
							<< std::endl;
					std::cout << "lastxplusreal = " << lastxplusreal << " cm   lastxminusreal = " << lastxminusreal << " cm"	<< std::endl;
				}

				// This is just a workaround as I have to divide by By (parabola becomes straight line if By is small)
				if (0 == Byminus)
				{
					Byminus = 1. / 10000.;
				}
				if (0 == Byplus)
				{
					Byplus = 1. / 10000.;
				}
			}

			Double_t ztantheta = zshifted / tantheta;
			Double_t a = -ztantheta + pzstuff / Byminus;
			xminusshifted = a - sqrt(a * a - zshifted / pzinv / c / sin(realtheta) / Byminus - ztantheta * ztantheta);
			a = -ztantheta + pzstuff / Byplus;
			xplusshifted = a + sqrt(a * a - zshifted / pzinv / c / sin(realtheta) / Byplus - ztantheta * ztantheta);


			lastxminusshifted = xminusshifted;
			lastxplusshifted = xplusshifted;

			//std::cout << "\n\n(z+zOffset, xminus, xplus) = (" << z<< ", " << xminus << ", " << xplus << ")\n\n\n";
		} // iterated to find correct B field

		xplusreal[indexForArrays] = xplusshifted - hitshiftinx;
		xminusreal[indexForArrays] = xminusshifted - hitshiftinx;


		if (zreal[indexForArrays] > 415. && zreal[indexForArrays] < 490.)
		{
			// TODO:: Try alternative: determine maximum of B field

			//the following lines average the B field (probably not what I want to do...)
			++howManyPointsInBField;
			// integrate B field along the path of the particle (might be helpful for reconstructing pz
			po[2] = zreal[indexForArrays];
			po[0] = xplusreal[indexForArrays];
			fField->GetFieldValue(po, BB); //return value in KG (G3)
			BMeanPlus += BB[1]/10.;

			po[0] = xminusreal[indexForArrays];
			fField->GetFieldValue(po, BB); //return value in KG (G3)
			BMeanMinus += BB[1]/10.;
		}

	} // for loop over z coordinate

	BMeanPlus = BMeanPlus/howManyPointsInBField;
	BMeanMinus = BMeanMinus/howManyPointsInBField;

	if (1<verbose)
	{
		cout << "??? BMeanPlus = " << BMeanPlus << std::endl;
		cout << "??? BMeanMinus = " << BMeanMinus << std::endl;
	}

	ParabolaFitInXZPlane->DrawGraph(nPoints, zreal, xplusreal,"P,SAME");
	ParabolaFitInXZPlane->DrawGraph(nPoints, zreal, xminusreal,"P,SAME");
	return max(BMeanPlus,BMeanMinus);
}


// This procedure draws the magnetic field in pointer fField into histogram Bfield
void DrawField(PndMultiField *fField, TH2F* Bfield)
{
	int verbose = 0;
	// For magnetic field access
	Double_t po[3], BB[3];
	Double_t Btot = 0.;

	po[1] = 0.; // set y position to 0 (because FTS currently does not provide a y position

	for (Double_t z = -199.; z < 800.; z += 2.)
	{
		po[2] = z;
		for (Double_t x = -249.; x < 250.; x += 2.)
		{
			//  cout << "X = " << x << endl;
			po[0] = x;
			fField->GetFieldValue(po, BB); //return value in KG (G3)
			if (7<verbose)
			{
				std::cout << "B_y = " << BB[1]/10. << " T for Z = " << po[2] << " cm X = " << po[0] << " cm" << std::endl;
			}
			Btot = TMath::Sqrt(BB[0] * BB[0] + BB[1] * BB[1] + BB[2] * BB[2]);
			Bfield->Fill(z, x, Btot / 10.); // This plots B_total
			//Bfield->Fill(z,x,BB[1]/ 10.); // This plots By = y-component of B field
		}
	}
}











































// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// ------------------------ MACRO STARTS HERE --------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
void plotTrackCands(int iEvent)
{

	UInt_t fVerbose = 0;


	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	//	  basiclibs(); // this gives an error when compiling

	// Load the example libraries
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libField");



	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->SetStyle("Plain");
	gStyle->SetMarkerStyle(20);
	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);



	// -----------------------------------------------------------------------
	// FileNames
	TString DigiFile = "digi_complete.root";
	TString SimFile = "sim_complete.root";
	// -----------------------------------------------------------------------






	// General stuff
	const Double_t BeamMomentum = 15.; //6.991;
	const Double_t meinpi = 3.14159265;
	const Int_t ResolutionX = 800, ResolutionY = 600; // for plotting

	// -----------------------------------------------------------------------

	// for B field access
	Double_t By = 0.;
	Double_t po[3], BB[3];
	Double_t BMeanForParabolapz = 0;
	Double_t BMeanForParabola = 0;

	// -----------------------------------------------------------------------






	// Histograms
	//	TH2D* hits = new TH2D("hits", "hits", 276, 250, 800, 331, -330, 330);
	//	TString mcPointsHistoname = "mcPoints event ";
	//	mcPointsHistoname += iEvent;
	//	TH2D* mcPoints = new TH2D(mcPointsHistoname, mcPointsHistoname, 1200, -200, 1000, 500, -250,
	//			250);

	TH2F* Bfield = NULL;

	Bfield = new TH2F("Bfield", "", 650, -200, 1100, 250, -250, 250);
	Bfield->GetXaxis()->SetTitle("z [cm]");
	Bfield->GetYaxis()->SetTitle("x [cm]");


	// 2014
	//	TH2F* houghspaceLine= new TH2F("houghspaceLine", "houghspaceLine", invthetastepForLine*(thetalimithighForLineHoughSpacePlot-thetalimitlowForLineHoughSpacePlot), thetalimitlowForLineHoughSpacePlot, thetalimithighForLineHoughSpacePlot, invthetastepForLine*40, -50., 50.);
	//	houghspaceLine->GetXaxis()->SetTitle("#theta [^{0}]");
	//	houghspaceLine->GetYaxis()->SetTitle("b [cm]");
	////	TH2D* Fitinxzplane= new TH2D("Fitinxzplane", "Fitinxzplane", 800, 0, 800, 500, -250, 250);
	//	TGraph* FitInXZPlane = new TGraph();
	//	FitInXZPlane->SetMarkerColor(4);
	//	FitInXZPlane->SetMarkerStyle(1);
	//	FitInXZPlane->SetMarkerSize(1.1);
	//	FitInXZPlane->SetLineColor(4);
	//	FitInXZPlane->SetLineWidth(1.5);





	// Initialize the B-field // new method
	fField= new PndMultiField("FULL", BeamMomentum);  //beam momentum is BeamMomentum in GeV/c (for scaling of dipole field)
	fField->Init();




	//	// That is what I used before Mohammad did some changes
	//
	//	  // Create the Simulation run manager--------------------------------
	//	   FairRunSim *fRun = new FairRunSim();
	//	   //fRun->SetName(SimEngine.Data() );
	//	   //fRun->SetOutputFile(OutputFile.Data());
	//	   fRun->SetBeamMom(BeamMomentum);
	//	   //fRun->SetMaterials(MediaFile.Data());
	//	   //FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
	//
	//	   PndMultiField *fField= new PndMultiField("FULL");
	//	   fField->Init();


	TCanvas *cregularspace = NULL;

	cregularspace = new TCanvas("cregularspace", "cregularspace", ResolutionX, ResolutionY);
	// Plot the B-field
	DrawField(fField, Bfield);
	Bfield->Draw("cont1");






	// get FTS Hits
	TFile* fDigi = new TFile(DigiFile.Data());
	TTree* tDigi = (TTree*) (fDigi->Get("cbmsim"));
	//tDigi->StartViewer();

	TClonesArray* fFtsHitArray = new TClonesArray("PndFtsHit");
	tDigi->SetBranchAddress("FTSHit", &fFtsHitArray);

	tDigi->GetEntry(iEvent); // Look at event iEvent







	// 2014
	//		TCanvas *choughspaceLine = new TCanvas("choughspaceLine", "choughspaceLine",
	//				ResolutionX, ResolutionY);
	//		houghspaceLine->DrawCopy("Lego2z");





	// 2014
	//    // find peak for line hough space
	//	Double_t bpeakLine = 0., weightLine=0.;
	//	Int_t binmaxglobalLine = 0;
	//	// Call peak finder and plot the solution if it was found!
	//	// Bool_t PeakFinder(TString option, TH2F *houghspace, Int_t& binmaxglobal, Double_t& thetapeak, Double_t& pzinvpeak, Double_t& weight)
	//	if (kTRUE == PeakFinder(peakfinderOption, houghspaceLine, binmaxglobalLine, thetaPeakLine, bpeakLine, weightLine))
	//	{
	//		if (1<fVerbose) { std::cout << "Peak for line found: binmaxglobalLine = " << binmaxglobalLine
	//			<< "  thetaPeakLine = " << thetaPeakLine << "  bpeakLine = "
	//			<< bpeakLine << std::endl; }
	//
	////		void MakeHoughLineFit(Double_t b, Double_t theta,
	////				Double_t weight, const Double_t zOffset, TGraph* returnTGraph)
	//
	//		if (kFALSE == noPlots) { cregularspace->cd(0); }
	//		MakeHoughLineFit(bpeakLine, thetaPeakLine, weightLine, zOffset, FitInXZPlane);
	//	}
	//	else
	//	{
	//		std::cout << "Error: Peak finder had a problem with hough space for line!!!" << std::endl;
	//	}








	//2014
	//	if (kFALSE == noPlots)
	//	{
	//		TCanvas *choughspaceParabola = new TCanvas("choughspaceParabola", "choughspaceParabola",
	//				ResolutionX, ResolutionY);
	//		houghspaceParabola->DrawCopy("Lego2z");
	//	}



	// Call peak finder and plot the solution if it was found!
	// Bool_t PeakFinder(TString option, TH2F *houghspace, Int_t& binmaxglobal, Double_t& thetapeak, Double_t& pzinvpeak, Double_t& weight)
	if (kTRUE == PeakFinder(peakfinderOption, houghspaceParabola, binmaxglobal, thetaPeakParabolaFrominvpz, pzinvpeak, weight))
	{
		if (1<fVerbose)
		{
			std::cout << "peak found: binmaxglobal = " << binmaxglobal
					<< "  thetaPeakParabolaFrominvpz = " << thetaPeakParabolaFrominvpz << "  pzinvpeak = "
					<< pzinvpeak << " height = " << weight << std::endl;
		}


		if (kFALSE == noPlots) { cregularspace->cd(0); }
		BMeanForParabola = MakeHoughParabolaFitwithBfield(fField, pzinvpeak, thetaPeakParabolaFrominvpz, weight, zOffset, hitshiftinx, 1., c, keepBConstant, FitInXZPlane);
		//			void MakeHoughParabolaFitwithBfield(PndMultiField *fField, Double_t pzinv, Double_t theta,
		//			Double_t weight, Double_t zOffset, Double_t hitshiftinx, Double_t zscaling, const Double_t c,
		//			const Bool_t keepBConstant, TH2D* Fitinxzplane)
	}
	else
	{
		std::cout << "Error: Peak finder had a problem!!!" << std::endl;
	}



	// ------------------------ FOR TESTING PARABOLA WITH PZ --------------------

	// Do parabola hough transform with pz instead of 1/pz as parameter (shifts FTS hits by hitshiftinx)
	if (kTRUE == MakeHoughSpace("parabolapz", fFtsHitArray, onlyusehitsfromzForParabola, onlyusehitsuptozForParabola, onlyUseHitsFromNonSkewedStraws, thetalimitlowForParabola, thetalimithighForParabola, thetastepForParabola, hitshiftinx, zOffset, c, fField, keepBConstant, nHitsForHoughSpaceParabolapz, houghspaceParabolapz))
	{
		if (1<fVerbose) { std::cout << "Hough Space for parabola with pz instead of 1/pz was created successfully!" << std::endl; }
	}
	else
	{
		std::cout << "Hough Space for parabola with pz instead of 1/pz could not be created! " << std::endl;
	}

	if (kFALSE == noPlots)
	{
		TCanvas *choughspaceParabolapz = new TCanvas("choughspaceParabolapz", "choughspaceParabolapz",
				ResolutionX, ResolutionY);
		houghspaceParabolapz->DrawCopy("Lego2z");
	}



	Double_t weightpz=0.;
	Int_t binmaxglobalpz = 0;
	pzpeak = 0.;

	// Call peak finder and plot the solution if it was found!
	if (kTRUE == PeakFinder(peakfinderOption, houghspaceParabolapz, binmaxglobalpz, thetaPeakParabolaFrompz, pzpeak, weightpz))
	{
		if (1<fVerbose)
		{
			std::cout << "peak found: binmaxglobal = " << binmaxglobalpz
					<< "  thetaPeakParabolaFrompz = " << thetaPeakParabolaFrompz << "  pzinvpeak = "
					<< pzpeak << " height = " << weightpz << std::endl;
		}

		if (kFALSE == noPlots) { cregularspace->cd(0); }
		BMeanForParabolapz = MakeHoughParabolaFitwithBfield(fField, 1./pzpeak, thetaPeakParabolaFrompz, weightpz, zOffset, hitshiftinx, 1., c, keepBConstant, FitInXZPlane);
	}
	else
	{
		std::cout << "Error: Peak finder had a problem!!!" << std::endl;
	}









	//  // This code can be used to test the fit drawing method with B field
	//  for (Double_t thetatest=0.1; thetatest <30.; thetatest+=14.)
	//  {
	//	void MakeHoughParabolaFitwithBfield(PndMultiField *fField, Double_t pzinv, Double_t theta,
	//			Double_t weight, Double_t zOffset, Double_t hitshiftinx, Double_t zscaling, const Double_t c,
	//			const Bool_t keepBConstant, TH2D* Fitinxzplane)
	//  // make some test plots for 1/pz
	//  //MakeHoughParabolaFitwithBfield(fField,-1./300./thetatest,0.01,thetatest*thetatest,zOffset,0.,1.,c,keepBConstant,Fitinxzplane);
	//  // rotate in positive theta direction (counterclockwise)
	//  //MakeHoughParabolaFitwithBfield(fField,-1./1000.,thetatest,thetatest*thetatest,zOffset,0.,1.,c,keepBConstant,Fitinxzplane);
	//  // rotate in negative theta direction (clockwise)
	//  MakeHoughParabolaFitwithBfield(fField,-1./3000.,-thetatest,thetatest*thetatest,zOffset,0.,1.,c,keepBConstant,Fitinxzplane);
	//  }
	//
	//
	//  // This code can be used to test the fit drawing method without B field
	//  for (Double_t thetatest=0.1; thetatest <30.; thetatest+=14.)
	//  {
	//  // make some test plots for 1/pz
	//  //MakeHoughFit(-1./300./thetatest,0.01,thetatest*thetatest,zOffset,0.,1.,c,keepBConstant,Fitinxzplane);
	//  // rotate in positive theta direction (counterclockwise)
	//  //MakeHoughFit(-1./1000.,thetatest,thetatest*thetatest,zOffset,0.,1.,c,keepBConstant,Fitinxzplane);
	//  // rotate in negative theta direction (clockwise)
	//  MakeHoughFit(-1./3000.,-thetatest,thetatest*thetatest,zOffset,0.,1.,c,keepBConstant,Fitinxzplane);
	//  }

	// plot the fit
	//const int ResolutionX = 800, ResolutionY = 600;
	//TCanvas *cHoughFit=new TCanvas("cHoughFit","cHoughFit",ResolutionX,ResolutionY);


	//	Fitinxzplane->DrawCopy("colz,SAME");






























	cregularspace->cd(0);
	// Plot the MC Points
	TFile* fSim = new TFile(SimFile.Data());


	const int subdetectorCount = 11; //
	TString branchNames[subdetectorCount] =
	{ "FTSPoint", "STTPoint", "MVDPoint", "GEMPoint", "DrcBarPoint",
			"DrcPDPoint", "DskParticle", "PndDskFLGHit", "MdtPoint",
			"FtofPoint", "EmcHit" };
	TString tcaNames[subdetectorCount] =
	{ "PndFtsPoint", "PndSttPoint", "PndSdsMCPoint", "PndGemMCPoint",
			"PndDrcBarPoint", "PndDrcPDPoint", "PndDskParticle", "PndDskFLGHit",
			"PndMdtPoint", "PndFtofPoint", "PndEmcHit" };
	std::cout << "Event number " << iEvent << std::endl;


	TGraph *mcPoints = new TGraph();
	mcPoints->SetMarkerColor(2);
	mcPoints->SetMarkerStyle(29);
	mcPoints->SetMarkerSize(1.4);
	mcPoints->SetLineColor(2);
	for (int iSubdetector = 0; iSubdetector < subdetectorCount; ++iSubdetector)
	{
		cout << iSubdetector
				<< ". subdetector: branchNames[iSubdetector], tcaNames[iSubdetector] = "
				<< branchNames[iSubdetector] << " " << tcaNames[iSubdetector]
				                                                << std::endl;
		PlotMCTracks(iEvent, branchNames[iSubdetector], tcaNames[iSubdetector], mcPoints);
	}
	// do not close the file fSim, otherwise, you won't see any plots




	// Plot FTS hits
	TGraph* hits = new TGraph();
	hits->SetMarkerStyle(2);
	hits->SetMarkerColor(1);
	hits->SetMarkerSize(2.0);
	const Int_t nHits = fFtsHitArray->GetEntriesFast();
	Double_t hitX[nHits];
	Double_t hitY[nHits];
	Double_t hitZ[nHits];
	for (int iHit = 0; iHit < nHits; iHit++)
	{
		PndFtsHit* myHit = (PndFtsHit*) fFtsHitArray->At(iHit);
		if (kTRUE == onlyUseHitsFromNonSkewedStraws)
		{
			if (0 != myHit->GetSkewed())
			{
				if (0<fVerbose) {std::cout << "Not drawing hit with index " << iHit << " , because it comes from a skewed straw! LayerID = " << myHit->GetLayerID() << std::endl;}
				continue;
			}

		}
		TVector3 hit;
		myHit->Position(hit);

		hitX[iHit] = hit.X();
		hitY[iHit] = hit.Y();
		hitZ[iHit] = hit.Z();


		if (0<fVerbose)
		{
			std::cout << "Hit " << iHit << " : ";
			//myHit->Print();
			std::cout << "X: " << hitX[iHit] << " Y: " << hitY[iHit] << " Z: " << hitZ[iHit]
			                                                                           << std::endl;
		}
	}
	hits->DrawGraph(nHits, hitZ, hitX, "P,SAME");


	// cout particle's momentum
	TTree *tree=(TTree *) fSim->Get("cbmsim") ;
	TClonesArray* mc_array=new TClonesArray("PndMCTrack");
	tree->SetBranchAddress("MCTrack", &mc_array);
	tree->GetEntry(iEvent);
	for (Int_t mcIndex = 0; mcIndex < mc_array->GetEntriesFast(); ++mcIndex)
	{
		PndMCTrack *mctrack = (PndMCTrack*)mc_array->At(mcIndex);
		// only look at primary particles
		if (mctrack->GetMotherID()!=-1) continue;

		Int_t mc_pid = mctrack->GetPdgCode();
		Double_t mc_px = mctrack->GetMomentum().X();
		Double_t mc_py = mctrack->GetMomentum().Y();
		Double_t mc_pz = mctrack->GetMomentum().Z();
		//			Double_t mc_theta = mctrack->GetMomentum().Theta()*TMath::RadToDeg();
		//			Double_t mc_phi = mctrack->GetMomentum().Phi()*TMath::RadToDeg();
		//
		//			Double_t mc_theta_xz = atan2(mc_px,mc_pz)*TMath::RadToDeg();

		std::cout << "Primary particle with pid " << mc_pid << " has momentum = (" << mc_px << ", " << mc_py << ", " << mc_pz << ") GeV/c (at interaction point)" << std::endl;

		//			std::cout<<setprecision(4);
		TString histoname = "Single #mu^{-} event with MC p_{z} = ";
		// manually round mc_pz to a.bc, for example: 4.71
		TString numMCTruth="", numReco="";
		numMCTruth += mc_pz+0.0005;
		numMCTruth.Resize(5);
		histoname += numMCTruth;
		histoname += " GeV/c <-> FTS PR p_{z} = ";
		numReco +=pzinvpeak+0.0005;
		numReco.Resize(5);
		histoname += numReco;
		histoname += " GeV/c"; //, red = points, black = FTS hits";

		Bfield->SetTitle(histoname);

	} // end of MC loop

}


#endif
