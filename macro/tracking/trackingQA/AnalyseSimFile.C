/**
 @class AnalyseSimFile.C
 @brief Create some standard plots for simulation files
 Macro to generate histograms for particle types of primary particles
 @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 @date 4.7.2018
 **/
#include <vector>

std::map<int, double> GetFilledBins(TH1* histo)
{
    std::map<int, double> result;
    for (int ibin = 0; ibin < histo->GetNbinsX(); ibin++)
    {
        if (histo->GetBinContent(ibin) > 0)
        {
            std::cout << histo->GetBinCenter(ibin) << " : " << histo->GetBinContent(ibin) << std::endl;
            result[histo->GetBinCenter(ibin)]=histo->GetBinContent(ibin);
        }
    }
    return result;
}

void Draw2DHisto(TTree* tree, TString xValue, TString yValue, TString histoName, TCut cut)
{
    TString dataSelector = yValue;
    dataSelector += ":";
    dataSelector += xValue;
    dataSelector += ">>";
    dataSelector += histoName;
    tree->Draw(dataSelector, cut, "goff");
}

void FillPdgSelectHisto(std::map<int, double>& pdgValues, TH1* pdgHisto)
{
    int i = 0;
    for (auto pdg : pdgValues){
        pdgHisto->Fill(i,pdg.second);
        TString pdgName = "";
        pdgName += pdg.first;
        pdgHisto->GetXaxis()->SetBinLabel(i+1, pdgName.Data());
        i++;
    }
}

int AnalyseSimFile(TString prefix)
{

    PndFileNameCreator creator(prefix.Data());
    TString extension;
    TString simFile = creator.GetSimFileName();
    TString outputFile = creator.GetCustomFileName("sim_histos");
    TFile outfile(outputFile.Data(),"RECREATE");

    std::cout << "InFileName: " << simFile.Data() << std::endl;
    std::cout << "OutputFileName: " << outputFile.Data() << std::endl;

    TFile fileqa(simFile.Data());
    TTree *simtree = (TTree*) fileqa.Get("pndsim");
    std::cout << "SimTree: " << simtree << std::endl;
    std::cout << "SimTree.GetEntries() " << simtree->GetEntries() << std::endl;

    TString theta = "TMath::RadToDeg()*TMath::ATan2(TMath::Sqrt(MCTrack.fPx^2+MCTrack.fPy^2),MCTrack.fPz)";
    TString p = "TMath::Sqrt(MCTrack.fPx^2+MCTrack.fPy^2+MCTrack.fPz^2)";
    TString pt = "TMath::Sqrt(MCTrack.fPx^2+MCTrack.fPy^2)";
    TCut primCut = "MCTrack.GetMotherID()<0";
    TCut secCut = "MCTrack.GetMotherID()>0";

    TH1I* pdgHistoPrim = new TH1I("pdgHistoPrim", "pdgHistoPrim", 20001, -10000.5, 10000.5);
    TH1I* pdgHistoSec = new TH1I("pdgHistoSec", "pdgHistoSec", 20001, -10000.5, 10000.5);

    simtree->Draw("MCTrack.fPdgCode>>pdgHistoPrim", primCut, "goff");
    simtree->Draw("MCTrack.fPdgCode>>pdgHistoSec", secCut, "goff");

    std::map<int, double> pdgValues = GetFilledBins(pdgHistoPrim);
    TH1D* pdgHistoSelectPrim = new TH1D("pdgHistoSelectPrim","pdgHistoSelectPrim", pdgValues.size(),-0.5,pdgValues.size() -0.5);
    FillPdgSelectHisto(pdgValues, pdgHistoSelectPrim);

    std::map<int, double> pdgValuesSec = GetFilledBins(pdgHistoSec);
    TH1D* pdgHistoSelectSec = new TH1D("pdgHistoSelectSec","pdgHistoSelectSec", pdgValuesSec.size(),-0.5,pdgValuesSec.size() -0.5);
    FillPdgSelectHisto(pdgValuesSec, pdgHistoSelectSec);

    outfile.cd();
    pdgHistoSelectPrim->Write();
    pdgHistoSelectSec->Write();


    std::map<int, TH2*> ThetaPHistosPdg;
    std::map<int, TH2*> PtPzHistosPdg;
    TString histosNameThetaP = "histoThetaPforPdg_";
    TString histosNamePtPz = "histoPtPzforPdg_";
    TString pdgCut = "MCTrack.fPdgCode==";
    TString myHistoName = histosNameThetaP;
    myHistoName += "all";
    ThetaPHistosPdg[0] = new TH2I(myHistoName.Data(),myHistoName.Data(),180,0,180,150,0,15);
    Draw2DHisto(simtree, theta, p, myHistoName,primCut);

    myHistoName = histosNamePtPz;
    myHistoName += "all";
    PtPzHistosPdg[0] = new TH2I(myHistoName.Data(), myHistoName.Data(), 300, -15, 15, 150, 0, 15);
    Draw2DHisto(simtree, "MCTrack.fPz", pt, myHistoName, primCut);

    outfile.cd();
    ThetaPHistosPdg[0]->Write();
    PtPzHistosPdg[0]->Write();

    for (auto pdg : pdgValues){
        myHistoName = histosNameThetaP;
        myHistoName += pdg.first;
        TString myPdgString = pdgCut;
        myPdgString += pdg.first;
        TCut myPdgCut = myPdgString.Data();

        ThetaPHistosPdg[pdg.first] = new TH2I(myHistoName.Data(),myHistoName.Data(),180,0,180,150,0,15);
        Draw2DHisto(simtree, theta, p, myHistoName, myPdgCut&&primCut);

        myHistoName = histosNamePtPz;
        myHistoName += pdg.first;

        PtPzHistosPdg[pdg.first] = new TH2I(myHistoName.Data(),myHistoName.Data(),300,-15,15,150,0,15);
        Draw2DHisto(simtree, "MCTrack.fPz", pt, myHistoName, myPdgCut&&primCut);

        outfile.cd();
        ThetaPHistosPdg[pdg.first]->Write();
        PtPzHistosPdg[pdg.first]->Write();
    }

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    outfile.Close();
    return 0;
}
