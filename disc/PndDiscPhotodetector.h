//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Definition of photo sensors
//-------------------------------------------------------------------------

#ifndef DISCDIRC_PHOTODETECTOR_HH
#define DISCDIRC_PHOTODETECTOR_HH

#include "PndDiscSensorGridPhotodetector.h"

#include <Math/Interpolator.h>


class DiscDIRC_Photodetector : public SensorGrid::SensorGridPhotodetector
{
public:
    enum DesignID {
        DESIGN_SIPM = 1,
        DESIGN_LRD
    };

    DiscDIRC_Photodetector();
    DiscDIRC_Photodetector(DesignID id);

    void SetPDE(int n_entries, const double * wavelength_nm, const double * pde)
    {
        //pde_interpolator.SetData(n_entries, wavelength_nm, pde);
    }

    virtual double GetPDE(const double &wavelength_nm) const 
    {
        //return pde_interpolator.Eval(wavelength_nm);
        return wavelength_nm;
    }

protected:
    void Init();
    void Init(DesignID design_id);

private:
    //ROOT::Math::Interpolator pde_interpolator;
};


#endif // DISCDIRC_PHOTODETECTOR_HH
