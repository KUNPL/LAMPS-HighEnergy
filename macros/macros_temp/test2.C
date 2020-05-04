void test2()
{
  auto file = new TFile("/Users/ejungwoo/kebii/data/lamps2.mcreco.root");
  auto tree = (TTree *) file -> Get("data");

  //new TCanvas(); tree -> Draw("Hit.fMCError>>hist(200,-0.5,0.5)");
  //new TCanvas(); tree -> Draw("Hit.fTb:Hit.fMCError>>hist2(200,-5,5,200,0,512)","","colz");
  //new TCanvas(); tree -> Draw("Hit.fTb:Hit.fMCError>>hist2(200,-5,5,200,0,512)","","colz");
  //new TCanvas(); tree -> Draw("Hit.fMCID:Hit.fMCError","","colz");
  //new TCanvas(); tree -> Draw("Tracklet.fMCError:Tracklet.fMCID>>h3(100,-1,99,100,-5,5)","","colz");
  //new TCanvas(); tree -> Draw("Tracklet.GetNumHitIDs():Tracklet.fMCID>>h3(200,0,200,101,-1,100)","","colz");
  //new TCanvas(); tree -> Draw("Tracklet.GetNumHitIDs()");
  new TCanvas(); tree -> Draw("Tracklet.fMCPurity:Tracklet.fMCID>>h1(20,0,20,100,-1,2)","","colz");
  new TCanvas(); tree -> Draw("Tracklet.fMCPurity:Tracklet.fMCID>>h2(20,20,40,100,-1,2)","","colz");
  new TCanvas(); tree -> Draw("Tracklet.fMCPurity:Tracklet.fMCID>>h3(20,40,60,100,-1,2)","","colz");
  new TCanvas(); tree -> Draw("Tracklet.fMCPurity:Tracklet.fMCID>>h4(20,60,80,100,-1,2)","","colz");
  new TCanvas(); tree -> Draw("Tracklet.fMCPurity:Tracklet.fMCID>>h5(20,80,100,100,-1,2)","","colz");
}
