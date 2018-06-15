#ifndef LHVERTEXFINDINGTASK_HH
#define LHVERTEXFINDINGTASK_HH

#include "KBTask.hh"
#include "KBHelixTrackFitter.hh"
#include "KBVertex.hh"
#include "KBVector3.hh"

#include "TClonesArray.h"

class LHVertexFindingTask : public KBTask
{ 
  public:
    LHVertexFindingTask();
    virtual ~LHVertexFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    Double_t TestVertexAtK(KBVertex *vertex, KBVector3 testPosition, bool last = false);

    void SetVertexPersistency(bool val);

    void SetKRange(Double_t k1, Double_t k2);

  private:
    TClonesArray* fTrackArray;
    TClonesArray* fVertexArray;

    KBHelixTrackFitter *fTrackFitter;

    bool fPersistency = true;

    Double_t fK1 = -300.;
    Double_t fK2 = 900.;

    KBVector3::Axis fReferenceAxis = KBVector3::kZ;

  ClassDef(LHVertexFindingTask, 1)
};

#endif
