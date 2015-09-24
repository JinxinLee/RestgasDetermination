void setPandaStyle()
{
	TStyle *pandaStyle=new TStyle("PANDA","PANDA approved plots style");

	// use plain black on white colors
	pandaStyle->SetFrameBorderMode(0);
	pandaStyle->SetCanvasBorderMode(0);
	pandaStyle->SetPadBorderMode(0);
	pandaStyle->SetPadColor(0);
	pandaStyle->SetCanvasColor(0);
	pandaStyle->SetFrameFillColor(0);
	pandaStyle->SetFrameFillStyle(0);
	//pandaStyle->SetFillColor(0);
	
	// set the paper & margin sizes
	pandaStyle->SetPaperSize(20,26);
	pandaStyle->SetPadTopMargin(0.05);
	pandaStyle->SetPadRightMargin(0.05);
	pandaStyle->SetPadBottomMargin(0.14);
	pandaStyle->SetPadLeftMargin(0.12);

	pandaStyle->SetStatColor(0);
	pandaStyle->SetOptStat(1110) ; 
	pandaStyle->SetStatBorderSize(1);
	pandaStyle->SetStatFont(42);
	pandaStyle->SetStatFontSize(0.04);
	pandaStyle->SetStatX(0.95);
	pandaStyle->SetStatY(0.95);
	pandaStyle->SetStatW(0.14);
	pandaStyle->SetStatH(0.2);


	// use sans serif fonts
// 	pandaStyle->SetTextFont(132);
// 	pandaStyle->SetTextSize(0.08);
	pandaStyle->SetLabelFont(42,"x");
	pandaStyle->SetLabelFont(42,"y");
	pandaStyle->SetLabelFont(42,"z");
	pandaStyle->SetLabelSize(0.05,"x");
	pandaStyle->SetTitleSize(0.06,"x");
	pandaStyle->SetTitleOffset(1.0,"X");
	pandaStyle->SetLabelSize(0.05,"y");
	pandaStyle->SetTitleSize(0.06,"y");
	pandaStyle->SetTitleOffset(1.,"y");
	pandaStyle->SetLabelSize(0.05,"z");
	pandaStyle->SetTitleSize(0.06,"z");

	// use bold lines and markers
	pandaStyle->SetMarkerStyle(20);
//	pandaStyle->SetHistLineWidth(1.85);
	pandaStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

	// get rid of X error bars and y error bar caps
	pandaStyle->SetErrorX(0.001);

	// do not display any of the standard histogram decorations
	pandaStyle->SetOptTitle(0);
	pandaStyle->SetOptStat(0);
	pandaStyle->SetOptFit(1);
	pandaStyle->SetFitFormat(".3g");
		
	pandaStyle->SetPalette(1);

	// put tick marks on top and RHS of plots
 	pandaStyle->SetPadTickX(1);
 	pandaStyle->SetPadTickY(1);
	pandaStyle->SetTickLength(0.020,"xz");
	pandaStyle->SetTickLength(0.015,"y");
	//pandaStyle->SetLabelOffset(0.02,"xyz");

	gROOT->Reset();
	gROOT->SetStyle("PANDA");
	gROOT->ForceStyle();
}