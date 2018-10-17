#ifndef LHHITCLUSTERING_HH
#define LHHITCLUSTERING_HH

#include "LHHitClustering.hh"
#include "KBHit.hh"

class LHHitClustering : public KBTask
{
  public:
    LHHitClustering();
    virtual ~LHHitClustering() {};

    bool Init();
    void Exec(Option_t *);

  private:
    TClonesArray* fHitClusterArray;
    TClonesArray* fHitNClusterArray;
    TClonesArray* fHit;
    TClonesArray* fPad;

  ClassDef(LHHitClustering, 1)
};


#endif
