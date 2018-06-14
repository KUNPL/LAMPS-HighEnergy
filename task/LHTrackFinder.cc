//#define PRINT_PROCESS
//#define PULLIN

#include "LHTrackFinder.hh"

#include <iostream>
using namespace std;

ClassImp(LHTrackFinder)

bool LHTrackFinder::Init()
{
  fPadPlane = (LHPadPlane *) fTpc -> GetPadPlane();

  fCandHits = new vKBHit;
  fGoodHits = new vKBHit;
  fBadHits = new vKBHit;

  fDefaultScale = fPar -> GetParDouble("LHTF_defaultScale");
  fTrackWCutLL = fPar -> GetParDouble("LHTF_trackWCutLL");
  fTrackWCutHL = fPar -> GetParDouble("LHTF_trackWCutHL");
  fTrackHCutLL = fPar -> GetParDouble("LHTF_trackHCutLL");
  fTrackHCutHL = fPar -> GetParDouble("LHTF_trackHCutHL");

  TString axis = fPar -> GetParString("tpcBFieldAxis");
       if (axis=="x") fReferenceAxis = KBVector3::kX;
  else if (axis=="y") fReferenceAxis = KBVector3::kY;
  else if (axis=="z") fReferenceAxis = KBVector3::kZ;
  else {
    kb_error << "Error! tpcBFieldAxis is " << axis << endl;
    return false;
  }

  fFitter = KBHelixTrackFitter::GetFitter();
  fFitter -> SetReferenceAxis(fReferenceAxis);

  return true;
}

void LHTrackFinder::FindTrack(TClonesArray *in, TClonesArray *out)
{
#ifdef CHECK_INITIAL_HITS
  fCheckHitIndex = CHECK_INITIAL_HITS;
#endif
  fTrackArray = out;

  fPadPlane -> ResetHitMap();
  fPadPlane -> SetHitArray(in);

#ifdef CHECK_INITIAL_HITS
  fCheckHitIndex = CHECK_INITIAL_HITS;
#endif

  fCandHits -> clear();
  fGoodHits -> clear();
  fBadHits -> clear();

  //fPadPlane -> Print("detail");

  while(1)
  {
    KBHelixTrack *track = NewTrack();
    if (track == nullptr)
      break;

    bool survive = false;

#ifdef PRINT_PROCESS
    auto count0 = 0;
    if (++count0, kb_print << "Init " << count0 << endl, InitTrack(track)) {
      auto count1 = 0;
      if (++count1, kb_print << "Continuum " << count0 << "->" << count1 << endl, TrackContinuum(track)) {
        auto count2 = 0;
        if (++count2, kb_print << "Extrapolation " << count0 << "->" << count1 << "->" << count2 << endl, TrackExtrapolation(track)) {
          TrackConfirmation(track);
          survive = true;
        }
      }
    }
#else
    if (InitTrack(track))
      if (TrackContinuum(track))
        if (TrackExtrapolation(track)) {
          TrackConfirmation(track);
          survive = true;
        }
#endif

    Int_t numBadHits = fBadHits -> size();
    for (Int_t iHit = 0; iHit < numBadHits; ++iHit)
      fPadPlane -> AddHit(fBadHits -> at(iHit));
    fBadHits -> clear();

    //if (track -> GetNumHits() < 10) survive = false;

    if (survive) {
      vector<KBHit *> *trackHits = track -> GetHitArray();
      Int_t trackID = track -> GetTrackID();
      Int_t numTrackHits = trackHits -> size();
      for (Int_t iTrackHit = 0; iTrackHit < numTrackHits; ++iTrackHit) {
        KBHit *trackHit = trackHits -> at(iTrackHit);
        trackHit -> AddTrackCand(trackID);
        fPadPlane -> AddHit(trackHit);
      }
    }
    else {
      vector<KBHit *> *trackHits = track -> GetHitArray();
      Int_t numTrackHits = trackHits -> size();
      for (Int_t iTrackHit = 0; iTrackHit < numTrackHits; ++iTrackHit) {
        KBHit *trackHit = trackHits -> at(iTrackHit);
        trackHit -> AddTrackCand(-1);
        fPadPlane -> AddHit(trackHit);
      }
      fTrackArray -> Remove(track);
    }
  }

  fTrackArray -> Compress();

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; ++iTrack) {
    KBHelixTrack *track = (KBHelixTrack *) fTrackArray -> At(iTrack);
    track -> FinalizeHits();
  }
}

KBHelixTrack *LHTrackFinder::NewTrack()
{
  KBHit *hit = fPadPlane -> PullOutNextFreeHit();
#ifdef CHECK_INITIAL_HITS
  if (fCheckHitIndex>0) {
    hit -> Print();
    fPadPlane -> GetPad(hit -> GetPadID()) -> Print();
    fCheckHitIndex--;
  }
#endif
  if (hit == nullptr)
    return nullptr;

  Int_t idx = fTrackArray -> GetEntries();
  KBHelixTrack *track = new ((*fTrackArray)[idx]) KBHelixTrack(idx);
  track -> SetReferenceAxis(fReferenceAxis);
  track -> AddHit(hit);

  fGoodHits -> push_back(hit);

  return track;
}

bool LHTrackFinder::InitTrack(KBHelixTrack *track)
{
#ifdef PULLIN
  fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
#else
  fPadPlane -> PullOutNeighborHits(fGoodHits, fCandHits);
#endif
  fGoodHits -> clear();

  Int_t numCandHits = fCandHits -> size();;
  while (numCandHits != 0) {
    sort(fCandHits->begin(), fCandHits->end(), KBHitSortByDistanceTo(track->GetMean()));

    for (Int_t iHit = 0; iHit < numCandHits; iHit++) {
      KBHit *candHit = fCandHits -> back();
      fCandHits -> pop_back();

      Double_t quality = CorrelateSimple(track, candHit);

      if (quality > 0) {
        fGoodHits -> push_back(candHit);
        track -> AddHit(candHit);

        if (track -> GetNumHits() > 15) {
          UInt_t numCandHits2 = fCandHits -> size();
          for (UInt_t iCand = 0; iCand < numCandHits2; ++iCand)
            fPadPlane -> AddHit(fCandHits -> at(iCand));
          fCandHits -> clear();
          break;
        }

        if (track -> GetNumHits() > 6) {
          fFitter -> Fit(track);
          if (!(track -> GetNumHits() < 10 && track -> GetHelixRadius() < 30) && (track -> TrackLength() > 2.5 * track -> GetRMSW())) {
            return true;
          }
        }

        fFitter -> FitPlane(track);
      }
      else
        fBadHits -> push_back(candHit);
    }

#ifdef PULLIN
    fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
#else
    fPadPlane -> PullOutNeighborHits(fGoodHits, fCandHits);
#endif
    fGoodHits -> clear();

    numCandHits = fCandHits -> size();
  }

  for (UInt_t iBad = 0; iBad < fBadHits -> size(); ++iBad)
    fPadPlane -> AddHit(fBadHits -> at(iBad));
  fBadHits -> clear();

  return false;
}

bool LHTrackFinder::TrackContinuum(KBHelixTrack *track)
{
#ifdef PULLIN
  fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
#else
  fPadPlane -> PullOutNeighborHits(fGoodHits, fCandHits);
#endif
  fGoodHits -> clear();

  Int_t numCandHits = fCandHits -> size();

  while (numCandHits != 0)
  {
    sort(fCandHits -> begin(), fCandHits -> end(), KBHitSortCharge());

    for (Int_t iHit = 0; iHit < numCandHits; iHit++) {
      KBHit *candHit = fCandHits -> back();
      fCandHits -> pop_back();

      Double_t quality = 0; 
      if (CheckHitOwner(candHit) == -2)
        quality = Correlate(track, candHit);

      if (quality > 0) {
        fGoodHits -> push_back(candHit);
        track -> AddHit(candHit);
        fFitter -> Fit(track);
      } else
        fBadHits -> push_back(candHit);
    }

#ifdef PULLIN
    fPadPlane -> PullOutNeighborHitsIn(fGoodHits, fCandHits);
#else
    fPadPlane -> PullOutNeighborHits(fGoodHits, fCandHits);
#endif
    fGoodHits -> clear();

    numCandHits = fCandHits -> size();
  }

  return TrackQualityCheck(track);
}

bool LHTrackFinder::TrackExtrapolation(KBHelixTrack *track)
{
  for (UInt_t iBad = 0; iBad < fBadHits -> size(); ++iBad)
    fPadPlane -> AddHit(fBadHits -> at(iBad));
  fBadHits -> clear();

  Int_t count = 0;
  bool buildHead = true;
  Double_t extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, buildHead, extrapolationLength)) {
    if (++count > 200)
      break;
  }

  count = 0;
  buildHead = !buildHead;
  extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, buildHead, extrapolationLength)) {
    if (++count > 200)
      break;
  }

  for (UInt_t iBad = 0; iBad < fBadHits -> size(); ++iBad)
    fPadPlane -> AddHit(fBadHits -> at(iBad));
  fBadHits -> clear();

  //return TrackQualityCheck(track);
  return true;
}

bool LHTrackFinder::TrackConfirmation(KBHelixTrack *track)
{
  bool tailToHead = false;
  //if (track -> PositionAtTail().Z() > track -> PositionAtHead().Z())
  KBVector3 pTail(track -> PositionAtTail(), fReferenceAxis);
  KBVector3 pHead(track -> PositionAtHead(), fReferenceAxis);

  if (pHead.K() > pHead.K())
    tailToHead = true;

  for (UInt_t iBad = 0; iBad < fBadHits -> size(); ++iBad)
    fPadPlane -> AddHit(fBadHits -> at(iBad));
  fBadHits -> clear();
  ConfirmHits(track, tailToHead);

  tailToHead = !tailToHead; 

  for (UInt_t iBad = 0; iBad < fBadHits -> size(); ++iBad)
    fPadPlane -> AddHit(fBadHits -> at(iBad));
  fBadHits -> clear();
  ConfirmHits(track, tailToHead);

  for (UInt_t iBad = 0; iBad < fBadHits -> size(); ++iBad)
    fPadPlane -> AddHit(fBadHits -> at(iBad));
  fBadHits -> clear();

  return true;
}

Int_t LHTrackFinder::CheckHitOwner(KBHit *hit)
{
  vector<Int_t> *candTracks = hit -> GetTrackCandArray();
  if (candTracks -> size() == 0)
    return -2;

  Int_t trackID = -1;
  for (UInt_t iCand = 0; iCand < candTracks -> size(); ++iCand) {
    Int_t candTrackID = candTracks -> at(iCand);
    if (candTrackID != -1) {
      trackID = candTrackID;
    }
  }

  return trackID;
}

Double_t LHTrackFinder::Correlate(KBHelixTrack *track, KBHit *hit, Double_t rScale)
{
  Double_t scale = rScale * fDefaultScale;
  Double_t trackLength = track -> TrackLength();
  if (trackLength < 500.)
    scale = scale + (500. - trackLength)/500.;

  /*
  auto direction = track->Momentum().Unit();
  Double_t dot = abs(direction.Dot(KBVector3(fReferenceAxis,0,0,1).GetXYZ()));
  auto a = 1.;
  auto b = 1.;
  if (dot > .5) {
     a = (2*(dot-.5)+1); //
     b = (2*(dot-.5)+1); //
  }
  auto trackHCutLL = a*fTrackHCutLL;
  auto trackHCutHL = a*fTrackHCutHL;
  auto trackWCutLL = b*fTrackWCutLL;
  auto trackWCutHL = b*fTrackWCutHL;
  */
  auto trackHCutLL = fTrackHCutLL;
  auto trackHCutHL = fTrackHCutHL;
  auto trackWCutLL = fTrackWCutLL;
  auto trackWCutHL = fTrackWCutHL;

  Double_t rmsWCut = track -> GetRMSW();
  if (rmsWCut < trackWCutLL) rmsWCut = trackWCutLL;
  if (rmsWCut > trackWCutHL) rmsWCut = trackWCutHL;
  rmsWCut = scale * rmsWCut;

  Double_t rmsHCut = track -> GetRMSH();
  if (rmsHCut < trackHCutLL) rmsHCut = trackHCutLL;
  if (rmsHCut > trackHCutHL) rmsHCut = trackHCutHL;
  rmsHCut = scale * rmsHCut;

  TVector3 qHead = track -> Map(track -> PositionAtHead());
  TVector3 qTail = track -> Map(track -> PositionAtTail());
  TVector3 q = track -> Map(hit -> GetPosition());

  if (qHead.Z() > qTail.Z()) {
    if (LengthAlphaCut(track, q.Z() - qHead.Z())) return 0;
    if (LengthAlphaCut(track, qTail.Z() - q.Z())) return 0;
  } else {
    if (LengthAlphaCut(track, q.Z() - qTail.Z())) return 0;
    if (LengthAlphaCut(track, qHead.Z() - q.Z())) return 0;
  }

  Double_t dr = abs(q.X());
  Double_t quality = 0;
  if (dr < rmsWCut && abs(q.Y()) < rmsHCut)
    quality = sqrt((dr-rmsWCut)*(dr-rmsWCut)) / rmsWCut;

  return quality;
}

bool LHTrackFinder::LengthAlphaCut(KBHelixTrack *track, Double_t dLength)
{
  if (dLength > 0) {
    if (dLength > .5*track -> TrackLength()) {
      if (abs(track -> AlphaByLength(dLength)) > .5*TMath::Pi()) {
        return true;
      }
    }
  }
  return false;
}

Double_t LHTrackFinder::CorrelateSimple(KBHelixTrack *track, KBHit *hit)
{
  if (hit -> GetNumTrackCands() != 0)
    return 0;

  Double_t quality = 0;

  auto row = hit -> GetRow();
  auto layer = hit -> GetLayer();

  KBVector3 pos0(hit -> GetPosition(), fReferenceAxis);

  vector<KBHit *> *trackHits = track -> GetHitArray();
  bool kcut = false;
  Int_t numTrackHits = trackHits -> size();
  for (Int_t iTrackHit = 0; iTrackHit < numTrackHits; ++iTrackHit) {
    KBHit *trackHit = trackHits -> at(iTrackHit);
    if (row == trackHit -> GetRow() && layer == trackHit -> GetLayer())
      return 0;
    KBVector3 pos1(trackHit -> GetPosition(), fReferenceAxis);
    auto di = pos0.I() - pos1.I();
    auto dj = pos0.J() - pos1.J();
    auto padDisplacement = sqrt(di*di + dj*dj);
    Double_t kcutv = 1.2 * padDisplacement*abs(pos1.K())/sqrt(pos1.I()*pos1.I()+pos1.J()*pos1.J());

    if (kcutv < 4) kcutv = 4;
    if (abs(pos0.K()-pos1.K()) < kcutv) kcut = true;
  }
  if (kcut == false) {
    return 0;
  }

  ////////////////////////////////////////////////////////////////
  if (track -> IsBad()) {
    quality = 1;
  }
  ////////////////////////////////////////////////////////////////
  else if (track -> IsLine()) {
    KBVector3 perp = track -> PerpLine(hit -> GetPosition());

    Double_t rmsCut = track -> GetRMSH();
    if (rmsCut < fTrackHCutLL) rmsCut = fTrackHCutLL;
    if (rmsCut > fTrackHCutHL) rmsCut = fTrackHCutHL;
    rmsCut = 3 * rmsCut;

    if (perp.K() > rmsCut) {
      quality = 0;
    }
    else {
      perp.SetK(0);
      if (perp.Mag() < 15)
      //if (perp.Mag() < 10*pos1.K()/sqrt(pos1.Mag()))
      {
        quality = 1;
      }
    }
  }
  ////////////////////////////////////////////////////////////////
  else if (track -> IsPlane()) {
    Double_t dist = (track -> PerpPlane(hit -> GetPosition())).Mag();

    Double_t rmsCut = track -> GetRMSH();
    if (rmsCut < fTrackHCutLL) rmsCut = fTrackHCutLL;
    if (rmsCut > fTrackHCutHL) rmsCut = fTrackHCutHL;
    rmsCut = 3 * rmsCut;

    if (dist < rmsCut) {
      quality = 1;
    }
  }
  ////////////////////////////////////////////////////////////////

  return quality;
}

bool LHTrackFinder::ConfirmHits(KBHelixTrack *track, bool &tailToHead)
{
  track -> SortHits(!tailToHead);
  vector<KBHit *> *trackHits = track -> GetHitArray();
  Int_t numHits = trackHits -> size();

  TVector3 q, m;
  //Double_t lPre = track -> ExtrapolateByMap(trackHits->at(numHits-1)->GetPosition(), q, m);

  Double_t extrapolationLength = 10.;
  for (Int_t iHit = 1; iHit < numHits; iHit++) 
  {
    KBHit *trackHit = trackHits -> at(numHits-iHit-1);
    //Double_t lCur = track -> ExtrapolateByMap(trackHit->GetPosition(), q, m);

    Double_t quality = Correlate(track, trackHit);

    if (quality <= 0) {
      track -> RemoveHit(trackHit);
      trackHit -> RemoveTrackCand(trackHit -> GetTrackID());
      Int_t helicity = track -> Helicity();
      fFitter -> Fit(track);
      if (helicity != track -> Helicity())
        tailToHead = !tailToHead;

      continue;
    }

    /*
    Double_t dLength = abs(lCur - lPre);
    extrapolationLength = 10;
    while(dLength > 0 && AutoBuildByInterpolation(track, tailToHead, extrapolationLength, 1)) { dLength -= 10; }
    */
  }

  extrapolationLength = 0;
  while (AutoBuildByExtrapolation(track, tailToHead, extrapolationLength)) {}

  return true;
}

bool LHTrackFinder::AutoBuildByExtrapolation(KBHelixTrack *track, bool &buildHead, Double_t &extrapolationLength)
{
  TVector3 p;
  if (buildHead) p = track -> ExtrapolateHead(extrapolationLength);
  else           p = track -> ExtrapolateTail(extrapolationLength);

  return AutoBuildAtPosition(track, p, buildHead, extrapolationLength);
}

bool LHTrackFinder::AutoBuildByInterpolation(KBHelixTrack *track, bool &tailToHead, Double_t &extrapolationLength, Double_t rScale)
{
  TVector3 p;
  if (tailToHead) p = track -> InterpolateByLength(extrapolationLength);
  else            p = track -> InterpolateByLength(track -> TrackLength() - extrapolationLength);

  return AutoBuildAtPosition(track, p, tailToHead, extrapolationLength, rScale);
}

bool LHTrackFinder::AutoBuildAtPosition(KBHelixTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t rScale)
{
  //if (fPadPlane -> IsInBoundary(p.X(), p.Z()) == false)
  KBVector3 p2(p,fReferenceAxis);
  if (fPadPlane -> IsInBoundary(p2.I(), p2.J()) == false)
    return false;

  Int_t helicity = track -> Helicity();

  Double_t rms = 3*track -> GetRMSW();
  if (rms < 25) 
    rms = 25;

  Int_t range = Int_t(rms/8);
  TVector2 q(p2.I(), p2.J());
  fPadPlane -> PullOutNeighborHits(q, range, fCandHits);

  Int_t numCandHits = fCandHits -> size();
  Bool_t foundHit = false;

  if (numCandHits != 0) 
  {
    sort(fCandHits -> begin(), fCandHits -> end(), KBHitSortCharge());

    for (Int_t iHit = 0; iHit < numCandHits; iHit++) {
      KBHit *candHit = fCandHits -> back();
      fCandHits -> pop_back();
      TVector3 pos = candHit -> GetPosition();

      Double_t quality = 0; 
      if (CheckHitOwner(candHit) < 0) 
        quality = Correlate(track, candHit, rScale);

      if (quality > 0) {
        track -> AddHit(candHit);
        fFitter -> Fit(track);
        foundHit = true;
      } else
        fBadHits -> push_back(candHit);
    }
  }

  if (foundHit) {
    extrapolationLength = 10; 
    if (helicity != track -> Helicity())
      tailToHead = !tailToHead;
  }
  else {
    extrapolationLength += 10; 
    if (extrapolationLength > 3 * track -> TrackLength()) {
      return false;
    }
  }

  return true;
}

bool LHTrackFinder::TrackQualityCheck(KBHelixTrack *track)
{
  Double_t continuity = track -> Continuity();
  if (continuity < .6) {
    if (track -> TrackLength() * continuity < 500)
      return false;
  }

  if (track -> GetHelixRadius() < 25)
    return false;

  return true;
}
