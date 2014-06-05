/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#ifndef CHIGENCONTEXT_H
#define	CHIGENCONTEXT_H

#include <iostream>
#include <stdio.h>


//---------------------------DEBUG--------------------------------------------//

#define CHIGEN_LOG_FILE "ChiGen.log"

//---------------------------EVTGEN & PYTHIA----------------------------------//
#include "ChiGenRandomEngine.h"
#include "Pythia.h"
#include "EvtGen/EvtGen.hh"
#include "EvtGenExternal/EvtExternalGenList.hh"
#include "EvtGenBase/EvtAbsRadCorr.hh"
#include "EvtGenBase/EvtDecayBase.hh"


//---------------------------PARTICLES----------------------------------------//
/*
 * Particles properties
 */

#define PROTON_MASS 1.0
#define R20_CHI 0.075
#define CHI1_MASS 3.51
#define CHI2_MASS 3.556

#define GAMMA_PDG_ID 22
#define E_PDG_ID 11
#define MU_PDG_ID 13
#define PSI_PDG_ID 443

#define CHI1_PDG_ID 20443
#define CHI1_STRING "chi_c1"

#define CHI2_PDG_ID 445
#define CHI2_STRING "chi_c2"

namespace chigen {
    /**
     * Elapsed time in seconds
     * @return elapsed time in seconds
     */
    extern double elapsedTimeSeconds();
    /**
     * Returns random seed which was set at initialization
     * @return random seed that was set at initialization
     */
    extern long get_seed();

    namespace random {
        /**
         * Random engine for EvtGen and Pythia
         */
        extern ChiGenRandomEngine* random_engine;
        /**
         * Pythia random generator (wrapper of chigen::random::random_engine)
         */
        extern Pythia8::Rndm* pythia_random_engine;
    }

    namespace evtgen {
        /**
         * EvtGen pdl
         */
        extern std::string EvtGenPDL;
        /**
         * EvtGen default decay file
         */
        extern std::string EvtGenDecFile;
        /**
         * EvtGen polarized chi_c decay models
         */
        extern std::string EvtGenChiDecFile;
        /**
         * If true, then all EvtGen files are loaded
         */
        extern bool evt_gen_is_loaded;
        /**
         * Pointer to EvtGen instance
         */
        extern EvtGen* evt_gen;
        /**
         * Reads specified decay file and returns state
         * @param dec_file_name the decay file name
         */
        extern void read_dec_file(char* dec_file_name);
        /**
         * Throws exception if EvtGen is not loaded yet
         */
        extern void ensure_evt_gen_is_inialized();
        /**
         * String names of charmonia states for EvtGen
         */
        extern std::string chi_c1_str, chi_c2_str;
        /**
         * EvtGen ids of charmonia particles
         */
        extern EvtId chi_c1_evt_id, chi_c2_evt_id;
    }

    /**
     * Returns EvtGen particle id from its pdg code
     * @param pdgId PDG code
     * @return EvtGen particle id from its pdg code
     */
    extern EvtId pdgId2EvtId(int pdgId);

    /**
     * Returns true, if specified PDG code corresponds to charmonia
     * @param pdgCode PDG code
     * @return true, if specified PDG code corresponds to charmonia
     */
    extern bool isCharmonia(int pdgCode);

    namespace ostreams {
        /**
         * Suppress all output to terminal
         */
        extern bool suppress_all_cout;
        /**
         * Write log file
         */
        extern bool write_log_file;
        /**
         * Suppress all pandaroot output (to terminal and to log)
         */
        extern bool suppress_pandaroot;
        /**
         * Suppress pandaroot output to terminal
         */
        extern bool suppress_pandaroot_cout;
        /**
         * Print events
         */
        extern bool verbose_mode;

        /**
         * Log file
         */
        extern std::ostream* log_file;
        /**
         * Console (stdout)
         */
        extern std::ostream* terminal;
        /**
         * Tee stream, which duplicate stdout to log file
         */
        extern std::ostream* tee_stream;
        /**
         * Default output stream used by ChiGen
         */
        extern std::ostream* chigen_cout;

#define __chigen_cout__ (*chigen::ostreams::chigen_cout)<<std::fixed<<"["<<chigen::elapsedTimeSeconds()<<" ChiGen] "
#define __chigen_direct_cout__ (*chigen::ostreams::chigen_cout)
    }

    namespace pythia {
        /**
         * Pointer to Pythia instance
         */
        extern Pythia8::Pythia* pythia;
        /**
         * Pointer to Pythia PDF sets
         */
        extern Pythia8::PDF* pdf;
    }

    /**
     * Ensures that all required stuff is loaded and loads otherwise.
     */
    extern void ensure_chigen_is_initialized();
    /**
     * Initialize ChiGen with default seed ({@code gRandom->GetSeed()}).
     */
    extern void initialize();
    /**
     * Initialize ChiGen with specified seed.
     * 
     * @param seed seed
     */
    extern void initialize(long seed);
}

#endif	/* CHIGENCONTEXT_H */