// Supported with union (c) 2018 Union team

#ifndef __ZSTRING_H__VER3__
#define __ZSTRING_H__VER3__

namespace Gothic_II_Addon {

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
    zMEMPOOL_DECLARATION( zSTRING, 0x008CDC98 )

    zSTRING( char const* a0 )                                                     zCall( 0x004010C0 );
    zSTRING()                                                                     zCall( 0x00402AF0 );
    zSTRING( zSTRING const& )                                                     zCall( 0x00416500 );
    zSTRING( int )                                                                zCall( 0x00435870 );
    zSTRING( float, int )                                                         zCall( 0x00435970 );
    char& operator[]( unsigned int )                                              zCall( 0x00445A20 );
    zSTRING( double, int )                                                        zCall( 0x00454680 );
    zSTRING( unsigned long )                                                      zCall( 0x00461E90 );
    char* ToChar() const                                                          zCall( 0x004639D0 );
    zSTRING& Lower()                                                              zCall( 0x0046A9F0 );
    zSTRING& Upper()                                                              zCall( 0x0046AB00 );
    zSTRING& UpperFirstLowerRest()                                                zCall( 0x0046AC10 );
    zSTRING& Align( zTSTR_ALIGN, int, char )                                      zCall( 0x0046AD60 );
    zSTRING PickWord( unsigned int, zSTRING const&, zSTRING const& ) const        zCall( 0x0046AF80 );
    char const* PickWordPos( unsigned int, zSTRING const&, zSTRING const& ) const zCall( 0x0046B1D0 );
    zSTRING PickWord_Old( int, zSTRING const& ) const                             zCall( 0x0046B340 );
    int Insert( unsigned int, zSTRING const& )                                    zCall( 0x0046B400 );
    zSTRING Inserted( unsigned int, zSTRING const& ) const                        zCall( 0x0046B520 );
    int Overwrite( unsigned int, zSTRING const& )                                 zCall( 0x0046B6A0 );
    zSTRING Overwritten( unsigned int, zSTRING const& ) const                     zCall( 0x0046B890 );
    int DeleteRight( unsigned int )                                               zCall( 0x0046B990 );
    int Delete( unsigned int, unsigned long )                                     zCall( 0x0046BAA0 );
    zSTRING Deleted( unsigned int, unsigned long ) const                          zCall( 0x0046BBB0 );
    int Delete( zSTRING const&, zTSTR_KIND )                                      zCall( 0x0046BCF0 );
    zSTRING Deleted( zSTRING const&, zTSTR_KIND ) const                           zCall( 0x0046BEE0 );
    int Copy( unsigned int, unsigned long )                                       zCall( 0x0046BFC0 );
    zSTRING Copied( unsigned int, unsigned long ) const                           zCall( 0x0046C170 );
    int Copy( zSTRING const&, zTSTR_KIND const& )                                 zCall( 0x0046C2D0 );
    zSTRING Copied( zSTRING const&, zTSTR_KIND const& ) const                     zCall( 0x0046C550 );
    void TrimLeft( char )                                                         zCall( 0x0046C630 );
    void TrimRight( char )                                                        zCall( 0x0046C770 );
    int Search( int, char const*, unsigned int ) const                            zCall( 0x0046C920 );
    int SearchRev( zSTRING const&, unsigned int ) const                           zCall( 0x0046CA20 );
    bool __cdecl Sprintf( char const*, ... )                                      zCall( 0x0046CC60 );
    int Search( zSTRING const&, unsigned int ) const                              zCall( 0x00492680 );
    float ToFloat() const                                                         zCall( 0x004936C0 );
    zSTRING& operator =( char const* )                                            zCall( 0x004CFAF0 );
    zSTRING( char )                                                               zCall( 0x0051AC80 );
    zSTRING& operator =( zSTRING const& )                                         zCall( 0x0059CEB0 );
    void Clear()                                                                  zCall( 0x0059D010 );
    int Length() const                                                            zCall( 0x0059D0E0 );
    long ToInt() const                                                            zCall( 0x0059D0F0 );
    int Search( char const*, unsigned int ) const                                 zCall( 0x0059D110 );
    bool IsEmpty() const                                                          zCall( 0x00674210 );
    zSTRING& operator+= ( char const* )                                           zCall( 0x0067A7B0 );
    zSTRING( long )                                                               zCall( 0x006CFE50 );
    void Init()                                                                   zCall( 0x006D9B80 );
    zSTRING( zSTRING const* )                                                     zCall( 0x007928D0 );
    virtual ~zSTRING()                                                            zCall( 0x00401160 );  // scalar:0x00401140 vector:0x0041C760 destructor:0x00401160

    inline operator CString() {
      return ToChar();
    }
    inline operator const CString() const {
      return ToChar();
    }
  };

} // namespace Gothic_II_Addon

#endif // __ZSTRING_H__VER3__