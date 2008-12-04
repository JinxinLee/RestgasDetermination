/*
  Modified defs.h from Hugo Pereira in the compass coral alignment
  code
*/
#ifndef Defs_h
#define Defs_h
 
//#define BUFFER_SIZE 32000  /*!< root buffer max size before flushing to file */ // unshure if i need this
#define NPLAN 8         /*!< Max number of planes 4 detectors, 2
			 * planes each */
//#define DURANGE 20        /*!< max residual (in number of pitches). may bias the tree*/

/*! number of parameters/track
  0: x           ( perp to the beam horizontal) 
  1: tan(teta_x) ( in Ox,Oz plane)
  2: y           ( perp to the beam vertical
  3: tan(teta_y) (in Ox,Oz plane)
*/ 
#define NPARTRCK 4	      

/*! number of alignement parameters per plane:
  0: U offset perp to the wire: U+=\alpha_U
  1: Z offset along the beam: Z+=\alpha_Z
  2: T rotational offset, perp to the beam: T+=\alpha_T
  3: P pitch: U*=(1+\alpha_P)
  4: R R0 offset for the distance to the wire, proportional to T0 for
  drift like detectors: U+=sign(R)*\alpha_R         \\currently not in use
  5: L lorentz angle RT scaling for drift like detectors:
  R*=(1+\alpha_L) i.e. U+=\alpha_L*R                \\currently not in
  use
*/
#define NPARPLAN 4	

#define NGLB NPLAN*NPARPLAN /*!< maximum number of global parameters */ 
//#define PI 3.14159          /*!< pi */

#endif




