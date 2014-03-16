/*
 * PndLmdLumiFitOptions.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIFITOPTIONS_H_
#define PNDLMDLUMIFITOPTIONS_H_

#include "LumiImpl/LumiFitStructs.h"
#include "ModelFramework/core/ModelStructs.h"
#include "ModelFramework/fit/EstimatorOptions.h"

#include <set>
#include <string>

#include "TObject.h"

class PndLmdAcceptance;

/**
 * \brief This class contains the various fit options which are independent of the fit model itself.
 *
 * The user has the choice between several different fit options
 * - 1d or 2d fit
 * - use t instead theta
 * - acceptance correction
 * - detector smearing
 * - detector smearing parametrizations
 * - fit ranges
 * - estimator type
 * - estimator options
 *
 * The fields can only be set by the user within the #PndLmdFitFacade class, to
 * avoid further alterations of the variables after a their intial setting.
 */
class PndLmdLumiFitOptions: public TObject {
  friend class PndLmdFitFacade;
  friend class PndLmdLumiHelper;

private:
  LumiFit::PndLmdFitModelOptions model_opt;

  EstimatorOptions est_opt;

  LumiFit::LmdEstimatorType estimator_type;

  std::set<std::string, ModelStructs::string_comp> free_parameter_names;

public:
  PndLmdLumiFitOptions();

  const LumiFit::PndLmdFitModelOptions& getFitModelOptions() const;
  const EstimatorOptions& getEstimatorOptions() const;
  const std::set<std::string, ModelStructs::string_comp>& getFreeParameterSet() const;

  /**
   * Less then operator. Will return true if "this" fit options are "less" in value.
   */
  bool operator <(const PndLmdLumiFitOptions & rhs) const;
  /**
   * Greater then operator. Will return true if "this" fit options are "greater" in value.
   */
  bool operator >(const PndLmdLumiFitOptions & rhs) const;
  /**
   * Comparison operator. Will return true only if all fit options are equal in value.
   */
  bool operator ==(const PndLmdLumiFitOptions & fit_options) const;
  /**
   * Inverse comparison operator @see operator==()
   */
  bool operator !=(const PndLmdLumiFitOptions & fit_options) const;
  /**
   * Output stream operator for printing out fit options for information.
   */
  friend std::ostream & operator <<(std::ostream & os,
      const PndLmdLumiFitOptions & fit_options);

ClassDef(PndLmdLumiFitOptions,1)
  ;
};

#endif /* PNDLMDLUMIFITOPTIONS_H_ */
