#include "PndLmdGeometryHelper.h"
#include "PndGeoHandling.h"

#include <regex>

PndLmdGeometryHelper::~PndLmdGeometryHelper() {
}

/*
 * This function does all the important work of translating a lmd volume path to
 * the PndLmdHitLocationInfo. It uses the navigation paths as given in the
 * lmd geometry parameter config file.
 * The structure is as following
 * -> half: up: 0; down: 1
 * -> plane: 0 to n_p (n_p: number of detector planes -1); counting in beam
 * direction
 * -> module: 0 to n_m (n_m: number of modules per half plane -1);
 *    for top half: counting from left to right (clockwise) in beam direction
 *    for top half: counting from right to left (clockwise) in beam direction
 * -> sensor: sensor counter for a module. Total number of sensors per module is
 *    2x (modules per side). Counting starts on front (w.r.t. beam direction)
 */
PndLmdHitLocationInfo PndLmdGeometryHelper::translateVolumePathToHitLocationInfo(const std::string& volume_path) const {
	PndLmdHitLocationInfo hit_info;
	std::stringstream reg_exp;
	for (auto const& nav_path : navigation_paths) {
		reg_exp << "/" << nav_path << "_(\\d+)";
	}

	std::smatch match;

	if (std::regex_search(volume_path, match, std::regex(reg_exp.str()))) {
		hit_info.detector_half = (unsigned char) std::stoul(match[2]);
		hit_info.plane = (unsigned char) std::stoul(match[3]);
		hit_info.module = (unsigned char) std::stoul(match[4]);
		unsigned char sensor_id((unsigned char) std::stoul(match[5]));
		hit_info.module_side = 0;
		hit_info.module_sensor_id = sensor_id;

		unsigned int sensors_per_module_side = geometry_properties.get<unsigned int>("general.sensors_per_module_side");

		if (sensor_id > sensors_per_module_side - 1) {
			hit_info.module_side = 1;
			sensor_id = sensor_id % (sensors_per_module_side - 1);
		}
	} else {
		throw std::runtime_error("PndLmdGeometryHelper::translateVolumePathToHitLocationInfo: geometry "
				"navigation paths mismatch!"
				" Seems like you used a different lmd geo config file to create a lmd "
				"root geometry"
				" which was use in your simulations...");
	}

	return hit_info;
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::createMappingEntry(int sensor_id) {
	PndGeoHandling* geo_handling = PndGeoHandling::Instance();

	std::string vol_path(geo_handling->GetPath(sensor_id).Data());

	PndLmdHitLocationInfo hit_loc_info = translateVolumePathToHitLocationInfo(vol_path);
	volume_path_to_hit_info_mapping[vol_path] = hit_loc_info;
	sensor_id_to_hit_info_mapping[sensor_id] = hit_loc_info;

	return sensor_id_to_hit_info_mapping[sensor_id];
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::createMappingEntry(const std::string& volume_path) {
	PndGeoHandling* geo_handling = PndGeoHandling::Instance();

	int sensor_id(geo_handling->GetShortID(volume_path.c_str()));

	PndLmdHitLocationInfo hit_loc_info = translateVolumePathToHitLocationInfo(volume_path);
	volume_path_to_hit_info_mapping[volume_path] = hit_loc_info;
	sensor_id_to_hit_info_mapping[sensor_id] = hit_loc_info;

	return volume_path_to_hit_info_mapping[volume_path];
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::getHitLocationInfo(const std::string& volume_path) {
	auto const& result = volume_path_to_hit_info_mapping.find(volume_path);
	if (result != volume_path_to_hit_info_mapping.end()) {
		return result->second;
	} else {
		return createMappingEntry(volume_path);
	}
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::getHitLocationInfo(int sensor_id) {
	auto const& result = sensor_id_to_hit_info_mapping.find(sensor_id);
	if (result != sensor_id_to_hit_info_mapping.end()) {
		return result->second;
	} else {
		return createMappingEntry(sensor_id);
	}
}

std::vector<int> PndLmdGeometryHelper::getAvailableOverlapIDs() {
	std::vector<int> result;
	int overlapID;

	for (int iHalf = 0; iHalf < 2; iHalf++) {
		for (int iPlane = 0; iPlane < 4; iPlane++) {
			for (int iModule = 0; iModule < 5; iModule++) {
				for (int iOverlap = 0; iOverlap < 9; iOverlap++) {
					overlapID = 1000 * iHalf + 100 * iPlane + 10 * iModule + iOverlap;
					result.push_back(overlapID);
				}
			}
		}
	}
	return result;
}

TVector3 PndLmdGeometryHelper::transformPndGlobalToLmdLocal(const TVector3& global) {
	Double_t result[3];
	Double_t temp[3];

	temp[0] = global.X();
	temp[1] = global.Y();
	temp[2] = global.Z();

	TString actPath = fGeoManager->GetPath();

	fGeoManager->cd(lmd_root_path.c_str());
	TGeoMatrix *matrix(fGeoManager->GetCurrentNode()->GetMatrix());
	matrix->MasterToLocal(temp, result);

	if (actPath != "" && actPath != " ")
		fGeoManager->cd(actPath);

	return TVector3(result);
}

TVector3 PndLmdGeometryHelper::transformPndGlobalToSensor(const TVector3& global, int sensor_id) {
	Double_t result[3];
	Double_t temp[3];

	temp[0] = global.X();
	temp[1] = global.Y();
	temp[2] = global.Z();

	auto matrix = getMatrixPndGlobalToSensor(sensor_id);
	matrix.MasterToLocal(temp, result);

	return TVector3(result);
}

const TGeoHMatrix PndLmdGeometryHelper::getMatrixPndGlobalToSensor(const int sensorId) {

	PndGeoHandling* geo_handling = PndGeoHandling::Instance();
	std::string vol_path(geo_handling->GetPath(sensorId).Data());

	TString actPath = fGeoManager->GetPath();

	fGeoManager->CdTop();
	fGeoManager->cd(vol_path.c_str());

	// from active area to sensor
	fGeoManager->CdUp();
	TGeoMatrix *senToAct(fGeoManager->GetCurrentNode()->GetMatrix());

	//from sensor to module
	fGeoManager->CdUp();
	TGeoMatrix *modToSen(fGeoManager->GetCurrentNode()->GetMatrix());

	//from plane to module
	fGeoManager->CdUp();
	TGeoMatrix *plaToMod(fGeoManager->GetCurrentNode()->GetMatrix());

	//from half to plane
	fGeoManager->CdUp();
	TGeoMatrix *halToPla(fGeoManager->GetCurrentNode()->GetMatrix());

	//from lmd_local to plane
	fGeoManager->CdUp();
	TGeoMatrix *lmdToHal(fGeoManager->GetCurrentNode()->GetMatrix());

	//from global to lmd_local
	fGeoManager->CdUp();
	TGeoMatrix *gloToLmd(fGeoManager->GetCurrentNode()->GetMatrix());

	auto matrix = &((*gloToLmd) * (*lmdToHal) * (*halToPla) * (*plaToMod) * (*modToSen) * (*senToAct));

	//fGeoManager->LocalToMaster(temp, result);

	if (actPath != "" && actPath != " ")
		fGeoManager->cd(actPath);

	return TGeoHMatrix(matrix);
}

const TGeoHMatrix PndLmdGeometryHelper::getMatrixSensorToPndGlobal(const int sensorId){
	auto result = getMatrixPndGlobalToSensor(sensorId);
	return TGeoHMatrix(result.Inverse());
}

int PndLmdGeometryHelper::getOverlapIdFromSensorIDs(int id1, int id2) {

	int fhalf, fplane, fmodule, fside, fsensor;
	int bhalf, bplane, bmodule, bside, bsensor;

	auto const &infoOne = getHitLocationInfo(id1);
	auto const &infoTwo = getHitLocationInfo(id2);

	int smalloverlap = -1;

	fhalf = infoOne.detector_half;
	bhalf = infoTwo.detector_half;

	//the necessities for overlapping, must be on same half, plane, module and other side
	if (bhalf != fhalf) {
		return -1;
	}

	fside = infoOne.module_side;
	bside = infoTwo.module_side;

	fplane = infoOne.plane;
	bplane = infoTwo.plane;

	fmodule = infoOne.module;
	bmodule = infoTwo.module;

	fsensor = infoOne.module_sensor_id;
	bsensor = infoTwo.module_sensor_id;

	if (fside == bside) {
		return -1;
	}

	if (bplane != fplane) {
		return -1;
	}
	if (bmodule != fmodule) {
		return -1;
	}

	//0to5
	if (fsensor == 0 && bsensor == 5) {
		smalloverlap = 0;
	}
	//3to8
	else if (fsensor == 3 && bsensor == 8) {
		smalloverlap = 1;
	}
	//4to9
	else if (fsensor == 4 && bsensor == 9) {
		smalloverlap = 2;
	}
	//3to6
	else if (fsensor == 3 && bsensor == 6) {
		smalloverlap = 3;
	}
	//1to8
	else if (fsensor == 1 && bsensor == 8) {
		smalloverlap = 4;
	}
	//2to8
	else if (fsensor == 2 && bsensor == 8) {
		smalloverlap = 5;
	}
	//2to9
	else if (fsensor == 2 && bsensor == 9) {
		smalloverlap = 6;
	}
	//3to7
	else if (fsensor == 3 && bsensor == 7) {
		smalloverlap = 7;
	}
	//4to7
	else if (fsensor == 4 && bsensor == 7) {
		smalloverlap = 8;
	}
	return 1000 * fhalf + 100 * fplane + 10 * fmodule + smalloverlap;

}

