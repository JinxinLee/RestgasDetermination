#ifndef PNDVTXFITTERERROR_H
#define PNDVTXFITTERERROR_H

//No error
const unsigned int KF_NO_ERROR   = 0;
//
//Main
//
const unsigned int KF_DIV_ZERO   = 1;
const unsigned int KF_TRACK_SIZE = 2;
const unsigned int KF_INVERSE    = 3;
const unsigned int KF_INIT_CHISQ = 4;
const unsigned int KF_ARCSIN     = 5;
//
//Input Interface
//
const unsigned int KF_INPUT_DIV_ZERO    = 11;
const unsigned int KF_INPUT_TRACK_SIZE  = 12;
const unsigned int KF_INPUT_MATRIX_SIZE = 13;
const unsigned int KF_INPUT_CORRE_SIZE  = 14;
//
//Output Interface
//
const unsigned int KF_OUTPUT_DIV_ZERO   = 21;
const unsigned int KF_OUTPUT_TRACK_NUM  = 22;
const unsigned int KF_OUTPUT_OUT_RANGE  = 23;
const unsigned int KF_OUTPUT_INVERSE    = 24;

#endif
