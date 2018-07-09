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
  fVertexArray -> Clear("C");

  if (fTrackArray -> GetEntriesFast() < 2) {
    kb_warning << "Less than 2 tracks are given. Unable to find vertex." << endl;
    return;
  }

  KBVertex *vertex = new ((*fVertexArray)[0]) KBVertex();

  Double_t sLeast = 1.e8;
  Double_t kAtSLeast = 0;
  Double_t sTest = 0;

  for (auto k = -10.; k < 10.; k+=0.1) {
    KBVector3 testPosition(fReferenceAxis, 0, 0, k);
    sTest = TestVertexAtK(vertex, testPosition);
    if (sTest < sLeast) {
      sLeast = sTest;
      kAtSLeast = k;
    }
  }

  KBVector3 testPosition(fReferenceAxis, 0, 0, kAtSLeast);
  sTest = TestVertexAtK(vertex, testPosition, true);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
    track -> Fit();
    track -> DetermineParticleCharge(vertex -> GetPosition());
  }

  auto pos = vertex -> GetPosition();

  kb_info << "Found vertex at " << Form("(%.1f, %.1f, %.1f)",pos.X(),pos.Y(),pos.Z()) << " with " << vertex -> GetNumTracks() << " tracks" << endl;

  return;
}

Double_t LHVertexFindingTask::TestVertexAtK(KBVertex *vertex, KBVector3 testPosition, bool last)
{
  Double_t sTest = 0;
  Int_t numUsedTracks = 0;

  TVector3 averagePosition(0,0,0);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);

    auto xyzOnHelix = track -> ExtrapolateTo(testPosition);
    auto dist = (testPosition.GetXYZ()-xyzOnHelix).Mag();

    if (last && dist > 10)
      continue;

    averagePosition += xyzOnHelix;

    if (numUsedTracks != 0)
      sTest = ((Double_t)numUsedTracks)/(numUsedTracks+1)*sTest + dist/numUsedTracks;

    ++numUsedTracks;

    if (last) {
      track -> SetParentID(0);
      vertex -> AddTrack(track);
    }
  }

  averagePosition = (1./(numUsedTracks))*averagePosition;

  if (last) {
    for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
      KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
      if (track -> GetParentID() == 0) {
        track -> AddHit(vertex);
      }
    }
  }

  if (last)
    vertex -> SetPosition(averagePosition);

  return sTest;
}

void LHVertexFindingTask::SetVertexPersistency(bool val) { fPersistency = val; }
