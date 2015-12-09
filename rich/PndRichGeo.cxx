#include "PndRichGeo.h"
#include "FairGeoNode.h"

ClassImp(PndRichGeo)

// -----   Default constructor   -------------------------------------------
PndRichGeo::PndRichGeo()
  : FairGeoSet()
{
  // Constructor
  // fName has to be the name used in the geometry for all volumes.
  // If there is a mismatch the geometry cannot be build.
  fName="rich";
  maxSectors=0;
  maxModules=10;
   
  fRichOffset = TVector3( 0, 0, 700-35-10/*+50*/ ); //-70
  fAlBoxSize = TVector3( 300, 300, 100 );
  fAlBoxWallThickness = 0.05;
  fAerogelSize = TVector3( 290, 120, 4 );
  fAerogelOffset = TVector3( 0, 0, 1 );
  fnOpt = std::vector<Double_t>(1,1.05);
  fAerogelLayers = std::vector<Double_t>(1,1);
  fAngleExtansionInner = 1.0;
  fAngleExtansionOuter = 1.0;
  fMirrorCurvature = 20;
  fAngleOfMirrorPosition = 50;
  fMirrorThickness = 0.1;
  fMirrorLength = 290;
  fPhDetLength = 290;
  fPhDetThickness = 3;
  fBeamPipeHoleX = 10;
  fBeamPipeHoleY = 10;

   fMirrorType = 0;
   fFlatMirrorZ = std::vector<Double_t>(2);
   fFlatMirrorY = std::vector<Double_t>(2);
   fFlatMirrorZGlob = std::vector<Double_t>(2);
   fFlatMirrorYGlob = std::vector<Double_t>(2);
   fPhDetZ = std::vector<Double_t>(2);
   fPhDetY = std::vector<Double_t>(2);
}

void PndRichGeo::init(size_t ver) {
   double ka1                      = angleExtansionInner();
   double ka2                      = angleExtansionOuter();
   double beta                     = mirrorCurvature();
   double alpha                    = angleOfMirrorPosition();
    // Mirror
    double za = richOffset().Z() /*+ alBoxWallThickness()*/ + aerogelOffset().Z();
    double ya = aerogelSize().Y()/2;
    double wa = aerogelSize().Z();
    double yp1 = ya + wa;
    double zp1 = wa;
    alpha = alpha < 45 - beta ? alpha : 45 -beta;
    alpha *= M_PI/180;
    beta *= M_PI/180;
    double thetaCh = acos(1/nOpt()[0]);
    double theta = atan(ya/za);
    double alpha1 = 2*(alpha+beta) - ka1*thetaCh;
    double alpha2 = 2*(alpha-beta) + ka2*thetaCh + theta;
    double alpham = (alpha1+alpha2)/2;
    double zm1 = zp1 + yp1/tan(alpha1);
    double ym1 = 0;
    double zm2 = (zm1+ya*tan(alpha))/(1-tan(theta+ka2*thetaCh)*tan(alpha));
    double ym2 = (zm2-zm1)/tan(alpha);
    double zp2 = ((ym2-yp1+zm2*tan(alpha2))*tan(alpham)+zp1)/(1+tan(alpha2)*tan(alpham));
    double yp2 = yp1 + (zp2-zp1)/tan(alpham);
    if (yp2<ym2) {
       yp2 = ym2;
       zp2 = zm2;
       alpham = atan((zp2-zp1)/(yp2-yp1));
    }
    double phDetWidth = sqrt((zp1-zp2)*(zp1-zp2)+(yp1-yp2)*(yp1-yp2));
    double zmc = (zm1+zm2)/2;
    double ymc = (ym1+ym2)/2;
    double wm = sqrt((zm1-zm2)*(zm1-zm2)+(ym1-ym2)*(ym1-ym2));
    double rm = wm/2/sin(beta);
    double hm = rm*cos(beta);
    double zm0 = zmc-hm*cos(alpha)-alBoxSize().Z()/2+aerogelOffset().Z();
    double ym0 = ymc+hm*sin(alpha);
    double theta1 = 360-(alpha+beta)*180/M_PI;
    double theta2 = 360-(alpha-beta)*180/M_PI;
    double theta3 = 360-theta2;

   // photosensor pixel sizes
   fdX = 0.38016;
   fdY = 0.32;
   fdZ = 0;
   fiXmax = 2*(int)(fMirrorLength/2/fdX);
   fiYmax = 0; // see further
   
    fAerogelEntrancePositionZ = za;
   
    fMirrorRadius = rm;
    fMirrorAxis = TVector3( 0, ymc+hm*sin(alpha), za + zmc-hm*cos(alpha) );
    theta1 -= theta1<180 ? 0 : 360;
    theta2 -= theta2<180 ? 0 : 360;
    fMirrorThetaMin = theta1*M_PI/180;
    fMirrorThetaMax = theta2*M_PI/180;

   UInt_t nm;
   Double_t yShift, zShift, dn;

   size_t nlayers = 3;
   switch (nlayers)
   {
    case 2:
      fnOpt.resize(2);
      fnOpt[0] = 1.0487;
      fnOpt[1] = 1.0513;
      dn = 0.0013*1.75;
      //dn = 0.001; //optimal value of ref. index
      fnOpt[0] = 1.05 - dn;
      fnOpt[1] = 1.05 + dn;
      fAerogelLayers.resize(2);
      fAerogelLayers[0] = 0.5;
      fAerogelLayers[1] = 0.5;
      break;
    case 3:
      fnOpt.resize(3);
      //dn = 0.0013*2.0;
      dn = 0.0013*0.944737; //optimal value of ref. index
      fnOpt[0] = 1.05 - dn;
      fnOpt[1] = 1.05;
      fnOpt[2] = 1.05 + dn;
      fAerogelLayers.resize(3);
      fAerogelLayers[0] = 0.333333;
      fAerogelLayers[1] = 0.333334;
      fAerogelLayers[2] = 0.333333;
      break;
    default:
      break;
   }   
   yShift = 0;
   zShift = richOffset().Z() + aerogelOffset().Z();
   switch (ver)
   {
    case 1: // round mirror
      fPhDetY[0] = 60;
      fPhDetY[1] = 82.5719;
      fPhDetZ[0] = 5.00000000000000;
      fPhDetZ[1] = 58.957;
      fMirrorRadius = 130.35;
      fMirrorAxis = TVector3( 0, 100.366, -70.1726 ); //z relative to aerogel entrence
      fMirrorAxisGlob = TVector3( 0, 100.366+yShift, -70.1726+zShift ); //z relative to aerogel entrence
      fMirrorThetaMin = -0.878805;
      fMirrorThetaMax = -0.13694;
      break;
    case 11: // flat mirror (one part)
      fFlatMirrorY.resize(2);
      fFlatMirrorZ.resize(2);
      fFlatMirrorY[0] = 0;
      fFlatMirrorY[1] = 0;
      fFlatMirrorZ[0] = 0;
      fFlatMirrorZ[1] = 0;
      break;
    case 12: // flat mirror (two part)
      fFlatMirrorY.resize(3);
      fFlatMirrorZ.resize(3);
      fFlatMirrorY[0] = 0;
      fFlatMirrorY[1] = 0;
      fFlatMirrorY[2] = 0;
      fFlatMirrorZ[0] = 0;
      fFlatMirrorZ[1] = 0;
      fFlatMirrorZ[2] = 0;
      break;
    case 13: // flat mirror (three part)
      fFlatMirrorY.resize(4);
      fFlatMirrorZ.resize(4);
      // without refraction on the surface
      fFlatMirrorY[0] = 0;
      fFlatMirrorY[1] = 11.5823995523040;
      fFlatMirrorY[2] = 37.0077431709015;
      fFlatMirrorY[3] = 70.3521752085729;
      fFlatMirrorZ[0] = 12.6022270511333;
      fFlatMirrorZ[1] = 26.1117457127181;
      fFlatMirrorZ[2] = 46.4793071925098;
      fFlatMirrorZ[3] = 55.7853332421188;
      // with refraction on the surface
      fFlatMirrorY[0] = 0;
      fFlatMirrorY[1] = 10.6711191205503;
      fFlatMirrorY[2] = 36.5334780846887;
      fFlatMirrorY[3] = 70.9145714437628;
      fFlatMirrorZ[0] = 13.5631217316887;
      fFlatMirrorZ[1] = 25.9624125206647;
      fFlatMirrorZ[2] = 47.2843451066186;
      fFlatMirrorZ[3] = 57.1886957783356;
      fPhDetY[0] = 50;
      fPhDetY[1] = fFlatMirrorY[3];
      fPhDetZ[0] = 5.00000000000000;
      fPhDetZ[1] = fFlatMirrorZ[3];
      // with refraction on the surface
      fFlatMirrorY[0] = 0;
      fFlatMirrorY[1] = 15.4507438834428;
      fFlatMirrorY[2] = 53.6398528340896;
      fFlatMirrorY[3] = 86.6061734692679;
      fFlatMirrorZ[0] = 19.4472941521775;
      fFlatMirrorZ[1] = 36.1696395672866;
      fFlatMirrorZ[2] = 63.5142211603757;
      fFlatMirrorZ[3] = 67.0974637738877;
      fPhDetY[0] = 60;
      fPhDetY[1] = fFlatMirrorY[3];
      fPhDetZ[0] = 5.00000000000000;
      fPhDetZ[1] = fFlatMirrorZ[3];
//      fnOpt.resize(2);
//      fnOpt[0] = 1.0487;
//      fnOpt[1] = 1.0513;
//      dn = 0.0013*1.75;
//      dn = 0.001; //optimal value of ref. index
//      fnOpt[0] = 1.05 - dn;
//      fnOpt[1] = 1.05 + dn;
//      fAerogelLayers.resize(2);
//      fAerogelLayers[0] = 0.5;
//      fAerogelLayers[1] = 0.5;
      nm = fFlatMirrorZ.size();
      fFlatMirrorYGlob.resize(nm);
      fFlatMirrorZGlob.resize(nm);
      yShift = 0;
      zShift = richOffset().Z() + aerogelOffset().Z();
      for(UInt_t i=0; i<nm; i++) {
         fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
         fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
      }
      break;
    default:
      break;
   }
   fPhDetAngle = std::atan((fPhDetY[1]-fPhDetY[0])/(fPhDetZ[1]-fPhDetZ[0]));
   fPhDetP0U = TVector3(0,fPhDetY[0] + yShift,fPhDetZ[0] + zShift);
   fPhDetNxU = TVector3(1,0,0);
   fPhDetNyU = TVector3(0,fPhDetY[1]-fPhDetY[0],fPhDetZ[1]-fPhDetZ[0]).Unit();
   fPhDetNzU = fPhDetNxU.Cross(fPhDetNyU).Unit();
   fPhDetP0D = TVector3(0,-fPhDetY[0] - yShift,fPhDetZ[0] + zShift);
   fPhDetNxD = TVector3(1,0,0);
   fPhDetNyD = TVector3(0,fPhDetY[1]-fPhDetY[0],-(fPhDetZ[1]-fPhDetZ[0])).Unit();
   fPhDetNzD = fPhDetNxD.Cross(fPhDetNyD).Unit();
   fiYmax = (int)(2*std::sqrt((fPhDetY[1]-fPhDetY[0])*(fPhDetY[1]-fPhDetY[0])+
                             (fPhDetZ[1]-fPhDetZ[0])*(fPhDetZ[1]-fPhDetZ[0]))/fdY);
}

TVector3 PndRichGeo::PhDetPositionLocal(TVector3 pos)
{
   //photodetector: cell size
   if (pos.Y()>=0) {
      TVector3 dP = pos - fPhDetP0U;
      return TVector3(dP*fPhDetNxU,dP*fPhDetNyU,dP*fPhDetNzU);
   }
   else {
      TVector3 dP = pos - fPhDetP0D;
      return TVector3(dP*fPhDetNxD,dP*fPhDetNyD,dP*fPhDetNzD);
   }
}

TVector3 PndRichGeo::PhDetPositionGlobal(TVector3 pos)
{
   if (pos.Y()>=0)
      return fPhDetP0U + pos.X()*fPhDetNxU + pos.Y()*fPhDetNyU + pos.Z()*fPhDetNzU;
   else
      return fPhDetP0D + pos.X()*fPhDetNxD + pos.Y()*fPhDetNyD + pos.Z()*fPhDetNzD;
}

TVector3 PndRichGeo::PositionDiscretization(TVector3 pos,
                                            Double_t dX,
                                            Double_t dY,
                                            Double_t dZ)
{
   Double_t dX_ = dX>0 ? dX : fdX;
   Double_t dY_ = dY>0 ? dY : fdY;
   Double_t dZ_ = dZ>0 ? dZ : fdZ;
   Double_t x = pos.X();
   Double_t y = pos.Y();
   Double_t z = pos.Z();
   if (x&&dX) x = ((int)(x/dX_) + x/std::fabs(x)/2)*dX_;
   if (y&&dY) y = ((int)(y/dY_) + y/std::fabs(y)/2)*dY_;
   if (z&&dZ) z = ((int)(z/dZ_) + z/std::fabs(z)/2)*dZ_;
   return TVector3(x,y,z);
}

UInt_t PndRichGeo::IndexX(TVector3 pos)
{
   return (int)((pos.X()+fdX*fiXmax/2)/fdX);
}

UInt_t PndRichGeo::IndexY(TVector3 pos)
{
   return (int)((pos.Y()+fdY*fiXmax/2)/fdY);
}

TVector3 PndRichGeo::PixelPositionLocal(UInt_t ix, UInt_t iy)
{
   Double_t x = ix*fdX-fiXmax*fdX/2+0.5*fdX;
   Double_t y = iy*fdY-fiYmax*fdY/2+0.5*fdY;
   return TVector3(x,y,0);
}

TVector3 PndRichGeo::PixelPositionGlobal(UInt_t ix, UInt_t iy)
{
   return PhDetPositionGlobal(PixelPositionLocal(ix,iy));
}

// -------------------------------------------------------------------------

const char* PndRichGeo::getModuleName(Int_t m)
{
  /** Returns the module name of PndRich number m
      Setting PndRich here means that all modules names in the
      ASCII file should start with PndRich otherwise they will
      not be constructed
  */
  sprintf(modName,"rich0%i",m+1);
  return modName;
}

const char* PndRichGeo::getEleName(Int_t m)
{
  /** Returns the element name of Det number m */
  sprintf(eleName,"rich0%i",m+1);
  return eleName;
}
