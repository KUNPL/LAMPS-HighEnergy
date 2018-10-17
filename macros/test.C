void test(TString name = "lamps")
{
  auto run = KBRun::GetRun();
  run -> AddParameterFile("lamps.par");
  run -> SetIOFile(name+".mc", name+".test");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(false);

  auto electronics = new LHElectronicsTask();

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(drift);
  run -> Add(electronics);
  run -> Add(psa);
  run -> Add(new LHHitClustering());
  //run -> Add(new LHTrackFindingTask());

  //run -> Add(new LHVertexFindingTask());
  //run -> Add(new LHMCRecoMatchingTask());

  run -> Init();
  run -> SetAutoTermination(true);
  run -> Run();

}
