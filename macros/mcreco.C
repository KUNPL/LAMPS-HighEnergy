void mcreco(TString name = "lamps2_R100")
{
  auto run = KBRun::GetRun();
  run -> AddParameterFile("lamps2.par");
  run -> SetIOFile(name+".mc", name+".mcreco");
  run -> AddDetector(new LHTpc());

  auto drift = new LHDriftElectronTask();
  drift -> SetPadPersistency(false);

  auto electronics = new LHElectronicsTask();

  auto psa = new KBPSATask();
  psa -> SetPSA(new KBPSAFastFit());

  run -> Add(drift);
  run -> Add(electronics);
  run -> Add(psa);
  run -> Add(new LHTrackFindingTask());
  run -> Add(new LHVertexFindingTask());
  run -> Add(new LHMCRecoMatchingTask());

  run -> Init();
  run -> SetAutoTermination(false);
  run -> Run();

int num;
ifstream ifile("check.txt");
ifile >> num;
ifile.close();
ofstream ofile("check.txt",ofstream::trunc);
ofile << num-1 << endl;
ofile.close();

}
