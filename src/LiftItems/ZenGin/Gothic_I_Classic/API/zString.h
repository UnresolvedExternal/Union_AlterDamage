// Supported with union (c) 2018 Union team

#ifndef __ZSTRING_H__VER0__
#define __ZSTRING_H__VER0__

namespace Gothic_I_Classic {

  enum zTSTR_KIND {
    zSTR_ONLY,
    zSTR_BEFORE,
    zSTR_AFTER,
    zSTR_TO,
    zSTR_FROM
  };

  enum zTSTR_ALIGN {
    zSTR_RIGHT,
    zSTR_CENTER,
    zSTR_LEFT
  };

  class zSTRING {
    int allocater;
    char* vector;
    int length;
    int reserved;
  public:
    zMEMPOOL_DECLARATION( zSTRING, 0x008699A0 )

    void zSTRING_OnInit( char const* )                                            zCall( 0x004013A0 );
    void zSTRING_OnInit()                                                         zCall( 0x00402B30 );
    void zSTRING_OnInit( zSTRING const& )                                         zCall( 0x00415F40 );
    void zSTRING_OnInit( int )                                                    zCall( 0x00465810 );
    void zSTRING_OnInit( long )                                                   zCall( 0x00465910 );
    void zSTRING_OnInit( unsigned long )                                          zCall( 0x00465A10 );
    void zSTRING_OnInit( float, int )                                             zCall( 0x00465B10 );
    void zSTRING_OnInit( double, int )                                            zCall( 0x00465C60 );
    void zSTRING_OnInit( char )                                                   zCall( 0x0050AC70 );
    void zSTRING_OnInit( zSTRING const* )                                         zCall( 0x006E9570 );
    zSTRING( char const* a0 )                                                     zInit( zSTRING_OnInit( a0 ));
    zSTRING()                                                                     zInit( zSTRING_OnInit() );
    zSTRING( zSTRING const& a0 )                                                  zInit( zSTRING_OnInit( a0 ));
    zSTRING( int a0 )                                                             zInit( zSTRING_OnInit( a0 ));
    zSTRING( long a0 )                                                            zInit( zSTRING_OnInit( a0 ));
    zSTRING( unsigned long a0 )                                                   zInit( zSTRING_OnInit( a0 ));
    zSTRING( float a0, int a1 )                                                   zInit( zSTRING_OnInit( a0, a1 ));
    zSTRING( double a0, int a1 )                                                  zInit( zSTRING_OnInit( a0, a1 ));
    zSTRING( char a0 )                                                            zInit( zSTRING_OnInit( a0 ));
    zSTRING( zSTRING const* a0 )                                                  zInit( zSTRING_OnInit( a0 ));
    char& operator[]( unsigned int )                                              zCall( 0x00441AC0 );
    char* ToChar() const                                                          zCall( 0x0045E2E0 );
    zSTRING& Lower()                                                              zCall( 0x00465280 );
    zSTRING& Upper()                                                              zCall( 0x00465390 );
    zSTRING& UpperFirstLowerRest()                                                zCall( 0x004654A0 );
    zSTRING& Align( zTSTR_ALIGN, int, char )                                      zCall( 0x004655F0 );
    zSTRING PickWord( unsigned int, zSTRING const&, zSTRING const& ) const        zCall( 0x00465DB0 );
    char const* PickWordPos( unsigned int, zSTRING const&, zSTRING const& ) const zCall( 0x00466000 );
    zSTRING PickWord_Old( int, zSTRING const& ) const                             zCall( 0x00466170 );
    int Insert( unsigned int, zSTRING const& )                                    zCall( 0x00466230 );
    zSTRING Inserted( unsigned int, zSTRING const& ) const                        zCall( 0x00466350 );
    int Overwrite( unsigned int, zSTRING const& )                                 zCall( 0x004664D0 );
    zSTRING Overwritten( unsigned int, zSTRING const& ) const                     zCall( 0x004666C0 );
    int DeleteRight( unsigned int )                                               zCall( 0x004667C0 );
    int Delete( unsigned int, unsigned long )                                     zCall( 0x004668D0 );
    zSTRING Deleted( unsigned int, unsigned long ) const                          zCall( 0x004669E0 );
    int Delete( zSTRING const&, zTSTR_KIND )                                      zCall( 0x00466B20 );
    zSTRING Deleted( zSTRING const&, zTSTR_KIND ) const                           zCall( 0x00466D10 );
    int Copy( unsigned int, unsigned long )                                       zCall( 0x00466DF0 );
    zSTRING Copied( unsigned int, unsigned long ) const                           zCall( 0x00466FA0 );
    int Copy( zSTRING const&, zTSTR_KIND const& )                                 zCall( 0x00467100 );
    zSTRING Copied( zSTRING const&, zTSTR_KIND const& ) const                     zCall( 0x00467380 );
    void TrimLeft( char )                                                         zCall( 0x00467460 );
    void TrimRight( char )                                                        zCall( 0x004675A0 );
    int Search( int, char const*, unsigned int ) const                            zCall( 0x00467750 );
    int SearchRev( zSTRING const&, unsigned int ) const                           zCall( 0x00467850 );
    bool __cdecl Sprintf( char const*, ... )                                      zCall( 0x00467A90 );
    int Search( zSTRING const&, unsigned int ) const                              zCall( 0x00489C00 );
    zSTRING& operator =( char const* )                                            zCall( 0x004C5820 );
    zSTRING& operator =( zSTRING const& )                                         zCall( 0x0057E0C0 );
    void Clear()                                                                  zCall( 0x0057E220 );
    long ToInt() const                                                            zCall( 0x0057E2F0 );
    float ToFloat() const                                                         zCall( 0x0057E310 );
    int Search( char const*, unsigned int ) const                                 zCall( 0x0057E330 );
    zSTRING& operator+= ( char const* )                                           zCall( 0x00644ED0 );
    void Init()                                                                   zCall( 0x00737C50 );
    bool IsEmpty() const                                                          zCall( 0x0073F440 );
    virtual ~zSTRING()                                                            zCall( 0x00401260 );

    inline operator CString() {
      return ToChar();
    }
    inline operator const CString() const {
      return ToChar();
    }
  };

} // namespace Gothic_I_Classic

#endif // __ZSTRING_H__VER0__