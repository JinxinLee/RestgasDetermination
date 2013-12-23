#ifndef mzfunctions_pp_to_leplep_vandewi_h
#define mzfunctions_pp_to_leplep_vandewi_h 1

void mz_pp_to_leplep_vandewi_init(int seed, int particle_flag, double P, double GE_to_GM, double cos_theta_min, double cos_theta_max);

double mz_pp_to_leplep_vandewi_sigma(int particle_flag, double P, double GE, double GM, double cos_theta);

double mz_pp_to_leplep_vandewi_sigma_nonorm(int particle_flag, double P, double GE_to_GM, double cos_theta);

void mz_pp_to_leplep_vandewi_event(int particle_flag, double P, double GE_to_GM, double cos_theta_min, double cos_theta_max, double* lepplus_p, double* lepminus_p);



#endif
