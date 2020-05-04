void geo()
{
  auto tpc = new LHTpc();
  tpc -> AddPar("tpc_rec.par");
  tpc -> Init();

  auto padplane = tpc -> GetPadPlane();
  padplane -> Clear();

  auto hist_padplane = padplane -> GetHist("0");
  for (auto iPad = 0; iPad < padplane -> GetNumPads(); ++iPad) {
    auto pad2 = padplane -> GetPad(iPad);
    auto bin = hist_padplane -> FindBin(pad2 -> GetI(), pad2 -> GetJ());
  }
  auto cvs = padplane -> GetCanvas();
  hist_padplane -> SetName("PadPlane");
  hist_padplane -> SetTitle("PadPlane");

  gStyle -> SetPalette(kBird);
  hist_padplane -> Draw("");
  padplane -> DrawFrame();

  padplane -> PadPositionChecker();
  padplane -> PadNeighborChecker();
}

