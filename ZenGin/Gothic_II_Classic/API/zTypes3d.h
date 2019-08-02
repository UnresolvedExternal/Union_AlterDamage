// Supported with union (c) 2018 Union team

#ifndef __ZTYPES3D_H__VER2__
#define __ZTYPES3D_H__VER2__

namespace Gothic_II_Classic {
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
    void zCOLOR_OnInit( int )                                                              zCall( 0x00401F20 );
    void zCOLOR_OnInit( unsigned char, unsigned char, unsigned char, unsigned char )       zCall( 0x0072B4B0 );
    zCOLOR( int a0 )                                                                       zInit( zCOLOR_OnInit( a0 ));
    void Reset()                                                                           zCall( 0x0056D650 );
    zSTRING GetDescription() const                                                         zCall( 0x005F3DB0 );
    void SetByDescription( zSTRING const& )                                                zCall( 0x005F4120 );
    zSTRING GetDescriptionRGB() const                                                      zCall( 0x005F43A0 );
    void SetRGB( unsigned char, unsigned char, unsigned char )						       zCall( 0x00622FC0 );
    zCOLOR( unsigned char a0, unsigned char a1, unsigned char a2, unsigned char a3 = 255 ) zInit( zCOLOR_OnInit( a0, a1, a2, a3 ));
  };

  struct zTPlane {
    float distance;
    zVEC3 normal;

    zTPlane() {}
    int CalcSignbits() const                              zCall( 0x00545F80 );
    int ClassifyPointBSPBuild( zVEC3 const& ) const       zCall( 0x005F4650 );
    zSTRING Print() const                                 zCall( 0x005F46A0 );
    void Calc( zVEC3 const&, zVEC3 const&, zVEC3 const& ) zCall( 0x005F4A80 );
    void GetOriginUpRight( zVEC3&, zVEC3&, zVEC3& ) const zCall( 0x005F4B60 );
  };

} // namespace Gothic_II_Classic

#endif // __ZTYPES3D_H__VER2__