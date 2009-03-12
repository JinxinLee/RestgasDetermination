/*
 * anaLumiReco.C
 *
 *  Created on: Nov 22, 2008
 *      Author: tsito
 */
{
	//Load basic libraries

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Double_t pi=TMath::Pi();

	// Set a pointer to the file (tree) contains the output
	TFile *f_reco = new TFile("lumireco.root");
	TTree *tr_reco = (TTree*) f_reco->Get("cbmsim");

	TFile *f_mc = new TFile("lumisim.root");
	TTree *tr_mc = f_mc->Get("cbmsim") ;

	// Collect data in an array
	TClonesArray* reco_array = new TClonesArray("PndLumiRecoHit");
	tr_reco->SetBranchAddress("LumiRecoHit",&reco_array);

	TClonesArray* point = new TClonesArray("PndLumiPoint");
	tr_mc->SetBranchAddress("LumiPoint",&point);

	TClonesArray* track = new TClonesArray("PndMCTrack");
	tr_mc->SetBranchAddress("MCTrack",&track);

	//Histogram definition

	TH1F *hX_mc = new TH1F("","",100, -10., 10.);
	TH1F *hY_mc = new TH1F("","",100, -10., 10.);
	TH1F *hZ_mc = new TH1F("","",100, 1000., 1200.);
	TH1F *hX_reco = new TH1F("","",100, -10., 10.);
	TH1F *hY_reco = new TH1F("","",100, -10., 10.);
	TH1F *hZ_reco = new TH1F("","",100, 1000., 1200.);

	TH1F *hdX[4];
	TH1F *hdY[4];
	TH1F *hdR[4];
	for (Int_t m = 0; m < 4; m++){

		TH1F *hdX[m] = new TH1F("","",120, -0.002, 0.002);
		TH1F *hdY[m] = new TH1F("","",120, -0.002, 0.002);
		TH1F *hdR[m]= new TH1F ("","", 160, -2.*1E-3, 2.1E-3);

		hdX[m]->SetXTitle("dX[cm]");
		hdY[m]->SetXTitle("dY[cm]");
		hdR[m]->SetXTitle("dR[cm]");
	}

	TH1F *hdX_hor = new TH1F("","dX_hor",140, -20., 20.);
	TH1F *hdY_hor = new TH1F("","dY_hor",10, -20., 20.);
	TH1F *hdX_ver = new TH1F("","dX_ver",160, -20., 20.);
	TH1F *hdY_ver = new TH1F("","dY_ver",160, -20., 20.);

	TH1F *hdTheta = new TH1F("","",200, -5.*1E-4, 5.*1E-4);
	TH1F *hdTransMomentum = new TH1F("","",100, -4.*1E-4, 4.*1E-4);
	hdTheta->SetXTitle("dtheta [rad]");
	hdTransMomentum->SetXTitle("dt [(GeV)^{2}]");

	TH1F *hTheta_mc = new TH1F("","",100, 0., 4 *1E-2);
	hTheta_mc->SetLineColor(3);
	TH1F *hTheta_reco = new TH1F("","",100, 0., 4*1E-2);
	hTheta_reco->SetLineColor(4);


	TF1 *hfit_reco = new TF1("hfit_reco", "pol1",0.,1200.);
	TF1 *hfit_mc = new TF1("hfit_mc", "pol1",0.,1200.);

	TH2F *hPlan = new TH2F("","",100, -2.*1E-3, 2.1E-3, 100,-2.*1E-3, 2.1E-3);
	//Variable declaration
	Double_t energy;
	TVector3 pos_reco, pos_mc;
	Int_t nhits = 0;

	Double_t x_mc[4], x_reco[4], dx[4];
	Double_t y_mc[4], y_reco[4], dy[4];
	Double_t r_mc[4], r_reco[4], dr[4];
	Double_t z[4];
	Double_t z_err[4];
	//TODO errors !!
	Double_t err_x[4];
	Double_t err_y[4]};
	Double_t err[4] ;
	Double_t r_err[4];
	Double_t r_err_mc[4];
	Double_t theta_reco, theta_mc, dtheta, theta;
	Double_t t_reco, t_mc, dt;

	for ( Int_t i = 0 ; i < tr_reco->GetEntriesFast(); i++){
		tr_reco->GetEntry(i);
		tr_mc->GetEntry(i);

		if ( reco_array->GetEntriesFast() == 4 ){
			for (Int_t k = 0; k < reco_array->GetEntriesFast(); k++){
				PndLumiRecoHit *MyRecoHit = (PndLumiRecoHit*)reco_array->At(k);
				PndLumiPoint *MyPoint = (PndLumiPoint*)point->At(k);

				Int_t iTrack = MyPoint->GetTrackID();
				if (iTrack >=0){
					PndMCTrack *MyTrack = (PndMCTrack*)track->At(iTrack);
					if(MyTrack->GetMotherID()== -1){
						theta = MyTrack->GetMomentum().Theta();
					}
				}



				energy = MyRecoHit->GetRecoEnergy();
				pos_reco = MyRecoHit->GetRecoPosition();
				pos_mc = MyRecoHit->GetMCPosition();
				nhits = MyRecoHit->GetHitNumber();

				x_mc[k] = pos_mc.X();
				y_mc[k] = pos_mc.Y();
				r_mc[k] = TMath::Sqrt((x_mc[k]*x_mc[k])+
						(y_mc[k]*y_mc[k]));
				z[k] = pos_mc.Z();

				x_reco[k] = pos_reco.X();
				y_reco[k] = pos_reco.Y();
				r_reco[k] = TMath::Sqrt((x_reco[k]*x_reco[k])+
						(y_reco[k]*y_reco[k]));

				z_err[k] = 0.0;
				r_err[k] = err[k];

				dx[k] = x_reco[k] - x_mc[k];
				dy[k] = y_reco[k] - y_mc[k];
				hdX[k]->Fill(dx[k]);
				hdY[k]->Fill(dy[k]);


			}
			 
		}


		if ( reco_array->GetEntriesFast() == 4 ){

			TGraphErrors *tgr_mc = new TGraphErrors(4, z, r_mc,z_err,r_err_mc);
			tgr_mc->Fit("hfit_mc","QNF");
			theta_mc = TMath::ATan(hfit_mc->GetParameter(1));

			TGraphErrors *tgr_reco = new TGraphErrors(4, z, r_reco,z_err,r_err);
			tgr_reco->Fit("hfit_reco","QNF");
			theta_reco = TMath::ATan(hfit_reco->GetParameter(1));

			dtheta = theta_reco - theta;

			t_mc = -2*6.2*6.2*(1-TMath::Cos(theta));
			t_reco = -2*6.2*6.2*(1-TMath::Cos(theta_reco));
			dt = t_reco - t_mc;

			// Fill some histogram

			if (TMath::Abs(x_mc[0])>=3.0){
				hdX_hor->Fill((x_reco[0]-x_mc[0]));
				hdY_hor->Fill((y_reco[0]-y_mc[0]));
			}
			if (TMath::Abs(x_mc[0])<=1.0){
				hdX_ver->Fill((x_reco[0]-x_mc[0]));
				hdY_ver->Fill((y_reco[0]-y_mc[0]));
			}
			hdTheta->Fill(dtheta);
			hdTransMomentum->Fill(dt);

			hTheta_mc->Fill(theta);
			hTheta_reco->Fill(theta_reco);


		}


	}

	TCanvas *c_reco=new TCanvas("c_reco","c_reco",600,600);
	//c_reco->Divide(2);
	//c_reco->cd(1);
	//	hTheta_mc->Draw();
	//	hTheta_reco->Draw("same");
	//c_reco->cd(2);
		hdTheta->Draw();
		hdTheta-> Fit("gaus");
	TCanvas *c_reco6=new TCanvas("c_reco6","c_reco6",600,600);
	hdTransMomentum->Draw();
	hdTransMomentum-> Fit("gaus");


	TCanvas *c_reco1 = new TCanvas("c_reco1","c_reco1",0, 0, 600, 600);

	c_reco1.Divide(4,2);
	for(Int_t ic = 0 ; ic < 4; ic++){
		c_reco1.cd(ic+1); hdX[ic]->Draw();
		c_reco1.cd(ic+5); hdY[ic]->Draw();
	}


//	TCanvas *c_reco2 = new TCanvas("c_reco2","c_reco2",0, 0, 900, 900);
//	c_reco2.Divide(2);
//	c_reco2.cd(1); hdTheta->Draw();
//	c_reco2.cd(2); hdTransMomentum->Draw();

//	TCanvas *c_reco3 = new TCanvas("c_reco3","c_reco3",0, 0, 900, 900);
//	c_reco3.Divide(2,2);
//	c_reco3.cd(1); hdX_hor->Draw();
//	c_reco3.cd(2); hdY_hor->Draw();
//	c_reco3.cd(3); hdX_ver->Draw();
//	c_reco3.cd(4); hdY_ver->Draw();
//
//	TCanvas *c_reco4 = new TCanvas("c_reco4","c_reco4",0, 0, 900, 900);
//	c_reco4.Divide(2,2);
//	for(Int_t ic = 0 ; ic < 4; ic++){
//		c_reco4.cd(ic+1); hdR[ic]->Draw();
//	}

	//TCanvas *c_reco5 = new TCanvas("c_reco5","c_reco5",0, 0, 900, 900);
	//hPlan->Draw();


}
