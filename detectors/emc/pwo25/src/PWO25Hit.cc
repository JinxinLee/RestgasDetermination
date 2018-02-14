#include "PWO25Hit.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4TouchableHistory.hh"

G4Allocator<PWO25Hit> PWO25HitAllocator;

PWO25Hit::PWO25Hit(G4Step* aStep)
{
	step = aStep;
}

PWO25Hit::~PWO25Hit()
{}

PWO25Hit::PWO25Hit(const PWO25Hit &right)
{
	fDE = right.fDE;
	fModule = right.fModule;
}

const PWO25Hit& PWO25Hit::operator=(const PWO25Hit &right)
{
	fDE = right.fDE;
	fModule = right.fModule;
	return *this;
}

void PWO25Hit::Draw()
{
}

void PWO25Hit::Print()
{
	std::cout << "  Module ";
	std::cout.width(3);
	std::cout << fModule << ": ";
	std::cout.width(12);
	std::cout << fDE/MeV << " MeV" << std::endl;
}


