// Supported with union (c) 2018 Union team

#ifndef __ZTYPES3D_H__VER0__
#define __ZTYPES3D_H__VER0__

namespace Gothic_I_Classic {
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
    void zCOLOR_OnInit( int )                                                              zCall( 0x00401DE0 );
    void zCOLOR_OnInit( unsigned char, unsigned char, unsigned char, unsigned char )       zCall( 0x006E1BC0 );
    zCOLOR( int a0 )                                                                       zInit( zCOLOR_OnInit( a0 ));
    void Reset()                                                                           zCall( 0x00559960 );
    zSTRING GetDescription() const                                                         zCall( 0x005CF9E0 );
    void SetByDescription( zSTRING const& )                                                zCall( 0x005CFC60 );
    zSTRING GetDescriptionRGB() const                                                      zCall( 0x005CFEE0 );
    void SetRGB( unsigned char, unsigned char, unsigned char )                             zCall( 0x005FBB40 );
    zCOLOR( unsigned char a0, unsigned char a1, unsigned char a2, unsigned char a3 = 255 ) zInit( zCOLOR_OnInit( a0, a1, a2, a3 ));
  };

  struct zTPlane {
    float distance;
    zVEC3 normal;

    zTPlane() {}
    int EqualTo( zTPlane const&, float ) const                        zCall( 0x005D00F0 );
    int operator == ( zTPlane const& ) const                          zCall( 0x005D0150 );
    int Opposite( zTPlane const& ) const                              zCall( 0x005D01A0 );
    int ClassifyPoint( zVEC3 const& ) const                           zCall( 0x005D01E0 );
    int ClassifyPointBSPBuild( zVEC3 const& ) const                   zCall( 0x005D0230 );
    int CalcSignbits() const                                          zCall( 0x005D0280 );
    zSTRING Print() const                                             zCall( 0x005D02E0 );
    int Intersection( zVEC3 const&, zVEC3 const&, zVEC3& ) const      zCall( 0x005D05C0 );
    int IntersectionFront( zVEC3 const&, zVEC3 const&, zVEC3& ) const zCall( 0x005D0690 );
    void Calc( zVEC3 const&, zVEC3 const&, zVEC3 const& )             zCall( 0x005D0770 );
    void GetOriginUpRight( zVEC3&, zVEC3&, zVEC3& ) const             zCall( 0x005D0850 );
  };

} // namespace Gothic_I_Classic

#endif // __ZTYPES3D_H__VER0__