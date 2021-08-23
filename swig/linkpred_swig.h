
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
void GetRndWalkRestart(const PNEANet& SongUserNet,
                       const PNEANet& ArtistUserNet,
                       const PNEANet& ArtistSongNet,
                       double JumpProb,
                       double RandomHopProb,
                       double SongUserNetHopThresh,
                       double SongUserNetStayThresh,
                       double ArtistUserNetHopThresh,
                       double ArtistUserNetStayThresh,
                       double ArtistSongNetHopThresh,
                       double ArtistSongNetStayThresh,
                       const TIntV& StartNIdV,
                       TRnd& Rnd,
                       int N,
                       THash<TInt, TInt> &RwrNIdH){  
  for(int i = 0; i < N; i++){
    PNEANet Graph = SongUserNet;
    TStr netType = TStr("Song/User");
    int dislikes = 0;
    int locationId = StartNIdV.GetRndVal(Rnd);
    int latestSongId = -1;
    
    while (Rnd.GetUniDev() >= JumpProb){
      //Hop to random node in network
      if (Rnd.GetUniDev() >= RandomHopProb){
        locationId = Graph->GetRndNId();
        continue;
      }

      //Random neighbor or restart if no neighbor
      typename PNEANet::TObj::TNodeI location = Graph->GetNI(locationId);
      int d = location.GetOutDeg();
      if (d > 0){
        locationId = location.GetOutNId(Rnd.GetUniDevInt(d));
      } else {
        locationId = StartNIdV.GetRndVal(Rnd);
        continue;
      }
      
      //Save latest song id
      if (Graph->GetStrAttrDatN(locationId, "type") == TStr("song")){ 
        latestSongId = locationId;
      }
      
      int edgeId = Graph->GetEI(location.GetId(), locationId).GetId();
      TStr edgeType = Graph->GetStrAttrDatE(edgeId, "type");
      if (edgeType == TStr("dislike")){
        dislikes++;
      }
      
      //Network hopping
      double hopNet = Rnd.GetUniDev();
      if (netType == TStr("Song/User")){
        if (hopNet > SongUserNetStayThresh && hopNet > SongUserNetStayThresh && ArtistUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistUserNet;
          netType = TStr("Artist/User");
        } else if (hopNet > SongUserNetStayThresh && hopNet <= SongUserNetStayThresh && ArtistSongNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistSongNet;
          netType = TStr("Artist/Song");
        } else {
          Graph = SongUserNet;
          netType = TStr("Song/User");
        }
      } else if (netType == TStr("Artist/User")){
        if (hopNet > ArtistUserNetStayThresh && hopNet > ArtistUserNetStayThresh && SongUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = SongUserNet;
          netType = TStr("Song/User");
        } else if (hopNet > ArtistUserNetStayThresh && hopNet <= ArtistUserNetStayThresh && ArtistSongNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistSongNet;
          netType = TStr("Artist/Song");
        } else {
          Graph = ArtistUserNet;
          netType = TStr("Artist/User");
        }
      } else if (netType == TStr("Artist/Song")){
        if (hopNet > ArtistSongNetStayThresh && hopNet > ArtistSongNetStayThresh && ArtistUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistUserNet;
          netType = TStr("Artist/User");
        } else if (hopNet > ArtistSongNetStayThresh && hopNet <= ArtistSongNetStayThresh && SongUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = SongUserNet;
          netType = TStr("Song/User");
        } else {
          Graph = ArtistSongNet;
          netType = TStr("Artist/Song");
        }
      }
    }
  
    if (latestSongId == -1) continue;

    int score = dislikes % 2 ? -1 : 1;
    if (!RwrNIdH.IsKey(latestSongId)) 
      RwrNIdH.AddDat(latestSongId, score);
    else
      RwrNIdH.AddDat(latestSongId, RwrNIdH.GetDat(latestSongId) + score);
  }
}

/*template <class PGraph>
void GetRndWalkRestart(const PNEANet& SongUserNet,
                       const PNEANet& ArtistUserNet,
                       const PNEANet& ArtistSongNet,
                       double JumpProb,
                       double RandomHopProb,
                       double SongUserNetHopThresh,
                       double SongUserNetStayThresh,
                       double ArtistUserNetHopThresh,
                       double ArtistUserNetStayThresh,
                       double ArtistSongNetHopThresh,
                       double ArtistSongNetStayThresh,
                       const TIntV& StartNIdV,
                       const TRnd& Rnd,
                       int N,
                       THash<TInt, TInt> &RwrNIdH){  
  for(int i = 0; i < N; i++){
    PNEANet Graph = SongUserNet;
    TStr netType = TStr("Song/User");
    int dislikes = 0;
    int locationId = StartNIdV.GetRndVal(Rnd);
    int latestSongId = -1;
    
    while (Rnd.GetUniDev() >= JumpProb){
      //Hop to random node in network
      if (Rnd.GetUniDev() >= RandomHopProb){
        locationId = Graph->GetRndNId();
        continue;
      }

      //Random neighbor or restart if no neighbor
      typename PNEANet::TObj::TNodeI location = Graph->GetNI(locationId);
      int d = location.GetOutDeg();
      if (d > 0){
        locationId = location.GetOutNId(Rnd.GetUniDevInt(d));
      } else {
        locationId = StartNIdV.GetRndVal(Rnd);
        continue;
      }
      
      //Save latest song id
      if (Graph->GetStrAttrDatN(locationId, "type") == TStr("song")){ 
        latestSongId = locationId;
      }
      
      int edgeId = Graph->GetEI(location.GetId(), locationId).GetId();
      TStr edgeType = Graph->GetStrAttrDatE(edgeId, "type");
      if (edgeType == TStr("dislike")){
        dislikes++;
      }
      
      //Network hopping
      double hopNet = Rnd.GetUniDev();
      if (netType == TStr("Song/User")){
        if (hopNet > SongUserNetStayThresh && hopNet > SongUserNetStayThresh && ArtistUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistUserNet;
          netType = TStr("Artist/User");
        } else if (hopNet > SongUserNetStayThresh && hopNet <= SongUserNetStayThresh && ArtistSongNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistSongNet;
          netType = TStr("Artist/Song");
        } else {
          Graph = SongUserNet;
          netType = TStr("Song/User");
        }
      } else if (netType == TStr("Artist/User")){
        if (hopNet > ArtistUserNetStayThresh && hopNet > ArtistUserNetStayThresh && SongUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = SongUserNet;
          netType = TStr("Song/User");
        } else if (hopNet > ArtistUserNetStayThresh && hopNet <= ArtistUserNetStayThresh && ArtistSongNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistSongNet;
          netType = TStr("Artist/Song");
        } else {
          Graph = ArtistUserNet;
          netType = TStr("Artist/User");
        }
      } else if (netType == TStr("Artist/Song")){
        if (hopNet > ArtistSongNetStayThresh && hopNet > ArtistSongNetStayThresh && ArtistUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = ArtistUserNet;
          netType = TStr("Artist/User");
        } else if (hopNet > ArtistSongNetStayThresh && hopNet <= ArtistSongNetStayThresh && SongUserNet->GetNI(locationId).GetOutDeg() > 0){
          Graph = SongUserNet;
          netType = TStr("Song/User");
        } else {
          Graph = ArtistSongNet;
          netType = TStr("Artist/Song");
        }
      }
    }
  
    if (latestSongId == -1) continue;

    int score = dislikes % 2 ? -1 : 1;
    if (!RwrNIdH.IsKey(latestSongId)) 
      RwrNIdH.AddDat(latestSongId, score);
    else
      RwrNIdH.AddDat(latestSongId, RwrNIdH.GetDat(latestSongId) + score);
  }
}*/
