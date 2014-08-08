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
#include "TGComboBox.h"
#include "TGMenu.h"
#include "TGDockableFrame.h"
#include "TGFileDialog.h"

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
const char *filetypesscrn[] = { "All files",     "*",  0, 0 };

enum ETestCommandIdentifiers {
   HCId1,HCId2,HId1,
   
   HSId1,HSId2,HSId3,
   
   HComId1,HComId2,HComId3,
   
   HComRId1,HComRId2,HComRId3,HComRId4,HComRId5,HComRId6,
   
   HSlRAId1,HSlRAId2,HSlRAId3,HSlRAId4,HSlRAId5,HSlRAId6,
   
   HSlRpId1,HSlRpId2,HSlRpId3,HSlRpId4,HSlRpId5,HSlRpId6,
   
   HSlRjId1,HSlRjId2,HSlRjId3,HSlRjId4,HSlRjId5,HSlRjId6,
   
   HChRId1,HChRId2,HChRId3,HChRId4,HChRId5,HChRId6,
   
   HChId1, HChId2, HSlBins, HComPlot,
   
   M_FILE_OPEN, M_FILE_SAVE, M_FILE_SAVE_SCRN, M_FILE_EXIT
};

const Double_t m_gam  = 0.0;
const Double_t m_pi   = 0.13957;
const Double_t m_pi0  = 0.13498;
const Double_t m_et   = 0.5478;
const Double_t m_etp  = 0.9578;
const Double_t m_K    = 0.4937;
const Double_t m_K0   = 0.4976;
const Double_t m_jpsi = 3.0969;
const Double_t m_chic1= 3.511;
const Double_t m_hc   = 3.525;
const Double_t m_psip = 3.686;

const Double_t m_D0 = 1.864;
const Double_t m_Ds = 1.969;
const Double_t m_B0 = 5.279;

const Double_t m_rho  = 0.770;
const Double_t m_ome  = 0.7826;
const Double_t m_Kst  = 0.892;
const Double_t m_f098 = 0.980;
const Double_t m_phi  = 1.020;
const Double_t m_f212 = 1.275;
const Double_t m_f013 = 1.370;
const Double_t m_K014 = 1.412;
const Double_t m_K214 = 1.430;
const Double_t m_f015 = 1.500;
const Double_t m_f215 = 1.525;
const Double_t m_f017 = 1.710;
const Double_t m_f217 = 1.710;
const Double_t m_X387 = 3.872;
const Double_t m_Z390 = 3.900;
const Double_t m_Z443 = 4.430;
 
const Double_t G_rho  = 0.150;
const Double_t G_ome  = 0.0001;
const Double_t G_Kst  = 0.050;
const Double_t G_f098 = 0.070;
const Double_t G_phi  = 0.004;
const Double_t G_f212 = 0.185;
const Double_t G_f013 = 0.300;
const Double_t G_K014 = 0.294;
const Double_t G_K214 = 0.100;
const Double_t G_f015 = 0.112;
const Double_t G_f215 = 0.073;
const Double_t G_f017 = 0.125;
const Double_t G_f217 = 0.125;
const Double_t G_X387 = 0.002;
const Double_t G_Z390 = 0.063;
const Double_t G_Z443 = 0.107;

const Int_t    J_rho  = 1;
const Int_t    J_ome  = 1;
const Int_t    J_Kst  = 1;
const Int_t    J_f098 = 0;
const Int_t    J_phi  = 1;
const Int_t    J_f212 = 2;
const Int_t    J_f013 = 0;
const Int_t    J_K014 = 0;
const Int_t    J_K214 = 2;
const Int_t    J_f015 = 0;
const Int_t    J_f215 = 2;
const Int_t    J_f017 = 0;
const Int_t    J_f217 = 2;
const Int_t    J_X387 = 1;
const Int_t    J_Z390 = 0;
const Int_t    J_Z443 = 0;

const int MAX_FIN = 11; 
const Double_t m_fin[MAX_FIN]    = {m_gam,    m_pi,  m_pi0,     m_K, m_K0,    m_et,   m_etp,   m_jpsi,   m_chic1,     m_hc,    m_psip};
const TString  nam_fin[MAX_FIN]  = {"gamma",  "pi",  "pi0",     "K", "K0",    "eta",  "eta'",  "J/psi",  "chic1",     "hc",    "psi'"};
const TString  nam_pfin[MAX_FIN] = {"#gamma", "#pi", "#pi^{0}", "K", "K^{0}", "#eta", "#eta'", "J/#psi", "#chi_{c1}", "h_{c}", "#psi'"};

const int MAX_RES=16;

const TString  name_res[MAX_RES] = {"rho(770)","om(782)","K*(892)","f0(980)","phi(1020)","f2(1270)","f0(1370)","K0(1430)",
									"K2(1430)","f0(1500)","f2(1525)","f0(1710)","f2(1710)","X(3872)","Z(3900)","Z(4430)"};
const Double_t m_res[MAX_RES] = {m_rho, m_ome, m_Kst, m_f098, m_phi, m_f212, m_f013, m_K014, m_K214, m_f015, m_f215, m_f017, m_f217, m_X387, m_Z390, m_Z443};
const Double_t G_res[MAX_RES] = {G_rho, G_ome, G_Kst, G_f098, G_phi, G_f212, G_f013, G_K014, G_K214, G_f015, G_f215, G_f017, G_f217, G_X387, G_Z390, G_Z443};
const Int_t    J_res[MAX_RES] = {J_rho, J_ome, J_Kst, J_f098, J_phi, J_f212, J_f013, J_K014, J_K214, J_f015, J_f215, J_f017, J_f217, J_X387, J_Z390, J_Z443};

const Double_t M_max = 8.0;
const Double_t M_min = 0.5;

const Int_t ngbins = 140;
const Int_t lSlider = 140;
const Int_t lMSlider = 200;
  

TDalitzGui::TDalitzGui(const TGWindow *p,UInt_t w,UInt_t h)
{
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
 	fCanvas->GetCanvas()->GetPad(1)->SetRightMargin(0.1);
	
	gStyle->SetLabelSize(0.03,"Z");
	//gStyle->SetTitleOffset(0.12,"X");
	//gStyle->SetTitleOffset(0.12,"Y");

	// **************** The first row
	fVframe0 = new TGHorizontalFrame(fMain, 0, 0, 0);
	
	// Label for M (mother mass)
	fLab1 = new TGLabel(fVframe0, "M");
	
	// Text input for M
	fTeh1 = new TGTextEntry(fVframe0, fTbh1 = new TGTextBuffer(8), HId1);
	char tmp[20];
	sprintf(tmp,"%5.2f",fM);
	fTbh1->AddText(0, tmp);
   	fTeh1->Connect("ReturnPressed()", "TDalitzGui", this, "DoText()");
   	fTeh1->Resize(60);
	
	// resize button for kinmatic limits
	fCheckResize = new TGCheckButton(fVframe0,"Auto",HChId1);
	fCheckResize->SetState(kButtonDown);
    fCheckResize->Connect("Clicked()", "TDalitzGui", this, "DoResize()");

	// slider for choosing M
	fHslider1 = new TGHSlider(fVframe0,lMSlider);
	fHslider1->SetRange(Int_t(M_min*100),Int_t(M_max*100));
	fHslider1->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoSlider(Int_t)");
	fHslider1->SetPosition((Int_t)(fM*100.));
	fHslider1->Resize(135,20);
		
	// combo box for mass m1
	fLab2 = new TGLabel(fVframe0, "m1");
	fComFin1 = new TGComboBox(fVframe0,HComId1);
	ConfigComboFin(fComFin1);
	
	// combo box for mass m2
	fLab3 = new TGLabel(fVframe0, "m2");
	fComFin2 = new TGComboBox(fVframe0,HComId2);
	ConfigComboFin(fComFin2);
	
	// combo box for mass m3	
	fLab4 = new TGLabel(fVframe0, "m3");
	fComFin3 = new TGComboBox(fVframe0,HComId3);
	ConfigComboFin(fComFin3);
	
	// Text input for bins
	fLab5 = new TGLabel(fVframe0, "Bins");
	
	fTeh2 = new TGTextEntry(fVframe0, fTbh2 = new TGTextBuffer(10), HSlBins);
	sprintf(tmp,"%d",ndbins);
	fTbh2->AddText(0, tmp);
   	fTeh2->Connect("ReturnPressed()", "TDalitzGui", this, "Rebin()");
	fTeh2->Resize(40);
   	
   	// check box for color
	fCheckColor = new TGCheckButton(fVframe0,"Color",HChId1);
	fCheckColor->SetState(kButtonUp);
    fCheckColor->Connect("Clicked()", "TDalitzGui", this, "SwitchColor()");

	// combo box for choice of large plot
	fLab6 = new TGLabel(fVframe0, "Plot");
	fComPlot = new TGComboBox(fVframe0,HComPlot);
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
	fLab7 = new TGLabel(fVframe0, "Occ");
	// slider for choosing occupancy of random generated plots
	fSlOcc = new TGHSlider(fVframe0,70);
	fSlOcc->SetRange(10,1000);
	fSlOcc->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoParmSlider(Int_t)");
	fSlOcc->SetPosition((Int_t)(200.));
	fSlOcc->Resize(70,20);

	// ****************
	// the resonance setup GUI
	
	fVframe1 = new TGHorizontalFrame(fMain, 0, 0, 0);
	fVframe2 = new TGHorizontalFrame(fMain, 0, 0, 0);
	fVframe3 = new TGHorizontalFrame(fMain, 0, 0, 0);
	
	fLabR5N = new TGLabel(fVframe3,"R12");
	fLabR5A = new TGLabel(fVframe3,"A");fLabR5ph = new TGLabel(fVframe3,"ph");
	fLabR6A = new TGLabel(fVframe3,"A");fLabR6ph = new TGLabel(fVframe3,"ph");
	
	fLabR1N = new TGLabel(fVframe1,"R23");
	fLabR1A = new TGLabel(fVframe1,"A");fLabR1ph = new TGLabel(fVframe1,"ph");
	fLabR2A = new TGLabel(fVframe1,"A");fLabR2ph = new TGLabel(fVframe1,"ph");
	
	fLabR3N = new TGLabel(fVframe2,"R31");
	fLabR3A = new TGLabel(fVframe2,"A");fLabR3ph = new TGLabel(fVframe2,"ph");
	fLabR4A = new TGLabel(fVframe2,"A");fLabR4ph = new TGLabel(fVframe2,"ph");
	
	fComRes1 = new TGComboBox(fVframe1,HComRId1); ConfigComboRes(fComRes1);
	fComRes2 = new TGComboBox(fVframe1,HComRId2); ConfigComboRes(fComRes2);
	fComRes3 = new TGComboBox(fVframe2,HComRId3); ConfigComboRes(fComRes3);
	fComRes4 = new TGComboBox(fVframe2,HComRId4); ConfigComboRes(fComRes4);
	fComRes5 = new TGComboBox(fVframe3,HComRId5); ConfigComboRes(fComRes5);
	fComRes6 = new TGComboBox(fVframe3,HComRId6); ConfigComboRes(fComRes6);
	
	fSlRes1A = new TGHSlider(fVframe1,lSlider,kSlider1 | kScaleBoth, HSlRAId1);
	fSlRes1ph = new TGHSlider(fVframe1,lSlider,kSlider1 | kScaleBoth, HSlRpId1);
	ConfigSliderRes(fSlRes1A, fSlRes1ph);
	
	fSlRes2A = new TGHSlider(fVframe1,lSlider,kSlider1 | kScaleBoth, HSlRAId2);
	fSlRes2ph = new TGHSlider(fVframe1,lSlider,kSlider1 | kScaleBoth, HSlRpId2);
	ConfigSliderRes(fSlRes2A, fSlRes2ph);
	
	fSlRes3A = new TGHSlider(fVframe2,lSlider,kSlider1 | kScaleBoth, HSlRAId3);
	fSlRes3ph = new TGHSlider(fVframe2,lSlider,kSlider1 | kScaleBoth, HSlRpId3);
	ConfigSliderRes(fSlRes3A, fSlRes3ph);
	
	fSlRes4A = new TGHSlider(fVframe2,lSlider,kSlider1 | kScaleBoth, HSlRAId4);
	fSlRes4ph = new TGHSlider(fVframe2,lSlider,kSlider1 | kScaleBoth, HSlRpId4);
	ConfigSliderRes(fSlRes4A, fSlRes4ph);
	
	fSlRes5A = new TGHSlider(fVframe3,lSlider,kSlider1 | kScaleBoth, HSlRAId5);
	fSlRes5ph = new TGHSlider(fVframe3,lSlider,kSlider1 | kScaleBoth, HSlRpId5);
	ConfigSliderRes(fSlRes5A, fSlRes5ph);
	
	fSlRes6A = new TGHSlider(fVframe3,lSlider,kSlider1 | kScaleBoth, HSlRAId6);
	fSlRes6ph = new TGHSlider(fVframe3,lSlider,kSlider1 | kScaleBoth, HSlRpId6);
	ConfigSliderRes(fSlRes6A, fSlRes6ph);
	
	fChRes1 = new TGCheckButton(fVframe1,"On",HChRId1);
	fChRes1->SetState(kButtonDown);
    fChRes1->Connect("Clicked()", "TDalitzGui", this, "DoCheckRes()");
	
	fChRes2 = new TGCheckButton(fVframe1,"On",HChRId2);
	fChRes2->SetState(kButtonUp);
    fChRes2->Connect("Clicked()", "TDalitzGui", this, "DoCheckRes()");
	
	fChRes3 = new TGCheckButton(fVframe2,"On",HChRId3);
	fChRes3->SetState(kButtonDown);
    fChRes3->Connect("Clicked()", "TDalitzGui", this, "DoCheckRes()");
	
	fChRes4 = new TGCheckButton(fVframe2,"On",HChRId4);
	fChRes4->SetState(kButtonUp);
    fChRes4->Connect("Clicked()", "TDalitzGui", this, "DoCheckRes()");
	
	fChRes5 = new TGCheckButton(fVframe3,"On",HChRId5);
	fChRes5->SetState(kButtonDown);
    fChRes5->Connect("Clicked()", "TDalitzGui", this, "DoCheckRes()");
	
	fChRes6 = new TGCheckButton(fVframe3,"On",HChRId6);
	fChRes6->SetState(kButtonUp);
    fChRes6->Connect("Clicked()", "TDalitzGui", this, "DoCheckRes()");
		
	//--- layout for buttons: top align, equallyDoParmSlider expand horizontally
	fBly = new TGLayoutHints(kLHintsTop | kLHintsCenterX , 5, 5, 5, 5);
	
	//--- layout for the frame: place at bottom, right aligned
	fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5);
	fBfly2 = new TGLayoutHints(kLHintsTop | kLHintsLeft,    5, 5, 5, 5);
	fBfly3 = new TGLayoutHints(kLHintsTop | kLHintsRight,   5, 5, 5, 5);
	
	fVframe0->AddFrame(fLab1, fBfly2);
	fVframe0->AddFrame(fTeh1, fBfly2);
	fVframe0->AddFrame(fHslider1, fBfly2);
	fVframe0->AddFrame(fCheckResize, fBfly2);
	
	fVframe0->AddFrame(fLab2, fBfly2);
	fVframe0->AddFrame(fComFin1, fBfly2);
	fVframe0->AddFrame(fLab3, fBfly2);
	fVframe0->AddFrame(fComFin2, fBfly2);
	fVframe0->AddFrame(fLab4, fBfly2);
	fVframe0->AddFrame(fComFin3, fBfly2);
	
	fVframe0->AddFrame(fLab5, fBfly2);
	fVframe0->AddFrame(fTeh2, fBfly2);
	
	fVframe0->AddFrame(fCheckColor, fBfly2);
	fVframe0->AddFrame(fLab6,fBfly2);
	fVframe0->AddFrame(fComPlot, fBfly2);
	
	fVframe0->AddFrame(fLab7, fBfly2);
	fVframe0->AddFrame(fSlOcc, fBfly2);
	
	// *********************
	fVframe1->AddFrame(fLabR1N, fBfly2);
	
	fVframe1->AddFrame(fComRes1, fBfly2);
	fVframe1->AddFrame(fChRes1, fBfly2);
	fVframe1->AddFrame(fLabR1A, fBfly2);
	fVframe1->AddFrame(fSlRes1A, fBfly2);
	fVframe1->AddFrame(fLabR1ph, fBfly2);
	fVframe1->AddFrame(fSlRes1ph, fBfly2);
	
	fVframe1->AddFrame(fComRes2, fBfly2);
	fVframe1->AddFrame(fChRes2, fBfly2);
	fVframe1->AddFrame(fLabR2A, fBfly2);
	fVframe1->AddFrame(fSlRes2A, fBfly2);
	fVframe1->AddFrame(fLabR2ph, fBfly2);
	fVframe1->AddFrame(fSlRes2ph, fBfly2);
	
	fVframe2->AddFrame(fLabR3N, fBfly2);
	
	fVframe2->AddFrame(fComRes3, fBfly2);
	fVframe2->AddFrame(fChRes3, fBfly2);
	fVframe2->AddFrame(fLabR3A, fBfly2);
	fVframe2->AddFrame(fSlRes3A, fBfly2);
	fVframe2->AddFrame(fLabR3ph, fBfly2);
	fVframe2->AddFrame(fSlRes3ph, fBfly2);
	
	fVframe2->AddFrame(fComRes4, fBfly2);
	fVframe2->AddFrame(fChRes4, fBfly2);
	fVframe2->AddFrame(fLabR4A, fBfly2);
	fVframe2->AddFrame(fSlRes4A, fBfly2);
	fVframe2->AddFrame(fLabR4ph, fBfly2);
	fVframe2->AddFrame(fSlRes4ph, fBfly2);
	
	
	fVframe3->AddFrame(fLabR5N, fBfly2);
	
	fVframe3->AddFrame(fComRes5, fBfly2);
	fVframe3->AddFrame(fChRes5, fBfly2);
	fVframe3->AddFrame(fLabR5A, fBfly2);
	fVframe3->AddFrame(fSlRes5A, fBfly2);
	fVframe3->AddFrame(fLabR5ph, fBfly2);
	fVframe3->AddFrame(fSlRes5ph, fBfly2);

	fVframe3->AddFrame(fComRes6, fBfly2);
	fVframe3->AddFrame(fChRes6, fBfly2);
	fVframe3->AddFrame(fLabR6A, fBfly2);
	fVframe3->AddFrame(fSlRes6A, fBfly2);
	fVframe3->AddFrame(fLabR6ph, fBfly2);
	fVframe3->AddFrame(fSlRes6ph, fBfly2);
	
	//********************
	
	fMain->AddFrame(fCanvas, fLcan);
	fMain->AddFrame(fVframe0, fBfly2);
	fMain->AddFrame(fVframe1, fBfly2);
	fMain->AddFrame(fVframe2, fBfly2);
	fMain->AddFrame(fVframe3, fBfly2);
	
	fMain->SetWindowName("Dalitz GUI");
	fMain->MapSubwindows();
	fMain->Resize(fMain->GetDefaultSize());
	fMain->MapWindow();
	
	// prepare the plots etc
	
	ReadOutGui();
	
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

	fGraphKin = new TGraph(ngbins*2+1);
	fGraphKin->SetLineColor(2);
	CreateGraph(fGraphKin, ngbins, fM,fm1,fm2,fm3);
	
	DoResize();
	DrawPlot();
}

//___________________________________________________________________

void TDalitzGui::Rebin()
{
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
  
	  case M_FILE_EXIT:
		 {
			CloseWindow();   // terminate theApp no need to use SendCloseMessage()
			break;
		 }
   }
}

//___________________________________________________________________
// Load configuration from file and set interface

void TDalitzGui::LoadConfiguration(TString filename)
{
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
	DoSlider(pos);
	
	// resonance state (on/off)
	file >> dumi; 
	fChRes1->SetState((EButtonState)dumi);
	file >> dumi; 
	fChRes2->SetState((EButtonState)dumi);
	file >> dumi; 
	fChRes3->SetState((EButtonState)dumi);
	file >> dumi; 
	fChRes4->SetState((EButtonState)dumi);
	file >> dumi; 
	fChRes5->SetState((EButtonState)dumi);
	file >> dumi; 
	fChRes6->SetState((EButtonState)dumi);
	
	// resonance types and parameters
	file >> dumi;
	fComRes1->Select(dumi);
	file >> dumf;
	fSlRes1A->SetPosition(dumf);
	file >> dumf;
	fSlRes1ph->SetPosition(dumf);
	
	file >> dumi;
	fComRes2->Select(dumi);
	file >> dumf;
	fSlRes2A->SetPosition(dumf);
	file >> dumf;
	fSlRes2ph->SetPosition(dumf);
	
	file >> dumi;
	fComRes3->Select(dumi);
	file >> dumf;
	fSlRes3A->SetPosition(dumf);
	file >> dumf;
	fSlRes3ph->SetPosition(dumf);
	
	file >> dumi;
	fComRes4->Select(dumi);
	file >> dumf;
	fSlRes4A->SetPosition(dumf);
	file >> dumf;
	fSlRes4ph->SetPosition(dumf);
	
	file >> dumi;
	fComRes5->Select(dumi);
	file >> dumf;
	fSlRes5A->SetPosition(dumf);
	file >> dumf;
	fSlRes5ph->SetPosition(dumf);
	
	file >> dumi;
	fComRes6->Select(dumi);
	file >> dumf;
	fSlRes6A->SetPosition(dumf);
	file >> dumf;
	fSlRes6ph->SetPosition(dumf);
	
	fMain->BlockAllSignals(false);

	// select final states
	file >> dumi;
	fComFin1->Select(dumi);
	file >> dumi;
	fComFin2->Select(dumi);
	file >> dumi;
	fComFin3->Select(dumi);
	
	// number of bins
	file >> dumi;
	if (dumi>500) dumi=500;
	sprintf(tmp,"%d",dumi);
	fTeh2->SetText(tmp);
	
	// color state
	file >> dumi; 
	fCheckColor->SetState((EButtonState)dumi);
	SwitchColor();
	
	// selected plot to draw
	file >> dumi;
	fComPlot->Select(dumi);
		
	DoResize();
	DoParmSlider(0);
}

//___________________________________________________________________
// Save configuration to file

void TDalitzGui::SaveConfiguration(TString filename)
{
	if (filename=="") return;
	
	ofstream file;
	file.open(filename.Data());
	file.precision(5);
	 
	// save total energy
	file << fM << endl;
	
	// save resonance on/off switch states
	file << fChRes1->GetState() << endl;
	file << fChRes2->GetState() << endl;
	file << fChRes3->GetState() << endl;
	file << fChRes4->GetState() << endl;
	file << fChRes5->GetState() << endl;
	file << fChRes6->GetState() << endl;
	
	// save resonance types and parameters
	file << fComRes1->GetSelected() << endl;
	file << fSlRes1A->GetPosition() << endl;
	file << fSlRes1ph->GetPosition() << endl;

	file << fComRes2->GetSelected() << endl;
	file << fSlRes2A->GetPosition() << endl;
	file << fSlRes2ph->GetPosition() << endl;
	
	file << fComRes3->GetSelected() << endl;
	file << fSlRes3A->GetPosition() << endl;
	file << fSlRes3ph->GetPosition() << endl;
	
	file << fComRes4->GetSelected() << endl;
	file << fSlRes4A->GetPosition() << endl;
	file << fSlRes4ph->GetPosition() << endl;
	
	file << fComRes5->GetSelected() << endl;
	file << fSlRes5A->GetPosition() << endl;
	file << fSlRes5ph->GetPosition() << endl;
	
	file << fComRes6->GetSelected() << endl;
	file << fSlRes6A->GetPosition() << endl;
	file << fSlRes6ph->GetPosition() << endl;

	// save final state particles
	file << fComFin1->GetSelected() << endl;
	file << fComFin2->GetSelected() << endl;
	file << fComFin3->GetSelected() << endl;
	
	// save number of bins
	file <<  Int_t(atoi(fTbh2->GetString())) << endl;

	// color state (bw/color)
	file << fCheckColor->GetState() << endl;
	
	// save selected plot to draw
	file << fComPlot->GetSelected() << endl;

	file.close();
}

//___________________________________________________________________
// Set combo box for final states   
void TDalitzGui::ConfigComboFin(TGComboBox* b)
{
	for (int i=0;i<MAX_FIN;i++)
		b->AddEntry(nam_fin[i].Data(),i);

	b->Select(1);
    b->Resize(60, 20);
	b->Connect("Selected(Int_t)","TDalitzGui", this, "DoCombo(Int_t)");
}

//___________________________________________________________________
// Set combo box for resonances
void TDalitzGui::ConfigComboRes(TGComboBox* b)
{
	for (int i=0;i<MAX_RES;++i) b->AddEntry(name_res[i],i);

	b->Select(0);
    b->Resize(90, 20);
	b->Connect("Selected(Int_t)","TDalitzGui", this, "DoCombo(Int_t)");
}

//___________________________________________________________________
// Set slider for amplitude and phase    
void TDalitzGui::ConfigSliderRes(TGHSlider *a, TGHSlider *ph)
{
	ph->SetRange(0,628);
	ph->SetPosition(0);
	ph->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoParmSlider(Int_t)");
	ph->SetScale(12);
	
	a->SetRange(0,1000);
	a->SetPosition(500);
	a->Connect("PositionChanged(Int_t)", "TDalitzGui", this, "DoParmSlider(Int_t)");
	a->SetScale(12);
	
}

//___________________________________________________________________
// Draw all plots
void TDalitzGui::DrawPlot()
{   	
   	fCanvas->GetCanvas()->GetPad(2)->cd(1);
	fHs1Proj->Draw();
	
   	fCanvas->GetCanvas()->GetPad(2)->cd(2);
	fHs2Proj->Draw();
	
   	fCanvas->GetCanvas()->GetPad(2)->cd(3);
	fHs3Proj->Draw();

   	fCanvas->GetCanvas()->GetPad(2)->cd(4);
	fHDalitzI->Draw("colz");
	fGraphKin->Draw("same L");
	
   	fCanvas->GetCanvas()->cd(1);
	switch (fComPlot->GetSelected())
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
	palette2(0);
	
	fHs1Proj=0;
	fHs2Proj=0;
	
	for (int i=0;i<5;i++)
	{
		fRes1[i]=new CRes();
		fRes2[i]=new CRes();
		fRes3[i]=new CRes();
	}
	
	fM = m_Ds;
	
	fm1 = m_pi;
	fm2 = m_pi; 
	fm3 = m_pi;
	
	fnR1 = 1;
	fnR2 = 1;
	fnR3 = 1;
	
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
// configure all histograms
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
// Compute all plots 
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
	
	Int_t nRes = fnR1 + fnR2 + fnR3;
	
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
			
			if (fRes1[0]->GetState()) {	
				//Atmp = getAmp(fRes1[0], sqrt(s1), fm2, fm3, qR1, qM23) * Z_R(s, s1, s2, s3, 1, fRes1[0]->GetM0(), fRes1[0]->GetJ(), fm2, fm3, fm1);
				Atmp = getAmp(fRes1[0], sqrt(s1), fm2, fm3, qR1, qM23) * Z_Ralt(s2, cs2min, cs2max, fRes1[0]->GetJ(), fRes1[0]->GetM0(), sqrt(s1), fm3);
				//if (!Atmp.IsNaN(Atmp)) 
				{
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}	
			}
			
			if (fRes1[1]->GetState()) {	
				//Atmp = getAmp(fRes1[1], sqrt(s1), fm2, fm3, qR1, qM23) * Z_R(s, s1, s2, s3, 1, fRes1[1]->GetM0(), fRes1[1]->GetJ(), fm2, fm3, fm1);
				Atmp = getAmp(fRes1[1], sqrt(s1), fm2, fm3, qR1, qM23) * Z_Ralt(s2, cs2min, cs2max, fRes1[1]->GetJ(), fRes1[1]->GetM0(), sqrt(s1), fm3);
				//if (!Atmp.IsNaN(Atmp)) 
				{
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}	
			}
			
			if (fRes2[0]->GetState()) {	
				//Atmp = getAmp(fRes2[0], sqrt(s2), fm1, fm3, qR2, qM13) * Z_R(s, s2, s1, s3, 2, fRes2[0]->GetM0(), fRes2[0]->GetJ(), fm1, fm3, fm2);
				Atmp = getAmp(fRes2[0], sqrt(s2), fm1, fm3, qR2, qM13) * Z_Ralt(s1, cs1min, cs1max, fRes2[0]->GetJ(), fRes2[0]->GetM0(), sqrt(s2), fm1);
				//if (!Atmp.IsNaN(Atmp)) 
				{
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}	
			}
			
			if (fRes2[1]->GetState()) {	
				//Atmp = getAmp(fRes2[1], sqrt(s2), fm1, fm3, qR2, qM13) * Z_R(s, s2, s1, s3, 2, fRes2[1]->GetM0(), fRes2[1]->GetJ(), fm1, fm3, fm2);
				Atmp = getAmp(fRes2[1], sqrt(s2), fm1, fm3, qR2, qM13) * Z_Ralt(s1, cs1min, cs1max, fRes2[1]->GetJ(), fRes2[1]->GetM0(), sqrt(s2), fm1);
				//if (!Atmp.IsNaN(Atmp)) 
				{
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}	
			}
			
			if (fRes3[0]->GetState()) {	
				//Atmp = getAmp(fRes3[0], sqrt(s3), fm1, fm2, qR3, qM12) * Z_R(s, s3, s2, s1, 3, fRes3[0]->GetM0(), fRes3[0]->GetJ(), fm1, fm2, fm3);
				Atmp = getAmp(fRes3[0], sqrt(s3), fm1, fm2, qR3, qM12) * (Z_Ralt(s1, cs3min, cs3max, fRes3[0]->GetJ(), fRes3[0]->GetM0(), sqrt(s3), fm2)+1e-10);
				//if (!Atmp.IsNaN(Atmp)) 
				{
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}	
			}
			
			if (fRes3[1]->GetState()) {	
				//Atmp = getAmp(fRes3[1], sqrt(s3), fm1, fm2, qR3, qM12) * Z_R(s, s3, s2, s1, 3, fRes3[1]->GetM0(), fRes3[1]->GetJ(), fm1, fm2, fm3);
				Atmp = getAmp(fRes3[1], sqrt(s3), fm1, fm2, qR3, qM12) * (Z_Ralt(s1, cs3min, cs3max, fRes3[1]->GetJ(), fRes3[1]->GetM0(), sqrt(s3), fm2)+1e-10);
				//if (!Atmp.IsNaN(Atmp)) 
				{
					Atot1 += Atmp;
					A_in  += Atmp.Rho();
					if (nRes==2) { if (Adiff==0) Adiff = Atmp.Theta();else Adiff-=Atmp.Theta();}
				}	
			}
			
			double In2 = Atot1.Rho2();
			
			if (In2!=In2) In2=0.;
				
			if (Adiff>3.1416) Adiff-=2*3.1415892;
			if (Adiff<-3.1416) Adiff+=2*3.1415892;

			fHDalitz->SetBinContent(i,j,In2);
			fHDalitzI->SetBinContent(i,j,A_in*A_in);
			fHDalitzD->SetBinContent(i,j,A_in*A_in-In2);
			if (nRes==2) 
				fHDalitzPh->SetBinContent(i,j,fabs(Adiff));
			else 
				fHDalitzPh->SetBinContent(i,j,Atot1.Theta());

			fHs1Proj->Fill(s1, In2);
			fHs2Proj->Fill(s2, In2);
			//fHs3Proj->Fill(s3, In2);
						
		}
	}
	
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

			//fHs1M->Fill(sqrt(fHs1Proj->GetRandom()));
			//fHs2M->Fill(sqrt(fHs2Proj->GetRandom()));
			//fHs3M->Fill(sqrt(fHs3Proj->GetRandom()));
			
		}

		//fHs1Proj->Scale(1./fHs1Proj->GetSumOfWeights());
		//fHs2Proj->Scale(1./fHs2Proj->GetSumOfWeights());
		//fHs3Proj->Scale(1./fHs3Proj->GetSumOfWeights());		
		
		//fHs1M->Scale(1./fHs1M->GetSumOfWeights());
		//fHs2M->Scale(1./fHs2M->GetSumOfWeights());
		//fHs3M->Scale(1./fHs3M->GetSumOfWeights());
		
		fHs1Proj->Scale(1./fHs1Proj->GetMaximum());
		fHs2Proj->Scale(1./fHs2Proj->GetMaximum());
		fHs3Proj->Scale(1./fHs3Proj->GetMaximum());		
		
		fHs1M->Scale(1./fHs1M->GetMaximum());
		fHs2M->Scale(1./fHs2M->GetMaximum());
		fHs3M->Scale(1./fHs3M->GetMaximum());
		
		fHDalitz->Scale(100./fHDalitz->GetMaximum());
		fHDalitzI->Scale(100./fHDalitzI->GetMaximum());
		fHDalitzD->Scale(100./fHDalitzD->GetMaximum());
	}
	
	fHs1Proj->Smooth(1);
	fHs2Proj->Smooth(1);
	fHs3Proj->Smooth(1);
	
	fHs1M->Smooth(1);
	fHs2M->Smooth(1);
	fHs3M->Smooth(1);
}

//___________________________________________________________________
// not implemented yet; should return the fraction of a bin corresponding
// to (s1,s2) covering phase space
int TDalitzGui::IsInPhsp(int ix, int iy)
{
	double binwx = fHDalitz->GetXaxis()->GetBinWidth(ix);
	double minx  = fHDalitz->GetXaxis()->GetBinLowEdge(ix);
	double maxx  = minx + binwx;
	
	double binwy = fHDalitz->GetYaxis()->GetBinWidth(iy);
	double miny  = fHDalitz->GetYaxis()->GetBinLowEdge(iy);
	double maxy  = miny + binwy;
	
	int mode = 0;

	if ( (minx>s1min(miny, fM, fm1, fm2, fm3) && minx<s1max(miny,fM, fm1, fm2, fm3) && miny>s2min(minx, fM, fm1, fm2, fm3) && miny<s2max(minx, fM, fm1, fm2, fm3)) ) mode += 1; //minx, miny
	if ( (maxx>s1min(miny, fM, fm1, fm2, fm3) && maxx<s1max(miny,fM, fm1, fm2, fm3) && miny>s2min(maxx, fM, fm1, fm2, fm3) && miny<s2max(maxx, fM, fm1, fm2, fm3)) ) mode += 2; //maxx, miny
	if ( (minx>s1min(maxy, fM, fm1, fm2, fm3) && minx<s1max(maxy,fM, fm1, fm2, fm3) && maxy>s2min(minx, fM, fm1, fm2, fm3) && maxy<s2max(minx, fM, fm1, fm2, fm3)) ) mode += 4; //minx, maxy
	if ( (maxx>s1min(maxy, fM, fm1, fm2, fm3) && maxx<s1max(maxy,fM, fm1, fm2, fm3) && maxy>s2min(maxx, fM, fm1, fm2, fm3) && maxy<s2max(maxx, fM, fm1, fm2, fm3)) ) mode += 8; //maxx, maxy
	
	return mode;
}


//___________________________________________________________________
// not implemented yet; should return the fraction of a bin corresponding
// to (s1,s2) covering phase space
double TDalitzGui::GetBinFraction(int ix, int iy, double &s1, double &s2)
{
	double binwx = fHDalitz->GetXaxis()->GetBinWidth(ix);
	double minx  = fHDalitz->GetXaxis()->GetBinLowEdge(ix);
	double maxx  = minx + binwx;
	
	double binwy = fHDalitz->GetYaxis()->GetBinWidth(iy);
	double miny  = fHDalitz->GetYaxis()->GetBinLowEdge(iy);
	double maxy  = miny + binwy;
	
	// get absolute minimum and maximum s1
	double cs1min = simin(1, fM, fm1, fm2, fm3);
	double cs1max = simax(1, fM, fm1, fm2, fm3);
	
	// get absolute minimum and maximum s2
	double cs2min = simin(2, fM, fm1, fm2, fm3);
	double cs2max = simax(2, fM, fm1, fm2, fm3);

	double frac = 1.0;

	int type = 0;
		
	// we're not at a vertical edge
	if (minx>cs1min && maxx<cs1max)
	{
		// get lower and upper phsp limits in s2 for xmin (lower edge in s1)
		double minxs2min = s2min(minx, fM, fm1, fm2, fm3);
		double minxs2max = s2max(minx, fM, fm1, fm2, fm3);
		
		// get lower and upper phsp limits in s2 for xmax (upper edge in s1)
		double maxxs2min = s2min(maxx, fM, fm1, fm2, fm3);
		double maxxs2max = s2max(maxx, fM, fm1, fm2, fm3);
		
		if (minxs2max>maxy && maxxs2max>maxy) // bin is on lower edge of phsp
		{
			if (minxs2min>maxy) type+=10;
			else if (minxs2min>miny) type+=20;
			else type+=30;
			
			if (maxxs2min>maxy) type+=1;
			else if (maxxs2min>miny) type+=2;
			else type+=3;	
		}
		else  // bin is on upper edge of phsp
		{
			if (minxs2max<miny) type+=60;
			else if (minxs2max<maxy) type+=50;
			else type+=40;
			
			if (maxxs2max<miny) type+=6;
			else if (maxxs2max<maxy) type+=5;
			else type+=4;	
		}
		
		if (type==0  || type==33 || type==44) return 1.; // bin is completely covered
		if (type==11 || type==66) return 0.;            // bin is not in phsp

		double dy1 = (maxy - maxxs2min);
		double dy2 = (miny - maxxs2min);
		double dy3 = (maxy - minxs2min);
		double dy4 = (miny - minxs2min);
		
		double dy5 = (maxy - maxxs2max);
		double dy6 = (miny - maxxs2max);
		double dy7 = (maxy - minxs2max);
		double dy8 = (miny - minxs2max);
		
		double Dy  = (minxs2min - maxxs2min);
		double Dy2 = (minxs2max - maxxs2max);
		
		double xi1=0, xi2=0;
		double binar = binwx*binwy;

		switch (type)
		{
		case 12: 
			s1 = maxx; s2 = maxy; // move coordinate into phsp
			xi1 = dy1/Dy*binwx;
			frac = 0.5*xi1*dy1/binar;
			break;
		case 13:
			s1 = maxx; s2 = (maxy-miny)/2.;
			xi1 = dy1/Dy*binwx;
			xi2 = dy2/Dy*binwx;
			frac = (xi2 + 0.5*(xi1-xi2))/binwx;
			break;
		case 22:
			s1 = (maxx-minx)/2.; s2 = (dy1+dy3)*0.25;
			frac = (dy1+dy3)*0.5/binwy;
			break;
		case 23:
			xi1 = dy3/Dy*binwx;
			frac = 1.0-0.5*xi1*dy3/binar;
			break;
		case 45:
			xi1 = dy5/Dy2*binwx;
			frac = 1.0-0.5*xi1*dy5/binar;
			break;
		case 46:
			xi1 = dy5/Dy2*binwx;
			xi2 = dy6/Dy2*binwx;
			frac = (xi2 + 0.5*(xi1-xi2))/binwx;
			break;
		case 55:
			s1 = (maxx-minx)*0.5; s2 = (dy5+dy7)*0.25;
			frac = (dy5+dy7)*0.5/binwy;
			break;
		case 56:
			s1 = minx; s2 = miny;
			xi1 = dy7/Dy2*binwx;
			frac = 0.5*xi1*dy7/binar;
			break;
		default:
			break;
		}
	}

	frac = fabs(frac);
	cout <<"c("<<ix<<"/"<<iy<<")="<<type<<" "<<frac<<endl;
	return frac;
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

   	char buf[32];

	fM=(Float_t)pos/100.;
	
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
		
	fnR1 = fnR2 = fnR3 = 0;
	
	fActiveRes = "";

	Double_t A=0., Ph=0., J=0.;
	Int_t resId;
	
	fRes1[0]->SetState((Bool_t)fChRes1->GetState());
	fRes1[1]->SetState((Bool_t)fChRes2->GetState());
	fRes2[0]->SetState((Bool_t)fChRes3->GetState());
	fRes2[1]->SetState((Bool_t)fChRes4->GetState());
	fRes3[0]->SetState((Bool_t)fChRes5->GetState());
	fRes3[1]->SetState((Bool_t)fChRes6->GetState());

	if (fRes1[0]->GetState())
	{
		A  = Double_t(fSlRes1A->GetPosition())/100.;
		Ph = Double_t(fSlRes1ph->GetPosition())/100.;
		resId = fComRes1->GetSelected();
		fRes1[0]->Set(m_res[resId], G_res[resId], fm2, fm3, A, Ph, J_res[resId]);  
		fnR1++;
		fActiveRes+=name_res[resId]+", ";
	}
	
	if (fRes1[1]->GetState())
	{
		A  = Double_t(fSlRes2A->GetPosition())/100.;
		Ph = Double_t(fSlRes2ph->GetPosition())/100.;
		resId = fComRes2->GetSelected();
		fRes1[1]->Set(m_res[resId], G_res[resId], fm2, fm3, A, Ph, J_res[resId]);  
		fnR1++;
		fActiveRes+=name_res[resId]+", ";
	}
	
	if (fRes2[0]->GetState())
	{
		A  = Double_t(fSlRes3A->GetPosition())/100.;
		Ph = Double_t(fSlRes3ph->GetPosition())/100.;
		resId = fComRes3->GetSelected();
		fRes2[0]->Set(m_res[resId], G_res[resId], fm1, fm3, A, Ph, J_res[resId]);  
		fnR2++;
		fActiveRes+=name_res[resId]+", ";
	}
	
	if (fRes2[1]->GetState())
	{
		A  = Double_t(fSlRes4A->GetPosition())/100.;
		Ph = Double_t(fSlRes4ph->GetPosition())/100.;
		resId = fComRes4->GetSelected();
		fRes2[1]->Set(m_res[resId], G_res[resId], fm1, fm3, A, Ph, J_res[resId]);  
		fnR2++;
		fActiveRes+=name_res[resId]+", ";
	}
	
	if (fRes3[0]->GetState())
	{
		A  = Double_t(fSlRes5A->GetPosition())/100.;
		Ph = Double_t(fSlRes5ph->GetPosition())/100.;
		resId = fComRes5->GetSelected();
		fRes3[0]->Set(m_res[resId], G_res[resId], fm1, fm2, A, Ph, J_res[resId]);  
		fnR3++;
		fActiveRes+=name_res[resId]+", ";
	}
	
	if (fRes3[1]->GetState())
	{
		A  = Double_t(fSlRes6A->GetPosition())/100.;
		Ph = Double_t(fSlRes6ph->GetPosition())/100.;
		resId = fComRes6->GetSelected();
		fRes3[1]->Set(m_res[resId], G_res[resId], fm1, fm2, A, Ph, J_res[resId]);  
		fnR3++;
		fActiveRes+=name_res[resId]+", ";
	}
	
	fActiveRes=fActiveRes(0,fActiveRes.Length()-2);
	
	fn1=nam_pfin[fComFin1->GetSelected()];
	fn2=nam_pfin[fComFin2->GetSelected()];
	fn3=nam_pfin[fComFin3->GetSelected()];

}
//______________________________________________________________________________
// action taking place when a resonance slider is moved
void TDalitzGui::DoParmSlider(Int_t pos)
{
	ReadOutGui();    // read out the interface settings
	CreateDalitz();  // rebuild plots
	DrawPlot();      // draw plots 
}
//______________________________________________________________________________
// obsolete routine
void TDalitzGui::ComputeCache(Int_t ri)
{
	CRes *r;

	switch (ri)
	{
	case 1: r=fRes1[0];
		break;
	case 2: r=fRes1[1];
		break;
	case 3: r=fRes2[0];
		break;
	case 4: r=fRes2[1];
		break;
	default: r=0;
	}

	if (!r) return;

}

//______________________________________________________________________________
// Rebuild of all histograms when 
// a) the number of bins has changed
// b) M has changed

void TDalitzGui::DoResize()
{
	ndbins = Int_t(atoi(fTbh2->GetString()));
	if (ndbins>500)
	{
		ndbins=500;
		fTeh2->SetText("500");
	}
	
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
	
	Double_t s1maxl = simax(1, fM, fm1, fm2, fm3);
	Double_t s2maxl = simax(2, fM, fm1, fm2, fm3);
	Double_t s3maxl = simax(3, fM, fm1, fm2, fm3);
			
	fHDalitz   = new TH2F("fHDalitz","Dalitz Plot", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	fHDalitzSct= new TH2F("fHDalitzSct","Dalitz Plot (scat)", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	fHDalitzI  = new TH2F("fHDalitzI","Dalitz Plot (incoherent)", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	fHDalitzD  = new TH2F("fHDalitzD","Difference coherent-incoherent", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);
	fHDalitzPh = new TH2F("fHDalitzPh","Dalitz Plot (total phase)", ndbins, 0, s1maxl*1.05, ndbins, 0, s2maxl*1.05);

	fHs1Proj = new TH1D("fHs1Proj","Projection m^{2}_{23} ("+fn2+fn3+")",ndbins,0,s1maxl*1.05);
	fHs2Proj = new TH1D("fHs2Proj","Projection m^{2}_{31} ("+fn3+fn1+")",ndbins,0,s2maxl*1.05);
	fHs3Proj = new TH1D("fHs3Proj","Projection m^{2}_{12} ("+fn1+fn2+")",ndbins,0,s3maxl*1.05);
	
	fHs1Proj->SetXTitle("m_{23}^{2}("+fn2+fn3+") [GeV^{2}/c^{4}]");
	fHs1Proj->SetStats(0);
	
	fHs2Proj->SetXTitle("m_{31}^{2}("+fn3+fn1+") [GeV^{2}/c^{4}]");
	fHs2Proj->SetStats(0);

	fHs3Proj->SetXTitle("m_{12}^{2}("+fn1+fn2+") [GeV^{2}/c^{4}]");
	fHs3Proj->SetStats(0);

	fHs1M   = new TH1D("fHs1M","Mass m_{23} ("+fn2+fn3+")",ndbins,0,sqrt(s1maxl)*1.05);
	fHs2M   = new TH1D("fHs2M","Mass m_{31} ("+fn3+fn1+")",ndbins,0,sqrt(s2maxl)*1.05);
	fHs3M   = new TH1D("fHs3M","Mass m_{12} ("+fn1+fn2+")",ndbins,0,sqrt(s3maxl)*1.05);
	
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

	if (Wid!=HComPlot)
	{
		if (id<0) id=0;
		if (id>MAX_FIN-1) id=MAX_FIN-1;
	
		switch (Wid)
		{
		case HComId1: fm1=m_fin[id];
				break;
		case HComId2: fm2=m_fin[id];
				break;
		case HComId3: fm3=m_fin[id];
				break;
		default: ReadOutGui();
				break;
		}
	
		fn1=nam_pfin[fComFin1->GetSelected()];
		fn2=nam_pfin[fComFin2->GetSelected()];
		fn3=nam_pfin[fComFin3->GetSelected()];


		CreateGraph(fGraphKin, ngbins, fM, fm1, fm2, fm3);
		if (fCheckResize->GetState()) DoResize();
		else CreateDalitz();
	}
	else
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


