// Supported with union (c) 2018 Union team

#ifndef __ZPRIME_H__VER0__
#define __ZPRIME_H__VER0__

namespace Gothic_I_Classic {

  class zCPrime {
  public:

    zCPrime() {}
    static int IsPrime( unsigned long )             zCall( 0x0059DCF0 );
    static unsigned short PrimeNo( unsigned short ) zCall( 0x0059DDC0 );
    static unsigned long NextPrime( unsigned long ) zCall( 0x0059DED0 );
  };

} // namespace Gothic_I_Classic

#endif // __ZPRIME_H__VER0__