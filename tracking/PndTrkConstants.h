#ifndef PndTrkConstants_H
#define PndTrkConstants_H 1


// definition of constants used in PndTrk.... classes;

// integer constants
#define MAXHITSINCELL		20
// the following MAXHITSINFIT cannot be too large because the fit
// crashes 'silently' for too much memory consumption in the character arrays
// or takes too long time;


// MAXHITSINFIT has to be 12 when using the GLPK fitting procedure; there is no limitation
// with the other type of fits;
// #define MAXHITSINFIT		12
#define MAXFOUNDCLUSTERS	100  // maximum allowed clusters in the event;
#define MAXHITSINCLUSTER	200  // maximum allowed hits in a cluster;
#define MAXHITSINFIT		50
#define MAXMVDMCPOINTS		2000
#define MAX_NOT_CONNECTED	1	// maximum # of STT "holes" in the hits of a track;
#define MAXSKEWHITSINFIT	8
#define MAXSTTHITSINTRACK	40
#define MINIMUMHITSPERTRACK	2
#define MINOUTERHITSPERTRACK	5
#define TIMEOUT			60



// floating point constants

// APOTEMA means the radius of a circle INSCRIBED in a Hexagon (this is the same as the
// mathematical definition); otherwise the name Radius is used instead;
#define APOTEMAMAXINNERPARSTRAW	23.246827
#define APOTEMAMAXSKEWSTRAW	31.517569 // delimitation of the skew area
#define APOTEMAMINOUTERPARSTRAW	31.863369
#define APOTEMAMINSKEWSTRAW	23.246827 // delimitation of the skew area
#define APOTEMASTRAWDETECTORMIN	16.119  // minimum APOTEMA of the Stt detector in  cm
					// this is the Radius of the circle INSCRIBED in the
					// smaller Hexagon delimiting the STT detector;
// B field in Tesla
#define BFIELD			2.
#define CVEL			2.99792  //  velocity of light
// DELTAnR = range of nR in TrkAssociatedParallelHitsToHelixquater
#define DELTAnR			2.   //range of nR in TrkAssociatedParallelHitsToHelixquater
#define DIAMETERSTRAWTUBE	1.
#define DIAMETERSTRAWTUBE2	1.
#define DIMENSIONSCITIL		2.85
#define ERRORPIXEL		0.02611
#define ERRORSTRIP		0.02611
#define ERRORSQPIXEL		0.00068175
#define ERRORSQSTRIP		0.00068175
#define PMAX			100.

//  RADIUSSCITIL = radius of the cylinder tangent to the SciTil tiles in the middle of each tile;
#define RADIUSSCITIL		49.27

// maximum radius of the Stt detector in  cm; this is the radius CIRCUMSCRIBED to the outer Hexagon
// delimiting the STT detector;
#define RSTRAWDETECTORMAX	40.73
//  semilength of a axial straw;
#define	SEMILENGTH_STRAIGHT	75.
#define STRAWRADIUS		0.5
#define STRAWRESOLUTION		0.015
#define STRAW_SKEW_INCLINATION	3.
#define STTDRIFTVEL		0.0025	//   in cm/nsec
#define VERTICALGAP		4. // (cm) gap between Left and Right sections of detector.
// Z position of the center of the axial straw;
#define	ZCENTER_STRAIGHT	35.

#define PI			3.141592654
#define TWO_PI			6.283185307



#endif
