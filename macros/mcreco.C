void mcreco()
{
  auto run = KBRun::GetRun();
  run -> SetIOFile("lamps2.mc", "lamps2.mcreco");
  run -> AddDetector(new LHTpc());
  run -> AddParameterFile("lamps2.par");

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask();

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(drift);
  run -> Add(electronics);
  run -> Add(psa);
  run -> Add(new LHTrackFindingTask());
  run -> Add(new LHVertexFindingTask());

  run -> Init();
  run -> Run();
}
