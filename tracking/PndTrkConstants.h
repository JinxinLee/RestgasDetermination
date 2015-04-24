#ifndef PndTrkConstants_H
#define PndTrkConstants_H 1


// definition of constants used in PndTrk.... classes;

// integer constants
const int MAXHITSINCELL = 20,
// the following MAXHITSINFIT cannot be too large because the fit
// crashes 'silently' for too much memory consumption in the character arrays
// or takes too long time;

	TIMEOUT = 60;


// B field in Tesla
const Double_t
	BFIELD=2.,
	CVEL=2.99792,  //  velocity of light
// DELTAnR = range of nR in TrkAssociatedParallelHitsToHelixquater
	DELTAnR=2.,   //range of nR in TrkAssociatedParallelHitsToHelixquater
	PMAX=100.,
	VERTICALGAP=4.; // (cm) gap between Left and Right sections of detector.




//----------------------------- Mvd constants, according to Mvd design;
const Double_t
	ERRORPIXEL=0.02611,
	ERRORSTRIP=0.02611,
	ERRORSQPIXEL=0.00068175,
	ERRORSQSTRIP=0.00068175;

	// averge radius of the various layers of the barrel part;
const int
	MAXMVDMCPOINTS=2000;

//--------------------------------------- MVD Barrel
// 0 --> innermost layer;  3 --> outermost layer;

const int MVD_BARREL_LAYERS= 4;  // number of barrel layers;

// Radia of the Barrel Mvd layers;
// MVD_BARREL_AVERAGE_RADIUS[i] == AVERAGE Radius of i-th layer;
// MVD_BARREL_ZLIMITS[0][i] = Minimum Z of i-th layer,
// MVD_BARREL_ZLIMITS[1][i] = Maximum Z of i-th layer,
// MVD_BARREL_NOZONE_X[0 or 1] = X lower and upper limits of the zone of the beam pipe;
// MVD_BARREL_NOZONE_X[0 or 1] = Z lower and upper limits of the zone of the beam pipe;

const Double_t
	MVD_BARREL_AVERAGE_RADIUS[4] = {
				(2.18+2.858)/2. ,
				(4.73+5.282)/2. ,
				(8.972+9.686)/2. ,
				(12.32+12.994)/2.},
	MVD_BARREL_RADIASQMean[4] = { 6.4762, 25.15,  87.15, 160.31 },
	MVD_BARREL_RADIASQDifference[4] = { 3.5, 5.5, 13.32, 17.06 },
	MVD_BARREL_NOZONE_X[2] = {-2.,2.},
	MVD_BARREL_NOZONE_Z[2] = {-2.,2.},
	MVD_BARREL_ZLIMITS[2][4] = {-3.98,-7.9,-13.376,-16.917,
				     13.9,13.9,13.9,13.9};
//---------------------------------------- MVD Disks
// 0 --> most upstream layer;  5 --> most downstream layer;
const int  MVD_DISK_LAYERS= 6;  // number of Mvd disks;
//  Z positions;
const Double_t
	MVD_DISK_Z[6] = { 1.97, 3.97, 6.97, 9.97, 14.77, 21.77} ;

// each layer has 3 or 6 different pieces of sensors;
const int
	MVD_DISK_PIECES[6] = { 3, 3, 6, 6, 6, 6};
// since each layer has 3 or 5 pieces; here use 5 as the 2nd dimension of MVD_DISK_LAYER_XMAX ;
const Double_t

/*
	MVD_DISK_LAYER_XMAX[6][5] ={	0., 1.16, 2.32, 0. , 0.,
					0., 1.16, 2.32, 0. , 0.,
					1.165, 2.33, 4.66, 5.825, 6.99,
					1.165, 2.33, 4.66, 5.825, 6.99,
					1.165, 2.33, 4.66, 5.825, 6.99,
					1.165, 2.33, 4.66, 5.825, 6.99
					},
	MVD_DISK_LAYER_XMIN[6][5] ={	0., 1.16, 2.32, 0. , 0.,
					0., 1.16, 2.32, 0. , 0.,
					0., 1.165, 2.33, 4.66, 5.825,
					0., 1.165, 2.33, 4.66, 5.825,
					0., 1.165, 2.33, 4.66, 5.825,
					0., 1.165, 2.33, 4.66, 5.825
					},

*/
	MVD_DISK_LAYER_deltaX[6] = { 1.16, 1.16, 1.165,1.165,1.165,1.165},

	// MVD_DISK_LAYER_YMAX[i][j] --> i = index identifying the Disk layer; j = index identifying the sensor strip on layer i;
	MVD_DISK_LAYER_YMAX[6][6] ={	3.43,  2.26,  1.12, 0. , 0., 0.,
					3.43,  2.26,  1.12, 0. , 0., 0.,
					6.87, 6.87, 5.73 , 5.73, 4.59, 2.31,
					6.87, 6.87, 5.73 , 5.73, 4.59, 2.31,
					6.87, 6.87, 5.73 , 5.73, 4.59, 2.31,
					6.87, 6.87, 5.73 , 5.73, 4.59, 2.31
					},

	MVD_DISK_LAYER_YMIN[6][6] ={	1.19, 0. , 0. , 0., 0., 0.,
					1.19, 0. , 0. , 0., 0., 0.,
					1.12, 0. , 0. , 0., 0., 0., 
					1.12, 0. , 0. , 0., 0., 0., 
					1.12, 0. , 0. , 0., 0., 0., 
					1.12, 0. , 0. , 0., 0., 0.
					}; 


//-----------------------------



//----------------------------- SciTil detector constants
//  RADIUSSCITIL = radius of the cylinder tangent to the SciTil tiles in the middle of each tile;
const Double_t
	RADIUSSCITIL=49.27,
	DIMENSIONSCITIL=2.85;
//-----------------------------

//----------------------------- STT detector constants
// maximum radius of the Stt detector in  cm; this is the radius CIRCUMSCRIBED to the outer Hexagon
// delimiting the STT detector;
const Double_t
	RSTRAWDETECTORMAX=40.73,
// APOTEMA means the radius of a circle INSCRIBED in a Hexagon (this is the same as the
// mathematical definition); otherwise the name Radius is used instead;
	APOTEMAMAXINNERPARSTRAW=23.246827,
	APOTEMAMAXSKEWSTRAW=31.517569, // delimitation of the skew area
	APOTEMAMINOUTERPARSTRAW=31.863369,
	APOTEMAMINSKEWSTRAW=23.246827, // delimitation of the skew area
	APOTEMASTRAWDETECTORMIN=16.119;  // minimum APOTEMA of the Stt detector in  cm
					// this is the Radius of the circle INSCRIBED in the
					// smaller Hexagon delimiting the STT detector;
const int 
	MAXFOUNDCLUSTERS=100,  // maximum allowed clusters in the event;
	MAXHITSINCLUSTER=200,  // maximum allowed hits in a cluster;
	MAXHITSINFIT=50,
	MAXSKEWHITSINFIT=8,
	MAXSTTHITSINTRACK=40,

	MINIMUMHITSPERTRACK=2,
	MINOUTERHITSPERTRACK=5,
	MAX_NOT_CONNECTED=1;	// maximum # of STT "holes" in the hits of a track;

const Double_t
	DIAMETERSTRAWTUBE=1.,
	DIAMETERSTRAWTUBE2=1.,
//  semilength of a axial straw;
	SEMILENGTH_STRAIGHT=75.,
	STRAWRADIUS=0.5,
	STRAWRESOLUTION=0.015,
	STRAW_SKEW_INCLINATION=3.,
	STTDRIFTVEL=0.0025,	//   in cm/nsec
// Z position of the center of the axial straw;
	ZCENTER_STRAIGHT=35.;

//-----------------------------


const Double_t
	PI=3.141592654,
	TWO_PI=6.283185307;



#endif
