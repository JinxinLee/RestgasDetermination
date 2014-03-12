/**
 * Example of use of Photos C++ interface.
 * e+, e- -> tau + tau - HEPEVT events are constructed.
 * Taus are subsequently decayed via Photos.
 *
 * @author Tomasz Przedzinski
 * @date 24 November 2011
 */

#include "Photos/Photos.h"
#include "Photos/PhotosHEPEVTParticle.h"
#include "Photos/PhotosHEPEVTEvent.h"
using namespace std;
using namespace Photospp;

int NumberOfEvents = 1000;
int EventsToCheck  = 1000;

void checkMomentumConservationInEvent(PhotosHEPEVTEvent *evt)
{
  int n = evt->getParticleCount();
  double px=0.0,py=0.0,pz=0.0,e=0.0;

  for(int i=0;i<n;i++)
  {
    PhotosHEPEVTParticle *p = evt->getParticle(i);
    if(p->getStatus() != 1) continue;

    px += p->getPx();
    py += p->getPy();
    pz += p->getPz();
    e  += p->getE();
  }

  cout.precision(6);
  cout.setf(ios_base::floatfield);
  cout<<endl<<"Vector Sum: "<<px<<" "<<py<<" "<<pz<<" "<<e<<endl;
}

/** Create a simple e+ + e- -> Z -> tau+ tau- HEPEVT event **/
PhotosHEPEVTEvent* make_simple_tau_event(){

  PhotosHEPEVTEvent * evt = new PhotosHEPEVTEvent();

  const double amell = 0.0005111;

  // Make PhotosParticles for boosting
  PhotosHEPEVTParticle *first_e      = new PhotosHEPEVTParticle(  11, 6,  1.7763568394002505e-15, -3.5565894425761324e-15,  4.5521681043409913e+01, 4.5521681043409934e+01,     amell,              -1, -1,  2,  2);
  PhotosHEPEVTParticle *second_e     = new PhotosHEPEVTParticle( -11, 6, -1.7763568394002505e-15,  3.5488352204797800e-15, -4.5584999071936601e+01, 4.5584999071936622e+01,     amell,              -1, -1,  2,  2);
  PhotosHEPEVTParticle *intermediate = new PhotosHEPEVTParticle(  23, 5,  0,                       0,                      -6.3318028526687442e-02, 9.1106680115346506e+01, 9.1106658112716090e+01,  0,  1,  3,  4);
  PhotosHEPEVTParticle *first_tau    = new PhotosHEPEVTParticle(  15, 2, -2.3191595992562256e+01, -2.6310500920665142e+01, -2.9046412466624929e+01, 4.5573504956498098e+01, 1.7769900000002097e+00,  2, -1,  5,  6);
  PhotosHEPEVTParticle *second_tau   = new PhotosHEPEVTParticle( -15, 2,  2.3191595992562256e+01,  2.6310500920665142e+01,  2.8983094438098242e+01, 4.5533175158848429e+01, 1.7769900000000818e+00,  2, -1,  7,  8);
  PhotosHEPEVTParticle *t1d1         = new PhotosHEPEVTParticle(  16, 1, -1.2566536214715378e+00, -1.7970251138317268e+00, -1.3801323581022720e+00, 2.5910119010468553e+00, 9.9872238934040070e-03,  3, -1, -1, -1);
  PhotosHEPEVTParticle *t1d2         = new PhotosHEPEVTParticle(-211, 1, -2.1935073012334062e+01, -2.4513624017269400e+01, -2.7666443730700312e+01, 4.2982749776866747e+01, 1.3956783711910248e-01,  3, -1, -1, -1);
  PhotosHEPEVTParticle *t2d1         = new PhotosHEPEVTParticle( -16, 1,  8.4364531743909055e+00,  8.3202830831667836e+00,  9.6202800273055971e+00, 1.5262723881157640e+01, 9.9829332903027534e-03,  4, -1, -1, -1);
  PhotosHEPEVTParticle *t2d2         = new PhotosHEPEVTParticle( 211, 1,  1.4755273459419701e+01,  1.7990366047940022e+01,  1.9362977676297948e+01, 3.0270707771933196e+01, 1.3956753909587860e-01,  4, -1, -1, -1);
      
  // Order matters!
  evt->addParticle(first_e     );
  evt->addParticle(second_e    );
  evt->addParticle(intermediate);
  evt->addParticle(first_tau   );
  evt->addParticle(second_tau  );
  evt->addParticle(t1d1        );
  evt->addParticle(t1d2        );
  evt->addParticle(t2d1        );
  evt->addParticle(t2d2        );

  return evt;
}

/** Example of using Photos to process event stored in HEPEVT event record */
int main(void){

  Photos::initialize();

	int photonAdded=0,twoAdded=0,moreAdded=0,evtCount=0;

  // Begin event loop. Generate event.
  for (int iEvent = 0; iEvent < NumberOfEvents; ++iEvent) {

    if(iEvent%10000==0) cout<<"Event: "<<iEvent<<"\t("<<iEvent*(100./NumberOfEvents)<<"%)"<<endl;

    // Create simple event
    PhotosHEPEVTEvent * event = make_simple_tau_event();

    int buf = -event->getParticleCount();

    //cout << "BEFORE:"<<endl;
    //event->print();

    if(iEvent<EventsToCheck)
    {
      cout<<"                                          "<<endl;
      cout<<"Momentum conservation chceck BEFORE/AFTER Photos"<<endl;
      checkMomentumConservationInEvent(event);
    }
    
    event->process();

    buf += event->getParticleCount();
		if     (buf==1) photonAdded++;
		else if(buf==2) twoAdded++;
		else if(buf>2)  moreAdded++;

    if(iEvent<EventsToCheck)
    {
      checkMomentumConservationInEvent(event);
    }
    
    //cout << "AFTER:"<<endl;
    //event->print();

    evtCount++;

    //clean up
    delete event;
  }

	// Print results
	cout.precision(3);
	cout.setf(ios::fixed);
	cout<<endl;
	cout<<"Summary of processing simple events:    e+ e- -> Z -> tau+ tau-"<<endl;
	cout<<evtCount   <<"\tevents processed"<<endl;
	cout<<photonAdded<<"\ttimes one photon added to the event           \t("<<(photonAdded*100./evtCount)<<"%)"<<endl;
	cout<<twoAdded   <<"\ttimes two photons added to the event          \t("<<(twoAdded*100./evtCount)<<"%)"<<endl;
	cout<<moreAdded  <<"\ttimes more than two photons added to the event\t("<<(moreAdded*100./evtCount)<<"%)"<<endl<<endl;
	cout<<"(Contrary to results from MC-Tester, these values are technical and infrared unstable)"<<endl<<endl;
}

