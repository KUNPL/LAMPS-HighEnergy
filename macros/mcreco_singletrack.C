void mcreco_singletrack(TString name = "single_proton")
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
  run -> Add(new KBSingleHelixTask());

  run -> Init();
  run -> Run();
}
