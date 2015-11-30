#ifndef PNDCADISPLAY_H
#define PNDCADISPLAY_H

#ifdef CATRACKER_DISPLAY

#include "PndCAParameters.h"
#include "PndCAPerformanceBase.h"
#include "PndCANPletsV.h"

class PndCAGBTracker;
class PndCATracker;
class PndCATrack;
class PndCATrackParam;
class PndCAParam;
class PndCAPerformance;
class PndCAMCTrack;
class PndCALocalMCPoint;
class TCanvas;
class TPad;
class PndCAHit;
class PndCAHitV;
class PndCAHitsV;
class PndCATracks;

#include "TArc.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TArrow.h"
#include "TBox.h"
#include "TCrown.h"
#include "TMarker.h"
#include "TLatex.h"

#include <vector>
using std::vector;


/**
 * @class PndCADisplay
 */
class PndCADisplay
{
    
public:
    
    class PndCADisplayTmpHit;
    
    static PndCADisplay &Instance();
    
    PndCADisplay();
    
    virtual ~PndCADisplay();
    
    void Init();
    void Update();
    void ClearView();
    void Ask();
    void SetTPC( const PndCAParam& tpcParam);  // iklm
    void SetGB( const PndCAGBTracker * GBTracker );
    
    const PndCAGBTracker * GetGB(){return fGB;};
    int GetColor( int i ) const;
    int GetColorZ( double z ) const ;
    int GetColorY( double y ) const ;
    int GetColorK( double k ) const ;
    int GetTrackMC( const PndCADisplayTmpHit *vHits, int NHits );
    
    
    void DrawTPC();
    void DrawFwdMvd();
    void DrawFwdGBHits( const PndCAGBTracker &tracker);
    void DrawSlice( PndCATracker *slice, bool DrawRows = 0, bool DrawGrid = 1 );
    
    void DrawArc(float x, float y, float r, int Start = 1, Size_t width = 1 );
    void DrawPoint(float x, float y, float z, int Start = 1, Size_t width = 1 );
    
    void DrawGBPoint(float x, float y, float z, int Start = 1, Size_t width = 1 );
    void DrawGBPoint(float x, float y, float z, float angle = 0, int Start = 1, Size_t width = 1 );
    void DrawSliceOutTrackParam( int itr, int color, Size_t width );
    void DrawSliceOutTrack1( int itr, int color, Size_t width );
    void DrawHelix(float p0, float c, float z, float zStart, float z0, float xc, float yc, float r, float b, int color, Size_t width);
    void DrawParticleGlobal(float *param, float q, float tStart, float tEnd, float b, int color=kOrange, Size_t width = 1);
    void DrawParticleGlobal(float *param, float q, float n[4], float b, int color=kOrange, Size_t width = 1);
    void DrawGBLine(float x, float y, float z, float x2, float y2, float z2, int Start=1, Size_t width=1, int projection = -1 );
    
    bool DrawTrack( PndCATrackParam t, double Alpha, const PndCADisplayTmpHit *vHits,
                   int NHits, int color = -1, Size_t width = -1, bool pPoint = 0 );
    
    void DrawGBTrack( int itr, int color = -1, int width = -1 );
    void DrawRecoTrack( int itr, int color = -1, int width = -1 );
    void DrawMCTrack( int itr, int color = -1, int width = -1 );
    
    void HitToGlobal( const PndCAHit& hit, float& x, float& y, float &z );
    void HitToGlobal( const PndCAHitV& hit, int iV, float& x, float& y, float &z );
    
    void DrawGBHits    (const PndCAHits&     all);
    void DrawGBPoints();
    void DrawPVHisto(const vector<float>& pvHist, const PndCAParam& param);
    void DrawGBNPlets(const PndCANPletsV& all);
    void DrawGBTracks(const PndCATracks&  all);
    
    void DrawGBTrackFast( const PndCAGBTracker &tracker, int itr, int color = -1 );
    bool DrawTracklet( PndCATrackParam &track, const int *hitstore, int color = -1, int width = -1, bool pPoint = 0 );
    
    //     void DrawGBLinks( const PndCAGBTracker &tracker, int color = -1, Size_t width = -1 );
    
    void DrawGBHit( const PndCAGBTracker &tracker, int iHit, int color = -1, Size_t width = -1 );
    void DrawGBHits( const PndCAGBTracker &tracker, int color = -1, Size_t width = -1, int hitsType = -1 );
    
    void DrawTrackParam( PndCATrackParam t, int color = 1 );
    
    void SaveCanvasToFile( TString fileName);
    
    TPad *CanvasYX() { return fYX; }
    TPad *CanvasZX() { return fZX; }
    
protected:
    
    TCanvas *fCanvas;              // the canvas
    TPad *fYX, *fZX, *fZR;               // two views
    TCanvas *fFwdMvdCanvas;//, *fFwdMvdXYCanvas;              // the canvas
    TPad *fFwdMvdZY, *fFwdMvdZX, *fFwdMvdZR; //*fFwdMvdXY;               // two views
    bool fAsk;                      // flag to ask for the pressing key
    const PndCAGBTracker *fGB;      // the Aliglobal tracker
    PndCAPerformance *fPerf; // Performance class (mc labels etc)
    double fZMin, fZMax, fYMin, fYMax;// view parameters
    double fRInnerMin, fRInnerMax, fROuterMin, fROuterMax, fTPCZMin, fTPCZMax; // view parameters
    
    TArc fArc;       // parameters of drawing objects are copied from this members
    TLine fLine;     //!
    TPolyLine fPLine;//!
    TMarker fMarker; //!
    TBox fBox;       //!
    TCrown fCrown;   //!
    TLatex fLatex;   //!
    TArrow fArrow;
    
    bool fDrawOnlyRef; // draw only clusters from ref. mc tracks
    
private:
    PndCADisplay( const PndCADisplay& );
    PndCADisplay& operator=( const PndCADisplay& );
};

#endif //CATRACKER_DISPLAY

#endif
