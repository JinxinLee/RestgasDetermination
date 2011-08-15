
#ifndef DetEffi_h
#define DetEffi_h


//======
// ROOT
//======
#include <TRandom3.h>


class DetEffi
{
  public:

    DetEffi();
    virtual ~DetEffi(){}


    double _mcpEffi[501];
    double _mcpEffiOld[70];

    bool Effi( int wavelength = 0 );
    bool EffiOLD( int wavelength = 0 );

    bool EffiReal( int wavelength = 0 );
    bool EffiRealOLD( int wavelength = 0 );


    static TRandom3 rand;


    ClassDef(DetEffi, 1) // ROOT linking stuff
};


#endif

