// Mainframe macro generated from application: /home/stockman/fairroot/cbmsoft_old/tools/root/bin/root.exe
// By ROOT version 5.14/00 on 2007-10-31 15:25:55

#if !defined( __CINT__) || defined (__MAKECINT__)

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGuiBldHintsButton
#include "TGuiBldHintsButton.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TRootBrowser
#include "TRootBrowser.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"

#endif

void EventDisplay()
{
	MvdEventDisplay* display = new MvdEventDisplay(0,400,220);
}

class MvdEventDisplay {
	RQ_OBJECT("MvdEventDisplay")
	
public :
	MvdEventDisplay(const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~MvdEventDisplay();
	
	// slots
	void DoStartButton();
	void DoEventButton();
	void DoNextButton();
	void DoAnaEventsButton();
	void FillModulesListBox();
	void DrawSelectedModule(Int_t lbID, Int_t itemID);
	
public:
	
   // main frame
   TGMainFrame *mainFrame;
   // composite frame
   TGCompositeFrame *fMainFrame1660;

   // list box
   TGListBox *lbModules;

   // embedded canvas
   TRootEmbeddedCanvas *ecTab1Main;
   // embedded canvas
   TRootEmbeddedCanvas *ecTab1First;
   // embedded canvas
   TRootEmbeddedCanvas *ecTab1Second;
   
   TRootEmbeddedCanvas *ecTab2Main;
   
   TGTextButton *btnStartAna;
   TGTextButton *btnEventNr;
   TGTextButton *btnNextEvent;
   TGTextButton *btnAnaEvents;

   TGTextEntry *edtFileName;
   
   TGNumberEntry *netEventNr;
   TGNumberEntry *netEvents;

   MvdAllDataEventAna* ana;
};

MvdEventDisplay::MvdEventDisplay(const TGWindow* p, UInt_t w, UInt_t h){
	 // main frame
	   mainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
	   mainFrame->SetLayoutBroken(kTRUE);

	   // composite frame
	   TGCompositeFrame *fMainFrame826 = new TGCompositeFrame(mainFrame,981,859,kVerticalFrame);
	   fMainFrame826->SetLayoutBroken(kTRUE);

	   // composite frame
	   TGCompositeFrame *fCompositeFrame827 = new TGCompositeFrame(fMainFrame826,981,859,kVerticalFrame);
	   fCompositeFrame827->SetLayoutBroken(kTRUE);

	   // composite frame
	   TGCompositeFrame *fCompositeFrame828 = new TGCompositeFrame(fCompositeFrame827,929,848,kVerticalFrame);
	   fCompositeFrame828->SetLayoutBroken(kTRUE);

	   // list box
	   lbModules = new TGListBox(fCompositeFrame828);
	   lbModules->Resize(888,180);
	   fCompositeFrame828->AddFrame(lbModules, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   lbModules->MoveResize(0,592,888,180);
	   lbModules->Connect("Selected(Int_t, Int_t)","MvdEventDisplay",this,"DrawSelectedModule(Int_t, Int_t)");
	   
	   btnStartAna = new TGTextButton(fCompositeFrame828,"Start Analysis");
	   btnStartAna->SetTextJustify(36);
	   btnStartAna->Resize(99,24);
	   fCompositeFrame828->AddFrame(btnStartAna, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   btnStartAna->MoveResize(8,776,99,24);
	   btnStartAna->Connect("Pressed()","MvdEventDisplay",this,"DoStartButton()");

	   
	   btnEventNr = new TGTextButton(fCompositeFrame828,"Ana Event Nr");
	   btnEventNr->SetTextJustify(36);
	   btnEventNr->Resize(99,24);
	   fCompositeFrame828->AddFrame(btnEventNr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   btnEventNr->MoveResize(416,808,99,24);
	   btnEventNr->Connect("Pressed()","MvdEventDisplay",this,"DoEventButton()");
	   
	   btnNextEvent = new TGTextButton(fCompositeFrame828,"Next Event");
	   btnNextEvent->SetTextJustify(36);
	   btnNextEvent->Resize(99,24);
	   fCompositeFrame828->AddFrame(btnNextEvent, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   btnNextEvent->MoveResize(784,784,99,24);
	   btnNextEvent->Connect("Pressed()","MvdEventDisplay",this,"DoNextButton()");
	   
	   btnAnaEvents = new TGTextButton(fCompositeFrame828,"Ana Events");
	   btnAnaEvents->SetTextJustify(36);
	   btnAnaEvents->Resize(92,24);
	   fCompositeFrame828->AddFrame(btnAnaEvents, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   btnAnaEvents->MoveResize(416,776,92,24);
	   btnAnaEvents->Connect("Pressed()","MvdEventDisplay",this, "DoAnaEventsButton()");
		 
	   netEvents = new TGNumberEntry(fCompositeFrame828, (Double_t) 0,10,-1,(TGNumberFormat::EStyle) 5);
		fCompositeFrame828->AddFrame(netEvents, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
		netEvents->MoveResize(520,776,88,22);


	   TGFont *ufont;         // will reflect user font changes
	   ufont = gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

	   TGGC   *uGC;           // will reflect user GC changes
	   // graphics context changes
	   GCValues_t valEntry880;
	   valEntry880.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
	   gClient->GetColorByName("#000000",valEntry880.fForeground);
	   gClient->GetColorByName("#c0c0c0",valEntry880.fBackground);
	   valEntry880.fFillStyle = kFillSolid;
	   valEntry880.fFont = ufont->GetFontHandle();
	   valEntry880.fGraphicsExposures = kFALSE;
	   uGC = gClient->GetGC(&valEntry880, kTRUE);
	   
	   edtFileName = new TGTextEntry(fCompositeFrame828, new TGTextBuffer(15),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
	   edtFileName->SetMaxLength(255);
	   edtFileName->SetAlignment(kTextLeft);
	   edtFileName->SetText("");
	   edtFileName->Resize(344,edtFileName->GetDefaultHeight());
	   fCompositeFrame828->AddFrame(edtFileName, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   edtFileName->MoveResize(104,776,288,24);
	   
	   netEventNr = new TGNumberEntry(fCompositeFrame828, (Double_t) 0,9,-1,(TGNumberFormat::EStyle) 5);
	   fCompositeFrame828->AddFrame(netEventNr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   netEventNr->MoveResize(528,808,83,24);

	   // tab widget
	   TGTab *tabBox = new TGTab(fCompositeFrame828,886,576);

	   // container of "Tab1"
	   TGCompositeFrame *cfTab1;
	   cfTab1 = tabBox->AddTab("3D-Event");
	   cfTab1->SetLayoutManager(new TGVerticalLayout(cfTab1));
	   cfTab1->SetLayoutBroken(kTRUE);

	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame1265 = new TGHorizontalFrame(cfTab1,872,549,kHorizontalFrame);
	   fHorizontalFrame1265->SetLayoutBroken(kTRUE);

	   // embedded canvas
	   ecTab1Main = new TRootEmbeddedCanvas(0,fHorizontalFrame1265,640,544);
	   Int_t wecTab1Main = ecTab1Main->GetCanvasWindowId();
	   TCanvas *canTab1Main = new TCanvas("canTab1Main", 10, 10, wecTab1Main);
	   ecTab1Main->AdoptCanvas(canTab1Main);
	   fHorizontalFrame1265->AddFrame(ecTab1Main, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   ecTab1Main->MoveResize(0,0,640,544);

	   // vertical frame
	   TGVerticalFrame *fVerticalFrame1294 = new TGVerticalFrame(fHorizontalFrame1265,222,536,kVerticalFrame);
	   fVerticalFrame1294->SetLayoutBroken(kTRUE);

	   // embedded canvas
	   ecTab1First = new TRootEmbeddedCanvas(0,fVerticalFrame1294,216,264);
	   Int_t wecTab1First = ecTab1First->GetCanvasWindowId();
	   TCanvas *canTab1First = new TCanvas("canTab1First", 10, 10, wecTab1First);
	   ecTab1First->AdoptCanvas(canTab1First);
	   fVerticalFrame1294->AddFrame(ecTab1First, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   ecTab1First->MoveResize(0,264,216,264);

	   // embedded canvas
	   ecTab1Second = new TRootEmbeddedCanvas(0,fVerticalFrame1294,220,252);
	   Int_t wecTab1Second = ecTab1Second->GetCanvasWindowId();
	   TCanvas *canTab1Second = new TCanvas("canTab1Second", 10, 10, wecTab1Second);
	   ecTab1Second->AdoptCanvas(canTab1Second);
	   fVerticalFrame1294->AddFrame(ecTab1Second, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   ecTab1Second->MoveResize(0,2,220,252);

	   fHorizontalFrame1265->AddFrame(fVerticalFrame1294, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   fVerticalFrame1294->MoveResize(648,10,222,536);

	   cfTab1->AddFrame(fHorizontalFrame1265, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   fHorizontalFrame1265->MoveResize(0,0,872,549);


	   // container of "Tab2"
	   TGCompositeFrame *fCompositeFrame1239;
	   fCompositeFrame1239 = tabBox->AddTab("One Module");
	   fCompositeFrame1239->SetLayoutManager(new TGVerticalLayout(fCompositeFrame1239));
	   fCompositeFrame1239->SetLayoutBroken(kTRUE);

	   // embedded canvas
	   ecTab2Main = new TRootEmbeddedCanvas(0,fCompositeFrame1239,864,544);
	   Int_t wecTab2Main = ecTab2Main->GetCanvasWindowId();
	   TCanvas *canTab2Main = new TCanvas("canTab2Main", 10, 10, wecTab2Main);
	   canTab2Main->Divide(2,3);
	   ecTab2Main->AdoptCanvas(canTab2Main);
	   fCompositeFrame1239->AddFrame(ecTab2Main, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   ecTab2Main->MoveResize(8,0,864,544);


	   tabBox->SetTab(1);

	   tabBox->Resize(tabBox->GetDefaultSize());
	   fCompositeFrame828->AddFrame(tabBox, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   tabBox->MoveResize(2,8,886,576);

	   fCompositeFrame827->AddFrame(fCompositeFrame828, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	   fCompositeFrame828->MoveResize(16,0,929,848);

	   fMainFrame826->AddFrame(fCompositeFrame827, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	   fCompositeFrame827->MoveResize(0,0,981,859);

	   mainFrame->AddFrame(fMainFrame826, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	   fMainFrame826->MoveResize(0,0,981,859);

	   mainFrame->MapSubwindows();

	   mainFrame->Resize(mainFrame->GetDefaultSize());
	   mainFrame->MapWindow();
	   mainFrame->Resize(959,859);
	   
	   gStyle->SetPalette(1);
}

void MvdEventDisplay::DoStartButton()
{
	ana = new MvdAllDataEventAna(edtFileName->GetText());

}

void MvdEventDisplay::DoEventButton()
{
	ana->AnaEventNr(netEventNr->GetIntNumber());
	ana->DrawHisxy(ecTab1First->GetCanvas(),0);
	ana->DrawHisrz(ecTab1Second->GetCanvas(),0);
	ana->DrawTopVolume(ecTab1Main->GetCanvas(),0,"");//ogl draws detector in extra window
	ana->DrawHitTracks(ecTab1Main->GetCanvas());
	
	ecTab1Main->GetCanvas()->Modified();
	ecTab1Main->GetCanvas()->Update();
	ecTab1First->GetCanvas()->Modified();
	ecTab1First->GetCanvas()->Update();
	ecTab1Second->GetCanvas()->Modified();
	ecTab1Second->GetCanvas()->Update();
	
	FillModulesListBox();
}

void MvdEventDisplay::DoNextButton()
{
	std::cout << "NextButton pushed" << std::endl;
	ana->NextEvent();
	ana->DrawHisxy(ecTab1First->GetCanvas(),0);
	ana->DrawHisrz(ecTab1Second->GetCanvas(),0);
	ana->DrawTopVolume(ecTab1Main->GetCanvas());
	ana->DrawHitTracks(ecTab1Main->GetCanvas());
	
	ecTab1Main->GetCanvas()->Modified();
	ecTab1Main->GetCanvas()->Update();
	ecTab1First->GetCanvas()->Modified();
	ecTab1First->GetCanvas()->Update();
	ecTab1Second->GetCanvas()->Modified();
	ecTab1Second->GetCanvas()->Update();
	
	netEventNr->SetIntNumber(ana->_activeEvent);
	
	FillModulesListBox();

}

void MvdEventDisplay::DoAnaEventsButton()
{
	ana->AnaEvents(netEvents->GetIntNumber());
	ana->DrawTopVolume(ecTab1Main->GetCanvas());
	FillModulesListBox();
	ana->DrawHitPerCluster(ecTab1First->GetCanvas());
	ana->DrawResolution(ecTab1Second->GetCanvas());

	ecTab1First->GetCanvas()->Modified();
	ecTab1First->GetCanvas()->Update();
	ecTab1Second->GetCanvas()->Modified();
	ecTab1Second->GetCanvas()->Update();
}

void MvdEventDisplay::FillModulesListBox()
{
	ana->PrintHitArray();
	lbModules->RemoveAll();
	std::vector<TString> moduleNames = ana->GetModulesHit();
	
	for (int i = 0; i < moduleNames.size(); i++){
		lbModules->NewEntry(moduleNames[i]);
	}
//	lbModules->NewEntry("Test");
	lbModules->MapSubwindows();
	lbModules->Layout();
}

void MvdEventDisplay::DrawSelectedModule(Int_t lbID, Int_t itemID)
{
	TGTextLBEntry* lbText = (TGTextLBEntry*)lbModules->GetEntry(itemID);
	TString name;
	name = lbText->GetText()->GetString();
	std::cout << "Name: " << name << " ID: " << ana->geoH->GetID(name) << std::endl;
	ana->DrawAllHistos(ana->geoH->GetID(name), ecTab2Main->GetCanvas());
	gGeoManager->cd(name);
	
	TGeoHMatrix pos;
	pos = *(gGeoManager->GetCurrentMatrix());
	ana->_geoList->GetHitList("StripHitsGreen")->SetAllInvisible();
	ana->_geoList->GetHitList("PixHits4Green")->SetAllInvisible();
	ana->_geoList->GetHitList("PixHits8Green")->SetAllInvisible();
		
	if (ana->geoH->GetID(name).Contains("71")){
		//ana->_geoList->GetHitList("StripHits")->SetVisibility(pos, kFALSE);
		ana->_geoList->GetHitList("StripHitsGreen")->SetVisibility(pos, kTRUE);
	}
	else if (ana->geoH->GetID(name).Contains("89")){
		//ana->_geoList->GetHitList("PixHits4")->SetVisibility(pos, kFALSE);
		ana->_geoList->GetHitList("PixHits4Green")->SetVisibility(pos, kTRUE);
	}
	else if (ana->geoH->GetID(name).Contains("56")){
		//ana->_geoList->GetHitList("PixHits8")->SetVisibility(pos, kFALSE);
		ana->_geoList->GetHitList("PixHits8Green")->SetVisibility(pos, kTRUE);
	}
	ecTab1Main->GetCanvas()->Modified();
	ecTab1Main->GetCanvas()->Update();
		
}