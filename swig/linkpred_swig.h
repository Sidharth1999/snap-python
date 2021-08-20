
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

/*
The graph is effectively undirected - that means traditional 'dead-ends' cannot occur. Therefore, can infer the following (equivalent) statements:
1)An isolated node has to be a user node
2)As isolated node can only occur at the start node
3)A random walk cannot reach a dead-end at a song node
*/
template <class PGraph>
void GetRndWalkRestart(const PNEANet &Graph, double JumpProb, double RandomHopProb, const TIntV &StartNIdV, TRnd &Rnd, int N, THash<TInt, TInt> &RwrNIdH)
{
  for (int i = 0; i < N; i++)
  {
    int dislikes = 0;
    int locationId = StartNIdV.GetRndVal(Rnd);
    while (Rnd.GetUniDev() >= JumpProb)
    {
      if (Rnd.GetUniDev() >= RandomHopProb)
      {
        locationId = Graph->GetRndNId();
        continue;
      }
      
      bool resetted = false;
      typename PNEANet::TObj::TNodeI location = Graph->GetNI(locationId);
      int d = location.GetOutDeg();
      if (d > 0)
      {
        locationId = location.GetOutNId(Rnd.GetUniDevInt(d));
      }
      else
      {
        resetted = true
        locationId = StartNIdV.GetRndVal(Rnd);
      }
      
      if(!resetted) //then an edge was travelled
      {
        int edgeId = Graph->GetEI(location.GetId(), locationId).GetId();
        TStr edgeType = Graph->GetStrAttrDatE(edgeId, "type");
        if(edgeType() == "dislike")
        {
          dislikes++;
        }
      }
      
    }
    
    int score = dislikes % 2 ? -1 : 1;
    if (!RwrNIdH.IsKey(locationId))
    {
       RwrNIdH.AddDat(locationId, score);
    }
    else
    {
       RwrNIdH.AddDat(locationId, RwrNIdH.GetDat(locationId) + score);
    } 
  }
}
