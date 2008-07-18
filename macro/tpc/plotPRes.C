{

gROOT->Reset();
TString files[7]={"data/pions0.1GeV.reco.root",
		  "data/pions0.25GeV.reco.root",
		  "data/pions0.5GeV.reco.root",
		  "data/pions0.75GeV.reco.root",
		  "data/pions1.0GeV.reco.root",
		  "data/pions1.5GeV.reco.root",
		  "data/pions2.0GeV.reco.root"};

double mom[7]={0.1,0.25,0.5,0.75,1.0,1.5,2.0};
TCanvas* cp=new TCanvas("Momentum","Momentum",15,15,500,900);
cp->Divide(2,4);

TGraph* gmom=new TGraph(7);


for(int i=0; i<7; ++i){
    TFile* file=new TFile(files[i],"READ");
    TTree* tr=(TTree*)file->Get("cbmsim");
    cp->cd(i+1);

    double low=-mom[i]*0.05*mom[i]/0.25;
    double high=mom[i]*0.05*mom[i]/0.25;
 

    TString sel="TrackFitStat._p-";
    sel+=mom[i];
    sel+=">>h(50,";
    sel+=low;
    sel+=",";
    sel+=high;
    sel+=")";
    tr->Draw(sel);
    TH1* h=(TH1*)gDirectory->Get("h");
    h->Fit("gaus");
    h->SetLineColor(kBlue);
    f=h->GetFunction("gaus");
    double dp_p=f->GetParameter(2)/mom[i];

    gmom->SetPoint(i,mom[i],dp_p);
}

cp->cd(8);
gmom->Draw("AP");
}
