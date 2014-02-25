#ifndef PndTrkCTGeometryCalculations_H
#define PndTrkCTGeometryCalculations_H 1

// Root includes
#include "TROOT.h"



class PndTrkCTGeometryCalculations : public TObject
{

 public:

  /** Default constructor **/
  PndTrkCTGeometryCalculations(){};


  /** Destructor **/
  ~PndTrkCTGeometryCalculations(){};





  Double_t CalculateArcLength(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t Charge,
	Double_t *Xcross, // entrance-exit point
	Double_t *Ycross // entrance-exit point
	);


  bool CalculateCircleThru3Points(
	Double_t x1,
	Double_t y1,
	Double_t x2,
	Double_t y2,
	Double_t x3,
	Double_t y3,
	Double_t *o_x,
	Double_t *o_y,
	Double_t *r_r
	);

  void calculateintersections(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t C0x,
	Double_t C0y,
	Double_t C0z,
	Double_t r,
	Double_t vx,
	Double_t vy,
	Double_t vz,
	Int_t *STATUS,
	Double_t* POINTS
	);



  void CalculateSandZ(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t STRAWRESOLUTION,
	Short_t skewnum,
	Double_t info[][7],
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	Double_t S[2],
	Double_t Z[2],
	Double_t Zdrift[2],
	Double_t Zerror[2]
	);


  void ChooseEntranceExitbis(
	Double_t Oxx,
	Double_t Oyy,
	Short_t  Charge,
	Double_t FiStart,
	Short_t nIntersections,
	Double_t *XintersectionList,
	Double_t *YintersectionList,
	Double_t Xcross[2],	// output
	Double_t Ycross[2]	// output
	);



  void ChooseEntranceExit3(
	Double_t Oxx,
	Double_t Oyy,
	Short_t  Charge,
	Double_t FiStart,
	Short_t nIntersections,
	Double_t *XintersectionList,  // input and output;
	Double_t *YintersectionList  // input and output;
	);


  Double_t Dist_SZ(
	Double_t Rr,
	Double_t KAPPA,
	Double_t FI0,
	Double_t ZED,
	Double_t S,
	Int_t *nrounds
	);



  Double_t FindDistance(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t tanlow,
	Double_t tanmid,
	Double_t tanup,
	Double_t alfa,
	Double_t beta,
	Double_t gamma
	);

  void  FindingParallelTrackAngularRange(
	Double_t oX,
	Double_t oY,
	Double_t Rr,
	Short_t  Charge,
	Double_t *Fi_low_limit,	// Fi (in XY Helix frame) lower limit using
		// the Stt detector minimum/maximum radius
		// Fi_low_limit is ALWAYS between 0. and 2PI
	Double_t *Fi_up_limit,	// Fi (in XY Helix frame) upper limit using
		// the Stt detector maximum/minimum radius
		// Fi_up_limit is ALWAYS > Fi_low_limit and
		// possibly > 2PI.
	Short_t * status,
	Double_t Rmin,	// Rmin of cylindrical volume intersected by track;
	Double_t Rmax	// Rmax of cylindrical volume intersected by track;
	);


void  FindingParallelTrackAngularRange2(
	Double_t oX,	// input;
	Double_t oY,	// input;
	Double_t Rma,	// Rmax of cylindrical volume intersected by track;
	Double_t Rmi,	// Rmin of cylindrical volume intersected by track;
	Double_t Rr,	// input;
	Double_t *Fi_low_limit,	// output; Fi (in XY Helix frame) lower limit using
		// the Stt detector minimum/maximum radius
		// Fi_low_limit is ALWAYS between 0. and 2PI;
	Double_t *Fi_up_limit,	// output;	// Fi (in XY Helix frame) upper limit using
		// the Stt detector maximum/minimum radius
		// Fi_up_limit is ALWAYS > Fi_low_limit and
		// possibly > 2PI;
	Short_t * status	// output;
	);

  Short_t FindIntersectionsOuterCircle(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RMax,
	Double_t Xcross[2],
	Double_t Ycross[2]
	);



  Short_t FindTrackEntranceExitbiHexagonLeft(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Double_t Xcross[2],
	Double_t Ycross[2]
	);


  Short_t FindTrackEntranceExitbiHexagonLeft2(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Short_t &nIntersections,
	Double_t XintersectionList[16],
	Double_t YintersectionList[16]
	);

  Short_t FindTrackEntranceExitbiHexagonRight(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Double_t Xcross[2],
	Double_t Ycross[2]
	);


  Short_t FindTrackEntranceExitbiHexagonRight2(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Short_t &nIntersections,
	Double_t XintersectionList[16],
	Double_t YintersectionList[16]
	);

  Short_t FindTrackEntranceExitHexagonCircleLeft(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Double_t GAP,
	Double_t Xcross[2],
	Double_t Ycross[2]
	);


  Short_t FindTrackEntranceExitHexagonCircleLeft2(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Double_t GAP,
	Short_t &nIntersections,
	Double_t XintersectionList[12],
	Double_t YintersectionList[12]
	);

  Short_t FindTrackEntranceExitHexagonCircleRight(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Double_t GAP,
	Double_t Xcross[2],
	Double_t Ycross[2]
	);

  Short_t FindTrackEntranceExitHexagonCircleRight2(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Short_t  Charge,
	Double_t Start[3],
	Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
	Double_t ApotemaMax,
	Double_t GAP,
	Short_t &nIntersections,
	Double_t XintersectionList[12],
	Double_t YintersectionList[12]
	);

  bool IntersectionCircle_Segment(
	Double_t a, // coefficients implicit equation.
	Double_t b, // of segment : a*x + b*y + c =0.
	Double_t c,
	Double_t P1x, // point delimiting the segment.
	Double_t P2x, // point delimiting the segment.
	Double_t P1y, // point delimiting the segment.
	Double_t P2y, // point delimiting the segment.
	Double_t Oxx, // center of circle.
	Double_t Oyy,
	Double_t Rr, // Radius of circle.
	Short_t * Nintersections,
	Double_t XintersectionList[2],
	Double_t YintersectionList[2],
	Double_t *distance
	);



  bool IntersectionSciTil_Circle(
	Double_t DIMENSIONSCITIL,
	Double_t posizSciTilx,
	Double_t posizSciTily,
	Double_t Oxx, // center of circle.
	Double_t Oyy,
	Double_t Rr, // Radius of circle.
	Short_t * Nintersections,
	Double_t XintersectionList[2],
	Double_t YintersectionList[2]
	);


  Short_t   IntersectionsWithClosedbiHexagonLeft(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Ami,	// Apotema min of inner Hexagon;
	Double_t Ama,	// Apotema max of outer Hexagon;

	//-------- outputs
	Short_t *nIntersections,
	Double_t *XintersectionList,
	Double_t *YintersectionList
	);

  Short_t IntersectionsWithClosedbiHexagonRight(
	Double_t vgap,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Ami,	// Apotema min of inner Hexagon;
	Double_t Ama,	// Apotema max of outer Hexagon;

	//-------- outputs
	Short_t *nIntersections,
	Double_t *XintersectionList,
	Double_t *YintersectionList
	);


  Short_t  IntersectionsWithClosedPolygon(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t Rmi,	// Rmin of cylindrical volume intersected by track;
	Double_t Rma,	// Rmax of cylindrical volume intersected by track;

	//-------- outputs
	Short_t nIntersections[2],
	Double_t XintersectionList[][2],
	Double_t YintersectionList[][2]
	);



  Short_t IntersectionsWithGapSemicircle(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t gap,
	bool left,
	Double_t Rma,
	Double_t *XintersectionList,
	Double_t *YintersectionList
	);

  Short_t IntersectionsWithOpenPolygon(
	Double_t Oxx, // Track parameter
	Double_t Oyy, // Track parameter
	Double_t Rr, // Track parameter
	Short_t nSides, // input, n. of Sides of open Polygon.
	Double_t *a, //  coefficient of formula :  aX + bY + c = 0 defining
	Double_t *b, //  the Polygon sides.
	Double_t *c,
	Double_t *side_x,  // X,Y coordinate of the Sides vertices (in sequence, following
	Double_t *side_y,  // the Polygon along.
	//-------- outputs
	Double_t *XintersectionList, // XintersectionList
	Double_t *YintersectionList // YintersectionList.
	);


  bool IsInsideArc(
	Double_t Oxx,
	Double_t Oyy,
	Short_t Charge,
	Double_t Xcross[2],
	Double_t Ycross[2],
	Double_t Spoint
	);


  bool IsInTargetPipe(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t fi0,
	Double_t kappa,
	Short_t charge,
	Double_t gap
	);



  bool IsInternal(
	Double_t Px,	// point
	Double_t Py,
	Double_t Xtraslation,
	Double_t Ytraslation,
	Double_t Theta
	);





  ClassDef(PndTrkCTGeometryCalculations,1);

};

#endif
