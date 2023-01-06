#pragma once

#include "platform.h"

#define BITSET(VAR,Place)         ( (VAR) |= (uint8_t)((uint8_t)1<<(uint8_t)(Place)) )
#define BITCLEAR(VAR,Place)         ( (VAR) &= (uint8_t)((uint8_t)((uint8_t)1<<(uint8_t)(Place))^(uint8_t)255) )

#define BITINV(VAR,Place)         ( (VAR) ^= (uint8_t)((uint8_t)1<<(uint8_t)(Place)) )
#define BITAFF(VAR,Place,Value)   ((Value) ? \
                                   ((VAR) |= ((uint8_t)1<<(Place))) : \
                                   ((VAR) &= (((uint8_t)1<<(Place))^(uint8_t)255)))
#define BITMSC(Dest,Msk,Src)      ( (Dest) = ((Msk) & (Src)) | ((~(Msk)) & (Dest)) )

#define BITVAL(VAR,Place)         ((uint8_t)(VAR) & (uint8_t)((uint8_t)1<<(uint8_t)(Place)))

#define BYTE_0(n)                 ((uint8_t)((n) & (uint8_t)0xFF))        /*!< Returns the low byte of the 32-bit value */
#define BYTE_1(n)                 ((uint8_t)(BYTE_0((n) >> (uint8_t)8)))  /*!< Returns the second byte of the 32-bit value */
#define BYTE_2(n)                 ((uint8_t)(BYTE_0((n) >> (uint8_t)16))) /*!< Returns the third byte of the 32-bit value */
#define BYTE_3(n)                 ((uint8_t)(BYTE_0((n) >> (uint8_t)24))) /*!< Returns the high byte of the 32-bit value */

// get high or low bytes from 2 byte integer
#define LOWBYTE(i)  ((uint8_t)(i))
#define HIGHBYTE(i) ((uint8_t)(((uint16_t)(i))>>8))

#define ARRAYLEN(_arr) (sizeof(_arr) / sizeof(_arr[0]))
#define ARRAYEND(x) (&(x)[ARRAYLEN(x)])

#define BIT(x) (1 << (x))

/*
http://resnet.uoregon.edu/~gurney_j/jmpc/bitwise.html
*/
#define BX_(x) ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))
#define BITCOUNT(x) (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)

// Some definitions for calculation
#define SEC_PER_MIN             60UL
#define SEC_PER_HOUR            3600UL
#define SEC_PER_DAY             86400UL
#define SEC_PER_YEAR            (SEC_PER_DAY*365)

// extracts 1..4 characters from a string and interprets it as a decimal value
#define CONV_STR2DEC_1(str, i)  (str[i]>'0'?str[i]-'0':0)
#define CONV_STR2DEC_2(str, i)  (CONV_STR2DEC_1(str, i)*10 + str[i+1]-'0')
#define CONV_STR2DEC_3(str, i)  (CONV_STR2DEC_2(str, i)*10 + str[i+2]-'0')
#define CONV_STR2DEC_4(str, i)  (CONV_STR2DEC_3(str, i)*10 + str[i+3]-'0')

// Custom "glue logic" to convert the month name to a usable number
#define GET_MONTH(str, i)      (str[i]=='J' && str[i+1]=='a' && str[i+2]=='n' ? 1 :     \
                                str[i]=='F' && str[i+1]=='e' && str[i+2]=='b' ? 2 :     \
                                str[i]=='M' && str[i+1]=='a' && str[i+2]=='r' ? 3 :     \
                                str[i]=='A' && str[i+1]=='p' && str[i+2]=='r' ? 4 :     \
                                str[i]=='M' && str[i+1]=='a' && str[i+2]=='y' ? 5 :     \
                                str[i]=='J' && str[i+1]=='u' && str[i+2]=='n' ? 6 :     \
                                str[i]=='J' && str[i+1]=='u' && str[i+2]=='l' ? 7 :     \
                                str[i]=='A' && str[i+1]=='u' && str[i+2]=='g' ? 8 :     \
                                str[i]=='S' && str[i+1]=='e' && str[i+2]=='p' ? 9 :     \
                                str[i]=='O' && str[i+1]=='c' && str[i+2]=='t' ? 10 :    \
                                str[i]=='N' && str[i+1]=='o' && str[i+2]=='v' ? 11 :    \
                                str[i]=='D' && str[i+1]=='e' && str[i+2]=='c' ? 12 : 0)

// extract the information from the time string given by __TIME__ and __DATE__
#define __TIME_SECONDS__        CONV_STR2DEC_2(__TIME__, 6)
#define __TIME_MINUTES__        CONV_STR2DEC_2(__TIME__, 3)
#define __TIME_HOURS__          CONV_STR2DEC_2(__TIME__, 0)
#define __TIME_DAYS__           CONV_STR2DEC_2(__DATE__, 4)
#define __TIME_MONTH__          GET_MONTH(__DATE__, 0)
#define __TIME_YEARS__          CONV_STR2DEC_4(__DATE__, 7)

// Days in February
#define _UNIX_TIMESTAMP_FDAY(year) \
    (((year) % 400) == 0UL ? 29UL : \
        (((year) % 100) == 0UL ? 28UL : \
            (((year) % 4) == 0UL ? 29UL : \
                28UL)))

// Days in the year
#define _UNIX_TIMESTAMP_YDAY(year, month, day) \
    ( \
        /* January */    day \
        /* February */ + (month >=  2 ? 31UL : 0UL) \
        /* March */    + (month >=  3 ? _UNIX_TIMESTAMP_FDAY(year) : 0UL) \
        /* April */    + (month >=  4 ? 31UL : 0UL) \
        /* May */      + (month >=  5 ? 30UL : 0UL) \
        /* June */     + (month >=  6 ? 31UL : 0UL) \
        /* July */     + (month >=  7 ? 30UL : 0UL) \
        /* August */   + (month >=  8 ? 31UL : 0UL) \
        /* September */+ (month >=  9 ? 31UL : 0UL) \
        /* October */  + (month >= 10 ? 30UL : 0UL) \
        /* November */ + (month >= 11 ? 31UL : 0UL) \
        /* December */ + (month >= 12 ? 30UL : 0UL) \
    )

// get the UNIX timestamp from a digits representation
#define _UNIX_TIMESTAMP(year, month, day, hour, minute, second) \
    ( /* time */ second \
                + minute * SEC_PER_MIN \
                + hour * SEC_PER_HOUR \
    + /* year day (month + day) */ (_UNIX_TIMESTAMP_YDAY(year, month, day) - 1) * SEC_PER_DAY \
    + /* year */ (year - 1970UL) * SEC_PER_YEAR \
                + ((year - 1969UL) / 4UL) * SEC_PER_DAY \
                - ((year - 1901UL) / 100UL) * SEC_PER_DAY \
                + ((year - 1601UL) / 400UL) * SEC_PER_DAY \
    )

// the UNIX timestamp
#define UNIX_TIMESTAMP (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__, __TIME_MINUTES__, __TIME_SECONDS__))
