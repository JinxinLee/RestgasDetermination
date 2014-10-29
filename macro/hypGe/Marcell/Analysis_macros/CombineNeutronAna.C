#include<TH1D.h>
#include<TH2D.h>
#include<stdio.h>
#include<TString.h>
#include"TFile.h"
#include<iostream>
#include<fstream>

using namespace std;

void CombineNeutronAna(TString FileListname_ext,TString OutfileName_ext)
{
	TString PathFirstPart = getenv("SIMDATADIR");
	TString FullPath = PathFirstPart;
	FullPath += "/Neutron/Ana/";
	TString FileListname = FullPath;
	FileListname +=FileListname_ext;
	//FileListname +="TripleBall30_urqmd_2_filelist.txt";
	cout << FileListname<< endl;
	
	TH1D* hCombinedNHits;
	TH1D* hCombinedNHits_p;
	TH1D* hCombinedNAll;
	TH1D* hCombinedPAll;
	TH1D* hCombinedNMom;
	TH1D* hCombinedCrystalHits;
	TH1D* hCombinedNeutronEnergyDeposit;
	TH2D* hCombinedNeutronXYDistribution;
	
	TH1D* hCombinedNHits_buffer;
	TH1D* hCombinedNHits_p_buffer;
	TH1D* hCombinedNAll_buffer;
	TH1D* hCombinedPAll_buffer;
	TH1D* hCombinedNMom_buffer;
	TH1D* hCombinedCrystalHits_buffer;
	TH1D* hCombinedNeutronEnergyDeposit_buffer;
	TH2D* hCombinedNeutronXYDistribution_buffer;
	
	TH1D* hNeutronsPerCrystal = new TH1D("hNeutronsPerCrystal","Neutrons per Crystal",10000,0,9999);
	
	ifstream file(FileListname.Data());
	if(!file)												// check if file exists
	{
		cout << "ERROR - file does NOT exist" << endl;
	}
	else
	{
		Int_t i = 0;
		char buffer[100];
		TFile* InFile[1000];
		while (file.good())
		{
			file.getline(buffer,200);
			cout << buffer << endl;
			
			TString InFilename = FullPath;
			InFilename += buffer;
			cout << InFilename<< endl;
			InFile[i]= new TFile(InFilename);
			if (i ==0)
			{
				InFile[i]->GetObject("hNHits",hCombinedNHits);
				InFile[i]->GetObject("hNHits_p",hCombinedNHits_p);
				InFile[i]->GetObject("hNAll",hCombinedNAll);
				InFile[i]->GetObject("hPAll",hCombinedPAll);
				InFile[i]->GetObject("hNMom",hCombinedNMom);
				InFile[i]->GetObject("hCrystalHit",hCombinedCrystalHits);
				InFile[i]->GetObject("hNeutronEnergyDeposit",hCombinedNeutronEnergyDeposit);
				InFile[i]->GetObject("hNeutronXYDistribution",hCombinedNeutronXYDistribution);
				//hCombinedNAll->Draw();
				//cout << i << "   "<< hCrystalHits->GetEntries() << endl;
			}
			else
			{
				InFile[i]->GetObject("hNHits",hCombinedNHits_buffer);
				InFile[i]->GetObject("hNHits_p",hCombinedNHits_p_buffer);
				InFile[i]->GetObject("hNAll",hCombinedNAll_buffer);
				InFile[i]->GetObject("hPAll",hCombinedPAll_buffer);
				InFile[i]->GetObject("hNMom",hCombinedNMom_buffer);
				InFile[i]->GetObject("hCrystalHit",hCombinedCrystalHits_buffer);
				InFile[i]->GetObject("hNeutronXYDistribution",hCombinedNeutronXYDistribution_buffer);
				
				//cout << i << "   "<< hCrystalHits_buffer->GetEntries()<< endl;
				
				hCombinedNHits->Add(hCombinedNHits_buffer,1);
				hCombinedNHits_p->Add(hCombinedNHits_p_buffer,1);
				hCombinedNAll->Add(hCombinedNAll_buffer,1);
				hCombinedPAll->Add(hCombinedPAll_buffer,1);
				hCombinedNMom->Add(hCombinedNMom_buffer,1);
				hCombinedCrystalHits->Add(hCombinedCrystalHits_buffer,1);
				hCombinedNeutronEnergyDeposit->Add(hCombinedNeutronEnergyDeposit_buffer,1);
				hCombinedNeutronXYDistribution->Add(hCombinedNeutronXYDistribution_buffer,1);
				
			}
			//InFile[i]->Close();
			i++;
		}
		//TString InFilename = FullPath;
		//InFilename += buffer;
		//cout << InFilename<< endl;
		//TFile InFile(InFilename);
		//InFile.GetListOfKeys()->Print();
	}
	file.close();
	
	
	TString OutFilename =FullPath;
	OutFilename +="Combined/";
	OutFilename += OutfileName_ext;
	//OutFilename +="test1.root";
	TString TxTOutFilename = OutFilename;
	TxTOutFilename += ".txt";
	ofstream TxTOutfile(TxTOutFilename.Data());
	Int_t CrystalNumber = 1;
	cout << "Bin\tCrystal\tCluster\tNeutron hits"<<endl;
	TxTOutfile << "Bin\tCrystal\tCluster\tNeutron hits"<<endl;
	for (Int_t iBin=0;iBin<1700;iBin++)
	{
		
		if(hCombinedCrystalHits->GetBinContent(iBin))
		{
			cout << iBin << "\t"<< CrystalNumber <<"\t"<< iBin/100 <<"\t"<< hCombinedCrystalHits->GetBinContent(iBin)<< endl;
			hNeutronsPerCrystal->Fill(hCombinedCrystalHits->GetBinContent(iBin));
			TxTOutfile << iBin << "\t"<< CrystalNumber <<"\t"<< iBin/100 <<"\t"<< hCombinedCrystalHits->GetBinContent(iBin)<< endl;
			CrystalNumber++;
			
		}
	}
	TxTOutfile.close();
	//hCombinedCrystalHits->Draw();
	// Write combined output to file
	
	cout << OutFilename << endl;
	TFile *OutFile= new TFile(OutFilename,"RECREATE");
	OutFile->cd();
	hCombinedNHits->Write();
	hCombinedNHits_p->Write();
	hCombinedNAll->Write();
	hCombinedPAll->Write();
	hCombinedNMom->Write();
	hCombinedCrystalHits->Write();
	hNeutronsPerCrystal->Write();
	hCombinedNeutronEnergyDeposit->Write();
	hCombinedNeutronXYDistribution->Write();
	//hNeutronsPerCrystal->Draw();
	OutFile->ls();
	OutFile->Close();
}
