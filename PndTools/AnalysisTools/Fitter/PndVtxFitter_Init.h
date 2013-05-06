// kfitter_ini of KFitter
//
#ifndef PNDVTXFITTER_INIT_H
#define PNDVTXFITTER_INIT_H

const unsigned KF_PARAMETER_NUMBER    =  6;
const unsigned KF_SUBPARAMETER_NUMBER =  7;
const unsigned KF_NUM6                =  6;
const unsigned KF_NUM7                =  7;
const unsigned KF_BEFORE_FIT          =  0;
const unsigned KF_AFTER_FIT           =  1;
const unsigned KF_AT_DECAY_POINT      =  0;
const unsigned KF_NOT_DECAY_POINT     =  1;
const unsigned KF_NO_OVER_ITERATION   =  0;
const unsigned KF_OVER_ITERATION      =  1;
const double   KF_PHOTON_VELOCITY     =  0.00299792458;

const int      KF_FIX_MASS            =  1;
const int      KF_UNFIX_MASS          =  0;

const unsigned KF_MODE_WO_CORRELATION   = 0;
const unsigned KF_MODE_WITH_CORRELATION = 1;

const unsigned KF_MAX_TRACK_NUMBER     = 10;
const unsigned KF_MAX_TRACK_NUMBER2    = 30;
const unsigned KF_MAX_ITERATION_NUMBER = 15;
const double   KF_INIT_CHI2            = 1.0e+30;

const double   KF_MAGNETIC_FIELD       = 2.;

#define KF_WITH_OLD_INTERFACE 1
#define KF_WITH_BELLE_INTERFACE 1
const unsigned KF_DUMP_MEASUREMENT = 0;
const unsigned KF_DUMP_CORE_MATRIX = 1;
const unsigned KF_DUMP_FITTED      = 2;

#endif
