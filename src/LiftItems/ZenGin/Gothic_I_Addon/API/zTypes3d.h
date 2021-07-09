// Supported with union (c) 2018 Union team

#ifndef __ZTYPES3D_H__VER1__
#define __ZTYPES3D_H__VER1__

namespace Gothic_I_Addon {
  const float float_MIN = float_MIN;
  const float float_MAX = float_MAX;

  enum zTPlaneClass {
    zPLANE_INFRONT,
    zPLANE_BEHIND,
    zPLANE_ONPLANE,
    zPLANE_SPANNING
  };

  struct zCOLOR {
    union {
      group {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char alpha;
      };
      unsigned long dword;
    };

    zCOLOR() {}
    void zCOLOR_OnInit( int )                                                              zCall( 0x004021F0 );
    void zCOLOR_OnInit( unsigned char, unsigned char, unsigned char, unsigned char )       zCall( 0x0071A280 );
    zCOLOR( int a0 )                                                                       zInit( zCOLOR_OnInit( a0 ));
    void Reset()                                                                           zCall( 0x00571AA0 );
    zVEC3 GetVEC3() const                                                                  zCall( 0x005B4050 );
    zSTRING GetDescription() const                                                         zCall( 0x005EE8C0 );
    void SetByDescription( zSTRING const& )                                                zCall( 0x005EEBC0 );
    zSTRING GetDescriptionRGB() const                                                      zCall( 0x005EEE90 );
    void SetRGB( unsigned char, unsigned char, unsigned char )                             zCall( 0x0061D320 );
    zCOLOR( unsigned char a0, unsigned char a1, unsigned char a2, unsigned char a3 = 255 ) zInit( zCOLOR_OnInit( a0, a1, a2, a3 ));
  };

  struct zTPlane {
    float distance;
    zVEC3 normal;

    zTPlane() {}
    int EqualTo( zTPlane const&, float ) const                        zCall( 0x005EF0F0 );
    int operator == ( zTPlane const& ) const                          zCall( 0x005EF150 );
    int Opposite( zTPlane const& ) const                              zCall( 0x005EF1A0 );
    int ClassifyPoint( zVEC3 const& ) const                           zCall( 0x005EF1E0 );
    int ClassifyPointBSPBuild( zVEC3 const& ) const                   zCall( 0x005EF230 );
    int CalcSignbits() const                                          zCall( 0x005EF280 );
    zSTRING Print() const                                             zCall( 0x005EF2E0 );
    int Intersection( zVEC3 const&, zVEC3 const&, zVEC3& ) const      zCall( 0x005EF650 );
    int IntersectionFront( zVEC3 const&, zVEC3 const&, zVEC3& ) const zCall( 0x005EF740 );
    void Calc( zVEC3 const&, zVEC3 const&, zVEC3 const& )             zCall( 0x005EF830 );
    void GetOriginUpRight( zVEC3&, zVEC3&, zVEC3& ) const             zCall( 0x005EF920 );
  };

} // namespace Gothic_I_Addon

#endif // __ZTYPES3D_H__VER1__