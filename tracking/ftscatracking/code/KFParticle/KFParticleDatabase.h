#ifndef KFParticleDatabase_H
#define KFParticleDatabase_H

#include <map>

class KFParticleDatabase
{
 public:
  KFParticleDatabase();

  ~KFParticleDatabase() {};

  float GetMass(int pdg)
  {
    std::map<int, float>::iterator it;
    it=fMass.find(pdg);
    if(it != fMass.end()) return it->second;
    else return 0.13957;
  }

  static KFParticleDatabase* Instance() { return fDatabase; }

 private:
  std::map<int, float> fMass;

  static KFParticleDatabase* fDatabase;
};

#endif