#include "KBParameterContainer.hh"
#include "LHDetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4UniformMagField.hh"
#include "G4GlobalMagFieldMessenger.hh"

LHDetectorConstruction::LHDetectorConstruction()
: G4VUserDetectorConstruction()
{
}

LHDetectorConstruction::~LHDetectorConstruction()
{
}

G4VPhysicalVolume *LHDetectorConstruction::Construct()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();

  auto par = runManager -> GetParameterContainer();

  G4double tpcInnerRadius = par -> GetParDouble("rMinTPC");
  G4double tpcOuterRadius = par -> GetParDouble("rMaxTPC");
  G4double tpcLength = par -> GetParDouble("tpcLength");
  G4double tpcZOffset = par -> GetParDouble("zOffset");

  G4NistManager *nist = G4NistManager::Instance();
  G4double STPTemperature = 273.15;
  G4double labTemperature = STPTemperature + 20.*kelvin;
  
  G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
  G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);

  G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);
 
  G4double densityMethane = 0.717e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matMethaneGas = new G4Material("matMethaneGas ", densityMethane, 2, kStateGas, labTemperature);
  matMethaneGas -> AddElement(elementH, 4);
  matMethaneGas -> AddElement(elementC, 1);

  G4double densityP10 = .8*densityArGas + .2*densityMethane;
  G4Material *matP10 = new G4Material("matP10", densityP10, 2, kStateGas, labTemperature);
  matP10 -> AddMaterial(matArGas, 0.8*densityArGas/densityP10);
  matP10 -> AddMaterial(matMethaneGas, 0.2*densityMethane/densityP10);

  G4Material *matAir = nist -> FindOrBuildMaterial("G4_AIR");


  G4Tubs *solidWorld = new G4Tubs("World", 0, tpcOuterRadius*1.1, tpcLength, 0., 360*deg);
  G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
  G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);


  G4Tubs *solidTPC = new G4Tubs("TPC", tpcInnerRadius, tpcOuterRadius, .5*tpcLength, 0., 360*deg);
  G4LogicalVolume *logicTPC = new G4LogicalVolume(solidTPC, matP10, "TPC");
  {
    G4VisAttributes * attTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
    attTPC -> SetForceWireframe(true);
    logicTPC -> SetVisAttributes(attTPC);
  }
  logicTPC -> SetUserLimits(new G4UserLimits(1.*mm));
  auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset), logicTPC, "TPC", logicWorld, false, 0, true);
  runManager -> SetSensitiveDetector(pvp);


  new G4GlobalMagFieldMessenger(G4ThreeVector(0., 0., 0.5*tesla));

  return physWorld;
}
