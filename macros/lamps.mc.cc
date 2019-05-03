#include "globals.hh"
#include "KBCompiled.h"

#include "KBG4RunManager.hh"
#include "G4StepLimiterPhysics.hh"

#include "KBParameterContainer.hh"

#include "QGSP_BERT.hh"
#include "KBMCDataManager.hh"
#include "LHDetectorConstruction.hh"
#include "KBPrimaryGeneratorAction.hh"

#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

int main(int argc, char** argv)
{
  auto runManager = new KBG4RunManager();

  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());

  runManager -> SetUserInitialization(physicsList);
  runManager -> SetParameterContainer(argv[1]);
  runManager -> SetUserInitialization(new LHDetectorConstruction());
  runManager -> SetUserAction(new KBPrimaryGeneratorAction());
  runManager -> SetUserAction(new KBEventAction());
  runManager -> SetUserAction(new KBTrackingAction());
  runManager -> SetUserAction(new KBSteppingAction());
  runManager -> Initialize();
  runManager -> Run();

  delete runManager;

  return 0;
}
