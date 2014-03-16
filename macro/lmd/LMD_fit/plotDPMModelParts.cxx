#include "PndLmdResultPlotter.h"

#include <stdlib.h>
#include <utility>

const double theta_min = 2.5;
const double theta_max = 10.0;

const double log_scale = false;

int main(int argc, char* argv[]) {
	if (argc == 2) {
		PndLmdResultPlotter plotter;
		DataStructs::DimensionRange plot_range(theta_min, theta_max);

		plotter.plotDPMModelParts(atof(argv[1]), plot_range, log_scale);
	}
	return 0;
}
