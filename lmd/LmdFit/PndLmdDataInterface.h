/*
 * PndLmdDataInterface.h
 *
 *  Created on: Apr 17, 2013
 *      Author: steve
 */

#ifndef PNDLMDDATAINTERFACE_H_
#define PNDLMDDATAINTERFACE_H_

#include <set>
#include <vector>

#include "TString.h"
#include "TObject.h"

using std::set;

class PndLmdDataInterface: public TObject {
private:
  /** The number of events filled to the histogram. Events may be outside of specified range (under/overflow)*/
  int num_events;
  /** The lab momentum of the antiproton beam*/
  double p_lab;

  /** Name of the object. This will be automatically generated. */
  TString name;

  /**
   * List of filepaths that contain the data that was read in. Only used for
   * bookkeeping/logging so later on it can be identified which data was used
   * for this data object.
   */
  set<TString> filepath_list;

public:
  struct lmd_values {
    double t;
    double theta;
    double phi;

    bool reconstructed;
  };

  PndLmdDataInterface();
  PndLmdDataInterface(double plab);
  virtual ~PndLmdDataInterface();

  // getter methods
  int getNumEvents() const;
  double getLabMomentum() const;
  TString& getName();

  void setNumEvents(int num_events_);
  void setLabMomentum(double p_lab_);

  int addFileToList(TString filepath);

  // histogram filling methods
  virtual void fillHistograms(
      std::vector<
          std::pair<PndLmdDataInterface::lmd_values,
              PndLmdDataInterface::lmd_values> > &event_data) =0;
  ClassDef(PndLmdDataInterface,1)
};

#endif /* PNDLMDDATAINTERFACE_H_ */
