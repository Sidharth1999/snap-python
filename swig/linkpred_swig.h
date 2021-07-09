
// random walk with restars to node JumpNId
/*void GetRndWalkRestart(const PGraph& Graph, const double& JumpProb, const int& JumpNId, THash<TInt, TFlt>& RwrNIdH) {
  const double DefVal = 1.0/Graph->GetNodes();
  RwrNIdH.Clr(false);
  TIntH NIdOutDegH;
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    RwrNIdH.AddDat(NI.GetId(), DefVal);
    NIdOutDegH.AddDat(NI.GetId(), NI.GetOutDeg());
  }
  THash<TInt, TFlt> RwrNIdH2(Graph->GetNodes());
  for (int iter = 0; iter < 10; iter++) {
    double Sum = 0;
    for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
      double Val = 0;
      for (int i = 0; i < NI.GetInDeg(); i++) {
        const int InId = NI.GetInNId(i);
        Val += (1.0-JumpProb) * 1.0/(NIdOutDegH.GetDat(InId)) * RwrNIdH.GetDat(InId);
      }
      if (NI.GetId() == JumpNId) { Val+= JumpProb; }
      RwrNIdH.AddDat(NI.GetId(), Val);
      Sum += Val;
    }
    for (int i = 0; i < RwrNIdH.Len(); i++) {
      RwrNIdH[i] /= Sum;
    }
  }
}*/
/*int SamplePersonalizedPageRank(const PGraph &Graph, double JumpProb, const TIntV &StartNIdV, TRnd &Rnd)
{
  int locationId = StartNIdV.GetRndVal(Rnd);
  //printf("starting walk at %d\n", locationId);
  while (Rnd.GetUniDev() >= JumpProb)
  {
    TNGraph::TNodeI location = Graph->GetNI(locationId);
    int d = location.GetOutDeg();
    if (d > 0)
      locationId = location.GetOutNId(Rnd.GetUniDevInt(d));
    else
      locationId = StartNIdV.GetRndVal(Rnd);
  }
  return locationId;
}*/
template <class PGraph>
void GetRndWalkRestart(const PGraph &Graph, double JumpProb, const TIntV &StartNIdV, TRnd &Rnd, int N, THash<TInt, TInt> &RwrNIdH)
{
  for (int i = 0; i < N; i++)
  {
    int locationId = StartNIdV.GetRndVal(Rnd);
    //printf("starting walk at %d\n", locationId);
    while (Rnd.GetUniDev() >= JumpProb)
    {
      typename PGraph::TObj::TNodeI location = Graph->GetNI(locationId);
      int d = location.GetOutDeg();
      if (d > 0)
        locationId = location.GetOutNId(Rnd.GetUniDevInt(d));
      else
        locationId = StartNIdV.GetRndVal(Rnd);
    }
    if (!RwrNIdH.IsKey(locationId))
    {
      RwrNIdH.AddDat(locationId, 1);
    }
    else
    {
      //RwrNIdH.AddDat(locationId, 1);
      RwrNIdH[locationId] += 1;
    }
  }
}

/*template <class PGraph>
void GetRndWalkTopN(const PGraph &Graph, double JumpProb, int N, THash<TInt, TFlt> &RwsNIdH, THash<TInt, TInt> &RwrNIdH)
{
  return;
}*/
