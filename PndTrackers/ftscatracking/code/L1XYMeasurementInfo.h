#ifndef L1XYMeasurementInfo_h
#define L1XYMeasurementInfo_h 1

class L1XYMeasurementInfo{
  
  //* information about measurement
  
 public:
  L1XYMeasurementInfo():C00(Vc::Zero), C10(Vc::Zero), C11(Vc::Zero){}

  L1XYMeasurementInfo( const float_v& c00, const float_v& c10, const float_v& c11 ):C00(c00), C10(c10), C11(c11){}
  
  float_v C00, C10, C11;
};


#endif
