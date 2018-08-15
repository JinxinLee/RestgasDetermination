//----------------------------------------------------------------------------
// Author: Mustafa Schmidt (University of Giessen)
// E-Mail: mustafa.a.schmidt@physik.uni-giessen.de
// Date:   13.09.2015
//----------------------------------------------------------------------------


int analysis()
{

	TFile *file=TFile::Open("sim.root","read");

	TTree *tree = (TTree*)file->Get("pndsim");

	TClonesArray *particle = new TClonesArray("PndDiscParticleMCPoint");
	TClonesArray *sensor = new TClonesArray("PndDiscSensorMCPoint");

	tree->SetBranchAddress("DiscParticleMCPoint", &particle);
	tree->SetBranchAddress("DiscSensorMCPoint", &sensor);

	int events = tree->GetEntries();

	//cout << "Number of events: " << events << endl;

	TH2F *posxy = new TH2F("posxy", "Position on Radiator Disk", 2000,-100,100,2000,-100,100);
	posxy->GetXaxis()->SetTitle("Position x [mm]");
	posxy->GetYaxis()->SetTitle("Position y [mm]");
	posxy->SetStats(0);

	//Loop charged particles

	for(int i = 0; i < events; i++)
	{
		tree->GetEntry(i);

		int entries = particle->GetEntries();

		//cout << "Number of entries in event: " << entries << endl;

		for(int j = 0; j < entries; j++)
		{
			double x = ((PndDiscParticleMCPoint*)particle->At(j))->GetX();
			double y = ((PndDiscParticleMCPoint*)particle->At(j))->GetY();
			double z = ((PndDiscParticleMCPoint*)particle->At(j))->GetZ();

			//cout << "x = " << x << " y = " << y << " z = " << z << endl;

			if(z == 194 && ((PndDiscParticleMCPoint*)particle->At(j))->is_primary == true)
			{
				posxy->Fill(x,y);
			}
		}
	}

	TCanvas *c1 = new TCanvas();
	posxy->Draw("colz");

	//Histogram for photon hits

	TH2F *possensor = new TH2F("possensor", "Position on PMT", 108,0,108,100,-30,30);
	possensor->GetXaxis()->SetTitle("Position x [mm]");
	possensor->GetYaxis()->SetTitle("Position y [mm]");
	possensor->SetStats(0);

	for(int i = 0; i < events; i++)
	{
		tree->GetEntry(i);

		int entries = sensor->GetEntries();

		//cout << "Number of entries in event: " << entries << endl;

		for(int j = 0; j < entries; j++)
		{
			double x = ((PndDiscSensorMCPoint*)sensor->At(j))->GetX();
			double y = ((PndDiscSensorMCPoint*)sensor->At(j))->GetY();
			double z = ((PndDiscSensorMCPoint*)sensor->At(j))->GetZ();

			int vol = ((PndDiscSensorMCPoint*)sensor->At(j))->GetDetectorID();

			//cout << "x = " << x << " y = " << y << " z = " << z << endl;

			possensor->Fill(vol,y*10);
		}

	}

	TCanvas *c2 = new TCanvas();
	possensor->SetMinimum(-1);
	possensor->Draw("colz");

	//Reading out digitization file
	TFile *file2=TFile::Open("digi.root","read");
	TTree *tree2 = (TTree*)file2->Get("pndsim");
	TClonesArray *digit = new TClonesArray("PndDiscDigitizedHit");
	tree2->SetBranchAddress("DiscDigit", &digit);

	//Reading out reconstruction file
	TFile *file3=TFile::Open("reco.root","read");
	TTree *tree3 = (TTree*)file3->Get("pndsim");
	TClonesArray *recon = new TClonesArray("PndDiscReconResult");
	tree3->SetBranchAddress("DiscPatternPrediction", &recon);

	//Reading out PID file
	TFile *file4=TFile::Open("pid.root","read");
	TTree *tree4 = (TTree*)file4->Get("pndsim");
	TClonesArray *pid = new TClonesArray("PndDiscPID");
	tree4->SetBranchAddress("DiscPID", &pid);


	//Histogram for pixel hits
	TH2F *hitpattern = new TH2F("hitpattern", "Simulated Hitpattern", 108,0,108,100,0,100);
	hitpattern->GetXaxis()->SetTitle("Sensor ID");
	hitpattern->GetYaxis()->SetTitle("Pixel Number");
	hitpattern->SetStats(0);

	for(int i = 0; i < events; i++)
	{
		tree2->GetEntry(i);

		int hits = digit->GetEntries();

		//cout << "Number of entries in hits: " << hits << endl;

		for(int j = 0; j < hits; j++)
		{
			int pixel = ((PndDiscDigitizedHit*)digit->At(j))->GetPixelNumber();
			int detector_id = ((PndDiscDigitizedHit*)digit->At(j))->GetDetectorID();
			int readout_id = ((PndDiscDigitizedHit*)digit->At(j))->GetReadoutID();
			double tdc = ((PndDiscDigitizedHit*)digit->At(j))->GetTdcTime()*0.05;
			//cout << "Pixel: " << pixel << " Sensor: " << readout_id << endl;

			hitpattern->Fill(readout_id+27*detector_id, pixel, tdc);
		}

	}

	//Histogram for predicted hitpattern
	TH2F *hprediction = new TH2F("hprediction", "Predicted Hitpattern", 108,0,108,100,0,100);
	hprediction->GetXaxis()->SetTitle("Sensor ID");
	hprediction->GetYaxis()->SetTitle("Pixel Number");
	hprediction->SetStats(0);

	for(int i = 0; i < events; i++)
	{
		tree3->GetEntry(i);

		int entries = recon->GetEntries();

		//cout << "Number of entries in prediction: " << entries << endl;

		for(int j = 0; j < entries; j++)
		{
			int pixel = ((PndDiscReconResult*)recon->At(j))->pixel;
			double time = ((PndDiscReconResult*)recon->At(j))->time;
			int sensor_id = ((PndDiscReconResult*)recon->At(j))->sensor;

			if(pixel != 1)
			{
				hprediction->Fill(sensor_id, pixel, time/events/3);
			}
		}

	}

	TCanvas *c3 = new TCanvas();
	int width = c3->GetWw();
	int height = c3->GetWh();
	c3->SetWindowSize(1.5*width,height);
	c3->Divide(2);
	c3->cd(1);
	hitpattern->SetMinimum(-1);
	hitpattern->Draw("colz");
	c3->cd(2);
	hprediction->Draw("colz");
	c3->Update();


	TH1F *hlikelihood = new TH1F("hlikelihood", "Difference of Likelihood", 100, -100, 100);

	for(int i = 0; i < events; i++)
	{
		tree4->GetEntry(i);

		int entries = pid->GetEntries();

		//cout << "Number of entries in PID: " << entries << endl;

		for(int j = 0; j < entries; j++)
		{
			double diff = ((PndDiscPID*)pid->At(j))->loglikepion - ((PndDiscPID*)pid->At(j))->loglikekaon;

			hlikelihood->Fill(diff);
		}

	}

	TCanvas *c4 = new TCanvas();
	hlikelihood->Draw();
	c4->Update();

	//file2->Close();
	//file3->Close();
  return 0;
}
