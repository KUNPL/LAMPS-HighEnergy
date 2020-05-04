void draw_helix()
{
  auto run = KBRun::GetRun();
  run -> SetInputFile("single_IQMD.mcreco.root");
  run -> Init();
  auto trackArray = run -> GetBranchA("Tracklet");
  auto hitArray = run -> GetBranchA("Hit");

  auto event = 7;
  {
    run -> GetEvent(event);
    if (trackArray -> GetEntries()==0)
      return;

    auto track = (KBHelixTrack *) trackArray -> At(0);
    auto hitIDs = track -> GetHitIDArray();

    auto helix = new KBHelixTrack();
    for (auto id : *hitIDs) {
      cout << id << endl;
      auto hit = (KBHit *) hitArray -> At(id);
      helix -> AddHit(hit);
    }
    helix -> Fit();
    helix -> GetHitList() -> DrawFitCircle();
    helix -> Print("a");
  }
}
