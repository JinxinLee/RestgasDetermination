#include "PndTpcDEDXStorageHelper.h"

void PndTpcDEDXStorageHelper::SetDEDX(double de, double dx)
{
	fde=de;
	fdx=dx;
}

void PndTpcDEDXStorageHelper::GetDEDX(double &de, double &dx) const
{
	de=fde;
	dx=fdx;
}
