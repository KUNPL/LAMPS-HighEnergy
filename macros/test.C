void test()
{
  auto par = new KBParameterContainer("lamps_high.par");
  //par -> Print();

  int numNA = par -> GetParInt("numNeutronWall");
  for (auto iwall = 0; iwall < numNA; ++iwall) {
    KBVector3::Axis naStackAxis = par -> GetParAxis(Form("naStackAxis%d",iwall));
    int naNumStack = par -> GetParInt(Form("naNumStack%d",iwall));
    auto nadX = par -> GetParDouble(Form("nadX%d",iwall));
    auto nadY = par -> GetParDouble(Form("nadY%d",iwall));
    auto nadZ = par -> GetParDouble(Form("nadZ%d",iwall));
    auto naXOffset = par -> GetParDouble(Form("naXOffset%d",iwall));
    auto naYOffset = par -> GetParDouble(Form("naYOffset%d",iwall));
    auto naZOffset = par -> GetParDouble(Form("naZOffset%d",iwall));

    KBGeoBoxStack stack(naXOffset,naYOffset,naZOffset,nadX,nadY,nadZ,naNumStack,naStackAxis,KBVector3::kZ);
    stack.Print();

    auto g = stack.DrawStackGraph();
    ejungwoo::cv1();
    ejungwoo::make(g) -> Draw("a");

    auto hist = stack.DrawStackHist(Form("wall%d_hist",iwall));
    auto poly = stack.DrawStackHistPoly(Form("wall%d_poly",iwall));

    for (auto i=0; i<100; i++) {
      auto x = gRandom->Uniform(stack.GetLongAxisMin(), stack.GetLongAxisMax());
      auto y = gRandom->Uniform(stack.GetStackAxisMin(), stack.GetStackAxisMax());
      hist -> Fill(x,y);
      poly -> Fill(x,y);
    }

    ejungwoo::cv2();
    ejungwoo::make(hist) -> Draw("col");
    ejungwoo::cv2();
    ejungwoo::make(poly) -> Draw("col");
  }
}
