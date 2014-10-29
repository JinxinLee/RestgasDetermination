#include<TH1D.h>
#include<TH2D.h>
#include<stdio.h>
#include<TString.h>
#include"TFile.h"
#include<iostream>
#include<fstream>
#include"TROOT.h"

using namespace std;

void Combine_COSY_Ana()//TString FileListname_ext,TString OutfileName_ext)
{
	TString PathFirstPart = getenv("SIMDATADIR");
	TString FullPath = PathFirstPart;
	FullPath += "/COSY/Ana/ToCombine/";
	
	TString OutputPath = PathFirstPart + "/COSY/Ana/Combined/";		// more in while loop
	
	TString FileListFolder = FullPath;					// more is added later in while loop
	
	TString FolderListname = FileListFolder +"FolderListToCombine.txt";// File with folders of analysed data to combine
	
	char CommandBuffer[300];
	sprintf(CommandBuffer,".!ls -d %s*mom*Evts*/ > %s",FileListFolder.Data(),FolderListname.Data());
	gROOT->ProcessLine(CommandBuffer);
	ifstream FolderFile(FolderListname.Data());		
	int nDataset = 0;
	if(!FolderFile)
	{
		cout << "ERROR - file does NOT exist" << endl;
	}
	else
	{
		cout << "opened " << FolderListname.Data() << endl;
		while (FolderFile.good())
		{
			
			nDataset++;
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
	
			char FolderBuffer[200];
			FolderFile.getline(FolderBuffer,200);
			TString FolderStringBuffer = FolderBuffer;
			if (FolderStringBuffer.Length() == 0)
			{
				cout << "No more data found!" << endl;
				continue;
			}
			else
			{
				FolderStringBuffer.Remove(FolderStringBuffer.Length()-1);
				FolderStringBuffer.Remove(0,FolderStringBuffer.Last('/')+1);
				cout << "Combining Dataset No :" << nDataset<<"\t\t" <<"Folder: "<< FolderStringBuffer << endl;
			}
			FileListFolder = FullPath + FolderStringBuffer;
			char CommandBuffer[400];
			TString FileListname = FileListFolder + "/FileList.txt";
			sprintf(CommandBuffer,".!ls %s/*.root > %s",FileListFolder.Data(),FileListname.Data());
			cout << "Processing " << CommandBuffer<< endl;	
			gROOT->ProcessLine(CommandBuffer);
						
			ifstream file(FileListname.Data());
			if(!file)												// check if file exists
			{
				cout << "ERROR - file does NOT exist" << endl;
			}
			else
			{
				Int_t i = 0;
				char buffer[500];
				TFile* InFile[1000];
				while (file.good())
				{
					file.getline(buffer,500);
					//cout << buffer << endl;
					
					TString InFilename = buffer;
					if (!InFilename.EndsWith(".root"))
						continue;
					//InFilename += buffer;
					//cout << "IFN  " <<i << " \t"<<	 InFilename<< endl;
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
						InFile[i]->GetObject("hNeutronEnergyDeposit",hCombinedNeutronEnergyDeposit_buffer);
						InFile[i]->GetObject("hNeutronXYDistribution",hCombinedNeutronXYDistribution_buffer);
						
						//cout << i << "   "<< hCrystalHits_buffer->GetEntries()<< endl;
						
						hCombinedNHits->Add(hCombinedNHits_buffer,1) ;
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
		
			// writing histograms to file
			TString OutFilename =OutputPath;
			OutFilename += FolderStringBuffer;

			TString TxTOutFilename = OutFilename;
			OutFilename += ".root";
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
			delete OutFile;
			
			delete hNeutronsPerCrystal;
		}
		FolderFile.close();
	}
}
