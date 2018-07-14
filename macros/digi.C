void digi(TString name = "lamps2")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".mc", name+".digi");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(true);

  auto electronics = new LHElectronicsTask();

  run -> Add(drift);
  run -> Add(electronics);

  run -> Init();
  run -> Run();
}
