// The macro plot structure of emc fwd endcap.
// In each box, which corresponds to crystal its two indexes are  printed
// In the GUI two indexes and detectorID are shown in statusbar when mouse point over the crystal position
// Button "Save" save the drawing to the emc_structure.ps file which can be zoomed.
// The input file, which macro uses set at the top of the macro "file_name="
// it is output of the simulation, which contain the FAIRGeom object, from which PndEmcStructure is initialised
// Button "Recreate" recreate "structure.txt" from which GUI read positions of crystals and their indexes
	

#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGStatusBar.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGraph.h>
#include <TAxis.h>

string file_name="sim_emc.root";

int structure_analysis_emc()
{
  gROOT->SetStyle("Plain");
	
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();

  // The file structure.txt which contain for each detector in Fwd Endcap detectorId, two indexes and x,y,z coordinates
	ifstream f;
	f.open("structure.txt");
		
	if (!f.is_open())
	{
		std::cout<<"File does not exist"<<std::endl;
		TFile* fsim = new TFile(file_name.c_str()); 
		TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
		PndEmcMapper *emcMap=PndEmcMapper::Instance(2);
		PndEmcStructure::Instance()->Print("structure.txt",2);
		std::cout<<"File with emc structure is created"<<std::endl;
		
		// Delete lines from the file, which are not relatd to forward EMC
		string line;
		ifstream in("structure.txt");
		ofstream out("outfile.txt");
		//copy first line
		getline(in,line);
		out<<line<<"\n";
				
		while (getline(in,line))
		{
			if (line[0]!='3') continue; // Forward endcap start from 3
			out<<line<<"\n";
		}
		
		in.close();
		out.close();
		remove("structure.txt");
		rename("outfile.txt","structure.txt");
		std::cout<<"All but Forward emc is removed"<<std::endl;
		std::cout<<"Done"<<std::endl;
	}
	else 
	{
		f.close();
	}
	
	
   // Popup the GUI...
   new MyMainFrame(gClient->GetRoot(), 200, 200);
	
  return 0;
}



class MyMainFrame : public TGMainFrame {

private:
   TRootEmbeddedCanvas  *fEcan;
   TGStatusBar          *fStatusBar;
   
public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();
   void DoExit();
   void DoDraw();
	void DoSave();
	void DoRecreate();
	
   void SetStatusText(const char *txt, Int_t pi);
   void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);
   
   ClassDef(MyMainFrame, 0)
};

void MyMainFrame::DoDraw()
{
   TCanvas *c1 = fEcan->GetCanvas();
	ifstream f;
	f.open("structure.txt");
   c1->Range(-120,-120,120,120);
	
	Double_t x,y,z;
	long index;
	Int_t iX, iY;
	TPaveLabel *pt[5000];
	Int_t i_crys=0;
	Double_t tr = 2.51875;             // Size of the Crystal in its center (cm)
	  
	//Position of the first line with numbers
	f.seekg(30);
	while (f>>index>>iX>>iY>>x>>y>>z)
	{
		// check if tci belongs to Forward Endcap
		if ((iX<200)||(iX>300)) continue;
		if (i_crys%100==0) std::cout<<"Crystal in fwd endcap = "<<i_crys<<std::endl;
		i_crys++;
		
		TString label="";
		label+=iX;
		label+=",";
		label+=iY;
		
		TString name="";
		name+=index;

		pt[i_crys] = new TPaveLabel(x-tr/2.,y-tr/2., x+tr/2., y+tr/2.,label.Data());
		pt[i_crys]->SetBorderSize(1);
		pt[i_crys]->SetName(name);
		pt[i_crys]->Draw();
	}
	   
	// TCanvas::Update() draws the frame, after which it can be changed
   c1->Update();
   c1->Modified();
   c1->Update();
}

void MyMainFrame::DoExit()
{
   printf("Exit application...");
   gApplication->Terminate(0);
}

void MyMainFrame::DoRecreate()
{
	std::cout<<"Recreate structure.txt"<<std::endl;
	TFile* fsim = new TFile(file_name.c_str()); 
	TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
	PndEmcMapper *emcMap=PndEmcMapper::Instance(2);
	PndEmcStructure::Instance()->Print("structure.txt",2);
	std::cout<<"File with emc structure is created"<<std::endl;
	
	// Delete lines from the file, which are not relatd to forward EMC
	string line;
	ifstream infile("structure.txt");
	ofstream outfile("outfile.txt");
	//copy first linef
	getline(infile,line);
	outfile<<line<<"\n";
			
	while (getline(infile,line))
	{
		if (line[0]!='3') continue; // Forward endcap start from 3
		outfile<<line<<"\n";
	}
	
	infile.close();
	outfile.close();
	remove("structure.txt");
	rename("outfile.txt","structure.txt");
	std::cout<<"All but Forward emc is removed"<<std::endl;
	std::cout<<"Done"<<std::endl;

	
}

void MyMainFrame::DoSave()
{
	
	TCanvas *c1 = fEcan->GetCanvas();
	c1->SaveAs("emc_structure.ps");
	std::cout<<"Structure is saved to emc_structure.ps"<<std::endl;
}

void MyMainFrame::SetStatusText(const char *txt, Int_t pi)
{
   // Set text in status bar.
   fStatusBar->SetText(txt,pi);
}

void MyMainFrame::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
//  Writes the event status in the status bar parts

   const char *text2;
   char text0[50], text1[50];
	sprintf(text0, "(iX,iY)=%s", selected->GetTitle());
   SetStatusText(text0,0);
	sprintf(text1, "detId=%s", selected->GetName());
   SetStatusText(text1,1);
   text2 = selected->GetObjectInfo(px,py);
   SetStatusText(text2,2);
}

MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h) :
   TGMainFrame(p, w, h)
{
   // Create the embedded canvas
   fEcan = new TRootEmbeddedCanvas(0,this,500,400);
   Int_t wid = fEcan->GetCanvasWindowId();
   TCanvas *myc = new TCanvas("MyCanvas", 10,10,wid);
   fEcan->AdoptCanvas(myc);
   myc->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","MyMainFrame",this, 
               "EventInfo(Int_t,Int_t,Int_t,TObject*)");

   AddFrame(fEcan, new TGLayoutHints(kLHintsTop | kLHintsLeft | 
                                     kLHintsExpandX  | kLHintsExpandY,0,0,1,1));
   // status bar
   Int_t parts[] = {34, 33, 33};
   fStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
   fStatusBar->SetParts(parts, 3);
   fStatusBar->Draw3DCorner(kFALSE);
   AddFrame(fStatusBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));
   
   // Create a horizontal frame containing two buttons
   TGHorizontalFrame *hframe = new TGHorizontalFrame(this, 200, 40);
  
   TGTextButton *draw = new TGTextButton(hframe, "&Draw");
   draw->Connect("Clicked()", "MyMainFrame", this, "DoDraw()");
   hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
   TGTextButton *exit = new TGTextButton(hframe, "&Exit ");
   exit->Connect("Pressed()", "MyMainFrame", this, "DoExit()");
   hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
   
	TGTextButton *save = new TGTextButton(hframe, "&Save ");
   save->Connect("Clicked()", "MyMainFrame", this, "DoSave()");
   hframe->AddFrame(save, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

	TGTextButton *recreate = new TGTextButton(hframe, "&Recreate ");
   recreate->Connect("Clicked()", "MyMainFrame", this, "DoRecreate()");
   hframe->AddFrame(recreate, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
   
	AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));

   // Set a name to the main frame   
   SetWindowName("EMC Forward Endcap structure");
   MapSubwindows();

   // Initialize the layout algorithm via Resize()
   Resize(GetDefaultSize());

   // Map main frame
   MapWindow();
}


MyMainFrame::~MyMainFrame()
{
   // Clean up main frame...
   Cleanup();
   delete fEcan;
}
