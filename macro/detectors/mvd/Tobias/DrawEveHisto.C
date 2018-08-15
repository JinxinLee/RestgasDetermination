int DrawEveHisto(TH2* histo, TGeoHMatrix* matrix, double max = -1., TEveCompound* comp = 0)
{
	//TEveCompound* comp = new TEveCompound();

  TEveBoxSet* q = new TEveBoxSet(histo->GetTitle());
  gStyle->SetPalette(1, 0);


  if (max < 0)
	  TEveRGBAPalette* pal = new TEveRGBAPalette(1, histo->GetMaximum());
  else
	  TEveRGBAPalette* pal = new TEveRGBAPalette(1, max);

   q->SetPalette(pal);

  int histoSize = histo->GetNbinsX() * histo->GetNbinsY();
  q->Reset(TEveBoxSet::kBT_AABoxFixedDim, kFALSE, histoSize);
  q->SetDefDepth(1);
  q->SetDefHeight(1);
  q->SetDefWidth(1);

  double xoffset = histo->GetNbinsX()/2;
  double yoffset = histo->GetNbinsY()/2;
   for (Int_t x=0; x < histo->GetNbinsX(); x++) {
     for (Int_t y=0; y < histo->GetNbinsY(); y++){
       q->AddBox(x-xoffset,y-yoffset,0);
       q->DigitValue(histo->GetBinContent(x,y));
     }
   }
   q->RefitPlex();

   TGeoHMatrix testMatrix = (*matrix);
   Double_t scale[] = {0.1,0.1,0.1};		//Dim is 1 mm^3
   testMatrix.SetScale(scale);
   TEveTrans& t = q->RefMainTrans();
   t.SetFrom(testMatrix);

   if (comp > 0)
	   comp->AddElement(q);
   else
	   gEve->AddGlobalElement(q);
   //gEve->Redraw3D(kTRUE);
  return 0;
}
