#include "PndLmdGeometryHelper.h"
#include "PndGeoHandling.h"

#include <regex>

PndLmdGeometryHelper::~PndLmdGeometryHelper() {}

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
PndLmdHitLocationInfo
PndLmdGeometryHelper::translateVolumePathToHitLocationInfo(
    const std::string& volume_path) const {
  PndLmdHitLocationInfo hit_info;
  std::stringstream reg_exp;
  for (auto const& nav_path : navigation_paths) {
    reg_exp << "/" << nav_path << "_(\\d+)";
  }

  std::smatch match;

  if (std::regex_search(volume_path, match, std::regex(reg_exp.str()))) {
    hit_info.detector_half = (unsigned char)std::stoul(match[2]);
    hit_info.plane = (unsigned char)std::stoul(match[3]);
    hit_info.module = (unsigned char)std::stoul(match[4]);
    unsigned char sensor_id((unsigned char)std::stoul(match[5]));
    hit_info.module_side = 0;
    hit_info.module_sensor_id = sensor_id;

    unsigned int sensors_per_module_side =
        geometry_properties.get<unsigned int>(
            "general.sensors_per_module_side");

    if (sensor_id > sensors_per_module_side - 1) {
      hit_info.module_side = 1;
      sensor_id = sensor_id % (sensors_per_module_side - 1);
    }
  } else {
    throw std::runtime_error(
        "PndLmdGeometryHelper::translateVolumePathToHitLocationInfo: geometry "
        "navigation paths mismatch!"
        " Seems like you used a different lmd geo config file to create a lmd "
        "root geometry"
        " which was use in your simulations...");
  }

  return hit_info;
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::createMappingEntry(
    int sensor_id) {
  PndGeoHandling* geo_handling = PndGeoHandling::Instance();

  std::string vol_path(geo_handling->GetPath(sensor_id).Data());

  PndLmdHitLocationInfo hit_loc_info =
      translateVolumePathToHitLocationInfo(vol_path);
  volume_path_to_hit_info_mapping[vol_path] = hit_loc_info;
  sensor_id_to_hit_info_mapping[sensor_id] = hit_loc_info;

  return sensor_id_to_hit_info_mapping[sensor_id];
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::createMappingEntry(
    const std::string& volume_path) {
  PndGeoHandling* geo_handling = PndGeoHandling::Instance();

  int sensor_id(geo_handling->GetShortID(volume_path.c_str()));

  PndLmdHitLocationInfo hit_loc_info =
      translateVolumePathToHitLocationInfo(volume_path);
  volume_path_to_hit_info_mapping[volume_path] = hit_loc_info;
  sensor_id_to_hit_info_mapping[sensor_id] = hit_loc_info;

  return volume_path_to_hit_info_mapping[volume_path];
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::getHitLocationInfo(
    const std::string& volume_path) {
  auto const& result = volume_path_to_hit_info_mapping.find(volume_path);
  if (result != volume_path_to_hit_info_mapping.end()) {
    return result->second;
  } else {
    return createMappingEntry(volume_path);
  }
}

const PndLmdHitLocationInfo& PndLmdGeometryHelper::getHitLocationInfo(
    int sensor_id) {
  auto const& result = sensor_id_to_hit_info_mapping.find(sensor_id);
  if (result != sensor_id_to_hit_info_mapping.end()) {
    return result->second;
  } else {
    return createMappingEntry(sensor_id);
  }
}
