Int_t fEventID = 0;

void nx(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID);
  cout << "Event " << fEventID << endl;
}

//void eve(TString name = "/Users/ejungwoo/kebii/data/lamps_high.mcreco.root")
//void eve(TString name = "/Users/ejungwoo/kebii/data/lamps_high.mc.root")
void eve(TString name = "last")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> SetTag("eve");
  //run -> SelectEveBranches("Tracklet:Hit");
  run -> AddDetector(new LHTpc());
  //run -> AddDetector(new LHNeutronScintArray());
  run -> Init();
  //run -> AddPar("kbeve.par");
  run -> SetGeoTransparency(80);
  run -> RunEve(fEventID);
}
