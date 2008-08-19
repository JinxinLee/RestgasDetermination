#ifndef DIM_HH
#define DIM_HH
#include <string>
using namespace std;
class dim{
      private:
        double fMIN;
        double fMAX;
        int    nSEG;
        unsigned int fDNO;
        string sNAME; 
      public:
        dim();
        ~dim();
        dim(int seg,double min,double max,string sNAME,unsigned int dno);
//        void setMIN(double min) { fMIN = min ; }
//        void setMAX(double max) { fMAX = max ; }
//        void setSEG(double seg) { nSEG = seg ; }
//        void setNAME(string name) { sNAME = name ; }
        double getMIN()const    {return fMIN;}
        double getMAX() {return fMAX;}
        int getSEG() {return nSEG;}
        string getNAME() {return sNAME;}
        bool operator==(const dim& d) const {return fDNO == d.fDNO; }
        bool operator<(const dim& d) const {return fDNO < d.fDNO; }
};

#endif
