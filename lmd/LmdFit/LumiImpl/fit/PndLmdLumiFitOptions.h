/*
 * PndLmdLumiFitOptions.h
 *
 *  Created on: Jun 27, 2012
 *      Author: steve
 */

#ifndef PNDLMDLUMIFITOPTIONS_H_
#define PNDLMDLUMIFITOPTIONS_H_

#include "../LumiFitStructs.h" // .. is needed for rootcint to find the header
#include "TObject.h"
#include "TString.h"

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
 * In case the user only needs the standard luminosity fitting procedure, the
 * fit options instances should be created by the normal user only within the
 * #PndLmdFitFacade class.
 * For more advanced usage they can be created directly!
 *
 */
class PndLmdLumiFitOptions: public TObject {
public:
	/**
	 * Binary fit options of the model. See #LumiFit::LmdBinaryFitOptions for a
	 * detailed description.
	 */
	LumiFit::LmdBinaryFitOptions model_binary_options;

	PndLmdAcceptance *acceptance;

	unsigned int free_parameters_code;

	int smearing_model_type;
	int acc_intpol_type;
	int dpm_elastic_model_parts;

	LumiFit::LmdDimensionRange primary_dimension_fit_range;
	LumiFit::LmdDimensionRange secondary_dimension_fit_range;
	bool primary_dimension_fit_range_active;
	bool secondary_dimension_fit_range_active;

	TString resolution_parametrization_file_url;
	bool resolution_parametrization_file_url_active;

	void initBinaryOptions(unsigned long bit_flag_options);

public:
	PndLmdLumiFitOptions();
	//PndLmdLumiFitOptions(const PndLmdLumiFitOptions & fit_options);

	/**
	 * Get method for the model binary fit options
	 * @returns integer format of the binary fit options
	 */
	const LumiFit::LmdBinaryFitOptions & getModelBinaryOptions() const;
	void setModelBinaryOptions(
			LumiFit::LmdBinaryFitOptions model_binary_options_);

	PndLmdAcceptance *getAcceptance() const;
	void setAcceptance(PndLmdAcceptance *acceptance_);

	/**
	 * Get method for the smearing function type
	 * @returns smearing model type (atm: 0 = single gaussian, 1 = double gaussian)
	 */
	int getSmearingModelType() const;
	void setSmearingModelType(int smearing_type_);

	/**
	 * Get method for the smearing function name
	 * @returns smearing model name (i.e: "single gaussian" or "double gaussian")
	 */
	std::string getSmearingModelName() const;

	int getAcceptanceInterpolationType() const;
	void setAcceptanceInterpolationType(int acc_intpol_type_);

	/**
	 * @returns the code for which parameter is free in the fit
	 */
	unsigned int getFreeParametersCode() const;
	/**
	 * This function will set the #free_parameters_code field. This code decides
	 * which parameters will be set free in the fit later on.
	 * @param free_parameters_code_ is new value of #free_parameters_code
	 */
	void setFreeParametersCode(unsigned int free_parameters_code_);

	int getDpmElasticModelParts() const;
	void setDpmElasticModelParts(int dpm_elastic_model_parts_);

	LumiFit::LmdDimensionRange getPrimaryDimensionFitRange() const;
	LumiFit::LmdDimensionRange getSecondaryDimensionFitRange() const;
	void setPrimaryDimensionFitRange(
			LumiFit::LmdDimensionRange primary_dimension_fit_range_);
	void setSecondaryDimensionFitRange(
			LumiFit::LmdDimensionRange secondary_dimension_fit_range_);

	bool isPrimaryDimensionFitRangeActive() const;
	bool isSecondaryDimensionFitRangeActive() const;
	void setPrimaryDimensionFitRangeActive(
			bool primary_dimension_fit_range_active_);
	void setSecondaryDimensionFitRangeActive(
			bool secondary_dimension_fit_range_active_);
	TString getResolutionParametrizationFileUrl() const;
	void setResolutionParametrizationFileUrl(
			TString resolution_parametrization_file_url_);
	bool isResolutionParametrizationFileUrlActive() const;
	void setResolutionParametrizationFileUrlActive(
			bool resolution_parametrization_file_url_active_);

	/**
	 * Less then operator for NON binary options (so fit range etc).
	 * Will return true "this" fit options are "less" in value.
	 */
	bool lessThanNonBinaryOptions(const PndLmdLumiFitOptions & rhs) const;
	/**
	 * Less then operator. Will return true "this" fit options are "less" in value.
	 */
	bool operator <(const PndLmdLumiFitOptions & rhs) const;
	/**
	 * Greater then operator. Will return true "this" fit options are "greater" in value.
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
