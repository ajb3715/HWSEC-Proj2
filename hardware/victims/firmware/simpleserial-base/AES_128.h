/*
 * AES_128.h
 *
 */

 #ifndef AES_128_H_
 #define AES_128_H_
 
 #include "stdio.h"
 #include "string.h"
 #include <stdint.h>
 
 #define first(x) (( unsigned char )( x >> 24))
 #define second( x ) (( unsigned char ) ( x >> 16) )
 #define third( x ) (( unsigned char ) ( x >> 8) )
 #define fourth( x ) (( unsigned char ) ( x ) )
 #define toFirst( x ) ((( unsigned char ) x ) << 24)
 #define toSecond( x ) ((( unsigned char ) x ) << 16)
 #define toThird( x ) ((( unsigned char ) x ) << 8)
 #define toFourth( x ) ((( unsigned char ) x ) )
 extern const unsigned char SBox[256];
 extern const unsigned char xTime2[256];
 extern const unsigned char xTime3[256];
 void ExpandKey (unsigned char Key[][4], unsigned char Expanded_key[][4][4]);
 void AddRoundKey (unsigned char Key[][4], unsigned char StateArray[][4]);
 void AddMatrices( unsigned char (*Mat1)[4], unsigned char (*Mat2)[4], unsigned char(*res)[4]);
 void SubBytes (unsigned char StateArray[][4]);
 void ShiftRows (unsigned char Matrix[][4]);
 void MixColumns (unsigned char S_Array[][4]);
 uint32_t Cat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
 #endif 
 