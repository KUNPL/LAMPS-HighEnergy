#ifndef LHTRACKFINDER_HH
#define LHTRACKFINDER_HH

//#define CHECK_INITIAL_HITS 1 //XXX
//#define PRINT_PROCESS_SUMMARY

#include "KBTrackFinder.hh"
#include "KBHelixTrack.hh"

#include "TClonesArray.h"

#include "LHTpc.hh"
#include "LHPadPlane.hh"

#include <vector>
using namespace std;

typedef vector<KBTpcHit*> KBTpcHits;
typedef vector<KBHit*> KBHits;

class LHTrackFinder : public KBTrackFinder
{
  public:
    LHTrackFinder():KBTrackFinder("LHTrackFinder","LHTrackFinder") {}
    virtual ~LHTrackFinder() {}

    void SetTpc(LHTpc *tpc) { fTpc = tpc; }

    virtual bool Init();

    /**
     * @param in   Array of KBTpcHit
     * @param out  Array of KBTracklet
     */
    virtual void FindTrack(TClonesArray *in, TClonesArray *out);

  private:
    KBHelixTrack *NewTrack();
    bool InitTrack(KBHelixTrack* track);
    bool TrackContinuum(KBHelixTrack *track);
    bool TrackExtrapolation(KBHelixTrack *track);
    bool TrackConfirmation(KBHelixTrack* track);

    Int_t CheckHitOwner(KBTpcHit *hit);
    Double_t Correlate(KBHelixTrack *track, KBTpcHit *hit, Double_t scale=1);
    Double_t CorrelateSimple(KBHelixTrack *track, KBTpcHit *hit);
    bool LengthAlphaCut(KBHelixTrack *track, Double_t dLength);

    bool ConfirmHits(KBHelixTrack* track, bool &tailToHead);
    bool AutoBuildByExtrapolation(KBHelixTrack *track, bool &buildHead, Double_t &extrapolationLength);
    bool AutoBuildByInterpolation(KBHelixTrack *track, bool &tailToHead, Double_t &extrapolationLength, Double_t scale);
    bool AutoBuildAtPosition(KBHelixTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t scale=1);
    bool TrackQualityCheck(KBHelixTrack *track);
    Double_t Continuity(KBHelixTrack *track);

  private:
    LHTpc *fTpc = nullptr;
    LHPadPlane *fPadPlane = nullptr;
    TClonesArray *fTrackArray = nullptr;

    KBTpcHits *fCandHits = nullptr;
    KBTpcHits *fGoodHits = nullptr;
    KBTpcHits *fBadHits  = nullptr;

    Double_t fDefaultScale;
    Double_t fTrackWCutLL;  ///< Track width cut low limit
    Double_t fTrackWCutHL;  ///< Track width cut high limit
    Double_t fTrackHCutLL;  ///< Track height cut low limit
    Double_t fTrackHCutHL;  ///< Track height cut high limit

    KBVector3::Axis fReferenceAxis;

    KBTpcHit *fVertexPoint;

#ifdef PRINT_PROCESS_SUMMARY
    Int_t fCountNew;
    Int_t fCountInit;
    Int_t fCountConti;
    Int_t fCountExtrap;
    Int_t fCountConfirm;
#endif

#ifdef CHECK_INITIAL_HITS
    Int_t fCheckHitIndex = CHECK_INITIAL_HITS;
#endif

  ClassDef(LHTrackFinder, 1)
};

#endif
