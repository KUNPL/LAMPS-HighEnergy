//void mcreco(TString name = "tpc_rec")
//void mcreco(TString name = "single_IQMD")
void mcreco(TString name = "lamps_high")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".mcreco");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask(true);

  run -> Add(drift);
  run -> Add(electronics);

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(psa);
  run -> Add(new LHTrackFindingTask());
  run -> Add(new LHVertexFindingTask());

  run -> Init();
  //run -> Run();
  run -> RunSingle(0);
}
