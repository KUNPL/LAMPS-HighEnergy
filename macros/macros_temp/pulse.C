void pulse()
{
  KBPulseGenerator *pulseGen = new KBPulseGenerator();
  pulseGen -> Print();
  auto f1 = pulseGen -> GetPulseFunction();
  f1 -> SetParameters(1000,100);
  f1 -> SetNpx(1000);

  auto hist = new TH1D("hist","",512,0,512);
  for (auto i = 0; i < 512; ++i)
    hist -> SetBinContent(i+1,f1 -> Eval(i+0.5));
  hist -> Draw();
  f1 -> SetParameters(800,110);
  hist -> Fit(f1);
  f1 -> Draw("same");
}
