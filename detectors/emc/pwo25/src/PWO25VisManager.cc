#include "PWO25VisManager.hh"

#include "G4VRML1File.hh"
#include "G4OpenGLImmediateX.hh"

PWO25VisManager::PWO25VisManager() {}

void PWO25VisManager::RegisterGraphicsSystems () {

RegisterGraphicsSystem (new G4VRML1File);
RegisterGraphicsSystem (new G4OpenGLImmediateX);
}
