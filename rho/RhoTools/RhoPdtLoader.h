#ifndef RHOPDTLOADER_H
#define RHOPDTLOADER_H
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

#include "TDatabasePDG.h"

class RhoPdtLoader
{
public:
    static TDatabasePDG* ReadPDGTable(const char *filename, bool force=false); //Read the EvtGen style pdt.table

private:
    RhoPdtLoader() {}
    ~RhoPdtLoader() {}
public:
    ClassDef(RhoPdtLoader, 1)   // EvtGen Particle Data Table
};

#endif
