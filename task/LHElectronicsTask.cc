#include "KBRun.hh"
#include "LHElectronicsTask.hh"

#include <iostream>
using namespace std;

ClassImp(LHElectronicsTask)

LHElectronicsTask::LHElectronicsTask()
:KBTask("LHElectronicsTask","")
{
}

bool LHElectronicsTask::Init()
{
  KBRun *run = KBRun::GetRun();

  KBParameterContainer *par = run -> GetParameterContainer();
  KBDetector *det = run -> GetDetector();

  fNPlanes = det -> GetNPlanes();
  fNTbs = par -> GetParInt("nTbs");
  feVToADC = par -> GetParDouble("eVToADC");
  fDynamicRange = par -> GetParDouble("dynamicRange");

  fPadArray = (TClonesArray *) run -> GetBranch("Pad");

  KBPulseGenerator *pulseGen = new KBPulseGenerator();
  fPulseFunction = pulseGen -> GetPulseFunction();
  fPulseFunction -> SetParameters(feVToADC,0);

  return true;
}

void LHElectronicsTask::Exec(Option_t*)
{
  Int_t nPads = fPadArray -> GetEntries();
  for (Int_t iPad = 0; iPad < nPads; iPad++) {
    KBPad *pad = (KBPad *) fPadArray -> At(iPad);
    Double_t *in = pad -> GetBufferIn();
    Double_t out[512] = {0};

    for (Int_t iTb = 0; iTb < fNTbs; iTb++) {
      if (in[iTb] == 0)
        continue;

      for (Int_t iTb2 = 0; iTb2 < 100; iTb2++) {
        Int_t tb = iTb+iTb2;
        if (tb >= fNTbs)
          break;

        out[tb] += in[iTb] * fPulseFunction -> Eval(iTb2);
        if (out[tb] > fDynamicRange)
          out[tb] = fDynamicRange;
      }
    }

    auto saturated = false;
    Int_t saturatedAt = 100000;
    for (Int_t iTb = 0; iTb < fNTbs; iTb++) {
      if (out[iTb] > fDynamicRange) {
        saturated = true;
        saturatedAt = iTb;
        break;
      }
    }
    for (Int_t iTb = saturatedAt+10; iTb < fNTbs; iTb++)
      out[iTb] = 0;

    pad -> SetBufferOut(out);
  }

  kb_info << endl;
  
  return;
}
