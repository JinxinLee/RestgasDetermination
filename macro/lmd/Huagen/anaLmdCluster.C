/*
 * anaLmdCluster.C
 *  Created on: Oct 6, 2009
 *      Author: huagen
 */
//the macro anaLmdCluster is to plot the reco_hits and track theta
{

 // load library by run Macro
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

 // Timer
	TStopwatch timer;
	timer.Start();

// define some variables
	int nEvents = 1e7;
	bool verbose = false;
	double pi = TMath::Pi();

//To open the data file, MC point and Recon Hit
	TFile *mcf = new TFile("/private/huagen/simdata/Lmd_MC_BD_DPM_elastic_6.2_1.9mrad_5M_1.root");
//	TFile *mcf = new TFile("/private/huagen/simdata/Lmd_Test.root");
	TTree *tree1 = (TTree*)mcf->Get("cbmsim");
	TClonesArray *mc_array = new TClonesArray("PndSdsMCPoint");
	tree1->SetBranchAddress("LMDPoint",&mc_array);

	TClonesArray *mc_track = new TClonesArray("PndMCTrack");
	tree1->SetBranchAddress("MCTrack",&mc_track);

	TFile *rcf = new TFile("/private/huagen/simdata/Lmd_Reco_BD_DPM_elastic_6.2_1.9mrad_5M_1.root");
//	TFile *rcf = new TFile("/private/huagen/simdata/Lmd_Test_Reco.root");
	TTree  *tree2 = (TTree*)rcf->Get("cbmsim");
	TClonesArray *hit_array = new TClonesArray("PndSdsHit");
	tree2->SetBranchAddress("LMDHitsStrip",&hit_array);

//To define the histograms
	TH1F *mcTheta = new TH1F("MCHit theta","Theta fitted by MCHits",300, 0.001, 0.01);
	TH1F *rcTheta = new TH1F("Reco theta","Theta determined by RecoHits",300, 0.001, 0.01);
    //Define the plots for hit on first plane
    TH1F *refTheta = new TH1F("MCTrue theta","The track theta set in Generator",300, 0.001, 0.01);

    TH1F *dTheta = new TH1F("FirstMCHit-MCTrue","delta theta between FirstMCHit and MCTrue",300,-0.003,0.003);
	TH1F *dTheta0 = new TH1F("RecoHit-MCTrue","delta theta between RecoHit and MCTrue theta",300, -0.003,0.003);//theta resolution
//	TH1F *dTheta1 = new TH1F("FirstRecoHit-MCTrue","",300, -0.003,0.003);
	TH1F *dTheta2 = new TH1F("MCHit-MCTrue","",300, -0.003,0.003);
    TH1F *dTheta3 = new TH1F("RecoHit-MCHit","delta theta between RecoHit and MCHit",300, -0.003,0.003); //digi&reco effect

    TF1 *trackFit = new TF1("trackFit","pol1",-40,40);

// Define some gobal variables
	TVector3 vecmc, vecrc;
	double mcX=0, mcY=0, mcZ=0, rcX=0, rcY=0, rcZ=0;
	double trackthe,mctheta, rctheta,mcfirstthe,rcfirstthe,dthe0,dthe1,dthe2,dthe3,dtheta;
    double mc_x[4],mc_y[4],mc_z[4],r_mc[4];
    double rc_x[4],rc_y[4],rc_z[4],r_rc[4];
    double r_mc_err[4],r_rc_err[4],z_mc_err[4],z_rc_err[4];

//Loop the event and get theta
	for(int i =0; i<nEvents&&i<tree1->GetEntriesFast();i++)
	{
     	tree2->GetEntry(i);
		tree1->GetEntry(i);
		cout<<"the Event No is "<<i<<endl;

       if(hit_array->GetEntriesFast()==4)
       {
		for (int j=0; j<hit_array->GetEntriesFast();j++)
		  {//	cout<<"the point no "<<j<<" of Event "<<i<<endl;
			if(hit_array->GetEntriesFast()!=mc_array->GetEntriesFast()) continue;
		    PndSdsHit *hit = (PndSdsHit*)hit_array->At(j);
			PndSdsMCPoint *point = (PndSdsMCPoint*)mc_array->At(j);

			mcX = (point->GetPosition().X()+point->GetPositionOut().X())/2;
		    mcY = (point->GetPosition().Y()+point->GetPositionOut().Y())/2;
		    mcZ = (point->GetPosition().Z()+point->GetPositionOut().Z())/2;

//			mcX = point->GetPosition().X();//+point->GetPositionOut().X())/2;
//		    mcY = point->GetPosition().Y();//+point->GetPositionOut().Y())/2;
//		    mcZ = point->GetPosition().Z();//+point->GetPositionOut().Z())/2;
            cout<<"the mcX, mcY, mcZ are "<<mcX<<","<<mcY<<","<<mcZ<<endl;
		    vecmc.SetXYZ(mcX,mcY,mcZ);
		    vecrc = hit->GetPosition();
		    cout<<"the rcX, rcY, rcZ are "<<vecrc.x()<<","<<vecrc.Y()<<","<<vecrc.Z()<<endl;
		    Int_t iTrack = point->GetTrackID();
	//	    cout<<"The iTrack (point->GetTrackID() is :"<<iTrack<<endl;
			//if (iTrack>=0)
			// {
				PndMCTrack *track = (PndMCTrack*)mc_track->At(iTrack);
			// }
				if(track->GetMotherID()== -1) trackthe = track->GetMomentum().Theta();
				cout<<"the track theta is "<<trackthe<<endl;
		//     }
				   mc_x[j]=vecmc.X();
				   mc_y[j]=vecmc.Y();
				   mc_z[j]=vecmc.Z()-1110;
				   r_mc[j]=TMath::Sqrt(mc_x[j]*mc_x[j]+mc_y[j]*mc_y[j]);
				   std::cout<<"the r_mc[j] is "<<r_mc[j]<<std::endl;
				   std::cout<<"the mc_z[j] is "<<mc_z[j]<<std::endl;
				   r_mc_err[j]=0;
                   z_mc_err[j]=0;

				   rc_x[j]=vecrc.X();
				   rc_y[j]=vecrc.Y();
				   rc_z[j]=vecrc.Z()-1110;
				   r_rc[j]=TMath::Sqrt(rc_x[j]*rc_x[j]+rc_y[j]*rc_y[j]);
				   std::cout<<"the r_rc[j] is "<<r_rc[j]<<std::endl;
				   std::cout<<"the rc_z[j] is "<<rc_z[j]<<std::endl;
                   r_rc_err[j]=0;
                   z_rc_err[j]=0;
                   double tempmc = TMath::Sqrt(mc_x[0]*mc_x[0]+mc_y[0]*mc_y[0]);
                       	    mcfirstthe = TMath::ATan(tempmc/(mc_z[0]+1110));
                       	    cout<<"the first hit theta is "<<mcfirstthe<<endl;

       }//loop all events containing for 4 hits

       TGraphErrors *mctrack = new TGraphErrors(4,mc_z,r_mc,z_mc_err,r_mc_err);
       mctrack->Fit("trackFit","ONF");
       mctheta=TMath::ATan(trackFit->GetParameter(1));
       cout<<"the mc fit theta is "<<mctheta<<endl;

       TGraphErrors *rctrack = new TGraphErrors(4,rc_z,r_rc,z_rc_err,r_rc_err);
       rctrack->Fit("trackFit","ONF");
       rctheta=TMath::ATan(trackFit->GetParameter(1));
       cout<<"the rc fit theta is "<<rctheta<<endl;

        dthe0 = rctheta-trackthe;
	    dthe2 = mctheta-trackthe;
        dthe3 = rctheta-mctheta;
	    dtheta = mcfirstthe-trackthe;

		 //fill the histograms
	    rcTheta->Fill(rctheta);
	    mcTheta->Fill(mctheta);
        refTheta->Fill(trackthe);

	    dTheta0->Fill(dthe0);
        dTheta2->Fill(dthe2);
        dTheta3->Fill(dthe3);
        dTheta->Fill(dtheta);
	  //fill the plots
  }
}

	//draw the plots
    Double_t par[3];
    TF1 *total = new TF1("total","gaus",-0.06,0.06);
    total->SetLineColor(kRed);
    total->SetLineWidth(1);

    TCanvas *can1 = new TCanvas("Theta","MC,RC,Track theta",0,0,800, 800);
    can1->Divide(2,2);
    TPad* mypad = 0;

    can1->cd(1);    mcTheta->DrawCopy(); //mcTheta->Fit("gaus");
    can1->cd(2);    rcTheta->DrawCopy(); //rcTheta->Fit("gaus");
    can1->cd(3);    refTheta->DrawCopy();

    gStyle->SetOptFit();
    TCanvas *can2 = new TCanvas("Delta theat","MC,RC,Track",0,0,800, 800);
    can2->Divide(2,2);

    can2->cd(1);    dTheta0->DrawCopy();   dTheta0->Fit("total","R");
//    can2->cd(3);    dTheta1->DrawCopy(); //  dTheta1->Fit("total","R");
    can2->cd(2);    dTheta2->DrawCopy();   dTheta2->Fit("total","R");
    can2->cd(3);    dTheta3->DrawCopy();   dTheta3->Fit("total","R");
    can2->cd(4);    dTheta ->DrawCopy();   dTheta->Fit("total","R");

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
   cout<<endl<<endl;
   cout<<"the macro finished successfully"<<endl;
   cout<<"the real time is "<<rtime<<" and CPU time is "<<ctime<<endl;
   cout<<endl;

}
