/*
 * ModelStructs.h
 *
 *  Created on: May 27, 2013
 *      Author: steve
 */

#ifndef MODELSTRUCTS_H_
#define MODELSTRUCTS_H_

namespace ModelStructs {

/**
 * Small structure defining the comparison operator used in the map
 * #model_par_map. Will return true only if this model parameter is equal to
 * the parameter. It is used when concatenating multiple ModelPar objects to
 * avoid having multiple instances of the same parameter. The check is based
 * on a name comparison.
 */
struct stringpair_comp {
	bool operator()(const std::pair<std::string, std::string>& lhs
			, const std::pair<std::string, std::string>& rhs) const {
		if (lhs.first.compare(rhs.first) < 0) {
			return true;
		} else if (lhs.first.compare(rhs.first) == 0) {
			return lhs.second.compare(rhs.second) < 0;
		} else {
			return false;
		}
	}
};

struct string_comp {
	bool operator()(const std::string& lhs, const std::string& rhs) const {
		if (lhs.compare(rhs) < 0) {
			return true;
		}
		return false;
	}
};

}
#endif /* MODELSTRUCTS_H_ */
