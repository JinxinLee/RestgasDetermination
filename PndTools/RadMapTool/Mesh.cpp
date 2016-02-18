
#include "Mesh.h"
#include <TMath.h>
#include <TFile.h>

#include <iostream>
#include <iomanip>
#include <utility>

#include "G4UnitsTable.hh"

#define verbose true

bool momentumfilter(TVector3 mom, TVector3 diff){
  bool back = true;
  for(int i = 0; i < 3; i++) if(mom(i)*diff(i) < 0) back = false;
  return back;
}

Plane::Plane(){
  SetNormal(TVector3(0, 0, 0));
  SetDistance(0);
  corner1 = TVector3(0, 0, 0);
  corner2 = TVector3(0, 0, 0);
  corner3 = TVector3(0, 0, 0);
}

Plane::Plane(TVector3 _corner1, TVector3 _corner2, TVector3 _corner3,
             double dist){
  corner1 = _corner1;
  corner2 = _corner2;
  corner3 = _corner3;
  TVector3 dpv1 = corner2-corner1;
  TVector3 dpv2 = corner3-corner1;
  TVector3 n = (dpv1.Cross(dpv2)).Unit();
  SetNormal(n);
  SetDistance(dist);

  // std::cout << "p1: " << corner1.X() << ' ' << corner1.Y() << ' ' << corner1.Z() << std::endl;
  // std::cout << "p2: " << corner2.X() << ' ' << corner2.Y() << ' ' << corner2.Z() << std::endl;
  // std::cout << "p3: " << corner3.X() << ' ' << corner3.Y() << ' ' << corner3.Z() << std::endl;
  // std::cout << "dp1: " << dpv1.X() << ' ' << dpv1.Y() << ' ' << dpv1.Z() << std::endl;
  // std::cout << "dp2: " << dpv2.X() << ' ' << dpv2.Y() << ' ' << dpv2.Z() << std::endl;
  // std::cout << "normal: " << n.X() << ' ' << n.Y() << ' ' << n.Z() << std::endl;
}

TVector3 Plane::GetCorner(int i){
  switch(i){
  case 1:
    return Corner1();
  case 2:
    return Corner2();
  case 3:
    return Corner3();
  default:
    return TVector3(0, 0, 0);
  }
}



TVector3 Plane::LineIntersection(TVector3 begline, TVector3 endline){
  // plane: (p-corner1)*n = 0
  //    line: p=t*(end-beg)+beg
  //    (t*(end-beg) + beg - corner1)*n=0
  //    t*(end-beg)*n+(beg-corner1)*n=0
  //    n*(corner1-beg)/(n*(end-beg))=t


  TVector3 diffl = endline-begline;
  float nDotBeg = normal*(corner1-begline);
  float nDotDiff = normal*diffl;
  float tt = nDotBeg/nDotDiff;

  TVector3 ixp = begline + tt*diffl;

  // std::cout << diffl.X() << ' ' << diffl.Y() << ' ' << diffl.Z() << "   "
  //           << normal.X() << ' ' << normal.Y() << ' ' << normal.Z() << "   "
  //           << nDotBeg << ' ' << nDotDiff << ' ' << tt << std::endl;
  return ixp;

}





TVector3 intersectLine(TVector3 l0, TVector3 l1,// two points of the line
                       TVector3 p0, TVector3 p1, TVector3 p2,
                       float& eff) {//three points of the plane
  //axes of the plane
  TVector3 dpv1 = p1-p0;
  TVector3 dpv2 = p2-p0;
  TVector3 n = (dpv1.Cross(dpv2)).Unit();


  // Rotate the points (of the line) to the plane
  TVector3 vRotRay1 = TVector3 (dpv1*(l0-p1), dpv2*(l0-p1), n*(l0-p1));
  TVector3 vRotRay2 = TVector3 (dpv1*(l1-p1), dpv2*(l1-p1), n*(l1-p1));

  // float fPercent = vRotRay1.Z() / (vRotRay2.Z()-vRotRay1.Z());
  float fPercent = vRotRay1.Z() / (vRotRay1.Z()-vRotRay2.Z());

  eff = fPercent;
  TVector3 ixp = l0 + fPercent*(l1-l0);
  return ixp;

  //a-b (-> b->a)
}


BoxMesh::BoxMesh(BoxMesh& m){
  _MeshHisto = (TH2D*)m._MeshHisto->Clone();
  _StatHisto = (TH2I*)m._StatHisto->Clone();
  _orientation = m._orientation;
  _rotate = m._rotate;
  _volume = m._volume;
  _Name = m._Name;

  // oldId = 0;
}


BoxMesh::BoxMesh(const char* name,
                 int Xbins, Double_t Xlow, Double_t Xhigh,
                 int Ybins, Double_t Ylow, Double_t Yhigh,
                 int Zbins, Double_t Zlow, Double_t Zhigh){//in cm!!!!

  _Name = TString(name);
  _Xbins = Xbins; _Xlow = Xlow; _Xhigh = Xhigh;  
  _Ybins = Ybins; _Ylow = Ylow; _Yhigh = Yhigh;  
  _Zbins = Zbins; _Zlow = Zlow; _Zhigh = Zhigh;  
  _filter = TFormula("formula", "1");
}

BoxMesh::BoxMesh(const char* name,
                 int xbins, Double_t xlow, Double_t xhigh,
                 int ybins, Double_t ylow, Double_t yhigh,
                 Double_t zlow, Double_t zhigh,
                 orientation plane,
                 quantity Quantity){//in cm!!!!

  _Name = TString(name);

  _orientation = plane;
  SetQuantity(Quantity);

  switch (_orientation){
  case XY:
    _Xbins = xbins;
    _Xlow  = xlow;
    _Xhigh = xhigh;

    _Ybins = ybins;
    _Ylow  = ylow;
    _Yhigh = yhigh;

    _Zlow  = zlow;
    _Zhigh = zhigh;
    break;
  case YX:
    _Xbins = ybins;
    _Xlow = ylow;
    _Xhigh = yhigh;

    _Ybins = xbins;
    _Ylow = xlow;
    _Yhigh = xhigh;

    _Zlow  = zlow;
    _Zhigh = zhigh;
    break;

  case XZ:
    _Xbins = xbins;
    _Xlow = xlow;
    _Xhigh = xhigh;

    _Ylow  = zlow;
    _Yhigh = zhigh;

    _Zbins = ybins;
    _Zlow = ylow;
    _Zhigh = yhigh;
    break;
  case ZX:
    _Xbins = ybins;
    _Xlow = ylow;
    _Xhigh = yhigh;

    _Zbins = xbins;
    _Zlow = xlow;
    _Zhigh = xhigh;

    _Ylow  = zlow;
    _Yhigh = zhigh;
    break;

  case YZ:
    _Xlow  = zlow;
    _Xhigh = zhigh;

    _Ybins = xbins;
    _Ylow = xlow;
    _Yhigh = xhigh;

    _Zbins = ybins;
    _Zlow = ylow;
    _Zhigh = yhigh;
    break;
  case ZY:
    _Xlow  = zlow;
    _Xhigh = zhigh;

    _Ybins = ybins;
    _Ylow = ylow;
    _Yhigh = yhigh;

    _Zbins = xbins;
    _Zlow = xlow;
    _Zhigh = xhigh;
    break;
  };


}

BoxMesh::~BoxMesh(){
  delete _StatHisto;
  delete _MeshHisto;
}

void BoxMesh::SetFilter(const char* filter){
  //Pid=x, Ch=y, En=z, 
  TString temps(filter);
  temps.ReplaceAll("Pid", "x");
  temps.ReplaceAll("Ch" , "y");
  temps.ReplaceAll("Mom", "z");

  _filter = TFormula("formula", temps.Data());
}

void BoxMesh::SetQuantity(quantity Quantity){
  _quantity = Quantity;
}

void BoxMesh::SetOrientation(orientation plane, 
                             Double_t rotate,
                             axis Ax){
  _orientation = plane;
  SetOrientation(rotate,Ax);
}

void BoxMesh::SetOrientation(Double_t rotate,
                             axis Ax){

  _axis = Ax;
  _rotate = (rotate == 99999) ? 0 : rotate;

  char C[32];
  switch (_orientation){
  case XY:
    makeHisto("XY", _rotate,
              _Xbins, _Xlow, _Xhigh,
              _Ybins, _Ylow, _Yhigh);
    break;
  case YX:
    makeHisto("YX", _rotate,
              _Ybins, _Ylow, _Yhigh,
              _Xbins, _Xlow, _Xhigh);
    break;

  case XZ:
    makeHisto("XZ", _rotate,
              _Xbins, _Xlow, _Xhigh,
              _Zbins, _Zlow, _Zhigh);
    break;
  case ZX:
    makeHisto("ZX", _rotate,
              _Zbins, _Zlow, _Zhigh,
              _Xbins, _Xlow, _Xhigh);
    break;

  case YZ:
    makeHisto("YZ", _rotate,
              _Ybins, _Ylow, _Yhigh,
              _Zbins, _Zlow, _Zhigh);
    break;
  case ZY:
    makeHisto("ZY", _rotate,
              _Zbins, _Zlow, _Zhigh,
              _Ybins, _Ylow, _Yhigh);
    break;
  };
  _volume =
    _MeshHisto->GetXaxis()->GetBinWidth(1) * 
    _MeshHisto->GetYaxis()->GetBinWidth(1) * 
    _MeshHisto->GetZaxis()->GetBinWidth(1);

  // std::cout << "; Volume: " << _volume << '\t' << _MeshHisto->GetName() << ", "
  //           << _MeshHisto->GetXaxis()->GetBinWidth(1) << ' '
  //           << _MeshHisto->GetYaxis()->GetBinWidth(1) << ' ' 
  //           << _MeshHisto->GetZaxis()->GetBinWidth(1) << ' ' 
  //           << std::cout << std::flush;

}
// void BoxMesh::transform(Double_t Vh, Double_t Vv){
void BoxMesh::Transform(Double_t X, Double_t Y, Double_t Z){
  _X = X; _Y = Y; _Z = Z;
  Transform(  X,  Y,  Z,
             _X, _Y, _Z);
}

void BoxMesh::Transform(Double_t  X,  Double_t  Y,  Double_t  Z,
                        Double_t& X0, Double_t& Y0, Double_t& Z0){

  if(_rotate == 99999) _rotate = 0;
  if((_rotate == 0)){
    X0 = X; Y0 = Y; Z0 = Z; 
  }else{

    Double_t V1 = 0;
    Double_t V2 = 0;

    X0 = X;
    Y0 = Y;
    Z0 = Z;
    switch (_axis){
    case Xx:
      V1 = Y;
      V2 = Z;
      X0 = X;
      break;
    case Yy:
      V1 = X;
      V2 = Z;
      Y0 = Y;
      break;
    case Zz:
      V1 = X;
      V2 = Y;
      Z0 = Z;
      break;
    };
    
    Double_t _Vh, _Vv;
    if(_rotate != 0){
      Double_t R = TMath::Sqrt(V1*V1 + V2*V2);
      Double_t rot0 = TMath::ATan(V1/V2)*TMath::RadToDeg();
      if(V2 < 0) rot0 += 180;
      Double_t drot = rot0-_rotate;
      _Vh = R*TMath::Sin(1*drot*TMath::DegToRad());
      _Vv = R*TMath::Cos(1*drot*TMath::DegToRad());
    }else{
      _Vh = V1;
      _Vv = V2;
    }

    switch (_axis){
    case Xx:
      switch (_orientation){
      case XY: case YX: case XZ: case ZX:
        Y0 = _Vv;
        Z0 = _Vh;
        break;

      case YZ:
        Y0 = _Vh;
        Z0 = _Vv;
        break;
      case ZY:
        Y0 = _Vv;
        Z0 = _Vh;
        break;
      };
      break;

    case Yy:
      switch (_orientation){
      case XY: case YX: case YZ: case ZY:
        X0 = _Vh;
        Z0 = _Vv;
        break;

      case XZ:
        X0 = _Vh;
        Z0 = _Vv;
        break;
      case ZX:
        X0 = _Vv;
        Z0 = _Vh;
        break;
      };
      break;

    case Zz:
      switch (_orientation){
      case XZ: case ZX: case YZ: case ZY:
        X0 = _Vh;
        Y0 = _Vv;
        break;

      case XY:
        X0 = _Vh;
        Y0 = _Vv;
        break;
      case YX:
        X0 = _Vv;
        Y0 = _Vh;
        break;
      };
      break;
    };
  }
}

void BoxMesh::Fill(FairRadMapPoint *p){

  Int_t pid = p->GetPdg();
  Int_t cha = 0;// to be implemented
  Double_t mom = TMath::Sqrt((p->GetPx()*p->GetPx()) +
                             (p->GetPy()*p->GetPy()) +
                             (p->GetPz()*p->GetPz()));
  if(_filter.Eval(pid, cha, mom)){

    Double_t val = 0;
    Double_t edep;
    Double_t dens;

    
    Int_t oldBinX, oldBinY, oldBinZ;
    Int_t newBinX, newBinY, newBinZ;
    Int_t oldBin;
    Int_t newBin;
    Double_t dx;
    Double_t dy;
    Double_t dz;

    TVector3 vstart, vend, vcent, bcorner;
    switch (_quantity){

    case Edep:
      val = p->GetEnergyLoss()*1.60217657*1e-10;
      if(!isnan(val) && val !=0)
        Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);  
      break;

    case Dose:
      edep = p->GetEnergyLoss()*1.60217657*1e-10;//GeV->Joule
      dens = p->GetDensity();
      dens *= 0.001;//g/cm3 -> kg/cm3
      
      val = edep/(dens*_volume);//mass=density*vol,dens[kg/cm^3],vol[cm^3]!
      if(!isnan(val) && val !=0){
        Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);  
      }
      break;

    case Fluence:    
      val = CalcFluence(p);
      if(!isnan(val) && val !=0){
        Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);  
      }
      break;

    case Density:
      val = dens;
      if(!isnan(val) && val !=0){
        Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);  
      }
      break;

    case Mass:
      val = (dens*_volume);
      if(!isnan(val) && val !=0){
        Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);  
      }
      break;

    case SimpleFluence:
      TVector3 poststepv   = TVector3(p->GetXOut(), p->GetYOut(), p->GetZOut());
      TVector3 prestepv    = TVector3(p->GetX(), p->GetY(), p->GetZ());
      val = (poststepv-prestepv).Mag();
      if(!isnan(val) && val !=0){
        Fill(p->GetXOut(), p->GetYOut(), p->GetZOut(), val);  
      }
      break;

    // default:
    //   val = 0;
    }
  }
}

// void BoxMesh::Fill(FairRadMapPoint *p, Double_t we){
void BoxMesh::Fill(Int_t gBin, Double_t val){
  Int_t binX, binY, binZ;
  _MeshHisto->GetBinXYZ(gBin, binX, binY, binZ);
  Fill(binX, binY, binZ, val);
}

void BoxMesh::Fill(Double_t X, Double_t Y, Double_t Z, Double_t we){
  Transform(X, Y, Z);
  if(IsInside(_tX, _tY, _tZ)){

    switch (_orientation){
    case XY:
      _MeshHisto->Fill(_tX, _tY, we);
      _StatHisto->Fill(_tX, _tY);
      break;
    case YX:
      _MeshHisto->Fill(_tY, _tX, we);
      _StatHisto->Fill(_tY, _tX);
      break;

    case XZ:
      _MeshHisto->Fill(_tX, _tZ, we);
      _StatHisto->Fill(_tX, _tZ);
      break;
    case ZX:
      _MeshHisto->Fill(_tZ, _tX, we);
      _StatHisto->Fill(_tZ, _tX);
      break;

    case YZ:
      _MeshHisto->Fill(_tY, _tZ, we);
      _StatHisto->Fill(_tY, _tZ);
      break;
    case ZY:
      _MeshHisto->Fill(_tZ, _tY, we);
      _StatHisto->Fill(_tZ, _tY);
      break;
    };
  }
}

void BoxMesh::Scale(Double_t sca){
  // std::cout << _MeshHisto << std::endl;
  _MeshHisto->Scale(sca);
}

TH2D* BoxMesh::GetHisto(){
  return _MeshHisto;
}

void BoxMesh::Save(TFile* fout){
  fout->mkdir(_MeshHisto->GetName());
  fout->cd   (_MeshHisto->GetName());
  _MeshHisto->Clone()->Write();
  _StatHisto->Clone()->Write();
  fout->cd();
}

void BoxMesh::Save(){
  _MeshHisto->Write();
  _StatHisto->Write();
}

bool BoxMesh::IsInside(Double_t X, Double_t Y, Double_t Z){
  bool ok = false;
  if(((_Xlow <= X) && (X <= _Xhigh)) &&
     ((_Ylow <= Y) && (Y <= _Yhigh)) &&
     ((_Zlow <= Z) && (Z <= _Zhigh)))
    ok = true;
  return ok;
}

bool BoxMesh::IsInside(FairRadMapPoint *p){
  bool ok = false;
  double xin, yin, zin, xout, yout, zout;
  Transform(p->GetX(), p->GetY(), p->GetZ(),
            xin, yin, zin);
  Transform(p->GetXOut(), p->GetYOut(), p->GetZOut(),
            xout, yout, zout);

  if((((xin   <= _Xlow) && (_Xhigh <= xout))   ||
      ((xout  <= _Xlow) && (_Xhigh <= xin ))   ||
      // ((_Xlow <= xin)   && (xin    <= _Xhigh)) ||
      ((_Xlow <= xout)  && (xout   <= _Xhigh))) &&
     (((yin   <= _Ylow) && (_Yhigh <= yout))   ||
      ((yout  <= _Ylow) && (_Yhigh <= yin ))   ||
      // ((_Ylow <= yin)   && (yin    <= _Yhigh)) ||
      ((_Ylow <= yout)  && (yout   <= _Yhigh))) &&
     (((zin   <= _Zlow) && (_Zhigh <= zout))   ||
      ((zout  <= _Zlow) && (_Zhigh <= zin ))   ||
      // ((_Zlow <= zin)   && (zin    <= _Zhigh)) ||
      ((_Zlow <= zout)  && (zout   <= _Zhigh))))
    ok = true;

  return ok;
}

void BoxMesh::makeHisto(const char* Orient, Double_t rotate,
                        int Hbins, Double_t Hlow, Double_t Hhigh,
                        int Vbins, Double_t Vlow, Double_t Vhigh){
  char C[32];
  sprintf(C, "%s_Histo_%s_%g", _Name.Data(), Orient, _rotate);
  _MeshHisto = new TH2D(C, C,
                        Hbins, Hlow, Hhigh,
                        Vbins, Vlow, Vhigh);
  sprintf(C, "%s_StatHisto_%s_%g", _Name.Data(), Orient, _rotate);
  _StatHisto = new TH2I(C, C,
                        Hbins, Hlow, Hhigh,
                        Vbins, Vlow, Vhigh);

}

Double_t BoxMesh::CalcFluence(FairRadMapPoint *p){

  // if(verbose) std::cout << "BoxMesh::CalcFluence - 0" << std::endl;
  Double_t xx, yy, zz, dx = 0, dy = 0, dz =0;
  float eff;
  std::vector<Corner> CV;
  std::vector<Plane> PV;
  //The Z should be between zlow and zhigh!!
  Transform(p->GetXOut(), p->GetYOut(), p->GetZOut(),
            xx, yy, zz);
  std::pair<int, double> minp;
  bool OK, OK2;

  if(IsInside(p)){
    OK = false;
    if(IsInside(xx, yy, zz)) OK = true;
    switch (_orientation){
    case XY: case YX:
      dz = _Zlow;
      break;
    case XZ: case ZX:
      dy = _Ylow;
      break;
    case ZY: case YZ:
      dx = _Xlow;
      break;
    }
    
    TVector3 poststepv   = TVector3(p->GetXOut(), p->GetYOut(), p->GetZOut());
    TVector3 prestepv    = TVector3(p->GetX(), p->GetY(), p->GetZ());
    unsigned int prebin  = _MeshHisto->FindBin(prestepv.X(),
                                               prestepv.Y());
    unsigned int postbin = _MeshHisto->FindBin(poststepv.X(),
                                               poststepv.Y());
    Plane plane;
    int prebinX  = 0, prebinY  = 0, prebinZ  = 0;
    int postbinX = 0, postbinY = 0, postbinZ = 0;
    _MeshHisto->GetBinXYZ(postbin, postbinX, postbinY, postbinZ);
    _MeshHisto->GetBinXYZ(prebin, prebinX, prebinY, prebinZ);
    TVector3 tn(0, 0, 0);

    if(verbose) std::cout << "BoxMesh::CalcFluence - 1" << std::endl;

    // 1, only two planes: the narrowest bin lower and higher plane
    // 2, if the resulting two points in the range of the other two axis-limits
    // 3, start the stepping from one of those points

    // -------- 1 -------

    TVector3 isl, lbeg, lend;
    double min_x, min_y, min_z, max_x, max_y, max_z;

    if(!OK){//if the 'out' is not in the given range
      //the 'in' and the 'out' is outside of the given range, so it can be that it will cross it

      if(verbose) std::cout << "BoxMesh::CalcFluence - 2" << std::endl;
      OK2 = false;
      min_x = _Xlow;
      min_y = _Ylow;
      min_z = _Zlow;
    
      max_x = _Xhigh;
      max_y = _Yhigh;
      max_z = _Zhigh;
      if(verbose) std::cout << "min_z: " << min_z << ' ' << max_z << ' ' << dz << std::endl;

      //the 'front' and 'back' XY planes
      plane = Plane(TVector3(min_x+dx, min_y+dy, min_z+dz),
                    TVector3(max_x+dx, min_y+dy, min_z+dz),
                    TVector3(min_x+dx, max_y+dy, min_z+dz));//XY @ z=z_min
      isl = plane.LineIntersection(prestepv, poststepv);
      if(((min_x < isl.X()) && (isl.X() < max_x)) &&
         ((min_y < isl.Y()) && (isl.Y() < max_y))){
        OK2 = true;
      }
      if(verbose){
        std::cout << "Xcross1: " << min_x << ' ' << isl.X() << ' ' << max_x << std::endl;
        std::cout << "Ycross1: " << min_y << ' ' << isl.Y() << ' ' << max_y << std::endl;
        std::cout << "=> " << OK2 << std::endl;
      }
      lbeg = isl;

      plane = Plane(TVector3(min_x+dx, min_y+dy, max_z+dz),
                    TVector3(max_x+dx, min_y+dy, max_z+dz),
                    TVector3(min_x+dx, max_y+dy, max_z+dz));//XY @ z-z_max
      isl = plane.LineIntersection(prestepv, poststepv);
      if(((min_x < isl.X()) && (isl.X() < max_x)) &&
         ((min_y < isl.Y()) && (isl.Y() < max_y))){
        OK2 = OK2 && true;
      }
      if(verbose){
        std::cout << "Xcross2: " << min_x << ' ' << isl.X() << ' ' << max_x << std::endl;
        std::cout << "Ycross2: " << min_y << ' ' << isl.Y() << ' ' << max_y << std::endl;
        std::cout << "=> " << OK2 << std::endl;
      }
      lend = isl;

      if(OK2){//the intersections are on the XY planes withing boarders at both z_min and z_max
        if(verbose) std::cout << "BoxMesh::CalcFluence - 3" << std::endl;
        prestepv  = lbeg;
        poststepv = lend;
        prebin    = _MeshHisto->FindBin(prestepv.X(),
                                        prestepv.Y());
        postbin   = _MeshHisto->FindBin(poststepv.X(),
                                        poststepv.Y());
        _MeshHisto->GetBinXYZ(prebin, prebinX, prebinY, prebinZ);
        _MeshHisto->GetBinXYZ(postbin, postbinX, postbinY, postbinZ);
      }else{
        if(verbose) std::cout << "BoxMesh::CalcFluence - 4" << std::endl;
      }
    }else{
      // if(prebin != postbin){
      int counter = 0;
      while((prebin != postbin) && !tn.Z()){
        counter++;
        PV.clear();

        //       max
        //  /---/      y|    
        // /---/|       |   /z
        // ||  ||       |  /
        // |/--|/       | /
        // -----        |/
        // min          +----------
        //                        x     


        // .              corner1          corner2          corner3
        //plane1   [(min, min, min), (min, min, max), (max, min, min)] ZX
        //plane2   [(min, min, min), (max, min, min), (min, max, min)] XY
        //plane3   [(min, min, min), (min, min, max), (min, max, min)] ZY

        //plane4   [(min, max, min), (min, max, max), (max, max, min)] ZX
        //plane5   [(min, min, max), (max, min, max), (min, max, max)] XY
        //plane6   [(max, min, min), (max, min, max), (max, max, min)] ZY

        
        // //plane4   [(max, max, max), (max, max, min), (min, max, max)] XZ
        // //plane5   [(max, max, max), (min, max, max), (max, min, max)] 
        // //plane6   [(max, max, max), (max, max, min), (max, min, max)]
        

        min_x = (_MeshHisto->GetXaxis()->GetBinLowEdge(prebinX) < 0) ? 0 : _MeshHisto->GetXaxis()->GetBinLowEdge(prebinX);
        min_y = (_MeshHisto->GetYaxis()->GetBinLowEdge(prebinY) < 0) ? 0 : _MeshHisto->GetYaxis()->GetBinLowEdge(prebinY);
        min_z = (_MeshHisto->GetZaxis()->GetBinLowEdge(prebinZ) < 0) ? 0 : _MeshHisto->GetZaxis()->GetBinLowEdge(prebinZ);
        
        max_x = (_MeshHisto->GetXaxis()->GetBinUpEdge(prebinX) < 0) ? 0 : _MeshHisto->GetXaxis()->GetBinUpEdge(prebinX);
        max_y = (_MeshHisto->GetYaxis()->GetBinUpEdge(prebinY) < 0) ? 0 : _MeshHisto->GetYaxis()->GetBinUpEdge(prebinY);
        max_z = (_MeshHisto->GetZaxis()->GetBinUpEdge(prebinZ) < 0) ? 0 : _MeshHisto->GetZaxis()->GetBinUpEdge(prebinZ);
        max_z += _MeshHisto->GetZaxis()->GetBinWidth(1);
        
        if(verbose){
          std::cout << counter << " - Corners [[" << min_x << ", " << min_y << ", " << min_z << "] - ";
          std::cout <<                        "[" << max_x << ", " << max_y << ", " << max_z << "]]\n";
        }

        //plane1
        plane = Plane(TVector3(min_x+dx, min_y+dy, min_z+dz),
                      TVector3(min_x+dx, min_y+dy, max_z+dz),
                      TVector3(max_x+dx, min_y+dy, min_z+dz));
        PV.push_back(plane);
        //plane2
        plane = Plane(TVector3(min_x+dx, min_y+dy, min_z+dz),
                      TVector3(max_x+dx, min_y+dy, min_z+dz),
                      TVector3(min_x+dx, max_y+dy, min_z+dz));
        PV.push_back(plane);
        //plane3
        plane = Plane(TVector3(min_x+dx, min_y+dy, min_z+dz),
                      TVector3(min_x+dx, min_y+dy, max_z+dz),
                      TVector3(min_x+dx, max_y+dy, min_z+dz));
        PV.push_back(plane);
        //plane4
        plane = Plane(TVector3(min_x+dx, max_y+dy, min_z+dz),
                      TVector3(min_x+dx, max_y+dy, max_z+dz),
                      TVector3(max_x+dx, max_y+dy, min_z+dz));
        PV.push_back(plane);
        //plane5
        plane = Plane(TVector3(min_x+dx, min_y+dy, max_z+dz),
                      TVector3(max_x+dx, min_y+dy, max_z+dz),
                      TVector3(min_x+dx, max_y+dy, max_z+dz));
        PV.push_back(plane);
        //plane6
        plane = Plane(TVector3(max_x+dx, min_y+dy, min_z+dz),
                      TVector3(max_x+dx, min_y+dy, max_z+dz),
                      TVector3(max_x+dx, max_y+dy, min_z+dz));
        PV.push_back(plane);

        if(verbose)
          std::cout << "Start\n["
                    << prebin << " {" << prestepv.X() << ' ' << prestepv.Y() << ' ' << prestepv.Z() << "}("
                    << prebinX << ' ' << prebinY << ' ' << prebinZ << ")\n"
                    << ' ' << postbin << " {" << poststepv.X() << ' ' << poststepv.Y() << ' ' << poststepv.Z() << "}(" 
                    << postbinX << ' ' << postbinY << ' ' << postbinZ << ")]\n\n";

        minp = std::make_pair(-1, 111111);//(which plane, what is the disrance)

        //calculate the intersection with every each planes
        isl = PV.at(0).LineIntersection(prestepv, poststepv);
        if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
          if(minp.second > (isl-prestepv).Mag())
            minp = std::make_pair(0, (isl-prestepv).Mag());
        if(verbose)
          std::cout << "XZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
                    << (poststepv-prestepv).Mag() << ", "
                    << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
                    << (isl-prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
                    << (minp.second > (isl-prestepv).Mag()) << ' '
                    << "[" << minp.first << ", " << minp.second << std::endl;

        if(!OK2){
          // for(int index = 1; index < 4; index++){
          //   isl = PV.at(index).LineIntersection(prestepv, poststepv);
          //   if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
          //     if(minp.second > (isl-prestepv).Mag())
          //       minp = std::make_pair(1, (isl-prestepv).Mag());
          //   if(verbose)
          //     std::cout << index <<" (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
          //               << (poststepv-prestepv).Mag() << ", "
          //               << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
          //               << (isl-prestepv).Mag() << ' '
          //               << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
          //               << (minp.second > (isl-prestepv).Mag()) << ' '
          //               << "[" << minp.first << ", " << minp.second << "]" << std::endl;
          // }

          isl = PV.at(1).LineIntersection(prestepv, poststepv);
          if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
            if(minp.second > (isl-prestepv).Mag())
              minp = std::make_pair(1, (isl-prestepv).Mag());
          if(verbose)
            std::cout << "XY (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
                      << (poststepv-prestepv).Mag() << ", "
                      << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
                      << (isl-prestepv).Mag() << ' '
                      << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
                      << (minp.second > (isl-prestepv).Mag()) << ' '
                      << "[" << minp.first << ", " << minp.second << "]" << std::endl;
        }
        
        isl = PV.at(2).LineIntersection(prestepv, poststepv);
        if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
          if(minp.second > (isl-prestepv).Mag())
            minp = std::make_pair(2, (isl-prestepv).Mag());
        if(verbose)
          std::cout << "YZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
                    << (poststepv-prestepv).Mag() << ", "
                    << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
                    << (isl-prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
                    << (minp.second > (isl-prestepv).Mag()) << ' '
                    << "[" << minp.first << ", " << minp.second << "]" << std::endl;

        isl = PV.at(3).LineIntersection(prestepv, poststepv);
        if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
          if(minp.second > (isl-prestepv).Mag())
            minp = std::make_pair(3, (isl-prestepv).Mag());
        if(verbose)
          std::cout << "XZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
                    << (poststepv-prestepv).Mag() << ", "
                    << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
                    << (isl-prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
                    << (minp.second > (isl-prestepv).Mag()) << ' '
                    << "[" << minp.first << ", " << minp.second  << "]" << std::endl;

        if(!OK2){
          isl = PV.at(4).LineIntersection(prestepv, poststepv);
          if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
            if(minp.second > (isl-prestepv).Mag())
              minp = std::make_pair(4, (isl-prestepv).Mag());
          if(verbose)
            std::cout << "XY (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
                      << (poststepv-prestepv).Mag() << ", "
                      << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
                      << (isl-prestepv).Mag() << ' '
                      << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
                      << (minp.second > (isl-prestepv).Mag()) << ' '
                      << "[" << minp.first << ", " << minp.second  << "]" << std::endl;
        }
        
        isl = PV.at(5).LineIntersection(prestepv, poststepv);
        if(momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)))
          if(minp.second > (isl-prestepv).Mag())
            minp = std::make_pair(5, (isl-prestepv).Mag());
        if(verbose)
          std::cout << "YZ (" << (isl).X() << ", " << (isl).Y() << ", " << (isl).Z() << ") "
                    << (poststepv-prestepv).Mag() << ", "
                    << "(" << (isl-prestepv).X() << ", " << (isl-prestepv).Y() << ", " << (isl-prestepv).Z() << ") "
                    << (isl-prestepv).Mag() << ' '
                    << momentumfilter(TVector3(p->GetPx(), p->GetPy(), p->GetPz()), (isl-prestepv)) << ' '
                    << (minp.second > (isl-prestepv).Mag()) << ' '
                    << "[" << minp.first << ", " << minp.second  << "]" << std::endl;

        if(verbose)
          std::cout << "\nmomentum: "
                    << p->GetPx() << ' '
                    << p->GetPy() << ' '
                    << p->GetPz() << std::endl;

        //minp stores the smallest possible step
        if(0 <= minp.first){
          if(verbose){
            std::cout << "\nmomentum: "
                      << p->GetPx() << ' '
                      << p->GetPy() << ' '
                      << p->GetPz() << std::endl;
            std::cout << "[(----)] " << minp.first << ' ' << minp.second << std::endl;
            std::cout << "normal: "
                      << PV.at(minp.first).Normal().X() << ' '
                      << PV.at(minp.first).Normal().Y() << ' '
                      << PV.at(minp.first).Normal().Z() << std::endl;
          }

          isl = PV.at(minp.first).LineIntersection(prestepv, poststepv);
          Double_t step = (isl-prestepv).Mag();
          prestepv = isl;
          
          _MeshHisto->Fill(prestepv.X(), prestepv.Y(), step);
          _StatHisto->Fill(prestepv.X(), prestepv.Y());

          tn = PV.at(minp.first).Normal();
          float ds = 0;
          if(tn.X()){
            ds = _MeshHisto->GetXaxis()->GetBinWidth(1)/10.;
            if(verbose) std::cout << "Xprebin: " << prebin << " ---> ";
            if(p->GetPx() > 0){
              prebin = _MeshHisto->FindBin(isl.X() + ds,
                                           isl.Y());
              if(verbose) std::cout << "(" << isl.X() << " ---> " << isl.X() + ds << ' '  << p->GetPx() << " ) ";
            }else{
              prebin = _MeshHisto->FindBin(isl.X() - ds,
                                           isl.Y());
              if(verbose) std::cout << "(" << isl.X() << " ---> " << isl.X() - ds << ' '  << p->GetPx() << " ) ";
            }
            if(verbose) std::cout << prebin << std::endl;        
          }
          if(tn.Y()){
            ds = _MeshHisto->GetYaxis()->GetBinWidth(1)/10.;
            if(verbose) std::cout << "Yprebin: " << prebin << " ---> ";
            if(p->GetPy() > 0){
              prebin = _MeshHisto->FindBin(isl.X(),
                                           isl.Y() + ds);
              if(verbose) std::cout << "1(" << isl.Y() << " ---> " << isl.Y() + ds << " ) ";
            }else{
              prebin = _MeshHisto->FindBin(isl.X(),
                                           isl.Y()-ds);
              if(verbose) std::cout << "1(" << isl.Y() << " ---> " << isl.Y() - ds << " ) ";
            }
            if(verbose) std::cout << prebin << std::endl;        
          }

          _MeshHisto->GetBinXYZ(prebin, prebinX, prebinY, prebinZ);
          if(verbose) std::cout << "Stop\n["
                                << prebin << " {" << prestepv.X() << ' ' << prestepv.Y() << ' ' << prestepv.Z() << "}("
                                << prebinX << ' ' << prebinY << ' ' << prebinZ << ")\n"
                                << ' ' << postbin << " {" << poststepv.X() << ' ' << poststepv.Y() << ' ' << poststepv.Z() << "}(" 
                                << postbinX << ' ' << postbinY << ' ' << postbinZ << ")]\n\n";
          
          if(IsInside(p->GetX(), p->GetY(), p->GetZ())){
            if(verbose) std::cout << " Bingo\n";
          }
        }
      }
    }
  }
  return minp.second;
}


void InvMatVecProd(TMatrixD mat, TVector3 vec, TVector3& res){
  TMatrixD imat = mat.Invert();
  double val;
  double resl[3];
  for(int i = 0; i < 3; i++){
    val = 0;
    for(int j = 0; j < 3; j++){
      val+=imat[i][j]*vec[j];
    }
    resl[i] = val;
  }
  res = TVector3(resl[0], resl[1], resl[2]);
}
