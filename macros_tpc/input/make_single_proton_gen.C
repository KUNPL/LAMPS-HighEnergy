void make_single_proton_gen()
{
  TString nameFile = "single_proton_500.gen";
  Int_t numEvents = 1000;
  //Int_t numEvents = 10;
  ofstream file(nameFile);
  file << "p" << endl;;
  file << numEvents << endl;;
  for (auto iEvent=0; iEvent<numEvents; ++iEvent)
  {
    file << iEvent << " 1 0 0 0" << endl;
    //auto mag = gRandom -> Uniform(50,3000);
    auto mag = 500;
    auto phi = gRandom -> Uniform(0,2*TMath::Pi());
    auto theta = gRandom -> Uniform(0,TMath::Pi());
    TVector3 momentum(0,0,1);
    momentum.SetMag(mag);
    momentum.SetTheta(theta);
    momentum.SetPhi(phi);
    //cout  << "===" << endl;
    //cout  << mag << " " << theta*TMath::RadToDeg() << " " << phi*TMath::RadToDeg() << endl;
    //cout  << 2212 << " " << momentum.x() << " " << momentum.y() << " " << momentum.z() << endl;
    file << 2212 << " " << momentum.x() << " " << momentum.y() << " " << momentum.z() << endl;
  }
}
