int PandaSmartLabel(TString align="Rprel", Double_t xpos=-1, Double_t ypos=-1, Double_t scale=-1, TString str="-1", Double_t scale2=-1) {
  // H as the first parameter triggers printing comamnd options
  if (align == "H") {
   cout 
      << endl
      << "  USAGE: PandaSmartLabel(\"align\",xpos,ypos,scale,\"str\",scale2);" << endl
      << "  Prints the official BaBar label on the active ROOT pad" << endl
      << "      align   R or L, optionally with additional 'prel' or 'front': str is aligned to the right (default) or left edge of the \"PANDA\" label" << endl
      << "      xpos    X position of the \"PANDA\" label\'s bottom right (align==R)/left (==L) corner, 0 < xpos < 1, defaults to (L) left margin + 0.02 or (R) 1.0 - right margin - 0.02" << endl 
      << "      ypos    Y position of the \"PANDA\" label\'s bottom right (align==R)/left (==L) corner, 0 < ypos < 1, defaults to 1 - top margin - 0.08" << endl
      << "      scale   relative size of the label, defaults to 0.9" << endl 
      << "      str     LaTeX-style text that goes under the PANDA label." << endl
      << "      scale2  relative size of the second line of text, defaults to 0.45" << endl;
   cout <<endl
	  << "    Examples: " << endl
      << "      PandaSmartLabel(\"H\")         -> Prints out this help text" << endl
      << "      PandaSmartLabel()            -> Label 'PANDA//MC simulation' in top right corner" << endl
      << "      PandaSmartLabel(\"L\")         -> Label 'PANDA//MC simulation' in top left corner" << endl
      << "      PandaSmartLabel(\"-1\",-1,-1,-1,\"preliminary\",0.4) -> Label 'PANDA//preliminary' in top right corner " << endl
      << "      PandaSmartLabel(\"Rprel\")     -> Label 'PANDA' in top right corner  with a large 'PRELIMINARY' across the plot behind histograms (works only with gStyle->SetFrameFillStyle(0))" << endl
      << "      PandaSmartLabel(\"Lprelfront\")-> Label 'PANDA' in top left corner  with a large 'PRELIMINARY' across the plot behind histograms (works only with gStyle->SetFrameFillStyle(0))" << endl << endl;
    return();
  }

  if (gPad==0) return;
	
  // Making -1 a placeholder for function's default value 
  if (align=="") align="R";
  
  bool rightalign = !(align.Contains("L"));
  bool preliminary = align.Contains("prel");
  bool front = align.Contains("front");
  
  double mt = gPad->GetTopMargin();
  double mb = gPad->GetBottomMargin();
  double ml = gPad->GetLeftMargin();
  double mr = gPad->GetRightMargin();
  
  if (ypos==-1) ypos = 1.0 - mt - 0.08;
  
  if (rightalign) // Right align 
  {
	  if (xpos==-1) xpos = 1.0 - mr - 0.02;
  }
  else
  {							// Left align
	  if (xpos==-1) xpos = ml + 0.02;
  }
  
  if (scale == -1) scale = 0.9; 
  if (str == "-1") str = "MC simulation"; 
  if (scale2 == -1) scale2 = 0.45;
  

  // Draw the label 
  TLatex *panda = new TLatex();
  panda->SetNDC(kTRUE); 
   double fcx=1., fcy=1., dx=1.0-xpos, dy = 1.0-ypos;
//   
  double padSizeX = gPad->GetWw()*gPad->GetWNDC(); // Get pad's dimensions
  double padSizeY = gPad->GetWh()*gPad->GetHNDC();
//   if (padSizeX>padSizeY) fcx=padSizeY/padSizeX;
//   if (padSizeX<padSizeY) fcy=padSizeX/padSizeY;
//   
//   cout <<padSizeX<<" "<<padSizeY<<" : "<<fcx<<" "<<fcy<<endl;
//   
  panda->SetNDC(kTRUE); 

  if (preliminary)
  {
	  panda->SetTextFont(42);
	  panda->SetTextColor(17);
	  panda->SetTextSize(0.17*(1.-ml-mr));
	  panda->SetTextAlign(22);
	  double sizeX = padSizeX*(1. - ml - mr);
	  double sizeY = padSizeY*(1. - mt - mb);
	  double posX  = (ml+(1.-mr))*0.5;
	  double posY  = (mb+(1.-mt))*0.42;
	  panda->SetTextAngle(atan(sizeY/sizeX)/3.1415*142.);
	  panda->DrawLatex(posX,posY,"PRELIMINARY");
	  
	  TList *ll=gPad->GetListOfPrimitives();
	  if (!front) ll->AddAt(ll->RemoveAt(ll->GetEntries()-1),0);
  }
  
  panda->SetTextColor(13);
  panda->SetTextAngle(0);
  
  panda->SetTextFont(62); // Bold helvetiva
  if (rightalign) panda->SetTextAlign(31); // Right-Bottom
  else panda->SetTextAlign(11);            // Left bottom
  panda->SetTextSize(0.07*scale);
  panda->DrawLatex(xpos*fcy,ypos*fcx,"#bar{P}ANDA");
  
  panda->SetTextFont(42); // Helvetica
  if (rightalign) panda->SetTextAlign(33); // Right-Bottom
  else panda->SetTextAlign(13);            // Left bottom
  panda->SetTextSize(0.08*scale2);
  panda->DrawLatex(xpos*fcx,ypos*fcy*0.99,str);
 
  delete panda;
  
  return 0;
}
