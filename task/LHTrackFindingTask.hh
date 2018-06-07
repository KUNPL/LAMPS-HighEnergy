#ifndef LHTRACKFINDINGTASK_HH
#define LHTRACKFINDINGTASK_HH

#include "KBTask.hh"
#include "LHTrackFinder.hh"
#include "TClonesArray.h"

class LHTrackFindingTask : public KBTask
{ 
  public:
    LHTrackFindingTask();
    virtual ~LHTrackFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetTrackPersistency(bool val);

  private:
    TClonesArray* fHitArray;
    TClonesArray* fTrackArray;

    bool fPersistency = true;

    LHTrackFinder *fTrackFinder = nullptr;

  ClassDef(LHTrackFindingTask, 1)
};

#endif
