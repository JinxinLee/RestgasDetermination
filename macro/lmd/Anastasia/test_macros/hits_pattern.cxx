/*
 *
 *  Created on: Sep 20, 2014
 *  by A.Karavdina
 */
// to compile and run this macro do:
//# to compile it add in "# install #" part of ${PANDAROOT}/macro/lmd/Anastasia/test_macros/CMakeLists.txt lines:
//# add_executable(hits_pattern hits_pattern.cxx)
//# target_link_libraries(hits_pattern -lrt ${ROOT_LIBRARIES} LmdTool MathMore)
//#
//# to run it (and see options): 
//# ${PANDAROOT}/build/bin/./hits_pattern --help


#include<iostream>
#include<sstream>
#include<string>

#include<TString.h>
#include<TChain.h>
#include<TClonesArray.h>
#include<TROOT.h>
#include<TVector3.h>
#include<TH1.h>
#include<TH2.h>
#include<TApplication.h>
#include<TCanvas.h>
#include<LmdQA/PndLmdDigiQ.h>
#include<PndLmdDim.h>
#include<PndSdsMCPoint.h>
#include "TStyle.h"

using namespace std;

double last_percent(0);

void DrawProgressBar(int len, double percent) {
	if ((int)(last_percent*100) == (int)(percent*100)) return;
	//cout << " drawing " << endl;
	cout << "\x1B[2K"; // Erase the entire current line.
	cout << "\x1B[0E"; // Move to the beginning of the current line.
	string progress;
	for (int i = 0; i < len; ++i) {
	if (i < static_cast<int>(len * percent)) {
	progress += "=";
	} else {
	progress += " ";
	}
	}
	cout << "[" << progress << "] " << (static_cast<int>(100 * percent)) << "%";
	flush(cout); // Required.
	last_percent = percent;
}

int main(int nargs, char** args) {

  // const int scalefac = 1;// TEST
	// load first all dictionaries for ROOT in order
	// to read the files correctly
	gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
	// TStyle* m_gStyle = new TStyle("style_name","get rid from headears");
	// m_gStyle->SetOptStat(0);
	// m_gStyle->SetLabelSize(0.045,"xyz");
	// m_gStyle->SetTitleYSize(0.04);
	// m_gStyle->SetTitleXSize(0.04);
	// gROOT->SetStyle("style_name");
	TApplication myapp("myapp",0,0);
	TString storePath = ".";
	TString Pbeam = "0";
	// set the storepath to somewhere else, if provided
	if (nargs == 3) {
		storePath = args[1];
		Pbeam = args[2];
	} else {
		if (nargs == 1)
			cout << " Hint: you may change the standard path from "
					<< storePath.Data()
					<< " to any other folder by providing the path as an argument! "
					<< endl;
		if (nargs > 3)
			cout << " Error: too many arguments! " << endl;
	}

	TString outname = storePath + "/countRate_x_y_";
	outname+=Pbeam;
	outname+="GeV";
	TString outname_pdf_o = outname + ".pdf(";
	TString outname_pdf_c = outname + ".pdf)";
	TString outname_root = outname + ".root";

	TString startEvent = "0";
	// Input file (MC events)
	TString MCFile = storePath + "/Lumi_MC_";
	MCFile += startEvent;
	MCFile += ".root";
	TChain tMC("cbmsim");
	tMC.Add(MCFile);
	// Input file (sorted digi)
	TString DigiFile = storePath+"/Lumi_DigisQA_";
	DigiFile += startEvent;
	DigiFile += ".root";
	TChain tDigi("cbmsim");
	tDigi.Add(DigiFile);


	std::cout << "MCFile  : " << MCFile.Data() << std::endl;
	std::cout << "DigiFile  : " << DigiFile.Data() << std::endl;

	//--- MC info -----------------------------------------------------------------
	TClonesArray* true_tracks = new TClonesArray("PndMCTrack");
	tMC.SetBranchAddress("MCTrack", &true_tracks); //True Tracks to compare

	TClonesArray* true_points = new TClonesArray("PndSdsMCPoint");
	tMC.SetBranchAddress("LMDPoint", &true_points); //True Points to compare
	//----------------------------------------------------------------------------------
	int nEvents = tMC.GetEntries();

	//--- DigiQ info -----------------------------------------------------------------
	TClonesArray* digiq_points = new TClonesArray("PndLmdDigiQ");
	tDigi.SetBranchAddress("LMDPixelDigisQ", &digiq_points); //Digi hits
	//--------------------------------------------------------------------------------

	// Estimate scale factor (for count rate per second) -----------------------------
	double sigTot=0;
	double lumi=1e32;
	if(Pbeam=="1_5"){ //1.5 GeV
	   sigTot=124.63; // mb el(theta_min=0.12 grad)+inel
	}
	else{
	  if(Pbeam=="15"){ // 15 GeV
	    sigTot=50.26; // mb el(theta_min=0.12 grad)+inel
	  }
	  else{
	    sigTot=100;// some default value
	    cout<<"Sorry, I'm silly programm and have no idea about cross-section at Pbeam = "<<Pbeam<<endl;
	  }
	}
	double Nref = sigTot*1e-27*lumi;
	const double scalefac = (Nref/nEvents)*1e-3;//[in kHz]
	//--------------------------------------------------------------------------------

	// initialize the lmd dimension class
	PndLmdDim& lmddim = PndLmdDim::Get_instance();
	//	TString mtx_corr =  storePath+"/matrices_corrected.txt";
	TString mtx_corr =  "/home/karavdina/soft/pandaroot/geometry/trafo_matrices_lmd.dat";
	TString mtx_perfect = "/home/karavdina/soft/pandaroot/geometry/trafo_matrices_lmd.dat";
	lmddim.Read_transformation_matrices(mtx_perfect.Data(), false);
	lmddim.Read_transformation_matrices(mtx_corr.Data(), true);

	TCanvas canvas_map_x_y("canvas_map_x_y", "map x y", 800, 800);
	canvas_map_x_y.Divide(2,2);
	int maxSum=180;
	int maxEl=180;
	int maxInEl=50;
	if(Pbeam=="15"){
	  maxSum=25;

	  maxEl=25;
	  maxInEl=25;
	}
	TH2Poly* el_map_x_y_plane[4][2];
	TH2Poly* inel_map_x_y_plane[4][2];
	TH2Poly* sum_map_x_y_plane[4][2];
	for(int ipl=0; ipl<4; ipl++){
	  for(int is=0; is<2; is++){
	    TString name = "plane ";
	    name +=ipl;
	    name += " side ";
	    name +=is;
	    TString name_el = name + " (el. signal)";
	    TString name_inel = name + " (inel. signal)";
	    TString name_sum = name + " (el.+inel. signal)";
	    el_map_x_y_plane[ipl][is] = lmddim.Get_histogram_Plane(ipl,is);
	    el_map_x_y_plane[ipl][is]->SetTitle(name_el);
	    el_map_x_y_plane[ipl][is]->SetXTitle("X [cm]");
	    el_map_x_y_plane[ipl][is]->SetYTitle("Y [cm]");
	    el_map_x_y_plane[ipl][is]->SetZTitle("rate [kHz]");
	    el_map_x_y_plane[ipl][is]->GetZaxis()->SetRangeUser(0,maxEl);
	    el_map_x_y_plane[ipl][is]->SetContour(100);
	    inel_map_x_y_plane[ipl][is] = lmddim.Get_histogram_Plane(ipl,is);
	    inel_map_x_y_plane[ipl][is]->SetTitle(name_inel);
	    inel_map_x_y_plane[ipl][is]->SetXTitle("X [cm]");
	    inel_map_x_y_plane[ipl][is]->SetYTitle("Y [cm]");
	    inel_map_x_y_plane[ipl][is]->SetZTitle("rate [kHz]");
	    inel_map_x_y_plane[ipl][is]->GetZaxis()->SetRangeUser(0,maxInEl);
	    inel_map_x_y_plane[ipl][is]->SetContour(100);
	    sum_map_x_y_plane[ipl][is] = lmddim.Get_histogram_Plane(ipl,is);
	    sum_map_x_y_plane[ipl][is]->SetTitle(name_sum);
	    sum_map_x_y_plane[ipl][is]->SetXTitle("X [cm]");
	    sum_map_x_y_plane[ipl][is]->SetYTitle("Y [cm]");
	    sum_map_x_y_plane[ipl][is]->SetZTitle("rate [kHz]");
	    sum_map_x_y_plane[ipl][is]->GetZaxis()->SetRangeUser(0,maxSum);
	    sum_map_x_y_plane[ipl][is]->SetContour(100);
	  }
	}
	
	
	cout << " reading " << nEvents << " Events " << endl;
	int TOT_count=0;
	int el_count=0;
	int inel_count=0;
	for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
		DrawProgressBar(50, (iEvent+1)/((double)nEvents));
		tMC.GetEntry(iEvent);
		tDigi.GetEntry(iEvent);
		const int nDigi = digiq_points->GetEntriesFast();
		for (Int_t i=0; i<nDigi; i++){
		  PndLmdDigiQ* DigiPoint = (PndLmdDigiQ*)(digiq_points->At(i)); // read digi hit
		  bool sigFl = DigiPoint->GetFlSig();
		  int sensorID = DigiPoint->GetSensorID();
		  int column = DigiPoint->GetPixelColumn();
		  int row = DigiPoint->GetPixelRow();
		  int ihalf, iplane, imodule, iside, idie, isensor;
		  //  cout<<"sensorID = "<<sensorID<<endl;
		  lmddim.Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
		  TVector3 hitCoor = lmddim.Decode_hit(sensorID,column,row,true);
		  TVector3 hitCoorlmd = lmddim.Transform_global_to_lmd_local(hitCoor);
		  // cout<<hitCoor.X()<<" "<<hitCoor.Y()<<endl;
		  // Int_t ind = DigiPoint->GetIndex(0);
		  // PndSdsMCPoint* MCPoint = (PndSdsMCPoint*)true_points->At(ind);
		  // TVector3 MCcoord = MCPoint->GetPosition();
		  //TVector3 MClmd = lmddim.Transform_global_to_lmd_local(MCcoord);
		  //	  cout<<ihalf<<" "<<iplane<<" "<<imodule<<" "<<iside<<" "<<idie<<" "<<isensor<<endl;
		  //	  cout<<MCPoint->GetX()<<" "<<MCPoint->GetY()<<endl;
		  sum_map_x_y_plane[iplane][iside]->Fill(hitCoorlmd.X(),hitCoorlmd.Y(),scalefac);
		  TOT_count++;
		  if(sigFl){
		    el_map_x_y_plane[iplane][iside]->Fill(hitCoorlmd.X(),hitCoorlmd.Y(),scalefac);
		    el_count++;
		  }
		  else{
		    inel_map_x_y_plane[iplane][iside]->Fill(hitCoorlmd.X(),hitCoorlmd.Y(),scalefac);
		    inel_count++;
		  }
		 
		}

		if ((iEvent%1000)==0){
		  canvas_map_x_y.cd(1);
		  el_map_x_y_plane[0][0]->Draw("COLZ");
		  lmddim.Draw_Sensors(0,false,true,0);
		  canvas_map_x_y.cd(2);
		  inel_map_x_y_plane[0][0]->Draw("COLZ");
		  lmddim.Draw_Sensors(0,false,true,0);
		  canvas_map_x_y.cd(3);
		  el_map_x_y_plane[0][1]->Draw("COLZ");
		  lmddim.Draw_Sensors(0,false,true,0);
		  canvas_map_x_y.cd(4);
		  inel_map_x_y_plane[0][1]->Draw("COLZ");
		  lmddim.Draw_Sensors(0,false,true,0);
		  canvas_map_x_y.Update();
		}
	}

	TCanvas canvas_map("canvas_map_x_y", "map x y", 900, 900);
	for(int ipl=0; ipl<4; ipl++){
	  for(int is=0; is<2; is++){
	    sum_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_map.Print(outname_pdf_o.Data());
	    canvas_map.Clear();
	    el_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_map.Print(outname_pdf_o.Data());
	    canvas_map.Clear();
	    inel_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_map.Print(outname_pdf_o.Data());
	    canvas_map.Clear();
	  }
	}
	canvas_map.Clear();
	canvas_map.Print(outname_pdf_c.Data());

	TFile *f = new TFile(outname_root,"RECREATE");
	for(int ipl=0; ipl<4; ipl++){
	  for(int is=0; is<2; is++){
	    TString name_can = "canvas_map_x_y_sum_pl";
	    name_can +=ipl;
	    name_can +="_side_";
	    name_can +=is;
	    TCanvas canvas_1(name_can, "map x y (sum)", 900, 900);
	    sum_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_1.Write();
	    name_can = "canvas_map_x_y_el_pl";
	    name_can +=ipl;
	    name_can +="_side_";
	    name_can +=is;
	    TCanvas canvas_2(name_can, "map x y (el)", 900, 900);
	    el_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_2.Write();
	    name_can = "canvas_map_x_y_inel_pl";
	    name_can +=ipl;
	    name_can +="_side_";
	    name_can +=is;
	    TCanvas canvas_3(name_can, "map x y (inel)", 900, 900);
	    inel_map_x_y_plane[ipl][is]->Draw("COLZ");
	    lmddim.Draw_Sensors(ipl,false,true,is);
	    canvas_3.Write();
	  }
	}
	f->Write();
	f->Close();
	cout<<"------- SUMMARY -------"<<endl;
	cout<<"Pbeam = "<<Pbeam<<"GeV tot. cross-section "<<sigTot<<" mb"<<endl;
	cout<<"Overall count rate: "<<TOT_count*scalefac*1e-3<<" MHz"<<endl;
	cout<<"Elastic count rate: "<<el_count*scalefac*1e-3<<" MHz"<<endl;
	cout<<"Inelastic count rate: "<<inel_count*scalefac*1e-3<<" MHz"<<endl;
}
