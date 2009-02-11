/**

  @class PndGeoDsk

  @brief Interface to geometry file

  This is the interface to the geometry file.

  @author Peter Koch
  @date   2008-03-05
  @since  2007-10-28

**/


#ifndef PNDGEODSK_H
#define PNDGEODSK_H

#include "FairGeoSet.h"

class PndGeoDsk : public FairGeoSet
{

protected:
  char fEleName[5];  ///< substring for elementes in module
  char fModName[5];  ///< name of module


public:
  /// standard constructor
  PndGeoDsk();
  /// standard destructor
  ~PndGeoDsk() {}

  /// @brief return element name
  /// @param number the number of the element
  /// @return its name
  const char *getEleName(Int_t number);

  /// @brief return module name
  /// @param number the number of the module
  /// @return its name
  const char *getModuleName(Int_t number);

/// testing:
  void addRefNodes();

ClassDef(PndGeoDsk,0);

};

#endif // PNDGEODDRC_H
