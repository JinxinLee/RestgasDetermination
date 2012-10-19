/*
 * PndLmdDim.h
 *
 * this class gives you methods to retrieve parameters of the geometry
 * dimensions are in cm and radian
 *
 * Global comments:
 *
 * to do
 *
 *
 *  Created on: Oct 5, 2012
 *      Author: promme
 */

#ifndef PNDLMDDIM_H_
#define PNDLMDDIM_H_

#include <cmath>
#include <TRandom.h>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TVector3.h>
#include <TMatrixT.h>

typedef TMatrixT<double> TMatrixD; // hmm funny, should be declared in TMatrixT.h

using namespace std;

class PndLmdDim {
private:
	static PndLmdDim* pinstance;
	TGeoManager* fgGeoMan;
	PndLmdDim();
	PndLmdDim(const PndLmdDim& instance);
	PndLmdDim& operator=(const PndLmdDim& instance){return *this;}
	~PndLmdDim();
	// the navigation paths for the detector geometry are stored here
	vector<string> nav_paths;
public:
	static PndLmdDim& Get_instance();
	static PndLmdDim* Instance();

	// pi
	double pi;
	// number of detector planes
	unsigned int n_planes;
	// number of modules per plane half
	unsigned int nmodules;
	// position of planes where the first plane defines the origin
	double* plane_pos_z;
	// ****************************** plane half array *****************************
	// x in lmd reference frame
	double half_offset_x;
	// y in lmd reference frame
	double half_offset_y;
	// z in lmd reference frame
	double half_offset_z;
	// phi rotation in lmd reference frame
	double half_tilt_phi;
	// theta rotation in lmd reference frame
	double half_tilt_theta;
	// psi rotation in lmd reference frame
	double half_tilt_psi;
	// ****************************** plane half supports ******************************
	// x in the detector half reference frame
	double plane_half_offset_x;
	// y in the detector half reference frame
	double plane_half_offset_y;
	// z in the detector half reference frame
	double plane_half_offset_z;
	// phi rotation in the detector half reference frame
	double plane_half_tilt_phi;
	// theta rotation in the detector half reference frame
	double plane_half_tilt_theta;
	// psi rotation in the detector half reference frame
	double plane_half_tilt_psi;

	// ****************************** cvd cooling support discs ************************
	// cvd_diamond is cut out of 79.5 mm discs of 200 micron thickness
	// inner min. radius due to beam pipe + a safety margin
	double inner_rad;
	// not used yet but should be the outer acceptance
	double outer_rad;
	// number of CVD diamond discs per plane
	int n_cvd_discs;
	// radius of a CVD diamond disc
	double cvd_disc_rad;
	// the half of the diamond thickness
	double cvd_disc_thick_half;
	// even and odd discs in a plane will be shifted in z in order to prevent
	// mechanical damage during assembly
	double cvd_disc_even_odd_offset;
	// angle from the division of a circle into n_cvd_discs
	double delta_phi;
	// a polygon of n_cvd_discs sides fitting a radius of inner_rad
	// has a side length pol_side_lg of
	double pol_side_lg_half;
	// the minimum distance to the center of the polygone is given by
	double pol_side_dist_min;
	// the cvd disc has to be placed such that the disc crosses
	// the inner ring at an angle of 0 and delta_phi
	// this defines the distance to the center according to pythagoras
	double cvd_disc_dist;

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation

	// x is radial to the beam pipe when support is aligned
	double cvd_offset_x;
	// y is tangent to the beam pipe when support is aligned
	double cvd_offset_y;
	// z is along the beam pipe when support is aligned
	double cvd_offset_z;
	// phi rotation in the reference frame of the plane half support
	double cvd_tilt_phi;
	// theta rotation in the reference frame of the plane half support
	double cvd_tilt_theta;
	// psi rotation in the reference frame of the plane half support
	double cvd_tilt_psi;
	// thickness of the kapton flexible circuits to the sensors
	// (chosen to be thicker to simulate also the influence of the
	//  printed circuits themselves)
	double kapton_disc_thick_half;
	// *********************************** one side on a CVD disc *************************************
	// x is radial to the beam pipe when cvd is aligned
	double side_offset_x;
	// y is tangent to the beam pipe when cvd is aligned
	double side_offset_y;
	// z is along the beam pipe when cvd is aligned
	double side_offset_z; // should not be used due to clashing volumes with the support!
	// phi rotation in the reference frame of the cvd support
	double side_tilt_phi; // should not be used due to clashing volumes with the support!
	// theta rotation in the reference frame of the cvd support
	double side_tilt_theta;
	// psi rotation in the reference frame of the cvd support
	double side_tilt_psi; // should not be used due to clashing volumes with the support!
	// *********************************** HV-MAPS *************************************
	//
	//            left   right
	//
	//  |---------|----------|----------| top
	//  ||------|-||-------|-||-------|-|
	//  ||      | ||       | ||       | |
	//  ||  1   | ||   2   | ||   3   | |     row 1
	//  ||	    | || active| ||       | |
	//  ||------|-||-------|-||-------|-|
	//  |         | passive  |          |
	//  |---------|----------|----------| bottom
	//  gap
	//            |----------|----------| bottom
	//            | passive  |          |
	//            ||-------|-||-------|-|
	//            ||       | ||       | |
	//            ||   4   | ||   5   | |     row 2
	//            ||       | ||       | |
	//            ||-------|-||-------|-|
	//            |----------|----------| top
	//
	//            right   left
	//
	//                A   y; maps_n_row; height
	//                |
	//                |
	//                --> x; maps_n_col; width
	//
	// the current design foresees a rotation of the first parameters

	// even when several maps are placed on one die
	// those will be placed in the simulation next to
	// each other as separate detectors
	int maps_n_col;
	int maps_n_row;
	// enabled [row][col]
	bool** enabled;
	// number of sensors per side
	int n_sensors;
	// NOTE: MOST of the following VARIABLES are HALF of it
	// due to geometry construction in GEANT
	double maps_thickness;
	double maps_passive_top;
	double maps_passive_bottom;
	double maps_passive_left;
	double maps_passive_right;
	double maps_active_width;
	double maps_active_height;

	double maps_width;
	double maps_height;

	double maps_active_offset_x;
	double maps_active_offset_y;


	double die_gap; // (cm)

	double maps_die_width;
	double maps_die_height;

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation\
	// translations along z as well as rotations around x and y are
	// negligible for dies glued on a cvd diamond
	// rotation around z is not working yet

	// x is along the edge of the cvd disc
	double die_offset_x;
	// y is orthogonal to the edge of the cvd disc
	double die_offset_y;
	// z is along the beam pipe
	double die_offset_z; //should not be used -> crashing volumes;
	// x is a rotation around the edge of the cvd disc
	double die_tilt_phi; // please do not use yet
	// y is a rotation around the orthogonal component of the edge of the cvd disc
	double die_tilt_theta; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	double die_tilt_psi;// please do not use yet;
	//*********************************** lumi box parameters ***********************************
	// see CAD files for details
	// https://edms.cern.ch/nav/P:FAIR-000000719:V0/P:FAIR-000000726:V0/TAB3
	// width
	double box_size_x;
	// height
	double box_size_y;
	// length
	double box_size_z;
	// thickness of the V2A steel plates
	double box_thickness;
	// position of the inner rib
	double pos_rib;
	// beam pipe radius at entrance
	double rad_entrance;
	// beam pipe radius at exit
	double rad_exit;
	// beam pipe separating non interacting paricles
	double rad_pipe;
	// beam pipe thickness;
	double pipe_thickness;
	// cone height of the transition region
	double length_transision;
	// length of the inner pipe
	double length_pipe;
	// position of the first detector plane
	double pos_plane_0;
	//*********************************** global parameters *************************************
	// where bending starts with
	double end_seg_upstream;
	// the bending radius
	double r_bend;
	// and the angle of the circle path
	double phi_bend;
	// the point where both tangents of the straight beam pipe tubes meet is
	double pos_rot_z;
	// z position of the lmd
	double pos_z; //(cm)
	double end_seg_bend;
	// x position of the lmd
	double pos_x;
	// y position of the lmd
	double pos_y;
	double rot_x;
	double rot_y;
	double rot_z;

	// returns false when one of the variables exceeds design values
	bool Is_valid_idcall(int ihalf, int iplane = 0, int imodule = 0, int iside = 0, int idie = 0, int isensor = 0){
		if (ihalf   < 0 || ihalf   >= 2)
			return false;
		if (iplane  < 0 || iplane  >= n_planes)
			return false;
		if (imodule < 0 || imodule >= n_cvd_discs)
			return false;
		if (iside   < 0 || iside   >= 2)
			return false;
		if (idie    < 0 || idie    >= 2)
			return false;
		// allow to count the non existing inner sensor at die 2
		if (isensor < 0 || isensor >= n_sensors + 1)
			return false;
		return true;
	}

	// get the sensor id for a sensor on a given side, module and plane
	int Get_sensor_id(int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
		if (idie == 1) isensor += 2; // the parallel sensor to sensor 0 is not there!
		int result = isensor + (iside + (imodule + (iplane + ihalf * n_planes) * nmodules) * 2 ) * n_sensors;
		return result;
	}

	// get the sensor position by it's id in terms of plane module and side
	void Get_sensor_by_id(const int sensor_id, int& ihalf, int& iplane, int& imodule, int& iside, int& idie, int& isensor){
		int _sensor_id = sensor_id;
		isensor = _sensor_id % n_sensors;
		idie = 0;
		if (isensor > 2) {
			idie = 1;
			isensor -= 2; // add the first but non existing sensor at die 2
		}
		_sensor_id /= n_sensors;
		iside = _sensor_id % 2;
		_sensor_id /= 2;
		imodule = _sensor_id % nmodules;
		_sensor_id /= nmodules;
		iplane = _sensor_id % n_planes;
		_sensor_id /= n_planes;
		ihalf = _sensor_id % 2;
		if (!Is_valid_idcall(ihalf, iplane, imodule, iside, idie, isensor)){
			ihalf = 0;
			iplane = 0;
			imodule = 0;
			iside = 0;
			idie = 0;
			isensor = 0;
			cout << "Error in PndLmdDim::Get_sensor_by_id: "<< sensor_id <<" is not a valid sensor id!" << endl;
		}
	}

	// when calling for a displaced module, the generated
	// value must be kept same for the cvd diamond as well
	// as the sensors sitting on the diamond it self
	// the storage is realized by a map
	// the vector contains offsets in x, y, z, rotphi, rottheta, rotpsi;
	map<string, vector<double> > offsets;
	map<string, vector<double> >::iterator itoffset;
	string Generate_key(int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
		stringstream keystream;
		keystream << ihalf << iplane << imodule << iside << idie << isensor;
		return keystream.str();
	}

	// same structure as for offsets is used for the transformation matrices
	// stored are matrix operations
	// global -> local lumi
	//   key: ihalf = -1 iplane = -1 imodule = -1 iside = -1 idie = -1 isensor = -1
	// local lumi -> local side on cvd disc
	//   key: ihalf >=0 iplane >= 0 imodule >=0 iside = -1 idie = -1 isensor = -1
	// local side on cvd disc -> local sensor
	//   key: all variable
	map<string, TGeoMatrix* > transformation_matrices;
	map<string, TGeoMatrix* > transformation_matrices_aligned; // alternative aligned detector description
	map<string, TGeoMatrix* >::iterator it_transformation_matrices;

	// cleanup some maps containing only references
	void Cleanup();

	// read transformation matrices from a given file
	// aligned and not aligned are two separate maps
	// containing the description of the detector positions
	void Read_transformation_matrices(string filename, bool aligned = true);

	// write transformation matrices from a given file
	// aligned and not aligned are two separate maps
	// containing the description of the detector positions
	void Write_transformation_matrices(string filename, bool aligned = true);

	void Get_offset(int ihalf, int iplane, int imodule, int iside, int idie, int isensor,
			double& x, double& y, double& z,
			double& rotphi, double& rottheta, double& rotpsi);

	// several functions returning the position and orientation of
	// the luminosity detector

	// the global system is defined by PANDA
	// returns the position and rotation of the lmd detector
	// the origin is PANDA interaction point
	// first comes translation than rotation
	void Get_pos_lmd_global(double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz,
			bool misaligned = false){
		rotx = rot_x;
		roty = rot_y;// phi_bend;
		rotz = rot_z;
		x = pos_x;
		y = pos_y;
		z = pos_z;
	}
/*
	// the local system is where the first plane is at xyz = 0 and
	// the detector is oriented along z
	// module counting starts from the first plane (positive x and positive y)
	// rotation is right handed

	// rotation of the i'th module around z
	double Get_rot_z_local(int imodule){
		if (Is_valid_idcall(0, imodule))
			return delta_phi * imodule + delta_phi/2.;
		else
			return 0;
	}

	// returns the position and rotation of the i'th module at the i'th plane
	// the origin is the center of the cvd disc
	// first comes translation than rotation
	void Get_pos_mod_local(int iplane, int imodule,
			double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz,
			bool misaligned = false){
		//x = 0; y = 0; z = 0; rotx = 0; roty = 0; rotz = 0;
		if (Is_valid_idcall(iplane, imodule)){
			double angle = Get_rot_z_local(imodule);
			double add_z = cvd_disc_even_odd_offset;
			if ((imodule%2)==0) add_z = -add_z;
			x += cvd_disc_dist*cos(angle);
			y += cvd_disc_dist*sin(angle);
			z += plane_pos_z[iplane] + add_z;
			rotz += angle;
			if (misaligned){
				double offset_x, offset_y, offset_z, tilt_x, tilt_y, tilt_z;
				Get_offset(iplane, imodule, -1, -1,
						offset_x, offset_y, offset_z,
						tilt_x, tilt_y, tilt_z);
				// x and y rotation must take the orientation
				// of the rotational axis into account
				rotx += tilt_x;
				roty += tilt_y;
				rotz += tilt_z;
				double offset_r = offset_x;
				double offset_tan = offset_y;
				x += offset_r*cos(angle);
				y += offset_r*sin(angle);
				x -= offset_tan*sin(angle);
				y += offset_tan*cos(angle);
				z += offset_z;

			}
		}
	}

	// returns the position and rotation of the i'th sensor at the i'th plane
	// and i'th module
	// the origin is the center of the sensor
	// first comes translation than rotation
	void Get_pos_sens_local(int iplane, int imodule, int iside, int isensor,
			double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz,
			bool misaligned = false){
		x = 0; y = 0; z = 0; rotx = 0; roty = 0; rotz = 0;
		if (Is_valid_idcall(iplane, imodule, iside, isensor)){
			// retrieve the (aligned) center of the corresponding CVD disc
			Get_pos_mod_local(iplane, imodule, x, y, z, rotx, roty, rotz, misaligned);
			// distances in the coordinate system of the cvd disc
			// the inner edge
			double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
			// apply the sensor misalignment before cvd rotation
			double offset_x(0), offset_y(0), offset_z(0), tilt_x(0), tilt_y(0), tilt_z(0);
			if (misaligned){
				Get_offset(iplane, imodule, iside, -1,
							offset_x, offset_y, offset_z,
							tilt_x, tilt_y, tilt_z);
			}
			const double _sinhalf = sin (delta_phi/2.);
			const double _coshalf = cos (delta_phi/2.);
			const double _edge_y = -_sinhalf*inner_rad;
			// angle between the edge and the half of the cvd disc
			const double _edgeangle = asin(-_edge_y/cvd_disc_rad);
			const double _edge_x = - cvd_disc_rad * cos(_edgeangle);
			_x = _edge_x + offset_x*_coshalf + offset_y*_sinhalf;
			_y = _edge_y - offset_x*_sinhalf + offset_y*_coshalf;
			if (isensor < 3) {
				_x += (0.5+isensor)*maps_width*2.*_coshalf + maps_height*_sinhalf;
				_y -= (0.5+isensor)*maps_width*2.*_sinhalf - maps_height*_coshalf;
			}
			if (isensor >= 3 && isensor < n_sensors){
				_x += (0.5+isensor-2)*maps_width*2.*_coshalf + (maps_height*3. + die_gap)*_sinhalf;
				_y -= (0.5+isensor-2)*maps_width*2.*_sinhalf - (maps_height*3. + die_gap)*_coshalf;
			}
			_z = -cvd_disc_thick_half - maps_thickness + offset_z;

			_rotz = - delta_phi/2.;
			// the other side is not only displaced
			// but also rotated to keep the surface
			// on the correct side
			if (iside == 1) {
				_y = -_y;
				_z = -_z;
				_rotz = -_rotz;
				_rotx = pi;
			}
			// translate and rotate now into the cvd reference frame
			// taking an additional tilt into account
			x +=  _x*cos(rotz)-_y*sin(rotz);
			y +=  _x*sin(rotz)+_y*cos(rotz);
			z +=  _z;

			rotx += _rotx;
			roty += _roty;
			rotz += _rotz;

			if (0){
				cout << endl;
				//cout << Get_sensor_id(iplane, imodule, iside, isensor) << endl;
				cout  << '\t' << "iplane" << '\t' << "imodule" << '\t' << "iside" << '\t' << "isensor";
				cout  << '\t' << "x" << '\t' << "y" << '\t' << "z";
				cout  << '\t' << "rotx" << '\t' << "roty" << '\t' << "rotz" << endl;
				cout  << '\t' << iplane << '\t' << imodule << '\t' << iside << '\t' << isensor;
				cout  << '\t' << x << '\t' << y << '\t' << z;
				cout  << '\t' << rotx << '\t' << roty << '\t' << rotz << endl;
				double test_x = x;
				double test_y = y;
				double test_z = z;
				//transform_to_sensor_local(Get_sensor_id(iplane, imodule, iside, isensor), test_x, test_y, test_z, misaligned);
				//cout << '\t' << test_x << '\t' << test_y << '\t' << test_z << endl;
			}
		}
	}

	// x, y, z coordinates are expressed in the reference frame of one sensor
	void transform_to_lmd_local(double& x, double& y, double& z, bool misaligned = false){
		double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
		Get_pos_lmd_global(_x, _y, _z, _rotx, _roty, _rotz, misaligned);
		x -= _x;
		y -= _y;
		z -= _z;
		// for now only the rotation around the luminosity detector y axis is taken into account
		_z = z * cos(-rot_y) - x * sin (-rot_y);
		_x = z * sin(-rot_y) + x * cos (-rot_y);
		x = _x;
		z = _z;
	}

	// x, y, z coordinates are expressed in the reference frame of one sensor
	void transform_to_sensor_local(const int sensor_id, double& x, double& y, double& z, bool misaligned = false){
		int ihalf, iplane, imodule, iside, idie, isensor;
		transform_to_lmd_local(x, y, z);
		Get_sensor_by_id(sensor_id, ihalf, iplane, imodule, iside, idie, isensor);
		double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
		Get_pos_sens_local(iplane, imodule, iside, isensor, _x, _y, _z, _rotx, _roty, _rotz, misaligned);
		x -= _x;
		y -= _y;
		z -= _z;
		// for now only the rotation around the sensor z axis is taken into account
		_x = x * cos(-rot_z) - y * sin (-rot_z);
		_y = x * sin(-rot_z) + y * cos (-rot_z);
		x = _x;
		y = _y;
		// take into account that the sensitive area is not centered
		if (iside)
			x -= maps_active_offset_x;
		else // the rotated sensor
			x += maps_active_offset_x;
		y -= maps_active_offset_y;
	}
	*/

	// get the transformation matrix from the PANDA global reference frame to the
	// Luminosity reference frame
	TGeoMatrix& Get_transformation_global_to_lmd_local(bool aligned = true);

	// get the transformation matrix from lmd local reference frame to the
	// cvd disc surface reference frame
	TGeoMatrix& Get_transformation_lmd_local_to_module_side(
			int ihalf, int iplane, int imodule, int iside, bool aligned = true);

	// get the transformation matrix from lmd cvd disc surface frame to the
	// sensor reference frame
	TGeoMatrix& Get_transformation_module_side_to_sensor(
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned = true);

	// get the transformation matrix from PANDA global reference frame to the
	// sensor reference frame
	TGeoMatrix& Get_transformation_global_to_sensor(
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned = true);

	// get the inverse transformation matrix from the PANDA global reference frame to the
	// Luminosity reference frame
	TGeoMatrix& Get_transformation_lmd_local_to_global(bool aligned = true);

	// get the inverse transformation matrix from lmd local reference frame to the
	// cvd disc surface reference frame
	TGeoMatrix& Get_transformation_module_side_to_lmd_local(
			int ihalf, int iplane, int imodule, int iside, bool aligned = true);

	// get the inverse transformation matrix from lmd cvd disc surface frame to the
	// sensor reference frame
	TGeoMatrix& Get_transformation_sensor_to_module_side(
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned = true);

	// get the inverse transformation matrix from PANDA global reference frame to the
	// sensor reference frame
	TGeoMatrix& Get_transformation_sensor_to_global(
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned = true);

	// get the transformation matrix from a ideal sensor to the aligned one
	TGeoMatrix& Get_transformation_sensor_to_sensor_aligned(
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

	// get the transformation matrix from a aligned sensor to the ideal one
	TGeoMatrix& Get_transformation_sensor_aligned_to_sensor(
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

	//************************** 3d vector transformations ******************************

	// Transform from the PANDA global reference frame to the
	// Luminosity reference frame
	TVector3& Transform_global_to_lmd_local(const TVector3& point, bool isvector = false, bool aligned = true);

	// Transform from lmd local reference frame to the
	// cvd disc surface reference frame
	TVector3& Transform_lmd_local_to_module_side(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, bool isvector = false, bool aligned = true);

	// Transform from lmd cvd disc surface frame to the
	// sensor reference frame
	TVector3& Transform_module_side_to_sensor(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

	// Transform from PANDA global reference frame to the
	// sensor reference frame
	TVector3& Transform_global_to_sensor(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

	// Transform from the PANDA global reference frame to the
	// Luminosity reference frame
	TVector3& Transform_lmd_local_to_global(const TVector3& point, bool isvector = false, bool aligned = true);

	// Transform from lmd local reference frame to the
	// cvd disc surface reference frame
	TVector3& Transform_module_side_to_lmd_local(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, bool isvector = false, bool aligned = true);

	// Transform from lmd cvd disc surface frame to the
	// sensor reference frame
	TVector3& Transform_sensor_to_module_side(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

	// Transform from PANDA global reference frame to the
	// sensor reference frame
	TVector3& Transform_sensor_to_global(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false, bool aligned = true);

	// Transform from a ideal sensor to the aligned one
	TVector3& Transform_sensor_to_sensor_aligned(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false);

	// Transform from a aligned sensor to the ideal one
	TVector3& Transform_sensor_aligned_to_sensor(const TVector3& point,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector = false);

	// ************************* 3d matrix rotations ***************************************

	// Transform from the PANDA global reference frame to the
	// Luminosity reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_global_to_lmd_local(const TMatrixD& matrix, bool aligned = true);

	// Transform from lmd local reference frame to the
	// cvd disc surface reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_lmd_local_to_module_side(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, bool aligned = true);

	// Transform from lmd cvd disc surface frame to the
	// sensor reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_module_side_to_sensor(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

	// Transform from PANDA global reference frame to the
	// sensor reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_global_to_sensor(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

	// Transform from the PANDA global reference frame to the
	// Luminosity reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_lmd_local_to_global(const TMatrixD& matrix, bool aligned = true);

	// Transform from lmd local reference frame to the
	// cvd disc surface reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_module_side_to_lmd_local(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, bool aligned = true);

	// Transform from lmd cvd disc surface frame to the
	// sensor reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_sensor_to_module_side(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

	// Transform from PANDA global reference frame to the
	// sensor reference frame
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_sensor_to_global(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned = true);

	// Transform from a ideal sensor to the aligned one
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_sensor_to_sensor_aligned(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor);

	// Transform from a aligned sensor to the ideal one
	// treats only 3 x 3 matrices representing the space
	TMatrixD& Transform_sensor_aligned_to_sensor(const TMatrixD& matrix,
			int ihalf, int iplane, int imodule, int iside, int idie, int isensor);


	// *************************

	// get a pointer to the requested matrices with checks
	// returns NULL if no matrices available
	// do not delete!
	map<string, TGeoMatrix* >* Get_matrices(bool aligned = true);

	// get a pointer to the requested matrix with checks
	// returns NULL if no matrix available
	// do not delete!
	TGeoMatrix* Get_matrix(int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned = true);

	// x, y, z coordinate transformation from the PANDA global reference frame to the
	// local reference frame of the luminosity monitor
	void Transform_global_to_lmd_local(double& x, double& y, double& z, bool aligned = true);

	// x, y, z vector transformation from the PANDA global reference frame to the
	// local reference frame of the luminosity monitor
	void Transform_global_to_lmd_local_vect(double& x, double& y, double& z, bool aligned = true);

	// x, y, z coordinate transformation from the local sensor reference frame to the
	// local reference frame of the luminosity monitor
	//void transform_sensor_local_to_lmd_local(const int sensor_id, double& x, double& y, double& z, bool misaligned = false);

	// x, y, z coordinate transformation from the local luminosity reference frame to the
	// global reference frame of panda
	//void transform_local_lmd_to_global(const int sensor_id, double& x, double& y, double& z, bool misaligned = false);

	// x, y, z coordinate transformation between the local sensor reference frames
	// of misaligned and aligned sensors
	//void transform_local_sensor();


	// Generates the luminosity monitor geometry into the mother volume
	// Please make sure that mother volume is large enough or that it
	// is an assembly volume
	// in addition rotation and translation matrices are calculated
	// to store those into a file please use
	// Write_transformation_matrices(filename, false);
	void Generate_rootgeom(TGeoVolume& mothervol, bool misaligned = false);

};




#endif /* PNDLMDDIM_H_ */
