/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

#ifndef mzfunctions_pp_to_pipi_vandewi_h
#define mzfunctions_pp_to_pipi_vandewi_h 1

void mz_pp_to_pipi_vandewi_init(int seed, double P, double cos_theta_min,double cos_theta_max);

void mz_pp_to_pipi_get_p_lattice_site(double P, int& i_min, int& iflag);

void mz_pp_to_pipi_get_costheta_lattice_site(double cos_theta, int& k_min, int& kflag);

double mz_pp_to_pipi_vandewi_maximum_sigma(double P);

double mz_pp_to_pipi_vandewi_sigma_legendre(int i, double cos_theta);

double mz_pp_to_pipi_sigma(double P, double cos_theta);

void mz_pp_to_pipi_vandewi_event(double P, double cos_theta_min, double cos_theta_max, double* piplus_p, double* piminus_p);

#endif
