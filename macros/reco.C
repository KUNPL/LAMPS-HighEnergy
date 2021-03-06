void reco(TString name = "lamps_high")
{
  auto run = KBRun::GetRun();
  run -> SetIOFile(name+".digi", name+".reco");
  run -> AddDetector(new LHTpc());

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(psa);
  run -> Add(new LHTrackFindingTask());
  run -> Add(new LHVertexFindingTask());

  run -> Init();
  run -> RunSingle(0);
}
