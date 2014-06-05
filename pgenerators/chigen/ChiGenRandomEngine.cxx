/* 
 * @author Alexey Luchinsky
 * @author Stanislav Poslavsky (stvlpos (at) mail.ru)
 */

#include "ChiGenRandomEngine.h"

#include <stdexcept>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

namespace {
    /**
     * uniform distribution
     */
    boost::random::uniform_real_distribution<double> unif;
    /**
     * Boost Mersenne twister
     */
    boost::random::mt19937_64 innerGenerator;

    bool instance_created = false;
}

chigen::ChiGenRandomEngine::ChiGenRandomEngine(long s) {
    if (instance_created)
        throw std::runtime_error(std::string("ChiGenRandomEngine should be only one per session."));
    instance_created = true;
    seed = s;
    innerGenerator = boost::random::mt19937_64(seed);
    unif = boost::random::uniform_real_distribution<double>(0, 1);
}

chigen::ChiGenRandomEngine::~ChiGenRandomEngine() {
}

double chigen::ChiGenRandomEngine::random() {
    return unif(innerGenerator);
}

double chigen::ChiGenRandomEngine::flat() {
    return unif(innerGenerator);
}