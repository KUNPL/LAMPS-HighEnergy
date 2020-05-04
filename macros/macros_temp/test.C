void test()
{
  auto run = KBRun::GetRun();
  run -> SetInputFile("/Users/ejungwoo/kebii/data/lamps2.digi.root");
  run -> AddDetector(new LHTpc());
  run -> Init();
  run -> GetEvent(0);

  run -> Print();

  auto padArray = run -> GetBranchA("Pad");

  Int_t numPads = padArray -> GetEntries();
  cout << numPads << endl;
  for (auto iPad = 0; iPad < 24; ++iPad)
  //for (auto iPad = 0; iPad < numPads; ++iPad)
  {
    auto pad = (KBPad *) padArray -> At(iPad);
    new TCanvas();
    pad -> Draw("pomc");
    pad -> Print();
  }
}
