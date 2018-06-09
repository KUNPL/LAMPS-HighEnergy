#include "LHTpc.hh"
#include "LHPadPlane.hh"
#include "LHPadPlaneRPad.hh"

LHTpc::LHTpc()
:KBTpc("LAMPSTPC","LAMPS TPC")
{
}

bool LHTpc::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("LAMPS TPC", "LAMPS TPC Geometry");
  }

  auto rMinTPC = fPar -> GetParDouble("rMinTPC");
  auto rMaxTPC = fPar -> GetParDouble("rMaxTPC");
  auto   dzTPC = fPar -> GetParDouble("dzTPC");
  auto zOffset = fPar -> GetParDouble("zOffset");

  TGeoMedium *p10 = new TGeoMedium("p10", 1, new TGeoMaterial("p10"));

  TGeoVolume *top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  TGeoVolume *tpc = new TGeoVolumeAssembly("TPC");
  TGeoTranslation *offTPC = new TGeoTranslation("TPC offset",0,0,zOffset);

  TGeoVolume *gas = fGeoManager -> MakeTube("gas",p10,rMinTPC,rMaxTPC,dzTPC/2);
  gas -> SetVisibility(true);
  gas -> SetLineColor(kBlue-10);
  gas -> SetTransparency(90);

  top -> AddNode(tpc, 1, offTPC);
  tpc -> AddNode(gas, 1);

  fGeoManager -> CloseGeometry();

  return true;
}

bool LHTpc::BuildDetectorPlane()
{
  KBPadPlane *padplane = nullptr;
  if (fPar->GetParString("PadPlaneType") == "LHPadPlaneRPad")
    padplane = new LHPadPlaneRPad();
  else
    padplane = new LHPadPlane();

  padplane -> SetParameterContainer(fPar);
  padplane -> SetPlaneID(0);
  padplane -> SetPlaneK(fPlaneK[0]);
  padplane -> SetAxis(KBVector3::kX, KBVector3::kY);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
