/*
 * PndLmdDim.h
 *
 * this class gives you methods to retrieve parameters of the geometry
 * dimensions are in cm and radian
 *
 * Global comments:
 *
 * - with rotation around x, y, z I messed up. Sorry.
 *   x y z have now the usual meaning of euler angle operations meaning
 *   1. rotation around z
 *   2. rotation around x
 *   3. rotation around new z
 *
 * - important for sensor id:
 *   PANDAROOT generates a sensor id according to the order it was created
 *   For a working id -> sensor conversion one must create
 *   the luminosity detector with the following loops:
 *
 *   	for (int i_plane = 0; i_plane < 4; i_plane++) {
...
		for (int i_cvd_disc = 0; i_cvd_disc < LmdDim::n_cvd_discs-2; i_cvd_disc++) {
...
			for (int iside = 0; iside < 2; iside++) {
...
				int isensor = -1;
				for (int maps_col = 0; maps_col < LmdDim::maps_n_col; maps_col++) {
					for (int maps_row = 0; maps_row < LmdDim::maps_n_row; maps_row++) {
						if (LmdDim::enabled[maps_row][maps_col]) isensor++;

	otherwise all assumptions in LmdDim::transform_to_sensor_local won't work!
 *
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

using namespace std;

namespace LmdDim {
	// pi
	static const double pi = 3.141592654;
	// number of detector planes
	static const unsigned int n_planes = 4;
	// position of planes where the first plane defines the origin
	static const double plane_pos_z[4] = {0.0, 20.0, 30.0, 40.0};
	// ****************************** cvd cooling support discs ************************
	// cvd_diamond is cut out of 79.5 mm discs of 200 micron thickness
	// inner min. radius due to beam pipe + a safety margin
	static const double inner_rad = 3.7;
	// not used yet but should be the outer acceptance
	static const double outer_rad = 12.;
	// number of CVD diamond discs per plane
	static const int n_cvd_discs = 10;
	// radius of a CVD diamond disc
	static const double cvd_disc_rad = 7.95/2.;
	// the half of the diamond thickness
	static const double cvd_disc_thick_half = 0.01;
	// even and odd discs in a plane will be shifted in z in order to prevent
	// mechanical damage during assembly
	static const double cvd_disc_even_odd_offset = 0.25;
	// angle from the division of a circle into n_cvd_discs
	static const double delta_phi = 2.*pi / ((double) (n_cvd_discs));
	// a polygon of n_cvd_discs sides fitting a radius of inner_rad
	// has a side length pol_side_lg of
	static const double pol_side_lg_half = inner_rad * sin(delta_phi / 2.);
	// the minimum distance to the center of the polygone is given by
	static const double pol_side_dist_min = inner_rad * cos(delta_phi / 2.);
	// the cvd disc has to be placed such that the disc crosses
	// the inner ring at an angle of 0 and delta_phi
	// this defines the distance to the center according to pythagoras
	static const double cvd_disc_dist = pol_side_dist_min + sqrt(
		cvd_disc_rad * cvd_disc_rad - pol_side_lg_half * pol_side_lg_half);

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation

	// x is radial to the beam pipe
	static const double cvd_offset_x = 0.01;//2.01;
	// y is tangent to the beam pipe
	static const double cvd_offset_y = 0.01;//1.01;
	// z is along the beam pipe
	static const double cvd_offset_z = 0.01;//1.01;
	// x is a rotation around the radial component of the beam pipe
	static const double cvd_tilt_x = 0.; // please do not use yet
	// y is a rotation around the tangent component of the beam pipe
	static const double cvd_tilt_y = 0.; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	static const double cvd_tilt_z = 0.001;//1e0;
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
	static const int maps_n_col = 3;
	static const int maps_n_row = 2;
	// enabled [row][col]
	static const bool enabled [maps_n_row][maps_n_col] = {{true,true,true},{false,true,true}};
	static const int n_sensors = 5;
	// NOTE: MOST of the following VARIABLES are HALF of it
	// due to geometry construction in GEANT
	static const double maps_thickness      = 0.005/ 2.;
	static const double maps_passive_top    = 0.1 / 2.;//*12000.;
	static const double maps_passive_bottom = 0.4  / 2.;
	static const double maps_passive_left   = 0.2 / 2.;//*6000.;
	static const double maps_passive_right  = 0.05 / 2.;//*3000.;
	static const double maps_active_width   = 2.0  / 2. - maps_passive_left - maps_passive_right;
	static const double maps_active_height  = 2.0  / 2. - maps_passive_top  - maps_passive_bottom;

	static const double maps_width  = maps_passive_left + maps_active_width  + maps_passive_right ;
	static const double maps_height = maps_passive_top  + maps_active_height + maps_passive_bottom;

	static const double maps_active_offset_x   = (maps_passive_left - maps_passive_right )/2.;
	static const double maps_active_offset_y   = (maps_passive_top  - maps_passive_bottom)/2.;


	static const double die_gap = 0.1; // (cm)

	static const double maps_die_width  = maps_width  * maps_n_col;
	static const double maps_die_height = maps_height * maps_n_row + die_gap * maps_n_row - 1;

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation\
	// translations along z as well as rotations around x and y are
	// negligible for dies glued on a cvd diamond
	// rotation around z is not working yet

	// x is along the edge of the cvd disc
	static const double die_offset_x = 0.01;//2.01;
	// y is orthogonal to the edge of the cvd disc
	static const double die_offset_y = 0.01;//1.01;
	// z is along the beam pipe
	static const double die_offset_z = 0.; //should not be used -> crashing volumes;
	// x is a rotation around the edge of the cvd disc
	static const double die_tilt_x = 0.; // please do not use yet
	// y is a rotation around the orthogonal component of the edge of the cvd disc
	static const double die_tilt_y = 0.; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	static const double die_tilt_z = 0.;// please do not use yet;
	//*********************************** global parameters *************************************
	// where bending starts with
	static const double end_seg_upstream = 361;
	// the bending radius
	static const double r_bend = 5700.;
	// and the angle of the circle path
	static const double phi_bend = 40.0e-3;;
	// the point where both tangents of the straight beam pipe tubes meet is
	static const double pos_rot_z = end_seg_upstream + tan(phi_bend/2.)*r_bend;
	// z position of the lmd
	static const double pos_z = 1050. + 600.; //(cm)
	static const double end_seg_bend = end_seg_upstream+sin(phi_bend)*r_bend;
	// x position of the lmd
	static const double pos_x =
			( pos_z - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend);
	// y position of the lmd
	static const double pos_y = 0.;
	static const double rot_x = 0.;
	static const double rot_y = phi_bend;
	static const double rot_z = 0.;

	// returns false when one of the variables exceeds design values
	static bool Is_valid_idcall(int iplane, int imodule = 0, int iside = 0, int isensor = 0){
		if (iplane  < 0 || iplane  >= LmdDim::n_planes) return false;
		if (imodule < 0 || imodule >= LmdDim::n_cvd_discs) return false;
		if (iside   < 0 || iside   >= 2) return false;
		if (isensor < 0 || isensor >= LmdDim::n_sensors) return false;
		return true;
	}

	// get the sensor id for a sensor on a given side, module and plane
	static int Get_sensor_id(int iplane, int imodule, int iside, int isensor){
		return isensor + iside * n_sensors + imodule * n_sensors * 2 + iplane * n_cvd_discs * n_sensors * 2;
	}

	// get the sensor position by it's id in terms of plane module and side
	static void Get_sensor_by_id(const int sensor_id, int& iplane, int& imodule, int& iside, int& isensor){
		int _sensor_id = sensor_id;
		isensor = _sensor_id % n_sensors;
		_sensor_id /= n_sensors;
		iside = _sensor_id % 2;
		_sensor_id /= 2;
		imodule = _sensor_id % n_cvd_discs;
		_sensor_id /= n_cvd_discs;
		iplane = _sensor_id % n_planes;
		if (!Is_valid_idcall(iplane, imodule, iside, isensor)){
			iplane = 0;
			imodule = 0;
			iside = 0;
			isensor = 0;
			cout << "Error in PndLmdDim::Get_sensor_by_id: "<< sensor_id <<" is not a valid sensor id!" << endl;
		}
	}

	// when calling for a displaced module, the generated
	// value must be kept same for the cvd diamond as well
	// as the sensors sitting on the diamond it self
	// the storage is realized by a map
	// the vector contains offsets in x, y, z, rotx, roty, rotz;
	static map<string, vector<double> > offsets;
	static map<string, vector<double> >::iterator itoffset;
	static string Generate_offsetkey(int iplane, int imodule, int iside, int isensor){
		stringstream keystream;
		keystream << iplane << imodule << iside << isensor;
		return keystream.str();
	}
	static void Get_offset(int iplane, int imodule, int iside, int isensor,
			double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz){
		string key = LmdDim::Generate_offsetkey(iplane, imodule, iside, isensor);
		itoffset = offsets.find(key);
		if (itoffset != offsets.end()){
			x = itoffset->second[0];
			y = itoffset->second[1];
			z = itoffset->second[2];
			rotx = itoffset->second[3];
			roty = itoffset->second[4];
			rotz = itoffset->second[5];
		} else {
			// the precision of cvd discs is requested
			// when requested offset applies also to the sensors
			// on both sides
			if (iside < 0 && isensor < 0){
				x = gRandom->Gaus(0, cvd_offset_x);
				y = gRandom->Gaus(0, cvd_offset_y);
				z = gRandom->Gaus(0, cvd_offset_z);
				rotx = gRandom->Gaus(0, cvd_tilt_x);
				roty = gRandom->Gaus(0, cvd_tilt_y);
				rotz = gRandom->Gaus(0, cvd_tilt_z);
			}
			// the precision of dies containing sensors is requested
			// when requested offset applies to all sensors
			// on one side
			if (iside >= 0 && isensor < 0 ){
				x = gRandom->Gaus(0, die_offset_x);
				y = gRandom->Gaus(0, die_offset_y);
				z = gRandom->Gaus(0, die_offset_z);
				rotx = gRandom->Gaus(0, die_tilt_x);
				roty = gRandom->Gaus(0, die_tilt_y);
				rotz = gRandom->Gaus(0, die_tilt_z);
			}
			offsets[key].push_back(x);
			offsets[key].push_back(y);
			offsets[key].push_back(z);
			offsets[key].push_back(rotx);
			offsets[key].push_back(roty);
			offsets[key].push_back(rotz);
		}
	}

	// several functions returning the position and orientation of
	// the luminosity detector

	// the global system is defined by PANDA
	// returns the position and rotation of the lmd detector
	// the origin is PANDA interaction point
	// first comes translation than rotation
	static void Get_pos_lmd_global(double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz,
			bool misaligned = false){
		rotx = LmdDim::rot_x;
		roty = LmdDim::rot_y;// LmdDim::phi_bend;
		rotz = LmdDim::rot_z;
		x = LmdDim::pos_x;
		y = LmdDim::pos_y;
		z = LmdDim::pos_z;
	}

	// the local system is where the first plane is at xyz = 0 and
	// the detector is oriented along z
	// module counting starts from the first plane (positive x and positive y)
	// rotation is right handed

	// rotation of the i'th module around z
	static double Get_rot_z_local(int imodule){
		if (LmdDim::Is_valid_idcall(0, imodule))
			return LmdDim::delta_phi * imodule + LmdDim::delta_phi/2.;
		else
			return 0;
	}

	// returns the position and rotation of the i'th module at the i'th plane
	// the origin is the center of the cvd disc
	// first comes translation than rotation
	static void Get_pos_mod_local(int iplane, int imodule,
			double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz,
			bool misaligned = false){
		//x = 0; y = 0; z = 0; rotx = 0; roty = 0; rotz = 0;
		if (LmdDim::Is_valid_idcall(iplane, imodule)){
			double angle = Get_rot_z_local(imodule);
			double add_z = LmdDim::cvd_disc_even_odd_offset;
			if ((imodule%2)==0) add_z = -add_z;
			x += LmdDim::cvd_disc_dist*cos(angle);
			y += LmdDim::cvd_disc_dist*sin(angle);
			z += LmdDim::plane_pos_z[iplane] + add_z;
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
	static void Get_pos_sens_local(int iplane, int imodule, int iside, int isensor,
			double& x, double& y, double& z,
			double& rotx, double& roty, double& rotz,
			bool misaligned = false){
		x = 0; y = 0; z = 0; rotx = 0; roty = 0; rotz = 0;
		if (LmdDim::Is_valid_idcall(iplane, imodule, iside, isensor)){
			// retrieve the (aligned) center of the corresponding CVD disc
			LmdDim::Get_pos_mod_local(iplane, imodule, x, y, z, rotx, roty, rotz, misaligned);
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
			const double _sinhalf = sin (LmdDim::delta_phi/2.);
			const double _coshalf = cos (LmdDim::delta_phi/2.);
			const double _edge_y = -_sinhalf*LmdDim::inner_rad;
			// angle between the edge and the half of the cvd disc
			const double _edgeangle = asin(-_edge_y/LmdDim::cvd_disc_rad);
			const double _edge_x = - LmdDim::cvd_disc_rad * cos(_edgeangle);
			_x = _edge_x + offset_x*_coshalf + offset_y*_sinhalf;
			_y = _edge_y - offset_x*_sinhalf + offset_y*_coshalf;
			if (isensor < 3) {
				_x += (0.5+isensor)*maps_width*2.*_coshalf + maps_height*_sinhalf;
				_y -= (0.5+isensor)*maps_width*2.*_sinhalf - maps_height*_coshalf;
			}
			if (isensor >= 3 && isensor < LmdDim::n_sensors){
				_x += (0.5+isensor-2)*maps_width*2.*_coshalf + (maps_height*3. + die_gap)*_sinhalf;
				_y -= (0.5+isensor-2)*maps_width*2.*_sinhalf - (maps_height*3. + die_gap)*_coshalf;
			}
			_z = -LmdDim::cvd_disc_thick_half - LmdDim::maps_thickness + offset_z;

			_rotz = - LmdDim::delta_phi/2.;
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

			cout << endl;
			cout << Get_sensor_id(iplane, imodule, iside, isensor) << endl;
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

	// x, y, z coordinates are expressed in the reference frame of one sensor
	static void transform_to_lmd_local(double& x, double& y, double& z, bool misaligned = false){
		double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
		LmdDim::Get_pos_lmd_global(_x, _y, _z, _rotx, _roty, _rotz, misaligned);
		x -= _x;
		y -= _y;
		z -= _z;
		// for now only the rotation around the luminosity detector y axis is taken into account
		_z = z * cos(-LmdDim::rot_y) - x * sin (-LmdDim::rot_y);
		_x = z * sin(-LmdDim::rot_y) + x * cos (-LmdDim::rot_y);
		x = _x;
		z = _z;
	}

	// x, y, z coordinates are expressed in the reference frame of one sensor
	static void transform_to_sensor_local(const int sensor_id, double& x, double& y, double& z, bool misaligned = false){
		int iplane, imodule, iside, isensor;
		LmdDim::transform_to_lmd_local(x, y, z);
		LmdDim::Get_sensor_by_id(sensor_id, iplane, imodule, iside, isensor);
		double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
		Get_pos_sens_local(iplane, imodule, iside, isensor, _x, _y, _z, _rotx, _roty, _rotz, misaligned);
		x -= _x;
		y -= _y;
		z -= _z;
		// for now only the rotation around the sensor z axis is taken into account
		_x = x * cos(-LmdDim::rot_z) - y * sin (-LmdDim::rot_z);
		_y = x * sin(-LmdDim::rot_z) + y * cos (-LmdDim::rot_z);
		x = _x;
		y = _y;
		// take into account that the sensitive area is not centered
		if (iside)
			x -= maps_active_offset_x;
		else // the rotated sensor
			x += maps_active_offset_x;
		y -= maps_active_offset_y;
	}
}

class PndLmdDim {
public:
	PndLmdDim();
	virtual ~PndLmdDim();
};

#endif /* PNDLMDDIM_H_ */
