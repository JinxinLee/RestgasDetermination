/*
 * MSaveBigPng.C
 *
 *  Created on: 25.08.2015
 *      Author: Stockmanns
 */

int MSaveBigPNG(TString filename="", double scale=5) {
    TCanvas* old_canv = gPad->GetCanvas();

    gROOT->SetBatch(kTRUE);
    gROOT->ForceStyle(kTRUE);

    Int_t orig_msz = gStyle->GetMarkerSize();
    Int_t orig_mst = gStyle->GetMarkerStyle();
    Int_t orig_lt  = gStyle->GetLineWidth();

    gStyle->SetMarkerSize(1.0+scale/5);
    gStyle->SetMarkerStyle(20);
    gStyle->SetLineWidth(orig_lt*scale);

    if(filename == "") {
        filename = old_canv->GetName();
        filename += ".png";
    }

    Int_t old_width  = old_canv->GetWindowWidth();
    Int_t old_height = old_canv->GetWindowHeight();

    Int_t new_width = old_width * scale;
    Int_t new_height= old_height* scale;

    TCanvas* temp_canvas = new TCanvas("temp", "", new_width, new_height);
    old_canv->DrawClonePad();

    temp_canvas->Draw();
    temp_canvas->SaveAs(filename);
    temp_canvas->Close();

    gStyle->SetMarkerSize(orig_msz);
    gStyle->SetMarkerStyle(orig_mst);
    gStyle->SetLineWidth(orig_lt);

    gROOT->ForceStyle(kFALSE);
    gROOT->SetBatch(kFALSE);

    return 0;
}

