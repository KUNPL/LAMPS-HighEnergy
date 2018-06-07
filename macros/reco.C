void reco()
{
  auto run = new KBRun();
  run -> SetIOFile("lamps.digi", "lamps.reco");
  run -> AddDetector(new LHTpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(psa);
  run -> Add(new LHTrackFindingTask());
  run -> Add(new LHVertexFindingTask());

  run -> Init();
  run -> Run();
  //run -> RunInRange(3,6);
}
