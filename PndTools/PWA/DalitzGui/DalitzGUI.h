// --------------------------------------------------------------------------
// DalitzGui - ROOT based tool to plot 3body decay's with resonant behaviour
// --------------------------------------------------------------------------
// 
// Check accompanying README file for infos how to use
//
// Original author: Klaus Goetzen - GSI Darmstadt
// Last modified  : 2014/08/08
// --------------------------------------------------------------------------

#include "TRandom3.h"
#include <TQObject.h>
#include <RQ_OBJECT.h>
//#include "stdafx.h"

class TH2F;
class TRootEmbeddedCanvas;
class TGLayoutHints;
class TH1D;
class TGHorizontalFrame;
class TGVerticalFrame;
class TGLabel;
class TGHSlider;
class TGTextButton;
class TGCheckButton;
class TGTextEntry;
class TGTextBuffer;
class TGComboBox;
class TGraph;
class CRes;
class TGMainFrame;
class TGWindow;
class TGMenuBar;
class TGPopupMenu;
class TGDockableFrame;
class TGTransientFrame;
class TGDoubleHSlider;
class TGButton;

class TDalitzGui {
RQ_OBJECT("TDalitzGui")

private:
    TGMainFrame			*fMain;
   	TRootEmbeddedCanvas *fCanvas;
   	TGLayoutHints       *fLcan;
   	
   	TH2F                *fHDalitz;    // coherent (correct) Dalitz Plot
   	TH2F                *fHDalitzSct; // coherent (correct) Dalitz Plot (scatter plot)
   	TH2F                *fHDalitzI;   // incoherent Dalitz Plot
   	TH2F                *fHDalitzD;   // difference coherent-incoherent
   	TH2F                *fHDalitzPh;  // total absoluted phase of resulting amplitude
   	
   	TH1D                *fHs1Proj, *fHs2Proj,*fHs3Proj;  // 1D projections of squared masses
   	TH1D                *fHs1M, *fHs2M,*fHs3M;           // 1D projections of inv. masses (generated randomly from projections)
   	
    TGMenuBar           *fMenuBar;
    TGPopupMenu         *fMenuFile;
    TGDockableFrame     *fMenuDock;
  
	TGHorizontalFrame   *fVframe[4];
	//TGHorizontalFrame   *fVframe0, *fVframe1, *fVframe2, *fVframe3;
	TGLayoutHints       *fBly,*fBfly1,*fBfly2,*fBfly3, *fMenuBarItemLayout, *fMenuBarLayout;
	TGLabel				*fLab[7];
	//TGLabel             *fLab1, *fLab2, *fLab3, *fLab4, *fLab5, *fLab6, *fLab7;
	TGHSlider     	   	*fHslider1, *fHslider2, *fHslider3;
   	TGTextButton     	*fButResize;
   	TGCheckButton       *fCheckResize, *fCheckColor, *fCheckLog;
   	
	TGTextEntry         *fTeh1;
	TGTextBuffer        *fTbh1;
	TGTextEntry         *fTeh2;
	TGTextBuffer        *fTbh2;
	
	TGLabel				*fLabRN[3], *fLabRA[6], *fLabRph[6]; 
	
	TGComboBox			*fComRes[6];
	TGCheckButton       *fChRes[6];
	
	TGHSlider			*fSlResA[6], *fSlResph[6];

	TGHSlider           *fSlOcc;
	
	TGComboBox			*fComFin[3];

	TGComboBox			*fComPlot;

	TGraph              *fGraphKin;		
			
	CRes				*fRes[6];
	
	Int_t				fnR[3];

	//
	Double_t 			fM;
	Double_t 			fm1;
	Double_t 			fm2;
	Double_t 			fm3;

	TString				fn1, fn2, fn3; // Print names of particles
	
	Int_t				ndbins;
	
	TRandom3			fRand;
	Int_t               fOcc;
	
	TString				fActiveRes;
	
public:
   TDalitzGui(const TGWindow *p,UInt_t w,UInt_t h);
   virtual ~TDalitzGui();

	// signal handler
   void CloseWindow();
   void DoSlider(Int_t pos);
   void DoParmSlider(Int_t pos);
   void DoResize();
   void DoText();
   void DoCombo(Int_t id);
   void DoCheckRes() { DoParmSlider(0); }
   void HandleMenu(Int_t id);
   void Rebin();
   Int_t IdToSelection(Int_t id, int mode=1);
   
   //other functions
   void CreateGraph(TGraph* g, const Int_t n, const Double_t m, const Double_t m1, const Double_t m2, const Double_t m3);
   void CreateDalitz();
   void InitParams();
   void DrawMainPlot();
   void DrawPlot();
   void ReadOutGui();
   void ComputeCache(Int_t ri=1);
   void MakeAnimatedGif();
      
   void ConfigPlot();
   void ConfigComboFin(TGComboBox* b);
   void ConfigComboRes(TGComboBox* b);
   void ConfigSliderRes(TGHSlider *a, TGHSlider *p);//, TGHSlider *J);
   
   void LoadConfiguration(TString filename);
   void SaveConfiguration(TString filename);
   
   void SwitchColor();
   //void HandleButtons();
   

   ClassDef(TDalitzGui, 0)
};

class AniGifDialog {

private:
	TGTransientFrame     *fMain;
 	
 	TGHorizontalFrame    *fVframe0, *fVframe1;
	TGLayoutHints        *fL1, *fL2;

	TGLabel              *fLab[10];
	
	TGTextEntry          *fTeh1;
	TGTextBuffer         *fTbh1;
	TGTextEntry          *fTeh2;
	TGTextBuffer         *fTbh2;

	TGComboBox			 *fComPlot, *fComVar;

	TGDoubleHSlider      *fHslider1;
	
	TGButton             *fOkButton;
	TGButton             *fCancelButton;

public:
	AniGifDialog(const TGWindow *p, const TGWindow *main);
	virtual ~AniGifDialog();

	// slots
	void CloseWindow();
	void DoOK();
	void DoCombo(Int_t idx);
	void DoSlider();
	
private:
	double slmin, slmax;
   	
   	ClassDef(AniGifDialog, 0)
};
