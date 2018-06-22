#ifndef LHELECTRONICSTASK_HH
#define LHELECTRONICSTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"
#include "KBPulseGenerator.hh"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHElectronicsTask : public KBTask
{ 
  public:
    LHElectronicsTask();
    virtual ~LHElectronicsTask() {}

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fPadArray;

    Int_t fNPlanes;
    Int_t fNTbs;
    Double_t feVToADC;
    Double_t fDynamicRange;

    TF1 *fPulseFunction;

  ClassDef(LHElectronicsTask, 1)
};

#endif
