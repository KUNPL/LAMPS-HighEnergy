#include "LHVertexFindingTask.hh"
#include "KBHelixTrack.hh"

#include "KBRun.hh"

#include <iostream>
using namespace std;

ClassImp(LHVertexFindingTask)

LHVertexFindingTask::LHVertexFindingTask()
:KBTask("LHVertexFindingTask","")
{
}

bool LHVertexFindingTask::Init()
{
  fTrackFitter = new KBHelixTrackFitter();

  KBRun *run = KBRun::GetRun();

  fTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fVertexArray = new TClonesArray("KBVertex");
  run -> RegisterBranch("Vertex", fVertexArray, fPersistency);

  TString  axis = run -> GetParameterContainer() -> GetParString("tpcBFieldAxis");
       if (axis == "x") fReferenceAxis = KBVector3::kX;
  else if (axis == "y") fReferenceAxis = KBVector3::kY;
  else if (axis == "z") fReferenceAxis = KBVector3::kZ;

  return true;
}

void LHVertexFindingTask::Exec(Option_t*)
{
  fVertexArray -> Delete();

  if (fTrackArray -> GetEntriesFast() < 2) {
    kb_warning << "Less than 2 tracks are given. Unable to find vertex." << endl;
    return;
  }

  KBVertex *vertex = new ((*fVertexArray)[0]) KBVertex();

  const Int_t numSamples = 10;
  Double_t sLeast = 1.e8;
  Double_t kAtSLeast = 0;
  Double_t sTest = 0;
  Int_t nIterations = 5;

  Double_t kArray[10];
  Double_t dk = (fK2-fK1)/(numSamples-1);
  for (Int_t iSample = 0; iSample < numSamples; iSample++)
    kArray[iSample] = dk*iSample + fK1;

  for (Int_t iSample = 0; iSample < numSamples; ++iSample) {
    Double_t kTest = kArray[iSample];
    KBVector3 testPosition(fReferenceAxis, 0, 0, kTest);
    sTest = TestVertexAtK(vertex, testPosition);

    if (sTest < sLeast) {
      sLeast = sTest;
      kAtSLeast = kTest;
    }
  }
  --nIterations;

  for (Int_t iSample = 0; iSample <= numSamples; ++iSample) {
    Double_t kTest = kArray[iSample];
    KBVector3 testPosition(fReferenceAxis, 0, 0, kTest);
    sTest = TestVertexAtK(vertex, testPosition);
    if (sTest < sLeast) {
      sLeast = sTest;
      kAtSLeast = kTest;
    }
  }

  while (nIterations > 0) {
    fK1 = kAtSLeast - dk;
    fK2 = kAtSLeast + dk;
    dk = (fK2-fK1)/(numSamples-1);
    for (Int_t iSample = 0; iSample < numSamples; iSample++)
      kArray[iSample] = dk*iSample + fK1;

    for (Int_t iSample = 0; iSample < numSamples; ++iSample) {
      Double_t kTest = kArray[iSample];
      KBVector3 testPosition(fReferenceAxis, 0, 0, kTest);
      sTest = TestVertexAtK(vertex, testPosition);
      if (sTest < sLeast) {
        sLeast = sTest;
        kAtSLeast = kTest;
      }
    }

    nIterations--;
  }

  KBVector3 testPosition(fReferenceAxis, 0, 0, kAtSLeast);
  sTest = TestVertexAtK(vertex, testPosition, true);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
    track -> DetermineParticleCharge(vertex -> GetPosition());
    fTrackFitter -> Fit(track);
  }

  auto pos = vertex -> GetPosition();

  kb_info << "Found vertex at " << Form("(%.1f, %.1f, %.1f)",pos.X(),pos.Y(),pos.Z()) << " with " << vertex -> GetNumTracks() << " tracks" << endl;

  return;
}

Double_t LHVertexFindingTask::TestVertexAtK(KBVertex *vertex, KBVector3 testPosition, bool last)
{
  Double_t sTest = 0;
  Int_t numUsedTracks = 0;

  KBVector3 position = testPosition;

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);

    TVector3 xyzOnHelix;
    Double_t alpha;
    track -> ExtrapolateToPointK(testPosition, xyzOnHelix, alpha);
    KBVector3 posOnHelix(xyzOnHelix,fReferenceAxis);

    position.SetI((numUsedTracks*position.I()+posOnHelix.I())/(numUsedTracks+1));
    position.SetJ((numUsedTracks*position.J()+posOnHelix.J())/(numUsedTracks+1));

    auto dist = (position-posOnHelix).Mag();
    if (numUsedTracks != 0)
      sTest = (double)numUsedTracks/(numUsedTracks+1)*sTest + dist/numUsedTracks;

    ++numUsedTracks;

    if (last) {
      track -> SetParentID(0);
      vertex -> AddTrack(track);
    }
  }

  if (last)
    vertex -> SetPosition(position.GetXYZ());

  return sTest;
}

void LHVertexFindingTask::SetVertexPersistency(bool val) { fPersistency = val; }
