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

public:
	PndLmdGeometryFactory(const boost::property_tree::ptree& geometry_property_tree_);
	virtual ~PndLmdGeometryFactory();

	void retrieveMaterial(FairGeoLoader* geoLoad);

	void generateLmdGeometry(TGeoVolume& mother_volume) const;

	TGeoVolume* generateVacuumBox() const;
	void generateBeamPipe(TGeoVolume& mother_volume) const;
	void generateDetectorHalf(TGeoVolume& mother_volume) const;
	void generateCoolingStructure(TGeoVolume& mother_volume) const;
	void generateSensorModule(TGeoVolume& mother_volume) const;
	void generateSensor(TGeoVolume& mother_volume) const;

};

#endif /* LMD_LMDMC_PNDLMDGEOMETRYFACTORY_H_ */
