//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoPdtLoader                                 //
//                                                                      //
// Read the EvtGen style PDT.                       //
//                                                                      //
// Author: Marcel Kunze, RUB, 1999-2000                 //
// Copyright (C) 1999-2001, Ruhr-University Bochum.         //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/RhoPdtLoader.h"
#include <iostream>
#include <sstream>
#include <string>
#include "Rtypes.h"
#include "TError.h"
#include "TList.h"

using namespace std;

ClassImp(RhoPdtLoader)

// Read the EvtGen table

TDatabasePDG* RhoPdtLoader::ReadPDGTable(const char *filename, bool force)
{
    TDatabasePDG* pdgtable = TDatabasePDG::Instance();
    if(force) {
      //cout<<"RhoPdtLoader: old pdg "<<pdgtable<<endl;
      pdgtable->~TDatabasePDG();
      //cout<<"RhoPdtLoader: deleted pdg "<<pdgtable<<endl;
      pdgtable = TDatabasePDG::Instance();
      //pdgtable = new TDatabasePDG();
      //cout<<"RhoPdtLoader: new pdg "<<pdgtable<<endl;
      Info("RhoPdtLoader","Forcing an empty PDG table start.");
      pdgtable->ReadPDGTable("/dev/null"); // ensure to have a total empty table
    }
      
    // read list of particles from a file in EvtGen format.
    // if the particle list does not exist, it is created, otherwise
    // particles are added to the existing list.
    // See $ROOTSYS/etc/pdg_table.txt and EvtGen/evt.pdl to see the
    // file formats.

    const Float_t HBARC = 197.327*1.e-3*1.e-13; // GeV*cm

    FILE *ifl = fopen(filename,"r");
    if (ifl == 0) {
        Error("ReadPDGTable","Could not open PDG particle file %s",filename);
        return pdgtable;
    }

    char line[512];
    while ( fgets(line,512,ifl) ) {
        if (strlen(line) >= 511) {
            Error("ReadPDGTable","input line is too long");
            return pdgtable;
        }
        istringstream linestr(line);
        string opcode;
        char subcode;
        linestr >> opcode >> subcode;

        if ( opcode == "*" )
            continue;

        if ( opcode == "end" )
            break;

        else if ( opcode == "add" ) {
            switch (subcode) {
            case 'p':
            {
                string classname;
                linestr >> classname;
                // if (classname == "Collision" || classname == "Parton")
                if (classname == "Collision" )
                    continue;

                string name;
                int type;
                float mass, width, cut, charge, spin, lifetime;

                linestr >> name >> type;
                linestr >> mass >> width >> cut >> charge;
                linestr >> spin >> lifetime;

                //charge /= 3.0;
                if (classname != "Meson")
                    spin /= 2.0;

                // lifetime is c*tau (mm)
                if (lifetime > 0.0 && width < 1e-10)
                    width = HBARC / (lifetime/10.0);

                Bool_t stable = (lifetime <= 0);

                pdgtable->AddParticle(name.c_str(), name.c_str(), mass, stable, width,
                                      charge, classname.c_str(), type, -1, 0);
                break;
            }

            case 'c':
            {
                int     ptype, nchild;
                float   bf;
                string decayer;

                linestr >> ptype >> bf >> decayer >> nchild;
                TParticlePDG *parent = pdgtable->GetParticle(ptype);
                if (parent == 0) continue;

                TList *kids = new TList();

                int i;
                for (i=0; i<nchild; i++ )
                {
                    int ctype;
                    linestr >> ctype;
                    TParticlePDG* secondary = pdgtable->GetParticle(ctype);
                    if ( secondary ==0 ) break;
                    kids->Add(secondary);
                }

                //parent->AddDecay(bf, kids ); // Not yet implemented
                break;
            }

            case 'd':
                break;

            default:
                Error("ReadPDGTable","unknown subcode %d for operation add",subcode);
                break;
            }
        }
    }

    fclose(ifl);

    return pdgtable;
}
