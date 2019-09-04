#ifndef PndEventBuilderAnaTask_H_
#define PndEventBuilderAnaTask_H_

#include "TClonesArray.h"
#include "TH2D.h"
//#include "PndSdsHit.h"

#include "FairLink.h"

#include "PndPersistencyTask.h"

#include <vector>

struct MCPoint {
        MCPoint(): isFound(false), isPrimary(false) {};
        FairLink link;
        bool isFound;
        bool isPrimary;
        friend std::ostream& operator<<(std::ostream& os, const MCPoint data){
            os << data.link << " : isPrimary " << data.isPrimary << " isFound " << data.isFound << std::endl;
            return os;
        }
};

struct MCEvent {
    std::vector<FairLink> fMCTracks;
    std::map<int, std::vector< MCPoint > > fMCPoints; ///< MCPoints for BranchID
//    std::map<int, std::vector<bool> > fPointFound;


//    void ResetFoundPoints(){
//        for (auto & mcpoints : fMCPoints){
//            std::cout << "-I- MCEvent::ResetFoundPoints "<< mcpoints.first << " / " << mcpoints.second.size() << std::endl;
//            fPointFound[mcpoints.first].clear();
//            fPointFound[mcpoints.first].resize(mcpoints.second.size(), false);
//
//        }
//    }

    friend std::ostream& operator<<(std::ostream& os, const MCEvent data){
        os << "MCTracks: ";
        for (auto track : data.fMCTracks){
            os << track << " ";
        }
        os << std::endl;
        for (auto branch : data.fMCPoints){
            os << FairRootManager::Instance()->GetBranchName(branch.first) << std::endl;
            for (auto link : branch.second){
                os << link << " ";
            }
            os << std::endl;
            int branchId = branch.first;
//            std::map<int, std::vector<bool> > foundMap = data.fPointFound;
//            std::vector<bool> foundArray = foundMap[branchId];
//            if (foundArray.size() == branch.second.size()){
//                for (auto found : foundArray){
//                    std::cout << found << " ";
//                }
//                os << std::endl;
//            }
        }
//        std::cout << "PointsFoundMap: " << std::endl;
//        for (auto pointFound : data.fPointFound){
//            os << pointFound.first << "/" << pointFound.second.size() << std::endl;
//        }
//        os << std::endl;
        return os;
    }

};

struct BranchInfo {
       std::map<int, double> fPercentageMCPoints; ///< percentage of found MCPoints per MCEvent
       std::map<int, int> fNumberOfFoundMCPoints; ///< number of found MCPoint per MCEvent
       std::map<int, int> fTotalNumberOfPoints; ///< number of MCPoints per MCEvent
       std::map<int, double> fPercentagePrimaryMCPoints; ///< percentage of found MCPoints per MCEvent
       std::map<int, int> fNumberOfFoundPrimaryMCPoints; ///< number of found MCPoint per MCEvent
       std::map<int, int> fTotalNumberOfPrimaryPoints; ///< number of MCPoints per MCEvent
       int fNumberOfPointsOutsideMC;
       friend std::ostream& operator<<(std::ostream& os, const BranchInfo data){
           os << "Found Points: " << std::endl;
           for (auto value : data.fNumberOfFoundMCPoints){
               std::map<int, double> percentageMap = data.fPercentageMCPoints;
               std::map<int, int> totalNumbersMap = data.fTotalNumberOfPoints;

               os << value.first << " : " << value.second<< "/" << totalNumbersMap[value.first] << " = " << percentageMap[value.first] * 100 << " %" << std::endl;
           }
           os << "Found Primary Points: " << std::endl;
           for (auto value : data.fNumberOfFoundPrimaryMCPoints){
               std::map<int, double> percentageMap = data.fPercentagePrimaryMCPoints;
               std::map<int, int> totalNumbersMap = data.fTotalNumberOfPrimaryPoints;

               os << value.first << " : " << value.second<< "/" << totalNumbersMap[value.first] << " = " << percentageMap[value.first] * 100 << " %" << std::endl;
           }
           os << "Additional hits: " << data.fNumberOfPointsOutsideMC << std::endl;
           return os;
       }
};

struct EventInfo {
        std::map<int, MCEvent> fMCEvents;   ///< map<MCEventNumber, MCEvents>
        std::map<int, BranchInfo> fBranchInfo; ///< map<Branch,  BranchInfo>
        void CalculateBranchInfo();
        friend std::ostream& operator<<(std::ostream& os, const EventInfo data){
            for (auto branch : data.fBranchInfo){
                std::cout << "-I- EventInfo for branch: " << FairRootManager::Instance()->GetBranchName(branch.first) << std::endl;
                os << branch.second;
                os << std::endl;
            }
            return os;
        }
};

struct BranchHistos {
        BranchHistos(TString branchName, int nEvents, int nMCEvents){
            fAbsoluteHits = new TH2D("AbsoluteHits"+branchName, "AbsoluteHits"+branchName, nEvents + 1, -0.5, nEvents + 0.5, nMCEvents + 1, -0.5, nMCEvents + 0.5);
            fRelativeHits = new TH2D("RelativeHits"+branchName, "RelativeHits"+branchName, nEvents + 1, -0.5, nEvents + 0.5, nMCEvents + 1, -0.5, nMCEvents + 0.5);
            fAbsolutePrimaryHits = new TH2D("AbsolutePrimaryHits"+branchName, "AbsolutePrimaryHits"+branchName, nEvents + 1, -0.5, nEvents + 0.5, nMCEvents + 1, -0.5, nMCEvents + 0.5);
            fRelativePrimaryHits = new TH2D("RelativePrimaryHits"+branchName, "RelativePrimaryHits"+branchName, nEvents + 1, -0.5, nEvents + 0.5, nMCEvents + 1, -0.5, nMCEvents + 0.5);
            fOutsideMC = new TH1D("OutsideMC"+branchName, "OutsideMC"+branchName, nEvents + 1, -0.5, nEvents + 0.5);
            fOutsideMCHisto = new TH1D("OutsideMCHisto"+branchName, "OutsideMCHisto"+branchName, 1000, 0, 10000);
            fAbsoluteHitsHisto = new TH1D("AbsoluteHitsHisto"+branchName, "AbsoluteHitsHisto"+branchName, 1000, 0, 1000);
            fAbsolutePrimaryHitsHisto = new TH1D("AbsolutePrimaryHitsHisto"+branchName, "AbsolutePrimaryHitsHisto"+branchName, 1000, 0, 1000);
            fRelativeHitsHisto = new TH1D("RelatvieHitsHisto"+branchName, "RelatvieHitsHisto"+branchName, 100, 0, 2.0);
            fRelativePrimaryHitsHisto = new TH1D("RelatviePrimaryHitsHisto"+branchName, "RelatviePrimaryHitsHisto"+branchName, 100, 0, 2.0);
            fEventOverlap = new TH1D("EventOverlap" + branchName, "EventOverlap" + branchName, 100,0,100);

        };

        void WriteHistos()
        {
            fAbsoluteHits->Write();
            fAbsoluteHitsHisto->Write();
            fRelativeHits->Write();
            fRelativeHitsHisto->Write();
            fAbsolutePrimaryHits->Write();
            fAbsolutePrimaryHitsHisto->Write();
            fRelativePrimaryHits->Write();
            fRelativePrimaryHitsHisto->Write();
            fOutsideMC->Write();
            fOutsideMCHisto->Write();
            fEventOverlap->Write();
        }

        TH2D* fAbsoluteHits;
        TH1D* fAbsoluteHitsHisto;
        TH2D* fRelativeHits;
        TH1D* fRelativeHitsHisto;
        TH2D* fAbsolutePrimaryHits;
        TH1D* fAbsolutePrimaryHitsHisto;
        TH2D* fRelativePrimaryHits;
        TH1D* fRelativePrimaryHitsHisto;
        TH1D* fOutsideMC;
        TH1D* fOutsideMCHisto;
        TH1D* fEventOverlap;
};


class PndEventBuilderAnaTask : public PndPersistencyTask
{
public:
	PndEventBuilderAnaTask();
	virtual ~PndEventBuilderAnaTask();

	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();
    virtual void FinishTask();

    void SetBranchName(TString name) {fMainBranchName = name;};

    void AddAdditionalBranches(TString branchName){
    	fAddHitArray[branchName] = nullptr;
    }

protected:
    std::set<int> GetMCEventIDs(TClonesArray* array);
    void AssignHitsToPoints(TString branchName, TClonesArray* hitArray);
    MCEvent GetMCInfo(int entryNr);

    void FillHistos();

    TString GetPointBranch(TString hitbranch){
        if (hitbranch.Contains("STT"))
            return "STTPoint";
        if (hitbranch.Contains("MVD"))
            return "MVDPoint";
        if (hitbranch.Contains("GEM"))
            return "GEMPoint";
        if (hitbranch.Contains("SciT"))
            return "GEMPoint";
        if (hitbranch.Contains("SciTPoint"))
            return "GEMPoint";
        if (hitbranch.Contains("EMC"))
            return "EmcHit";
        if (hitbranch.Contains("FTS"))
            return "FTSPoint";
        if (hitbranch.Contains("Ftof"))
            return "FtofPoint";
        return "";
    }


private:


  TString fMainBranchName;
  TClonesArray* fMainHitArray;

  std::map<TString, TClonesArray*> fAddHitArray;
  std::map<TString, BranchHistos*> fBranchHistos;

  TClonesArray* fMCArray;
  std::map<int, TClonesArray*> fMCPointArrays;
  std::map<int, MCEvent> fMCEvents;
  std::vector<EventInfo> fEventInfo;
  int fEntryNr;

  Bool_t fInitDone;


  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndEventBuilderAnaTask,1);


};

#endif /*PndEventBuilderAnaTask_H_*/
