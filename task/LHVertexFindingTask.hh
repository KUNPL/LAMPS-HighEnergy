#ifndef LHVERTEXFINDINGTASK_HH
#define LHVERTEXFINDINGTASK_HH

#include "KBTask.hh"
#include "KBHelixTrackFitter.hh"
#include "KBVertex.hh"

#include "TClonesArray.h"

class LHVertexFindingTask : public KBTask
{ 
  public:
    LHVertexFindingTask();
    virtual ~LHVertexFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    Double_t TestVertexAtK(KBVertex *vertex, Int_t itID, TVector3 &v, bool last = false);

    void SetVertexPersistency(bool val);

  private:
    TClonesArray* fTrackArray;
    TClonesArray* fVertexArray;

    KBHelixTrackFitter *fTrackFitter;

    bool fPersistency = true;

  ClassDef(LHVertexFindingTask, 1)
};

#endif
