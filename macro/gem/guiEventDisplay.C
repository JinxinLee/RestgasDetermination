#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TObjArray.h>
#include <TPolyLine.h>
#include <TRootEmbeddedCanvas.h>
#include <TStopwatch.h>
#include <RQ_OBJECT.h>

Double_t fWindowSize = 1000.;

class MyMainFrame {

  RQ_OBJECT("MyMainFrame")


 private:
  TGMainFrame *fMain;

  TRootEmbeddedCanvas *fEcanvas;

  TGStatusBar *fInfoStatusBar;

  TGNumberEntryField *fPSizeEntry;
  TGNumberEntryField *fPCenXEntry;
  TGNumberEntryField *fPCenYEntry;
  TGNumberEntryField *fPCenZEntry;

  TGNumberEntryField *fEventToDrawEntry;
  TGNumberEntryField *fPointToDrawEntry;

  TStopwatch        fTimer;

  Double_t fPlotSize;
  Double_t fPlotCenterX;
  Double_t fPlotCenterY;
  Double_t fPlotCenterZ;

  Int_t fDrawStation;
  Int_t fNofStations;

  TObjArray* fRealStations;
  TObjArray* fStations;

  Double_t fStationsInnerRadius[10];
  Double_t fStationsOuterRadius[10];
  Double_t fStationsZPosition  [10];

  Int_t fDigitSchemeToDraw;

  Int_t fNofGemPoints;
  Int_t fNofMCTracks;
  Int_t fNofGemHits;
  Int_t fNofGemDigis;

  TString fBaseString;
  TString fParFString;

  TFile*   fSimFile;
  TTree*   fSimTree;
  TClonesArray *fGemPointArray;
  TClonesArray *fMCTrackArray;

  TFile*   fDigiFile;
  TTree*   fDigiTree;
  TFile*   fHitsFile;
  TTree*   fHitsTree;
  TClonesArray *fGemHitArray;
  TClonesArray *fGemDigiArray;

  PndGemMCPoint* fGemPoint;

  Bool_t fSimRecoFilesRead;

public:
  MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~MyMainFrame();
  
  void DrawPoints();
  void DrawDigis();
  void DrawHitErrors();
  void DrawHits();
  void DoDraw();

  void ReadParameters();

  void SetDigitSchemeToDraw(Int_t itemp) {fDigitSchemeToDraw = itemp-1; DoDraw(); };

  void ReadEvent();
  void ChangeBaseString (char* carr) { fBaseString  = Form("%s",carr); };
  void ChangeParFString (char* carr) { fParFString  = Form("%s",carr); };

  void ChangePlotParams  ();
  void ChangePlotSize    (char* tempD) { fPlotSize    = atof(tempD); };
  void ChangePlotCenterX (char* tempD) { fPlotCenterX = atof(tempD); };
  void ChangePlotCenterY (char* tempD) { fPlotCenterY = atof(tempD); };
  void ChangePlotCenterZ (char* tempD) { fPlotCenterZ = atof(tempD); };

  void MoveUP   () { fPlotCenterY+=fPlotSize/ 2.; DoDraw(); ChangePlotParams(); }
  void Moveup   () { fPlotCenterY+=fPlotSize/10.; DoDraw(); ChangePlotParams(); }
  void MoveDOWN () { fPlotCenterY-=fPlotSize/ 2.; DoDraw(); ChangePlotParams(); }
  void Movedown () { fPlotCenterY-=fPlotSize/10.; DoDraw(); ChangePlotParams(); }
  void MoveLEFT () { fPlotCenterX-=fPlotSize/ 2.; DoDraw(); ChangePlotParams(); }
  void Moveleft () { fPlotCenterX-=fPlotSize/10.; DoDraw(); ChangePlotParams(); }
  void MoveRIGHT() { fPlotCenterX+=fPlotSize/ 2.; DoDraw(); ChangePlotParams(); }
  void Moveright() { fPlotCenterX+=fPlotSize/10.; DoDraw(); ChangePlotParams(); }

  void ZoomIn   () { fPlotSize    =fPlotSize/ 2.; DoDraw(); ChangePlotParams(); }
  void ZoomOut  () { fPlotSize    =fPlotSize* 2.; DoDraw(); ChangePlotParams(); }

  void PrevStat () {   
    if ( fDrawStation == 0 ) fDrawStation = fNofStations;
    fDrawStation = fDrawStation - 1;
    DoDraw(); ChangePlotParams();
  }
  void NextStat () {
    fDrawStation = fDrawStation + 1;
    if ( fDrawStation == fNofStations ) fDrawStation = 0;
    DoDraw(); ChangePlotParams();
  }

  void DoInfoStatusBar(Int_t event, Int_t x, Int_t y, TObject *sel);

  void SaveCanvas();

};

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {

  // Create a main frame
  fSimRecoFilesRead = kFALSE;

  fMain = new TGMainFrame(p,w,h);

  ChangeBaseString("Gem_3Stations_15GeV_n1000");
  ChangeParFString("$VMCWORKDIR/macro/params/gem_3Stations.digi.par");

  fDigitSchemeToDraw = 0;

  ReadParameters();

  fDrawStation = 0;
  /*  fNofStations = 4;
  fStationsInnerRadius[0] =  5.0;  fStationsInnerRadius[1] =   5.0;  fStationsInnerRadius[2] =   5.0;  fStationsInnerRadius[3] =   5.0;
  fStationsOuterRadius[0] = 42.0;  fStationsOuterRadius[1] =  42.0;  fStationsOuterRadius[2] =  66.0;  fStationsOuterRadius[3] =  90.0;
  fStationsZPosition  [0] = 90.0;  fStationsZPosition  [1] = 120.0;  fStationsZPosition  [2] = 150.0;  fStationsZPosition  [3] = 180.0;*/

  ChangePlotSize    ("100.0");
  ChangePlotCenterX ("0.0");
  ChangePlotCenterY ("0.0");
  ChangePlotCenterZ ("0.0");

  // Create canvas widget
  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,fWindowSize*1.2,fWindowSize);

  TGVerticalFrame *v1frame = new TGVerticalFrame(fMain,fWindowSize*.9,fWindowSize);

  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",v1frame,fWindowSize*.8,fWindowSize*.8);
  v1frame->AddFrame(fEcanvas, new TGLayoutHints(kLHintsLeft| kLHintsTop,
						1,1,1,1));

  hframe->AddFrame(v1frame, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  
  TGVerticalFrame *v2frame = new TGVerticalFrame(hframe,fWindowSize*.25,fWindowSize*1);

  fScanvas = new TRootEmbeddedCanvas("Scanvas",v2frame,fWindowSize*.25,fWindowSize*.25);
  v2frame->AddFrame(fScanvas, new TGLayoutHints(kLHintsCenterX| kLHintsTop,
					       1,1,1,1));

   TGHorizontalFrame *h2frame = new TGHorizontalFrame(v2frame,fWindowSize*.1,fWindowSize*.300);

  TGVerticalFrame *v2bframe = new TGVerticalFrame(v2frame,fWindowSize*.050,fWindowSize*.500);

  TGTextEntry *spStText = new TGTextEntry(v2bframe,fBaseString.Data());
  v2bframe->AddFrame(spStText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  spStText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeBaseString(char*)");

  TGTextEntry *teParFText = new TGTextEntry(v2bframe,fParFString.Data());
  v2bframe->AddFrame(teParFText, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  teParFText->Connect("TextChanged(char*)","MyMainFrame",this,"ChangeParFString(char*)");

//   TGCompositeFrame* frameEventPoint = new TGCompositeFrame(v2bframe);
//   frameEventPoint->SetLayoutManager(new TGMatrixLayout(frameEventPoint,2,3,0));
//   v2bframe->AddFrame(frameEventPoint, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGHorizontalFrame *eventFrame = new TGHorizontalFrame(v2bframe,fWindowSize*.1,fWindowSize*.300);
  v2bframe->AddFrame(eventFrame, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,2,2,2,2));
  
  TGTextButton *readEv = new TGTextButton(eventFrame,"&Draw event");
  readEv->Connect("Clicked()","MyMainFrame",this,"ReadEvent()");
  eventFrame->AddFrame(readEv, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fEventToDrawEntry = new TGNumberEntryField(eventFrame,-1,0,
				       TGNumberFormat::kNESInteger);
  eventFrame->AddFrame(fEventToDrawEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGHorizontalFrame *pointFrame = new TGHorizontalFrame(v2bframe,fWindowSize*.1,fWindowSize*.300);
  v2bframe->AddFrame(pointFrame, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,2,2,2,2));

  TGTextButton *readPnt = new TGTextButton(pointFrame,"&Draw point");
  readPnt->Connect("Clicked()","MyMainFrame",this,"ReadPoint()");
  pointFrame->AddFrame(readPnt, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  fPointToDrawEntry = new TGNumberEntryField(pointFrame,-1,0,
				       TGNumberFormat::kNESInteger);
  pointFrame->AddFrame(fPointToDrawEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  v2frame->AddFrame(v2bframe, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,2,2,2,2));

  TGVerticalFrame *v2cframe = new TGVerticalFrame(h2frame,fWindowSize*.050,fWindowSize*.500);

  TGTextButton *saveCanv = new TGTextButton(v2cframe,"&Save canvas");
  saveCanv->Connect("Clicked()","MyMainFrame",this,"SaveCanvas()");
  v2cframe->AddFrame(saveCanv, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGTextButton *exit = new TGTextButton(v2cframe,"&Exit",
					"gApplication->Terminate(0)");
  v2cframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  // TGButtonGroup *digitBGroup = new TGButtonGroup(v2cframe,"Digitization layer",kVerticalFrame);
  // v2cframe->AddFrame(digitBGroup,new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  // TGRadioButton *digitButton[4];
  // digitButton[0] = new TGRadioButton(digitBGroup,"theta strips");
  // digitButton[1] = new TGRadioButton(digitBGroup,"theta+ strips");
  // digitButton[2] = new TGRadioButton(digitBGroup,"theta- strips");
  // digitButton[3] = new TGRadioButton(digitBGroup,"radial strips");
  // digitButton[fDigitSchemeToDraw]->SetState(kButtonDown);
  // digitBGroup->Connect("Pressed(Int_t)","MyMainFrame",this,"SetDigitSchemeToDraw(Int_t)");

  h2frame->AddFrame(v2cframe, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,2,2,2,2));

  v2frame->AddFrame(h2frame, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,2,2,2,2));

  TGGroupFrame *posizeFr = new TGGroupFrame(v2frame, "Size and positions", kVerticalFrame);
  posizeFr->SetTitlePos(TGGroupFrame::kRight); // right aligned
  v2frame->AddFrame(posizeFr, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  posizeFr->SetLayoutManager(new TGMatrixLayout(posizeFr, 0, 2, 10));

  posizeFr->AddFrame(new TGLabel(posizeFr, new TGHotString("Size, cm")));
  fPSizeEntry = new TGNumberEntryField(posizeFr,-1,fPlotSize,
				      TGNumberFormat::kNESRealThree);
  posizeFr->AddFrame(fPSizeEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fPSizeEntry->Connect("TextChanged(char*)","MyMainFrame",this,"ChangePlotSize(char*)");
  
  posizeFr->AddFrame(new TGLabel(posizeFr, new TGHotString("X center, cm")));
  fPCenXEntry = new TGNumberEntryField(posizeFr,-1,fPlotCenterX,
							  TGNumberFormat::kNESRealThree);
  posizeFr->AddFrame(fPCenXEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fPCenXEntry->Connect("TextChanged(char*)","MyMainFrame",this,"ChangePlotCenterX(char*)");

  posizeFr->AddFrame(new TGLabel(posizeFr, new TGHotString("Y center, cm")));
  fPCenYEntry = new TGNumberEntryField(posizeFr,-1,fPlotCenterY,
							  TGNumberFormat::kNESRealThree);
  posizeFr->AddFrame(fPCenYEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fPCenYEntry->Connect("TextChanged(char*)","MyMainFrame",this,"ChangePlotCenterY(char*)");

  posizeFr->AddFrame(new TGLabel(posizeFr, new TGHotString("Z center, cm")));
  fPCenZEntry = new TGNumberEntryField(posizeFr,-1,fPlotCenterZ,
							  TGNumberFormat::kNESRealThree);
  posizeFr->AddFrame(fPCenZEntry, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fPCenZEntry->Connect("TextChanged(char*)","MyMainFrame",this,"ChangePlotCenterZ(char*)");

  TGGroupFrame *horMframe = new TGGroupFrame(v2frame, "Navigation", kHorizontalFrame);
  horMframe->SetTitlePos(TGGroupFrame::kRight); // right aligned
  //  TGHorizontalFrame *horMframe = new TGHorizontalFrame(v2frame,100,300);

  TGVerticalFrame *vHMframe = new TGVerticalFrame(horMframe,fWindowSize*.040,fWindowSize*.300);

  TGTextButton *moveUPBut = new TGTextButton(vHMframe,"UP");
  moveUPBut->Connect("Clicked()","MyMainFrame",this,"MoveUP()");
  moveUPBut->SetToolTipText("move up by half frame");
  vHMframe->AddFrame(moveUPBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  moveUPBut->Resize(100,50);
  TGTextButton *moveupBut = new TGTextButton(vHMframe,"up");
  moveupBut->Connect("Clicked()","MyMainFrame",this,"Moveup()");
  moveupBut->SetToolTipText("move up by tenth frame");
  vHMframe->AddFrame(moveupBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));

  TGHorizontalFrame *butVHMframe = new TGHorizontalFrame(vHMframe,fWindowSize*.040,fWindowSize*.300);

  TGTextButton *moveLEFTBut = new TGTextButton(butVHMframe,"LEFT");
  moveLEFTBut->Connect("Clicked()","MyMainFrame",this,"MoveLEFT()");
  moveLEFTBut->SetToolTipText("move left by half frame");
  butVHMframe->AddFrame(moveLEFTBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  TGTextButton *moveleftBut = new TGTextButton(butVHMframe,"left");
  moveleftBut->Connect("Clicked()","MyMainFrame",this,"Moveleft()");
  moveleftBut->SetToolTipText("move left by tenth frame");
  butVHMframe->AddFrame(moveleftBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  TGTextButton *drawBut = new TGTextButton(butVHMframe,"&Draw");
  drawBut->Connect("Clicked()","MyMainFrame",this,"DoDraw()");
  butVHMframe->AddFrame(drawBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  TGTextButton *moverightBut = new TGTextButton(butVHMframe,"right");
  moverightBut->Connect("Clicked()","MyMainFrame",this,"Moveright()");
  moverightBut->SetToolTipText("move right by tenth frame");
  butVHMframe->AddFrame(moverightBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  TGTextButton *moveRIGHTBut = new TGTextButton(butVHMframe,"RIGHT");
  moveRIGHTBut->Connect("Clicked()","MyMainFrame",this,"MoveRIGHT()");
  moveRIGHTBut->SetToolTipText("move right by half frame");
  butVHMframe->AddFrame(moveRIGHTBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));

  vHMframe->AddFrame(butVHMframe, new TGLayoutHints(kLHintsCenterX,0,0,0,0));

  TGTextButton *movedownBut = new TGTextButton(vHMframe,"down");
  movedownBut->Connect("Clicked()","MyMainFrame",this,"Movedown()");
  movedownBut->SetToolTipText("move down by tenth frame");
  vHMframe->AddFrame(movedownBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  TGTextButton *moveDOWNBut = new TGTextButton(vHMframe,"DOWN");
  moveDOWNBut->Connect("Clicked()","MyMainFrame",this,"MoveDOWN()");
  moveDOWNBut->SetToolTipText("move down by half frame");
  vHMframe->AddFrame(moveDOWNBut, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
  
  horMframe->AddFrame(vHMframe, new TGLayoutHints(kLHintsCenterX,0,0,0,0));

  TGVerticalFrame *vHM2frame = new TGVerticalFrame(horMframe,fWindowSize*.030,fWindowSize*.300);

  TGTextButton *zoominBut = new TGTextButton(vHM2frame,"Zoom in");
  zoominBut->Connect("Clicked()","MyMainFrame",this,"ZoomIn()");
  vHM2frame->AddFrame(zoominBut, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
  TGTextButton *zoomoutBut = new TGTextButton(vHM2frame,"Zoom out");
  zoomoutBut->Connect("Clicked()","MyMainFrame",this,"ZoomOut()");
  vHM2frame->AddFrame(zoomoutBut, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));

  TGTextButton *prevStBut = new TGTextButton(vHM2frame,"Prev stat");
  prevStBut->Connect("Clicked()","MyMainFrame",this,"PrevStat()");
  vHM2frame->AddFrame(prevStBut, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));
  TGTextButton *nextStBut = new TGTextButton(vHM2frame,"Next stat");
  nextStBut->Connect("Clicked()","MyMainFrame",this,"NextStat()");
  vHM2frame->AddFrame(nextStBut, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,5,5,5,5));

  horMframe->AddFrame(vHM2frame, new TGLayoutHints(kLHintsCenterX|kLHintsCenterY,0,0,0,0));

  v2frame->AddFrame(horMframe, new TGLayoutHints(kLHintsCenterX,0,0,0,0));

  hframe->AddFrame(v2frame, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsLeft,2,2,2,2));

  Int_t parts[] = {5, 6, 6, 4, 4, 4, 4, 4, 15, 18, 30};
  fInfoStatusBar = new TGStatusBar(fMain,50,10,kHorizontalFrame);
  fInfoStatusBar->SetParts(parts,11);
  fMain->AddFrame(fInfoStatusBar,new TGLayoutHints(kLHintsBottom | kLHintsLeft |
						   kLHintsExpandX,0,0,2,0));

  ChangePlotParams();
  DoDraw();
  
  fMain->SetWindowName("Simple Example");
  // Map all subwindows of main frame
  fMain->MapSubwindows();
  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());
  // Map main frame
  fMain->MapWindow();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DoDraw() {
  fTimer.Start();

  TCanvas *fCanvas = fEcanvas->GetCanvas();

  fEcanvas->SetName(Form("Station %d",fDrawStation+1));

  fCanvas->Clear();
  
  //  cout << "plot size " << fPlotSize << " at " << fPlotCenterX << " " << fPlotCenterY << " " << fPlotCenterZ << endl;

  Double_t markerSize = 2.;

  if ( fPlotSize > 1.1 ) markerSize = 1.;

  if ( fPlotSize == 0. ) fPlotSize = 0.001;

  Double_t minX = fPlotCenterX-fPlotSize;  
  Double_t maxX = fPlotCenterX+fPlotSize;  
  Double_t minY = fPlotCenterY-fPlotSize;  
  Double_t maxY = fPlotCenterY+fPlotSize;  

  fCanvas->Range(minX,minY,maxX,maxY);
  fCanvas->SetFillColor(0);
  fCanvas->SetBorderSize(0);
  fCanvas->SetBorderMode(0);
  fCanvas->SetFrameFillColor(0);
  fCanvas->SetGrid(0,0);
  fCanvas->SetLogy(0);

  TEllipse* outerRim = new TEllipse(0.,0.,fStationsOuterRadius[fDrawStation],fStationsOuterRadius[fDrawStation]);
  outerRim->Draw();

  
  if ( fSimRecoFilesRead ) {
    //    cout << "TRYING TO DRAW !!!" << endl;
    DrawHitErrors();
    DrawDigis();
    DrawPoints();
    DrawHits();
  }
  fInfoStatusBar->SetText(Form("Station %d",fDrawStation+1),0);

  TEllipse* innerRim = new TEllipse(0.,0.,fStationsInnerRadius[fDrawStation],fStationsInnerRadius[fDrawStation]);
  innerRim->SetFillStyle(0);
  innerRim->Draw();

  TLatex* tx = new TLatex(.2*minX+.8*maxX,.05*minY+.95*maxY,Form("Station #%d",fDrawStation+1));     tx->SetTextSize(0.03);    tx->Draw();
  Double_t sizeScale = TMath::Ceil(TMath::Log10(fPlotSize));
  TLine* scale = new TLine(.15*minX+.85*maxX-10**(sizeScale-2),.07*minY+.93*maxY,.15*minX+.85*maxX,.07*minY+.93*maxY);
  TLatex* ts = new TLatex(.2*minX+.8*maxX,.1*minY+.9*maxY,Form("%.2f cm",10**(sizeScale-2)));     ts->SetTextSize(0.03);    ts->Draw();
  scale->SetLineWidth(2);
  scale->Draw();
  
  //  TLatex* tc = new TLatex(.2*minX+.8*maxX,.15*minY+.85*maxY,Form("%d channels",nofStrips));     tc->SetTextSize(0.03);    tc->Draw();

  fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  		   "MyMainFrame", this,"DoInfoStatusBar(Int_t,Int_t,Int_t,TObject*)");
  fCanvas->cd();
  fCanvas->Update();
  
  //  cout << " DoDraw() END --> time = " << fTimer.RealTime() << endl;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DrawPoints() {
  Double_t markerSize = 2.;
  if ( fPlotSize > 1.1 ) markerSize = 1.;
  Int_t markerColor = 1;

  PndGemMCPoint* pointToDraw;
  for ( Int_t ipoint = 0 ; ipoint < fNofGemPoints ; ipoint++ ) {
    markerColor = 1; 

    pointToDraw = (PndGemMCPoint*)fGemPointArray->At(ipoint);

    //return stationNr*256+sensorNr*16+segmentNr;

    //  TString stationIdentifier = pointToDraw->GetDetName();    
    Int_t stationNr = pointToDraw->GetSensorId();
    Int_t segmentNr = stationNr%16;
    Int_t sensorNr = (stationNr/16)%16;
    stationNr = stationNr/256;
    //    cout << "point at " << pointToDraw->GetZ() << ", sid = " << pointToDraw->GetSensorId() << " : " << stationNr << " / " << sensorNr << " / " << segmentNr << endl;
    //    if ( !stationIdentifier.Contains(Form("Disk%d",fDrawStation+1)) ) continue;
    if ( stationNr != fDrawStation+1 ) continue;

    TMarker* pointCenter = new TMarker(pointToDraw->GetX(),pointToDraw->GetY(),20);
    pointCenter->SetUniqueID(ipoint);
    //    cout << "set uid to " << ipoint << endl;

    // if ( stationIdentifier.Contains("Gem1") ) markerColor = 3;
    // if ( stationIdentifier.Contains("Pad") ) markerColor = 2;
    // if ( stationIdentifier.Contains("Gem6") ) markerColor = 4; 
    if ( sensorNr == 1 ) markerColor = 3;
   if ( sensorNr == 2 ) markerColor = 4;
    
    pointCenter->SetMarkerColor(markerColor);
    pointCenter->Draw();

    //    if ( stationIdentifier.Contains("Pad") ) {
    if ( sensorNr == 2 ) {
      PndMCTrack* trackSel = (PndMCTrack*)fMCTrackArray->At(pointToDraw->GetTrackID());
      TVector3 startVertex   = trackSel->GetStartVertex();

      Int_t textColor = 2;
      if ( startVertex.Mag() < 0.001 ) textColor = 3;

      TPaveText* trackNoText = new TPaveText(pointToDraw->GetX(),pointToDraw->GetY(),
					     pointToDraw->GetX(),pointToDraw->GetY());
      TText* textNumber = trackNoText->AddText(Form("%d",pointToDraw->GetTrackID()));

      // trackNoText->AddText("center");
      textNumber->SetTextSize(0.05);
      textNumber->SetTextColor(textColor);
      trackNoText->Draw();

      Double_t posY = pointToDraw->GetY();

      TPaveText* trackPdgText = new TPaveText(pointToDraw->GetX(),posY-fPlotSize/20.,
					      pointToDraw->GetX(),posY-fPlotSize/20.);

      trackPdgText->AddText(Form("%d",trackSel->GetPdgCode()));
      trackPdgText->SetTextSize(0.02);
      trackPdgText->SetTextColor(textColor);
      trackPdgText->Draw();

    }

  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DrawDigis() {
  PndGemDigi* digiToDraw;
  PndGemStation* digiStation;
  PndGemSensor* digiSensor;
  Int_t digiStationNr ;
  Int_t digiSensorNr  ;
  Int_t digiSide      ;
  Int_t channelNumber ;

  for ( Int_t idigi = 0 ; idigi < fNofGemDigis ; idigi++ ) {
    //    cout << "drawing digi " << idigi << endl;

    //    if ( idigi < fNofGemDigis ) {
      digiToDraw = (PndGemDigi*)fGemDigiArray->At(idigi);
      digiStationNr   = digiToDraw->GetStationNr();
      digiSensorNr    = digiToDraw->GetSensorNr();
      digiSide      = digiToDraw->GetSide();
      channelNumber = digiToDraw->GetChannelNr();
      /*    }
    else {
      if ( idigi < fNofGemDigis+4 ) {
	digiStationNr   = 1;
	digiSensorNr    = 1+(idigi-fNofGemDigis)/2;
	digiSide      = (idigi-fNofGemDigis)%2;
	channelNumber = 1;
      }
      else {
	digiStationNr   = 1;
	digiSensorNr    = 1+(idigi-4-fNofGemDigis)/2;
	digiSide      = (idigi-4-fNofGemDigis)%2;
	channelNumber = 1047;
      }
      }*/

    //    cout << " digi is in station " << digiStationNr << endl;
    if ( digiStationNr != fDrawStation+1 ) continue;
    //    cout << " digi is in sensor " << flush << digiSensorNr << endl;
    digiStation = (PndGemStation*)fStations->At(fDrawStation);
    digiSensor  = (PndGemSensor*) digiStation->GetSensorByNr(digiSensorNr);
    //    digiSensor->Print();

    if ( digiSensor->GetStripAngle(digiSide) > 89. ) {
      //      cout << "to draw ellipse strip" << endl;
      Double_t rRadius = digiSensor->GetInnerRadius() + channelNumber*digiSensor->GetPitch(digiSide);
      TEllipse* rStrip = new TEllipse(0.,0.,rRadius,rRadius);
      rStrip->SetFillStyle(0);
      rStrip->SetLineColor(2);
      rStrip->Draw("");
      Double_t lRadius = digiSensor->GetInnerRadius() + (channelNumber-1)*digiSensor->GetPitch(digiSide);
      TEllipse* lStrip = new TEllipse(0.,0.,lRadius,lRadius);
      lStrip->SetFillStyle(0);
      lStrip->SetLineColor(2);
      lStrip->Draw("");

    }
    else {
      //      cout << "to draw radial strip" << endl;
      Double_t deltaTheta = TMath::DegToRad()*digiSensor->GetStripAngle(digiSide);
      Double_t innerPStripPitch = digiSensor->GetPitch(digiSide);
    
      //      Double_t innerCirc = digiSensor->GetInnerRadius()*2.*TMath::Pi();
      Int_t nofStrips = (digiSide?digiSensor->GetNChannelsBack():digiSensor->GetNChannelsFront());
    
      //      if (  digiSensor->GetStripAngle(digiSide) == 0. )     cout << "channel number = " << channelNumber << endl;
      Double_t radialStripSpan = TMath::ATan(digiSensor->GetPitch(digiSide)/digiSensor->GetInnerRadius());

      //      Double_t channelTheta = (channelNumber-1) * 2.*TMath::Pi()/((Double_t)(nofStrips));
      Double_t channelTheta = (channelNumber-1) * radialStripSpan;//2.*TMath::Pi()/((Double_t)(nofStrips));
      Double_t cosCT = TMath::Cos(channelTheta);
      Double_t sinCT = TMath::Sin(channelTheta);

      //      Double_t radialStripSpan = 2.*TMath::Pi()/((Double_t)(nofStrips));
      Double_t cosRSS = TMath::Cos(radialStripSpan);
      Double_t sinRSS = TMath::Sin(radialStripSpan);
      
      Double_t xBot = 0;
      Double_t xTop = TMath::Sin(deltaTheta)*digiSensor->GetOuterRadius();
      Double_t yBot = digiSensor->GetInnerRadius();
      Double_t yTop = TMath::Cos(deltaTheta)*digiSensor->GetOuterRadius();

      Double_t xpstr[5];
      Double_t ypstr[5];

      xpstr[0] = xBot*cosCT+yBot*sinCT;
      ypstr[0] =-xBot*sinCT+yBot*cosCT;
      xpstr[1] = xTop*cosCT+yTop*sinCT;
      ypstr[1] =-xTop*sinCT+yTop*cosCT;

      xpstr[2] = xpstr[1]*cosRSS+ypstr[1]*sinRSS;
      ypstr[2] =-xpstr[1]*sinRSS+ypstr[1]*cosRSS;
      xpstr[3] = xpstr[0]*cosRSS+ypstr[0]*sinRSS;
      ypstr[3] =-xpstr[0]*sinRSS+ypstr[0]*cosRSS;
      xpstr[4] = xpstr[0];
      ypstr[4] = ypstr[0];
      
      Int_t colorCode = 4;
      if ( digiSensor->GetStripAngle(digiSide) > 0. ) {
	colorCode = 1;
      }
      if ( digiSensor->GetStripAngle(digiSide) < 0. ) {
	colorCode = 6;
      }

      TPolyLine* strip = new TPolyLine(5,xpstr,ypstr);
      strip->SetUniqueID(666666);
      //      cout << "this strip has id = " << strip->GetUniqueID() << endl;
      strip->SetLineWidth(2);
      strip->SetLineColor(colorCode);
      strip->SetFillColor(colorCode);
      strip->Draw("");
      
    }
    //    cout << "done" << endl;    
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DrawHits() {
  Double_t markerSize = 2.;
  if ( fPlotSize > 1.1 ) markerSize = 1.;
  Int_t markerColor = 1;
  Int_t markerStyle = 29;

  PndGemHit* hitToDraw;
  for ( Int_t ihit = 0 ; ihit < fNofGemHits ; ihit++ ) {
    markerColor = 1; 
    markerStyle = 29;
    markerSize = 3.;
    if ( fPlotSize > 1.1 ) markerSize = 1.5;

    hitToDraw = (PndGemHit*)fGemHitArray->At(ihit);

    Int_t hitStationNr = hitToDraw->GetStationNr();

    if ( hitStationNr != fDrawStation+1 ) continue;

    Double_t hitX = hitToDraw->GetX();
    Double_t hitY = hitToDraw->GetY();
    TMarker* hitCenter = new TMarker(hitX,hitY,markerStyle);
    if ( hitToDraw->GetRefIndex() >= 0 ) {
      hitCenter->SetUniqueID(hitToDraw->GetRefIndex());
      hitCenter->SetMarkerColor(markerColor);
      hitCenter->SetMarkerSize(markerSize);
    }
    else {
      hitCenter->SetUniqueID(666666);
      hitCenter->SetMarkerColor(2);
      hitCenter->SetMarkerSize(markerSize);
    }

    hitCenter->Draw();

  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DrawHitErrors() {
  Double_t markerSize = 2.;
  if ( fPlotSize > 1.1 ) markerSize = 1.;
  Int_t markerColor = 13;
  Int_t markerStyle = 29;

  PndGemHit* hitToDraw;
  for ( Int_t ihit = 0 ; ihit < fNofGemHits ; ihit++ ) {
    markerColor = 11; 

    hitToDraw = (PndGemHit*)fGemHitArray->At(ihit);

    Int_t hitStationNr = hitToDraw->GetStationNr();

    if ( hitStationNr != fDrawStation+1 ) continue;

    Double_t hitX = hitToDraw->GetX();
    Double_t hitY = hitToDraw->GetY();

    Double_t errRValue = hitToDraw->GetDr()*TMath::Sqrt(3.);
    Double_t errPValue = hitToDraw->GetDp()*TMath::Sqrt(3.);
    //    Double_t phiAValue = 0.;
    Double_t phiAValue = TMath::ATan(hitX/hitY);
    if ( hitY < 0 ) phiAValue += TMath::Pi();
    else if ( hitX < 0 ) phiAValue +=  2.*TMath::Pi();

    Double_t cosPhi = TMath::Cos(phiAValue);
    Double_t sinPhi = TMath::Sin(phiAValue);

    Double_t xErr[5], yErr[5];
    xErr[0] = hitX-errPValue*cosPhi+errRValue*sinPhi; yErr[0] = hitY+errPValue*sinPhi+errRValue*cosPhi;
    xErr[1] = hitX+errPValue*cosPhi+errRValue*sinPhi; yErr[1] = hitY-errPValue*sinPhi+errRValue*cosPhi;
    xErr[2] = hitX+errPValue*cosPhi-errRValue*sinPhi; yErr[2] = hitY-errPValue*sinPhi-errRValue*cosPhi;
    xErr[3] = hitX-errPValue*cosPhi-errRValue*sinPhi; yErr[3] = hitY+errPValue*sinPhi-errRValue*cosPhi;
    xErr[4] = xErr[0];                                yErr[4] = yErr[0];

    TPolyLine* errorBox = new TPolyLine(5,xErr,yErr);
    if ( hitToDraw->GetRefIndex() > 0 ) 
      errorBox->SetUniqueID(hitToDraw->GetRefIndex());
    else
      errorBox->SetUniqueID(666666);

    errorBox->SetLineWidth(0);
    errorBox->SetLineColor(markerColor);
    errorBox->SetFillColor(markerColor);
    errorBox->Draw("");

  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ChangePlotParams() {
  //  cout << "problem here?" << endl;
  fPlotCenterZ = fStationsZPosition[fDrawStation];
  //  cout << fPSizeEntry << endl;
  //  cout << "plot size " << fPlotSize << " at " << fPlotCenterX << " " << fPlotCenterY << " " << fPlotCenterZ << endl;
  TString tempString = Form("%1.3f",(Float_t)fPlotSize);
  fPSizeEntry->SetText(tempString.Data());
  tempString = Form("%1.3f",(Float_t)fPlotCenterX);
  fPCenXEntry->SetText(tempString.Data());
  tempString = Form("%1.3f",(Float_t)fPlotCenterY);
  fPCenYEntry->SetText(tempString.Data());
  tempString = Form("%1.3f",(Float_t)fPlotCenterZ);
  fPCenZEntry->SetText(tempString.Data());
  //  cout << "seems so" << endl;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::SaveCanvas() {
  TCanvas *fCanvas = fEcanvas->GetCanvas();

  TString saveToFile = "outputFile.gif";
  cout << "will save canvas as \"" << saveToFile.Data() << "\"" << endl;

  fCanvas->SaveAs(saveToFile.Data());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ReadPoint() {
  Int_t iPoint = fPointToDrawEntry->GetIntNumber();

  if ( iPoint >= fNofGemPoints ) { cout << "last point reached" << endl; return; }

  PndGemMCPoint* fGemPoint = (PndGemMCPoint*)fGemPointArray->At(iPoint);

  fPlotSize    = 0.5;
  fPlotCenterX = fGemPoint->GetX();  
  fPlotCenterY = fGemPoint->GetY();  
  fPlotCenterZ = fGemPoint->GetZ();  

  TString stationIdentifier = fGemPoint->GetDetName();
  stationIdentifier.Remove(stationIdentifier.Last('/'),stationIdentifier.Length());
  stationIdentifier.Remove(0,stationIdentifier.Last('/')+1);
  stationIdentifier.Remove(0,8);
  stationIdentifier.Remove(1,9);

  fDrawStation = stationIdentifier.Atoi()-1;

  /*  cout << "POINT #" << iPoint << " at (" 
       << fPlotCenterX << "," 
       << fPlotCenterY << "," 
       << fPlotCenterZ << ") in \"" 
       << fGemPoint->GetDetName() << "\" -> " << stationIdentifier.Data() << endl;
  */

  ChangePlotParams();

  DoDraw();  

  TString tempString = Form("%d",iPoint+1);
  fPointToDrawEntry->SetText(tempString.Data());

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ReadEvent() {
  if ( !fSimRecoFilesRead ) {
    fSimFile = TFile::Open(Form("%s.root",fBaseString.Data()));   

    if ( !fSimFile ) 
      return;

    cout << "simulation file \"" << fSimFile->GetName() << "\" loaded" << endl;
    fSimTree       = (TTree*) fSimFile->Get("cbmsim");

    fGemPointArray = new TClonesArray("PndGemMCPoint");
    fSimTree->SetBranchAddress("GEMPoint",&fGemPointArray) ;

    fMCTrackArray = new TClonesArray("PndMCTrack");
    fSimTree->SetBranchAddress("MCTrack",&fMCTrackArray) ;

    fDigiFile = TFile::Open(Form("%s_digi.root",fBaseString.Data()));
    fHitsFile = TFile::Open(Form("%s_hits.root",fBaseString.Data()));

    if ( !fDigiFile || !fHitsFile ) 
      return;

    cout << "digi file \"" << fDigiFile->GetName() << "\" loaded" << endl;
    fDigiTree       = (TTree*) fDigiFile->Get("cbmsim");
    fGemDigiArray  = new TClonesArray("PndGemDigi");
    fDigiTree->SetBranchAddress("GEMDigi",&fGemDigiArray) ;

    cout << "hits file \"" << fHitsFile->GetName() << "\" loaded" << endl;
    fHitsTree       = (TTree*) fHitsFile->Get("cbmsim");
    fGemHitArray   = new TClonesArray("PndGemHit");
    fHitsTree->SetBranchAddress("GEMHit",&fGemHitArray) ;

    fSimRecoFilesRead = kTRUE;
  }

  Int_t iEvent = fEventToDrawEntry->GetIntNumber();

  fSimTree->GetEntry(iEvent);
  fDigiTree->GetEntry(iEvent);
  fHitsTree->GetEntry(iEvent);

  fNofGemPoints = fGemPointArray->GetEntriesFast();
  fNofMCTracks  = fMCTrackArray ->GetEntriesFast();
  fNofGemHits   = fGemHitArray  ->GetEntriesFast();
  fNofGemDigis  = fGemDigiArray ->GetEntriesFast();

  Int_t falseHits = 0;
  PndGemHit* hitToDraw;
  for ( Int_t ihit = 0 ; ihit < fNofGemHits ; ihit++ ) {
    hitToDraw = (PndGemHit*)fGemHitArray->At(ihit);
    if ( hitToDraw->GetRefIndex() == -1 ) 
      falseHits ++;
//       cout << "GOT FALSE HIT" << endl;
  }

  cout << " --> Event #" << iEvent << ": " 
       << fNofGemPoints << " points, "
       << fNofMCTracks << " MCtracks, "
       << fNofGemHits << " hits, "
       << fNofGemDigis << " digis. # of false hits: " << falseHits << endl;


  TString tempString = Form("%d",iEvent+1);
  fEventToDrawEntry->SetText(tempString.Data());

  DoDraw();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::ReadParameters() {
  fStations = new TObjArray(10);
  fRealStations = new TObjArray(10);

  TString dir = getenv("VMCWORKDIR");
  TString geoFileName = fParFString.Data();
  geoFileName.ReplaceAll("$VMCWORKDIR",dir.Data());

  ifstream fin(geoFileName.Data());
  
  string dummyText;         //dummy text

  if(!fin) { cout << "sorry, no file \"" << geoFileName.Data() << "\"" << endl; return; }

  TString check;

  do {
    getline(fin,dummyText);
  }
  while ( dummyText.find('#') != -1 );
  getline(fin,dummyText);
  // cout << "finished reading the beginning" << endl;
  Double_t parameters[1000];
  Int_t nofParameters = 0;

  TString bufNumber;

  while ( fin ) {
    getline(fin,dummyText);
    if ( dummyText.find("TrackFinder") != -1 ) break;
    //     cout << "reading " << dummyText << endl;
    dummyText.replace(0,dummyText.find_first_not_of(' '),"");   
    // cout << "now it is " << dummyText << endl;
    while ( dummyText.find(',') != -1 ) {
      parameters[nofParameters++] = atof(dummyText.c_str());
      //  cout << "parameter[" << nofParameters-1 << "] = " << parameters[nofParameters-1] << endl;
      dummyText.replace(0,dummyText.find(',')+1,"");
      // cout << " and it is " << dummyText << endl;
    }
    if ( atof(dummyText.c_str()) != 0. )     
      parameters[nofParameters++] = atof(dummyText.c_str());
    // cout << " --> " << nofParameters << " parameters" << endl;
  }
  //cout << "parameter[" << nofParameters-1 << "] = " << parameters[nofParameters-1] << endl;
  fin.close();

  Int_t arrayIndex = 0;

  Int_t   stationNr   = 0;
  TString stationName = "";
  Int_t   nofSensors = 0;
  Int_t   sensorNr    = 0;
  TString sensorName  = "";
  PndGemStation*  station = NULL;
  PndGemSensor*   sensor  = NULL;

  PndGemStation*  realStation = NULL;
  PndGemSensor*   realSensor  = NULL;


  while ( arrayIndex < nofParameters ) {
    stationNr   = (Int_t)parameters[arrayIndex+0];
    stationName = Form("Gem_Station_%d",stationNr);
    
    station = new PndGemStation(stationName.Data(), stationNr, parameters[arrayIndex+1], TMath::Pi()*parameters[arrayIndex+2]/180. );
    realStation = new PndGemStation(stationName.Data(), stationNr, parameters[arrayIndex+1], TMath::Pi()*parameters[arrayIndex+2]/180. );

    fStationsZPosition  [stationNr-1] = station->GetZ();
    cout << "ADDING STATION AT " << station->GetZ() << endl;
    fStations->Add(station);
    fRealStations->Add(realStation);

    arrayIndex += 4;
    
    nofSensors = (Int_t)parameters[arrayIndex-1];
    
    for ( Int_t isec = 0 ; isec < nofSensors ; isec++ ) {
      if ( isec == 0 ) {
	fStationsInnerRadius[stationNr-1] = parameters[arrayIndex+ 6]; 
	fStationsOuterRadius[stationNr-1] = parameters[arrayIndex+ 7];
      }

      sensorNr   = (Int_t)parameters[arrayIndex+0];
      sensorName = Form("Gem_Disk%d_Gem%s_Sensor_kapton",stationNr,(sensorNr==1?"1":"6"));
      
      realSensor = new PndGemSensor(sensorName.Data(), stationNr, sensorNr, (Int_t)parameters[arrayIndex+1],
				    parameters[arrayIndex+ 2], parameters[arrayIndex+ 3], parameters[arrayIndex+ 4],
				    -TMath::Pi()*parameters[arrayIndex+ 5]/180.,
				    parameters[arrayIndex+ 6], parameters[arrayIndex+ 7], 
				    parameters[arrayIndex+ 8],
				    parameters[arrayIndex+ 9], parameters[arrayIndex+10], 
				    parameters[arrayIndex+11], parameters[arrayIndex+12]);
      
      for ( Int_t itemp = 0 ; itemp < 2 ; itemp++ ) {
	if ( TMath::Abs(parameters[arrayIndex+ 9+itemp]) <  1. || 
	     TMath::Abs(parameters[arrayIndex+ 9+itemp]) > 89. )
	  continue;

	Double_t c = TMath::Cos((180.-parameters[arrayIndex+ 9+itemp])*TMath::DegToRad());
	Double_t i = parameters[arrayIndex+ 6];
	Double_t r = parameters[arrayIndex+ 7];
	Double_t m = i*c+TMath::Sqrt(i*i*c*c-i*i+r*r);

	Double_t b = TMath::ACos((i*i+r*r-m*m)/(2.*i*r))*TMath::RadToDeg();

	if ( parameters[arrayIndex+ 9+itemp] > 0. ) parameters[arrayIndex+ 9+itemp] =  b;
	if ( parameters[arrayIndex+ 9+itemp] < 0. ) parameters[arrayIndex+ 9+itemp] = -b;
	
	cout << "angle = " << parameters[arrayIndex+ 9+itemp] << endl;
      }

      sensor = new PndGemSensor(sensorName.Data(), stationNr, sensorNr, (Int_t)parameters[arrayIndex+1],
				parameters[arrayIndex+ 2], parameters[arrayIndex+ 3], parameters[arrayIndex+ 4],
				-TMath::Pi()*parameters[arrayIndex+ 5]/180.,
				parameters[arrayIndex+ 6], parameters[arrayIndex+ 7], 
				parameters[arrayIndex+ 8],
				parameters[arrayIndex+ 9], parameters[arrayIndex+10], 
				parameters[arrayIndex+11], parameters[arrayIndex+12]);
      
      station->AddSensor(sensor);
      realStation->AddSensor(realSensor);
      
      arrayIndex+=13;
    }
  }
  fNofStations = fStations->GetEntries();
  cout << "Made " << fNofStations << " stations." << endl;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void MyMainFrame::DoInfoStatusBar(Int_t event, Int_t x, Int_t y,
				  TObject *sel) {
  TString selName = sel->GetName();
  TCanvas *fCanvas = fEcanvas->GetCanvas();
  //  cout << "cursor at " << fCanvas->AbsPixeltoX(x) << " " << fCanvas->AbsPixeltoY(y) << endl;

  Double_t xReal = fCanvas->AbsPixeltoX(x);
  Double_t yReal = fCanvas->AbsPixeltoY(y);

  TString colorName[2][2] = {"blue","red","green","yellow"};

  if (event == kMouseMotion){
    //    cout << "being at " << selName.Data() << " "  << sel->GetUniqueID() << endl;
    fInfoStatusBar->SetText(Form("%.6f",xReal),1);
    fInfoStatusBar->SetText(Form("%.6f",yReal),2);

    //    cout << "it is " << selName.Data() << " with id = " << sel->GetUniqueID() << endl;

    if ( ( selName.Contains("TMarker") || selName.Contains("TPolyLine") ) && sel->GetUniqueID()!= 666666 ) {
      cout << "it is " << selName.Data() << " with id = " << sel->GetUniqueID() << endl;
      fInfoStatusBar->SetText(Form("%d",sel->GetUniqueID()),7);
      PndGemMCPoint* pointSel = (PndGemMCPoint*)fGemPointArray->At(sel->GetUniqueID());
      cout << "pointSel at " << pointSel->GetX() << " " << pointSel->GetY() << " " << pointSel->GetZ() << endl;
      fInfoStatusBar->SetText(Form("pos: (%.4f,%.4f,%.4f)",
				   pointSel->GetX(),
				   pointSel->GetY(),
				   pointSel->GetZ()),8);
      Double_t px = pointSel->GetPx();
      Double_t py = pointSel->GetPy();
      Double_t pz = pointSel->GetPz();
      fInfoStatusBar->SetText(Form("mom: (%.4f,%.4f,%.4f) -> %.4f",
				   px,py,pz,
				   TMath::Sqrt(px*px+py*py+pz*pz)),9);
      PndMCTrack* trackSel = (PndMCTrack*)fMCTrackArray->At(pointSel->GetTrackID());
      TVector3 startVertex   = trackSel->GetStartVertex();
      TVector3 startMomentum = trackSel->GetMomentum();
      px = startMomentum.X();
      py = startMomentum.Y();
      pz = startMomentum.Z();

      fInfoStatusBar->SetText(Form("%d from (%.2f,%.2f,%.2f), mom: (%.2f,%.2f,%.2f)-> %.4f",
				   trackSel->GetPdgCode(),startVertex.X(),startVertex.Y(),startVertex.Z(),
				   px,py,pz,
				   TMath::Sqrt(px*px+py*py+pz*pz)),10);
    }
    else {
      fInfoStatusBar->SetText("",7);
      fInfoStatusBar->SetText("",8);
      fInfoStatusBar->SetText("",9);
      fInfoStatusBar->SetText("",10);
    }

    PndGemStation* digiStation;
    PndGemSensor* digiSensor;
    digiStation = (PndGemStation*)fRealStations->At(fDrawStation);

    for ( Int_t isens = 0 ; isens < digiStation->GetNSensors() ; isens++ ) {
      digiSensor  = (PndGemSensor*) digiStation->GetSensorByNr(isens+1);
      for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
	Int_t channelNumber = digiSensor->GetChannel(xReal,yReal,iside);
	fInfoStatusBar->SetText(Form("%d",channelNumber),3+isens*2+iside);
	TGCompositeFrame* barPart = fInfoStatusBar->GetBarPart(3+isens*2+iside);

	Pixel_t colorPix;
 	if ( channelNumber == - 1 ) {
	  gClient->GetColorByName("grey",colorPix);
	  barPart->SetBackgroundColor(colorPix);
	}
	else {
	  gClient->GetColorByName(colorName[isens][iside].Data(),colorPix);
	  barPart->SetBackgroundColor(colorPix);
// 	  gClient->GetColorByName("red",colorPix);
// 	  barPart->SetForegroundColor(colorPix);
	}
      }
    }
  }
  
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MyMainFrame::~MyMainFrame() {
  // Clean up used widgets: frames, buttons, layouthints
  fMain->Cleanup();
  delete fMain;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void guiEventDisplay() {
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(),fWindowSize*1.2,fWindowSize);
}
//-----------------------------------------------------------------------------

