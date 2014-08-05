#include "TRandom3.h"
#include <TQObject.h>
#include <RQ_OBJECT.h>
//#include "stdafx.h"

class TH2F;
class TRootEmbeddedCanvas;
class TGLayoutHints;
class TH1D;
class TGHorizontalFrame;
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

class TDalitzGui {
RQ_OBJECT("TDalitzGui")

private:
    TGMainFrame			*fMain;
   	TRootEmbeddedCanvas *fCanvas;
   	TGLayoutHints       *fLcan;
   	
   	TH2F                *fHDalitz;    // coherent (correct) Dalitz Plot
   	TH2F                *fHDalitzI;   // incoherent Dalitz Plot
   	TH2F                *fHDalitzD;   // difference coherent-incoherent
   	TH2F                *fHDalitzPh;  // total absoluted phase of resulting amplitude
   	
   	TH1D                *fHs1Proj, *fHs2Proj,*fHs3Proj;
   	TH1D                *fHs1M, *fHs2M,*fHs3M;
   	
    TGMenuBar           *fMenuBar;
    TGPopupMenu         *fMenuFile;
    TGDockableFrame     *fMenuDock;
  
	TGHorizontalFrame   *fVframe0, *fVframe1, *fVframe2, *fVframe3;
	TGLayoutHints       *fBly,*fBfly1,*fBfly2,*fBfly3, *fMenuBarItemLayout, *fMenuBarLayout;
	TGLabel             *fLab1, *fLab2, *fLab3, *fLab4, *fLab5, *fLab6;
	TGHSlider     	   	*fHslider1, *fHslider2, *fHslider3;
   	TGTextButton     	*fButResize;
   	TGCheckButton       *fCheckResize,*fCheckColor;
   	
	TGTextEntry         *fTeh1;
	TGTextBuffer        *fTbh1;
	TGTextEntry         *fTeh2;
	TGTextBuffer        *fTbh2;
	
	TGLabel             *fLabR1N, *fLabR1A, *fLabR1ph, *fLabR2A, *fLabR2ph;
	TGLabel             *fLabR3N, *fLabR3A, *fLabR3ph, *fLabR4A, *fLabR4ph;
	TGLabel             *fLabR5N, *fLabR5A, *fLabR5ph, *fLabR6A, *fLabR6ph;
	TGLabel             *fLabR1J, *fLabR2J, *fLabR3J, *fLabR4J, *fLabR5J, *fLabR6J;
	
	TGComboBox          *fComRes1, *fComRes2, *fComRes3;
	TGComboBox          *fComRes4, *fComRes5, *fComRes6;
	
  	TGCheckButton       *fChRes1, *fChRes2, *fChRes3;
  	TGCheckButton       *fChRes4, *fChRes5, *fChRes6;
	
	TGHSlider           *fSlRes1A, *fSlRes1ph, *fSlRes1J;
	TGHSlider           *fSlRes2A, *fSlRes2ph, *fSlRes2J;
	TGHSlider           *fSlRes3A, *fSlRes3ph, *fSlRes3J;
	TGHSlider           *fSlRes4A, *fSlRes4ph, *fSlRes4J;
	TGHSlider           *fSlRes5A, *fSlRes5ph, *fSlRes5J;
	TGHSlider           *fSlRes6A, *fSlRes6ph, *fSlRes6J;
	
	TGHSlider           *fSlBins;
	
	TGComboBox          *fComFin1, *fComFin2, *fComFin3;

	TGComboBox			*fComPlot;

	TGraph              *fGraphKin;		
			
	CRes			   	*fRes1[2];
	CRes			   	*fRes2[2];
	CRes			   	*fRes3[2];
	
	Int_t 				fnR1;
	Int_t 				fnR2;
	Int_t 				fnR3;
	
	Double_t 			fM;
	Double_t 			fm1;
	Double_t 			fm2;
	Double_t 			fm3;

	TString				fn1, fn2, fn3; // Print names of particles
	
	Int_t				ndbins;
	
	TRandom3			fRand;
	
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
   
   //other functions
   void CreateGraph(TGraph* g, const Int_t n, const Double_t m, const Double_t m1, const Double_t m2, const Double_t m3);
   void CreateDalitz();
   double GetBinFraction(int ix, int iy, double &s1, double &s2);
   int IsInPhsp(int ix, int iy);
   void InitParams();
   void DrawPlot();
   void ReadOutGui();
   void ComputeCache(Int_t ri=1);
      
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

