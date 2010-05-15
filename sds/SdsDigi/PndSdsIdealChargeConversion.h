#ifndef PNDSDSIDEALCHARGECONVERSION_H
#define PNDSDSIDEALCHARGECONVERSION_H

//! ideal charge conversion
//! @author D.-L.Pohl <d.pohl@fz-juelich.de>

#include "PndSdsChargeConversion.h"

class PndSdsIdealChargeConversion : public PndSdsChargeConversion{
public :

	PndSdsIdealChargeConversion() : PndSdsChargeConversion(kIdeal){};
	~PndSdsIdealChargeConversion(){};
	Double_t ChargeToDigiValue(Double_t charge){return charge;};
	Double_t DigiValueToCharge(Double_t digi){return digi;};

ClassDef(PndSdsIdealChargeConversion, 1);
};
#endif /* PNDSDSIDEALCHARGECONVERSION_H */
