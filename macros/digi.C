void digi()
{
  auto run = KBRun::GetRun();
  run -> SetIOFile("lamps.mc", "lamps.digi");
  run -> AddDetector(new LHTpc());
  run -> AddParameterFile("lamps.par");

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask();

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  auto track = new LHTrackFindingTask();

  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
}
