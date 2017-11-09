/*
 * PndLmdGeometryFactory.h
 *
 *  Created on: Nov 6, 2017
 *      Author: steve
 */

#ifndef LMD_LMDMC_PNDLMDGEOMETRYFACTORY_H_
#define LMD_LMDMC_PNDLMDGEOMETRYFACTORY_H_

#include <vector>

#include <boost/property_tree/ptree.hpp>

class TGeoManager;
class TGeoVolume;
class FairGeoLoader;

class PndLmdGeometryFactory {
	boost::property_tree::ptree geometry_property_tree;
	std::vector<std::string> navigation_paths;
	TGeoManager* gGeoMan;

	// volumes which will be replicated
	TGeoVolume* active_sensor_volume;
	TGeoVolume* passive_sensor_volume;
	TGeoVolume* sensor_cables_volume;
	TGeoVolume* aluminum_support_volume;
	TGeoVolume* cvd_disc_volume;
	// replicated volume counters
	unsigned int global_sensor_id_counter;
	unsigned int global_aluminum_support_counter;
	unsigned int global_cvd_disc_counter;

	void retrieveMaterial(FairGeoLoader* geoLoad);

	TGeoVolume* generateVacuumBox() const;
	void generateBeamPipe(TGeoVolume& mother_volume) const;

	TGeoVolume* generateDetectorHalf(bool is_upper_half) const;

	void generateSensorModule(TGeoVolume& mother_volume) const;
	void generateCoolingStructures();
	void generateSensor();

public:
	PndLmdGeometryFactory(const boost::property_tree::ptree& geometry_property_tree_);
	virtual ~PndLmdGeometryFactory();

	void init(FairGeoLoader* geoLoad);

	void generateLmdGeometry(TGeoVolume& mother_volume) const;
};

#endif /* LMD_LMDMC_PNDLMDGEOMETRYFACTORY_H_ */
