/* ********************************************
 * MVA Tools and function definitions.        *
 * Author: M.Babai@rug.nl                     *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */
#include "PndMvaTools.h"

/**
 * Print list of ROC objects.
 *@param RocList The list of ROC points
 */
void printRoc(std::vector< ROCPoints > const& rc)
{
  for(size_t i = 0; i < rc.size(); ++i)
  {
    std::cout << "rc ["<< i<< "] { FP_rate = " << rc[i].FP_rate
	      << " TP_rate = " << rc[i].TP_rate
	      << " TN_rate = " << rc[i].TN_rate
	      << " FN_rate = " << rc[i].FN_rate
	      << " fp = " << rc[i].fp
	      << " tp = " << rc[i].tp
	      << " thr = " << rc[i].thr
	      << " }\n";
  }
}

/**
 * Write the list of ROC objects in a file.
 *@param FileName The name of the file to write into.
 *@param RocList The list of the ROC poits to write.
 */
void WriteRocToFile( std::string const& fName,
		     std::vector< ROCPoints > const& rc)
{
  std::cout << "<-I-> Write ROC values to file " << fName
	    << '\n';
  std::ofstream OutPut;
  
  OutPut.open (fName.c_str());
  OutPut << "# ROC graph points\n"
	 << "# <index>\t <FP_rate>\t <TP_rate>\t <TN_rate>\t <FN_rate>\t"
	 << " <fp>\t <tp>\t <fn>\t <tn>\t <threshold>\n";
  for(size_t i = 0; i < rc.size(); ++i)
  {
    OutPut << "   " << i << "\t "
	   << rc[i].FP_rate << "\t "
	   << rc[i].TP_rate << "\t "
	   << rc[i].TN_rate << "\t "
	   << rc[i].FN_rate << "\t "
	   << rc[i].fp << "\t "
	   << rc[i].tp << "\t "
	   << rc[i].fn << "\t "
	   << rc[i].tn << "\t "
	   << rc[i].thr
	   << '\n';
  }
  OutPut.close();
}

/**
 * Print the list of classifier outputs.
 *@param OutPutList The list of outputs created by a classifier.
 */
void print(std::vector <ClassifierOutPuts> const& el)
{
  std::map<std::string, float>::const_iterator it;
  for(size_t i = 0; i < el.size(); ++i)
  {
    std::cout << "O_Label = "  << el[i].realLabel
	      << ", G_Label = " << el[i].givenLabel
	      << '\n';
    std::map<std::string, float> const& out = (el[i]).getClsOut();
    for(it = out.begin(); it != out.end(); ++it)
    {
      std::cout << it->first << " " << it->second << " ";
    }
  }
  std::cout << '\n';
}
