/*
 * PndLmdDim.cpp
 *
 * this class gives you methods to retrieve parameters of the geometry
 *
 *  Created on: Oct 5, 2012
 *      Author: promme
 */

#include <PndLmdDim.h>
#include<TGeoMatrix.h>

PndLmdDim* PndLmdDim::instance = 0;


PndLmdDim::PndLmdDim()
{
	// pi
	pi = 3.141592654;
	// number of detector planes
	n_planes = 4;
	// position of planes where the first plane defines the origin
	plane_pos_z = new double[4];
	plane_pos_z[0] = 0.0; plane_pos_z[1] = 20.0; plane_pos_z[2] = 30.0; plane_pos_z[3] = 40.0;
	// ****************************** cvd cooling support discs ************************
	// cvd_diamond is cut out of 79.5 mm discs of 200 micron thickness
	// inner min. radius due to beam pipe + a safety margin
	inner_rad = 3.7;
	// not used yet but should be the outer acceptance
	outer_rad = 12.;
	// number of CVD diamond discs per plane
	n_cvd_discs = 10;
	// radius of a CVD diamond disc
	cvd_disc_rad = 7.95/2.;
	// the half of the diamond thickness
	cvd_disc_thick_half = 0.01;
	// even and odd discs in a plane will be shifted in z in order to prevent
	// mechanical damage during assembly
	cvd_disc_even_odd_offset = 0.25;
	// angle from the division of a circle into n_cvd_discs
	delta_phi = 2.*pi / ((double) (n_cvd_discs));
	// a polygon of n_cvd_discs sides fitting a radius of inner_rad
	// has a side length pol_side_lg of
	pol_side_lg_half = inner_rad * sin(delta_phi / 2.);
	// the minimum distance to the center of the polygone is given by
	pol_side_dist_min = inner_rad * cos(delta_phi / 2.);
	// the cvd disc has to be placed such that the disc crosses
	// the inner ring at an angle of 0 and delta_phi
	// this defines the distance to the center according to pythagoras
	cvd_disc_dist = pol_side_dist_min + sqrt(
		cvd_disc_rad * cvd_disc_rad - pol_side_lg_half * pol_side_lg_half);

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation

	// x is radial to the beam pipe
	cvd_offset_x = 0.01;//2.01;
	// y is tangent to the beam pipe
	cvd_offset_y = 0.01;//1.01;
	// z is along the beam pipe
	cvd_offset_z = 0.01;//1.01;
	// x is a rotation around the radial component of the beam pipe
	cvd_tilt_x = 0.; // please do not use yet
	// y is a rotation around the tangent component of the beam pipe
	cvd_tilt_y = 0.; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	cvd_tilt_z = 0.001;//1e0;
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
	maps_n_col = 3;
	maps_n_row = 2;
	// enabled [row][col]
	enabled = new bool*[maps_n_row];
	enabled[0] = new bool[maps_n_col];
	enabled[1] = new bool[maps_n_col];
	enabled[0][0] = true;
	enabled[0][1] = true;
	enabled[0][2] = true;
	enabled[1][0] = false;
	enabled[1][1] = true;
	enabled[1][2] = true;
	n_sensors = 5;
	// NOTE: MOST of the following VARIABLES are HALF of it
	// due to geometry construction in GEANT
	maps_thickness      = 0.005/ 2.;
	maps_passive_top    = 0.1 / 2.;//*12000.;
	maps_passive_bottom = 0.4  / 2.;
	maps_passive_left   = 0.2 / 2.;//*6000.;
	maps_passive_right  = 0.05 / 2.;//*3000.;
	maps_active_width   = 2.0  / 2. - maps_passive_left - maps_passive_right;
	maps_active_height  = 2.0  / 2. - maps_passive_top  - maps_passive_bottom;

	maps_width  = maps_passive_left + maps_active_width  + maps_passive_right ;
	maps_height = maps_passive_top  + maps_active_height + maps_passive_bottom;

	maps_active_offset_x   = (maps_passive_left - maps_passive_right )/2.;
	maps_active_offset_y   = (maps_passive_top  - maps_passive_bottom)/2.;


	die_gap = 0.1; // (cm)

	maps_die_width  = maps_width  * maps_n_col;
	maps_die_height = maps_height * maps_n_row + die_gap * maps_n_row - 1;

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation\
	// translations along z as well as rotations around x and y are
	// negligible for dies glued on a cvd diamond
	// rotation around z is not working yet

	// x is along the edge of the cvd disc
	die_offset_x = 0.01;//2.01;
	// y is orthogonal to the edge of the cvd disc
	die_offset_y = 0.01;//1.01;
	// z is along the beam pipe
	die_offset_z = 0.; //should not be used -> crashing volumes;
	// x is a rotation around the edge of the cvd disc
	die_tilt_x = 0.; // please do not use yet
	// y is a rotation around the orthogonal component of the edge of the cvd disc
	die_tilt_y = 0.; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	die_tilt_z = 0.;// please do not use yet;
	//*********************************** lumi box parameters ***********************************
	// see CAD files for details
	// https://edms.cern.ch/nav/P:FAIR-000000719:V0/P:FAIR-000000726:V0/TAB3
	// width
	box_size_x = 36./2.;
	// height
	box_size_y = 50./2.;
	// length
	box_size_z = 110./2.;
	// thickness of the V2A steel plates
	box_thickness = 0.5;
	// position of the inner rib
	pos_rib = 10.;
	// beam pipe radius at entrance
	rad_entrance = 9.;
	// beam pipe radius at exit
	rad_exit = 4.5;
	// beam pipe separating non interacting paricles
	rad_pipe = 3.5;
	// beam pipe thickness;
	pipe_thickness = 0.1;
	// cone height of the transition region
	length_transision = 30.;
	// length of the inner pipe
	length_pipe = 60.;
	//*********************************** global parameters *************************************
	// where bending starts with
	end_seg_upstream = 361;
	// the bending radius
	r_bend = 5700.;
	// and the angle of the circle path
	phi_bend = 40.0e-3;;
	// the point where both tangents of the straight beam pipe tubes meet is
	pos_rot_z = end_seg_upstream + tan(phi_bend/2.)*r_bend;
	// z position of the lmd box
	pos_z = 1050.; //(cm)
	// position of the first detector plane
	pos_plane_0 = 80.;
	end_seg_bend = end_seg_upstream+sin(phi_bend)*r_bend;
	// x position of the lmd
	pos_x =
			( pos_z - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend);
	// y position of the lmd
	pos_y = 0.;
	rot_x = 0.;
	rot_y = phi_bend;
	rot_z = 0.;

	TGeoRotation* georot_no = new TGeoRotation("georot_no", 0., 0., 0.); // no rotation
}



PndLmdDim::PndLmdDim(const PndLmdDim & instance)
{
}



PndLmdDim::~PndLmdDim()
{
	delete instance;
}



PndLmdDim & PndLmdDim::Get_instance()
{
	if (!instance){
		instance = new PndLmdDim();
	}
	return (*instance);
}



void PndLmdDim::transform_sensor_local_to_lmd_local(const int sensor_id, double & x, double & y, double & z, bool misaligned)
{
}



void PndLmdDim::transform_local_lmd_to_global(const int sensor_id, double & x, double & y, double & z, bool misaligned)
{
}



void PndLmdDim::transform_local_sensor()
{
}
