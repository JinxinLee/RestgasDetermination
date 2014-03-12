#ifndef _f_Init_included_
#define _f_Init_included_

extern "C"
{

	extern struct
	{
		double alpha;
		double xphcut;
	} phocop_;

	extern struct
	{
		double fsec;
		double fint;
		double expeps;
		int interf;
		int isec;
		int itre;
		int iexp;
		int iftop;
		int ifw;
	} phokey_;

	extern struct
	{
		int status[10];
	        int ifstop;
	} phosta_;

	extern struct
	{
		double pi;
		double twopi;
	} phpico_;

	extern struct
	{
		int phlun;
	} pholun_;

	//debug mode on if ipoin <  1 and ipoinm > 1
	extern struct
	{
		int ipoin;
		int ipoinm;
	} phlupy_;

	/** Initialize kinematic corrections */
	void phcork_(int * modcor);

	/** Single branch processing */
	void photos_make_c_(int * id);

	/* PHOINI subroutines */
	int  iphqrk_(int *i);
	int  iphekl_(int *i);

	/* Printout of error messages */
	void phoerr_(int *imes,char *text,double *data);

}

#endif
