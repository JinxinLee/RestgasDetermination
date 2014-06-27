/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include "ChiGenContext.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <stdexcept>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include "TRandom.h"

namespace chigen {


#define __millisecondsTime__ boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds()

    double elapsedTimeSeconds() {
        static const double start = (double) __millisecondsTime__;
        return ((__millisecondsTime__ - start) / 1e3);
    }

    namespace {
        long __seed__ = 0;

        std::string toString(int a) {
            std::stringstream ss;
            ss << a;
            return ss.str();
        }
    };

    namespace {
        typedef boost::iostreams::tee_device<std::ostream, std::ofstream> TeeDevice;
        typedef boost::iostreams::stream<TeeDevice> TeeStream;
    }

    namespace ostreams {
        bool suppress_all_cout = false;
        bool write_log_file = false;
        bool suppress_pandaroot = false;
        bool suppress_pandaroot_cout = false;
        bool verbose_mode = true;


        std::ostream* log_file = 0;
        std::ostream* terminal = 0;
        std::ostream* tee_stream = 0;
        std::ostream* chigen_cout = 0;

        void initialize_streams() {
            remove(CHIGEN_LOG_FILE);
            std::ofstream* __temp_logFile__ = new std::ofstream();
            if (write_log_file)
                __temp_logFile__->open(CHIGEN_LOG_FILE);
            ostreams::log_file = new std::ostream(__temp_logFile__->rdbuf());
            ostreams::terminal = new std::ostream(std::cout.rdbuf());
            TeeDevice __temp_tee_device(*ostreams::terminal, *__temp_logFile__);
            ostreams::tee_stream = new TeeStream(__temp_tee_device);

            if (suppress_all_cout && write_log_file) {
                //write all to log.file
                std::cout.rdbuf(__temp_logFile__->rdbuf());
                ostreams::chigen_cout = &std::cout;
            } else if (suppress_all_cout && !write_log_file) {
                //no any cout at all
                std::cout.rdbuf(0);
                ostreams::chigen_cout = &std::cout;
            } else if (write_log_file) {
                //writing log file
                if (suppress_pandaroot) {
                    //suppress panda root in log file
                    std::cout.rdbuf(0);
                    ostreams::chigen_cout = ostreams::tee_stream;
                } else if (suppress_pandaroot_cout) {
                    //suppress pandaroot in terminal but not in log
                    std::cout.rdbuf(__temp_logFile__->rdbuf());
                    ostreams::chigen_cout = ostreams::tee_stream;
                } else {
                    //both pandaroot & chigen goes to log and terminal
                    std::cout.rdbuf(ostreams::tee_stream->rdbuf());
                    ostreams::chigen_cout = ostreams::tee_stream;
                }
            } else {
                //no log file will be created
                if (suppress_pandaroot || suppress_pandaroot_cout) {
                    //suppress pandaroot in terminal
                    std::cout.rdbuf(0);
                }
                ostreams::chigen_cout = ostreams::terminal;
            }
        }
    }

    namespace random {
        ChiGenRandomEngine* random_engine = 0;
        Pythia8::Rndm* pythia_random_engine = 0;

        void initialize_random() {
            random::random_engine = new ChiGenRandomEngine(__seed__);
            random::pythia_random_engine = new Pythia8::Rndm(__seed__);
            random::pythia_random_engine->rndmEnginePtr(random::random_engine);
        }
    };

    namespace evtgen {
        namespace {

            std::string addVMCDir(const char* _str) {
                static std::string workingDir = getenv("VMCWORKDIR");
                return (workingDir + _str);
            };
        }
        std::string EvtGenPDL = addVMCDir("/pgenerators/EvtGen/EvtGen/Private/evt.pdl");
        std::string EvtGenDecFile = addVMCDir("/pgenerators/EvtGen/EvtGen/Private/DECAY.DEC");
        std::string EvtGenChiDecFile = addVMCDir("/pgenerators/chigen/PolarizedDecays.dec");

        bool evt_gen_is_loaded = false;
        EvtGen* evt_gen = 0;

        namespace {
            bool evt_gen_ids_are_loaded = false;
        }

        std::string chi_c1_str = std::string(CHI1_STRING),
        chi_c2_str = std::string(CHI2_STRING),
        x3872_str = std::string(X3872_STRING);

        void ensure_evt_gen_is_inialized() {
            if (!evt_gen_is_loaded)
                throw runtime_error("EvtGen is not loaded yet.");

            if (evt_gen_ids_are_loaded)
                return;

            chi_c1_evt_id = EvtPDL::getId(chi_c1_str);
            chi_c2_evt_id = EvtPDL::getId(chi_c2_str);
            x3872_evt_id = EvtPDL::getId(x3872_str);
            evt_gen_ids_are_loaded = true;
        }

        EvtId chi_c1_evt_id, chi_c2_evt_id, x3872_evt_id;

        void initialize_evtgen() {
            //initializing EvtGen
            EvtExternalGenList genList;
            EvtAbsRadCorr* radCorrEngine = genList.getPhotosModel();
            std::list<EvtDecayBase*> extraModels = genList.getListOfModels();

            evtgen::evt_gen = new EvtGen(
                    EvtGenDecFile.c_str(),
                    EvtGenPDL.c_str(),
                    random::random_engine,
                    radCorrEngine, &extraModels);

            evt_gen->readUDecay(EvtGenChiDecFile.c_str());

            evtgen::evt_gen_is_loaded = true;
            ensure_evt_gen_is_inialized();
        }

        void read_dec_file(char* dec_file_name) {
            ensure_evt_gen_is_inialized();
            evt_gen->readUDecay(dec_file_name);
        }
    };

    namespace pythia {
        Pythia8::Pythia* pythia = 0;
        Pythia8::PDF* pdf = 0;

        void initialize_pythia() {
            pdf = new Pythia8::CTEQ6pdf(2212, 1, getenv("PYTHIA8DATA"));

            pythia = new Pythia8::Pythia(getenv("PYTHIA8DATA"));
            pythia->rndm = *random::pythia_random_engine;

            pythia->readString("ProcessLevel:all = off");
            //not allow charmonia to decay with Pythia
            pythia->readString("20443:mayDecay = off");
            pythia->readString("445:mayDecay = off");
            pythia->readString("443:mayDecay = off");
            
            //x3872 is not implemented in PYTHIA
            pythia->particleData.addParticle(
                    X3872_PDG_ID, // id
                    X3872_STRING, // name
                    3, // spinType
                    0, // chargeType
                    0, // colType
                    X3872_MASS, // m0
                    0.0001, // mWidth
                    X3872_MASS-0.0001, // mMin
                    X3872_MASS+0.0001 // mMax
                    );
            pythia->readString("9920443:mayDecay = off");
           
            pythia->init();
        }
    };

    namespace {
        bool chigen_is_initialized = false;
    }

    void ensure_chigen_is_initialized() {
        if (!chigen_is_initialized)
            initialize();
    }

    void initialize() {
        initialize((long) gRandom->GetSeed());
    }

    void initialize(long seed) {
        chigen_is_initialized = true;

        __seed__ = seed;

        //initializing streams
        ostreams::initialize_streams();

        __chigen_cout__ << " Initializing with seed " << __seed__ << "." << std::endl;

        //initializing random 
        srand(seed); //<-to ensure that std::random will use same seed
        random::initialize_random();

        __chigen_cout__ << " Initializing EvtGen ...";
        //initializing EvtGen
        evtgen::initialize_evtgen();
        __chigen_direct_cout__ << "done." << std::endl;

        __chigen_cout__ << " Initializing Pythia ...";
        //initializing Pythia
        pythia::initialize_pythia();
        __chigen_direct_cout__ << "done." << std::endl;

    };

    long get_seed() {
        return __seed__;
    };

    EvtId pdgId2EvtId(int pdgId) {
        evtgen::ensure_evt_gen_is_inialized();
        switch (pdgId) {
            case CHI1_PDG_ID:
                return evtgen::chi_c1_evt_id;
            case CHI2_PDG_ID:
                return evtgen::chi_c2_evt_id;
            case X3872_PDG_ID:
                return evtgen::x3872_evt_id;
            default:
                throw std::runtime_error(std::string("Unknown particle id: ") + toString(pdgId));
        }
    }

    bool isPWaveCharmonia(int pdgCode) {
        return pdgCode == CHI1_PDG_ID || pdgCode == CHI2_PDG_ID || pdgCode == X3872_PDG_ID;
    };
    
    bool isCharmonia(int pdgCode) {
        return pdgCode == CHI1_PDG_ID || pdgCode == CHI2_PDG_ID 
                || pdgCode == X3872_PDG_ID || pdgCode == PSI_PDG_ID;
    };
    
}

