#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"

//#include "PndTrack.h"
#include "FairTrackParP.h"

#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "RhoVtxPoca.h"

void ConvertTrackToRhoCandList(TString prefix = "barrel") {
    TFile* file = TFile::Open(prefix + "_reco.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Couldn't open the file!" << std::endl;
        return;
    }

    // get the TTree
    TTree* tree = (TTree*)file->Get("pndsim");  // 根据你的文件结构修改
    TClonesArray* finalGenTrackArray = new TClonesArray("PndTrack");
    tree->SetBranchAddress("FinalGenTrack", &finalGenTrackArray);

    // 创建 RhoCandList
    RhoCandList trackList;

    // 遍历事件
    Long64_t nEvents = tree->GetEntries();
    for (Long64_t i = 0; i < nEvents; ++i) {
        tree->GetEntry(i);

        Int_t nTracks = finalGenTrackArray->GetEntries();
        for (Int_t j = 0; j < nTracks; ++j) {
            PndTrack* track = (PndTrack*)finalGenTrackArray->At(j);
            if (!track) continue;

            // 提取轨迹参数
            FairTrackParP par = track->GetParamFirst();  // 使用起始点
            TVector3 pos(par.GetX(), par.GetY(), par.GetZ());
            TVector3 mom(par.GetPx(), par.GetPy(), par.GetPz());
            Double_t charge = par.GetQ();

            // 构造动量四矢量
            Double_t mass = 0.13957;  // 假设是 π⁺，根据实际情况修改
            Double_t energy = sqrt(mom.Mag2() + mass * mass);
            TLorentzVector p4(mom, energy);

            // 构造 RhoCandidate
            RhoCandidate* cand = new RhoCandidate(p4, charge);
            cand->SetPos(pos);
            cand->SetType(211);  // 设置为 π⁺，根据需要修改

            // 可选：设置协方差矩阵（如果你有）
            // cand->SetCov7(...);

            // 加入列表
            trackList.Put(cand);
        }

        // 顶点估计
        TVector3 vertex;
        RhoVtxPoca pocaTool;
        Double_t quality = pocaTool.GetPocaVtx(vertex, trackList);

        std::cout << "Event " << i << " vertex estimate: (" 
                  << vertex.X() << ", " << vertex.Y() << ", " << vertex.Z() 
                  << "), quality = " << quality << std::endl;

        trackList.Clear();  // 清空列表准备下一个事件
    }

    file->Close();
    delete finalGenTrackArray;
}

