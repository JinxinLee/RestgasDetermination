testInclusive() {

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
    rootlogon();
    gSystem->Load("libEvtGen");
    gSystem->Load("libEvtGenExternal");
    gSystem->Load("libCHIGEN");

    chigen::ostreams::suppress_all_cout = false;
    chigen::ostreams::write_log_file = true;
    chigen::ostreams::suppress_pandaroot = false;
    chigen::ostreams::suppress_pandaroot_cout = false;
    chigen::ostreams::verbose_mode = true;
    
    
    chigen::evtgen::EvtGenChiDecFile = "../PolarizedDecays.dec";
    
   // chigen::initialize();

    //      bool suppress_all_cout = false;
    //        bool write_log_file = true;
    //        bool suppress_pandaroot = false;
    //        bool suppress_pandaroot_cout = false;
    //        bool verbose_mode = true;

    //partonic model for chi_{c2}
    using namespace chigen;
    models::chi_2_1 chi1(5.5);
    ChiGen chigen(chi1);
    //    chigen::PndChiGenExclusive chigen(chi1);

    //    FairRunSim *fRun = new FairRunSim();s
    //    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    //    fRun->SetGenerator(primGen);

    for (int i = 0; i < 4; ++i) {
        chigen.next();
        //        chigen.ReadEvent(primGen);
    }

}