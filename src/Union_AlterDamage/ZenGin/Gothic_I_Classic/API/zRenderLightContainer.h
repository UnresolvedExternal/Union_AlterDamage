// Supported with union (c) 2018 Union team

#ifndef __ZRENDER_LIGHT_CONTAINER_H__VER0__
#define __ZRENDER_LIGHT_CONTAINER_H__VER0__

namespace Gothic_I_Classic {
  const int zRENDERLIGHT_LIGHT_MAX = 8;

  class zCRenderLightContainer {
  public:

    struct zTLightScore {
      int index;
      float score;

      zTLightScore() {}
    };

    zCRenderLight lightList[zRENDERLIGHT_LIGHT_MAX];
    int numLights;
    int doPrelight;
    zMAT4 trafoObjToCam;

    zCRenderLightContainer() {}
    ~zCRenderLightContainer()                                          zCall( 0x005609F0 );
    void CreatePrelightConfig( zTBBox3D const& )                       zCall( 0x005AF3A0 );
    int __fastcall CollectLights_FastPath( zTRenderContext const& )    zCall( 0x005AF550 );
    void __fastcall CollectLights_DynLights( zTRenderContext const& )  zCall( 0x005AF7A0 );
    void __fastcall CollectLights_StatLights( zTRenderContext const& ) zCall( 0x005AFCF0 );
    void __fastcall CollectLights( zTRenderContext const& )            zCall( 0x005B0160 );
    zVEC3 CalcStatLightDir( zTRenderContext const& )                   zCall( 0x005B02B0 );
    zVEC3 LightVertex( zVEC3 const&, zVEC3 const& ) const              zCall( 0x005B0420 );
    void __fastcall Activate( zMAT4 const& )                           zCall( 0x005B0680 );
    void SetupRendererLights()                                         zCall( 0x005B07A0 );
    static int CompareLightScore( void const*, void const* )           zCall( 0x005AF380 );
    static void S_ReleaseVobLightRefs()                                zCall( 0x005AF790 );
  };

} // namespace Gothic_I_Classic

#endif // __ZRENDER_LIGHT_CONTAINER_H__VER0__