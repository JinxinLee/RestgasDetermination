/*
 * PndSttSkewStrawPzFinder.cxx
 *
 *  Created on: Mar 17, 2016
 *      Author: walan603
 */

#include "PndSttSkewStrawPzFinder.h"

//ROOT includes	REMOVE WHEN DONE DEVELOPING
#include "TCanvas.h"
#include "TH2D.h"

//For sorting skew straws according to layer
struct sort_pred {

    bool operator()(const std::pair<PndSttHit,int> &left,
    		const std::pair<PndSttHit,int> &right) {

        return left.second < right.second;

    }
};

/*
 * Public member functions
 */

PndSttSkewStrawPzFinder::PndSttSkewStrawPzFinder(TClonesArray* sttTubeArray, TClonesArray* sttHitArray) : fVerbose(0),
	fTubeArray(sttTubeArray), fSTTHits(sttHitArray), LineCombiAngleThreshold(90.), fSteps(180.) {

	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder initialized"
				<< endl;
	}
	fStrawMap = new PndSttStrawMap(sttTubeArray);
	fHoughHisto = new TH2D("HoughSpace","HoughSpace", 180, 0, 180, 200, -100, 200);

}

void PndSttSkewStrawPzFinder::AddPndRiemannTracks(vector<PndRiemannTrack> AllRiemannTracks) {
	fVectorPndRiemannTrack = AllRiemannTracks;

	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder::AddPndRiemannTracks() added "
				<<AllRiemannTracks.size()
				<<" Riemann tracks."
				<< endl;
	}

}

void PndSttSkewStrawPzFinder::AddPndTracks(vector<PndTrack> AllTracks) {
	fVectorPndTrack = AllTracks;

	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder::AddPndTracks() added "
				<<AllTracks.size()
				<<" tracks."
				<< endl;
	}
}

void PndSttSkewStrawPzFinder::AddPndTrackCands(vector<PndTrackCand> AllTrackCands) {
	fVectorPndTrackCand = AllTrackCands;

	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder::AddPndTrackCands() added "
				<<AllTrackCands.size()
				<<" tracks."
				<< endl;
	}
}

void PndSttSkewStrawPzFinder::AddPndSttHits(vector<vector<PndSttHit> > AllSkewedHits) {
	fVectorSkewedSttHits = AllSkewedHits;

	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder::AddPndSttHits() added "
				<<AllSkewedHits.size()
				<<" vectors of PndSttHits."
				<< endl;
	}
}

void PndSttSkewStrawPzFinder::ExtractPz() {
	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder::ExtractPz()"
				<< endl;
	}

	//Fill fVectorSkewedSttHits with only SkewedSttHits
	InitSkewed();

	vector< vector<PndSttHit> > output;
	/*
	 * Index i runs over all Track candidates. Following input TCloneArray's
	 * are assumed to have the same index i:
	 *
	 * fVectorPndTrackCand
	 * fVectorPndTrack
	 * fVectorPndRiemannTrack
	 */
	for (int i = 0; i < fVectorPndTrackCand.size(); ++i) {

		Double_t riemannR = fVectorPndRiemannTrack.at(i).r();
		TVector2 riemanncircle(fVectorPndRiemannTrack.at(i).orig()[0],fVectorPndRiemannTrack.at(i).orig()[1]);
		Int_t charge = fVectorPndTrack.at(i).GetParamFirst().GetQ();	//q=1 => clockwise, q=-q 0> anti-clockwise


		if (fVerbose > 0) cout<<"ExtractSkewedHits - Correcting hits for track: "<<i<<endl;

		output = MoveSkewedHitsToCircle(riemanncircle,riemannR,fVectorSkewedSttHits.at(i));//CHANGE FUNCTION HERE

		if (output.empty()) {
			if (fVerbose > 0 ) cout<<"ExtractSkewedHits - No hits in track: "<<i<<" were adjusted!"<<endl;
			continue;
		}

		Double_t Phi0;
		if (output.at(0).size() > 1) {
			TVector2 FirstHit1(output.at(0).at(0).GetX() - riemanncircle.X(),output.at(0).at(0).GetY() - riemanncircle.Y());
			TVector2 FirstHit2(output.at(0).at(1).GetX() - riemanncircle.X(),output.at(0).at(1).GetY() - riemanncircle.Y());

			Double_t FirstHitPhi1 = FirstHit1.Phi();
			Double_t FirstHitPhi2 = FirstHit2.Phi();


			if (charge < 0 && (FirstHitPhi1 < FirstHitPhi2)) {
				Phi0 = FirstHitPhi1;
			}
			else if (charge < 0 && (FirstHitPhi1 > FirstHitPhi2)) {
				Phi0 = FirstHitPhi2;
			}
			else if (charge > 0 && (FirstHitPhi1 > FirstHitPhi2)) {
				Phi0 = FirstHitPhi1;
			}
			else if (charge > 0 && (FirstHitPhi1 < FirstHitPhi2)) {
				Phi0 = FirstHitPhi2;
			}
		}
		else {
			TVector2 FirstHit(output.at(0).at(0).GetX() - riemanncircle.X(),output.at(0).at(0).GetY() - riemanncircle.Y());
			Phi0 = FirstHit.Phi();
		}

		Double_t S0 = Phi0*riemannR; //Currently unused todo:Fix the phi angle such that the z-phi points spans a continuous line and not a triangle function

		vector< vector<TVector2> > ZPhiPairVector;
		for (int j = 0; j < output.size(); ++j)//Loop over all stt hits
		{
			vector<TVector2> ZPhiPair;
			if (output.at(j).empty()) {
				cout<<"ExtractSkewedHits - Hit: "<<j<<" in track: "<<i<<  "was not adjusted adjusted!"<<endl;
				continue;
			}

			//cout<<"ExtractSkewedHits - New list of adjusted hits"<<endl;
			for (int k = 0; k < output.at(j).size(); ++k)//Loop over all possible adjusted hits for one hit
			{
				Double_t x0 = output.at(j).at(k).GetX() - riemanncircle.X();
				Double_t y0 = output.at(j).at(k).GetY() - riemanncircle.Y();
				TVector2 pos(x0,y0);

				Double_t phi = pos.Phi();
				//cout<<"pos.phi(): "<<pos.Phi()<<endl;
				Double_t S = phi*riemannR;

				TVector2 ZPhi(output.at(j).at(k).GetZ(),S);
				if (fVerbose > 1 ) cout<<"ExtractSkewedHits - SkewHit Z: "<<output.at(j).at(k).GetZ()<<" Phi: "<<phi<<endl;

				ZPhiPair.push_back(ZPhi);

			}

			ZPhiPairVector.push_back(ZPhiPair);

		}

		/*
		 * From here, choose a Z-finding method; Line combi, Hough or linear regresion
		 */

		if (ZPhiPairVector.size() < 3) {
			if (fVerbose > 0) cout<<"<=2 skewed straw hits, Cannot continue!"<<endl;
			continue;
		}

		/*
		 * Line combi code
		 */
/*
		if (ZPhiPairVector.size() > 9) {
			if (fVerbose > 0) cout<<"Too many skewed straws, exploding combi"<<endl;
			continue;
		}

		//Find the best set of z-phi coordinates
		vector<TVector2> TrueZPhi = LineCombiIsoFinder(ZPhiPairVector);
*/

		/*
		 * End line combi code
		 */

		//TVector2 TheilSenLinePar = TheilSen(ZPhiPairVector);
		//TVector2 Linreg = PzLineFitExtract2(ZPhiPairVector);
		//cout<<"All points lin reg slope: "<<Linreg.Y()<<endl;

		/*
		 * Hough transform code
		 */

		Double_t Zmin = 99999., Zmax = -1., Phimin = 99999., Phimax = -1.;

		for (int k = 0; k < ZPhiPairVector.size(); k++){
			for (int j = 0; j < ZPhiPairVector.at(k).size(); j++){
				if (ZPhiPairVector.at(k).at(j).X() < Zmin) { Zmin = ZPhiPairVector.at(k).at(j).X(); };
				if (ZPhiPairVector.at(k).at(j).X() > Zmax) { Zmax = ZPhiPairVector.at(k).at(j).X(); };

				if (ZPhiPairVector.at(k).at(j).Y() < Phimin) { Phimin = ZPhiPairVector.at(k).at(j).Y(); };
				if (ZPhiPairVector.at(k).at(j).Y() > Phimax) { Phimax = ZPhiPairVector.at(k).at(j).Y(); };
			}
		}
		Double_t maxDist;
		if (TMath::Abs(Zmax - Zmin)>TMath::Abs(Phimax-Phimin)) {
			maxDist = (sqrt(2)*TMath::Abs(Zmax - Zmin));
		}
		else {
			maxDist = (sqrt(2)*TMath::Abs(Phimax - Phimin));
		}

		TH2D *HoughSpace = new TH2D("HoughSpace","HoughSpace", 180, 0, 180, 100, -maxDist, maxDist);
		TVector2 resultparams;

		vector< vector<TVector2> > translate_ZPhi = TranslateZPhi(ZPhiPairVector);
		ZPhiPairVector = translate_ZPhi;

		vector<TVector2> TrueZPhi = HoughTrueIsoFinder(ZPhiPairVector, HoughSpace, resultparams);

		//REMOVE WHEN DONE DEVELOPING
		TCanvas *HoughCanvas = new TCanvas();
		HoughSpace->Draw("COLZ");
		HoughCanvas->Update();
		//HoughSpace->Delete();


/*
		TVector2 resultparams;

		vector<TVector2> TrueZPhi = HoughTrueIsoFinder(ZPhiPairVector, fHoughHisto, resultparams);
		fHoughHisto->Clear();

		if (fHoughHisto != 0) {

			TCanvas * tempCanvas = new TCanvas("canvasHough","canvasHough Track: "+TString::Itoa(i,10),0, 0, 1000, 800);

			fHoughHisto->Draw("COLZ");

			tempCanvas->Update();
		}
*/
		/*
		 * End Hough transform code
		 */

		if (TrueZPhi.empty()) {
			cout<<"No true Zphi was selected"<<endl;
			continue;
		}

		//Perform a straight line fit with the selected z-phi coordinates
		TVector2 lineparams = PzLineFitExtract(TrueZPhi);

		//Double_t charge = fVectorPndTrack.at(i).GetParamFirst().GetQ();

		//Calculate momentum z-component
		Double_t pz = TMath::Sqrt(fVectorPndTrack.at(i).GetParamFirst().GetPx()*fVectorPndTrack.at(i).GetParamFirst().GetPx()+
				fVectorPndTrack.at(i).GetParamFirst().GetPy()*fVectorPndTrack.at(i).GetParamFirst().GetPy());
		if (charge > 0) {
			pz = (-1.)*pz/(lineparams.Y());
		}
		else (pz = pz/(lineparams.Y()));

		//Generate new set of track parameters with the z-information
		FairTrackParP trackParamFirst = GetTrackParam(fVectorPndTrack.at(i).GetParamFirst(),
				fVectorPndRiemannTrack.at(i), lineparams, pz);

		FairTrackParP trackParamLast = GetTrackParam(fVectorPndTrack.at(i).GetParamLast(),
				fVectorPndRiemannTrack.at(i), lineparams, pz);

		//Generate new PndTrack
		PndTrackCand newCand =  fVectorPndTrackCand.at(i);
		PndTrack newTrack(trackParamFirst,trackParamLast,newCand);

		//Store track info
		fResultPndRiemannTrack.push_back(fVectorPndRiemannTrack.at(i));
		fResultPndTrackCand.push_back(newCand);
		fResultPndTrack.push_back(newTrack);

		//Store additional PzFinder information
		PndSttSkewStrawPzFinderData resultData;
		resultData.setLineSlope(lineparams.Y());
		resultData.setLineIntercept(lineparams.X());
		resultData.setPhiPairVector(ZPhiPairVector);
		resultData.setTrueZPhi(TrueZPhi);
		fResultPzData.push_back(resultData);

	}

}

/*
 * Private member functions
 */

//todo:Implement me!
bool PndSttSkewStrawPzFinder::Clockwise(vector<vector<TVector2> > ZPhiPairVector, TVector2 center){
	return false;
}

void PndSttSkewStrawPzFinder::InitSkewed() {
	if (fVerbose > 0) {
		cout << "PndSttSkewStrawPzFinder::InitSkewed()"
				<< endl;
	}

	vector<PndTrackCandHit> tracklist;
	PndSttTube myTube;
	PndSttHit hit;

	for (int i = 0; i < fVectorPndTrackCand.size(); ++i) {

		tracklist = fVectorPndTrackCand.at(i).GetSortedHits();
		vector<pair<PndSttHit,int> > hitPairs;
		for (int j = 0; j < tracklist.size(); ++j)
		{
			hit = *(PndSttHit*) fSTTHits->At(tracklist.at(j).GetHitId());
			myTube = *(PndSttTube*) fTubeArray->At(hit.GetTubeID());
			if (myTube.IsSkew()) {
				hitPairs.push_back(pair<PndSttHit,int> (hit,hit.GetTubeID()));
			}

			//Sorting the skewed hits based on their layer ID
			std::sort(hitPairs.begin(), hitPairs.end(), sort_pred());
		}

		vector<PndSttHit> skewHits;
		for (int j = 0; j < hitPairs.size(); j++){
			skewHits.push_back(hitPairs.at(j).first);

		}

		fVectorSkewedSttHits.push_back(skewHits);
	}

}

vector<vector<PndSttHit> > PndSttSkewStrawPzFinder::MoveSkewedHitsToCircle(TVector2 circle, Double_t circlerad,
		vector<PndSttHit> skewhits){

	vector<vector <PndSttHit> > result;
	for (Int_t i = 0; i < skewhits.size(); i++){
		if (fVerbose > 1 ) cout<<"MoveSkewedHitsToCircle() Hit: "<<i<<" is being adjusted"<<endl;
		vector<PndSttHit> tempResult;
		PndSttHit myHit = skewhits.at(i);
		PndSttTube myTube = *(PndSttTube*) fTubeArray->At(myHit.GetTubeID());

		Double_t tubeLengthHalf = myTube.GetHalfLength();
		TVector3 wireDirection = myTube.GetWireDirection();
		TVector3 tubePosition = myTube.GetPosition();

		TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
		TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

		TVector2 intersection1, intersection2;
		Int_t nofint = ComputeSegmentCircleIntersection(TVector2(endPoint.X(), endPoint.Y()),
				TVector2(startPoint.X(), startPoint.Y()), circle.X(), circle.Y(), circlerad, intersection1, intersection2);

		if (nofint == 0) continue;

		PndSttHit fixhit = myHit;

		if (fVerbose > 1 ) cout<<"MoveSkewedHitsToCircle() Intersection1 X: "<<intersection1.X()<<" Y: "<<intersection1.Y()<<endl;

		if (nofint == 2) {
			if (fVerbose > 1 ) cout<<"MoveSkewedHitsToCircle() Intersection2 X: "<<intersection1.X()<<" Y: "<<intersection1.Y()<<endl;
		}

		TVector2 tangent = ComputeTangentInPoint(circle.X(), circle.Y(), intersection1);

		// Rotate the tangent/point/(wire, not explicitly) clockwise
		// in order to have the wire parallel to the x axis.
		// Translate coordinates such that the wire is ON the x-axis
		// From PndTrkTools.cxx, author: Lia Lavezzi
		double beta = wireDirection.Phi();
		if(beta < 0) beta += TMath::Pi();

		// ... rotate the tangent
		double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
		double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();

		TVector2 rottangent(rtx, rty);
		rottangent = rottangent.Unit();

		// ... rotate the point
		double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
		double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();

		// translation
		Double_t deltay = ry;
		rty -= deltay;
		ry -= deltay;

		// rotm, rotp
		Double_t rotm = rottangent.Y()/rottangent.X();
		Double_t rotp = ry - rotm * rx;

		// ellipsis
		double a = myHit.GetIsochrone() * TMath::Cos(3*TMath::DegToRad()); // CHECK skew angle hard coded
		double b = myHit.GetIsochrone();

		// center of ellipsis
		Double_t x0a, x0b, y0;
		y0 = 0.;
		x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
		x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

		// intersection point
		double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
		double intya = rotm * intxa + rotp;
		double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
		double intyb = rotm * intxb + rotp;

		// 4. retraslate/rerotate all back to the original plane
		// retranslate
		y0 += deltay;
		intya  += deltay;
		intyb  += deltay;

		// rerotate
		double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
		double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
		double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
		double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;

		double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
		double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
		double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
		double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;

		intxa = intxanew;
		intya = intyanew;
		intxb = intxbnew;
		intyb = intybnew;

		//x0a, y0a - center of the first ellipse
		//x0b, y0b - center of the second ellipse
		x0a = x0anew;
		double y0a = y0anew;
		x0b = x0bnew;
		double y0b = y0bnew;

	    double dx = intxa - x0a;
	    double dy = intya - y0a;
	    TVector3 dxdy(dx, dy, 0.0);

	    TVector3 tfirst = endPoint + dxdy;
	    TVector3 tsecond = startPoint + dxdy;

	    double t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
	    double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

	    tfirst = endPoint - dxdy;
	    tsecond = startPoint - dxdy;

	    t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) /  ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
	    double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

	    //Set new coordinates for hits and plot isochrones
		fixhit.SetX(x0a);
		fixhit.SetY(y0a);
		fixhit.SetX(intxa);
		fixhit.SetY(intya);
		fixhit.SetZ(intza);
		tempResult.push_back(fixhit);

		fixhit.SetX(x0b);
		fixhit.SetY(y0b);
		fixhit.SetX(intxb);
		fixhit.SetY(intyb);
		fixhit.SetZ(intzb);
		tempResult.push_back(fixhit);

		result.push_back(tempResult);
	}
	return result;
}

//Calculate points where a segment of a line (xy-projection of skewed wire, between points ex1 and ex2)
//intersects a circle (xy-projection of track helix) centered at (xc,yc) with radius R.
//Author: Lia Lavezzi
Int_t PndSttSkewStrawPzFinder::ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2,
		double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2) {

	double xa, ya, xb, yb;
	double delta;

	if((ex2.X() - ex1.X()) != 0) {
		double m = (ex2.Y() - ex1.Y())/(ex2.X() - ex1.X());
		double p = ex2.Y() - m * ex2.X();;

		delta = - (m * xc + p - yc) * (m * xc + p - yc) +  R * R * (m * m + 1);

		if(delta < 0 || TMath::IsNaN(delta))  return 0;
		xa = (-(m * (p - yc) - xc) + sqrt(delta))/(m * m + 1);
		ya = m * xa + p;
		xb = (-(m * (p - yc) - xc) - sqrt(delta))/(m * m + 1);
		yb = m * xb + p;
	}
	else {

		delta = R * R - (ex1.X() - xc) * (ex1.X() - xc);

		if(delta < 0 || TMath::IsNaN(delta)) return 0;

		xa = ex1.X();
		ya = yc + sqrt(delta);
		xb = ex1.X();
		yb = yc - sqrt(delta);
	}

	TVector2 int1(xa, ya);
	TVector2 int2(xb, yb);

	TVector2 distance11 = int1 - ex1;
	TVector2 distance12 = int1 - ex2;

	TVector2 distance21 = int2 - ex1;
	TVector2 distance22 = int2 - ex2;

	TVector2 length = ex1 - ex2;
	Int_t result = 0;

	if(distance11.Mod() < length.Mod() && distance12.Mod() < length.Mod()) {
		// cout << "intersection 1" << endl;
		result++;
		intersection1 = int1;
	}

	if(distance22.Mod() < length.Mod() && distance21.Mod() < length.Mod()) {
		// cout << "intersection 2" << endl;
		if(result == 1) intersection2 = int2;
		else intersection1 = int2;
		result++;
	}
	// if delta = 0 we have two identical intersections
	if(delta == 0) result--; // CHECK
	return result;
}

TVector2 PndSttSkewStrawPzFinder::ComputeTangentInPoint(double xc, double yc, TVector2 point) {
	TVector2 center(xc, yc);
	TVector2 fromcentertoint = point - center;
	TVector2 tangent(fromcentertoint.Y(), -fromcentertoint.X());
	return tangent;
}

Double_t PndSttSkewStrawPzFinder::CalculateResidual(Double_t lineSlope, Double_t lineIntercept, TVector2 ZPhiPoint){
	Double_t PhiAtPoint = lineSlope*ZPhiPoint.X() + lineIntercept;
	Double_t result = ZPhiPoint.Y() - PhiAtPoint;
	return result;
}

/*
 * Functions for cellular automaton method with lines
 */

vector<TVector2> PndSttSkewStrawPzFinder::LineCombiIsoFinder(vector<vector< TVector2> > ZPhiPairVector){
	vector<vector<pair<vector<int>,double> > > matrix2;
	vector<pair<vector<int>,double> > tmp2;
	vector<int> tmpIndices;
	TVector2 first, second;
	TVector2 v1, v2;
	double CosAngle, Angle;
	vector<int> nPairArray;

	for (int i = 1; i < ZPhiPairVector.size()-1; i++){//Loop over straws

		if (fVerbose > 1 ) cout<<"LineCombiIsoFinder - New segment centered around straw: "<<i<<endl;

		for (int j = 0; j < ZPhiPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1

			for (int k = 0; k < ZPhiPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i

				for (int l = 0; l < ZPhiPairVector.at(i+1).size(); l++){//Loop over all Z-phi in straw i+1
					v1 = ZPhiPairVector.at(i+1).at(l) - ZPhiPairVector.at(i).at(k);
					v2 = ZPhiPairVector.at(i-1).at(j) - ZPhiPairVector.at(i).at(k);
					CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
					Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();

					//if(Angle > 180 - LineCombiAngleThreshold){//exclude paths
						tmpIndices.push_back(j);
						tmpIndices.push_back(k);
						tmpIndices.push_back(l);
						tmp2.push_back(pair<vector<int>,double> (tmpIndices,Angle));
						tmpIndices.clear();
						if (fVerbose > 4 ) cout<<"LineCombiIsoFinder - Indices j: "
								<<j<<" k: "<<k<<" l: "<<l<<" Angle: "<<Angle<<endl;
					//}
				}
			}
		}

		nPairArray.push_back(tmp2.size());
		matrix2.push_back(tmp2);
		tmp2.clear();

	}

	vector<int> current;
	current.assign(matrix2.size(),0);
	pair<vector<int>,double> bestIndices;
	bestIndices.first.assign(matrix2.size(),0);
	bestIndices.second=0.;

	NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);

	if (bestIndices.second == 0. && ZPhiPairVector.size() < 10) {

		Double_t temp=LineCombiAngleThreshold;
		LineCombiAngleThreshold = 180.;
		NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);
		LineCombiAngleThreshold=temp;
	}

	if (fVerbose > 1 ) cout<<"Final answer: "<<endl;

	if (fVerbose > 1 ) {

		for (int i = 0; i < bestIndices.first.size();i++){
			cout<<bestIndices.first.at(i)<<" ";
		}
		cout<<endl;
	}


	if (fVerbose > 0 ) cout<<"sum: "<<bestIndices.second<<endl;
	int a, b, c;

	vector<TVector2> result;

	if (bestIndices.second > 0.) {
		for (int i = 0; i<matrix2.size();i++) {

			a = matrix2.at(i).at(bestIndices.first.at(i)).first.at(0);
			result.push_back(ZPhiPairVector.at(i).at(a));

			if (i == matrix2.size()-1){
				b = matrix2.at(i).at(bestIndices.first.at(i)).first.at(1);
				c = matrix2.at(i).at(bestIndices.first.at(i)).first.at(2);
				result.push_back(ZPhiPairVector.at(i+1).at(b));
				result.push_back(ZPhiPairVector.at(i+2).at(c));
			}
		}
	}
/*
	//Transform Phi*r to arclength starting at first s=0
	Double_t startPhi = result.at(0).Y();
	Double_t oldval;

	for (int i = 0; i<result.size();i++){
		oldval = result.at(i).Y();
		result.at(i).Set(result.at(i).X(),TMath::Abs(result.at(i).Y()-startPhi));
		if (fVerbose > 2 ) cout<<"LineCombiIsoFinder - R*phi: "<<oldval<<" Arc length: "<<result.at(i).Y()<<endl;
	}
*/
	return result;

}

//Recursive for loop. Loops over angles between all pairs of lines. Calculate the sum of the angles for all possible
//paths from the first to the last skewed straw.
void PndSttSkewStrawPzFinder::NestedFor(vector<int> &times, vector<int> &current, pair<vector<int>,double> &best,
		double sum, vector<vector<pair<vector<int>,double> > > matrix, int depth) {
    double tmp;
    bool possible = true;
	if (depth == times.size()) {

    	if (sum > best.second) {
    		for (int i = 0; i<matrix.size();i++) {
    			int a, b, c;
    			a = matrix.at(i).at(current.at(i)).first.at(0);
    			b = matrix.at(i).at(current.at(i)).first.at(1);
    			c = matrix.at(i).at(current.at(i)).first.at(2);
    		}
        	best.first = current;
        	best.second = sum;
        	if (fVerbose > 1 ) cout<<"NestedFor - Weight: "<<sum<<endl;
    	}
    	return;
    }

    for (int i = 0; i < times[depth]; ++i) {
    	if (depth>0) {
    		//Check if lines are connected
    		if (matrix.at(depth).at(i).first.at(1) != matrix.at(depth-1).at(current.at(depth-1)).first.at(2)) continue;
    		if (matrix.at(depth).at(i).first.at(0) != matrix.at(depth-1).at(current.at(depth-1)).first.at(1)) continue;
    		//Check if angle is too small
    		if (matrix.at(depth).at(i).second < 180. - LineCombiAngleThreshold) continue;
    	}
    	tmp = sum;
    	current[depth]=i;
    	sum+= matrix.at(depth).at(i).second;

    	NestedFor(times, current, best, sum, matrix, depth + 1);

        sum = tmp;
    }
}

/*
 * Functions for linear Hough transform
 */

vector<TVector2> PndSttSkewStrawPzFinder::HoughTrueIsoFinder(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams){
	//Generate lines for each Z-phi point, fill Houghspace with generated line parameters
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			pair<Double_t, Double_t> AngleRange(0.,180.);
			Double_t theta = AngleRange.first;
			Double_t StepSize = ( AngleRange.second-AngleRange.first)/fSteps;
			Double_t Z = ZPhiPairVector.at(i).at(j).X();
			Double_t Phi = ZPhiPairVector.at(i).at(j).Y();
			for (int k = 0; k < fSteps; k++) {
				Double_t dist = Z*TMath::Cos(theta*TMath::DegToRad()) + Phi*TMath::Sin(theta*TMath::DegToRad());
				HoughSpace->Fill(theta,dist);
				theta += StepSize;
			}
		}
	}
	//Find maximum in Hough space
	Int_t MaxDist, MaxTheta, MaxBin, BinID;
	HoughSpace->GetMaximumBin(MaxDist,MaxTheta,MaxBin);
	Double_t MaxdDist = HoughSpace->GetXaxis()->GetBinCenter(MaxDist);
	Double_t MaxdTheta = HoughSpace->GetYaxis()->GetBinCenter(MaxTheta);
	Double_t MaxdBin = HoughSpace->GetBinContent(MaxDist,MaxTheta);

	BinID = HoughSpace->GetBin(MaxDist,MaxTheta,MaxBin);
	cout<<"bin max: "<<HoughSpace->GetMaximumBin(MaxTheta,MaxDist,MaxBin)<<" "<<MaxdBin<<endl;

	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder - HoughSpaceMaxima x: "<<MaxdDist<<" y: "<<MaxdTheta<<endl;
	Double_t slope = 1/TMath::Tan(MaxdTheta*TMath::DegToRad());
	//Calculate true line parameters
	Double_t m = MaxdDist/TMath::Sin(MaxdTheta*TMath::DegToRad());
	lineparams.Set(slope,m);
	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder - Z-Phi line Slope: "<<slope<<" Intercept: "<<m<<endl;
	//Loop over Z-phi pairs and reject the one with larges POCA for each skewed straw
	vector<TVector2> result;
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		Double_t poca = 999999.;
		Int_t index = 0;
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			Double_t temppoca = TMath::Abs(slope*ZPhiPairVector.at(i).at(j).X() - 1*ZPhiPairVector.at(i).at(j).Y() + m)/TMath::Sqrt(pow(slope,2) + pow(-1.,2));

			if (temppoca < poca) {
				poca = temppoca;
				index = j;
			}
		}
		result.push_back(ZPhiPairVector.at(i).at(index));
	}
	return result;
}

/*
 * Second function for linear Hough transform
 * only generates alla lines between all pairs of z-phi points
 */

vector<TVector2> PndSttSkewStrawPzFinder::HoughTrueIsoFinder2(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams){
	//Generate only lines between all z-phi candidates, fill Hough space
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			double z1 = ZPhiPairVector.at(i).at(j).X();
			double phi1 = ZPhiPairVector.at(i).at(j).Y();
			for (int k = i+1; k < ZPhiPairVector.size(); k++){
				for (int l = 0; l < ZPhiPairVector.at(k).size(); l++){
					double z2 = ZPhiPairVector.at(k).at(l).X();
					double phi2 = ZPhiPairVector.at(k).at(l).Y();
					double theta = TMath::ATan(-(phi2-phi1)/(z2-z1));
					double dist = z1*TMath::Cos(theta) + phi1*TMath::Sin(theta);
					if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - theta: "<<theta*TMath::RadToDeg()<<" dist: "<<dist<<endl;
					HoughSpace->Fill(theta*TMath::RadToDeg(),dist);

				}
			}
		}
	}
	//Find maximum in Hough space
	Int_t MaxDist, MaxTheta, MaxBin, BinID;
	HoughSpace->GetMaximumBin(MaxTheta,MaxDist,MaxBin);

	Double_t MaxdTheta = HoughSpace->GetXaxis()->GetBinCenter(MaxTheta);
	Double_t MaxdDist = HoughSpace->GetYaxis()->GetBinCenter(MaxDist);
	Double_t MaxdBin = HoughSpace->GetBinContent(MaxTheta,MaxDist);

	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - HoughSpaceMaxima x: "<<MaxdTheta<<" y: "<<MaxdDist<<endl;
	//Calculate true line parameters
	Double_t slope = -1/TMath::Tan(MaxdTheta*TMath::DegToRad());
	Double_t m = MaxdDist/TMath::Sin(MaxdTheta*TMath::DegToRad());
	lineparams.Set(slope,m);
	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - Z-Phi line Slope: "<<slope<<" Intercept: "<<m<<endl;
	//Loop over Z-phi pairs and reject the one with larges POCA for each skewed straw
	vector<TVector2> result;
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		Double_t poca = 999999.;
		Int_t index = 0;
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			Double_t temppoca = TMath::Abs(slope*ZPhiPairVector.at(i).at(j).X() - 1*ZPhiPairVector.at(i).at(j).Y() + m)/TMath::Sqrt(pow(slope,2) + pow(-1.,2));

			if (temppoca < poca) {
				poca = temppoca;
				index = j;
			}
		}
		result.push_back(ZPhiPairVector.at(i).at(index));
	}

	return result;
}

vector<vector< TVector2> > PndSttSkewStrawPzFinder::TranslateZPhi(vector<vector< TVector2> > ZPhiPairVector){
	Double_t Zmin = 99999., Zmax = -1., Phimin = 99999., Phimax = -1.;

	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			if (ZPhiPairVector.at(i).at(j).X() < Zmin) { Zmin = ZPhiPairVector.at(i).at(j).X(); };
			if (ZPhiPairVector.at(i).at(j).X() > Zmax) { Zmax = ZPhiPairVector.at(i).at(j).X(); };

			if (ZPhiPairVector.at(i).at(j).Y() < Phimin) { Phimin = ZPhiPairVector.at(i).at(j).Y(); };
			if (ZPhiPairVector.at(i).at(j).Y() > Phimax) { Phimax = ZPhiPairVector.at(i).at(j).Y(); };
		}
	}
	cout<<"zmin zmax phimin phimax: "<<Zmin<<" "<<Zmax<<" "<<" "<<Phimin<<" "<<Phimax<<endl;

	vector<vector< TVector2> > result = ZPhiPairVector;

	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			result.at(i).at(j).Set(ZPhiPairVector.at(i).at(j).X()-Zmin,ZPhiPairVector.at(i).at(j).Y()-Phimin);
		}
	}

	return result;
}

/*
 * FUnctions for linear regression
 */

TVector2 PndSttSkewStrawPzFinder::TheilSen(vector<vector< TVector2> > ZPhiPairVector){
	TVector2 result;

	vector<Double_t> SlopeVector;

	for (int i = 0; i < ZPhiPairVector.size()-1; i++){//Loop over all straws minus last one

		for (int j = i+1; j < ZPhiPairVector.size(); j++){//Loop over straws from i+1 to and including last one

			for (int k = 0; k < ZPhiPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i

				for (int l = 0; l < ZPhiPairVector.at(j).size(); l++){//Loop over all Z-phi in straw j


					Double_t slope = (ZPhiPairVector.at(j).at(l).Y() - ZPhiPairVector.at(i).at(k).Y())/(ZPhiPairVector.at(j).at(l).X() - ZPhiPairVector.at(i).at(k).X());
					SlopeVector.push_back(slope);

				}

			}

		}

	}

	sort(SlopeVector.begin(),SlopeVector.end());

	cout<<"Slopes ordered: "<<endl;
	for (int i = 0; i < SlopeVector.size(); i++) {
		cout<<"Slope: "<<SlopeVector.at(i)<<endl;
	}

	Double_t MedianSlope;
	if (SlopeVector.size() % 2 == 0) {
		auto middle = SlopeVector.begin();
		advance(middle, SlopeVector.size()/2);//TODO: Calculate median as mean of two middle elements
		MedianSlope = *middle;
	}
	else {
		auto middle = SlopeVector.begin();
		advance(middle, SlopeVector.size()/2);
		MedianSlope = *middle;
	}
	cout<<"Median slope: "<<MedianSlope<<endl;

	return result;
}

TVector2 PndSttSkewStrawPzFinder::PzLineFitExtract(vector< TVector2> ZPhi){
	//Simple linear regression, Z on X-axis Phi on Y-axis
	//Errors not taken into account
	Double_t meanZ = 0.;
	Double_t meanPhi = 0.;
	for (int i = 0; i < ZPhi.size(); i++){
		if (fVerbose > 0 ) cout<<"PzLineFitExtract - True hit Z: "<<ZPhi.at(i).X()<<" Phi*R: "<<ZPhi.at(i).Y()<<endl;
		meanZ += ZPhi.at(i).X();
		meanPhi += ZPhi.at(i).Y();
	}
	meanZ /=(ZPhi.size());
	meanPhi /=(ZPhi.size());
	Double_t betanume = 0.;
	Double_t betadeno = 0.;
	for (int i = 0; i < ZPhi.size(); i++){
		betanume += (ZPhi.at(i).X() - meanZ)*(ZPhi.at(i).Y() - meanPhi);
		betadeno += pow(ZPhi.at(i).X() - meanZ,2);
	}
	Double_t beta = betanume/betadeno;
	Double_t alpha = meanPhi - beta*meanZ;
	if (fVerbose > 0 ) cout<<"PzLineFitExtract - Slope: "<<beta<<" Intercept: "<<alpha<<endl;
	TVector2 result(alpha,beta);
	return result;
}

TVector2 PndSttSkewStrawPzFinder::PzLineFitExtract2(vector<vector< TVector2> > ZPhiPairVector){
	//Simple linear regression, Z on X-axis Phi on Y-axis
	//Errors not taken into account
	Double_t meanZ = 0.;
	Double_t meanPhi = 0.;
	Int_t N = 0;
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			meanZ += ZPhiPairVector.at(i).at(j).X();
			meanPhi += ZPhiPairVector.at(i).at(j).Y();
			N++;
		}
	}
	meanZ /= N;
	meanPhi /= N;
	Double_t betanume = 0.;
	Double_t betadeno = 0.;
	for (int i = 0; i < ZPhiPairVector.size(); i++){
		for (int j = 0; j < ZPhiPairVector.at(i).size(); j++){
			betanume += (ZPhiPairVector.at(i).at(j).X() - meanZ)*(ZPhiPairVector.at(i).at(j).Y() - meanPhi);
			betadeno += pow(ZPhiPairVector.at(i).at(j).X() - meanZ,2);
		}
	}
	Double_t beta = betanume/betadeno;
	Double_t alpha = meanPhi - beta*meanZ;
	if (fVerbose > 0 ) cout<<"PzLineFitExtract - Slope: "<<beta<<" Intercept: "<<alpha<<endl;
	TVector2 result(alpha,beta);
	return result;
}

FairTrackParP PndSttSkewStrawPzFinder::GetTrackParam(FairTrackParP oldParam, PndRiemannTrack circle, TVector2 lineparams, Double_t pz) {
	Double_t xnew = circle.orig()[0];
	Double_t ynew = circle.orig()[1];
	Double_t rnew = circle.r();

	TVector2 Pos(oldParam.GetX() - xnew, oldParam.GetY() - ynew);
	Double_t Phi = Pos.Phi()*rnew;
	TVector3 Hit(oldParam.GetX(),oldParam.GetY(),(Phi - lineparams.X())/lineparams.Y());

	TVector3 HitError(oldParam.GetDX(),oldParam.GetDY(),2);

	if (fVerbose > 0) cout<<"Hit Phi*R: "<<Phi<<endl;
	if (fVerbose > 0) cout<<"New position: "<<Hit.X()<<" "<<Hit.Y()<<" "<<Hit.Z()<<endl;

    TVector3 originFirst(Hit);

	TVector3 Mom(oldParam.GetPx(),oldParam.GetPy(),pz);
	if (fVerbose > 0) cout<<"New momentum vector: "<<Mom.X()<<" "<<Mom.Y()<<" "<<Mom.Z()<<endl;

    TVector3 momError(0.02, 0.02, 0.02);
    TVector3 dj(1,0,0);
    TVector3 dk(0,1,0);

	FairTrackParP result(Hit,Mom,HitError,momError,oldParam.GetQ(),Hit,dj,dk);

	return result;
}





