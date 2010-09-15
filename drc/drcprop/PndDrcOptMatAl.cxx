// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatAl.h"
#include <algorithm>
//#include "PndDrcOptMatAbs.h"
//
//#include "TObject.h"
//#include "TVector3.h"
//#include "TRandom.h"
//
#include <iostream>
using std::cout;
using std::cerr;
//using std::cin;
using std::endl;
//
#include <cmath>
//#include <valarray>
//using std::valarray;
//
//#include <fstream>
//using std::fstream;
//
//#include <string>
//using std::string;
//
//#include <list>
//using std::list;

//----------------------------------------------------------------------
PndDrcOptMatAl::PndDrcOptMatAl()
{
	fName = "aluminium";
}
//----------------------------------------------------------------------
PndDrcOptMatAl* PndDrcOptMatAl::Clone() const
{
    return new PndDrcOptMatAl(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatAl::Copy(const PndDrcOptMatAl& mat)
{
}//----------------------------------------------------------------------
PndDrcOptMatAl::PndDrcOptMatAl(const PndDrcOptMatAl& mat)
    : PndDrcOptMatAbs(mat)
{
    if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatAl::PndDrcOptMatAl"
                <<"(const PndDrcOptMatAl&) "
                <<mat.fName<<endl;
    Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatAl& PndDrcOptMatAl::operator=(const PndDrcOptMatAl& mat)
{
    if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatAl::operator="
                <<"(const PndDrcOptMatAl&) "
                <<mat.fName<<endl;
    if (&mat != this)
    {
        static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
        Copy(mat);
    }
    return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatAl::RefIndex(const double lambda) const
{
    // values from  http://ee.byu.edu/photonics/opticalconstants.phtml (metallic aluminium)
    const static double kLam[57] = { // lambda in nm
        300.0,
        310.0,
        317.9,
        326.3,
        335.1,
        344.4,
        350.0,
        354.2,
        364.7,
        375.7,
        387.5,
        400.0,
        413.3,
        427.5,
        442.8,
        450.0,
        459.2,
        476.9,
        495.9,
        500.0,
        506.1,
        516.6,
        527.6,
        539.1,
        550.0,
        551.0,
        563.6,
        576.7,
        590.4,
        600.0,
        604.8,
        619.9,
        635.8,
        650.0,
        652.6,
        670.2,
        688.8,
        700.0,
        708.5,
        729.3,
        750.0,
        751.4,
        774.9,
        799.9,
        825.0,
        826.6,
        850.0,
        855.1,
        875.0,
        885.6,
        900.0,
        918.4,
        925.0,
        950.0,
        953.7,
        991.9,
        1000.0
    };

    const static double kI[57] = { // refractive Index
        0.2760,
        0.2940,
        0.3100,
        0.3260,
        0.3440,
        0.3640,
        0.3750,
        0.3850,
        0.4070,
        0.4320,
        0.4600,
        0.4900,
        0.5230,
        0.5580,
        0.5980,
        0.6180,
        0.6440,
        0.6950,
        0.7550,
        0.7690,
        0.7890,
        0.8260,
        0.8670,
        0.9120,
        0.9580,
        0.9630,
        1.0200,
        1.0800,
        1.1500,
        1.2000,
        1.2200,
        1.3000,
        1.3900,
        1.4700,
        1.4900,
        1.6000,
        1.7400,
        1.8300,
        1.9100,
        2.1400,
        2.4000,
        2.4100,
        2.6300,
        2.8000,
        2.7500,
        2.7400,
        2.6100,
        2.5800,
        2.3800,
        2.2400,
        2.0600,
        1.8600,
        1.7700,
        1.4900,
        1.5800,
        1.3700,
        1.3500
    };


    if (lambda>1000)
    {
        return 1.35; // set eveything above 1000nm to the value of 1000nm
    }
    else if (lambda<300)
    {
        return 0.276; // same here
    }
    else
    {
      // find right bin
        int ibin=-1;
        for (int i=1; i<57; i++)
        {
            if (lambda>=kLam[i-1] && lambda<kLam[i])
            {
                ibin = i;
            }
        }
        if (ibin==-1)
        {
            cout << lambda << endl;
            cerr<<" *** PndDrcOptMatAl::RefIndex: "
                    <<"this line should never been hit"<<endl;
            exit(EXIT_FAILURE);
        }
        double frac = (kLam[ibin-1]-lambda)/(kLam[ibin-1]-kLam[ibin]);
        return( kI[ibin-1] + frac * (kI[ibin]-kI[ibin-1]) );
    }
}
//----------------------------------------------------------------------
double PndDrcOptMatAl::RefIndexDeriv(const double lambda) const
{
    return -666;
}
//----------------------------------------------------------------------
double PndDrcOptMatAl::Extinction(const double lambda) const
{
    // values from  http://ee.byu.edu/photonics/opticalconstants.phtml (metallic aluminium)
    // unfortunately above site is not available anymore
    // to verify these values use: http://refractiveindex.info
    const static double kLam[57] = { // lambda in nm
        300.0,
        310.0,
        317.9,
        326.3,
        335.1,
        344.4,
        350.0,
        354.2,
        364.7,
        375.7,
        387.5,
        400.0,
        413.3,
        427.5,
        442.8,
        450.0,
        459.2,
        476.9,
        495.9,
        500.0,
        506.1,
        516.6,
        527.6,
        539.1,
        550.0,
        551.0,
        563.6,
        576.7,
        590.4,
        600.0,
        604.8,
        619.9,
        635.8,
        650.0,
        652.6,
        670.2,
        688.8,
        700.0,
        708.5,
        729.3,
        750.0,
        751.4,
        774.9,
        799.9,
        825.0,
        826.6,
        850.0,
        855.1,
        875.0,
        885.6,
        900.0,
        918.4,
        925.0,
        950.0,
        953.7,
        991.9,
        1000.0
    };

    // extinction coefficients (normally this number is positive )
    // ROOT bug with complex number: TComplex::ASin is wrong for positive imaginary parts
    //  => extinction coefficients have to be negative
    const static double kE[57] = {
        -3.6100,
        -3.7400,
        -3.8400,
        -3.9500,
        -4.0600,
        -4.1700,
        -4.2400,
        -4.3000,
        -4.4300,
        -4.5600,
        -4.7100,
        -4.8600,
        -5.0200,
        -5.2000,
        -5.3800,
        -5.4700,
        -5.5800,
        -5.8000,
        -6.0300,
        -6.0800,
        -6.1500,
        -6.2800,
        -6.4200,
        -6.5500,
        -6.6900,
        -6.7000,
        -6.8500,
        -7.0000,
        -7.1500,
        -7.2600,
        -7.3100,
        -7.4800,
        -7.6500,
        -7.7900,
        -7.8200,
        -8.0100,
        -8.2100,
        -8.3100,
        -8.3900,
        -8.5700,
        -8.6200,
        -8.6200,
        -8.6000,
        -8.4500,
        -8.3100,
        -8.3100,
        -8.2200,
        -8.2100,
        -8.1800,
        -8.2100,
        -8.3000,
        -8.4400,
        -8.4900,
        -8.8800,
        -8.9500,
        -9.4900,
        -9.5800
    };


    if (lambda>1000)
    {
        return 9.58; // set eveything above 1000nm to the value of 1000nm
    }
    else if (lambda<300)
    {
        return 3.61; // same here
    }
    else
    {
      // find right bin
        int ibin=-1;
        for (int i=1; i<57; i++)
        {
            if (lambda>=kLam[i-1] && lambda<kLam[i])
            {
                ibin = i;
            }
        }
        if (ibin==-1)
        {
            cerr<<" *** PndDrcOptMatAl::Extinction: "
                    <<"this line should never been hit"<<endl;
            exit(EXIT_FAILURE);
        }
        double frac = (kLam[ibin-1]-lambda)/(kLam[ibin-1]-kLam[ibin]);
        return( kE[ibin-1] + frac * (kE[ibin]-kE[ibin-1]) );
    }
}
//----------------------------------------------------------------------
bool PndDrcOptMatAl::AbsorptionFlag(double lambda, double length) const
{
    return true; // always absorbed
}
