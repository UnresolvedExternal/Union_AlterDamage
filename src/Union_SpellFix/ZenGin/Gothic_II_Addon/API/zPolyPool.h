// Supported with union (c) 2018 Union team

#ifndef __ZPOLY_POOL_H__VER3__
#define __ZPOLY_POOL_H__VER3__

#include "z3d.h"
#include "zVisual.h"

namespace Gothic_II_Addon {

  class zCVertexFeaturePool {
  public:
    GETSmallArrayNative<zCVertFeature>* Pool;
    GETSmallArrayNative<zCVertFeature*>* PtrPool;
    int NumOfAllocatedVertexFeatures;

    zCVertexFeaturePool() {}
  };

  class zCPolygonPool {
  public:
    GETSmallArrayNative<zCPolygon>* Pool;
    int NumOfAllocatedPolygon;

    zCPolygonPool() {}
  };

  class zCVertexPool {
  public:
    GETSmallArrayNative<zCVertex>* Pool;
    GETSmallArrayNative<zCVertex*>* PtrPool;
    int NumOfAllocatedVertex;

    zCVertexPool() {}
  };

  class zCMeshPool {
  public:
    zCVertexPool* vertexPool;
    zCPolygonPool* polygonPool;
    zCVertexFeaturePool* vertexFeaturePool;
    int meshIndex;
    zCMesh* meshObject;

    zCMeshPool() {}
  };

  class zCMeshesPool {
  public:
    int IsActive;
    GETSmallArrayNative<zCMeshPool*>* pool;

    zCMeshesPool() {}
  };

} // namespace Gothic_II_Addon

#endif // __ZPOLY_POOL_H__VER3__