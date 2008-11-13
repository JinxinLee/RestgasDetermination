#ifndef DIM_H
#define DIM_H
#include <string>
using namespace std;
class dim{
      private:
        double fMIN;
        double fMAX;
        int    nSEG;
        int fDNO;
        string sNAME;
        void copy(dim const &other);
        void destroy(); 
      public:
        dim();
        ~dim();
        dim(string sNAME, int dno,double min,double max,int seg);
        dim(dim const &other);
//        void setMIN(double min) { fMIN = min ; }
//        void setMAX(double max) { fMAX = max ; }
//        void setSEG(double seg) { nSEG = seg ; }
//        void setNAME(string name) { sNAME = name ; }
        double getMIN()const    {return fMIN;}
        double getMAX() const {return fMAX;}
        int getSEG()  const {return nSEG;}
        string getNAME() const {return sNAME;}
        int getDNO() const {return fDNO;}
        inline bool operator==(const dim& d) const; // {return fDNO == d.fDNO; }
        inline bool operator<(const dim& d) const;  //{return fDNO < d.fDNO; }
        dim &operator=(dim const &other) ;
        void print();
};

#endif
