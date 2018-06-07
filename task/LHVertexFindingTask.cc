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

  Double_t y0 = 300;
  Double_t dy = 100;
  Double_t s0 = 1.e8;

  const Int_t numSamples = 9;

  Int_t halfOfSamples = (numSamples)/2;

  Double_t yArray[numSamples] = {0};
  for (Int_t iSample = 0; iSample <= numSamples; iSample++)
    yArray[iSample] = (iSample - halfOfSamples) * dy + y0;

  Int_t nIterations = 4;

  for (Int_t iSample = 0; iSample <= numSamples; ++iSample) {
    Double_t y = yArray[iSample];
    TVector3 v(0, y, 0);
    Double_t s = TestVertexAtK(vertex, nIterations, v);

    if (s < s0) {
      s0 = s;
      y0 = y;
    }
  }

  while (nIterations > 0) {
    dy = dy/halfOfSamples;
    for (Int_t iSample = 0; iSample <= numSamples; iSample++)
      yArray[iSample] = (iSample - halfOfSamples) * dy + y0;

    for (Int_t iSample = 0; iSample <= numSamples; ++iSample) {
      Double_t y = yArray[iSample];
      TVector3 v(0, y, 0);
      Double_t s = TestVertexAtK(vertex, nIterations, v);

      if (s < s0) {
        s0 = s;
        y0 = y;
      }
    }

    nIterations--;
  }

  TVector3 v(0, y0, 0);
  TestVertexAtK(vertex, 0, v, true);

  vertex -> SetPosition(v);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
    track -> DetermineParticleCharge(vertex -> GetPosition());
    fTrackFitter -> Fit(track);
  }

  kb_info << "Found vertex at " << Form("(%.1f, %.1f, %.1f)",v.X(),v.Y(),v.Z()) << " with " << vertex -> GetNumTracks() << " tracks" << endl;

  return;
}

Double_t LHVertexFindingTask::TestVertexAtK(KBVertex *vertex, Int_t itID, TVector3 &v, bool last)
{
  Double_t s = 0;
  Int_t numUsedTracks = 0;

  v.SetX(0);
  v.SetZ(0);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);

    TVector3 p;
    Double_t alpha;
    track -> ExtrapolateToPointK(TVector3(0,v.Y(),0), p, alpha);

    v.SetX((numUsedTracks*v.X() + p.X())/(numUsedTracks+1));
    v.SetZ((numUsedTracks*v.Z() + p.Z())/(numUsedTracks+1));

    if (numUsedTracks != 0)
      s = (double)numUsedTracks/(numUsedTracks+1)*s + (v-p).Mag()/numUsedTracks;

    numUsedTracks++;

    if (last) {
      track -> SetParentID(0);
      vertex -> AddTrack(track);
    }
  }

  vertex -> AddSSet(KBVSSet(itID, v.Y(), s, numUsedTracks));

  return s;
}

void LHVertexFindingTask::SetVertexPersistency(bool val) { fPersistency = val; }
