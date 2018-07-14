#ifndef LHDRIFTElECTRONTASK_HH
#define LHDRIFTElECTRONTASK_HH

#include "KBTask.hh"
#include "KBTpc.hh"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHDriftElectronTask : public KBTask
{ 
  public:
    LHDriftElectronTask();
    virtual ~LHDriftElectronTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

  private:
    TClonesArray* fMCStepArray;
    TClonesArray* fPadArray;
    bool fPersistency = true;

    KBTpc *fTpc = nullptr;
    KBPadPlane *fPadPlane = nullptr;
    Int_t fNPlanes = 0;

    Double_t fDriftVelocity = 0;
    Double_t fCoefLD = 0;
    Double_t fCoefTD = 0;
    Double_t fEIonize = 0;

    Int_t fNElInCluster = 0;

    TH2D *fDiffusionFunction = nullptr;
    TF1 *fGainFunction = nullptr;
    Double_t fGainZeroRatio = 0;

    Int_t fNTbs;
    Double_t fTbTime;

    Double_t fSelectedTrackID = -1;

  ClassDef(LHDriftElectronTask, 1)
};

#endif
