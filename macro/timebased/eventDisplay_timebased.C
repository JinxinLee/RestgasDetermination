int eventDisplay_timebased(TString prefix = "../data/evtcomplete")
{

    Bool_t enablePointDraw = kTRUE;
    Bool_t enableHitDraw = kTRUE;
    Bool_t enableTrackDraw = kTRUE;

    TString parAsciiFile = "all.par";

    TString input = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
    TString output = "dummy";
    TString friend1 = "digi_timebased";
    TString friend2 = "sim";
    TString friend3 = "recoLocal_timebased";
    TString friend4 = "";

    // -----   Initial Settings   --------------------------------------------
    PndMasterRunAna *fRun = new PndMasterRunAna();
    fRun->SetInput(input);
    fRun->SetOutput(output);
    fRun->AddFriend(friend1);
    fRun->AddFriend(friend2);
    fRun->AddFriend(friend3);
    fRun->AddFriend(friend4);
    fRun->SetParamAsciiFile(parAsciiFile);
    fRun->Setup(prefix);
    fRun->RunWithTimeStamps();
    fRun->UseFairLinks(kTRUE);

    FairEventManager *fMan = new FairEventManager();

    PndSttRootGeometryInitTask* task = new PndSttRootGeometryInitTask();
    fRun->AddTask(task);

    //----------------------Traks and points -------------------------------------
    if (enablePointDraw)
    {
        FairMCTracks *Track = new FairMCTracks("Monte-Carlo Tracks");
        FairMCPointDraw *MvdPoints = new FairMCPointDraw("MVDPoint", kBlue, kFullSquare);
        FairHitDraw *EMCPoints = new FairHitDraw("EmcHit");
        PndEmcHitDraw *EMCPoints2 = new PndEmcHitDraw("EmcHit");
        EMCPoints->SetBoxDimensions(2., 2., 2.);
        EMCPoints2->SetBoxDimensions(2., 2., 2.);
        FairMCPointDraw *TofSciFPoint = new FairMCPointDraw("SciTPoint", kTeal, kFullSquare);
        FairMCPointDraw *MdtPoint = new FairMCPointDraw("MdtPoint", kAzure, kFullSquare);
        FairMCPointDraw *PndDrcBarPoint = new FairMCPointDraw("DrcBarPoint", kGreen, kFullSquare);
        FairMCPointDraw *PndDrcPDPoint = new FairMCPointDraw("DrcPDPoint", kViolet, kFullSquare);
        FairMCPointDraw *PndDskParticle = new FairMCPointDraw("DskParticle", kYellow, kFullSquare);
        FairMCPointDraw *PndDskFLGHit = new FairMCPointDraw("PndDskFLGHit", kPink, kFullSquare);
        FairMCPointDraw *PndSTTPoint = new FairMCPointDraw("STTPoint", kMagenta, kFullSquare);
        FairMCPointDraw *PndGEMPoint = new FairMCPointDraw("GEMPoint", kRed, kFullSquare);
        FairMCPointDraw *PndFTSPoint = new FairMCPointDraw("FTSPoint", kMagenta, kFullSquare);
        FairMCPointDraw *PndFtofPoint = new FairMCPointDraw("FtofPoint", kGreen, kFullSquare);

        PndEmcHitCaloDraw *EmcHitCalo = new PndEmcHitCaloDraw("EmcHit");

        fMan->AddTask(Track);
        fMan->AddTask(MvdPoints);
        fMan->AddTask(EMCPoints);
        fMan->AddTask(EMCPoints2);
        fMan->AddTask(TofSciFPoint);
        fMan->AddTask(MdtPoint);
        fMan->AddTask(PndDrcBarPoint);
        fMan->AddTask(PndDrcPDPoint);
        fMan->AddTask(PndDskParticle);
        fMan->AddTask(PndDskFLGHit);
        fMan->AddTask(PndSTTPoint);
        fMan->AddTask(PndGEMPoint);
        fMan->AddTask(PndFTSPoint);
        fMan->AddTask(PndFtofPoint);
        fMan->AddTask(EmcHitCalo);

    }

    if (enableHitDraw)
    {
        FairHitDraw *MvdRecoHit = new FairHitDraw("MVDHitsPixel");
        MvdRecoHit->SetTimeWindowPlus(5);
        MvdRecoHit->SetTimeWindowMinus(5);

        FairHitDraw *MvdRecoStrip = new FairHitDraw("MVDHitsStrip");
        MvdRecoStrip->SetTimeWindowPlus(5);
        MvdRecoStrip->SetTimeWindowMinus(5);

        FairHitDraw *STTHits = new FairHitDraw("STTHit");
        STTHits->SetTimeWindowPlus(200);
        STTHits->SetTimeWindowMinus(1);

        PndSttIsochroneDraw* STTIsochrone = new PndSttIsochroneDraw("STTHit");
        STTIsochrone->SetTimeWindowPlus(250);
        STTIsochrone->SetTimeWindowMinus(1);
        STTIsochrone->UseIsochroneTime();

        FairHitDraw *SciTHit = new FairHitDraw("SciTHit");
        FairHitDraw *MdtHit = new FairHitDraw("MdtHit");
        FairHitDraw *DrcHit = new FairHitDraw("DrcHit");
        FairHitDraw *DrcPDHit = new FairHitDraw("DrcPDHit");
        FairHitDraw *GEMHit = new FairHitDraw("GEMHit");
        FairHitDraw *FTSHit = new FairHitDraw("FTSHit");
        FairHitDraw *FtofHit = new FairHitDraw("FtofHit");
        PndEmcClusterDraw *EmcBump = new PndEmcClusterDraw("EmcBump");
        EmcBump->SetBoxDimensions(4.0, 4.0, 4.0);
        fMan->AddTask(MvdRecoHit);
        fMan->AddTask(MvdRecoStrip);
        fMan->AddTask(STTHits);
        fMan->AddTask(STTIsochrone);
        fMan->AddTask(SciTHit);
        fMan->AddTask(MdtHit);
        fMan->AddTask(DrcHit);
        fMan->AddTask(DrcPDHit);
        fMan->AddTask(GEMHit);
        fMan->AddTask(FTSHit);
        fMan->AddTask(FtofHit);
        fMan->AddTask(EmcBump);
    }
//
    if (enableTrackDraw)
    {
        PndTrackDraw* BarrelTrackDraw = new PndTrackDraw("BarrelTrack", kTRUE);
        BarrelTrackDraw->SetTimeWindowPlus(5);
        BarrelTrackDraw->SetTimeWindowMinus(5);

        PndTrackDraw* SttMvdTrack = new PndTrackDraw("BarrelGenTrack", kTRUE);
        PndTrackDraw* SttMvdGemTrack = new PndTrackDraw("SttMvdGemTrack", kTRUE);
        PndTrackDraw* FtsIdealTrack = new PndTrackDraw("FtsIdealTrack");
        PndTrackDraw* SttMvdGemGenTrack = new PndTrackDraw("SttMvdGemGenTrack", kTRUE);
        PndTrackDraw* FtsIdealGenTrack = new PndTrackDraw("FtsIdealGenTrack");
        fMan->AddTask(BarrelTrackDraw);
        fMan->AddTask(SttMvdTrack);
        fMan->AddTask(SttMvdGemTrack);
        fMan->AddTask(SttMvdGemGenTrack);
        fMan->AddTask(FtsIdealTrack);
        fMan->AddTask(FtsIdealGenTrack);
    }

    fMan->Init();

    return 0;
}
