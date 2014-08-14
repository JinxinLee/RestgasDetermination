// --------------------------------------------------------------------------
// DalitzGui - ROOT based tool to plot 3body decay's with resonant behaviour
// --------------------------------------------------------------------------
// 
// Check accompanying README file for infos how to use
//
// Original author: Klaus Goetzen - GSI Darmstadt
// Last modified  : 2014/08/08
// --------------------------------------------------------------------------

#include <TApplication.h>
#include <TClass.h>
#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLayout.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TGSlider.h"
#include "TGTextEntry.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "TGComboBox.h"
#include "TGMenu.h"
#include "TGDockableFrame.h"
#include "TGFileDialog.h"  
#include "TGFrame.h"
#include "TGDoubleSlider.h"
#include "TSystem.h"
#include "TLatex.h"

#include "TClass.h"
#include "TStyle.h"
#include "TString.h"

#include "TH1F.h"
#include "TGraph.h"

#include <iostream>
#include <fstream>

#include "DalitzGUI.h"
#include "KinTools.cpp"

const char *filetypes[] =     { "DalitzGui cfg files",    "*.cfg", "All files",     "*",      0, 0 };
const char *filetypesgif[] =  { "GIF Images",    "*.gif",     0, 0 };
const char *filetypesscrn[] = { "All files",     "*",  0, 0 };

enum ETestCommandIdentifiers {
   HCId1,HCId2,  HId1,
   
   HSId1,HSId2,HSId3,
   
   HComId1,HComId2,HComId3,
   
   HComRId1,HComRId2,HComRId3,HComRId4,HComRId5,HComRId6,
   
   HSlRAId1,HSlRAId2,HSlRAId3,HSlRAId4,HSlRAId5,HSlRAId6,
   
   HSlRpId1,HSlRpId2,HSlRpId3,HSlRpId4,HSlRpId5,HSlRpId6,
   
   HSlRjId1,HSlRjId2,HSlRjId3,HSlRjId4,HSlRjId5,HSlRjId6,
   
   HChRId1,HChRId2,HChRId3,HChRId4,HChRId5,HChRId6,
   
   HChId1, HChId2, HChId3, HSlBins, HComPlot,
   
   M_FILE_OPEN, M_FILE_SAVE, M_FILE_SAVE_SCRN, M_FILE_EXIT, M_ANIGIF
};

const int MAX_FIN = 100; 
const int MAX_RES = 100;
const int MAX_BIN = 500;

int N_FIN=0, N_RES=0;

const double plot_margin = 0.05;  // margin of plots as fraction of phsp range

// Parameters for final states and resonances will be read from file 'DalitzGUI.ini'
Int_t    id_fin[MAX_FIN];
Double_t m_fin[MAX_FIN];
TString  nam_fin[MAX_FIN], nam_pfin[MAX_FIN];

Int_t	 id_res[MAX_RES], J_res[MAX_RES];
Double_t m_res[MAX_RES], G_res[MAX_RES];
TString name_res[MAX_RES];

// The available variables for animated GIF creation
const TString  vars_tex[13] = {"#sqrt{s}", "A_{23,1}","#phi_{23,1}", "A_{23,2}","#phi_{23,2}", "A_{31,1}","#phi_{31,1}", "A_{31,2}","#phi_{31,2}",
								 "A_{12,1}","#phi_{12,1}", "A_{12,2}","#phi_{12,2}"};
const TString  vars[13]     = {"M", "A23_1", "ph23_1", "A23_2", "ph23_2", "A31_1", "ph31_1", "A31_2", "ph31_2", "A12_1", "ph12_1", "A12_2", "ph12_2"};

// some constants
const Int_t ngbins = 140;		// number of points for phase space surronding graph
const Int_t lSlider = 140;		// width of amp and phase sliders
const Int_t lMSlider = 200;		

// Range of sqrt(s) in GeV
const Double_t M_min = 0.5;
const Double_t M_max = 7.5;

// Range for amplitude A and phase Ph
const Int_t A_min  = 0;		// min slider amplitude value
const Int_t A_max  = 1500;	// max slider amplitude value
const Int_t Ph_min = 0;	    // min slider phase value
const Int_t Ph_max = 628;   // max slider phase value
  
// some globals to exchange values between TDalitzGui and AniGidDialog
int      gAniGifVar   = 0;
int      gAniGifPlot  = 0;
Float_t  gAniGifMin   = 0;
Float_t  gAniGifMax   = 1;
Float_t  gAniGifSteps = 20; 
Float_t  gAniGifTime  = 20;
bool gAniGifOk = true;
TString gAniFileName="DalitzGui_ani.gif";


//___________________________________________________________________
// The DalitzGui constructor

TDalitzGui::TDalitzGui(const TGWindow *p,UInt_t w,UInt_t h)
{
	int i,j;

  	fMain = new TGMainFrame(p,w,h); 
  	fMain->Move(30,20);
  	fMain->SetWMPosition(30,20); 

	fMain->Connect("CloseWindow()","TApplication",gApplication,"Terminate()");
    
	fMain->SetCleanup(kDeepCleanup);
	InitParams();	

	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameFillColor(0);

	// Menu bar
    fMenuDock = new TGDockableFrame(fMain);
    fMain->AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
	fMenuFile = new TGPopupMenu(gClient->GetRoot());
	fMenuFile->AddEntry("&Open ...", M_FILE_OPEN);
	fMenuFile->AddEntry("&Save as...", M_FILE_SAVE);
	fMenuFile->AddEntry("Save &canvas as...", M_FILE_SAVE_SCRN);
	fMenuFile->AddEntry("Create animated &GIF", M_ANIGIF);
	fMenuFile->AddSeparator();
    fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
	
	fMenuFile->Connect("Activated(Int_t)", "TDalitzGui", this, "HandleMenu(Int_t)");

	fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
    fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
	fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
	
	fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
	fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);
   
	// Create an embedded canvas and add to the main frame, centered in x and y
	// and with 30 pixel margins all around
	fCanvas = new TRootEmbeddedCanvas("Canvas", fMain, 1000, 500);
	fLcan = new TGLayoutHints(kLHintsCenterX|kLHintsCenterY|kLHintsExpandX|kLHintsExpandY,5,5,1,5);

	fCanvas->GetCanvas()->SetFillColor(0);
	fCanvas->GetCanvas()->SetFrameFillColor(0);
	fCanvas->GetCanvas()->SetBorderMode(0);
	
	fCanvas->GetCanvas()->Divide(2,1);
 	fCanvas->GetCanvas()->GetPad(2)->Divide(2,2);
 	fCanvas->GetCanvas()->GetPad(1)->SetRightMargin(0.105);
	
	gStyle->SetLabelSize(0.03,"Z");
	//gStyle->SetTitleOffset(0.12,"X");
	//gStyle->SetTitleOffset(0.12,"Y");

	// **************** The first row
	fVframe[0] = new TGHorizontalFrame(fMain, 0, 0, 0);
	
	// Label for M (mother mass)
	fLab[0] = new TGLabel(fVframe[0], "M");
	
	// Text input for M
	fTeh1 = new TGTextEntry(fVframe[0], fTbh1 = new TGTextBuffer(8), HId1);
	char tmp[20];
	sprintf(tmp,"%5.2f",fM);
	fTbh1->AddText(0, tmp);
   	fTeh1->Connect("ReturnPressed()", "TDalitzGui", this, "DoText()");
   	fTeh1->Resize(50);
	
	// resize button for kinmatic limits
	fCheckResize = new TGCheckButton(fVframe[0],"Auto",HChId1);
	fCheckResize->SetState(kButtonDown);
    fCheckResize->Connect("Clicked()", "TDalitzGui", this, "DoSlider(Int_t)");

	// slider for choosing M
	fHslider1 = new TGHSlider(fVframe[0],lMSlider);
	fHslider1->SetRange(Int_t(M_min*100),Int_t(M_max*100));
	fHslider1->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoSlider(Int_t)");
	fHslider1->SetPosition((Int_t)(fM*100.));
	fHslider1->Resize(140,20);
	fHslider1->SetScale(20);
		
	// combo box for mass m1-m3
	for (i=0;i<3;++i)
	{
		fLab[i+1] = new TGLabel(fVframe[0], TString::Format("m%d",i+1));
		fComFin[i] = new TGComboBox(fVframe[0],HComId1+i);
		ConfigComboFin(fComFin[i]);		
	}
	
	// Text input for bins
	fLab[4] = new TGLabel(fVframe[0], "Bins");
	
	fTeh2 = new TGTextEntry(fVframe[0], fTbh2 = new TGTextBuffer(10), HSlBins);
	sprintf(tmp,"%d",ndbins);
	fTbh2->AddText(0, tmp);
   	fTeh2->Connect("ReturnPressed()", "TDalitzGui", this, "Rebin()");
	fTeh2->Resize(40);
   	
   	// check box for color
	fCheckColor = new TGCheckButton(fVframe[0],"Color",HChId1);
	fCheckColor->SetState(kButtonUp);
    fCheckColor->Connect("Clicked()", "TDalitzGui", this, "SwitchColor()");

   	// check box for log scale
	fCheckLog = new TGCheckButton(fVframe[0],"Log",HChId3);
	fCheckLog->SetState(kButtonUp);
    fCheckLog->Connect("Clicked()", "TDalitzGui", this, "DrawPlot()");

	// combo box for choice of large plot
	fLab[5] = new TGLabel(fVframe[0], "Plot");
	fComPlot = new TGComboBox(fVframe[0],HComPlot);
	fComPlot->AddEntry("Dalitz",0);
	fComPlot->AddEntry("Dalitz (scat)",10);
	fComPlot->AddEntry("Dalitz In",1);
	fComPlot->AddEntry("Dalitz Diff",2);
	fComPlot->AddEntry("Dalitz Ph",3);
	fComPlot->AddEntry("Proj s23",4);
	fComPlot->AddEntry("Proj s31",5);
	fComPlot->AddEntry("Proj s12",6);
	fComPlot->AddEntry("Mass m23",7);
	fComPlot->AddEntry("Mass m31",8);
	fComPlot->AddEntry("Mass m12",9);
	fComPlot->Select(0);
	fComPlot->Connect("Selected(Int_t)","TDalitzGui", this, "DoCombo(Int_t)");
	fComPlot->Resize(100,20);
	
	// Label for occupancy slider
	fLab[6] = new TGLabel(fVframe[0], "Occ");
	// slider for choosing occupancy of random generated plots
	fSlOcc = new TGHSlider(fVframe[0],70);
	fSlOcc->SetRange(10,1000);
	fSlOcc->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoParmSlider(Int_t)");
	fSlOcc->SetPosition((Int_t)(200.));
	fSlOcc->Resize(80,20);
	fSlOcc->SetScale(8);

	// ****************
	// the resonance setup GUI
	
	for (i=1;i<4;++i) fVframe[i] = new TGHorizontalFrame(fMain, 0, 0, 0);

	fLabRN[0] = new TGLabel(fVframe[1],"R23");
	fLabRN[1] = new TGLabel(fVframe[2],"R31");
	fLabRN[2] = new TGLabel(fVframe[3],"R12");

	for (i=0;i<6;++i)
	{
		fLabRA[i]  = new TGLabel(fVframe[i/2+1],"A");
		fLabRph[i] = new TGLabel(fVframe[i/2+1],"ph");
		
		fSlResA[i]  = new TGHSlider(fVframe[i/2+1],lSlider,kSlider1 | kScaleBoth, HSlRAId1 + i);
		fSlResph[i] = new TGHSlider(fVframe[i/2+1],lSlider,kSlider1 | kScaleBoth, HSlRpId1 + i);
		ConfigSliderRes(fSlResA[i], fSlResph[i]);

		fComRes[i] =  new TGComboBox(fVframe[i/2+1],HComRId1 + i); 
		ConfigComboRes(fComRes[i]);
		
		fChRes[i] = new TGCheckButton(fVframe[i/2+1],"On",HChRId1 + i);
		if (i%2) 
			fChRes[i]->SetState(kButtonUp);
		else 
			fChRes[i]->SetState(kButtonDown);

		fChRes[i]->Connect("Clicked()", "TDalitzGui", this, "DoParmSlider(Int_t)");
	}
			
	//--- layout for buttons: top align, equallyDoParmSlider expand horizontally
	fBly = new TGLayoutHints(kLHintsTop | kLHintsCenterX , 5, 5, 5, 5);
	
	//--- layout for the frame: place at bottom, right aligned
	//fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5);
	fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsLeft,    4, 4, 4, 4);
	fBfly2 = new TGLayoutHints(kLHintsTop | kLHintsLeft,   3, 3, 4, 4);
	fBfly3 = new TGLayoutHints(kLHintsTop | kLHintsRight,   5, 5, 5, 5);
	
	fVframe[0]->AddFrame(fLab[0], fBfly2);         // Label 'M'
	fVframe[0]->AddFrame(fTeh1, fBfly2);         // Textinput for M
	fVframe[0]->AddFrame(fHslider1, fBfly2);     // Slider for M
	
	fVframe[0]->AddFrame(fCheckResize, fBfly2);  // Checkbox 'Auto'
	
	fVframe[0]->AddFrame(fLab[1], fBfly2);         // Label 'm1'
	fVframe[0]->AddFrame(fComFin[0], fBfly2);      // ComboBox m1
	fVframe[0]->AddFrame(fLab[2], fBfly2);         // Label 'm2'
	fVframe[0]->AddFrame(fComFin[1], fBfly2);      // ComboBox m2
	fVframe[0]->AddFrame(fLab[3], fBfly2);         // Label 'm3'
	fVframe[0]->AddFrame(fComFin[2], fBfly2);      // ComboBox m3
	
	fVframe[0]->AddFrame(fLab[5],fBfly2);          // Label 'Plot'
	fVframe[0]->AddFrame(fComPlot, fBfly2);      // ComboBox Plot

	fVframe[0]->AddFrame(fLab[4], fBfly2);         // Label 'Bins'
	fVframe[0]->AddFrame(fTeh2, fBfly2);         // Textinput for Bins
	
	fVframe[0]->AddFrame(fCheckColor, fBfly2);   // Checkbox 'Color'

	fVframe[0]->AddFrame(fCheckLog, fBfly2);   // Checkbox 'Log'
	
	fVframe[0]->AddFrame(fLab[6], fBfly2);         // Label 'Occ'
	fVframe[0]->AddFrame(fSlOcc, fBfly2);        // Slider for Occ
	
	
	// *** The Resonance Parameters *******************
	fVframe[1]->AddFrame(fLabRN[0], fBfly1);
	fVframe[2]->AddFrame(fLabRN[1], fBfly1);
	fVframe[3]->AddFrame(fLabRN[2], fBfly1);
	
	for (i=0;i<6;++i)
	{
		fVframe[i/2+1]->AddFrame(fComRes[i],fBfly1);
		fVframe[i/2+1]->AddFrame(fChRes[i], fBfly1);
		fVframe[i/2+1]->AddFrame(fLabRA[i], fBfly1);
		fVframe[i/2+1]->AddFrame(fSlResA[i], fBfly1);
		fVframe[i/2+1]->AddFrame(fLabRph[i], fBfly1);
		fVframe[i/2+1]->AddFrame(fSlResph[i], fBfly1);
	}
	
	//********************
	
	fMain->AddFrame(fCanvas, fLcan);
	fMain->AddFrame(fVframe[0], fBfly2);
	fMain->AddFrame(fVframe[1], fBfly2);
	fMain->AddFrame(fVframe[2], fBfly2);
	fMain->AddFrame(fVframe[3], fBfly2);
	
	fMain->SetWindowName("Dalitz GUI");
	fMain->MapSubwindows();
	fMain->Resize(fMain->GetDefaultSize());
	fMain->MapWindow();
	
	// prepare the plots etc
	
	fHDalitz = 0;
	fHDalitzSct = 0;
	fHDalitzI = 0;
	fHDalitzD = 0;
	fHDalitzPh = 0;
	
	fHs1Proj = 0;
	fHs2Proj = 0;
	fHs3Proj = 0; 

	fHs1M = 0;
	fHs2M = 0;
	fHs3M = 0; 
	
	fOcc = 200;

	// init gui
	
	fGraphKin = new TGraph(ngbins*2+1);
	fGraphKin->SetLineColor(2);

	for (i=0;i<3;++i) fComFin[i]->Select(1, false);
	for (i=0;i<6;++i) fComRes[i]->Select(0,false);

	ReadOutGui();	

	CreateGraph(fGraphKin, ngbins, fM,fm1,fm2,fm3);
	
	DoResize();
	DrawPlot();
}

//___________________________________________________________________

void TDalitzGui::Rebin()
{
	for (int i=0;i<6;++i) fRes[i]->SetCacheValid(kFALSE);
	DoResize();
	DrawPlot();
}
//___________________________________________________________________

void TDalitzGui::HandleMenu(Int_t id)
{
   // Handle menu items.

   switch (id) {

      case M_FILE_OPEN:
         {
            static TString dir(".");
            TGFileInfo fi;
            fi.fFileTypes = filetypes;
            fi.fIniDir    = StrDup(dir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
			LoadConfiguration(fi.fFilename);
            dir = fi.fIniDir;
         }
         break;

      case M_FILE_SAVE:
	      {
            static TString dir(".");
            TGFileInfo fi;
            fi.fFileTypes = filetypes;
            fi.fIniDir    = StrDup(dir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
			SaveConfiguration(fi.fFilename);
            dir = fi.fIniDir;
	      }
          break;
          
      case M_FILE_SAVE_SCRN:
	      {
            static TString dir(".");
            TGFileInfo fi;
            fi.fFileTypes = filetypesscrn;
            fi.fIniDir    = StrDup(dir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
            //TString fnm(fi.fFilename);
            //TString ending(fnm(fnm.Length()-3,3));
			//if (ending!="pdf" && ending!="gif") fnm+=".gif";
			if (fi.fFilename)
				fCanvas->GetCanvas()->SaveAs(fi.fFilename);
            dir = fi.fIniDir;
	      }
          break;
          
      case M_ANIGIF:
      	  gAniGifOk=false;
      	  new AniGifDialog(gClient->GetRoot(), fMain);
      	  if (gAniGifOk) MakeAnimatedGif();
          break;
  
	  case M_FILE_EXIT:
		 {
			CloseWindow();   // terminate theApp no need to use SendCloseMessage()
			break;
		 }
   }
}
//___________________________________________________________________

Int_t TDalitzGui::IdToSelection(Int_t id, int mode ) //  FS: mode=0, RES: mode=1
{
	Int_t sel = -1;
	if (mode==0)
	{
		for (int i=0;i<N_FIN;++i) if (id_fin[i]==id) sel=i;
	}
	else if (mode==1)
	{
		for (int i=0;i<N_RES;++i) if (id_res[i]==id) sel=i;
	}	
	//cout <<"mode "<<mode<<" id "<<id<<" sel "<<sel<<endl;
	return sel;
}

//___________________________________________________________________
// Load configuration from file and set interface

void TDalitzGui::LoadConfiguration(TString filename)
{
	int i;

	if (filename=="") return;
	
	ifstream file(filename.Data());
	if (!file.is_open()) return;
	
	fMain->BlockAllSignals(true);

	double dumf;
	int dumi;
	
	// total energy
	file >> fM;
	char tmp[20];
	sprintf(tmp,"%5.2f",fM);
	fTeh1->SetText(tmp);
	Int_t pos = Int_t(atof(tmp)*100);
	fHslider1->SetPosition(pos);
	
	// resonance state (on/off)
	for (i=0;i<6;++i)
	{
		file >> dumi; 
		fChRes[i]->SetState((EButtonState)dumi);
	}
	
	// resonance types and parameters
	for (i=0;i<6;++i)
	{
		file >> dumi;
		fComRes[i]->Select(dumi);	
		file >> dumf;
		fSlResA[i]->SetPosition(dumf);
		file >> dumf;
		fSlResph[i]->SetPosition(dumf);
	}
	
	// select final states
	for (i=0;i<3;++i)
	{
		file >> dumi;
		fComFin[i]->Select(dumi);
	}
	
	// number of bins
	file >> dumi;
	if (dumi>MAX_BIN) dumi=MAX_BIN;
	sprintf(tmp,"%d",dumi);
	fTeh2->SetText(tmp);
	
	// color state
	file >> dumi; 
	fCheckColor->SetState((EButtonState)dumi);
	
	// selected plot to draw
	file >> dumi;
	fComPlot->Select(dumi);
	
	// auto state 
	file >> dumi;
	fCheckResize->SetState((EButtonState)dumi);
	
	// log scale state 
	file >> dumi;
	fCheckLog->SetState((EButtonState)dumi);
	
	// occupancy slider position
	file >> dumf;
	fSlOcc->SetPosition(dumf);
	
	fMain->BlockAllSignals(false);
	
	// color
	if (fComPlot->GetSelected()==3) return;
	palette2(fCheckColor->GetState());

	ReadOutGui();    // read out the interface settings
	DoSlider(0);
}

//___________________________________________________________________
// Save configuration to file

void TDalitzGui::SaveConfiguration(TString filename)
{
	int i;

	if (filename=="") return;
	
	if (!filename.Contains(".")) filename+=".cfg";
	
	ofstream file;
	file.open(filename.Data());
	file.precision(5);
	 
	// save total energy
	file << fM << endl;
	
	// save resonance on/off switch states
	for (i=0;i<6;++i) file << fChRes[i]->GetState() << endl;
	
	// save resonance types and parameters
	for (i=0;i<6;++i)
	{
		file << fComRes[i]->GetSelectedEntry()->EntryId() << endl;
		file << fSlResA[i]->GetPosition() << endl;
		file << fSlResph[i]->GetPosition() << endl;
	}

	// save final state particles
	for (i=0;i<3;++i) file << fComFin[i]->GetSelectedEntry()->EntryId() << endl;

	//
	// save number of bins
	file <<  Int_t(atoi(fTbh2->GetString())) << endl;

	// color state (bw/color)
	file << fCheckColor->GetState() << endl;
	
	// save selected plot to draw
	file << fComPlot->GetSelected() << endl;
	
	// auto state 
	file << fCheckResize->GetState() << endl;
	
	// log scale state 
	file << fCheckLog->GetState() << endl;
	
	// occupancy slider position
	file << fSlOcc->GetPosition() << endl;
	
	file.close();
}

//___________________________________________________________________
// Set combo box for final states   
void TDalitzGui::ConfigComboFin(TGComboBox* b)
{
	for (int i=0;i<N_FIN;i++)
		b->AddEntry(nam_fin[i].Data(),id_fin[i]);

	b->Connect("Selected(Int_t)","TDalitzGui", this, "DoCombo(Int_t)");
    b->Resize(55, 20);
}

//___________________________________________________________________
// Set combo box for resonances
void TDalitzGui::ConfigComboRes(TGComboBox* b)
{
	for (int i=0;i<N_RES;++i) b->AddEntry(name_res[i],id_res[i]);

	b->Connect("Selected(Int_t)","TDalitzGui", this, "DoCombo(Int_t)");
    b->Resize(90, 20);
}

//___________________________________________________________________
// Set slider for amplitude and phase    
void TDalitzGui::ConfigSliderRes(TGHSlider *a, TGHSlider *ph)
{
	ph->SetRange(Ph_min,Ph_max);
	ph->SetPosition(0);
	ph->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoParmSlider(Int_t)");
	//ph->Connect("Released()", "TDalitzGui", this, "DrawAllPlot()");
	ph->SetScale(11);
	ph->Resize(110,20);
	
	a->SetRange(A_min,A_max);
	a->SetPosition(500);
	a->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoParmSlider(Int_t)");
	//a->Connect("Released()", "TDalitzGui", this, "DrawAllPlot()");
	a->SetScale(14);
	a->Resize(140,20);
}

//___________________________________________________________________
// Draw all plots
void TDalitzGui::DrawPlot()
{   	
	Bool_t logscale = fCheckLog->GetState();
	
   	fCanvas->GetCanvas()->GetPad(2)->cd(1)->SetLogy(logscale);
	fHs1Proj->Draw();
	
   	fCanvas->GetCanvas()->GetPad(2)->cd(2)->SetLogy(logscale);
	fHs2Proj->Draw();
	
   	fCanvas->GetCanvas()->GetPad(2)->cd(3)->SetLogy(logscale);
	fHs3Proj->Draw();

   	fCanvas->GetCanvas()->GetPad(2)->cd(4)->SetLogz(logscale);
	fHDalitzI->Draw("colz");
	fGraphKin->Draw("same L");

	DrawMainPlot();

	fCanvas->GetCanvas()->cd(1);	
   	fCanvas->GetCanvas()->Update();
}
//___________________________________________________________________
// Draw all plots
void TDalitzGui::DrawMainPlot()
{   	
	Bool_t logscale = fCheckLog->GetState();

	int plotidx = fComPlot->GetSelected();
	TPad *pad = (TPad*) fCanvas->GetCanvas()->cd(1);
	pad->SetLogy(0);
	pad->SetLogz(0);
   	
   	if (plotidx<3) pad->SetLogz(logscale);    	
    if (plotidx>3 && plotidx<10) pad->SetLogy(logscale);    	
   	    	
	switch (plotidx)
	{
	case 1: fHDalitzI->Draw("colz");
			fGraphKin->Draw("same L");
			break;
			
	case 2: fHDalitzD->Draw("colz");
			fGraphKin->Draw("same L");
			break;
			
	case 3: fHDalitzPh->Draw("colz");
			fGraphKin->Draw("same L");
			break;
			
	case 4: fHs1Proj->Draw();
			break;
			
	case 5: fHs2Proj->Draw();
			break;
			
	case 6: fHs3Proj->Draw();
			break;
			
	case 7: fHs1M->Draw();
			break;
			
	case 8: fHs2M->Draw();
			break;
			
	case 9: fHs3M->Draw();
			break;
			
	case 10: fHDalitzSct->Draw("scat");
			break;
			
	default: fHDalitz->Draw("colz");
			 fGraphKin->Draw("same L");
			 break;
	}

	fCanvas->GetCanvas()->cd(0);	
   	fCanvas->GetCanvas()->Update();

}
//___________________________________________________________________

void TDalitzGui::InitParams()
{
	// Read Final States and Resonances from 'DalitzGUI.ini'
	ifstream file("DalitzGUI.ini");
	if (!file.is_open()) 
	{
		cout <<"DalitzGUI [FATAL]: Unable to open DalitzGUI.ini"<<endl;
		exit(0);
	}
	else cout <<"Reading 'DalitzGUI.ini' ... "<<endl;
	
	char line[500];
	
	N_RES = 0;
	N_FIN = 0;
	
	TString toks[10];

	while (!file.eof())
	{
		file.getline(line,499);
		TString sline(line);
		if (sline.BeginsWith("#")||sline=="") continue;		
		
		// split the line into tokens
		TObjArray *tok = sline.Tokenize(" ");
		int N=tok->GetEntries();	
		// and convert to normal TStrings
		for (int i=0;i<N;++i) if (i<10) toks[i] = ((TObjString*)tok->At(i))->String();
		
		// Read in a final state particle definition
		if (toks[0] == "FS" && N==5)
		{
			id_fin[N_FIN]   = toks[1].Atoi();
			nam_fin[N_FIN]  = toks[2]; 
			nam_pfin[N_FIN] = toks[3]; 
			m_fin[N_FIN]    = toks[4].Atof();
			
			cout << "[Final State] ID "<<id_fin[N_FIN]<<" : "<<nam_fin[N_FIN]<<" ("<<nam_pfin[N_FIN]<<"), m = "<<m_fin[N_FIN]<<endl;
			
			if (IdToSelection(id_fin[N_FIN],0)>0) 
			{
				cout <<"*** WARNING: Final state with ID "<<id_fin[N_FIN]<<" already exists! Skipping entry."<<endl;
				continue;
			}
			
			N_FIN++;
		}

		// Read in a resonance definition
		if (toks[0] == "RES")
		{
			id_res[N_RES]   = toks[1].Atoi();
			name_res[N_RES] = toks[2]; 
			m_res[N_RES]    = toks[3].Atof(); 
			G_res[N_RES]    = toks[4].Atof();
			J_res[N_RES]    = toks[5].Atoi();
			
			cout << "[Resonance  ] ID "<<id_res[N_RES]<<" : "<<name_res[N_RES]<<", m0 / G0 / J = "<<m_res[N_RES]<<" / "<<G_res[N_RES]<<" / "<<J_res[N_RES]<<endl;

			if (IdToSelection(id_res[N_RES],1)>0) 
			{
				cout <<"*** WARNING: Resonance with ID "<<id_res[N_RES]<<" already exists! Skipping entry."<<endl;
				continue;
			}
			
			N_RES++;
		}	
	}
	
	cout <<"Found "<<N_FIN<<" final states and "<<N_RES<<" resonances in total."<<endl;
		
	// Init other params
	palette2(0);
	
	fHs1Proj=0;
	fHs2Proj=0;
	
	for (int i=0;i<6;++i) 
	{
		fRes[i] = new CRes();
		fRes[i]->SetCacheLen(MAX_BIN*MAX_BIN);
	}
	
	fM = 1.97;
	
	fm1 = m_res[2];
	
	fnR[0] = 1;
	fnR[1] = 1;
	fnR[2] = 1;
	
	ndbins = 100;
		
}

//___________________________________________________________________
// create the graph marking the phase space   
void TDalitzGui::CreateGraph(TGraph* g, Int_t n, Double_t m, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s1minl = simin(1, m, m1, m2, m3);
	Double_t s1maxl = simax(1, m, m1, m2, m3);
	
	Double_t binw = (s1maxl-s1minl)/(Double_t)(n);
	
	Int_t i=0;
	
	Double_t ymin,ymax,x;
	
	for (i=0; i<n; i++)
	{
		x = i*binw + s1minl;
		if (x<=s1minl) x=s1minl+binw/20;
		if (x>=s1maxl) x=s1maxl-binw/20;
		
		ymin = s2min(x, m, m1, m2, m3);
		ymax = s2max(x, m, m1, m2, m3);
		
		g->SetPoint(i,x,ymin);
		g->SetPoint(n*2-i,x,ymax);
	}

	g->SetPoint(n,s1maxl,s2min(s1maxl-binw/20,m,m1,m2,m3));
	g->GetPoint(0,x,ymin);
	g->SetPoint(n*2,x,ymin);
	
}

//___________________________________________________________________
// configure all 2D histograms
void TDalitzGui::ConfigPlot()
{
	fHDalitz->SetStats(0);
	fHDalitz->SetTitle("Dalitzplot: "+fn1+" "+fn2+" "+fn3);
	fHDalitz->GetXaxis()->SetTitleOffset(1.2);
	fHDalitz->GetYaxis()->SetTitleOffset(1.3);
	fHDalitz->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHDalitz->SetYTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");
    
	fHDalitzSct->SetStats(0);
	fHDalitzSct->SetTitle("Dalitzplot: "+fn1+" "+fn2+" "+fn3);
	fHDalitzSct->GetXaxis()->SetTitleOffset(1.2);
	fHDalitzSct->GetYaxis()->SetTitleOffset(1.3);
	fHDalitzSct->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHDalitzSct->SetYTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");
    
    fHDalitzI->SetStats(0);
	fHDalitzI->SetTitle("Dalitzplot incoherent: "+fn1+" "+fn2+" "+fn3);
	fHDalitzI->GetXaxis()->SetTitleOffset(1.2);
	fHDalitzI->GetYaxis()->SetTitleOffset(1.3);
	fHDalitzI->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHDalitzI->SetYTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");

    fHDalitzD->SetStats(0);
	fHDalitzD->SetTitle("Difference coherent-incoherent: "+fn1+" "+fn2+" "+fn3);
	fHDalitzD->GetXaxis()->SetTitleOffset(1.2);
	fHDalitzD->GetYaxis()->SetTitleOffset(1.3);
	fHDalitzD->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHDalitzD->SetYTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");
	
    fHDalitzPh->SetStats(0);
	fHDalitzPh->SetTitle("Dalitzplot phase: "+fn1+" "+fn2+" "+fn3);
	fHDalitzPh->GetXaxis()->SetTitleOffset(1.2);
	fHDalitzPh->GetYaxis()->SetTitleOffset(1.3);
	fHDalitzPh->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHDalitzPh->SetYTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");

	fHDalitz->SetContour(99);
	fHDalitzI->SetContour(99);
	fHDalitzD->SetContour(99);
	fHDalitzPh->SetContour(99);
}

//___________________________________________________________________
// Compute all plots (the main routine)
void TDalitzGui::CreateDalitz()
{
 	fHDalitz->Reset();
 	fHDalitz->SetContour(99);
 	fHDalitz->SetStats(0);
 
 	fHDalitzSct->Reset();
		
 	fHDalitzI->Reset();
 	fHDalitzI->SetContour(99);
 	fHDalitzI->SetStats(0);
	
 	fHDalitzD->Reset();
 	fHDalitzD->SetContour(99);
 	fHDalitzD->SetStats(0);

 	fHDalitzPh->Reset();
 	fHDalitzPh->SetContour(99);
 	fHDalitzPh->SetStats(0);
 	fHDalitzPh->SetMaximum(3.1416);
 	fHDalitzPh->SetMinimum(-3.1416);
	
	fHs1Proj->Reset();
	fHs2Proj->Reset();
	fHs3Proj->Reset();
	
	fHs1M->Reset();
	fHs2M->Reset();
	fHs3M->Reset();
	
	Int_t i,j,nx,ny;
	
	Double_t s = fM*fM;
	
	Int_t nRes = fnR[0] + fnR[1] + fnR[2];
	
	if (nRes==2) fHDalitzPh->SetTitle("Relative phase of "+fActiveRes);
	else fHDalitzPh->SetTitle("Dalitzplot total phase: "+fn1+" "+fn2+" "+fn3);
	
	Double_t sum_kin = s + fm1*fm1 + fm2*fm2 + fm3*fm3;
	
	Double_t s1minl = simin(1, fM, fm1, fm2, fm3);
	Double_t s1maxl = simax(1, fM, fm1, fm2, fm3);
		
	nx = fHDalitz->GetNbinsX();
	ny = fHDalitz->GetNbinsY();
	
	Double_t binwx = fHDalitz->GetXaxis()->GetBinWidth(1); 
	Double_t binwy = fHDalitz->GetYaxis()->GetBinWidth(1); 
	Double_t binwz = sqrt(binwx*binwx+binwy*binwy);
		
	for (i=1;i<nx+1;++i)
	{
		double s1 = fHDalitz->GetXaxis()->GetBinCenter(i);
		
		if (s1<s1minl || s1>s1maxl) continue;
	
		Double_t cs2min=s2min(s1, fM, fm1, fm2, fm3);
		Double_t cs2max=s2max(s1, fM, fm1, fm2, fm3);
	
		for (j=1;j<ny+1;++j)
		{
			double s2 = fHDalitz->GetYaxis()->GetBinCenter(j);
			
			if (s2<cs2min || s2>cs2max) continue;
			
			Double_t cs1min=s1min(s2, fM, fm1, fm2, fm3);
			Double_t cs1max=s1max(s2, fM, fm1, fm2, fm3);
												
			double s3 = sum_kin - s1 - s2;

			Double_t cs3min=s1min(s3, fM, fm1, fm3, fm2);
			Double_t cs3max=s1max(s3, fM, fm1, fm3, fm2);

			TComplex Atot1(0,0), Atmp(0,0);
			Double_t A_in=0.0, Adiff=0.;
			
			Double_t qR1 = sqrt(lambda(s,s1,fm1*fm1)/s1)/2.;
			Double_t qR2 = sqrt(lambda(s,s2,fm2*fm2)/s2)/2.;
			Double_t qR3 = sqrt(lambda(s,s3,fm3*fm3)/s3)/2.;
			
			// alternative summing
			Double_t qM23 =  breakup(sqrt(s1),fm2,fm3);
			Double_t qM13 =  breakup(sqrt(s2),fm1,fm3);
			Double_t qM12 =  breakup(sqrt(s3),fm1,fm2);

			int cidx = i + MAX_BIN*j;

			for (int k=0;k<6;++k)
			{
				double p0, p1, p2, p3, p4, p5, p6, p7, p8;
				switch (k/2) 
				{
				case 0: p0 = sqrt(s1); p1 = fm2; p2 = fm3; p3 = qR1; p4 = qM23; p5 = s2; p6 = cs2min; p7 = cs2max;
					break;
				case 1: p0 = sqrt(s2); p1 = fm1; p2 = fm3; p3 = qR2; p4 = qM13; p5 = s1; p6 = cs1min; p7 = cs1max;
					break;
				case 2: p0 = sqrt(s3); p1 = fm1; p2 = fm2; p3 = qR3; p4 = qM12; p5 = s1; p6 = cs3min; p7 = cs3max;
					break;
				}
				//switch (k/2) 
				//{
				//case 0: p0 = s1; p1 = s2; p2 = s3; p3 = fm2; p4 = fm3; p5 = fm1; p6 = s2; p7 = cs2min; p8 = cs2max; 
				//	break;
				//case 1: p0 = s2; p1 = s1; p2 = s3; p3 = fm1; p4 = fm3; p5 = fm2; p6 = s1; p7 = cs1min; p8 = cs1max; 
				//	break;
				//case 2: p0 = s3; p1 = s2; p2 = s3; p3 = fm1; p4 = fm2; p5 = fm1; p6 = s1; p7 = cs3min; p8 = cs3max;
				//	break;
				//}

				if (fRes[k]->GetState()) 
				{	
					if (fRes[k]->IsCacheValid()) 
						Atmp = fRes[k]->GetCache(cidx);
					else
					{
						Atmp = getAmp(fRes[k], p0, p1, p2, p3, p4) * (Z_Ralt(p5, p6, p7, fRes[k]->GetJ(), fRes[k]->GetM0(), p0, p2) + 1e-10);
						//Atmp = getAmpEvt(fRes[k], p0, p1, p2, p3, p4, p5) * (Z_Ralt(p6, p7, p8, fRes[k]->GetJ()) + 1e-10);
						fRes[k]->SetCache(cidx,Atmp);
					}
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}
			}
						
			double In2 = Atot1.Rho2();
			
			if (In2!=In2) In2=0.;
				
			if (Adiff>3.1416) Adiff-=2*3.1415926;
			if (Adiff<-3.1416) Adiff+=2*3.1415926;

			fHDalitz->SetBinContent(i,j,In2);
			fHDalitzI->SetBinContent(i,j,A_in*A_in);
			fHDalitzD->SetBinContent(i,j,A_in*A_in-In2);
			if (nRes==2) 
				fHDalitzPh->SetBinContent(i,j,fabs(Adiff));
			else 
				fHDalitzPh->SetBinContent(i,j,Atot1.Theta());

			fHs1Proj->Fill(s1, In2);
			fHs2Proj->Fill(s2, In2);
						
		}
	}
	
	for (i=0;i<6;++i) fRes[i]->SetCacheValid(kTRUE);

	// if Dalitz plot not empty, create all random generated plots from distribution
	if (fHDalitz->GetSumOfWeights()>0)
	{
		double rs1,rs2;
		for (int jj=0;jj<ndbins*fOcc;++jj)
		{
			fHDalitz->GetRandom2(rs1,rs2);
			double rs3 = sum_kin - rs1 - rs2;

			fHs3Proj->Fill(rs3);
			
			fHs1M->Fill(sqrt(rs1));
			fHs2M->Fill(sqrt(rs2));
			fHs3M->Fill(sqrt(rs3));

			fHDalitzSct->Fill(rs1, rs2);

		}

		fHs1Proj->Scale(1./fHs1Proj->GetMaximum());
		fHs2Proj->Scale(1./fHs2Proj->GetMaximum());
		
		fHs1M->Scale(1./fHs1M->GetMaximum());
		fHs2M->Scale(1./fHs2M->GetMaximum());
		
		fHDalitz->Scale(1./fHDalitz->GetMaximum());
		fHDalitzI->Scale(1./fHDalitzI->GetMaximum());
		fHDalitzD->Scale(1./fHDalitzD->GetMaximum());
	}
	
	// when not auto resize, these two plots might be empty
	if (fHs3Proj->GetMaximum()>0) fHs3Proj->Scale(1./fHs3Proj->GetMaximum());		
	if (fHs3M->GetMaximum()>0) fHs3M->Scale(1./fHs3M->GetMaximum());
	
	// some slight smoothing on the 1D plots
	fHs1Proj->Smooth(1);
	fHs2Proj->Smooth(1);
	fHs3Proj->Smooth(1);
	
	fHs1M->Smooth(1);
	fHs2M->Smooth(1);
	fHs3M->Smooth(1);
}

//___________________________________________________________________
// handles text input for M
void TDalitzGui::DoText()
{
	// Handle text entry widgets.
	
	TGTextEntry *te = (TGTextEntry *) gTQSender;
	Int_t id = te->WidgetId();
	
	Int_t pos = Int_t(atof(fTbh1->GetString())*100);
	
	if (pos<M_min*100) pos=M_min*100;
	if (pos>M_max*100) pos=M_max*100;
	
	switch (id) {
		case HId1:
			fHslider1->SetPosition(pos);
			break;
		default:
			break;
	}
	
	DoSlider(pos);
}

//___________________________________________________________________
// handles the slider for M 
void TDalitzGui::DoSlider(Int_t pos)
{
   // Handle slider widgets.

	// Invalidate all caches
	for (int i=0;i<6;++i) fRes[i]->SetCacheValid(kFALSE);

   	char buf[32];

	//fM=(Float_t)pos/100.;
	fM = (Float_t)fHslider1->GetPosition()/100.;
	
   	sprintf(buf, "%.2f",fM );
   	fTbh1->Clear();
   	fTbh1->AddText(0, buf);
   	fTeh1->SetCursorPosition(4);
   	fTeh1->Deselect();
   
   	gClient->NeedRedraw(fTeh1);

	CreateGraph(fGraphKin, ngbins, fM, fm1, fm2, fm3);
	
	if (fCheckResize->GetState()) 
		DoResize();
	else 
		CreateDalitz();
		
	DrawPlot();
}

//______________________________________________________________________________
// Read out current settings of the interface
void TDalitzGui::ReadOutGui()
{
	fOcc = fSlOcc->GetPosition();
		
	fnR[0] = fnR[1] = fnR[2] = 0;
	//fnR1 = fnR2 = fnR3 = 0;
	
	fActiveRes = "";

	Double_t A=0., Ph=0., J=0.;
	Int_t resId;

	fm1=m_fin[IdToSelection(fComFin[0]->GetSelected(),0)];
	fm2=m_fin[IdToSelection(fComFin[1]->GetSelected(),0)];
	fm3=m_fin[IdToSelection(fComFin[2]->GetSelected(),0)];

	fn1=nam_pfin[IdToSelection(fComFin[0]->GetSelected(),0)];
	fn2=nam_pfin[IdToSelection(fComFin[1]->GetSelected(),0)];
	fn3=nam_pfin[IdToSelection(fComFin[2]->GetSelected(),0)];
	
	for (int i=0;i<6;++i) 
	{
		fRes[i]->SetState((Bool_t)fChRes[i]->GetState());
		
		if (fRes[i]->GetState())
		{
			double ffm1, ffm2;
			switch (i/2)
			{
			case 0: ffm1 = fm2; ffm2 = fm3; break;
			case 1: ffm1 = fm1; ffm2 = fm3; break;
			case 2: ffm1 = fm1; ffm2 = fm2; break;
			}
			A  = Double_t(fSlResA[i]->GetPosition())/100.;
			Ph = Double_t(fSlResph[i]->GetPosition())/100.;
			resId = IdToSelection(fComRes[i]->GetSelected());
			fRes[i]->Set(m_res[resId], G_res[resId], ffm1, ffm2, A, Ph, J_res[resId]);  
			fnR[i%2]++;
			fActiveRes+=name_res[resId]+", ";
		}

	}
	
	fActiveRes=fActiveRes(0,fActiveRes.Length()-2);
	
}
//______________________________________________________________________________
// action taking place when a resonance slider is moved
void TDalitzGui::DoParmSlider(Int_t pos)
{
	TGFrame *frm = (TGFrame *) gTQSender;
	TGHSlider *sl=0;
    if (frm->IsA()->InheritsFrom(TGHSlider::Class())) sl = (TGHSlider*) frm;

	Int_t id = -1;

	if (sl) id = sl->WidgetId();
	
	int residx = 0;

	if (id==-1) // no widget id? invalidate all caches
	{
		for (int i=0;i<6;++i) fRes[i]->SetCacheValid(kFALSE);
	}
	else // invalidate only one cache
	{
		if ((id-HSlRAId1)>=0 && (id-HSlRAId1)<6) residx = id-HSlRAId1;
		else if ((id-HSlRpId1)>=0 && (id-HSlRpId1)<6) residx = id-HSlRpId1;
		else if ((id-HChRId1)>=0 && (id-HChRId1)<6) residx = id-HChRId1;
		fRes[residx]->SetCacheValid(kFALSE);
	}

	ReadOutGui();    // read out the interface settings
	CreateDalitz();  // rebuild plots
	DrawPlot();      // draw plots 
}
//______________________________________________________________________________
// obsolete routine
void TDalitzGui::ComputeCache(Int_t ri)
{
	//CRes *r;

	//switch (ri)
	//{
	//case 1: r=fRes1[0];
	//	break;
	//case 2: r=fRes1[1];
	//	break;
	//case 3: r=fRes2[0];
	//	break;
	//case 4: r=fRes2[1];
	//	break;
	//default: r=0;
	//}

	//if (!r) return;

}

//______________________________________________________________________________
// Rebuild of all histograms when 
// a) the number of bins has changed
// b) M has changed

void TDalitzGui::DoResize()
{
	// clear all plots
	if (fHDalitz)  fHDalitz->Reset();
	if (fHDalitzSct)  fHDalitzSct->Reset();
	if (fHDalitzI)  fHDalitzI->Reset();
	if (fHDalitzD)  fHDalitzD->Reset();
	if (fHDalitzPh)  fHDalitzPh->Reset();

	if (fHs1Proj)  fHs1Proj->Reset(); //s1 Projection
	if (fHs2Proj)  fHs2Proj->Reset(); //s2 Projection
	if (fHs3Proj)  fHs3Proj->Reset(); //s3 Projection
	
	if (fHs1M)  fHs1M->Reset(); //s1 Mass
	if (fHs2M)  fHs2M->Reset(); //s2 Mass
	if (fHs3M)  fHs3M->Reset(); //s3 Mass
	
	// set number of bins
	ndbins = Int_t(atoi(fTbh2->GetString()));
	if (ndbins>MAX_BIN)
	{
		ndbins=MAX_BIN;
		fTeh2->SetText(TString::Format("%d",MAX_BIN));
	}

	// compute phase space borders
	Double_t s1minl = simin(1, fM, fm1, fm2, fm3);
	Double_t s2minl = simin(2, fM, fm1, fm2, fm3);
	Double_t s3minl = simin(3, fM, fm1, fm2, fm3);
			
	Double_t s1maxl = simax(1, fM, fm1, fm2, fm3);
	Double_t s2maxl = simax(2, fM, fm1, fm2, fm3);
	Double_t s3maxl = simax(3, fM, fm1, fm2, fm3);
	
	Double_t ds1 = s1maxl-s1minl, ds2 = s2maxl-s2minl, ds3 = s3maxl-s3minl;
	
	// if there is no phsp available don't do anything
	if (ds1<0 || ds2<0 || ds3<0 ) return;
	
	Double_t ds1m = ds1*plot_margin, ds2m = ds2*plot_margin, ds3m = ds3*plot_margin;
	
	Double_t hs1min = s1minl>ds1m ? s1minl-ds1m : 0.0;
	Double_t hs2min = s2minl>ds2m ? s2minl-ds2m : 0.0;
	Double_t hs3min = s3minl>ds3m ? s3minl-ds3m : 0.0;
	
	// recreate histograms
	if (fHDalitz) delete fHDalitz;
	if (fHDalitzSct) delete fHDalitzSct;
	if (fHDalitzI) delete fHDalitzI;
	if (fHDalitzD) delete fHDalitzD;
	if (fHDalitzPh) delete fHDalitzPh;

	if (fHs1Proj) delete fHs1Proj; //s1 Projection
	if (fHs2Proj) delete fHs2Proj; //s2 Projection
	if (fHs3Proj) delete fHs3Proj; //s3 Projection
	
	if (fHs1M) delete fHs1M; //s1 Mass
	if (fHs2M) delete fHs2M; //s2 Mass
	if (fHs3M) delete fHs3M; //s3 Mass
	
	// Plots have always 0 as lower limit		
	//fHDalitz   = new TH2F("fHDalitz","Dalitz Plot", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	//fHDalitzSct= new TH2F("fHDalitzSct","Dalitz Plot (scat)", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	//fHDalitzI  = new TH2F("fHDalitzI","Dalitz Plot (incoherent)", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	//fHDalitzD  = new TH2F("fHDalitzD","Difference coherent-incoherent", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	//fHDalitzPh = new TH2F("fHDalitzPh","Dalitz Plot (total phase)", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);

	//fHs1Proj = new TH1D("fHs1Proj","Projection m^{2}_{23} ("+fn2+fn3+")",ndbins,0,s1maxl*1.05);
	//fHs2Proj = new TH1D("fHs2Proj","Projection m^{2}_{31} ("+fn3+fn1+")",ndbins,0,s2maxl*1.05);
	//fHs3Proj = new TH1D("fHs3Proj","Projection m^{2}_{12} ("+fn1+fn2+")",ndbins,0,s3maxl*1.05);
	//fHs1M   = new TH1D("fHs1M","Mass m_{23} ("+fn2+fn3+")",ndbins,0,sqrt(s1maxl)*1.05);
	//fHs2M   = new TH1D("fHs2M","Mass m_{31} ("+fn3+fn1+")",ndbins,0,sqrt(s2maxl)*1.05);
	//fHs3M   = new TH1D("fHs3M","Mass m_{12} ("+fn1+fn2+")",ndbins,0,sqrt(s3maxl)*1.05);	

	fHDalitz   = new TH2F("fHDalitz","Dalitz Plot",                     ndbins, hs1min, s1maxl + ds1m, ndbins, hs2min, s2maxl + ds2m);
	fHDalitzSct= new TH2F("fHDalitzSct","Dalitz Plot (scat)",           ndbins, hs1min, s1maxl + ds1m, ndbins, hs2min, s2maxl + ds2m);
	fHDalitzI  = new TH2F("fHDalitzI","Dalitz Plot (incoherent)",       ndbins, hs1min, s1maxl + ds1m, ndbins, hs2min, s2maxl + ds2m);
	fHDalitzD  = new TH2F("fHDalitzD","Difference coherent-incoherent", ndbins, hs1min, s1maxl + ds1m, ndbins, hs2min, s2maxl + ds2m);
	fHDalitzPh = new TH2F("fHDalitzPh","Dalitz Plot (total phase)",     ndbins, hs1min, s1maxl + ds1m, ndbins, hs2min, s2maxl + ds2m);

	fHs1Proj = new TH1D("fHs1Proj","Projection m^{2}_{23} ("+fn2+fn3+")", ndbins, hs1min, s1maxl + ds1m);
	fHs2Proj = new TH1D("fHs2Proj","Projection m^{2}_{31} ("+fn3+fn1+")", ndbins, hs2min, s2maxl + ds2m);
	fHs3Proj = new TH1D("fHs3Proj","Projection m^{2}_{12} ("+fn1+fn2+")", ndbins, hs3min, s3maxl + ds3m);
	fHs1M   = new TH1D("fHs1M","Mass m_{23} ("+fn2+fn3+")",               ndbins, sqrt(hs1min), sqrt(s1maxl + ds1m));
	fHs2M   = new TH1D("fHs2M","Mass m_{31} ("+fn3+fn1+")",               ndbins, sqrt(hs2min), sqrt(s2maxl + ds2m));
	fHs3M   = new TH1D("fHs3M","Mass m_{12} ("+fn1+fn2+")",               ndbins, sqrt(hs3min), sqrt(s3maxl + ds3m));
	
	fHs1Proj->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHs1Proj->SetStats(0);
	
	fHs2Proj->SetXTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");
	fHs2Proj->SetStats(0);

	fHs3Proj->SetXTitle("m_{12}^{2}("+fn1+fn2+") [GeV^{2}/c^{4}]");
	fHs3Proj->SetStats(0);
	
	fHs1M->SetXTitle("m_{23}("+fn2+fn3+") [GeV/c^{2}]");
	fHs1M->SetStats(0);
	
	fHs2M->SetXTitle("m_{31}("+fn3+fn1+") [GeV/c^{2}]");
	fHs2M->SetStats(0);

	fHs3M->SetXTitle("m_{12}("+fn1+fn2+") [GeV/c^{2}]");
	fHs3M->SetStats(0);
	
	ConfigPlot();
	CreateDalitz();
}


//______________________________________________________________________________
// handle change of combo box selection (either final state or resonance)
void TDalitzGui::DoCombo(Int_t id)
{
	TGFrame *frm = (TGFrame *) gTQSender;
	TGComboBox *cb=0;
    if (frm->IsA()->InheritsFrom(TGComboBox::Class()))
      cb = (TGComboBox*) frm;
      
	Int_t Wid = cb->WidgetId();

	if (Wid!=HComPlot)  // ComboBox Final State (FS) or Resonance
	{
		if (Wid<=HComId3) // If final state changed, all chaches are invalid
		{
			for (int i=0;i<6;++i) fRes[i]->SetCacheValid(kFALSE);
		}
		else // If resonance changes, only that cache is invalid
		{
			fRes[Wid-HComRId1]->SetCacheValid(kFALSE);
		}
		if (id<0) id=0;
		if (id>N_FIN-1) id=N_FIN-1;
	
		ReadOutGui();

		CreateGraph(fGraphKin, ngbins, fM, fm1, fm2, fm3);
		if (fCheckResize->GetState()) DoResize();
		else CreateDalitz();
	}
	else  // ComboBox Plot
	{
		if (id==3) palette2(2);
		else SwitchColor();
	}
	
	DrawPlot();
}

//______________________________________________________________________________
// toggle color/bw 
// in case the phase histogram is shown, a special color palette is loaded
void TDalitzGui::SwitchColor()
{
	if (fComPlot->GetSelected()==3) return;
	palette2(fCheckColor->GetState());
	DrawPlot();
}
//______________________________________________________________________________

TDalitzGui::~TDalitzGui()
{
   // Clean up
   fMain->Cleanup();
   //delete fMain;
	
}

//______________________________________________________________________________
void TDalitzGui::CloseWindow()
{
   gApplication->Terminate(0);
}

//______________________________________________________________________________


int main(int argc, char** argv)
{
	TApplication theApp("App",&argc,argv);
	new TDalitzGui(gClient->GetRoot(),10,10);
	theApp.Run();
	return 0;
}


//______________________________________________________________________________

void DalitzGUI()
{
	new TDalitzGui(gClient->GetRoot(), 20, 20);
}
//______________________________________________________________________________

void TDalitzGui::MakeAnimatedGif()
{	
	TString fname=gAniFileName;

	gSystem->Exec("rm "+fname);
	fname+="+";
	fname+=(int) gAniGifTime;
	
	if (gAniGifVar==0)
	{
		gAniGifMin*=100.;
		gAniGifMax*=100.;
	}

	cout <<"Creating animated GIF for "<<vars[gAniGifVar]<<" ["<<gAniGifMin/100.<<" ... "<<gAniGifMax/100.<<"]";
	cout <<", "<<gAniGifSteps<<" frames, delta_t = "<<gAniGifTime<<" ms"<<endl;
	
	double step = (gAniGifMax - gAniGifMin)/gAniGifSteps;
	
	TGHSlider *sl=0;
	
	if (gAniGifVar == 0) sl = fHslider1;
	else if (gAniGifVar%2) sl = fSlResA[(gAniGifVar-1)/2];
	else sl = fSlResph[gAniGifVar/2 - 1];
	
	int bufPlot       = fComPlot->GetSelected();
	Double_t bufSlider = sl->GetPosition();
	
	fComPlot->Select(gAniGifPlot);
	
	Float_t var = gAniGifMin;
	TLatex tex;
	tex.SetTextColor(1);
	tex.SetTextSize(0.04);
	
	TPad *pad = (TPad*)fCanvas->GetCanvas()->cd(1);
	
	cout <<"Progress: "<<flush;
		
	int cnt = 0;
	while (var<gAniGifMax+0.5*step)
	{
		//cout <<var/100. <<" "<<flush;
		cout <<"#"<<flush;
		sl->SetPosition(var);
		
		if (gAniGifVar==0) DoSlider(0);
		else DoParmSlider(0);
		
		fCanvas->GetCanvas()->cd(1);
		
		TString txtvar = TString::Format("%s = %.2f",vars_tex[(int)gAniGifVar].Data(),var/100.);
		
		double xmin = pad->GetUxmin(), xmax = pad->GetUxmax();
		double ymin = pad->GetUymin(), ymax = pad->GetUymax();
		tex.DrawLatex(0.72*(xmax-xmin)+xmin,0.93*(ymax-ymin)+ymin,txtvar);
		
		fCanvas->GetCanvas()->Update();
		fCanvas->GetCanvas()->SaveAs(fname);
		
		var+=step;
	}
	
	cout <<" ... done"<<endl;
	
	fComPlot->Select(bufPlot);
	sl->SetPosition(bufSlider);
	
	DoSlider(0);
	DoParmSlider(0);
}

//______________________________________________________________________________
//______________________________________________________________________________


//______________________________________________________________________________
// The Dialog Window for Animated GIF Export
//______________________________________________________________________________

AniGifDialog::AniGifDialog(const TGWindow *p, const TGWindow *main)
{
	// build widgets
	fMain = new TGTransientFrame(p, main, 10, 10, kVerticalFrame);
	fMain->Connect("CloseWindow()", "AniGifDialog", this, "CloseWindow()");
	fMain->DontCallClose(); // to avoid double deletions.

	// use hierarchical cleaning
	fMain->SetCleanup(kDeepCleanup);
   
	// **************** The first row
	fVframe0 = new TGHorizontalFrame(fMain, 0, 0, 0);
	fVframe1 = new TGHorizontalFrame(fMain, 0, 0, 0);
	
	// combo box for choice of large plot
	fLab[0] = new TGLabel(fVframe0, "Plot");
	fComPlot = new TGComboBox(fVframe0,0);
	fComPlot->AddEntry("Dalitz",0);
	fComPlot->AddEntry("Dalitz (scat)",10);
	fComPlot->AddEntry("Dalitz In",1);
	fComPlot->AddEntry("Dalitz Diff",2);
	fComPlot->AddEntry("Dalitz Ph",3);
	fComPlot->AddEntry("Proj s23",4);
	fComPlot->AddEntry("Proj s31",5);
	fComPlot->AddEntry("Proj s12",6);
	fComPlot->AddEntry("Mass m23",7);
	fComPlot->AddEntry("Mass m31",8);
	fComPlot->AddEntry("Mass m12",9);
	fComPlot->Select(0);
	fComPlot->Resize(100,20);
	
	// combo box variable
	fLab[1] = new TGLabel(fVframe0, "Var");
	fComVar = new TGComboBox(fVframe0,1);
	for (int i=0;i<13;++i)
		fComVar->AddEntry(vars[i],i);
	fComVar->Select(0);
	fComVar->Connect("Selected(Int_t)","AniGifDialog", this, "DoCombo(Int_t)");
	fComVar->Resize(80,20);
	
	
	// Text input for steps
	fLab[5] = new TGLabel(fVframe0, "Steps");
	fTeh1 = new TGTextEntry(fVframe0, fTbh1 = new TGTextBuffer(8), 2);
	char tmp[20];
	sprintf(tmp,"20");
	fTbh1->AddText(0, tmp);
  	fTeh1->Resize(50);
	
	// Text input for time
	fLab[6] = new TGLabel(fVframe0, "Time");
	fTeh2 = new TGTextEntry(fVframe0, fTbh2 = new TGTextBuffer(8), 2);
	sprintf(tmp,"20");
	fTbh2->AddText(0, tmp);
  	fTeh2->Resize(50);
	
	// Label for range
	fLab[2] = new TGLabel(fVframe1, "Range");
	
	fLab[3] = new TGLabel(fVframe1, "1000");
	fLab[3]->Resize(60,20);
	fLab[4] = new TGLabel(fVframe1, "1350");
	fLab[4]->Resize(60,20);
	
	// slider for choosing M
	fHslider1 = new TGDoubleHSlider(fVframe1, 200);
	fHslider1->SetRange(0,1);
	fHslider1->SetPosition(0.0, 1.0);
	fHslider1->Connect("PositionChanged()","AniGifDialog",this,"DoSlider()");
	fHslider1->Resize(200,20);
	fHslider1->SetScale(20);
	
	fOkButton = new TGTextButton(fVframe1, " Save as ", 1);
	fOkButton->Connect("Clicked()", "AniGifDialog", this, "DoOK()");
	fCancelButton = new TGTextButton(fVframe1, " Cancel ", 1);
	fCancelButton->Connect("Clicked()", "AniGifDialog", this, "CloseWindow()");
	
	
	//--- layout for the frame: place at bottom, right aligned
	fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft,    4, 4, 4, 4);
	fL2 = new TGLayoutHints(kLHintsTop | kLHintsLeft,    2, 2, 2, 2);
	
	fVframe0->AddFrame(fLab[0], fL1);
	fVframe0->AddFrame(fComPlot, fL1);
	
	fVframe0->AddFrame(fLab[1], fL1);
	fVframe0->AddFrame(fComVar, fL1);
	
	fVframe0->AddFrame(fLab[5], fL1);
	fVframe0->AddFrame(fTeh1, fL1);
	
	fVframe0->AddFrame(fLab[6], fL1);
	fVframe0->AddFrame(fTeh2, fL1);
	
	
	fVframe1->AddFrame(fLab[2], fL1); 
	fVframe1->AddFrame(fLab[3], fL1); 
	fVframe1->AddFrame(fHslider1, fL1);
	fVframe1->AddFrame(fLab[4], fL1);
	 
	fVframe1->AddFrame(fOkButton, fL1);
	fVframe1->AddFrame(fCancelButton, fL1);

	fMain->AddFrame(fVframe0, fL2);	
	fMain->AddFrame(fVframe1, fL2);	
	
	// set dialog box title
	fMain->SetWindowName("Create Animated GIF");
	fMain->SetIconName("Create Animated GIF");
	fMain->SetClassHints("AniGifDialog", "AniGifDialog");
	
	// resize & move to center
	fMain->MapSubwindows();
	UInt_t width = fMain->GetDefaultWidth();
	UInt_t height = fMain->GetDefaultHeight();
	fMain->Resize(width, height);
	fMain->CenterOnParent();
	
	// make the message box non-resizable
	fMain->SetWMSize(width, height);
	fMain->SetWMSizeHints(width, height, width, height, 0, 0);
	fMain->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH | kMWMDecorMaximize |
					  kMWMDecorMinimize | kMWMDecorMenu,
					  kMWMFuncAll | kMWMFuncResize | kMWMFuncMaximize |
					  kMWMFuncMinimize, kMWMInputModeless);

	slmin = M_min;
	slmax = M_max;
	
	DoSlider();

	fMain->MapWindow();
	gClient->WaitFor(fMain);
}

AniGifDialog::~AniGifDialog()
{
   // dtor

   fMain->DeleteWindow();
}

void AniGifDialog::CloseWindow()
{
 	delete this;
}

void AniGifDialog::DoCombo(Int_t idx)
{
	if (idx==0) //M
	{
		slmin = M_min;
		slmax = M_max;
	}
	else if (idx%2) //Amps
	{
		slmin = A_min;
		slmax = A_max;
	}
	else 
	{
		slmin = Ph_min;
		slmax = Ph_max;
	}
	
	DoSlider();
}

void AniGifDialog::DoSlider()
{
	Float_t pos1, pos2;
	fHslider1->GetPosition(pos1, pos2);
	
	Float_t cmin = (slmax-slmin)*pos1+slmin;
	Float_t cmax = (slmax-slmin)*pos2+slmin;
	
	if (fComVar->GetSelected()!=0) 
	{
		cmin/=100.;
		cmax/=100.;
	}
	
	fLab[3]->SetText((TString::Format("%4.2f",cmin)).Data());
	fLab[4]->SetText((TString::Format("%4.2f",cmax)).Data());
}

void AniGifDialog::DoOK()
{
   	// Handle ok button.
	static TString dir(".");
	TGFileInfo fi;
	fi.fFileTypes = filetypesgif;
	fi.fIniDir    = StrDup(dir);
	new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);

	if (fi.fFilename)
	{		
		gAniFileName=fi.fFilename;
        TString ending(gAniFileName(gAniFileName.Length()-3,3));
		if (ending!="gif") gAniFileName+=".gif";

		dir = fi.fIniDir;
		
		Float_t pos1,pos2;
		fHslider1->GetPosition(pos1,pos2);
		
		Float_t cmin = (slmax-slmin)*pos1+slmin;
		Float_t cmax = (slmax-slmin)*pos2+slmin;
	  
		gAniGifOk = true;

		gAniGifVar   = fComVar->GetSelected();
		gAniGifPlot  = fComPlot->GetSelected();
		gAniGifMin   = cmin;
		gAniGifMax   = cmax;
		gAniGifSteps = atof(fTeh1->GetText()); 
		gAniGifTime  = atof(fTeh2->GetText());
		
		if (gAniGifSteps<2) gAniGifSteps=2;
		if (gAniGifSteps>1000) gAniGifSteps=1000;
		
		if (gAniGifTime<1) gAniGifTime=1;
		if (gAniGifTime>1000) gAniGifTime=1000;
		
		fMain->SendCloseMessage();
	}
}

