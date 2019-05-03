#include "LHTrackFindingTask.hh"

#include "KBRun.hh"

#include <iostream>
using namespace std;

ClassImp(LHTrackFindingTask)

LHTrackFindingTask::LHTrackFindingTask()
:KBTask("LHTrackFindingTask","")
{
}

bool LHTrackFindingTask::Init()
{
  KBRun *run = KBRun::GetRun();
  KBParameterContainer *par = run -> GetParameterContainer();

  fHitArray = (TClonesArray *) run -> GetBranch("Hit");

  fTrackArray = new TClonesArray("KBHelixTrack");
  run -> RegisterBranch("Tracklet", fTrackArray, fPersistency);

  LHTpc *tpc = (LHTpc *) (run -> GetDetectorSystem() -> GetTpc());

  fTrackFinder = new LHTrackFinder();
  fTrackFinder -> SetParameterContainer(par);
  fTrackFinder -> SetTpc(tpc);
  fTrackFinder -> SetRank(fRank+1);
  auto initTF = fTrackFinder -> Init();
  if (initTF == false)
    return false;

  return true;
}

void LHTrackFindingTask::Exec(Option_t*)
{
  fTrackArray -> Clear("C");

  fTrackFinder -> FindTrack(fHitArray, fTrackArray);

  kb_info << "Number of found tracks: " << fTrackArray -> GetEntries() << endl;

  return;
}

void LHTrackFindingTask::SetTrackPersistency(bool val) { fPersistency = val; }
