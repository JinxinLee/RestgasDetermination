
#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TEfficiency.h"
#include "TStyle.h"

/**
 * 效率修正脚本
 * 
 * 功能：
 * 1. 读取真实数据 (Data Rec)
 * 2. 读取MC重建数据 (MC Rec)
 * 3. 读取MC生成数据 (MC Gen) - 用于计算分母
 * 4. 计算效率 Efficiency = MC Rec / MC Gen
 * 5. 对真实数据进行修正 Corrected Data = Data Rec / Efficiency
 * 
 * 使用方法：
 * root -l 'efficiency_correction.C("data_ana.root", "mc_ana.root", "mc_sim.root")'
 */
void efficiency_correction(
    TString realDataFile = "data_ana_final.root", // 真实数据文件 (经过ana)
    TString mcRecFile = "mc_ana_final.root",      // MC重建数据文件 (经过ana)
    TString mcGenFile = "mc_sim.root",            // MC生成数据文件 (sim输出，包含MCTrack)
    TString realDataGenFile = "",                 // 真实数据对应的生成文件 (可选，用于验证)
    TString plotVar = "pvz",                      // 需要修正的变量名 (在ana文件的ntpDp树中)
    TString genVar = "MCTrack.fStartZ",           // 对应的生成级变量 (在sim文件的pndsim树中)
    TString recCut = "pvz != -999.0",             // 重建数据的筛选条件
    TString genCut = "MCTrack.fMotherID==-1&&MCTrack.fPdgCode==2212", // 生成级粒子的筛选条件
    int nBins = 100,
    double xMin = -50.0,
    double xMax = 50.0
)
{
    // 设置绘图风格
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);

    // ---------------------------------------------------------
    // 1. 获取真实数据分布 (Numerator for Data)
    // ---------------------------------------------------------
    TFile* fData = TFile::Open(realDataFile);
    if (!fData || fData->IsZombie()) { 
        std::cout << "Error: Cannot open Real Data file: " << realDataFile << std::endl; 
        return; 
    }
    TTree* tData = (TTree*)fData->Get("ntpDp");
    if (!tData) { std::cout << "Error: Cannot find tree 'ntpDp' in Data file." << std::endl; return; }

    TH1F* hData = new TH1F("hData", "Real Data (Rec);Z (cm);Counts", nBins, xMin, xMax);
    tData->Draw(plotVar + ">>hData", recCut);
    hData->SetDirectory(0); // 从文件中解离
    fData->Close();

    // ---------------------------------------------------------
    // 2. 获取MC重建分布 (Numerator for Efficiency)
    // ---------------------------------------------------------
    TFile* fMCRec = TFile::Open(mcRecFile);
    if (!fMCRec || fMCRec->IsZombie()) { 
        std::cout << "Error: Cannot open MC Rec file: " << mcRecFile << std::endl; 
        return; 
    }
    TTree* tMCRec = (TTree*)fMCRec->Get("ntpDp");
    if (!tMCRec) { std::cout << "Error: Cannot find tree 'ntpDp' in MC Rec file." << std::endl; return; }

    TH1F* hMCRec = new TH1F("hMCRec", "MC Reconstructed;Z (cm);Counts", nBins, xMin, xMax);
    tMCRec->Draw(plotVar + ">>hMCRec", recCut);
    hMCRec->SetDirectory(0);
    fMCRec->Close();

    // ---------------------------------------------------------
    // 3. 获取MC生成分布 (Denominator for Efficiency)
    // ---------------------------------------------------------
    // 注意：这是计算绝对效率的关键。必须使用包含所有生成粒子的sim文件。
    TFile* fMCGen = TFile::Open(mcGenFile);
    if (!fMCGen || fMCGen->IsZombie()) { 
        std::cout << "Error: Cannot open MC Gen file: " << mcGenFile << std::endl; 
        std::cout << "Please provide the simulation file (sim.root) to calculate efficiency denominator." << std::endl;
        return; 
    }
    // 尝试获取 cbmsim 或 pndsim
    tMCGen = (TTree*)fMCGen->Get("pndsim");
    
    if (!tMCGen) { std::cout << "Error: Cannot find tree 'cbmsim' or 'pndsim' in MC Gen file." << std::endl; return; }

    TH1F* hMCGen = new TH1F("hMCGen", "MC Generated;Z (cm);Counts", nBins, xMin, xMax);
    
    std::cout << "Drawing Gen variable: " << genVar << " with cut: " << genCut << std::endl;
    // 注意：这里假设genVar可以直接访问。如果是TClonesArray，可能需要更复杂的Draw语法
    // 例如 "MCTrack.fP" 或 "MCTrack[0].fP"
    tMCGen->Draw(genVar + ">>hMCGen", genCut); 
    hMCGen->SetDirectory(0);
    fMCGen->Close();

    // ---------------------------------------------------------
    // 3.5 获取真实数据的生成分布 (Optional Truth for Verification)
    // ---------------------------------------------------------
    TH1F* hDataGen = nullptr;
    if (realDataGenFile != "") {
        TFile* fDataGen = TFile::Open(realDataGenFile);
        if (!fDataGen || fDataGen->IsZombie()) {
            std::cout << "Warning: Cannot open Real Data Gen file: " << realDataGenFile << std::endl;
        } else {
            TTree* tDataGen = (TTree*)fDataGen->Get("cbmsim");
            if (!tDataGen) tDataGen = (TTree*)fDataGen->Get("pndsim");
            
            if (tDataGen) {
                hDataGen = new TH1F("hDataGen", "Real Data Generated (Truth);Z (cm);Counts", nBins, xMin, xMax);
                std::cout << "Drawing Real Data Gen variable..." << std::endl;
                tDataGen->Draw(genVar + ">>hDataGen", genCut);
                hDataGen->SetDirectory(0);
            } else {
                std::cout << "Warning: Cannot find tree 'cbmsim' or 'pndsim' in Real Data Gen file." << std::endl;
            }
            fDataGen->Close();
        }
    }

    // ---------------------------------------------------------
    // 4. 计算效率 (Efficiency)
    // ---------------------------------------------------------
    TH1F* hEff = (TH1F*)hMCRec->Clone("hEff");
    hEff->SetTitle("Efficiency;Variable;Efficiency");
    // 使用Bayesian误差或Binomial误差计算效率
    // 这里简单使用 Divide，如果需要更严谨的误差棒，可以使用 TEfficiency
    hEff->Divide(hMCRec, hMCGen, 1, 1, "B"); 

    // ---------------------------------------------------------
    // 5. 修正真实数据 (Correction)
    // ---------------------------------------------------------
    TH1F* hCorrected = (TH1F*)hData->Clone("hCorrected");
    hCorrected->SetTitle("Corrected Data Distribution;Variable;Counts (Corrected)");
    hCorrected->Divide(hEff);

    // ---------------------------------------------------------
    // 6. 绘图与保存
    // ---------------------------------------------------------
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 1200, 800);
    c1->Divide(2, 2);

    // Pad 1: 原始数据与MC重建对比 (形状对比)
    c1->cd(1);
    hData->SetLineColor(kBlack);
    hData->SetMarkerStyle(20);
    hData->Draw("E");
    
    TH1F* hMCRecScaled = (TH1F*)hMCRec->Clone("hMCRecScaled");
    if (hMCRec->Integral() > 0) hMCRecScaled->Scale(hData->Integral() / hMCRec->Integral()); // 归一化以便比较形状
    hMCRecScaled->SetLineColor(kRed);
    hMCRecScaled->Draw("HIST SAME");
    
    TLegend* leg1 = new TLegend(0.5, 0.7, 0.85, 0.85);
    leg1->AddEntry(hData, "Real Data (Rec)", "lp");
    leg1->AddEntry(hMCRecScaled, "MC Rec (Scaled)", "l");
    leg1->Draw();

    // Pad 2: MC 生成 vs 重建 (效率来源)
    c1->cd(2);
    hMCGen->SetLineColor(kBlue);
    hMCGen->Draw("HIST");
    hMCRec->SetLineColor(kRed);
    hMCRec->Draw("HIST SAME");
    
    TLegend* leg2 = new TLegend(0.5, 0.7, 0.85, 0.85);
    leg2->AddEntry(hMCGen, "MC Gen", "l");
    leg2->AddEntry(hMCRec, "MC Rec", "l");
    leg2->Draw();

    // Pad 3: 效率曲线
    c1->cd(3);
    hEff->SetLineColor(kGreen+2);
    hEff->SetMarkerStyle(21);
    hEff->SetMinimum(0.0);
    hEff->SetMaximum(1.1);
    hEff->Draw("E");

    // Pad 4: 修正后的数据
    c1->cd(4);
    hCorrected->SetLineColor(kBlue+2);
    hCorrected->SetMarkerStyle(20);
    hCorrected->Draw("E");

    if (hDataGen) {
        hDataGen->SetLineColor(kMagenta);
        hDataGen->SetLineStyle(2);
        hDataGen->SetLineWidth(2);
        hDataGen->Draw("HIST SAME");
        
        TLegend* leg4 = new TLegend(0.5, 0.7, 0.85, 0.85);
        leg4->AddEntry(hCorrected, "Corrected Data", "lp");
        leg4->AddEntry(hDataGen, "True Distribution (Gen)", "l");
        leg4->Draw();
    }

    // 保存结果
    c1->SaveAs("efficiency_correction_result.png");
    
    TFile* fOut = new TFile("corrected_data_output.root", "RECREATE");
    hData->Write();
    hMCRec->Write();
    hMCGen->Write();
    hEff->Write();
    hCorrected->Write();
    if (hDataGen) hDataGen->Write();
    fOut->Close();

    std::cout << "Analysis Complete." << std::endl;
    std::cout << "Plot saved to efficiency_correction_result.png" << std::endl;
    std::cout << "Root file saved to corrected_data_output.root" << std::endl;
}
