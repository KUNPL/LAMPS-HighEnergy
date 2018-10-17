#include "KBRun.hh"
#include "KBHit.hh"
#include "KBTpcHit.hh"
#include "LHHitClustering.hh"
#include "LHPadPlane.hh"

using namespace std;

ClassImp(LHHitClustering)

LHHitClustering::LHHitClustering()
  :KBTask("LHHitClustering","")
{
}

bool LHHitClustering::Init()
{
  KBRun* run = KBRun::GetRun();
  fHit = (TClonesArray*) run->GetBranch("Hit");
  //fPad = (TClonesArray*) run->GetBranch("Pad");

  fHitClusterArray = new TClonesArray("KBHit");
  run->RegisterBranch("HitCluster", fHitClusterArray, true);
  fHitNClusterArray = new TClonesArray("KBHit");
  run->RegisterBranch("HitNCluster", fHitNClusterArray, true);

  return true;
}

void LHHitClustering::Exec(Option_t *)
{
  fHitClusterArray->Clear("C");
  fHitNClusterArray->Clear("C");

  Int_t nHits = fHit->GetEntriesFast();
  Int_t nClusters=0;
  Int_t nNClusters=0;


  Int_t HitPaired[3000][2]={{0},{0}};

  for(Int_t iHit=0; iHit<nHits; iHit++)
  {

    KBHit* hit = (KBHit*) fHit->At(iHit);
    auto hitchecking = (KBTpcHit*) fHit->At(iHit);
    auto padID = hitchecking->GetPadID();

    for(Int_t jHit=iHit; jHit<nHits; jHit++)
    {
      if(jHit==iHit) continue;

      KBHit* comHit = (KBHit*) fHit->At(jHit);
      auto distX = hit->GetX() - comHit->GetX();
      auto distY = hit->GetY() - comHit->GetY();
      auto distZ = hit->GetZ() - comHit->GetZ();
      auto r = sqrt(distX*distX + distY*distY + distZ*distZ);

      if(r<3.15) 
      {
        auto hitcluster = (KBHit*) fHitClusterArray->ConstructedAt(nClusters);
        //cout << r << endl;
        hitcluster->AddHit(comHit);
        hitcluster->SetHitID(nClusters);
        //hitcluster->SetTrackID(jHit);
        HitPaired[nClusters][0]=jHit;
        HitPaired[nClusters][1]=iHit;
        nClusters++;
      //cout << "Cluster : " << hitcluster->GetX() << endl;
      }
    }

  }
  kb_info << "Number of Clusterized : " << nClusters << endl;

  Int_t j=0;
  for(Int_t iHit=0; iHit<nHits; iHit++)
  {
    for(Int_t i=j; i<3000; i++)
    {
      if(iHit==HitPaired[i][0] || iHit==HitPaired[i][1]) 
      {
        j++;
        break;
      }

      auto hitNcluster = (KBHit*) fHitNClusterArray->ConstructedAt(nNClusters);
      KBHit* hit = (KBHit*) fHit->At(iHit);
      hitNcluster->SetHitID(hit->GetHitID());
      hitNcluster->SetTrackID(hit->GetHitID());
      hitNcluster->SetCharge(hit->GetCharge());
      hitNcluster->SetX(hit->GetX());
      hitNcluster->SetY(hit->GetY());
      hitNcluster->SetZ(hit->GetZ());
      nNClusters++;
      //cout << "NCluster : " << hitNcluster->GetX() << endl;
      break;
    }
  }


  nNClusters = 0;
  j=0;
  for(Int_t iHit=0; iHit<nHits; iHit++)
  {
    for(Int_t i=j; i<3000; i++)
    {
      if(iHit==HitPaired[i][0] || iHit==HitPaired[i][1]) 
      {
        j++;
        break;
      }

      auto hitcluster = (KBHit*) fHitClusterArray->ConstructedAt(nNClusters+nClusters+1);
      KBHit* hit = (KBHit*) fHit->At(iHit);
      hitcluster->AddHit(hit);
      nNClusters++;
      //cout << "NCluster : " << hitNcluster->GetX() << endl;
      break;
    }
  }
  kb_info << "Number of Non Clusterized : " << nNClusters << endl;
/*
  auto pad= new LHPadPlane();
  pad->SetParameterContainer("lamps.par");
  pad->Init();

  for(Int_t i=0; i<nHits; i++)
  {
    auto hitT = (KBTpcHit*) fHitClusterArray->At(i);
    //cout << "X : " << hitT->GetX() << "Z : " << hitT->GetZ() << endl;
    hitT->Print();
    hitT->SetPadID(pad->FindPadID(hitT->GetX(), hitT->GetZ()));
  }
*/
}
