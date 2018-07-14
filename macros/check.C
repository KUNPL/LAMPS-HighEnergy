void check()
{
  auto momMC = TVector3();
  auto file = new TFile("$KEBIPATH/data/lamps2.mc.root");
  auto tree = (TTree *) file -> Get("data");

  auto histAngleMC = new TH1D("histAngleMC", "MC Angle distribution;angle;Counts", 100, 0, 4);
  auto histAngleReco = new TH1D("histAngleReco", "Reco Angle distribution;angle;Counts", 100, 0, 4);
  auto histAngleRecoReverse = new TH1D("histAngleRecoReverse", "Reco Angle distribution(Reverse);angle;Counts", 100, 0, 4);

  TClonesArray *stepArray = nullptr;
  tree -> SetBranchAddress("MCTrack",&stepArray);

  double angle;

  auto numEvent = tree -> GetEntries();
  for (auto iEvent = 0; iEvent < numEvent; iEvent++) 
  {
    tree->GetEntry(iEvent);
    auto numSteps = stepArray->GetEntries();
    for (auto iStep = 0; iStep < numSteps; iStep++)
    {
      auto step = (KBMCTrack *) stepArray -> At(iStep);
      if(step->GetPDG() == 2112) continue;
      momMC = step->GetMomentum();
      if(momMC.Z() < 0 ) angle = TMath::Pi() + TMath::ATan(sqrt(momMC.X()*momMC.X() + momMC.Y()*momMC.Y())/momMC.Z());
      if(momMC.Z() > 0 ) angle = TMath::ATan(sqrt(momMC.X()*momMC.X() + momMC.Y()*momMC.Y())/momMC.Z());
      histAngleMC->Fill(angle);
    } 
  }



  auto momMCreco = TVector3();
  auto fileRECO = new TFile("$KEBIPATH/data/lamps2.mcreco.root");
  auto treeRECO = (TTree *) fileRECO -> Get("data");

  TClonesArray *stepArrayRECO = nullptr;
  treeRECO -> SetBranchAddress("Tracklet",&stepArrayRECO);
  
  auto numEventRECO = tree -> GetEntries();
  for (auto iEvent = 0; iEvent < numEventRECO; iEvent++) 
  {
    treeRECO->GetEntry(iEvent);
    auto numSteps = stepArrayRECO->GetEntries();
    for (auto iStep = 0; iStep < numSteps; iStep++)
    {
      auto stepRECO = (KBHelixTrack *) stepArrayRECO -> At(iStep);
      if(stepRECO->GetParentID()!=0) continue;
      momMCreco = stepRECO->Momentum();
      if(momMCreco.Z() < 0 ) angle = TMath::Pi() + TMath::ATan(sqrt(momMCreco.X()*momMCreco.X() + momMCreco.Y()*momMCreco.Y())/momMCreco.Z());
      if(momMCreco.Z() > 0 ) angle = TMath::ATan(sqrt(momMCreco.X()*momMCreco.X() + momMCreco.Y()*momMCreco.Y())/momMCreco.Z());
      histAngleReco->Fill(angle);
      histAngleRecoReverse->Fill(TMath::Pi()-angle);
    }
  }

  auto c = new TCanvas("c", "", 1400,700);
  c->Divide(2,1);
  c->cd(1);
  histAngleMC->Draw();
  c->cd(2);
  histAngleReco->Draw();
  c->SaveAs("ReDistAngle.png");
  
  auto c1 = new TCanvas("c1", "", 1400,700);
  c1->Divide(2,1);
  c1->cd(1);
  histAngleMC->Draw();
  c1->cd(2);
  histAngleRecoReverse->Draw();

  c1->SaveAs("ReDistAngle_reverse.png");

  cout << (double)histAngleMC->GetMaximumBin()*4/100*(180/TMath::Pi()) << endl;
  cout << (double)histAngleRecoReverse->GetMaximumBin()*4/100*(180/TMath::Pi()) << endl;
}
