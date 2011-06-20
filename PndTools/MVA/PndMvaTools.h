/* ********************************************
 * MVA Tools and function definitions.        *
 * Author: M.Babai@rug.nl                     *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */

#ifndef PND_MVA_TOOLS_H
#define PND_MVA_TOOLS_H

// C++
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

/**
 * Structure used to hold the classifier output (label and distance or
 * prob.) for each example in the test set together with the original
 * class name.
 */
struct ClassifierOutPuts
{
  //public:
  // Constructors
  ClassifierOutPuts()
    : realLabel ("ALABEL"),
      givenLabel("NOLABEL")
  {};
  /**
   *@param Rlabel True label.
   *@param Glabel Given label.
   *@param clsOut Classifier output for each label.
   */
  ClassifierOutPuts(std::string const& Rlabel, std::string const& Glabel,
		    std::map<std::string, float> const& clsOut)
    : realLabel  (Rlabel),
      givenLabel (Glabel),
      clsOuts    (clsOut)
  {};

  // Destructor
  virtual ~ClassifierOutPuts()
  {};

  // Copy Const
  ClassifierOutPuts(ClassifierOutPuts const& ot)
    : realLabel  (ot.realLabel),
      givenLabel (ot.givenLabel),
      clsOuts    (ot.clsOuts)
  {};

  // Operators.  
  ClassifierOutPuts& operator=(ClassifierOutPuts const& ot)
  {
    this->realLabel  = ot.realLabel;
    this->givenLabel = ot.givenLabel;
    this->clsOuts    = ot.clsOuts;
    return (*this);
  };

  // Get classifier output for all available labels.
  std::map<std::string, float> const& getClsOut() const
  {
    return clsOuts;
  };

  // Variables
  std::string realLabel;// Original label
  std::string givenLabel;// Given label
  std::map<std::string, float> clsOuts; // Classifier outputs per label
  
  //protected:
private:
  //==
  inline bool operator> (ClassifierOutPuts const& ot) const;
  inline bool operator< (ClassifierOutPuts const& ot) const;
  inline bool operator==(ClassifierOutPuts const& ot) const;
};

/**
 * Structure to hold the ROC points.
 */
struct ROCPoints
{
  // Constructors
  ROCPoints()
    : FP_rate(0.0),
      TP_rate(0.0),
      TN_rate(0.0),
      FN_rate(0.0),
      fp(0),
      tp(0),
      fn(0),
      tn(0),
      thr(0.0)
  {};
  /**
   *@param fpr False positief rate.
   *@param tpr True positief rate.
   *@param tnr True negatief rate.
   *@param fnr False negatief rate.
   *@param nfp False positief count.
   *@param ntp True positief count.
   *@param nfn False negatief.
   *@param ntn True negatief.
   *@param curThr Current treshold value.
   */
  ROCPoints(float  const fpr, float  const tpr,
	    float  const tnr, float  const fnr,
	    size_t const nfp, size_t const ntp,
	    size_t const nfn, size_t const ntn,
	    float const curThr)
    : FP_rate(fpr),
      TP_rate(tpr),
      TN_rate(tnr),
      FN_rate(fnr),
      fp(nfp),
      tp(ntp),
      fn(nfn),
      tn(ntn),
      thr(curThr)
  {};
  
  // Destructor
  virtual ~ROCPoints()
  {};
  
  // Copy Const
  ROCPoints(ROCPoints const& ot)
    : FP_rate(ot.FP_rate),
      TP_rate(ot.TP_rate),
      TN_rate(ot.TN_rate),
      FN_rate(ot.FN_rate),
      fp(ot.fp),
      tp(ot.tp),
      fn(ot.fn),
      tn(ot.tn),
      thr(ot.thr)
  {};
  
  // Operators.  
  ROCPoints& operator=(ROCPoints const& ot)
  {
    this->FP_rate = ot.FP_rate;
    this->TP_rate = ot.TP_rate;
    this->TN_rate = ot.TN_rate;
    this->FN_rate = ot.FN_rate;
    this->fp      = ot.fp;
    this->tp      = ot.tp;
    this->fn      = ot.fn;
    this->tn      = ot.tn;
    this->thr     = ot.thr;
    return (*this);
  };

  // Variables
  float FP_rate;// False positief rate
  float TP_rate;// True positief rate
  float TN_rate;// True negatief rate
  float FN_rate;// False negatief rate
  size_t fp;// False positief count
  size_t tp;// True positief count
  size_t fn;// False negatief
  size_t tn;// True negatief
  float  thr;// Treshold value

  //  protected:
  
private:
  bool operator==(ROCPoints const& ot) const;
  bool operator>( ROCPoints const& ot) const;
  bool operator<( ROCPoints const& ot) const;
};

//____________ Functions and modifiers.

/**
 * Print list of ROC objects.
 *@param RocList The list of ROC points
 */
void printRoc(std::vector< ROCPoints > const& RocList);
  
/**
 * Write the list of ROC objects in a file.
 *@param FileName The name of the file to write into.
 *@param RocList The list of the ROC poits to write.
 */
void WriteRocToFile( std::string const& FileName,
		     std::vector< ROCPoints > const& RocList);

/**
 * Print the list of classifier outputs.
 *@param OutPutList The list of outputs created by a classifier.
 */
void print(std::vector< ClassifierOutPuts > const& OutPutList);

// C style function declarations
#ifdef __cplusplus
extern "C"
{
#endif
  // Place your C code and/or C headers here.  
#ifdef __cplusplus
}
#endif

#endif// interface definition
