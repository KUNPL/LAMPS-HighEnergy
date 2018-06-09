#include "LHPadPlaneRPad.hh"

#include "TGraph.h"
#include "TMath.h"
#include "TVector2.h"
#include "TH2Poly.h"
#include "TCollection.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TCanvas.h"

#include <iostream>
using namespace std;

ClassImp(LHPadPlaneRPad)

LHPadPlaneRPad::LHPadPlaneRPad()
:KBPadPlane("LAMPSTPC PadPlane RectangularPads", "LAMPS-TPC Pad Plane with rectangular pads")
{
}

bool LHPadPlaneRPad::Init()
{
  fRMin = fPar -> GetParDouble("rMinTPC");
  fRMax = fPar -> GetParDouble("rMaxTPC");

  fTanPi1o8 = TMath::Tan(TMath::Pi()*1./8.);
  fTanPi3o8 = TMath::Tan(TMath::Pi()*3./8.);
  fTanPi5o8 = TMath::Tan(TMath::Pi()*5./8.);
  fTanPi7o8 = TMath::Tan(TMath::Pi()*7./8.);
  for (Int_t i = 0; i < 8; i++) {
    fCosPiNo4[i] = TMath::Cos(TMath::Pi()*i/4.);
    fSinPiNo4[i] = TMath::Sin(TMath::Pi()*i/4.);
  }

  fFuncXRightBound
    = new TF1("RightBound",Form("%f*(x-%f)+%f",fTanPi3o8,.5*fWPPBot,fYPPMin),0,500);
  fFuncXRightBoundInverse
    = new TF1("RightBoundInverse",Form("(x-%f)/%f+%f",fYPPMin,fTanPi3o8,.5*fWPPBot),0,500);

  fXSpacing = fPadGap + fPadWid;
  fYSpacing = fPadGap + fPadHei;

  ////////////////////////////////////////////////////////////////////////////////////

  for (Int_t section = 0; section < 8; section++)
  {
    Double_t phiSection = section * TMath::Pi()/4.;
    //Double_t phiSection = section * TMath::Pi()/4. + TMath::Pi()/2.;

    for (Int_t layer = 0; true; ++layer) 
    {
      auto yPadBot = fYPPMin+fYSpacing*layer;
      if (yPadBot > fYPPMax) {
        fLayerMax = layer-1;
        break;
      }

      bool cuttedTop = false;
      auto yPadTop = fYPPMin+fPadHei+fYSpacing*layer;
      if (yPadTop > fYPPMax) {
        yPadTop = fYPPMax;
        cuttedTop = true;
      }

      auto breakFromRow = false;
      for (Int_t row = 1; true; ++row)
      {
        if (breakFromRow)
          break;

        for (auto pm : {1, -1}) // right-half and left-half
        {
          auto xPadInn = .5*fPadGap+fXSpacing*(row-1);
          auto xPadOut = .5*fPadGap+fXSpacing*(row-1)+fPadWid;

          bool cuttedTo3 = false;
          bool cuttedTo4 = false;
          bool cuttedTo5 = false;

          if (yPadTop < fFuncXRightBound->Eval(xPadInn)) {
            fHalfRowMax.push_back(row-1);
            breakFromRow = true;
            break;
          }
          else if (yPadTop < fFuncXRightBound->Eval(xPadOut)) cuttedTo3 = true;
          else if (yPadBot < fFuncXRightBound->Eval(xPadInn)) cuttedTo4 = true;
          else if (yPadBot < fFuncXRightBound->Eval(xPadOut)) cuttedTo5 = true;

          xPadInn = pm*xPadInn;
          xPadOut = pm*xPadOut;

          TVector2 posCenter(.5*(xPadInn+xPadOut), .5*(yPadBot+yPadTop));
          posCenter = posCenter.Rotate(phiSection);
          auto pad = AddPad(section, pm*row, layer, posCenter.X(), posCenter.Y());

          TVector2 posCorner(xPadInn, yPadTop);
          posCorner = posCorner.Rotate(phiSection);
          pad -> AddPadCorner(posCorner.X(), posCorner.Y());
          if (cuttedTo3)
          {
            posCorner.Set(xPadInn, fFuncXRightBound->Eval(pm*xPadInn));
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            posCorner.Set(pm*fFuncXRightBoundInverse->Eval(yPadTop), yPadTop);
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            auto xAverage = 0.;
            auto yAverage = 0.;
            auto corners = pad -> GetPadCorners();
            for (auto iCorner = 0; iCorner < 3; ++iCorner) {
              TVector2 corner = corners->at(iCorner);
              xAverage += corner.X();
              yAverage += corner.Y();
            }
            pad -> SetPosition(xAverage/3, yAverage/3);

            continue;
          }

          posCorner.Set(xPadOut, yPadTop);
          posCorner = posCorner.Rotate(phiSection);
          pad -> AddPadCorner(posCorner.X(), posCorner.Y());
          if (cuttedTo4)
          {
            posCorner.Set(xPadOut, fFuncXRightBound->Eval(pm*xPadOut));
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            posCorner.Set(xPadInn, fFuncXRightBound->Eval(pm*xPadInn));
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            auto xAverage = 0.;
            auto yAverage = 0.;
            auto corners = pad -> GetPadCorners();
            for (auto iCorner = 0; iCorner < 4; ++iCorner) {
              TVector2 corner = corners->at(iCorner);
              xAverage += corner.X();
              yAverage += corner.Y();
            }
            pad -> SetPosition(xAverage/4, yAverage/4);

            continue;
          }

          if (cuttedTo5)
          {
            posCorner.Set(xPadOut, fFuncXRightBound->Eval(pm*xPadOut));
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            posCorner.Set(pm*fFuncXRightBoundInverse->Eval(yPadBot), yPadBot);
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            posCorner.Set(xPadInn, yPadBot);
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            auto xAverage = 0.;
            auto yAverage = 0.;
            auto corners = pad -> GetPadCorners();
            for (auto iCorner = 0; iCorner < 5; ++iCorner) {
              TVector2 corner = corners->at(iCorner);
              xAverage += corner.X();
              yAverage += corner.Y();
            }
            pad -> SetPosition(xAverage/5, yAverage/5);
          }
          else {
            posCorner.Set(xPadOut, yPadBot);
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());

            posCorner.Set(xPadInn, yPadBot);
            posCorner = posCorner.Rotate(phiSection);
            pad -> AddPadCorner(posCorner.X(), posCorner.Y());
          }
        }
      }
    }
  }

  fChannelArray -> Sort();

  Int_t numPads = fChannelArray -> GetEntriesFast();
  for (Int_t iPad = 0; iPad < numPads; iPad++) {
    KBPad *pad = (KBPad *) fChannelArray -> At(iPad);
    auto pos = pad -> GetPosition();
    auto r = sqrt(pos.X()*pos.X() + pos.Y()*pos.Y());

    if (r < fRMin || r > fRMax) {
      pad -> SetPadID(-1);
      fChannelArray -> Remove(pad);
      continue;
    }

    auto corners = pad -> GetPadCorners();
    Int_t numPoints = corners -> size();
    if (numPoints == 3) {
      auto corner1 = corners -> at(0);
      auto corner2 = corners -> at(1);
      auto corner3 = corners -> at(2);
      if (.5*abs(corner1.X()-corner2.X())*abs(corner1.Y()-corner3.Y()) < 5)
        pad -> SetPadID(-1);
        fChannelArray -> Remove(pad);
    }
  }
  fChannelArray -> Compress();

  numPads = fChannelArray -> GetEntriesFast();
  for (Int_t iPad = 0; iPad < numPads; iPad++) {
    KBPad *pad = (KBPad *) fChannelArray -> At(iPad);
    pad -> SetPadID(iPad);
    MapPad(pad);
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer < fLayerMax; iLayer++) {
      Int_t nHalfRows = fHalfRowMax[iLayer];
      for (Int_t iRow = -nHalfRows; iRow < nHalfRows; iRow++) {
        if (iRow == 0)
          continue;
        std::vector<Int_t> key0;
        key0.push_back(iSection);
        key0.push_back(iRow);
        key0.push_back(iLayer);
        KBPad *pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

        Int_t row1 = iRow+1;
        if (iRow == -1)
          row1 = 1;

        std::vector<Int_t> key1;
        key1.push_back(iSection);
        key1.push_back(row1);
        key1.push_back(iLayer);
        KBPad *pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);

        if (pad0 -> GetPadID() != -1 && pad1 -> GetPadID() != -1) {
          pad0 -> AddNeighborPad(pad1);
          pad1 -> AddNeighborPad(pad0);
        }
      }
    }
  }

  for (Int_t iSection = 0; iSection < 8; iSection++) {
    for (Int_t iLayer = 0; iLayer < fLayerMax-1; iLayer++) {
      Int_t nHalfRows0 = fHalfRowMax[iLayer];
      Int_t nHalfRows1 = fHalfRowMax[iLayer+1];
      for (Int_t iRow = -nHalfRows0; iRow < nHalfRows0; iRow++) {
        if (iRow == 0)
          continue;

        std::vector<Int_t> key0;
        key0.push_back(iSection);
        key0.push_back(iRow);
        key0.push_back(iLayer);
        KBPad *pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

        std::vector<Int_t> key1;
        key1.push_back(iSection);
        key1.push_back(iRow);
        key1.push_back(iLayer+1);
        KBPad *pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);

        if (pad0 -> GetPadID() != -1 && pad1 -> GetPadID() != -1) {
          pad0 -> AddNeighborPad(pad1);
          pad1 -> AddNeighborPad(pad0);
        }

        if (iRow+1<=nHalfRows1) {
          std::vector<Int_t> key2;
          key2.push_back(iSection);
          key2.push_back(iRow+1);
          key2.push_back(iLayer+1);
          KBPad *pad2 = (KBPad *) fChannelArray -> At(fPadMap[key2]);

          if (pad0 -> GetPadID() != -1 && pad2 -> GetPadID() != -1) {
            pad0 -> AddNeighborPad(pad2);
            pad2 -> AddNeighborPad(pad0);
          }
        }

        if (iRow-1>=-nHalfRows1) {
          std::vector<Int_t> key2;
          key2.push_back(iSection);
          key2.push_back(iRow-1);
          key2.push_back(iLayer+1);
          KBPad *pad2 = (KBPad *) fChannelArray -> At(fPadMap[key2]);

          if (pad0 -> GetPadID() != -1 && pad2 -> GetPadID() != -1) {
            pad0 -> AddNeighborPad(pad2);
            pad2 -> AddNeighborPad(pad0);
          }
        }
      }
    }
  }

  for (Int_t iLayer = 0; iLayer < fLayerMax; iLayer++) {
    Int_t nHalfRows = fHalfRowMax[iLayer];
    for (Int_t iSection = 0; iSection < 8; iSection++) {
      std::vector<Int_t> key0;
      key0.push_back(iSection);
      key0.push_back(nHalfRows);
      key0.push_back(iLayer);
      KBPad *pad0 = (KBPad *) fChannelArray -> At(fPadMap[key0]);

      Int_t section1 = iSection+1;
      if (iSection == 7)
        section1 = 0;

      std::vector<Int_t> key1;
      key1.push_back(section1);
      key1.push_back(-nHalfRows);
      key1.push_back(iLayer);
      KBPad *pad1 = (KBPad *) fChannelArray -> At(fPadMap[key1]);

      pad0 -> AddNeighborPad(pad1);
      pad1 -> AddNeighborPad(pad0);
    }
  }

  return true;
}

Int_t LHPadPlaneRPad::FindPadID(Int_t section, Int_t row, Int_t layer)
{
  if (layer < 0 || layer >= fLayerMax)
    return -1;

  if (row == 0 || std::abs(row) > fHalfRowMax[layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Int_t LHPadPlaneRPad::FindPadID(Double_t i, Double_t j)
{
  Int_t section = FindSection(i,j);

  Double_t xRot =  i*fCosPiNo4[section] + j*fSinPiNo4[section];
  Double_t yRot = -i*fSinPiNo4[section] + j*fCosPiNo4[section];

  Double_t yFromMin = yRot - fYPPMin;
  if (yFromMin < 0)
    return -1;

  Int_t layer = (Int_t)(yFromMin/fYSpacing);
  if (yFromMin - layer*fYSpacing > fPadHei)
    return -1;

  Int_t pm = 1;
  if (xRot < 0) {
    xRot = -xRot;
    pm = -1;
  }

  if (xRot < .5*fPadGap)
    return -1;

  Double_t xFromMin = xRot - .5*fPadGap;
  Int_t row = (Int_t)(xFromMin/fXSpacing)+1;
  if (xFromMin - (row-1)*fXSpacing > fPadWid)
    return -1;

  if (row > fHalfRowMax[layer])
    return -1;

  std::vector<Int_t> key;
  key.push_back(section);
  key.push_back(pm*row);
  key.push_back(layer);

  Int_t id = fPadMap[key];

  return id;
}

Double_t LHPadPlaneRPad::PadDisplacement() const
{
  return 10;
}

bool LHPadPlaneRPad::IsInBoundary(Double_t i, Double_t j)
{
  Double_t r = TMath::Sqrt(i*i+j*j);
  if (r < fRMin || r > fRMax)
    return false;

  return true;
}

TH2* LHPadPlaneRPad::GetHist(Option_t *option)
{
  if (fH2Plane != nullptr)
    return fH2Plane; 

  TH2Poly *h2 = new TH2Poly();

  Double_t xPoints[6] = {0};
  Double_t yPoints[6] = {0};

  KBPad *pad;
  TIter iterPads(fChannelArray);
  Int_t selectSection = TString(option).Atoi();
  while ((pad = (KBPad *) iterPads.Next())) 
  {
    if(selectSection != -1 && selectSection != pad -> GetSection())
      continue;

    auto corners = pad -> GetPadCorners();

    Int_t numPoints = corners->size();
    for (auto iCorner = 0; iCorner < numPoints; ++iCorner)
    {
      TVector2 corner = corners->at(iCorner);
      xPoints[iCorner] = corner.X();
      yPoints[iCorner] = corner.Y();
    }
    TVector2 corner = corners->at(0);
    xPoints[numPoints] = corner.X();
    yPoints[numPoints] = corner.Y();

    h2 -> AddBin(numPoints+1, xPoints, yPoints);
  }

  if (true) {
    TList *bins = h2->GetBins(); TIter Next(bins);
    TObject *obj;
    TH2PolyBin *b;
    TGraph *g;
    while ((obj = Next())) {
      b = (TH2PolyBin*)obj;
      g = (TGraph*)b->GetPolygon();
      if (g) g->SetLineWidth(1);
      if (g) g->SetLineColor(kGray+2);
    }
  }

  fH2Plane = (TH2 *) h2;
  fH2Plane -> SetStats(0);
  fH2Plane -> SetTitle(";x (mm); y (mm)");
  fH2Plane -> GetXaxis() -> CenterTitle();
  fH2Plane -> GetYaxis() -> CenterTitle();

  return fH2Plane;
}

void LHPadPlaneRPad::DrawFrame(Option_t *)
{
  Color_t lineColor = kGray;

  TEllipse *circleOut = new TEllipse(0, 0, fRMax, fRMax);
  circleOut -> SetFillStyle(0);
  circleOut -> SetLineColor(lineColor);
  circleOut -> Draw("samel");

  TEllipse *circleIn = new TEllipse(0, 0, fRMin, fRMin);
  circleIn -> SetFillStyle(0);
  circleIn -> SetLineColor(lineColor);
  circleIn -> Draw("samel");

  TLine* line1 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*1/8),  fRMax*TMath::Sin(TMath::Pi()*1/8),
                fRMin*TMath::Cos(TMath::Pi()*1/8),  fRMin*TMath::Sin(TMath::Pi()*1/8));
  TLine* line2 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*3/8),  fRMax*TMath::Sin(TMath::Pi()*3/8),
                fRMin*TMath::Cos(TMath::Pi()*3/8),  fRMin*TMath::Sin(TMath::Pi()*3/8));
  TLine* line3 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*5/8),  fRMax*TMath::Sin(TMath::Pi()*5/8),
                fRMin*TMath::Cos(TMath::Pi()*5/8),  fRMin*TMath::Sin(TMath::Pi()*5/8));
  TLine* line4 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*7/8),  fRMax*TMath::Sin(TMath::Pi()*7/8),
                fRMin*TMath::Cos(TMath::Pi()*7/8),  fRMin*TMath::Sin(TMath::Pi()*7/8));
  TLine* line5 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*9/8),  fRMax*TMath::Sin(TMath::Pi()*9/8),
                fRMin*TMath::Cos(TMath::Pi()*9/8),  fRMin*TMath::Sin(TMath::Pi()*9/8));
  TLine* line6 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*11/8), fRMax*TMath::Sin(TMath::Pi()*11/8),
                fRMin*TMath::Cos(TMath::Pi()*11/8), fRMin*TMath::Sin(TMath::Pi()*11/8));
  TLine* line7 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*13/8), fRMax*TMath::Sin(TMath::Pi()*13/8),
                fRMin*TMath::Cos(TMath::Pi()*13/8), fRMin*TMath::Sin(TMath::Pi()*13/8));
  TLine* line8 
    = new TLine(fRMax*TMath::Cos(TMath::Pi()*15/8), fRMax*TMath::Sin(TMath::Pi()*15/8),
                fRMin*TMath::Cos(TMath::Pi()*15/8), fRMin*TMath::Sin(TMath::Pi()*15/8));

  line1 -> SetLineColor(lineColor);
  line2 -> SetLineColor(lineColor);
  line3 -> SetLineColor(lineColor);
  line4 -> SetLineColor(lineColor);
  line5 -> SetLineColor(lineColor);
  line6 -> SetLineColor(lineColor);
  line7 -> SetLineColor(lineColor);
  line8 -> SetLineColor(lineColor);

  line1 -> Draw("samel");
  line2 -> Draw("samel");
  line3 -> Draw("samel");
  line4 -> Draw("samel");
  line5 -> Draw("samel");
  line6 -> Draw("samel");
  line7 -> Draw("samel");
  line8 -> Draw("samel");
}

TCanvas *LHPadPlaneRPad::GetCanvas(Option_t *)
{
  if (fCanvas == nullptr)
    fCanvas = new TCanvas(fName+Form("%d",fPlaneID),fName,800,800);

  return fCanvas;
}

KBPad *LHPadPlaneRPad::AddPad(Int_t section, Int_t row, Int_t layer, Double_t i, Double_t j)
{
  KBPad *pad = new KBPad();
  pad -> SetPosition(i, j);
  pad -> SetSectionRowLayer(section, row, layer);
  fChannelArray -> Add(pad);

  return pad;
}

void LHPadPlaneRPad::MapPad(KBPad *pad)
{
  std::vector<Int_t> key;
  key.push_back(pad -> GetSection());
  key.push_back(pad -> GetRow());
  key.push_back(pad -> GetLayer());
  fPadMap.insert(std::pair<std::vector<Int_t>, Int_t>(key,pad->GetPadID()));
}

Int_t LHPadPlaneRPad::FindSection(Double_t i, Double_t j)
{
  if (j > fTanPi3o8*i) {
    if (j > fTanPi1o8*i) {
      if (j > fTanPi7o8*i) {
        if (j > fTanPi5o8*i) {
          return 0;
        } else return 1;
      } else return 2;
    } else return 3;
  }
  else
  {
    if (j < fTanPi1o8*i) {
      if (j < fTanPi7o8*i) {
        if (j < fTanPi5o8*i) {
          return 4;
        } else return 5;
      } else return 6;
    } else return 7;
  }
}
