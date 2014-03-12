#include "PhotosHEPEVTEvent.h"
#include "Log.h"

namespace Photospp
{

PhotosHEPEVTEvent::~PhotosHEPEVTEvent()
{
  for(unsigned int i=0;i<particle_list.size();i++) delete particle_list[i];
}

PhotosHEPEVTEvent::PhotosHEPEVTEvent() {}

void PhotosHEPEVTEvent::addParticle(PhotosHEPEVTParticle *p)
{
  p->setEvent(this);

  p->setBarcode(particle_list.size());
  particle_list.push_back(p);
}

PhotosHEPEVTParticle *PhotosHEPEVTEvent::getParticle(int i)
{
  if( i<0 || i>=(int)particle_list.size() ) return NULL;
  return particle_list[i];
}

void PhotosHEPEVTEvent::setParticle(int i, PhotosHEPEVTParticle *p)
{
  if( i<0 || i>=(int)particle_list.size() ) return;
  particle_list[i] = p;
}

int PhotosHEPEVTEvent::getParticleCount()
{
  return particle_list.size();
}

std::vector<PhotosParticle*> PhotosHEPEVTEvent::getParticleList()
{
  std::vector<PhotosParticle*> ret;

  for(unsigned int i=0;i<particle_list.size();i++) ret.push_back( (PhotosParticle*)particle_list[i] );

  return ret;
}

void PhotosHEPEVTEvent::print()
{
  Log::Info()<<"PhotosHEPEVTEvent"<<endl<<"-----------------"<<endl;
  for(unsigned int i=0;i<particle_list.size();i++) particle_list[i]->print();
}

void PhotosHEPEVTEvent::clear()
{
  for(unsigned int i=0;i<particle_list.size();i++) delete particle_list[i];
  particle_list.clear();
}

#ifdef USE_HEPEVT_INTERFACE

void PhotosHEPEVTEvent::read_event_from_HEPEVT(PhotosHEPEVTEvent *evt)
{
  if(evt==NULL) return;
  
  for(int i=0; i<hepevt_.nhep; i++)
  {
    PhotosHEPEVTParticle *p = new PhotosHEPEVTParticle
    (
      hepevt_.idhep [i],
      hepevt_.isthep[i],
      hepevt_.phep  [i][0],
      hepevt_.phep  [i][1],
      hepevt_.phep  [i][2],
      hepevt_.phep  [i][3],
      hepevt_.phep  [i][4],
      hepevt_.jmohep[i][0]-1,
      hepevt_.jmohep[i][1]-1,
      hepevt_.jdahep[i][0]-1,
      hepevt_.jdahep[i][1]-1
    );
    evt->addParticle(p);
  }
}

void PhotosHEPEVTEvent::write_event_to_HEPEVT(PhotosHEPEVTEvent *evt)
{
  if(evt==NULL) return;
  
  hepevt_.nhep = evt->getParticleCount();
  
  for(int i=0; i<hepevt_.nhep; i++)
  {
    PhotosHEPEVTParticle *p = evt->getParticle(i);
    
    hepevt_.idhep [i]   =p->getPdgID();
    hepevt_.isthep[i]   =p->getStatus();
    hepevt_.phep  [i][0]=p->getPx();
    hepevt_.phep  [i][1]=p->getPy();
    hepevt_.phep  [i][2]=p->getPz();
    hepevt_.phep  [i][3]=p->getE();
    hepevt_.phep  [i][4]=p->getMass();
    hepevt_.jmohep[i][0]=p->getFirstMotherIndex()  +1;
    hepevt_.jmohep[i][1]=p->getSecondMotherIndex() +1;
    hepevt_.jdahep[i][0]=p->getDaughterRangeStart()+1;
    hepevt_.jdahep[i][1]=p->getDaughterRangeEnd()  +1;
    hepevt_.vhep  [i][0]=0.0;
    hepevt_.vhep  [i][1]=0.0;
    hepevt_.vhep  [i][2]=0.0;
    hepevt_.vhep  [i][3]=0.0;
  }
}

#endif

} // namespace Photospp
