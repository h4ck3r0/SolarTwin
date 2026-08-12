#ifndef RTW_HEADER_PV_Array_Model_private_h_
#define RTW_HEADER_PV_Array_Model_private_h_
#include "rtwtypes.h"
#include "builtin_typeid_types.h"
#include "multiword_types.h"
#include <math.h>
#include <stdlib.h>
#include "PV_Array_Model.h"
#if !defined(rt_VALIDATE_MEMORY)
#define rt_VALIDATE_MEMORY(S, ptr)   if(!(ptr)) {\
  ssSetErrorStatus(rtS, RT_MEMORY_ALLOCATION_ERROR);\
  }
#endif
#if !defined(rt_FREE)
#if !defined(_WIN32)
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((ptr));\
  (ptr) = (NULL);\
  }
#else
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((void *)(ptr));\
  (ptr) = (NULL);\
  }
#endif
#endif
#ifndef CodeFormat
#define CodeFormat   S-Function
#else
#undef CodeFormat
#define CodeFormat   S-Function
#endif
#ifndef S_FUNCTION_NAME
#define S_FUNCTION_NAME   simulink_only_sfcn
#else
#undef S_FUNCTION_NAME
#define S_FUNCTION_NAME   simulink_only_sfcn
#endif
#ifndef S_FUNCTION_LEVEL
#define S_FUNCTION_LEVEL  2
#else
#undef S_FUNCTION_LEVEL
#define S_FUNCTION_LEVEL  2
#endif
#ifndef RTW_GENERATED_S_FUNCTION
#define RTW_GENERATED_S_FUNCTION
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm)        NULL
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val)
#endif
#if !defined(RTW_SFUNCTION_DEFINES)
#define RTW_SFUNCTION_DEFINES
#ifndef _RTW_COMMON_DEFINES_
#define _RTW_COMMON_DEFINES_
#endif
#endif
#ifndef __RTW_UTFREE__
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
void * rt_TDelayCreateBuf ( int_T numBuffer , int_T bufSz , int_T elemSz ) ;
boolean_T rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T * tailPtr ,
int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * * tBufPtr ,
real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf , boolean_T
istransportdelay , int_T * maxNewBufSzPtr ) ; real_T rt_TDelayInterpolate (
real_T tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T
bufSz , int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput
, boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) ; extern
real_T look1_pbinlxpw ( real_T u0 , const real_T bp0 [ ] , const real_T table
[ ] , uint32_T prevIndex [ ] , uint32_T maxIndex ) ; extern void
sfun_discreteVariableDelay ( SimStruct * rts ) ; void rt_TDelayFreeBuf ( void
* buf ) ; extern void jspz1xohdo ( gj3ohc1kk3 * localB , fjazhlmyos * localP
) ; extern void fiy4znthvv ( uint8_T fmdbtw42m3 , const real_T c2ubopwwjd [ 2
] , real_T g2zrxgfvcs , gj3ohc1kk3 * localB , oqxlv1t1or * localDW ) ; extern
void brnjra0xun ( brzkpe5yck * localB , fcezepw4oj * localP ) ; extern void
flkfnl3dqu ( uint8_T pw0iil31u4 , const real_T hnlegfoeow [ 2 ] , real_T
jl3rqh3r3n , brzkpe5yck * localB , np452uldfz * localDW ) ; extern void
gj0ekbre2u ( SimStruct * rtS_i , real_T kwzh0lz0ar , creal_T p2ionxkoyh ,
creal_T cabkv5wct2 , creal_T hkgcxqxzgv , pij4s3w30k * localB , baqwe1p1zs *
localDW , m3uhrpfyin * localP ) ; extern void ihqnx345xu ( SimStruct * rtS_e
, real_T kix2j21ure , creal_T o3vpyt2sql , creal_T myolzlxrer , creal_T
egbcgmbq04 , pjkmgaolpi * localB , lkl4gg004l * localDW , pneeuxrdyc * localP
) ; extern void pvdad0deeo ( ghzael4l32 * localDW , pcr2y0buvk * localP ,
e0vkhe1hid * localX ) ; extern void h0ryf0cnmm ( SimStruct * rtS_g ,
ghzael4l32 * localDW , pcr2y0buvk * localP , mjc13vhdfp * localXdis ) ;
extern void jj4trrgi3n ( go1ko1exrf * localB , ghzael4l32 * localDW ,
bvo2qlqek0 * localXdot ) ; extern void nainrbgoka ( ghzael4l32 * localDW ) ;
extern void gxsaitrvqe ( SimStruct * rtS_f , go1ko1exrf * localB , ghzael4l32
* localDW , pcr2y0buvk * localP ) ; extern void kyedbo0mxu ( SimStruct *
rtS_j , boolean_T hmajr40tp3 , const real_T lghjdroza4 [ 3 ] , go1ko1exrf *
localB , ghzael4l32 * localDW , pcr2y0buvk * localP , e0vkhe1hid * localX ,
mjc13vhdfp * localXdis ) ; extern void i4sjz31q1b ( o3cgxb1nm5 * localDW ,
klg0lcyohc * localP , h1l3104bn0 * localX ) ; extern void cvm1yt3zx5 (
SimStruct * rtS_b , o3cgxb1nm5 * localDW , klg0lcyohc * localP , hlxxn2qtq3 *
localXdis ) ; extern void fdfvudi3q4 ( pq3igreacf * localB , o3cgxb1nm5 *
localDW , fyvjfwczbw * localXdot ) ; extern void kqyojj2hz0 ( pq3igreacf *
localB , o3cgxb1nm5 * localDW , klg0lcyohc * localP , c3runus3ia * localZCSV
) ; extern void fblsrs2ge4 ( o3cgxb1nm5 * localDW ) ; extern void pwyhxd4z2i
( SimStruct * rtS_f , pq3igreacf * localB , o3cgxb1nm5 * localDW , klg0lcyohc
* localP ) ; extern void gllp1vimnh ( SimStruct * rtS_c , boolean_T
bgwpfbivee , const real_T cgiiotktlo [ 3 ] , pq3igreacf * localB , o3cgxb1nm5
* localDW , klg0lcyohc * localP , h1l3104bn0 * localX , hlxxn2qtq3 *
localXdis ) ; extern void g5flx2hgit ( ghzael4l32 * localDW ) ; extern void
b0l2el5i05 ( o3cgxb1nm5 * localDW ) ;
#if defined(MULTITASKING)
#error Models using the variable step solvers cannot define MULTITASKING
#endif
#endif
