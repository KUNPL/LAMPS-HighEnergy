#ifndef LAMPSTPC_HH
#define LAMPSTPC_HH

#include "KBTpc.hh"

class LHTpc : public KBTpc
{
  public:
    LHTpc();
    virtual ~LHTpc() {};

  protected:
    virtual bool BuildGeometry();
    virtual bool BuildDetectorPlane();

  ClassDef(LHTpc, 1)
};

#endif
