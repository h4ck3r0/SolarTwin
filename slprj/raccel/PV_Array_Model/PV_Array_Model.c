#include "rt_logging_mmi.h"
#include "PV_Array_Model_capi.h"
#include <math.h>
#include "PV_Array_Model.h"
#include "PV_Array_Model_private.h"
#include "PV_Array_Model_dt.h"
extern void * CreateDiagnosticAsVoidPtr_wrapper ( const char * id , int nargs
, ... ) ; RTWExtModeInfo * gblRTWExtModeInfo = NULL ; extern boolean_T
gblExtModeStartPktReceived ; void raccelForceExtModeShutdown ( ) { if ( !
gblExtModeStartPktReceived ) { boolean_T stopRequested = false ;
rtExtModeWaitForStartPkt ( gblRTWExtModeInfo , 4 , & stopRequested ) ; }
rtExtModeShutdown ( 4 ) ; }
#include "slsv_diagnostic_codegen_c_api.h"
const int_T gblNumToFiles = 0 ; const int_T gblNumFrFiles = 0 ; const int_T
gblNumFrWksBlocks = 0 ;
#ifdef RSIM_WITH_SOLVER_MULTITASKING
boolean_T gbl_raccel_isMultitasking = 1 ;
#else
boolean_T gbl_raccel_isMultitasking = 0 ;
#endif
boolean_T gbl_raccel_tid01eq = 0 ; int_T gbl_raccel_NumST = 5 ; const char_T
* gbl_raccel_Version = "10.1 (R2020a) 18-Nov-2019" ; void
raccel_setup_MMIStateLog ( SimStruct * S ) {
#ifdef UseMMIDataLogging
rt_FillStateSigInfoFromMMI ( ssGetRTWLogInfo ( S ) , & ssGetErrorStatus ( S )
) ;
#else
UNUSED_PARAMETER ( S ) ;
#endif
} static DataMapInfo rt_dataMapInfo ; DataMapInfo * rt_dataMapInfoPtr = &
rt_dataMapInfo ; rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr = & (
rt_dataMapInfo . mmi ) ; const char * gblSlvrJacPatternFileName =
"slprj\\raccel\\PV_Array_Model\\PV_Array_Model_Jpattern.mat" ; const int_T
gblNumRootInportBlks = 0 ; const int_T gblNumModelInputs = 0 ; extern
rtInportTUtable * gblInportTUtables ; extern const char * gblInportFileName ;
extern void * gblAperiodicPartitionHitTimes ; const int_T
gblInportDataTypeIdx [ ] = { - 1 } ; const int_T gblInportDims [ ] = { - 1 }
; const int_T gblInportComplex [ ] = { - 1 } ; const int_T
gblInportInterpoFlag [ ] = { - 1 } ; const int_T gblInportContinuous [ ] = {
- 1 } ; int_T enableFcnCallFlag [ ] = { 1 , 1 , 1 , 1 , 1 } ; const char *
raccelLoadInputsAndAperiodicHitTimes ( const char * inportFileName , int *
matFileFormat ) { return rt_RapidReadInportsMatFile ( inportFileName ,
matFileFormat , 1 ) ; }
#include "simstruc.h"
#include "fixedpoint.h"
const real_T PV_Array_Model_RGND = 0.0 ; B rtB ; X rtX ; DW rtDW ; static
SimStruct model_S ; SimStruct * const rtS = & model_S ;
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ;
#endif
void * rt_TDelayCreateBuf ( int_T numBuffer , int_T bufSz , int_T elemSz ) {
return ( ( void * ) utMalloc ( numBuffer * bufSz * elemSz ) ) ; }
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr , int_T * tailPtr ,
int_T * headPtr , int_T * lastPtr , real_T tMinusDelay , real_T * * tBufPtr ,
real_T * * uBufPtr , real_T * * xBufPtr , boolean_T isfixedbuf , boolean_T
istransportdelay , int_T * maxNewBufSzPtr ) { int_T testIdx ; int_T tail = *
tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf = * tBufPtr ; real_T *
xBuf = ( NULL ) ; int_T numBuffer = 2 ; if ( istransportdelay ) { numBuffer =
3 ; xBuf = * xBufPtr ; } testIdx = ( tail < ( bufSz - 1 ) ) ? ( tail + 1 ) :
0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] ) && ! isfixedbuf ) { int_T j ;
real_T * tempT ; real_T * tempU ; real_T * tempX = ( NULL ) ; real_T * uBuf =
* uBufPtr ; int_T newBufSz = bufSz + 1024 ; if ( newBufSz > * maxNewBufSzPtr
) { * maxNewBufSzPtr = newBufSz ; } tempU = ( real_T * ) utMalloc ( numBuffer
* newBufSz * sizeof ( real_T ) ) ; if ( tempU == ( NULL ) ) { return ( false
) ; } tempT = tempU + newBufSz ; if ( istransportdelay ) tempX = tempT +
newBufSz ; for ( j = tail ; j < bufSz ; j ++ ) { tempT [ j - tail ] = tBuf [
j ] ; tempU [ j - tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j -
tail ] = xBuf [ j ] ; } for ( j = 0 ; j < tail ; j ++ ) { tempT [ j + bufSz -
tail ] = tBuf [ j ] ; tempU [ j + bufSz - tail ] = uBuf [ j ] ; if (
istransportdelay ) tempX [ j + bufSz - tail ] = xBuf [ j ] ; } if ( * lastPtr
> tail ) { * lastPtr -= tail ; } else { * lastPtr += ( bufSz - tail ) ; } *
tailPtr = 0 ; * headPtr = bufSz ; utFree ( uBuf ) ; * bufSzPtr = newBufSz ; *
tBufPtr = tempT ; * uBufPtr = tempU ; if ( istransportdelay ) * xBufPtr =
tempX ; } else { * tailPtr = testIdx ; } return ( true ) ; } real_T
rt_TDelayInterpolate ( real_T tMinusDelay , real_T tStart , real_T * tBuf ,
real_T * uBuf , int_T bufSz , int_T * lastIdx , int_T oldestIdx , int_T
newIdx , real_T initOutput , boolean_T discrete , boolean_T
minorStepAndTAtLastMajorOutput ) { int_T i ; real_T yout , t1 , t2 , u1 , u2
; if ( ( newIdx == 0 ) && ( oldestIdx == 0 ) && ( tMinusDelay > tStart ) )
return initOutput ; if ( tMinusDelay <= tStart ) return initOutput ; if ( (
tMinusDelay <= tBuf [ oldestIdx ] ) ) { if ( discrete ) { return ( uBuf [
oldestIdx ] ) ; } else { int_T tempIdx = oldestIdx + 1 ; if ( oldestIdx ==
bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [ oldestIdx ] ; t2 = tBuf [ tempIdx ] ;
u1 = uBuf [ oldestIdx ] ; u2 = uBuf [ tempIdx ] ; if ( t2 == t1 ) { if (
tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else { real_T f1 =
( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout = f1 * u1 +
f2 * u2 ; } return yout ; } } if ( minorStepAndTAtLastMajorOutput ) { if (
newIdx != 0 ) { if ( * lastIdx == newIdx ) { ( * lastIdx ) -- ; } newIdx -- ;
} else { if ( * lastIdx == newIdx ) { * lastIdx = bufSz - 1 ; } newIdx =
bufSz - 1 ; } } i = * lastIdx ; if ( tBuf [ i ] < tMinusDelay ) { while (
tBuf [ i ] < tMinusDelay ) { if ( i == newIdx ) break ; i = ( i < ( bufSz - 1
) ) ? ( i + 1 ) : 0 ; } } else { while ( tBuf [ i ] >= tMinusDelay ) { i = (
i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0
; } * lastIdx = i ; if ( discrete ) { double tempEps = ( DBL_EPSILON ) *
128.0 ; double localEps = tempEps * muDoubleScalarAbs ( tBuf [ i ] ) ; if (
tempEps > localEps ) { localEps = tempEps ; } localEps = localEps / 2.0 ; if
( tMinusDelay >= ( tBuf [ i ] - localEps ) ) { yout = uBuf [ i ] ; } else {
if ( i == 0 ) { yout = uBuf [ bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ;
} } } else { if ( i == 0 ) { t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1
] ; } else { t1 = tBuf [ i - 1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ;
u2 = uBuf [ i ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; }
else { yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 )
; real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } } return ( yout ) ; }
real_T look1_pbinlxpw ( real_T u0 , const real_T bp0 [ ] , const real_T table
[ ] , uint32_T prevIndex [ ] , uint32_T maxIndex ) { real_T frac ; uint32_T
iRght ; uint32_T iLeft ; uint32_T bpIdx ; uint32_T found ; if ( u0 <= bp0 [
0U ] ) { bpIdx = 0U ; frac = ( u0 - bp0 [ 0U ] ) / ( bp0 [ 1U ] - bp0 [ 0U ]
) ; } else if ( u0 < bp0 [ maxIndex ] ) { bpIdx = prevIndex [ 0U ] ; iLeft =
0U ; iRght = maxIndex ; found = 0U ; while ( found == 0U ) { if ( u0 < bp0 [
bpIdx ] ) { iRght = bpIdx - 1U ; bpIdx = ( iRght + iLeft ) >> 1U ; } else if
( u0 < bp0 [ bpIdx + 1U ] ) { found = 1U ; } else { iLeft = bpIdx + 1U ;
bpIdx = ( iRght + iLeft ) >> 1U ; } } frac = ( u0 - bp0 [ bpIdx ] ) / ( bp0 [
bpIdx + 1U ] - bp0 [ bpIdx ] ) ; } else { bpIdx = maxIndex - 1U ; frac = ( u0
- bp0 [ maxIndex - 1U ] ) / ( bp0 [ maxIndex ] - bp0 [ maxIndex - 1U ] ) ; }
prevIndex [ 0U ] = bpIdx ; return ( table [ bpIdx + 1U ] - table [ bpIdx ] )
* frac + table [ bpIdx ] ; }
#ifndef __RTW_UTFREE__  
extern void utFree ( void * ) ;
#endif
void rt_TDelayFreeBuf ( void * buf ) { utFree ( buf ) ; } void jspz1xohdo (
gj3ohc1kk3 * localB , fjazhlmyos * localP ) { localB -> jgensia2ex = localP
-> dq_Y0 [ 0 ] ; localB -> l2faoiogru = localP -> dq_Y0 [ 1 ] ; } void
fiy4znthvv ( uint8_T fmdbtw42m3 , const real_T c2ubopwwjd [ 2 ] , real_T
g2zrxgfvcs , gj3ohc1kk3 * localB , oqxlv1t1or * localDW ) { if ( fmdbtw42m3 >
0 ) { localB -> jgensia2ex = c2ubopwwjd [ 0 ] * muDoubleScalarSin (
g2zrxgfvcs ) - c2ubopwwjd [ 1 ] * muDoubleScalarCos ( g2zrxgfvcs ) ; localB
-> l2faoiogru = c2ubopwwjd [ 0 ] * muDoubleScalarCos ( g2zrxgfvcs ) +
c2ubopwwjd [ 1 ] * muDoubleScalarSin ( g2zrxgfvcs ) ; srUpdateBC ( localDW ->
cyyxenso3u ) ; } } void brnjra0xun ( brzkpe5yck * localB , fcezepw4oj *
localP ) { localB -> i0fvut4tar = localP -> dq_Y0 [ 0 ] ; localB ->
k5tvpsreru = localP -> dq_Y0 [ 1 ] ; } void flkfnl3dqu ( uint8_T pw0iil31u4 ,
const real_T hnlegfoeow [ 2 ] , real_T jl3rqh3r3n , brzkpe5yck * localB ,
np452uldfz * localDW ) { if ( pw0iil31u4 > 0 ) { localB -> i0fvut4tar =
hnlegfoeow [ 0 ] * muDoubleScalarCos ( jl3rqh3r3n ) + hnlegfoeow [ 1 ] *
muDoubleScalarSin ( jl3rqh3r3n ) ; localB -> k5tvpsreru = - hnlegfoeow [ 0 ]
* muDoubleScalarSin ( jl3rqh3r3n ) + hnlegfoeow [ 1 ] * muDoubleScalarCos (
jl3rqh3r3n ) ; srUpdateBC ( localDW -> nevvtwmofe ) ; } } void gj0ekbre2u (
SimStruct * rtS_i , real_T kwzh0lz0ar , creal_T p2ionxkoyh , creal_T
cabkv5wct2 , creal_T hkgcxqxzgv , pij4s3w30k * localB , baqwe1p1zs * localDW
, m3uhrpfyin * localP ) { if ( kwzh0lz0ar > 0.0 ) { localB -> pcsvo2nso5 . re
= ( ( ( localP -> Gain1_Gain [ 0 ] . re * p2ionxkoyh . re - localP ->
Gain1_Gain [ 0 ] . im * p2ionxkoyh . im ) + ( localP -> Gain1_Gain [ 1 ] . re
* cabkv5wct2 . re - localP -> Gain1_Gain [ 1 ] . im * cabkv5wct2 . im ) ) + (
localP -> Gain1_Gain [ 2 ] . re * hkgcxqxzgv . re - localP -> Gain1_Gain [ 2
] . im * hkgcxqxzgv . im ) ) * localP -> Gain3_Gain ; localB -> pcsvo2nso5 .
im = ( ( ( localP -> Gain1_Gain [ 0 ] . re * p2ionxkoyh . im + localP ->
Gain1_Gain [ 0 ] . im * p2ionxkoyh . re ) + ( localP -> Gain1_Gain [ 1 ] . re
* cabkv5wct2 . im + localP -> Gain1_Gain [ 1 ] . im * cabkv5wct2 . re ) ) + (
localP -> Gain1_Gain [ 2 ] . re * hkgcxqxzgv . im + localP -> Gain1_Gain [ 2
] . im * hkgcxqxzgv . re ) ) * localP -> Gain3_Gain ; if ( ssIsMajorTimeStep
( rtS_i ) ) { srUpdateBC ( localDW -> hrxuy113gk ) ; } } } void ihqnx345xu (
SimStruct * rtS_e , real_T kix2j21ure , creal_T o3vpyt2sql , creal_T
myolzlxrer , creal_T egbcgmbq04 , pjkmgaolpi * localB , lkl4gg004l * localDW
, pneeuxrdyc * localP ) { if ( kix2j21ure > 0.0 ) { localB -> exp35rwk2r . re
= ( ( o3vpyt2sql . re + myolzlxrer . re ) + egbcgmbq04 . re ) * localP ->
Gain3_Gain ; localB -> exp35rwk2r . im = ( ( o3vpyt2sql . im + myolzlxrer .
im ) + egbcgmbq04 . im ) * localP -> Gain3_Gain ; if ( ssIsMajorTimeStep (
rtS_e ) ) { srUpdateBC ( localDW -> bwjj0lhfrh ) ; } } } void pvdad0deeo (
ghzael4l32 * localDW , pcr2y0buvk * localP , e0vkhe1hid * localX ) { localX
-> e2h5nin1ya [ 0 ] = localP -> integrator_IC ; localDW -> fl0yqnilhd [ 0 ] =
localP -> Memory_InitialCondition ; localX -> ndbmdbddqe [ 0 ] = localP ->
integrator_IC_lexuvhoanq ; localDW -> h54atg5cgd [ 0 ] = localP ->
Memory_InitialCondition_iskxsdaa2y ; localX -> e2h5nin1ya [ 1 ] = localP ->
integrator_IC ; localDW -> fl0yqnilhd [ 1 ] = localP ->
Memory_InitialCondition ; localX -> ndbmdbddqe [ 1 ] = localP ->
integrator_IC_lexuvhoanq ; localDW -> h54atg5cgd [ 1 ] = localP ->
Memory_InitialCondition_iskxsdaa2y ; localX -> e2h5nin1ya [ 2 ] = localP ->
integrator_IC ; localDW -> fl0yqnilhd [ 2 ] = localP ->
Memory_InitialCondition ; localX -> ndbmdbddqe [ 2 ] = localP ->
integrator_IC_lexuvhoanq ; localDW -> h54atg5cgd [ 2 ] = localP ->
Memory_InitialCondition_iskxsdaa2y ; } void nainrbgoka ( ghzael4l32 * localDW
) { localDW -> op4r1m20ns = false ; } void h0ryf0cnmm ( SimStruct * rtS_g ,
ghzael4l32 * localDW , pcr2y0buvk * localP , mjc13vhdfp * localXdis ) {
localDW -> op4r1m20ns = false ; ssSetBlockStateForSolverChangedAtMajorStep (
rtS_g ) ; ( void ) memset ( & ( localXdis -> e2h5nin1ya ) , 1 , 6 * sizeof (
boolean_T ) ) ; { { int_T i1 ; int_T * iw_Tail = & localDW -> ccgy3ccmth .
Tail [ 0 ] ; int_T * iw_Head = & localDW -> ccgy3ccmth . Head [ 0 ] ; int_T *
iw_Last = & localDW -> ccgy3ccmth . Last [ 0 ] ; int_T * iw_CircularBufSize =
& localDW -> ccgy3ccmth . CircularBufSize [ 0 ] ; void * * pw_TUbufferPtrs =
& localDW -> bdokffy1ag . TUbufferPtrs [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++
) { real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS_g ,
"tdelay memory allocation error" ) ; return ; } iw_Tail [ i1 ] = 0 ; iw_Head
[ i1 ] = 0 ; iw_Last [ i1 ] = 0 ; iw_CircularBufSize [ i1 ] = 8192 ; pBuffer
[ 0 ] = localP -> TransportDelay_InitOutput ; pBuffer [ 8192 ] = ssGetT (
rtS_g ) ; pw_TUbufferPtrs [ i1 ] = ( void * ) & pBuffer [ 0 ] ;
pw_TUbufferPtrs [ i1 + 3 ] = ( void * ) & pBuffer [ 8192 ] ; } } } { { int_T
i1 ; int_T * iw_Tail = & localDW -> e5x1ldeknm . Tail [ 0 ] ; int_T * iw_Head
= & localDW -> e5x1ldeknm . Head [ 0 ] ; int_T * iw_Last = & localDW ->
e5x1ldeknm . Last [ 0 ] ; int_T * iw_CircularBufSize = & localDW ->
e5x1ldeknm . CircularBufSize [ 0 ] ; void * * pw_TUbufferPtrs = & localDW ->
lhvzznxbru . TUbufferPtrs [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) { real_T *
pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ;
if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS_g ,
"tdelay memory allocation error" ) ; return ; } iw_Tail [ i1 ] = 0 ; iw_Head
[ i1 ] = 0 ; iw_Last [ i1 ] = 0 ; iw_CircularBufSize [ i1 ] = 8192 ; pBuffer
[ 0 ] = localP -> TransportDelay_InitOutput_gwyp41kcsd ; pBuffer [ 8192 ] =
ssGetT ( rtS_g ) ; pw_TUbufferPtrs [ i1 ] = ( void * ) & pBuffer [ 0 ] ;
pw_TUbufferPtrs [ i1 + 3 ] = ( void * ) & pBuffer [ 8192 ] ; } } } } void
kyedbo0mxu ( SimStruct * rtS_j , boolean_T hmajr40tp3 , const real_T
lghjdroza4 [ 3 ] , go1ko1exrf * localB , ghzael4l32 * localDW , pcr2y0buvk *
localP , e0vkhe1hid * localX , mjc13vhdfp * localXdis ) { real_T kciabdctyi ;
if ( ssIsSampleHit ( rtS_j , 1 , 0 ) && ssIsMajorTimeStep ( rtS_j ) ) { if (
hmajr40tp3 ) { if ( ! localDW -> op4r1m20ns ) { if ( ssGetTaskTime ( rtS_j ,
1 ) != ssGetTStart ( rtS_j ) ) { ssSetBlockStateForSolverChangedAtMajorStep (
rtS_j ) ; } ( void ) memset ( & ( localXdis -> e2h5nin1ya ) , 0 , 6 * sizeof
( boolean_T ) ) ; localDW -> op4r1m20ns = true ; } } else { if ( localDW ->
op4r1m20ns ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS_j ) ; ( void
) memset ( & ( localXdis -> e2h5nin1ya ) , 1 , 6 * sizeof ( boolean_T ) ) ;
nainrbgoka ( localDW ) ; } } } if ( localDW -> op4r1m20ns ) { localB ->
k2st2nj1lo [ 0 ] = localX -> e2h5nin1ya [ 0 ] ; localB -> k2st2nj1lo [ 1 ] =
localX -> e2h5nin1ya [ 1 ] ; localB -> k2st2nj1lo [ 2 ] = localX ->
e2h5nin1ya [ 2 ] ; { real_T * * uBuffer = ( real_T * * ) & localDW ->
bdokffy1ag . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
localDW -> bdokffy1ag . TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT ( rtS_j
) ; real_T tMinusDelay ; { int_T i1 ; real_T * y0 = & localB -> pst4sv04lw [
0 ] ; int_T * iw_Tail = & localDW -> ccgy3ccmth . Tail [ 0 ] ; int_T *
iw_Head = & localDW -> ccgy3ccmth . Head [ 0 ] ; int_T * iw_Last = & localDW
-> ccgy3ccmth . Last [ 0 ] ; int_T * iw_CircularBufSize = & localDW ->
ccgy3ccmth . CircularBufSize [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) {
tMinusDelay = ( ( localP -> TransportDelay_Delay > 0.0 ) ? localP ->
TransportDelay_Delay : 0.0 ) ; tMinusDelay = simTime - tMinusDelay ; y0 [ i1
] = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ] , iw_Tail [ i1 ] , iw_Head [ i1
] , localP -> TransportDelay_InitOutput , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( rtS_j ) && ( ssGetTimeOfLastOutput ( rtS_j ) == ssGetT (
rtS_j ) ) ) ) ; tBuffer ++ ; uBuffer ++ ; } } } if ( ssIsSampleHit ( rtS_j ,
1 , 0 ) ) { localB -> iw2e3rkjnh = localP -> K1_Value ; localB -> nkff3onzga
[ 0 ] = localDW -> fl0yqnilhd [ 0 ] ; localB -> nkff3onzga [ 1 ] = localDW ->
fl0yqnilhd [ 1 ] ; localB -> nkff3onzga [ 2 ] = localDW -> fl0yqnilhd [ 2 ] ;
} if ( ssGetT ( rtS_j ) >= localB -> iw2e3rkjnh ) { localB -> hk1ldf3i0k [ 0
] = localB -> k2st2nj1lo [ 0 ] - localB -> pst4sv04lw [ 0 ] ; localB ->
ol2ictkqzg [ 0 ] = localP -> Gain_Gain_nx51kbuuf3 * localB -> hk1ldf3i0k [ 0
] ; localB -> eizdwwjmoe [ 0 ] = localB -> ol2ictkqzg [ 0 ] ; localB ->
hk1ldf3i0k [ 1 ] = localB -> k2st2nj1lo [ 1 ] - localB -> pst4sv04lw [ 1 ] ;
localB -> ol2ictkqzg [ 1 ] = localP -> Gain_Gain_nx51kbuuf3 * localB ->
hk1ldf3i0k [ 1 ] ; localB -> eizdwwjmoe [ 1 ] = localB -> ol2ictkqzg [ 1 ] ;
localB -> hk1ldf3i0k [ 2 ] = localB -> k2st2nj1lo [ 2 ] - localB ->
pst4sv04lw [ 2 ] ; localB -> ol2ictkqzg [ 2 ] = localP ->
Gain_Gain_nx51kbuuf3 * localB -> hk1ldf3i0k [ 2 ] ; localB -> eizdwwjmoe [ 2
] = localB -> ol2ictkqzg [ 2 ] ; } else { localB -> eizdwwjmoe [ 0 ] = localB
-> nkff3onzga [ 0 ] ; localB -> eizdwwjmoe [ 1 ] = localB -> nkff3onzga [ 1 ]
; localB -> eizdwwjmoe [ 2 ] = localB -> nkff3onzga [ 2 ] ; } localB ->
icalmuobxj [ 0 ] = localX -> ndbmdbddqe [ 0 ] ; localB -> icalmuobxj [ 1 ] =
localX -> ndbmdbddqe [ 1 ] ; localB -> icalmuobxj [ 2 ] = localX ->
ndbmdbddqe [ 2 ] ; { real_T * * uBuffer = ( real_T * * ) & localDW ->
lhvzznxbru . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
localDW -> lhvzznxbru . TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT ( rtS_j
) ; real_T tMinusDelay ; { int_T i1 ; real_T * y0 = & localB -> pxnysvjhrw [
0 ] ; int_T * iw_Tail = & localDW -> e5x1ldeknm . Tail [ 0 ] ; int_T *
iw_Head = & localDW -> e5x1ldeknm . Head [ 0 ] ; int_T * iw_Last = & localDW
-> e5x1ldeknm . Last [ 0 ] ; int_T * iw_CircularBufSize = & localDW ->
e5x1ldeknm . CircularBufSize [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) {
tMinusDelay = ( ( localP -> TransportDelay_Delay_ftnitesb5l > 0.0 ) ? localP
-> TransportDelay_Delay_ftnitesb5l : 0.0 ) ; tMinusDelay = simTime -
tMinusDelay ; y0 [ i1 ] = rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ] , iw_Tail
[ i1 ] , iw_Head [ i1 ] , localP -> TransportDelay_InitOutput_gwyp41kcsd , 0
, ( boolean_T ) ( ssIsMinorTimeStep ( rtS_j ) && ( ssGetTimeOfLastOutput (
rtS_j ) == ssGetT ( rtS_j ) ) ) ) ; tBuffer ++ ; uBuffer ++ ; } } } if (
ssIsSampleHit ( rtS_j , 1 , 0 ) ) { localB -> cqe0khjtcl = localP ->
K1_Value_ky41retfs4 ; localB -> dekrkdeyqh [ 0 ] = localDW -> h54atg5cgd [ 0
] ; localB -> dekrkdeyqh [ 1 ] = localDW -> h54atg5cgd [ 1 ] ; localB ->
dekrkdeyqh [ 2 ] = localDW -> h54atg5cgd [ 2 ] ; } if ( ssGetT ( rtS_j ) >=
localB -> cqe0khjtcl ) { localB -> btlpw3mp4z [ 0 ] = localB -> icalmuobxj [
0 ] - localB -> pxnysvjhrw [ 0 ] ; localB -> np40bymzcs [ 0 ] = localP ->
Gain_Gain * localB -> btlpw3mp4z [ 0 ] ; localB -> hxglutqeuf [ 0 ] = localB
-> np40bymzcs [ 0 ] ; localB -> btlpw3mp4z [ 1 ] = localB -> icalmuobxj [ 1 ]
- localB -> pxnysvjhrw [ 1 ] ; localB -> np40bymzcs [ 1 ] = localP ->
Gain_Gain * localB -> btlpw3mp4z [ 1 ] ; localB -> hxglutqeuf [ 1 ] = localB
-> np40bymzcs [ 1 ] ; localB -> btlpw3mp4z [ 2 ] = localB -> icalmuobxj [ 2 ]
- localB -> pxnysvjhrw [ 2 ] ; localB -> np40bymzcs [ 2 ] = localP ->
Gain_Gain * localB -> btlpw3mp4z [ 2 ] ; localB -> hxglutqeuf [ 2 ] = localB
-> np40bymzcs [ 2 ] ; } else { localB -> hxglutqeuf [ 0 ] = localB ->
dekrkdeyqh [ 0 ] ; localB -> hxglutqeuf [ 1 ] = localB -> dekrkdeyqh [ 1 ] ;
localB -> hxglutqeuf [ 2 ] = localB -> dekrkdeyqh [ 2 ] ; } localB ->
o2kqm2z0ta [ 0 ] . re = localB -> eizdwwjmoe [ 0 ] ; localB -> o2kqm2z0ta [ 0
] . im = localB -> hxglutqeuf [ 0 ] ; localB -> o2kqm2z0ta [ 1 ] . re =
localB -> eizdwwjmoe [ 1 ] ; localB -> o2kqm2z0ta [ 1 ] . im = localB ->
hxglutqeuf [ 1 ] ; localB -> o2kqm2z0ta [ 2 ] . re = localB -> eizdwwjmoe [ 2
] ; localB -> o2kqm2z0ta [ 2 ] . im = localB -> hxglutqeuf [ 2 ] ; kciabdctyi
= muDoubleScalarSin ( localP -> sinwt_Freq * ssGetTaskTime ( rtS_j , 0 ) +
localP -> sinwt_Phase ) * localP -> sinwt_Amp + localP -> sinwt_Bias ; localB
-> echkhwo04e [ 0 ] = lghjdroza4 [ 0 ] * kciabdctyi ; localB -> echkhwo04e [
1 ] = lghjdroza4 [ 1 ] * kciabdctyi ; localB -> echkhwo04e [ 2 ] = lghjdroza4
[ 2 ] * kciabdctyi ; kciabdctyi = muDoubleScalarSin ( localP -> coswt_Freq *
ssGetTaskTime ( rtS_j , 0 ) + localP -> coswt_Phase ) * localP -> coswt_Amp +
localP -> coswt_Bias ; localB -> pefvlkkdkt [ 0 ] = lghjdroza4 [ 0 ] *
kciabdctyi ; localB -> pefvlkkdkt [ 1 ] = lghjdroza4 [ 1 ] * kciabdctyi ;
localB -> pefvlkkdkt [ 2 ] = lghjdroza4 [ 2 ] * kciabdctyi ; localB ->
me4ylfixyn [ 0 ] = localP -> Gain_Gain_gq3qylxz5y * muDoubleScalarHypot (
localB -> o2kqm2z0ta [ 0 ] . re , localB -> o2kqm2z0ta [ 0 ] . im ) ; localB
-> me4ylfixyn [ 1 ] = localP -> Gain_Gain_gq3qylxz5y * muDoubleScalarHypot (
localB -> o2kqm2z0ta [ 1 ] . re , localB -> o2kqm2z0ta [ 1 ] . im ) ; localB
-> me4ylfixyn [ 2 ] = localP -> Gain_Gain_gq3qylxz5y * muDoubleScalarHypot (
localB -> o2kqm2z0ta [ 2 ] . re , localB -> o2kqm2z0ta [ 2 ] . im ) ; if (
ssIsMajorTimeStep ( rtS_j ) ) { srUpdateBC ( localDW -> gpfaohkthl ) ; } } }
void gxsaitrvqe ( SimStruct * rtS_f , go1ko1exrf * localB , ghzael4l32 *
localDW , pcr2y0buvk * localP ) { if ( localDW -> op4r1m20ns ) { { real_T * *
uBuffer = ( real_T * * ) & localDW -> bdokffy1ag . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & localDW -> bdokffy1ag . TUbufferPtrs [
3 ] ; real_T simTime = ssGetT ( rtS_f ) ; localDW -> ccgy3ccmth . Head [ 0 ]
= ( ( localDW -> ccgy3ccmth . Head [ 0 ] < ( localDW -> ccgy3ccmth .
CircularBufSize [ 0 ] - 1 ) ) ? ( localDW -> ccgy3ccmth . Head [ 0 ] + 1 ) :
0 ) ; if ( localDW -> ccgy3ccmth . Head [ 0 ] == localDW -> ccgy3ccmth . Tail
[ 0 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( & localDW -> ccgy3ccmth .
CircularBufSize [ 0 ] , & localDW -> ccgy3ccmth . Tail [ 0 ] , & localDW ->
ccgy3ccmth . Head [ 0 ] , & localDW -> ccgy3ccmth . Last [ 0 ] , simTime -
localP -> TransportDelay_Delay , tBuffer , uBuffer , ( NULL ) , ( boolean_T )
0 , false , & localDW -> ccgy3ccmth . MaxNewBufSize ) ) { ssSetErrorStatus (
rtS_f , "tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [
localDW -> ccgy3ccmth . Head [ 0 ] ] = simTime ; ( * uBuffer ++ ) [ localDW
-> ccgy3ccmth . Head [ 0 ] ] = localB -> k2st2nj1lo [ 0 ] ; localDW ->
ccgy3ccmth . Head [ 1 ] = ( ( localDW -> ccgy3ccmth . Head [ 1 ] < ( localDW
-> ccgy3ccmth . CircularBufSize [ 1 ] - 1 ) ) ? ( localDW -> ccgy3ccmth .
Head [ 1 ] + 1 ) : 0 ) ; if ( localDW -> ccgy3ccmth . Head [ 1 ] == localDW
-> ccgy3ccmth . Tail [ 1 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( &
localDW -> ccgy3ccmth . CircularBufSize [ 1 ] , & localDW -> ccgy3ccmth .
Tail [ 1 ] , & localDW -> ccgy3ccmth . Head [ 1 ] , & localDW -> ccgy3ccmth .
Last [ 1 ] , simTime - localP -> TransportDelay_Delay , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & localDW -> ccgy3ccmth . MaxNewBufSize )
) { ssSetErrorStatus ( rtS_f , "tdelay memory allocation error" ) ; return ;
} } ( * tBuffer ++ ) [ localDW -> ccgy3ccmth . Head [ 1 ] ] = simTime ; ( *
uBuffer ++ ) [ localDW -> ccgy3ccmth . Head [ 1 ] ] = localB -> k2st2nj1lo [
1 ] ; localDW -> ccgy3ccmth . Head [ 2 ] = ( ( localDW -> ccgy3ccmth . Head [
2 ] < ( localDW -> ccgy3ccmth . CircularBufSize [ 2 ] - 1 ) ) ? ( localDW ->
ccgy3ccmth . Head [ 2 ] + 1 ) : 0 ) ; if ( localDW -> ccgy3ccmth . Head [ 2 ]
== localDW -> ccgy3ccmth . Tail [ 2 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf
( & localDW -> ccgy3ccmth . CircularBufSize [ 2 ] , & localDW -> ccgy3ccmth .
Tail [ 2 ] , & localDW -> ccgy3ccmth . Head [ 2 ] , & localDW -> ccgy3ccmth .
Last [ 2 ] , simTime - localP -> TransportDelay_Delay , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & localDW -> ccgy3ccmth . MaxNewBufSize )
) { ssSetErrorStatus ( rtS_f , "tdelay memory allocation error" ) ; return ;
} } ( * tBuffer ) [ localDW -> ccgy3ccmth . Head [ 2 ] ] = simTime ; ( *
uBuffer ) [ localDW -> ccgy3ccmth . Head [ 2 ] ] = localB -> k2st2nj1lo [ 2 ]
; } if ( ssIsSampleHit ( rtS_f , 1 , 0 ) ) { localDW -> fl0yqnilhd [ 0 ] =
localB -> eizdwwjmoe [ 0 ] ; localDW -> fl0yqnilhd [ 1 ] = localB ->
eizdwwjmoe [ 1 ] ; localDW -> fl0yqnilhd [ 2 ] = localB -> eizdwwjmoe [ 2 ] ;
} { real_T * * uBuffer = ( real_T * * ) & localDW -> lhvzznxbru .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & localDW ->
lhvzznxbru . TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT ( rtS_f ) ; localDW
-> e5x1ldeknm . Head [ 0 ] = ( ( localDW -> e5x1ldeknm . Head [ 0 ] < (
localDW -> e5x1ldeknm . CircularBufSize [ 0 ] - 1 ) ) ? ( localDW ->
e5x1ldeknm . Head [ 0 ] + 1 ) : 0 ) ; if ( localDW -> e5x1ldeknm . Head [ 0 ]
== localDW -> e5x1ldeknm . Tail [ 0 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf
( & localDW -> e5x1ldeknm . CircularBufSize [ 0 ] , & localDW -> e5x1ldeknm .
Tail [ 0 ] , & localDW -> e5x1ldeknm . Head [ 0 ] , & localDW -> e5x1ldeknm .
Last [ 0 ] , simTime - localP -> TransportDelay_Delay_ftnitesb5l , tBuffer ,
uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & localDW -> e5x1ldeknm .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS_f ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [ localDW
-> e5x1ldeknm . Head [ 0 ] ] = simTime ; ( * uBuffer ++ ) [ localDW ->
e5x1ldeknm . Head [ 0 ] ] = localB -> icalmuobxj [ 0 ] ; localDW ->
e5x1ldeknm . Head [ 1 ] = ( ( localDW -> e5x1ldeknm . Head [ 1 ] < ( localDW
-> e5x1ldeknm . CircularBufSize [ 1 ] - 1 ) ) ? ( localDW -> e5x1ldeknm .
Head [ 1 ] + 1 ) : 0 ) ; if ( localDW -> e5x1ldeknm . Head [ 1 ] == localDW
-> e5x1ldeknm . Tail [ 1 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( &
localDW -> e5x1ldeknm . CircularBufSize [ 1 ] , & localDW -> e5x1ldeknm .
Tail [ 1 ] , & localDW -> e5x1ldeknm . Head [ 1 ] , & localDW -> e5x1ldeknm .
Last [ 1 ] , simTime - localP -> TransportDelay_Delay_ftnitesb5l , tBuffer ,
uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & localDW -> e5x1ldeknm .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS_f ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [ localDW
-> e5x1ldeknm . Head [ 1 ] ] = simTime ; ( * uBuffer ++ ) [ localDW ->
e5x1ldeknm . Head [ 1 ] ] = localB -> icalmuobxj [ 1 ] ; localDW ->
e5x1ldeknm . Head [ 2 ] = ( ( localDW -> e5x1ldeknm . Head [ 2 ] < ( localDW
-> e5x1ldeknm . CircularBufSize [ 2 ] - 1 ) ) ? ( localDW -> e5x1ldeknm .
Head [ 2 ] + 1 ) : 0 ) ; if ( localDW -> e5x1ldeknm . Head [ 2 ] == localDW
-> e5x1ldeknm . Tail [ 2 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( &
localDW -> e5x1ldeknm . CircularBufSize [ 2 ] , & localDW -> e5x1ldeknm .
Tail [ 2 ] , & localDW -> e5x1ldeknm . Head [ 2 ] , & localDW -> e5x1ldeknm .
Last [ 2 ] , simTime - localP -> TransportDelay_Delay_ftnitesb5l , tBuffer ,
uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & localDW -> e5x1ldeknm .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS_f ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ localDW ->
e5x1ldeknm . Head [ 2 ] ] = simTime ; ( * uBuffer ) [ localDW -> e5x1ldeknm .
Head [ 2 ] ] = localB -> icalmuobxj [ 2 ] ; } if ( ssIsSampleHit ( rtS_f , 1
, 0 ) ) { localDW -> h54atg5cgd [ 0 ] = localB -> hxglutqeuf [ 0 ] ; localDW
-> h54atg5cgd [ 1 ] = localB -> hxglutqeuf [ 1 ] ; localDW -> h54atg5cgd [ 2
] = localB -> hxglutqeuf [ 2 ] ; } } } void jj4trrgi3n ( go1ko1exrf * localB
, ghzael4l32 * localDW , bvo2qlqek0 * localXdot ) { if ( localDW ->
op4r1m20ns ) { localXdot -> e2h5nin1ya [ 0 ] = localB -> echkhwo04e [ 0 ] ;
localXdot -> ndbmdbddqe [ 0 ] = localB -> pefvlkkdkt [ 0 ] ; localXdot ->
e2h5nin1ya [ 1 ] = localB -> echkhwo04e [ 1 ] ; localXdot -> ndbmdbddqe [ 1 ]
= localB -> pefvlkkdkt [ 1 ] ; localXdot -> e2h5nin1ya [ 2 ] = localB ->
echkhwo04e [ 2 ] ; localXdot -> ndbmdbddqe [ 2 ] = localB -> pefvlkkdkt [ 2 ]
; } else { { real_T * dx ; int_T i ; dx = & ( localXdot -> e2h5nin1ya [ 0 ] )
; for ( i = 0 ; i < 6 ; i ++ ) { dx [ i ] = 0.0 ; } } } } void g5flx2hgit (
ghzael4l32 * localDW ) { { int_T i1 ; void * * pw_TUbufferPtrs = & localDW ->
bdokffy1ag . TUbufferPtrs [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) {
rt_TDelayFreeBuf ( pw_TUbufferPtrs [ i1 ] ) ; } } { int_T i1 ; void * *
pw_TUbufferPtrs = & localDW -> lhvzznxbru . TUbufferPtrs [ 0 ] ; for ( i1 = 0
; i1 < 3 ; i1 ++ ) { rt_TDelayFreeBuf ( pw_TUbufferPtrs [ i1 ] ) ; } } } void
i4sjz31q1b ( o3cgxb1nm5 * localDW , klg0lcyohc * localP , h1l3104bn0 * localX
) { localX -> mr3ym4eux3 [ 0 ] = localP -> integrator_IC ; localDW ->
kdzyu5dfoi [ 0 ] = localP -> Memory_InitialCondition ; localX -> mr3ym4eux3 [
1 ] = localP -> integrator_IC ; localDW -> kdzyu5dfoi [ 1 ] = localP ->
Memory_InitialCondition ; localX -> mr3ym4eux3 [ 2 ] = localP ->
integrator_IC ; localDW -> kdzyu5dfoi [ 2 ] = localP ->
Memory_InitialCondition ; } void fblsrs2ge4 ( o3cgxb1nm5 * localDW ) {
localDW -> f2xlxmm2rg = false ; } void cvm1yt3zx5 ( SimStruct * rtS_b ,
o3cgxb1nm5 * localDW , klg0lcyohc * localP , hlxxn2qtq3 * localXdis ) {
localDW -> f2xlxmm2rg = false ; ssSetBlockStateForSolverChangedAtMajorStep (
rtS_b ) ; ( void ) memset ( & ( localXdis -> mr3ym4eux3 ) , 1 , 3 * sizeof (
boolean_T ) ) ; { { int_T i1 ; int_T * iw_Tail = & localDW -> aeinmpqosl .
Tail [ 0 ] ; int_T * iw_Head = & localDW -> aeinmpqosl . Head [ 0 ] ; int_T *
iw_Last = & localDW -> aeinmpqosl . Last [ 0 ] ; int_T * iw_CircularBufSize =
& localDW -> aeinmpqosl . CircularBufSize [ 0 ] ; void * * pw_TUbufferPtrs =
& localDW -> gwwponjnqb . TUbufferPtrs [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++
) { real_T * pBuffer = ( real_T * ) rt_TDelayCreateBuf ( 2 , 8192 , sizeof (
real_T ) ) ; if ( pBuffer == ( NULL ) ) { ssSetErrorStatus ( rtS_b ,
"tdelay memory allocation error" ) ; return ; } iw_Tail [ i1 ] = 0 ; iw_Head
[ i1 ] = 0 ; iw_Last [ i1 ] = 0 ; iw_CircularBufSize [ i1 ] = 8192 ; pBuffer
[ 0 ] = localP -> TransportDelay_InitOutput ; pBuffer [ 8192 ] = ssGetT (
rtS_b ) ; pw_TUbufferPtrs [ i1 ] = ( void * ) & pBuffer [ 0 ] ;
pw_TUbufferPtrs [ i1 + 3 ] = ( void * ) & pBuffer [ 8192 ] ; } } } localDW ->
hw0rx0e4r4 = 0 ; } void gllp1vimnh ( SimStruct * rtS_c , boolean_T bgwpfbivee
, const real_T cgiiotktlo [ 3 ] , pq3igreacf * localB , o3cgxb1nm5 * localDW
, klg0lcyohc * localP , h1l3104bn0 * localX , hlxxn2qtq3 * localXdis ) { if (
ssIsSampleHit ( rtS_c , 1 , 0 ) && ssIsMajorTimeStep ( rtS_c ) ) { if (
bgwpfbivee ) { if ( ! localDW -> f2xlxmm2rg ) { if ( ssGetTaskTime ( rtS_c ,
1 ) != ssGetTStart ( rtS_c ) ) { ssSetBlockStateForSolverChangedAtMajorStep (
rtS_c ) ; } ( void ) memset ( & ( localXdis -> mr3ym4eux3 ) , 0 , 3 * sizeof
( boolean_T ) ) ; localDW -> f2xlxmm2rg = true ; } } else { if ( localDW ->
f2xlxmm2rg ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS_c ) ; ( void
) memset ( & ( localXdis -> mr3ym4eux3 ) , 1 , 3 * sizeof ( boolean_T ) ) ;
fblsrs2ge4 ( localDW ) ; } } } if ( localDW -> f2xlxmm2rg ) { localB ->
a3xvaqx1gx [ 0 ] = localX -> mr3ym4eux3 [ 0 ] ; localB -> a3xvaqx1gx [ 1 ] =
localX -> mr3ym4eux3 [ 1 ] ; localB -> a3xvaqx1gx [ 2 ] = localX ->
mr3ym4eux3 [ 2 ] ; { real_T * * uBuffer = ( real_T * * ) & localDW ->
gwwponjnqb . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) &
localDW -> gwwponjnqb . TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT ( rtS_c
) ; real_T tMinusDelay ; { int_T i1 ; real_T * y0 = & localB -> a52c55k23g [
0 ] ; int_T * iw_Tail = & localDW -> aeinmpqosl . Tail [ 0 ] ; int_T *
iw_Head = & localDW -> aeinmpqosl . Head [ 0 ] ; int_T * iw_Last = & localDW
-> aeinmpqosl . Last [ 0 ] ; int_T * iw_CircularBufSize = & localDW ->
aeinmpqosl . CircularBufSize [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) {
tMinusDelay = ( ( localP -> TransportDelay_Delay > 0.0 ) ? localP ->
TransportDelay_Delay : 0.0 ) ; tMinusDelay = simTime - tMinusDelay ; y0 [ i1
] = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer ,
iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ] , iw_Tail [ i1 ] , iw_Head [ i1
] , localP -> TransportDelay_InitOutput , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( rtS_c ) && ( ssGetTimeOfLastOutput ( rtS_c ) == ssGetT (
rtS_c ) ) ) ) ; tBuffer ++ ; uBuffer ++ ; } } } if ( ssIsSampleHit ( rtS_c ,
1 , 0 ) ) { localB -> lr13frcvpj = localP -> K1_Value ; localB -> lqyqt2horm
[ 0 ] = localDW -> kdzyu5dfoi [ 0 ] ; localB -> lqyqt2horm [ 1 ] = localDW ->
kdzyu5dfoi [ 1 ] ; localB -> lqyqt2horm [ 2 ] = localDW -> kdzyu5dfoi [ 2 ] ;
} if ( ssGetT ( rtS_c ) >= localB -> lr13frcvpj ) { localB -> mf4drh5hrb [ 0
] = localB -> a3xvaqx1gx [ 0 ] - localB -> a52c55k23g [ 0 ] ; localB ->
idrljgtxlp [ 0 ] = localP -> Gain_Gain * localB -> mf4drh5hrb [ 0 ] ; localB
-> gs4rvks0h4 [ 0 ] = localB -> idrljgtxlp [ 0 ] ; localB -> mf4drh5hrb [ 1 ]
= localB -> a3xvaqx1gx [ 1 ] - localB -> a52c55k23g [ 1 ] ; localB ->
idrljgtxlp [ 1 ] = localP -> Gain_Gain * localB -> mf4drh5hrb [ 1 ] ; localB
-> gs4rvks0h4 [ 1 ] = localB -> idrljgtxlp [ 1 ] ; localB -> mf4drh5hrb [ 2 ]
= localB -> a3xvaqx1gx [ 2 ] - localB -> a52c55k23g [ 2 ] ; localB ->
idrljgtxlp [ 2 ] = localP -> Gain_Gain * localB -> mf4drh5hrb [ 2 ] ; localB
-> gs4rvks0h4 [ 2 ] = localB -> idrljgtxlp [ 2 ] ; } else { localB ->
gs4rvks0h4 [ 0 ] = localB -> lqyqt2horm [ 0 ] ; localB -> gs4rvks0h4 [ 1 ] =
localB -> lqyqt2horm [ 1 ] ; localB -> gs4rvks0h4 [ 2 ] = localB ->
lqyqt2horm [ 2 ] ; } localB -> jeg1ixfqcc [ 0 ] = cgiiotktlo [ 0 ] *
cgiiotktlo [ 0 ] ; localB -> jeg1ixfqcc [ 1 ] = cgiiotktlo [ 1 ] * cgiiotktlo
[ 1 ] ; localB -> jeg1ixfqcc [ 2 ] = cgiiotktlo [ 2 ] * cgiiotktlo [ 2 ] ; if
( ssIsMajorTimeStep ( rtS_c ) ) { localDW -> lxm3e5s4uc [ 0 ] = localB ->
gs4rvks0h4 [ 0 ] >= localP -> Saturationtoavoidnegativesqrt_UpperSat ? 1 :
localB -> gs4rvks0h4 [ 0 ] > localP -> Saturationtoavoidnegativesqrt_LowerSat
? 0 : - 1 ; localDW -> lxm3e5s4uc [ 1 ] = localB -> gs4rvks0h4 [ 1 ] >=
localP -> Saturationtoavoidnegativesqrt_UpperSat ? 1 : localB -> gs4rvks0h4 [
1 ] > localP -> Saturationtoavoidnegativesqrt_LowerSat ? 0 : - 1 ; localDW ->
lxm3e5s4uc [ 2 ] = localB -> gs4rvks0h4 [ 2 ] >= localP ->
Saturationtoavoidnegativesqrt_UpperSat ? 1 : localB -> gs4rvks0h4 [ 2 ] >
localP -> Saturationtoavoidnegativesqrt_LowerSat ? 0 : - 1 ; } localB ->
d2yyti4lqk [ 0 ] = localDW -> lxm3e5s4uc [ 0 ] == 1 ? localP ->
Saturationtoavoidnegativesqrt_UpperSat : localDW -> lxm3e5s4uc [ 0 ] == - 1 ?
localP -> Saturationtoavoidnegativesqrt_LowerSat : localB -> gs4rvks0h4 [ 0 ]
; localB -> d2yyti4lqk [ 1 ] = localDW -> lxm3e5s4uc [ 1 ] == 1 ? localP ->
Saturationtoavoidnegativesqrt_UpperSat : localDW -> lxm3e5s4uc [ 1 ] == - 1 ?
localP -> Saturationtoavoidnegativesqrt_LowerSat : localB -> gs4rvks0h4 [ 1 ]
; localB -> d2yyti4lqk [ 2 ] = localDW -> lxm3e5s4uc [ 2 ] == 1 ? localP ->
Saturationtoavoidnegativesqrt_UpperSat : localDW -> lxm3e5s4uc [ 2 ] == - 1 ?
localP -> Saturationtoavoidnegativesqrt_LowerSat : localB -> gs4rvks0h4 [ 2 ]
; if ( ssIsMajorTimeStep ( rtS_c ) ) { if ( localDW -> hw0rx0e4r4 != 0 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS_c ) ; localDW -> hw0rx0e4r4
= 0 ; } localB -> gx0tigur0o [ 0 ] = muDoubleScalarSqrt ( localB ->
d2yyti4lqk [ 0 ] ) ; localB -> gx0tigur0o [ 1 ] = muDoubleScalarSqrt ( localB
-> d2yyti4lqk [ 1 ] ) ; localB -> gx0tigur0o [ 2 ] = muDoubleScalarSqrt (
localB -> d2yyti4lqk [ 2 ] ) ; srUpdateBC ( localDW -> oqyvh1ylxe ) ; } else
{ if ( localB -> d2yyti4lqk [ 0 ] < 0.0 ) { localB -> gx0tigur0o [ 0 ] = -
muDoubleScalarSqrt ( muDoubleScalarAbs ( localB -> d2yyti4lqk [ 0 ] ) ) ; }
else { localB -> gx0tigur0o [ 0 ] = muDoubleScalarSqrt ( localB -> d2yyti4lqk
[ 0 ] ) ; } if ( localB -> d2yyti4lqk [ 0 ] < 0.0 ) { localDW -> hw0rx0e4r4 =
1 ; } if ( localB -> d2yyti4lqk [ 1 ] < 0.0 ) { localB -> gx0tigur0o [ 1 ] =
- muDoubleScalarSqrt ( muDoubleScalarAbs ( localB -> d2yyti4lqk [ 1 ] ) ) ; }
else { localB -> gx0tigur0o [ 1 ] = muDoubleScalarSqrt ( localB -> d2yyti4lqk
[ 1 ] ) ; } if ( localB -> d2yyti4lqk [ 1 ] < 0.0 ) { localDW -> hw0rx0e4r4 =
1 ; } if ( localB -> d2yyti4lqk [ 2 ] < 0.0 ) { localB -> gx0tigur0o [ 2 ] =
- muDoubleScalarSqrt ( muDoubleScalarAbs ( localB -> d2yyti4lqk [ 2 ] ) ) ; }
else { localB -> gx0tigur0o [ 2 ] = muDoubleScalarSqrt ( localB -> d2yyti4lqk
[ 2 ] ) ; } if ( localB -> d2yyti4lqk [ 2 ] < 0.0 ) { localDW -> hw0rx0e4r4 =
1 ; } } } } void pwyhxd4z2i ( SimStruct * rtS_f , pq3igreacf * localB ,
o3cgxb1nm5 * localDW , klg0lcyohc * localP ) { if ( localDW -> f2xlxmm2rg ) {
{ real_T * * uBuffer = ( real_T * * ) & localDW -> gwwponjnqb . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & localDW -> gwwponjnqb .
TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT ( rtS_f ) ; localDW ->
aeinmpqosl . Head [ 0 ] = ( ( localDW -> aeinmpqosl . Head [ 0 ] < ( localDW
-> aeinmpqosl . CircularBufSize [ 0 ] - 1 ) ) ? ( localDW -> aeinmpqosl .
Head [ 0 ] + 1 ) : 0 ) ; if ( localDW -> aeinmpqosl . Head [ 0 ] == localDW
-> aeinmpqosl . Tail [ 0 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( &
localDW -> aeinmpqosl . CircularBufSize [ 0 ] , & localDW -> aeinmpqosl .
Tail [ 0 ] , & localDW -> aeinmpqosl . Head [ 0 ] , & localDW -> aeinmpqosl .
Last [ 0 ] , simTime - localP -> TransportDelay_Delay , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & localDW -> aeinmpqosl . MaxNewBufSize )
) { ssSetErrorStatus ( rtS_f , "tdelay memory allocation error" ) ; return ;
} } ( * tBuffer ++ ) [ localDW -> aeinmpqosl . Head [ 0 ] ] = simTime ; ( *
uBuffer ++ ) [ localDW -> aeinmpqosl . Head [ 0 ] ] = localB -> a3xvaqx1gx [
0 ] ; localDW -> aeinmpqosl . Head [ 1 ] = ( ( localDW -> aeinmpqosl . Head [
1 ] < ( localDW -> aeinmpqosl . CircularBufSize [ 1 ] - 1 ) ) ? ( localDW ->
aeinmpqosl . Head [ 1 ] + 1 ) : 0 ) ; if ( localDW -> aeinmpqosl . Head [ 1 ]
== localDW -> aeinmpqosl . Tail [ 1 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf
( & localDW -> aeinmpqosl . CircularBufSize [ 1 ] , & localDW -> aeinmpqosl .
Tail [ 1 ] , & localDW -> aeinmpqosl . Head [ 1 ] , & localDW -> aeinmpqosl .
Last [ 1 ] , simTime - localP -> TransportDelay_Delay , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & localDW -> aeinmpqosl . MaxNewBufSize )
) { ssSetErrorStatus ( rtS_f , "tdelay memory allocation error" ) ; return ;
} } ( * tBuffer ++ ) [ localDW -> aeinmpqosl . Head [ 1 ] ] = simTime ; ( *
uBuffer ++ ) [ localDW -> aeinmpqosl . Head [ 1 ] ] = localB -> a3xvaqx1gx [
1 ] ; localDW -> aeinmpqosl . Head [ 2 ] = ( ( localDW -> aeinmpqosl . Head [
2 ] < ( localDW -> aeinmpqosl . CircularBufSize [ 2 ] - 1 ) ) ? ( localDW ->
aeinmpqosl . Head [ 2 ] + 1 ) : 0 ) ; if ( localDW -> aeinmpqosl . Head [ 2 ]
== localDW -> aeinmpqosl . Tail [ 2 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf
( & localDW -> aeinmpqosl . CircularBufSize [ 2 ] , & localDW -> aeinmpqosl .
Tail [ 2 ] , & localDW -> aeinmpqosl . Head [ 2 ] , & localDW -> aeinmpqosl .
Last [ 2 ] , simTime - localP -> TransportDelay_Delay , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & localDW -> aeinmpqosl . MaxNewBufSize )
) { ssSetErrorStatus ( rtS_f , "tdelay memory allocation error" ) ; return ;
} } ( * tBuffer ) [ localDW -> aeinmpqosl . Head [ 2 ] ] = simTime ; ( *
uBuffer ) [ localDW -> aeinmpqosl . Head [ 2 ] ] = localB -> a3xvaqx1gx [ 2 ]
; } if ( ssIsSampleHit ( rtS_f , 1 , 0 ) ) { localDW -> kdzyu5dfoi [ 0 ] =
localB -> gs4rvks0h4 [ 0 ] ; localDW -> kdzyu5dfoi [ 1 ] = localB ->
gs4rvks0h4 [ 1 ] ; localDW -> kdzyu5dfoi [ 2 ] = localB -> gs4rvks0h4 [ 2 ] ;
} } } void fdfvudi3q4 ( pq3igreacf * localB , o3cgxb1nm5 * localDW ,
fyvjfwczbw * localXdot ) { if ( localDW -> f2xlxmm2rg ) { localXdot ->
mr3ym4eux3 [ 0 ] = localB -> jeg1ixfqcc [ 0 ] ; localXdot -> mr3ym4eux3 [ 1 ]
= localB -> jeg1ixfqcc [ 1 ] ; localXdot -> mr3ym4eux3 [ 2 ] = localB ->
jeg1ixfqcc [ 2 ] ; } else { { real_T * dx ; int_T i ; dx = & ( localXdot ->
mr3ym4eux3 [ 0 ] ) ; for ( i = 0 ; i < 3 ; i ++ ) { dx [ i ] = 0.0 ; } } } }
void kqyojj2hz0 ( pq3igreacf * localB , o3cgxb1nm5 * localDW , klg0lcyohc *
localP , c3runus3ia * localZCSV ) { if ( localDW -> f2xlxmm2rg ) { localZCSV
-> huwxhf2bl0 [ 0 ] = localB -> gs4rvks0h4 [ 0 ] - localP ->
Saturationtoavoidnegativesqrt_UpperSat ; localZCSV -> k0flqjs0k3 [ 0 ] =
localB -> gs4rvks0h4 [ 0 ] - localP -> Saturationtoavoidnegativesqrt_LowerSat
; localZCSV -> huwxhf2bl0 [ 1 ] = localB -> gs4rvks0h4 [ 1 ] - localP ->
Saturationtoavoidnegativesqrt_UpperSat ; localZCSV -> k0flqjs0k3 [ 1 ] =
localB -> gs4rvks0h4 [ 1 ] - localP -> Saturationtoavoidnegativesqrt_LowerSat
; localZCSV -> huwxhf2bl0 [ 2 ] = localB -> gs4rvks0h4 [ 2 ] - localP ->
Saturationtoavoidnegativesqrt_UpperSat ; localZCSV -> k0flqjs0k3 [ 2 ] =
localB -> gs4rvks0h4 [ 2 ] - localP -> Saturationtoavoidnegativesqrt_LowerSat
; } else { { real_T * zcsv = & ( localZCSV -> huwxhf2bl0 [ 0 ] ) ; int_T i ;
for ( i = 0 ; i < 6 ; i ++ ) { zcsv [ i ] = 0.0 ; } } } } void b0l2el5i05 (
o3cgxb1nm5 * localDW ) { { int_T i1 ; void * * pw_TUbufferPtrs = & localDW ->
gwwponjnqb . TUbufferPtrs [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) {
rt_TDelayFreeBuf ( pw_TUbufferPtrs [ i1 ] ) ; } } } void MdlInitialize ( void
) { { SimStruct * rts = ssGetSFunction ( rtS , 2 ) ; sfcnInitializeConditions
( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct
* rts = ssGetSFunction ( rtS , 3 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 4 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 5 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 6 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 7 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 8 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 9 ) ; sfcnInitializeConditions ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtDW . jreglfqlm5 = rtP .
UnitDelay_InitialCondition_mp5gfaiube ; rtDW . ca3dtz0zaw = rtP .
RateLimiter_IC ; { int32_T i , j ; real_T * As = ( real_T * ) rtDW .
o1xie1rqxt . AS ; real_T * Bs = ( real_T * ) rtDW . o1xie1rqxt . BS ; real_T
* Cs = ( real_T * ) rtDW . o1xie1rqxt . CS ; real_T * Ds = ( real_T * ) rtDW
. o1xie1rqxt . DS ; real_T * X0 = ( real_T * ) & rtDW . hllwh2h1oo [ 0 ] ;
for ( i = 0 ; i < 34 ; i ++ ) { X0 [ i ] = ( rtP . StateSpace_X0_param [ i ]
) ; } for ( i = 0 ; i < 34 ; i ++ ) { for ( j = 0 ; j < 34 ; j ++ ) As [ i *
34 + j ] = ( rtP . StateSpace_AS_param [ i + j * 34 ] ) ; for ( j = 0 ; j <
35 ; j ++ ) Bs [ i * 35 + j ] = ( rtP . StateSpace_BS_param [ i + j * 34 ] )
; } for ( i = 0 ; i < 46 ; i ++ ) { for ( j = 0 ; j < 34 ; j ++ ) Cs [ i * 34
+ j ] = ( rtP . StateSpace_CS_param [ i + j * 46 ] ) ; } for ( i = 0 ; i < 46
; i ++ ) { for ( j = 0 ; j < 35 ; j ++ ) Ds [ i * 35 + j ] = ( rtP .
StateSpace_DS_param [ i + j * 46 ] ) ; } { int_T * switch_status = ( int_T *
) rtDW . o1xie1rqxt . SWITCH_STATUS ; int_T * gState = ( int_T * ) rtDW .
o1xie1rqxt . G_STATE ; real_T * yswitch = ( real_T * ) rtDW . o1xie1rqxt .
Y_SWITCH ; int_T * switchTypes = ( int_T * ) rtDW . o1xie1rqxt . SWITCH_TYPES
; int_T * idxOutSw = ( int_T * ) rtDW . o1xie1rqxt . IDX_OUT_SW ; int_T *
switch_status_init = ( int_T * ) rtDW . o1xie1rqxt . SWITCH_STATUS_INIT ;
switch_status [ 0 ] = 0 ; switch_status_init [ 0 ] = 0 ; gState [ 0 ] = (
int_T ) 0.0 ; yswitch [ 0 ] = 1 / 0.001 ; switchTypes [ 0 ] = ( int_T ) 7.0 ;
idxOutSw [ 0 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 1 ] = 0 ;
switch_status_init [ 1 ] = 0 ; gState [ 1 ] = ( int_T ) 0.0 ; yswitch [ 1 ] =
1 / 0.001 ; switchTypes [ 1 ] = ( int_T ) 7.0 ; idxOutSw [ 1 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 2 ] = 0 ; switch_status_init [ 2 ] = 0 ; gState [
2 ] = ( int_T ) 0.0 ; yswitch [ 2 ] = 1 / 0.001 ; switchTypes [ 2 ] = ( int_T
) 7.0 ; idxOutSw [ 2 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 3 ] = 0 ;
switch_status_init [ 3 ] = 0 ; gState [ 3 ] = ( int_T ) 0.0 ; yswitch [ 3 ] =
1 / 0.001 ; switchTypes [ 3 ] = ( int_T ) 7.0 ; idxOutSw [ 3 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 4 ] = 0 ; switch_status_init [ 4 ] = 0 ; gState [
4 ] = ( int_T ) 0.0 ; yswitch [ 4 ] = 1 / 0.001 ; switchTypes [ 4 ] = ( int_T
) 3.0 ; idxOutSw [ 4 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 5 ] = 0 ;
switch_status_init [ 5 ] = 0 ; gState [ 5 ] = ( int_T ) 0.0 ; yswitch [ 5 ] =
1 / 0.001 ; switchTypes [ 5 ] = ( int_T ) 3.0 ; idxOutSw [ 5 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 6 ] = 0 ; switch_status_init [ 6 ] = 0 ; gState [
6 ] = ( int_T ) 0.0 ; yswitch [ 6 ] = 1 / 0.001 ; switchTypes [ 6 ] = ( int_T
) 7.0 ; idxOutSw [ 6 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 7 ] = 0 ;
switch_status_init [ 7 ] = 0 ; gState [ 7 ] = ( int_T ) 0.0 ; yswitch [ 7 ] =
1 / 0.001 ; switchTypes [ 7 ] = ( int_T ) 7.0 ; idxOutSw [ 7 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 8 ] = 0 ; switch_status_init [ 8 ] = 0 ; gState [
8 ] = ( int_T ) 0.0 ; yswitch [ 8 ] = 1 / 0.001 ; switchTypes [ 8 ] = ( int_T
) 7.0 ; idxOutSw [ 8 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 9 ] = 0 ;
switch_status_init [ 9 ] = 0 ; gState [ 9 ] = ( int_T ) 0.0 ; yswitch [ 9 ] =
1 / 0.001 ; switchTypes [ 9 ] = ( int_T ) 7.0 ; idxOutSw [ 9 ] = ( ( int_T )
0.0 ) - 1 ; switch_status [ 10 ] = 0 ; switch_status_init [ 10 ] = 0 ; gState
[ 10 ] = ( int_T ) 0.0 ; yswitch [ 10 ] = 1 / 0.001 ; switchTypes [ 10 ] = (
int_T ) 3.0 ; idxOutSw [ 10 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 11 ]
= 0 ; switch_status_init [ 11 ] = 0 ; gState [ 11 ] = ( int_T ) 0.0 ; yswitch
[ 11 ] = 1 / 0.001 ; switchTypes [ 11 ] = ( int_T ) 3.0 ; idxOutSw [ 11 ] = (
( int_T ) 0.0 ) - 1 ; switch_status [ 12 ] = 0 ; switch_status_init [ 12 ] =
0 ; gState [ 12 ] = ( int_T ) 0.0 ; yswitch [ 12 ] = 1 / 0.001 ; switchTypes
[ 12 ] = ( int_T ) 7.0 ; idxOutSw [ 12 ] = ( ( int_T ) 0.0 ) - 1 ;
switch_status [ 13 ] = 0 ; switch_status_init [ 13 ] = 0 ; gState [ 13 ] = (
int_T ) 0.0 ; yswitch [ 13 ] = 1 / 0.001 ; switchTypes [ 13 ] = ( int_T ) 7.0
; idxOutSw [ 13 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 14 ] = 0 ;
switch_status_init [ 14 ] = 0 ; gState [ 14 ] = ( int_T ) 0.0 ; yswitch [ 14
] = 1 / 0.001 ; switchTypes [ 14 ] = ( int_T ) 7.0 ; idxOutSw [ 14 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 15 ] = 0 ; switch_status_init [ 15 ] = 0
; gState [ 15 ] = ( int_T ) 0.0 ; yswitch [ 15 ] = 1 / 0.001 ; switchTypes [
15 ] = ( int_T ) 7.0 ; idxOutSw [ 15 ] = ( ( int_T ) 0.0 ) - 1 ;
switch_status [ 16 ] = 0 ; switch_status_init [ 16 ] = 0 ; gState [ 16 ] = (
int_T ) 0.0 ; yswitch [ 16 ] = 1 / 0.001 ; switchTypes [ 16 ] = ( int_T ) 3.0
; idxOutSw [ 16 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 17 ] = 0 ;
switch_status_init [ 17 ] = 0 ; gState [ 17 ] = ( int_T ) 0.0 ; yswitch [ 17
] = 1 / 0.001 ; switchTypes [ 17 ] = ( int_T ) 3.0 ; idxOutSw [ 17 ] = ( (
int_T ) 0.0 ) - 1 ; } } rtDW . m1peilszpy = rtB . l4y5mcjvpy ; rtDW .
bvgqcazfkc = 2 ; rtDW . dnrqy23bwk = rtP . PI_InitialConditionForIntegrator ;
rtDW . jzbmotzdka [ 0 ] = rtP . PI_InitialConditionForIntegrator_bmolgnst2b ;
rtDW . jzbmotzdka [ 1 ] = rtP . PI_InitialConditionForIntegrator_bmolgnst2b ;
rtDW . jv0y1evb0u [ 0 ] = rtP . UnitDelay_InitialCondition_fy3uxswg0s ; rtDW
. jv0y1evb0u [ 1 ] = rtP . UnitDelay_InitialCondition_fy3uxswg0s ; rtDW .
jv0y1evb0u [ 2 ] = rtP . UnitDelay_InitialCondition_fy3uxswg0s ; rtDW .
giqv4pyyys = rtP . UnitDelay_InitialCondition_hvmmrz5whh ; rtDW . mwfdpj20rn
= rtP . Integ4_IC_f03w4yglrt ; { SimStruct * rts = ssGetSFunction ( rtS , 10
) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != (
NULL ) ) return ; } rtDW . m5fkweyhnz = rtP .
UnitDelay_InitialCondition_hw44anb1qk ; rtDW . oyi34irdce = rtP .
UnitDelay1_InitialCondition_dbtlgsoueo ; rtDW . fmpdeay2dq = rtP .
DiscreteDerivative_InitialStates ; rtDW . evobiqhvg0 = rtP . Discrete_Init ;
rtDW . aswhfhtphz = rtP . RateLimiter_IC_gnpihpl5du ; rtDW . c3rg43fwng = rtP
. Delay_x1_InitialCondition ; rtDW . puez01wvqo = rtP .
Delay_x2_InitialCondition ; rtDW . otc3kwibnn = rtP . Integ4_IC_mncfk0choj ;
{ SimStruct * rts = ssGetSFunction ( rtS , 11 ) ; sfcnInitializeConditions (
rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtDW .
inc2xczn1j = rtP . UnitDelay_InitialCondition_ma0l54m1mt ; rtDW . hglk0ykmnp
= rtP . UnitDelay1_InitialCondition_h0y3tjrtyc ; rtDW . iyz3blnd0r = rtP .
Integ4_IC_iz52cymmtl ; { SimStruct * rts = ssGetSFunction ( rtS , 12 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . okoa0ga0es = rtP . UnitDelay_InitialCondition_f1pmb4tsqq
; rtDW . mlqfus2yxb = rtP . UnitDelay1_InitialCondition_fmg3kvl22k ; rtDW .
cv44xoew41 = rtP . Integ4_IC_pitekfoys0 ; { SimStruct * rts = ssGetSFunction
( rtS , 13 ) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtDW . amx5pvlykq = rtP .
UnitDelay_InitialCondition_du0hifmua4 ; rtDW . l1nmdmtivq = rtP .
UnitDelay1_InitialCondition_bxzmk5e5pe ; rtDW . c3vwiq1oz1 = rtP .
Integ4_IC_p5hl13nikg ; { SimStruct * rts = ssGetSFunction ( rtS , 14 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . g55lsvsoyn = rtP . UnitDelay_InitialCondition_fluz1hs4rw
; rtDW . bf0dooycmi = rtP . UnitDelay1_InitialCondition_oxna4lbe4y ; rtDW .
is34tmu21f = rtP . Integ4_IC_ln1mjxwhre ; { SimStruct * rts = ssGetSFunction
( rtS , 15 ) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtDW . bgndzrawrf = rtP .
UnitDelay_InitialCondition_l3h1l0mohd ; rtDW . jwtcvtmaeo = rtP .
UnitDelay1_InitialCondition_mxloi3dinr ; rtDW . crf5cecvju = rtP .
Integ4_IC_g12toj1ern ; { SimStruct * rts = ssGetSFunction ( rtS , 16 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . o54g2pguii = rtP . UnitDelay_InitialCondition_ictydfu5gx
; rtDW . berzbzjptr = rtP . UnitDelay1_InitialCondition_ew2tfa0xiv ; rtDW .
eyj3xgyyy5 = rtP . Integ4_IC_kpvxlhwixo ; { SimStruct * rts = ssGetSFunction
( rtS , 17 ) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtDW . iwb0lp4dat = rtP .
UnitDelay_InitialCondition_gbfuarhyw0 ; rtDW . mmleedibcx = rtP .
UnitDelay1_InitialCondition_ajpewtdlxk ; rtDW . iwtvu1xbzm = rtP .
Integ4_IC_bcihf3qerl ; { SimStruct * rts = ssGetSFunction ( rtS , 18 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . fdgmxn5rpv = rtP . UnitDelay_InitialCondition_c1mnajnz03
; rtDW . mkbsymk41p = rtP . UnitDelay1_InitialCondition_fwlh0v4clc ; rtDW .
h4kspo12ff = rtP . Integ4_IC_ljmxfnx2ig ; { SimStruct * rts = ssGetSFunction
( rtS , 19 ) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtDW . ad2sn3k3gj = rtP .
UnitDelay_InitialCondition_d4fsgzspon ; rtDW . jy2blxntru = rtP .
UnitDelay1_InitialCondition_nwwwd2tqrt ; rtDW . dbrynerjdo = rtP .
Integ4_IC_b4moxyhjbs ; { SimStruct * rts = ssGetSFunction ( rtS , 20 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . ioxogrdks0 = rtP . UnitDelay_InitialCondition_iu5allnmxd
; rtDW . ejhppynn0z = rtP . UnitDelay1_InitialCondition_fsrsshkqoc ; rtDW .
dqddocajhg = rtP . Integ4_IC_etee4cslmc ; { SimStruct * rts = ssGetSFunction
( rtS , 21 ) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtDW . e2jfcom1xu = rtP .
UnitDelay_InitialCondition_bh2031guu1 ; rtDW . km0mzzn3gj = rtP .
UnitDelay1_InitialCondition_gcar3snp12 ; rtDW . llg10jep1x = rtP .
Integ4_IC_lektlq1jie ; { SimStruct * rts = ssGetSFunction ( rtS , 22 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . kkj5lgh0sd = rtP . UnitDelay_InitialCondition_nj2mac4354
; rtDW . oersb5zkry = rtP . UnitDelay1_InitialCondition_exqimte31v ; rtDW .
mxwbciposz = rtP . Integ4_IC_omqg2tqo15 ; { SimStruct * rts = ssGetSFunction
( rtS , 23 ) ; sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtDW . mvp44fdh2o = rtP .
UnitDelay_InitialCondition_drj2csvrhz ; rtDW . fwzzbi3hli = rtP .
UnitDelay1_InitialCondition_lzsilqa4qy ; rtDW . pjo1bnuqfk = rtP .
Integ4_IC_fzbpruzddz ; { SimStruct * rts = ssGetSFunction ( rtS , 24 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . h45pdgkmki = rtP . UnitDelay_InitialCondition_bp1tsi5gga
; rtDW . ldxbwl52qd = rtP . UnitDelay1_InitialCondition_ovdhgn1wwe ; rtX .
f2y4dmvcyy [ 0 ] = rtP . integrator_IC ; rtDW . dpquh1vse1 [ 0 ] = rtP .
Memory_InitialCondition ; rtX . nv45bjhe3t [ 0 ] = rtP .
integrator_IC_gfcwranmqu ; rtDW . mvp3sao44j [ 0 ] = rtP .
Memory_InitialCondition_j5vuqn1rvq ; rtX . f2y4dmvcyy [ 1 ] = rtP .
integrator_IC ; rtDW . dpquh1vse1 [ 1 ] = rtP . Memory_InitialCondition ; rtX
. nv45bjhe3t [ 1 ] = rtP . integrator_IC_gfcwranmqu ; rtDW . mvp3sao44j [ 1 ]
= rtP . Memory_InitialCondition_j5vuqn1rvq ; rtX . f2y4dmvcyy [ 2 ] = rtP .
integrator_IC ; rtDW . dpquh1vse1 [ 2 ] = rtP . Memory_InitialCondition ; rtX
. nv45bjhe3t [ 2 ] = rtP . integrator_IC_gfcwranmqu ; rtDW . mvp3sao44j [ 2 ]
= rtP . Memory_InitialCondition_j5vuqn1rvq ; brnjra0xun ( & rtB . ibrtt4ftsl
, & rtP . ibrtt4ftsl ) ; jspz1xohdo ( & rtB . bdvvkdwqfm , & rtP . bdvvkdwqfm
) ; rtDW . c1tw4ogoog = false ; rtDW . k2yf4d0qko = 0.0 ; rtDW . j0fknedvcm =
0.0 ; brnjra0xun ( & rtB . imgy1z4qes , & rtP . imgy1z4qes ) ; jspz1xohdo ( &
rtB . gusdfzbcqa , & rtP . gusdfzbcqa ) ; rtB . kk5fvlgob4 [ 0 ] = rtP .
Uref_Y0 ; rtB . kve2c32fv4 [ 0 ] = rtP . Uref_Y0_jnwoqle1jv ; rtB .
khgh0in0mh [ 0 ] = rtP . Uref_Y0_dmfzsftjfi ; rtB . kk5fvlgob4 [ 1 ] = rtP .
Uref_Y0 ; rtB . kve2c32fv4 [ 1 ] = rtP . Uref_Y0_jnwoqle1jv ; rtB .
khgh0in0mh [ 1 ] = rtP . Uref_Y0_dmfzsftjfi ; rtB . kk5fvlgob4 [ 2 ] = rtP .
Uref_Y0 ; rtB . kve2c32fv4 [ 2 ] = rtP . Uref_Y0_jnwoqle1jv ; rtB .
khgh0in0mh [ 2 ] = rtP . Uref_Y0_dmfzsftjfi ; rtDW . lzgy3px31z = rtP .
Integ4_IC ; { SimStruct * rts = ssGetSFunction ( rtS , 0 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . omufifkqbz = rtP . UnitDelay_InitialCondition ; rtDW .
nndlftph12 = rtP . UnitDelay1_InitialCondition ; rtDW . ohydhrpaay = rtP .
Integ4_IC_bmrwd2awok ; { SimStruct * rts = ssGetSFunction ( rtS , 1 ) ;
sfcnInitializeConditions ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL )
) return ; } rtDW . eznk4sr03e = rtP . UnitDelay_InitialCondition_jycnfiyaap
; rtDW . aktl42ci5l = rtP . UnitDelay1_InitialCondition_eoex0bhdpa ;
brnjra0xun ( & rtB . flkfnl3dquj , & rtP . flkfnl3dquj ) ; jspz1xohdo ( & rtB
. fiy4znthvv3 , & rtP . fiy4znthvv3 ) ; rtB . puricsu5uo = rtP . Gain_Y0 ;
brnjra0xun ( & rtB . gvjufizpfu , & rtP . gvjufizpfu ) ; jspz1xohdo ( & rtB .
h0t1xli1qx , & rtP . h0t1xli1qx ) ; rtB . esbgstqc1k = rtP . S_Y0 [ 0 ] ; rtB
. iqsu5vb0qp = rtP . S_Y0 [ 1 ] ; rtB . lhrhljsi0l [ 0 ] = rtP .
S_Y0_o3ff3poqj5 ; rtB . lhrhljsi0l [ 1 ] = rtP . S_Y0_o3ff3poqj5 ; rtB .
lhrhljsi0l [ 2 ] = rtP . S_Y0_o3ff3poqj5 ; pvdad0deeo ( & rtDW . kyedbo0mxuc
, & rtP . kyedbo0mxuc , & rtX . kyedbo0mxuc ) ; i4sjz31q1b ( & rtDW .
gllp1vimnhl , & rtP . gllp1vimnhl , & rtX . gllp1vimnhl ) ; pvdad0deeo ( &
rtDW . oa2z5wykl2 , & rtP . oa2z5wykl2 , & rtX . oa2z5wykl2 ) ; i4sjz31q1b (
& rtDW . e2mpwvt2ua , & rtP . e2mpwvt2ua , & rtX . e2mpwvt2ua ) ; i4sjz31q1b
( & rtDW . jhi4zkfadx , & rtP . jhi4zkfadx , & rtX . jhi4zkfadx ) ;
pvdad0deeo ( & rtDW . mdskmorhmi , & rtP . mdskmorhmi , & rtX . mdskmorhmi )
; i4sjz31q1b ( & rtDW . blrugcnivv , & rtP . blrugcnivv , & rtX . blrugcnivv
) ; pvdad0deeo ( & rtDW . iks0grgus1 , & rtP . iks0grgus1 , & rtX .
iks0grgus1 ) ; i4sjz31q1b ( & rtDW . glvlabfrtp , & rtP . glvlabfrtp , & rtX
. glvlabfrtp ) ; pvdad0deeo ( & rtDW . drtk0acyhp , & rtP . drtk0acyhp , &
rtX . drtk0acyhp ) ; i4sjz31q1b ( & rtDW . daw0ymwmw4 , & rtP . daw0ymwmw4 ,
& rtX . daw0ymwmw4 ) ; pvdad0deeo ( & rtDW . pu1c2mp3al , & rtP . pu1c2mp3al
, & rtX . pu1c2mp3al ) ; } void MdlEnable ( void ) { rtDW . jyxrya54ck = 1 ;
rtDW . kc2urh0o2k = 1 ; rtDW . gwu3cfmboo = 1 ; rtDW . oneka2f33e = 1U ; rtDW
. gu2gnqzjnr = 1 ; rtDW . fcwaufktro = 1U ; rtDW . oaicnmy4cr = 1U ; rtDW .
oi4s4s3b2v = 1 ; rtDW . d5srsnrycq = 1U ; rtDW . bysaiwysza = 1 ; rtDW .
acdm1beais = 1U ; rtDW . fp5gljg1na = 1 ; rtDW . i4s40mrg1p = 1U ; rtDW .
f05y3hvfbl = 1 ; rtDW . e4uypnvxm4 = 1U ; rtDW . nzuplyapo4 = 1 ; rtDW .
leakran3jy = 1U ; rtDW . aaw5cbewzs = 1 ; rtDW . grw0tq5110 = 1U ; rtDW .
fhmti5fnmk = 1 ; rtDW . ir0a11kmwf = 1U ; rtDW . ofkxr3wbpq = 1 ; rtDW .
bixjjcy32b = 1U ; rtDW . lmxa3ssfz4 = 1 ; rtDW . lob2ph3yhu = 1U ; rtDW .
kso0zmov5s = 1 ; rtDW . fuhanhc4z4 = 1U ; rtDW . gqdl0mj0xk = 1 ; rtDW .
phos00jdib = 1U ; rtDW . bn3v01mqq4 = 1 ; rtDW . ckxcx1vdsj = 1U ; } void
MdlStart ( void ) { { void * * slioCatalogueAddr = rt_slioCatalogueAddr ( ) ;
void * r2 = ( NULL ) ; void * * pOSigstreamManagerAddr = ( NULL ) ; const int
maxErrorBufferSize = 16384 ; char errMsgCreatingOSigstreamManager [ 16384 ] ;
bool errorCreatingOSigstreamManager = false ; const char *
errorAddingR2SharedResource = ( NULL ) ; * slioCatalogueAddr =
rtwGetNewSlioCatalogue ( rt_GetMatSigLogSelectorFileName ( ) ) ;
errorAddingR2SharedResource = rtwAddR2SharedResource (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) , 1 ) ; if (
errorAddingR2SharedResource != ( NULL ) ) { rtwTerminateSlioCatalogue (
slioCatalogueAddr ) ; * slioCatalogueAddr = ( NULL ) ; ssSetErrorStatus ( rtS
, errorAddingR2SharedResource ) ; return ; } r2 = rtwGetR2SharedResource (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) ) ;
pOSigstreamManagerAddr = rt_GetOSigstreamManagerAddr ( ) ;
errorCreatingOSigstreamManager = rtwOSigstreamManagerCreateInstance (
rt_GetMatSigLogSelectorFileName ( ) , r2 , pOSigstreamManagerAddr ,
errMsgCreatingOSigstreamManager , maxErrorBufferSize ) ; if (
errorCreatingOSigstreamManager ) { * pOSigstreamManagerAddr = ( NULL ) ;
ssSetErrorStatus ( rtS , errMsgCreatingOSigstreamManager ) ; return ; } } {
bool externalInputIsInDatasetFormat = false ; void * pISigstreamManager =
rt_GetISigstreamManager ( ) ; rtwISigstreamManagerGetInputIsInDatasetFormat (
pISigstreamManager , & externalInputIsInDatasetFormat ) ; if (
externalInputIsInDatasetFormat ) { } } { RTWLogSignalInfo rt_ScopeSignalInfo
; static int_T rt_ScopeSignalWidths [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalNumDimensions [ ] = { 1 , 1 } ; static int_T
rt_ScopeSignalDimensions [ ] = { 1 , 1 } ; static void * rt_ScopeCurrSigDims
[ ] = { ( NULL ) , ( NULL ) } ; static int_T rt_ScopeCurrSigDimsSize [ ] = {
4 , 4 } ; static const char_T * rt_ScopeSignalLabels [ ] = { "Va (V)" ,
"Ia (A)" } ; static char_T rt_ScopeSignalTitles [ ] = "Va (V)Ia (A)" ; static
int_T rt_ScopeSignalTitleLengths [ ] = { 6 , 6 } ; static boolean_T
rt_ScopeSignalIsVarDims [ ] = { 0 , 0 } ; static int_T
rt_ScopeSignalPlotStyles [ ] = { 1 , 1 } ; BuiltInDTypeId dTypes [ 2 ] = {
SS_DOUBLE , SS_DOUBLE } ; static char_T rt_ScopeBlockName [ ] =
"PV_Array_Model/Va Ia B1" ; static int_T rt_ScopeFrameData [ ] = { 0 , 0 } ;
static RTWPreprocessingFcnPtr rt_ScopeSignalLoggingPreprocessingFcnPtrs [ ] =
{ ( NULL ) , ( NULL ) } ; rt_ScopeSignalInfo . numSignals = 2 ;
rt_ScopeSignalInfo . numCols = rt_ScopeSignalWidths ; rt_ScopeSignalInfo .
numDims = rt_ScopeSignalNumDimensions ; rt_ScopeSignalInfo . dims =
rt_ScopeSignalDimensions ; rt_ScopeSignalInfo . isVarDims =
rt_ScopeSignalIsVarDims ; rt_ScopeSignalInfo . currSigDims =
rt_ScopeCurrSigDims ; rt_ScopeSignalInfo . currSigDimsSize =
rt_ScopeCurrSigDimsSize ; rt_ScopeSignalInfo . dataTypes = dTypes ;
rt_ScopeSignalInfo . complexSignals = ( NULL ) ; rt_ScopeSignalInfo .
frameData = rt_ScopeFrameData ; rt_ScopeSignalInfo . preprocessingPtrs =
rt_ScopeSignalLoggingPreprocessingFcnPtrs ; rt_ScopeSignalInfo . labels .
cptr = rt_ScopeSignalLabels ; rt_ScopeSignalInfo . titles =
rt_ScopeSignalTitles ; rt_ScopeSignalInfo . titleLengths =
rt_ScopeSignalTitleLengths ; rt_ScopeSignalInfo . plotStyles =
rt_ScopeSignalPlotStyles ; rt_ScopeSignalInfo . blockNames . cptr = ( NULL )
; rt_ScopeSignalInfo . stateNames . cptr = ( NULL ) ; rt_ScopeSignalInfo .
crossMdlRef = ( NULL ) ; rt_ScopeSignalInfo . dataTypeConvert = ( NULL ) ;
rtDW . jquzhmx440 . LoggedData [ 0 ] = rt_CreateStructLogVar (
ssGetRTWLogInfo ( rtS ) , ssGetTStart ( rtS ) , ssGetTFinal ( rtS ) , 0.0 , (
& ssGetErrorStatus ( rtS ) ) , "VaIa_Grid" , 1 , 0 , 1 ,
5.0505050505050507E-6 , & rt_ScopeSignalInfo , rt_ScopeBlockName ) ; if (
rtDW . jquzhmx440 . LoggedData [ 0 ] == ( NULL ) ) return ; } {
RTWLogSignalInfo rt_ScopeSignalInfo ; static int_T rt_ScopeSignalWidths [ ] =
{ 1 } ; static int_T rt_ScopeSignalNumDimensions [ ] = { 1 } ; static int_T
rt_ScopeSignalDimensions [ ] = { 1 } ; static void * rt_ScopeCurrSigDims [ ]
= { ( NULL ) } ; static int_T rt_ScopeCurrSigDimsSize [ ] = { 4 } ; static
const char_T * rt_ScopeSignalLabels [ ] = { "" } ; static char_T
rt_ScopeSignalTitles [ ] = "" ; static int_T rt_ScopeSignalTitleLengths [ ] =
{ 0 } ; static boolean_T rt_ScopeSignalIsVarDims [ ] = { 0 } ; static int_T
rt_ScopeSignalPlotStyles [ ] = { 1 } ; BuiltInDTypeId dTypes [ 1 ] = {
SS_DOUBLE } ; static char_T rt_ScopeBlockName [ ] =
"PV_Array_Model/Subsystem1/Vab_Inverter" ; static int_T rt_ScopeFrameData [ ]
= { 0 } ; static RTWPreprocessingFcnPtr
rt_ScopeSignalLoggingPreprocessingFcnPtrs [ ] = { ( NULL ) } ;
rt_ScopeSignalInfo . numSignals = 1 ; rt_ScopeSignalInfo . numCols =
rt_ScopeSignalWidths ; rt_ScopeSignalInfo . numDims =
rt_ScopeSignalNumDimensions ; rt_ScopeSignalInfo . dims =
rt_ScopeSignalDimensions ; rt_ScopeSignalInfo . isVarDims =
rt_ScopeSignalIsVarDims ; rt_ScopeSignalInfo . currSigDims =
rt_ScopeCurrSigDims ; rt_ScopeSignalInfo . currSigDimsSize =
rt_ScopeCurrSigDimsSize ; rt_ScopeSignalInfo . dataTypes = dTypes ;
rt_ScopeSignalInfo . complexSignals = ( NULL ) ; rt_ScopeSignalInfo .
frameData = rt_ScopeFrameData ; rt_ScopeSignalInfo . preprocessingPtrs =
rt_ScopeSignalLoggingPreprocessingFcnPtrs ; rt_ScopeSignalInfo . labels .
cptr = rt_ScopeSignalLabels ; rt_ScopeSignalInfo . titles =
rt_ScopeSignalTitles ; rt_ScopeSignalInfo . titleLengths =
rt_ScopeSignalTitleLengths ; rt_ScopeSignalInfo . plotStyles =
rt_ScopeSignalPlotStyles ; rt_ScopeSignalInfo . blockNames . cptr = ( NULL )
; rt_ScopeSignalInfo . stateNames . cptr = ( NULL ) ; rt_ScopeSignalInfo .
crossMdlRef = ( NULL ) ; rt_ScopeSignalInfo . dataTypeConvert = ( NULL ) ;
rtDW . iek4salud1 . LoggedData = rt_CreateStructLogVar ( ssGetRTWLogInfo (
rtS ) , ssGetTStart ( rtS ) , ssGetTFinal ( rtS ) , 0.0 , ( &
ssGetErrorStatus ( rtS ) ) , "Vab_VSC1" , 1 , 0 , 1 , 5.0505050505050507E-6 ,
& rt_ScopeSignalInfo , rt_ScopeBlockName ) ; if ( rtDW . iek4salud1 .
LoggedData == ( NULL ) ) return ; } rtDW . iy4nvvktj2 = false ;
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; { SimStruct * rts =
ssGetSFunction ( rtS , 0 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } { SimStruct * rts = ssGetSFunction ( rtS , 1 ) ;
sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; }
rtB . o4gd0y3v5b [ 0 ] = rtP . Constant_Value_jsrxor425n [ 0 ] ; rtB .
o4gd0y3v5b [ 1 ] = rtP . Constant_Value_jsrxor425n [ 1 ] ; rtB . o4gd0y3v5b [
2 ] = rtP . Constant_Value_jsrxor425n [ 2 ] ; { SimStruct * rts =
ssGetSFunction ( rtS , 2 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtB . dywo4uhhag [ 0 ] = rtP . Constant1_Value [ 0
] ; rtB . dywo4uhhag [ 1 ] = rtP . Constant1_Value [ 1 ] ; rtB . dywo4uhhag [
2 ] = rtP . Constant1_Value [ 2 ] ; { SimStruct * rts = ssGetSFunction ( rtS
, 3 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } { SimStruct * rts = ssGetSFunction ( rtS , 4 ) ; sfcnStart ( rts )
; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 5 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtB . ft0dn4u15h [ 0 ] = rtP .
Constant_Value_g4zd2j3sx3 [ 0 ] ; rtB . ft0dn4u15h [ 1 ] = rtP .
Constant_Value_g4zd2j3sx3 [ 1 ] ; rtB . ft0dn4u15h [ 2 ] = rtP .
Constant_Value_g4zd2j3sx3 [ 2 ] ; { SimStruct * rts = ssGetSFunction ( rtS ,
6 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return
; } rtB . ekwirjyast [ 0 ] = rtP . Constant1_Value_ay2jtmeeno [ 0 ] ; rtB .
ekwirjyast [ 1 ] = rtP . Constant1_Value_ay2jtmeeno [ 1 ] ; rtB . ekwirjyast
[ 2 ] = rtP . Constant1_Value_ay2jtmeeno [ 2 ] ; { SimStruct * rts =
ssGetSFunction ( rtS , 7 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } { SimStruct * rts = ssGetSFunction ( rtS , 8 ) ;
sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } {
SimStruct * rts = ssGetSFunction ( rtS , 9 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . SineWaveA_Hsin =
muDoubleScalarSin ( 6.2831853071795862 * rtP . Fnom * 5.0505050505050507E-6 )
; rtP . SineWaveA_HCos = muDoubleScalarCos ( 6.2831853071795862 * rtP . Fnom
* 5.0505050505050507E-6 ) ; rtP . SineWaveA_PSin = muDoubleScalarSin ( 0.0 -
6.2831853071795862 * rtP . Fnom * 5.0505050505050507E-6 ) ; rtP .
SineWaveA_PCos = muDoubleScalarCos ( 0.0 - 6.2831853071795862 * rtP . Fnom *
5.0505050505050507E-6 ) ; rtP . SineWaveB_Hsin = muDoubleScalarSin (
6.2831853071795862 * rtP . Fnom * 5.0505050505050507E-6 ) ; rtP .
SineWaveB_HCos = muDoubleScalarCos ( 6.2831853071795862 * rtP . Fnom *
5.0505050505050507E-6 ) ; rtP . SineWaveB_PSin = muDoubleScalarSin ( -
2.0943951023931953 - 6.2831853071795862 * rtP . Fnom * 5.0505050505050507E-6
) ; rtP . SineWaveB_PCos = muDoubleScalarCos ( - 2.0943951023931953 -
6.2831853071795862 * rtP . Fnom * 5.0505050505050507E-6 ) ; rtP .
SineWaveC_Hsin = muDoubleScalarSin ( 6.2831853071795862 * rtP . Fnom *
5.0505050505050507E-6 ) ; rtP . SineWaveC_HCos = muDoubleScalarCos (
6.2831853071795862 * rtP . Fnom * 5.0505050505050507E-6 ) ; rtP .
SineWaveC_PSin = muDoubleScalarSin ( 2.0943951023931953 - 6.2831853071795862
* rtP . Fnom * 5.0505050505050507E-6 ) ; rtP . SineWaveC_PCos =
muDoubleScalarCos ( 2.0943951023931953 - 6.2831853071795862 * rtP . Fnom *
5.0505050505050507E-6 ) ; { rtDW . o1xie1rqxt . AS = ( real_T * ) calloc ( 34
* 34 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . BS = ( real_T * ) calloc (
34 * 35 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . CS = ( real_T * ) calloc
( 46 * 34 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . DS = ( real_T * )
calloc ( 46 * 35 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . DX_COL = (
real_T * ) calloc ( 46 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . TMP2 = (
real_T * ) calloc ( 35 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . BD_COL = (
real_T * ) calloc ( 34 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . TMP1 = (
real_T * ) calloc ( 34 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt . XTMP = (
real_T * ) calloc ( 34 , sizeof ( real_T ) ) ; rtDW . o1xie1rqxt .
SWITCH_STATUS = ( int_T * ) calloc ( 18 , sizeof ( int_T ) ) ; rtDW .
o1xie1rqxt . SW_CHG = ( int_T * ) calloc ( 18 , sizeof ( int_T ) ) ; rtDW .
o1xie1rqxt . G_STATE = ( int_T * ) calloc ( 18 , sizeof ( int_T ) ) ; rtDW .
o1xie1rqxt . Y_SWITCH = ( real_T * ) calloc ( 18 , sizeof ( real_T ) ) ; rtDW
. o1xie1rqxt . SWITCH_TYPES = ( int_T * ) calloc ( 18 , sizeof ( int_T ) ) ;
rtDW . o1xie1rqxt . IDX_OUT_SW = ( int_T * ) calloc ( 18 , sizeof ( int_T ) )
; rtDW . o1xie1rqxt . SWITCH_STATUS_INIT = ( int_T * ) calloc ( 18 , sizeof (
int_T ) ) ; rtDW . o1xie1rqxt . USWLAST = ( real_T * ) calloc ( 18 , sizeof (
real_T ) ) ; } rtB . l4y5mcjvpy = rtP . Initial_Value ; rtDW . mktm0brmw5 =
true ; { SimStruct * rts = ssGetSFunction ( rtS , 10 ) ; sfcnStart ( rts ) ;
if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . SineWave_Hsin [
0 ] = muDoubleScalarSin ( rtP . SineWave_Freq * 5.0505050505050507E-6 ) ; rtP
. SineWave_HCos [ 0 ] = muDoubleScalarCos ( rtP . SineWave_Freq *
5.0505050505050507E-6 ) ; rtP . SineWave_Hsin [ 1 ] = muDoubleScalarSin ( rtP
. SineWave_Freq * 5.0505050505050507E-6 ) ; rtP . SineWave_HCos [ 1 ] =
muDoubleScalarCos ( rtP . SineWave_Freq * 5.0505050505050507E-6 ) ; rtP .
SineWave_Hsin [ 2 ] = muDoubleScalarSin ( rtP . SineWave_Freq *
5.0505050505050507E-6 ) ; rtP . SineWave_HCos [ 2 ] = muDoubleScalarCos ( rtP
. SineWave_Freq * 5.0505050505050507E-6 ) ; rtP . SineWave_PSin [ 0 ] =
muDoubleScalarSin ( 0.0 - rtP . SineWave_Freq * 5.0505050505050507E-6 ) ; rtP
. SineWave_PCos [ 0 ] = muDoubleScalarCos ( 0.0 - rtP . SineWave_Freq *
5.0505050505050507E-6 ) ; rtP . SineWave_PSin [ 1 ] = muDoubleScalarSin ( -
2.0943951023931953 - rtP . SineWave_Freq * 5.0505050505050507E-6 ) ; rtP .
SineWave_PCos [ 1 ] = muDoubleScalarCos ( - 2.0943951023931953 - rtP .
SineWave_Freq * 5.0505050505050507E-6 ) ; rtP . SineWave_PSin [ 2 ] =
muDoubleScalarSin ( 2.0943951023931953 - rtP . SineWave_Freq *
5.0505050505050507E-6 ) ; rtP . SineWave_PCos [ 2 ] = muDoubleScalarCos (
2.0943951023931953 - rtP . SineWave_Freq * 5.0505050505050507E-6 ) ; rtB .
mkrkhji0cg = rtP . K1_Value ; { SimStruct * rts = ssGetSFunction ( rtS , 11 )
; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; }
rtB . l12i0pjkca = rtP . K1_Value_jbgdaiaown ; { SimStruct * rts =
ssGetSFunction ( rtS , 12 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts
) != ( NULL ) ) return ; } rtP . sinwt_Hsin = muDoubleScalarSin ( rtP .
sinwt_Freq * 5.0505050505050505E-5 ) ; rtP . sinwt_HCos = muDoubleScalarCos (
rtP . sinwt_Freq * 5.0505050505050505E-5 ) ; rtP . sinwt_PSin =
muDoubleScalarSin ( 0.0 - rtP . sinwt_Freq * 5.0505050505050505E-5 ) ; rtP .
sinwt_PCos = muDoubleScalarCos ( 0.0 - rtP . sinwt_Freq *
5.0505050505050505E-5 ) ; rtB . lvxda1zemz = rtP . K1_Value_mnxxbxkzc3 ; {
SimStruct * rts = ssGetSFunction ( rtS , 13 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . coswt_Hsin =
muDoubleScalarSin ( rtP . coswt_Freq * 5.0505050505050505E-5 ) ; rtP .
coswt_HCos = muDoubleScalarCos ( rtP . coswt_Freq * 5.0505050505050505E-5 ) ;
rtP . coswt_PSin = muDoubleScalarSin ( 1.5707963267948966 - rtP . coswt_Freq
* 5.0505050505050505E-5 ) ; rtP . coswt_PCos = muDoubleScalarCos (
1.5707963267948966 - rtP . coswt_Freq * 5.0505050505050505E-5 ) ; rtB .
hdh2j1iy05 = rtP . K1_Value_mgtlppjp42 ; { SimStruct * rts = ssGetSFunction (
rtS , 14 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtP . sinwt_Hsin_e0rgtjbjpx = muDoubleScalarSin ( rtP .
sinwt_Freq_p3beiezqka * 5.0505050505050505E-5 ) ; rtP . sinwt_HCos_cafhkcfovv
= muDoubleScalarCos ( rtP . sinwt_Freq_p3beiezqka * 5.0505050505050505E-5 ) ;
rtP . sinwt_PSin_modl3cgbwm = muDoubleScalarSin ( 0.0 - rtP .
sinwt_Freq_p3beiezqka * 5.0505050505050505E-5 ) ; rtP . sinwt_PCos_cyveu5wuih
= muDoubleScalarCos ( 0.0 - rtP . sinwt_Freq_p3beiezqka *
5.0505050505050505E-5 ) ; rtB . psu4dh2yoj = rtP . K1_Value_exdwlktlap ; {
SimStruct * rts = ssGetSFunction ( rtS , 15 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . coswt_Hsin_omgodzfzvz
= muDoubleScalarSin ( rtP . coswt_Freq_gttzxcelpy * 5.0505050505050505E-5 ) ;
rtP . coswt_HCos_oz23s2cxn2 = muDoubleScalarCos ( rtP . coswt_Freq_gttzxcelpy
* 5.0505050505050505E-5 ) ; rtP . coswt_PSin_khvpsemk1b = muDoubleScalarSin (
1.5707963267948966 - rtP . coswt_Freq_gttzxcelpy * 5.0505050505050505E-5 ) ;
rtP . coswt_PCos_ontpys2zdd = muDoubleScalarCos ( 1.5707963267948966 - rtP .
coswt_Freq_gttzxcelpy * 5.0505050505050505E-5 ) ; rtB . bxlm4t4z32 = rtP .
K1_Value_g33io5pe2n ; { SimStruct * rts = ssGetSFunction ( rtS , 16 ) ;
sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; }
rtP . sinwt_Hsin_ltv4wuiu5i = muDoubleScalarSin ( rtP . sinwt_Freq_ibh040bo2d
* 5.0505050505050505E-5 ) ; rtP . sinwt_HCos_lozsdsini2 = muDoubleScalarCos (
rtP . sinwt_Freq_ibh040bo2d * 5.0505050505050505E-5 ) ; rtP .
sinwt_PSin_ism3nzmned = muDoubleScalarSin ( 0.0 - rtP . sinwt_Freq_ibh040bo2d
* 5.0505050505050505E-5 ) ; rtP . sinwt_PCos_esk52xey53 = muDoubleScalarCos (
0.0 - rtP . sinwt_Freq_ibh040bo2d * 5.0505050505050505E-5 ) ; rtB .
a2i2ov02lb = rtP . K1_Value_nofhnhfgqe ; { SimStruct * rts = ssGetSFunction (
rtS , 17 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtP . coswt_Hsin_p0lqeycuo2 = muDoubleScalarSin ( rtP .
coswt_Freq_gp2xaew4z5 * 5.0505050505050505E-5 ) ; rtP . coswt_HCos_e1lcq0jydt
= muDoubleScalarCos ( rtP . coswt_Freq_gp2xaew4z5 * 5.0505050505050505E-5 ) ;
rtP . coswt_PSin_pzecylb5ie = muDoubleScalarSin ( 1.5707963267948966 - rtP .
coswt_Freq_gp2xaew4z5 * 5.0505050505050505E-5 ) ; rtP . coswt_PCos_apooxuqtce
= muDoubleScalarCos ( 1.5707963267948966 - rtP . coswt_Freq_gp2xaew4z5 *
5.0505050505050505E-5 ) ; rtB . pf0efcoh3z = rtP . K1_Value_piw4pfcum5 ; {
SimStruct * rts = ssGetSFunction ( rtS , 18 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . sinwt_Hsin_f4ff1jm01o
= muDoubleScalarSin ( rtP . sinwt_Freq_g4kja400xj * 5.0505050505050505E-5 ) ;
rtP . sinwt_HCos_ikm4ke3nt1 = muDoubleScalarCos ( rtP . sinwt_Freq_g4kja400xj
* 5.0505050505050505E-5 ) ; rtP . sinwt_PSin_mxp5pxyazu = muDoubleScalarSin (
0.0 - rtP . sinwt_Freq_g4kja400xj * 5.0505050505050505E-5 ) ; rtP .
sinwt_PCos_ffxrlya5uh = muDoubleScalarCos ( 0.0 - rtP . sinwt_Freq_g4kja400xj
* 5.0505050505050505E-5 ) ; rtB . fvjbmcpghe = rtP . K1_Value_ly41q0lfne ; {
SimStruct * rts = ssGetSFunction ( rtS , 19 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . coswt_Hsin_ki40gemxaa
= muDoubleScalarSin ( rtP . coswt_Freq_iohcbhspnz * 5.0505050505050505E-5 ) ;
rtP . coswt_HCos_ih2fmjgzvo = muDoubleScalarCos ( rtP . coswt_Freq_iohcbhspnz
* 5.0505050505050505E-5 ) ; rtP . coswt_PSin_ckpub3oin5 = muDoubleScalarSin (
1.5707963267948966 - rtP . coswt_Freq_iohcbhspnz * 5.0505050505050505E-5 ) ;
rtP . coswt_PCos_jgy3xycijn = muDoubleScalarCos ( 1.5707963267948966 - rtP .
coswt_Freq_iohcbhspnz * 5.0505050505050505E-5 ) ; rtB . gmfn04oapy = rtP .
K1_Value_odshxihjs1 ; { SimStruct * rts = ssGetSFunction ( rtS , 20 ) ;
sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; }
rtP . sinwt_Hsin_i0nd0ubqqg = muDoubleScalarSin ( rtP . sinwt_Freq_pkofylmhnt
* 5.0505050505050505E-5 ) ; rtP . sinwt_HCos_c3em0todhq = muDoubleScalarCos (
rtP . sinwt_Freq_pkofylmhnt * 5.0505050505050505E-5 ) ; rtP .
sinwt_PSin_bllazdudnu = muDoubleScalarSin ( 0.0 - rtP . sinwt_Freq_pkofylmhnt
* 5.0505050505050505E-5 ) ; rtP . sinwt_PCos_ilquu0arpw = muDoubleScalarCos (
0.0 - rtP . sinwt_Freq_pkofylmhnt * 5.0505050505050505E-5 ) ; rtB .
ilbet5yid2 = rtP . K1_Value_fascbhcv3t ; { SimStruct * rts = ssGetSFunction (
rtS , 21 ) ; sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtP . coswt_Hsin_fuzcqltv4d = muDoubleScalarSin ( rtP .
coswt_Freq_aek3lx4btw * 5.0505050505050505E-5 ) ; rtP . coswt_HCos_cs3y4qtzeb
= muDoubleScalarCos ( rtP . coswt_Freq_aek3lx4btw * 5.0505050505050505E-5 ) ;
rtP . coswt_PSin_bdolgwlxdg = muDoubleScalarSin ( 1.5707963267948966 - rtP .
coswt_Freq_aek3lx4btw * 5.0505050505050505E-5 ) ; rtP . coswt_PCos_oqm4st2je0
= muDoubleScalarCos ( 1.5707963267948966 - rtP . coswt_Freq_aek3lx4btw *
5.0505050505050505E-5 ) ; rtB . nabvxanmoe = rtP . K1_Value_okiumpl42l ; {
SimStruct * rts = ssGetSFunction ( rtS , 22 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . sinwt_Hsin_kfaxcimxsb
= muDoubleScalarSin ( rtP . sinwt_Freq_i5xw5disvz * 5.0505050505050505E-5 ) ;
rtP . sinwt_HCos_lv5zpnsoyo = muDoubleScalarCos ( rtP . sinwt_Freq_i5xw5disvz
* 5.0505050505050505E-5 ) ; rtP . sinwt_PSin_bs1ts2ayri = muDoubleScalarSin (
0.0 - rtP . sinwt_Freq_i5xw5disvz * 5.0505050505050505E-5 ) ; rtP .
sinwt_PCos_owldbifmfn = muDoubleScalarCos ( 0.0 - rtP . sinwt_Freq_i5xw5disvz
* 5.0505050505050505E-5 ) ; rtB . inbgwouqfx = rtP . K1_Value_fl2t5hhsxt ; {
SimStruct * rts = ssGetSFunction ( rtS , 23 ) ; sfcnStart ( rts ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtP . coswt_Hsin_nt154hil4g
= muDoubleScalarSin ( rtP . coswt_Freq_hhpfztkccd * 5.0505050505050505E-5 ) ;
rtP . coswt_HCos_dtrnnwyvoi = muDoubleScalarCos ( rtP . coswt_Freq_hhpfztkccd
* 5.0505050505050505E-5 ) ; rtP . coswt_PSin_pc2mzqmykn = muDoubleScalarSin (
1.5707963267948966 - rtP . coswt_Freq_hhpfztkccd * 5.0505050505050505E-5 ) ;
rtP . coswt_PCos_exeoqhe2el = muDoubleScalarCos ( 1.5707963267948966 - rtP .
coswt_Freq_hhpfztkccd * 5.0505050505050505E-5 ) ; rtB . l1d3sn10xu = rtP .
K1_Value_huwvvbdsqd ; { SimStruct * rts = ssGetSFunction ( rtS , 24 ) ;
sfcnStart ( rts ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; }
h0ryf0cnmm ( rtS , & rtDW . kyedbo0mxuc , & rtP . kyedbo0mxuc , & ( ( XDis *
) ssGetContStateDisabled ( rtS ) ) -> kyedbo0mxuc ) ; cvm1yt3zx5 ( rtS , &
rtDW . gllp1vimnhl , & rtP . gllp1vimnhl , & ( ( XDis * )
ssGetContStateDisabled ( rtS ) ) -> gllp1vimnhl ) ; h0ryf0cnmm ( rtS , & rtDW
. oa2z5wykl2 , & rtP . oa2z5wykl2 , & ( ( XDis * ) ssGetContStateDisabled (
rtS ) ) -> oa2z5wykl2 ) ; cvm1yt3zx5 ( rtS , & rtDW . e2mpwvt2ua , & rtP .
e2mpwvt2ua , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> e2mpwvt2ua )
; cvm1yt3zx5 ( rtS , & rtDW . jhi4zkfadx , & rtP . jhi4zkfadx , & ( ( XDis *
) ssGetContStateDisabled ( rtS ) ) -> jhi4zkfadx ) ; h0ryf0cnmm ( rtS , &
rtDW . mdskmorhmi , & rtP . mdskmorhmi , & ( ( XDis * )
ssGetContStateDisabled ( rtS ) ) -> mdskmorhmi ) ; { { int_T i1 ; int_T *
iw_Tail = & rtDW . drhtugugfl . Tail [ 0 ] ; int_T * iw_Head = & rtDW .
drhtugugfl . Head [ 0 ] ; int_T * iw_Last = & rtDW . drhtugugfl . Last [ 0 ]
; int_T * iw_CircularBufSize = & rtDW . drhtugugfl . CircularBufSize [ 0 ] ;
void * * pw_TUbufferPtrs = & rtDW . lfqiakux1j . TUbufferPtrs [ 0 ] ; for (
i1 = 0 ; i1 < 3 ; i1 ++ ) { real_T * pBuffer = ( real_T * )
rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ; if ( pBuffer == ( NULL
) ) { ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ;
} iw_Tail [ i1 ] = 0 ; iw_Head [ i1 ] = 0 ; iw_Last [ i1 ] = 0 ;
iw_CircularBufSize [ i1 ] = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
pw_TUbufferPtrs [ i1 ] = ( void * ) & pBuffer [ 0 ] ; pw_TUbufferPtrs [ i1 +
3 ] = ( void * ) & pBuffer [ 8192 ] ; } } } cvm1yt3zx5 ( rtS , & rtDW .
blrugcnivv , & rtP . blrugcnivv , & ( ( XDis * ) ssGetContStateDisabled ( rtS
) ) -> blrugcnivv ) ; h0ryf0cnmm ( rtS , & rtDW . iks0grgus1 , & rtP .
iks0grgus1 , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> iks0grgus1 )
; rtDW . flcrf2pa4r = 0 ; cvm1yt3zx5 ( rtS , & rtDW . glvlabfrtp , & rtP .
glvlabfrtp , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> glvlabfrtp )
; h0ryf0cnmm ( rtS , & rtDW . drtk0acyhp , & rtP . drtk0acyhp , & ( ( XDis *
) ssGetContStateDisabled ( rtS ) ) -> drtk0acyhp ) ; { { int_T i1 ; int_T *
iw_Tail = & rtDW . nw5suzqpq2 . Tail [ 0 ] ; int_T * iw_Head = & rtDW .
nw5suzqpq2 . Head [ 0 ] ; int_T * iw_Last = & rtDW . nw5suzqpq2 . Last [ 0 ]
; int_T * iw_CircularBufSize = & rtDW . nw5suzqpq2 . CircularBufSize [ 0 ] ;
void * * pw_TUbufferPtrs = & rtDW . chf5u4x2og . TUbufferPtrs [ 0 ] ; for (
i1 = 0 ; i1 < 3 ; i1 ++ ) { real_T * pBuffer = ( real_T * )
rt_TDelayCreateBuf ( 2 , 8192 , sizeof ( real_T ) ) ; if ( pBuffer == ( NULL
) ) { ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ;
} iw_Tail [ i1 ] = 0 ; iw_Head [ i1 ] = 0 ; iw_Last [ i1 ] = 0 ;
iw_CircularBufSize [ i1 ] = 8192 ; pBuffer [ 0 ] = rtP .
TransportDelay_InitOutput_pykn05w1hn ; pBuffer [ 8192 ] = ssGetT ( rtS ) ;
pw_TUbufferPtrs [ i1 ] = ( void * ) & pBuffer [ 0 ] ; pw_TUbufferPtrs [ i1 +
3 ] = ( void * ) & pBuffer [ 8192 ] ; } } } cvm1yt3zx5 ( rtS , & rtDW .
daw0ymwmw4 , & rtP . daw0ymwmw4 , & ( ( XDis * ) ssGetContStateDisabled ( rtS
) ) -> daw0ymwmw4 ) ; h0ryf0cnmm ( rtS , & rtDW . pu1c2mp3al , & rtP .
pu1c2mp3al , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> pu1c2mp3al )
; rtDW . mv3mqzgj3o = 0 ; MdlInitialize ( ) ; MdlEnable ( ) ; } void
MdlOutputs ( int_T tid ) { creal_T c4vhn0azip ; creal_T ozyauwv5je ; creal_T
jrcfqdma2u ; creal_T otzzblqr2k ; creal_T m0bkcj42lo ; creal_T gmxsiixs0p ;
real_T lorwfzo4st [ 3 ] ; real_T frwtha21fi [ 3 ] ; real_T fq5sy4tkq0 [ 3 ] ;
real_T d3ib2izzs4 [ 3 ] ; real_T kgv0hwwrkz [ 3 ] ; real_T chprme3skc [ 3 ] ;
real_T P_p ; real_T dP ; real_T bzw3crffge ; real_T fljxf0zdcw [ 3 ] ; real_T
bfhciemo10 [ 3 ] ; real_T dmxfhmawdj [ 3 ] ; real_T ejdkw0tzmz [ 3 ] ; real_T
cfs3g00azs [ 3 ] ; real_T nx5egkjgi3 ; int32_T i ; real_T lfrwzvun5e_idx_0 ;
real_T lfrwzvun5e_idx_1 ; real_T jicqzbt3a1_idx_0 ; real_T jicqzbt3a1_idx_1 ;
srClearBC ( rtDW . nitdz0i0m1 ) ; srClearBC ( rtDW . b3ha3knqwy ) ; srClearBC
( rtDW . cpk3jnibze ) ; srClearBC ( rtDW . fiy4znthvv3 . cyyxenso3u ) ;
srClearBC ( rtDW . flkfnl3dquj . nevvtwmofe ) ; srClearBC ( rtDW . ntzroniten
) ; srClearBC ( rtDW . le4iyy3q51 ) ; srClearBC ( rtDW . b43mt2kku5 ) ;
srClearBC ( rtDW . gj0ekbre2up . hrxuy113gk ) ; srClearBC ( rtDW .
ihqnx345xuh . bwjj0lhfrh ) ; srClearBC ( rtDW . kyedbo0mxuc . gpfaohkthl ) ;
srClearBC ( rtDW . gllp1vimnhl . oqyvh1ylxe ) ; if ( ssIsSampleHit ( rtS , 2
, 0 ) ) { rtB . o4gd0y3v5b [ 0 ] = rtP . Constant_Value_jsrxor425n [ 0 ] ;
rtB . o4gd0y3v5b [ 1 ] = rtP . Constant_Value_jsrxor425n [ 1 ] ; rtB .
o4gd0y3v5b [ 2 ] = rtP . Constant_Value_jsrxor425n [ 2 ] ; { SimStruct * rts
= ssGetSFunction ( rtS , 2 ) ; sfcnOutputs ( rts , 0 ) ; } rtB . dywo4uhhag [
0 ] = rtP . Constant1_Value [ 0 ] ; rtB . dywo4uhhag [ 1 ] = rtP .
Constant1_Value [ 1 ] ; rtB . dywo4uhhag [ 2 ] = rtP . Constant1_Value [ 2 ]
; { SimStruct * rts = ssGetSFunction ( rtS , 3 ) ; sfcnOutputs ( rts , 0 ) ;
} fljxf0zdcw [ 0 ] = rtP . uhZ_Gain [ 0 ] * rtB . g3hcemkbbs [ 0 ] - rtB .
atzxigju2m [ 0 ] ; fljxf0zdcw [ 1 ] = rtP . uhZ_Gain [ 1 ] * rtB . g3hcemkbbs
[ 1 ] - rtB . atzxigju2m [ 1 ] ; fljxf0zdcw [ 2 ] = rtP . uhZ_Gain [ 2 ] *
rtB . g3hcemkbbs [ 2 ] - rtB . atzxigju2m [ 2 ] ; { SimStruct * rts =
ssGetSFunction ( rtS , 4 ) ; sfcnOutputs ( rts , 0 ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 5 ) ; sfcnOutputs ( rts , 0 ) ; } bzw3crffge = rtP .
uhZ_Gain_lwvpl3ylzi [ 0 ] * rtB . hu3llba3ca [ 0 ] - rtB . d0r0bfxxhv [ 0 ] ;
dmxfhmawdj [ 0 ] = rtP . Gain6_Gain_cnv0nkvg1u [ 0 ] * fljxf0zdcw [ 0 ] + rtP
. Gain3_Gain_otnifcmile [ 0 ] * bzw3crffge ; bfhciemo10 [ 0 ] = bzw3crffge ;
bzw3crffge = rtP . uhZ_Gain_lwvpl3ylzi [ 1 ] * rtB . hu3llba3ca [ 1 ] - rtB .
d0r0bfxxhv [ 1 ] ; dmxfhmawdj [ 1 ] = rtP . Gain6_Gain_cnv0nkvg1u [ 1 ] *
fljxf0zdcw [ 1 ] + rtP . Gain3_Gain_otnifcmile [ 1 ] * bzw3crffge ;
bfhciemo10 [ 1 ] = bzw3crffge ; bzw3crffge = rtP . uhZ_Gain_lwvpl3ylzi [ 2 ]
* rtB . hu3llba3ca [ 2 ] - rtB . d0r0bfxxhv [ 2 ] ; dmxfhmawdj [ 2 ] = rtP .
Gain6_Gain_cnv0nkvg1u [ 2 ] * fljxf0zdcw [ 2 ] + rtP . Gain3_Gain_otnifcmile
[ 2 ] * bzw3crffge ; bfhciemo10 [ 2 ] = bzw3crffge ; for ( i = 0 ; i < 3 ; i
++ ) { rtB . oq3ipm0wus [ i ] = 0.0 ; rtB . oq3ipm0wus [ i ] += rtP .
mode2phaseS_Gain [ i ] * dmxfhmawdj [ 0 ] ; rtB . oq3ipm0wus [ i ] += rtP .
mode2phaseS_Gain [ i + 3 ] * dmxfhmawdj [ 1 ] ; rtB . oq3ipm0wus [ i ] += rtP
. mode2phaseS_Gain [ i + 6 ] * dmxfhmawdj [ 2 ] ; bfhciemo10 [ i ] = rtP .
Gain4_Gain [ i ] * fljxf0zdcw [ i ] + rtP . Gain5_Gain [ i ] * bfhciemo10 [ i
] ; } for ( i = 0 ; i < 3 ; i ++ ) { rtB . ntgaxamgwu [ i ] = 0.0 ; rtB .
ntgaxamgwu [ i ] += rtP . mode2phaseR_Gain [ i ] * bfhciemo10 [ 0 ] ; rtB .
ntgaxamgwu [ i ] += rtP . mode2phaseR_Gain [ i + 3 ] * bfhciemo10 [ 1 ] ; rtB
. ntgaxamgwu [ i ] += rtP . mode2phaseR_Gain [ i + 6 ] * bfhciemo10 [ 2 ] ;
rtB . ft0dn4u15h [ i ] = rtP . Constant_Value_g4zd2j3sx3 [ i ] ; } {
SimStruct * rts = ssGetSFunction ( rtS , 6 ) ; sfcnOutputs ( rts , 0 ) ; }
rtB . ekwirjyast [ 0 ] = rtP . Constant1_Value_ay2jtmeeno [ 0 ] ; rtB .
ekwirjyast [ 1 ] = rtP . Constant1_Value_ay2jtmeeno [ 1 ] ; rtB . ekwirjyast
[ 2 ] = rtP . Constant1_Value_ay2jtmeeno [ 2 ] ; { SimStruct * rts =
ssGetSFunction ( rtS , 7 ) ; sfcnOutputs ( rts , 0 ) ; } fljxf0zdcw [ 0 ] =
rtP . uhZ_Gain_hoxhm3zthg [ 0 ] * rtB . bk3h34bboy [ 0 ] - rtB . aj3jiuqdac [
0 ] ; fljxf0zdcw [ 1 ] = rtP . uhZ_Gain_hoxhm3zthg [ 1 ] * rtB . bk3h34bboy [
1 ] - rtB . aj3jiuqdac [ 1 ] ; fljxf0zdcw [ 2 ] = rtP . uhZ_Gain_hoxhm3zthg [
2 ] * rtB . bk3h34bboy [ 2 ] - rtB . aj3jiuqdac [ 2 ] ; { SimStruct * rts =
ssGetSFunction ( rtS , 8 ) ; sfcnOutputs ( rts , 0 ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 9 ) ; sfcnOutputs ( rts , 0 ) ; } bzw3crffge = rtP .
uhZ_Gain_azgxhb2bie [ 0 ] * rtB . nj1powhokr [ 0 ] - rtB . ikd1laxvge [ 0 ] ;
cfs3g00azs [ 0 ] = rtP . Gain6_Gain_mkgiv5ecem [ 0 ] * fljxf0zdcw [ 0 ] + rtP
. Gain3_Gain_cabqd5r1xu [ 0 ] * bzw3crffge ; ejdkw0tzmz [ 0 ] = bzw3crffge ;
bzw3crffge = rtP . uhZ_Gain_azgxhb2bie [ 1 ] * rtB . nj1powhokr [ 1 ] - rtB .
ikd1laxvge [ 1 ] ; cfs3g00azs [ 1 ] = rtP . Gain6_Gain_mkgiv5ecem [ 1 ] *
fljxf0zdcw [ 1 ] + rtP . Gain3_Gain_cabqd5r1xu [ 1 ] * bzw3crffge ;
ejdkw0tzmz [ 1 ] = bzw3crffge ; bzw3crffge = rtP . uhZ_Gain_azgxhb2bie [ 2 ]
* rtB . nj1powhokr [ 2 ] - rtB . ikd1laxvge [ 2 ] ; cfs3g00azs [ 2 ] = rtP .
Gain6_Gain_mkgiv5ecem [ 2 ] * fljxf0zdcw [ 2 ] + rtP . Gain3_Gain_cabqd5r1xu
[ 2 ] * bzw3crffge ; ejdkw0tzmz [ 2 ] = bzw3crffge ; for ( i = 0 ; i < 3 ; i
++ ) { rtB . hjgv2ztqc5 [ i ] = 0.0 ; rtB . hjgv2ztqc5 [ i ] += rtP .
mode2phaseS_Gain_i4u4vmnkwg [ i ] * cfs3g00azs [ 0 ] ; rtB . hjgv2ztqc5 [ i ]
+= rtP . mode2phaseS_Gain_i4u4vmnkwg [ i + 3 ] * cfs3g00azs [ 1 ] ; rtB .
hjgv2ztqc5 [ i ] += rtP . mode2phaseS_Gain_i4u4vmnkwg [ i + 6 ] * cfs3g00azs
[ 2 ] ; ejdkw0tzmz [ i ] = rtP . Gain4_Gain_awti44fbee [ i ] * fljxf0zdcw [ i
] + rtP . Gain5_Gain_f34wg0pxhd [ i ] * ejdkw0tzmz [ i ] ; } for ( i = 0 ; i
< 3 ; i ++ ) { rtB . leghz2ze52 [ i ] = 0.0 ; rtB . leghz2ze52 [ i ] += rtP .
mode2phaseR_Gain_hi0adye1ac [ i ] * ejdkw0tzmz [ 0 ] ; rtB . leghz2ze52 [ i ]
+= rtP . mode2phaseR_Gain_hi0adye1ac [ i + 3 ] * ejdkw0tzmz [ 1 ] ; rtB .
leghz2ze52 [ i ] += rtP . mode2phaseR_Gain_hi0adye1ac [ i + 6 ] * ejdkw0tzmz
[ 2 ] ; } rtB . hoxbun55q5 = rtDW . jreglfqlm5 ; rtB . eif00tf41b = rt_Lookup
( rtP . LookUpTable_XData , 12 , ssGetTaskTime ( rtS , 2 ) , rtP .
LookUpTable_YData ) ; P_p = rtB . eif00tf41b - rtDW . ca3dtz0zaw ; if ( P_p >
rtP . RateLimiter_RisingLim ) { rtB . jrxr2bib5p = rtDW . ca3dtz0zaw + rtP .
RateLimiter_RisingLim ; } else if ( P_p < rtP . RateLimiter_FallingLim ) {
rtB . jrxr2bib5p = rtDW . ca3dtz0zaw + rtP . RateLimiter_FallingLim ; } else
{ rtB . jrxr2bib5p = rtB . eif00tf41b ; } rtDW . ca3dtz0zaw = rtB .
jrxr2bib5p ; bzw3crffge = rtP . uSref_Gain * rtB . jrxr2bib5p ; rtB .
evntpqggjc = rtB . atazzkiqtz * bzw3crffge ; if ( rtDW . jyxrya54ck != 0 ) {
rtDW . a1004yerwl = muDoubleScalarSin ( 6.2831853071795862 * rtP . Fnom *
ssGetTaskTime ( rtS , 2 ) ) ; rtDW . i5irbkj5h1 = muDoubleScalarCos (
6.2831853071795862 * rtP . Fnom * ssGetTaskTime ( rtS , 2 ) ) ; rtDW .
jyxrya54ck = 0 ; } rtB . nspdhd04fa = ( ( rtDW . a1004yerwl * rtP .
SineWaveA_PCos + rtDW . i5irbkj5h1 * rtP . SineWaveA_PSin ) * rtP .
SineWaveA_HCos + ( rtDW . i5irbkj5h1 * rtP . SineWaveA_PCos - rtDW .
a1004yerwl * rtP . SineWaveA_PSin ) * rtP . SineWaveA_Hsin ) * rtP .
SineWaveA_Amp + rtP . SineWaveA_Bias ; if ( rtDW . kc2urh0o2k != 0 ) { rtDW .
dqdihrjufk = muDoubleScalarSin ( 6.2831853071795862 * rtP . Fnom *
ssGetTaskTime ( rtS , 2 ) ) ; rtDW . fnoqoswl0l = muDoubleScalarCos (
6.2831853071795862 * rtP . Fnom * ssGetTaskTime ( rtS , 2 ) ) ; rtDW .
kc2urh0o2k = 0 ; } rtB . fegphm11cq = ( ( rtDW . dqdihrjufk * rtP .
SineWaveB_PCos + rtDW . fnoqoswl0l * rtP . SineWaveB_PSin ) * rtP .
SineWaveB_HCos + ( rtDW . fnoqoswl0l * rtP . SineWaveB_PCos - rtDW .
dqdihrjufk * rtP . SineWaveB_PSin ) * rtP . SineWaveB_Hsin ) * rtP .
SineWaveB_Amp + rtP . SineWaveB_Bias ; if ( rtDW . gwu3cfmboo != 0 ) { rtDW .
l31gaczqpr = muDoubleScalarSin ( 6.2831853071795862 * rtP . Fnom *
ssGetTaskTime ( rtS , 2 ) ) ; rtDW . eelkc34vvy = muDoubleScalarCos (
6.2831853071795862 * rtP . Fnom * ssGetTaskTime ( rtS , 2 ) ) ; rtDW .
gwu3cfmboo = 0 ; } rtB . j4jxwn4tn4 = ( ( rtDW . l31gaczqpr * rtP .
SineWaveC_PCos + rtDW . eelkc34vvy * rtP . SineWaveC_PSin ) * rtP .
SineWaveC_HCos + ( rtDW . eelkc34vvy * rtP . SineWaveC_PCos - rtDW .
l31gaczqpr * rtP . SineWaveC_PSin ) * rtP . SineWaveC_Hsin ) * rtP .
SineWaveC_Amp + rtP . SineWaveC_Bias ; { real_T accum ; int_T * switch_status
= ( int_T * ) rtDW . o1xie1rqxt . SWITCH_STATUS ; int_T * switch_status_init
= ( int_T * ) rtDW . o1xie1rqxt . SWITCH_STATUS_INIT ; int_T * SwitchChange =
( int_T * ) rtDW . o1xie1rqxt . SW_CHG ; int_T * gState = ( int_T * ) rtDW .
o1xie1rqxt . G_STATE ; real_T * yswitch = ( real_T * ) rtDW . o1xie1rqxt .
Y_SWITCH ; int_T * switchTypes = ( int_T * ) rtDW . o1xie1rqxt . SWITCH_TYPES
; int_T * idxOutSw = ( int_T * ) rtDW . o1xie1rqxt . IDX_OUT_SW ; real_T *
DxCol = ( real_T * ) rtDW . o1xie1rqxt . DX_COL ; real_T * tmp2 = ( real_T *
) rtDW . o1xie1rqxt . TMP2 ; real_T * BDcol = ( real_T * ) rtDW . o1xie1rqxt
. BD_COL ; real_T * tmp1 = ( real_T * ) rtDW . o1xie1rqxt . TMP1 ; real_T *
uswlast = ( real_T * ) rtDW . o1xie1rqxt . USWLAST ; int_T newState ; int_T
swChanged = 0 ; int loopsToDo = 20 ; real_T temp ; memcpy (
switch_status_init , switch_status , 18 * sizeof ( int_T ) ) ; memcpy (
uswlast , & rtB . hliocuqujf [ 0 ] , 18 * sizeof ( real_T ) ) ; do { if (
loopsToDo == 1 ) { swChanged = 0 ; { int_T i1 ; for ( i1 = 0 ; i1 < 18 ; i1
++ ) { swChanged = ( ( SwitchChange [ i1 ] = switch_status_init [ i1 ] -
switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1 ] =
switch_status_init [ i1 ] ; } } } else { real_T * Cs = ( real_T * ) rtDW .
o1xie1rqxt . CS ; real_T * Ds = ( real_T * ) rtDW . o1xie1rqxt . DS ; { int_T
i1 ; real_T * y0 = & rtB . hliocuqujf [ 0 ] ; for ( i1 = 0 ; i1 < 46 ; i1 ++
) { accum = 0.0 ; { int_T i2 ; real_T * xd = & rtDW . hllwh2h1oo [ 0 ] ; for
( i2 = 0 ; i2 < 34 ; i2 ++ ) { accum += * ( Cs ++ ) * xd [ i2 ] ; } } { int_T
i2 ; const real_T * u0 ; for ( i2 = 0 ; i2 < 18 ; i2 ++ ) { accum += * ( Ds
++ ) * 0.0 ; } accum += * ( Ds ++ ) * rtB . oq3ipm0wus [ 0 ] ; accum += * (
Ds ++ ) * rtB . oq3ipm0wus [ 1 ] ; accum += * ( Ds ++ ) * rtB . oq3ipm0wus [
2 ] ; accum += * ( Ds ++ ) * rtB . ntgaxamgwu [ 0 ] ; accum += * ( Ds ++ ) *
rtB . ntgaxamgwu [ 1 ] ; accum += * ( Ds ++ ) * rtB . ntgaxamgwu [ 2 ] ;
accum += * ( Ds ++ ) * rtB . hjgv2ztqc5 [ 0 ] ; accum += * ( Ds ++ ) * rtB .
hjgv2ztqc5 [ 1 ] ; accum += * ( Ds ++ ) * rtB . hjgv2ztqc5 [ 2 ] ; accum += *
( Ds ++ ) * rtB . leghz2ze52 [ 0 ] ; accum += * ( Ds ++ ) * rtB . leghz2ze52
[ 1 ] ; accum += * ( Ds ++ ) * rtB . leghz2ze52 [ 2 ] ; accum += * ( Ds ++ )
* rtB . hoxbun55q5 ; accum += * ( Ds ++ ) * rtB . evntpqggjc ; accum += * (
Ds ++ ) * rtB . nspdhd04fa ; accum += * ( Ds ++ ) * rtB . fegphm11cq ; accum
+= * ( Ds ++ ) * rtB . j4jxwn4tn4 ; } y0 [ i1 ] = accum ; } } swChanged = 0 ;
{ int_T i1 ; real_T * y0 = & rtB . hliocuqujf [ 0 ] ; for ( i1 = 0 ; i1 < 18
; i1 ++ ) { switch ( switchTypes [ i1 ] ) { case 3 : newState = y0 [ i1 ] >
0.0 ? 1 : ( ( y0 [ i1 ] < 0.0 ) ? 0 : switch_status [ i1 ] ) ; break ; case 7
: newState = ( ( y0 [ i1 ] > 0.0 ) && ( gState [ i1 ] > 0 ) ) || ( y0 [ i1 ]
< 0.0 ) ? 1 : ( ( ( y0 [ i1 ] > 0.0 ) && gState [ i1 ] == 0 ) ? 0 :
switch_status [ i1 ] ) ; break ; } swChanged = ( ( SwitchChange [ i1 ] =
newState - switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1
] = newState ; } } } if ( swChanged ) { real_T * As = ( real_T * ) rtDW .
o1xie1rqxt . AS ; real_T * Cs = ( real_T * ) rtDW . o1xie1rqxt . CS ; real_T
* Bs = ( real_T * ) rtDW . o1xie1rqxt . BS ; real_T * Ds = ( real_T * ) rtDW
. o1xie1rqxt . DS ; real_T a1 ; { int_T i1 ; for ( i1 = 0 ; i1 < 18 ; i1 ++ )
{ if ( SwitchChange [ i1 ] != 0 ) { a1 = 1000.0 * SwitchChange [ i1 ] ; temp
= 1 / ( 1 - Ds [ i1 * 36 ] * a1 ) ; { int_T i2 ; for ( i2 = 0 ; i2 < 46 ; i2
++ ) { DxCol [ i2 ] = Ds [ i2 * 35 + i1 ] * temp * a1 ; } } DxCol [ i1 ] =
temp ; { int_T i2 ; for ( i2 = 0 ; i2 < 34 ; i2 ++ ) { BDcol [ i2 ] = Bs [ i2
* 35 + i1 ] * a1 ; } } memcpy ( tmp1 , & Cs [ i1 * 34 ] , 34 * sizeof (
real_T ) ) ; memset ( & Cs [ i1 * 34 ] , '\0' , 34 * sizeof ( real_T ) ) ;
memcpy ( tmp2 , & Ds [ i1 * 35 ] , 35 * sizeof ( real_T ) ) ; memset ( & Ds [
i1 * 35 ] , '\0' , 35 * sizeof ( real_T ) ) ; { int_T i2 ; for ( i2 = 0 ; i2
< 46 ; i2 ++ ) { a1 = DxCol [ i2 ] ; { int_T i3 ; for ( i3 = 0 ; i3 < 34 ; i3
++ ) { Cs [ i2 * 34 + i3 ] += a1 * tmp1 [ i3 ] ; } } { int_T i3 ; for ( i3 =
0 ; i3 < 35 ; i3 ++ ) { Ds [ i2 * 35 + i3 ] += a1 * tmp2 [ i3 ] ; } } } } {
int_T i2 ; for ( i2 = 0 ; i2 < 34 ; i2 ++ ) { a1 = BDcol [ i2 ] ; { int_T i3
; for ( i3 = 0 ; i3 < 34 ; i3 ++ ) { As [ i2 * 34 + i3 ] += a1 * Cs [ i1 * 34
+ i3 ] ; } } { int_T i3 ; for ( i3 = 0 ; i3 < 35 ; i3 ++ ) { Bs [ i2 * 35 +
i3 ] += a1 * Ds [ i1 * 35 + i3 ] ; } } } } } } } } } while ( swChanged > 0 &&
-- loopsToDo > 0 ) ; if ( loopsToDo == 0 ) { real_T * Cs = ( real_T * ) rtDW
. o1xie1rqxt . CS ; real_T * Ds = ( real_T * ) rtDW . o1xie1rqxt . DS ; {
int_T i1 ; real_T * y0 = & rtB . hliocuqujf [ 0 ] ; for ( i1 = 0 ; i1 < 46 ;
i1 ++ ) { accum = 0.0 ; { int_T i2 ; real_T * xd = & rtDW . hllwh2h1oo [ 0 ]
; for ( i2 = 0 ; i2 < 34 ; i2 ++ ) { accum += * ( Cs ++ ) * xd [ i2 ] ; } } {
int_T i2 ; const real_T * u0 ; for ( i2 = 0 ; i2 < 18 ; i2 ++ ) { accum += *
( Ds ++ ) * 0.0 ; } accum += * ( Ds ++ ) * rtB . oq3ipm0wus [ 0 ] ; accum +=
* ( Ds ++ ) * rtB . oq3ipm0wus [ 1 ] ; accum += * ( Ds ++ ) * rtB .
oq3ipm0wus [ 2 ] ; accum += * ( Ds ++ ) * rtB . ntgaxamgwu [ 0 ] ; accum += *
( Ds ++ ) * rtB . ntgaxamgwu [ 1 ] ; accum += * ( Ds ++ ) * rtB . ntgaxamgwu
[ 2 ] ; accum += * ( Ds ++ ) * rtB . hjgv2ztqc5 [ 0 ] ; accum += * ( Ds ++ )
* rtB . hjgv2ztqc5 [ 1 ] ; accum += * ( Ds ++ ) * rtB . hjgv2ztqc5 [ 2 ] ;
accum += * ( Ds ++ ) * rtB . leghz2ze52 [ 0 ] ; accum += * ( Ds ++ ) * rtB .
leghz2ze52 [ 1 ] ; accum += * ( Ds ++ ) * rtB . leghz2ze52 [ 2 ] ; accum += *
( Ds ++ ) * rtB . hoxbun55q5 ; accum += * ( Ds ++ ) * rtB . evntpqggjc ;
accum += * ( Ds ++ ) * rtB . nspdhd04fa ; accum += * ( Ds ++ ) * rtB .
fegphm11cq ; accum += * ( Ds ++ ) * rtB . j4jxwn4tn4 ; } y0 [ i1 ] = accum ;
} } } { int_T i1 ; real_T * y1 = & rtB . iijvn1pij4 [ 0 ] ; for ( i1 = 0 ; i1
< 18 ; i1 ++ ) { y1 [ i1 ] = ( real_T ) switch_status [ i1 ] ; } } } rtB .
bsgixfioey = rtP . donotdeletethisgain_Gain * rtB . hliocuqujf [ 33 ] ; rtB .
ozgmksrvpc = rtP . donotdeletethisgain_Gain_hcm2y02ald * rtB . hliocuqujf [
42 ] ; rtB . j40qpekti4 [ 0 ] = rtP . donotdeletethisgain_Gain_hd2lurzjs1 *
rtB . hliocuqujf [ 39 ] * rtP . Kv_Gain ; rtB . j40qpekti4 [ 1 ] = rtP .
donotdeletethisgain_Gain_g0aitc3jds * rtB . hliocuqujf [ 40 ] * rtP . Kv_Gain
; rtB . j40qpekti4 [ 2 ] = rtP . donotdeletethisgain_Gain_j0j522ufey * rtB .
hliocuqujf [ 41 ] * rtP . Kv_Gain ; P_p = rtP . Vnom_prim *
1.7320508075688772 / rtP . Pnom / 1.4142135623730951 ; for ( i = 0 ; i < 3 ;
i ++ ) { lorwfzo4st [ i ] = rtP . Gain1_Gain_pnfzbhegxe * ( rtP .
Gain3_Gain_dyow2vdij1 [ i + 6 ] * ( P_p * rtB . j40qpekti4 [ 2 ] ) + ( rtP .
Gain3_Gain_dyow2vdij1 [ i + 3 ] * ( P_p * rtB . j40qpekti4 [ 1 ] ) + P_p *
rtB . j40qpekti4 [ 0 ] * rtP . Gain3_Gain_dyow2vdij1 [ i ] ) ) ; } } if (
ssIsSampleHit ( rtS , 3 , 0 ) ) { rtB . gjbagadbk5 = ( rtDW . m1peilszpy >
rtP . Constant4_Value ) ; if ( rtDW . mktm0brmw5 ) { rtDW . mktm0brmw5 =
false ; rtB . l4y5mcjvpy = rtP . Initial_Value ; } else { rtB . l4y5mcjvpy =
rtDW . m1peilszpy - rtP . Constant4_Value ; } if ( rtB . gjbagadbk5 && ( rtDW
. bvgqcazfkc <= 0 ) ) { rtDW . m1peilszpy = rtB . l4y5mcjvpy ; } rtB .
jnimo5lv0m = rtDW . m1peilszpy ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) {
flkfnl3dqu ( rtB . ndqy5pufgt , & lorwfzo4st [ 0 ] , rtB . jnimo5lv0m , & rtB
. ibrtt4ftsl , & rtDW . ibrtt4ftsl ) ; fiy4znthvv ( rtB . isub4roeyp , &
lorwfzo4st [ 0 ] , rtB . jnimo5lv0m , & rtB . bdvvkdwqfm , & rtDW .
bdvvkdwqfm ) ; if ( rtB . ndqy5pufgt != 0 ) { rtB . fztulndu2v [ 0 ] = rtB .
ibrtt4ftsl . i0fvut4tar ; rtB . fztulndu2v [ 1 ] = rtB . ibrtt4ftsl .
k5tvpsreru ; } else { rtB . fztulndu2v [ 0 ] = rtB . bdvvkdwqfm . jgensia2ex
; rtB . fztulndu2v [ 1 ] = rtB . bdvvkdwqfm . l2faoiogru ; } rtB . crwergbsh2
= rtP . donotdeletethisgain_Gain_m3rdldqf5v * rtB . hliocuqujf [ 37 ] ; } if
( ssIsSampleHit ( rtS , 3 , 0 ) ) { if ( ! rtDW . c1tw4ogoog ) { rtDW .
c1tw4ogoog = true ; rtDW . h1cmwk4uvu = rtP . Vnom_dc ; } P_p = rtB .
bsgixfioey * rtB . ozgmksrvpc ; dP = P_p - rtDW . j0fknedvcm ; if ( ( dP !=
0.0 ) && ( rtP . MPPT_On_Value != 0.0 ) ) { if ( dP < 0.0 ) { if ( rtB .
bsgixfioey - rtDW . k2yf4d0qko < 0.0 ) { dP = rtDW . h1cmwk4uvu + rtP .
Increment_MPPT ; } else { dP = rtDW . h1cmwk4uvu - rtP . Increment_MPPT ; } }
else if ( rtB . bsgixfioey - rtDW . k2yf4d0qko < 0.0 ) { dP = rtDW .
h1cmwk4uvu - rtP . Increment_MPPT ; } else { dP = rtDW . h1cmwk4uvu + rtP .
Increment_MPPT ; } } else { dP = rtDW . h1cmwk4uvu ; } if ( ( dP >= rtP .
Iph_1_Value ) || ( dP <= rtP . Iph_2_Value ) ) { dP = rtDW . h1cmwk4uvu ; }
rtDW . h1cmwk4uvu = dP ; rtDW . k2yf4d0qko = rtB . bsgixfioey ; rtDW .
j0fknedvcm = P_p ; if ( rtP . MPPT_On_Value != 0.0 ) { rtB . onqodwzqxe = dP
; } else { rtB . onqodwzqxe = rtP . Vnom_dc ; } } if ( ssIsSampleHit ( rtS ,
2 , 0 ) ) { rtB . icx34ifd2r = 1.0 / rtP . Vnom_dc * ( rtB . crwergbsh2 - rtB
. onqodwzqxe ) ; } if ( ssIsSampleHit ( rtS , 3 , 0 ) ) { rtB . ihq1wxrrwv =
rtP . Ki_VDCreg * rtB . icx34ifd2r ; rtB . ajjwqo0e1v = rtP .
Integrator_gainval * rtB . ihq1wxrrwv + rtDW . dnrqy23bwk ; P_p = rtP .
Kp_VDCreg * rtB . icx34ifd2r + rtB . ajjwqo0e1v ; if ( P_p > rtP .
LimitU_VDCreg ) { rtB . ad4rcuki4l = rtP . LimitU_VDCreg ; } else if ( P_p <
rtP . LimitL_VDCreg ) { rtB . ad4rcuki4l = rtP . LimitL_VDCreg ; } else { rtB
. ad4rcuki4l = P_p ; } } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB .
pbs5aqgeph [ 0 ] = rtB . ad4rcuki4l - rtB . fztulndu2v [ 0 ] ; rtB .
pbs5aqgeph [ 1 ] = rtP . Iq_ref_Value - rtB . fztulndu2v [ 1 ] ; } if (
ssIsSampleHit ( rtS , 3 , 0 ) ) { P_p = rtP . Kp_Ireg * rtB . pbs5aqgeph [ 0
] + rtDW . jzbmotzdka [ 0 ] ; if ( P_p > rtP . LimitU_Ireg ) { rtB .
adjdefmwjg [ 0 ] = rtP . LimitU_Ireg ; } else if ( P_p < rtP . LimitL_Ireg )
{ rtB . adjdefmwjg [ 0 ] = rtP . LimitL_Ireg ; } else { rtB . adjdefmwjg [ 0
] = P_p ; } P_p = rtP . Kp_Ireg * rtB . pbs5aqgeph [ 1 ] + rtDW . jzbmotzdka
[ 1 ] ; if ( P_p > rtP . LimitU_Ireg ) { rtB . adjdefmwjg [ 1 ] = rtP .
LimitU_Ireg ; } else if ( P_p < rtP . LimitL_Ireg ) { rtB . adjdefmwjg [ 1 ]
= rtP . LimitL_Ireg ; } else { rtB . adjdefmwjg [ 1 ] = P_p ; } } if (
ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB . mtdom0eizr [ 0 ] = rtP .
donotdeletethisgain_Gain_kb2zb2sa4y * rtB . hliocuqujf [ 30 ] * rtP .
Kv1_Gain ; rtB . mtdom0eizr [ 1 ] = rtP . donotdeletethisgain_Gain_kt5arp05v1
* rtB . hliocuqujf [ 31 ] * rtP . Kv1_Gain ; rtB . mtdom0eizr [ 2 ] = rtP .
donotdeletethisgain_Gain_bukv5qr0ht * rtB . hliocuqujf [ 32 ] * rtP .
Kv1_Gain ; P_p = 1.0 / ( rtP . Vnom_prim * 1.4142135623730951 /
1.7320508075688772 ) ; rtB . iajsx0ofcp [ 0 ] = P_p * rtB . mtdom0eizr [ 0 ]
; rtB . iajsx0ofcp [ 1 ] = P_p * rtB . mtdom0eizr [ 1 ] ; rtB . iajsx0ofcp [
2 ] = P_p * rtB . mtdom0eizr [ 2 ] ; for ( i = 0 ; i < 3 ; i ++ ) {
frwtha21fi [ i ] = rtP . Gain1_Gain_ozhyrxfmnh * ( rtP .
Gain3_Gain_p1jkth5m13 [ i + 6 ] * rtB . iajsx0ofcp [ 2 ] + ( rtP .
Gain3_Gain_p1jkth5m13 [ i + 3 ] * rtB . iajsx0ofcp [ 1 ] + rtP .
Gain3_Gain_p1jkth5m13 [ i ] * rtB . iajsx0ofcp [ 0 ] ) ) ; } flkfnl3dqu ( rtB
. pkbwlncsj5 , & frwtha21fi [ 0 ] , rtB . jnimo5lv0m , & rtB . imgy1z4qes , &
rtDW . imgy1z4qes ) ; fiy4znthvv ( rtB . hdzltqw2sw , & frwtha21fi [ 0 ] ,
rtB . jnimo5lv0m , & rtB . gusdfzbcqa , & rtDW . gusdfzbcqa ) ; if ( rtB .
pkbwlncsj5 != 0 ) { lfrwzvun5e_idx_0 = rtB . imgy1z4qes . i0fvut4tar ;
lfrwzvun5e_idx_1 = rtB . imgy1z4qes . k5tvpsreru ; } else { lfrwzvun5e_idx_0
= rtB . gusdfzbcqa . jgensia2ex ; lfrwzvun5e_idx_1 = rtB . gusdfzbcqa .
l2faoiogru ; } } if ( ssIsSampleHit ( rtS , 3 , 0 ) ) { rtB . bhsqp0azai =
rtP . Rff_Gain * rtB . ad4rcuki4l ; rtB . j5nk5rin3j = rtP . Lff_Gain * rtB .
ad4rcuki4l ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { nx5egkjgi3 = (
lfrwzvun5e_idx_0 + rtB . bhsqp0azai ) - rtB . fldwassvbh ; jicqzbt3a1_idx_0 =
rtB . adjdefmwjg [ 0 ] + nx5egkjgi3 ; jicqzbt3a1_idx_1 = ( ( lfrwzvun5e_idx_1
+ rtB . dr1q3vvfez ) + rtB . j5nk5rin3j ) + rtB . adjdefmwjg [ 1 ] ; if (
jicqzbt3a1_idx_0 > rtP . LimitU_Ireg ) { jicqzbt3a1_idx_0 = rtP . LimitU_Ireg
; } else { if ( jicqzbt3a1_idx_0 < rtP . LimitL_Ireg ) { jicqzbt3a1_idx_0 =
rtP . LimitL_Ireg ; } } if ( jicqzbt3a1_idx_1 > rtP . LimitU_Ireg ) {
jicqzbt3a1_idx_1 = rtP . LimitU_Ireg ; } else { if ( jicqzbt3a1_idx_1 < rtP .
LimitL_Ireg ) { jicqzbt3a1_idx_1 = rtP . LimitL_Ireg ; } } } if (
ssIsSampleHit ( rtS , 3 , 0 ) ) { rtB . el5lb42a5h [ 0 ] = rtP . Ki_Ireg *
rtB . pbs5aqgeph [ 0 ] ; rtB . el5lb42a5h [ 1 ] = rtP . Ki_Ireg * rtB .
pbs5aqgeph [ 1 ] ; rtB . om4giqthw2 [ 0 ] = rtDW . jv0y1evb0u [ 0 ] ; rtB .
om4giqthw2 [ 1 ] = rtDW . jv0y1evb0u [ 1 ] ; rtB . om4giqthw2 [ 2 ] = rtDW .
jv0y1evb0u [ 2 ] ; if ( rtP . Overmodulation_OverModType == 2.0 ) { if ( rtB
. om4giqthw2 [ 0 ] > rtP . DeadZone_End ) { nx5egkjgi3 = rtB . om4giqthw2 [ 0
] - rtP . DeadZone_End ; } else if ( rtB . om4giqthw2 [ 0 ] >= rtP .
DeadZone_Start ) { nx5egkjgi3 = 0.0 ; } else { nx5egkjgi3 = rtB . om4giqthw2
[ 0 ] - rtP . DeadZone_Start ; } if ( rtB . om4giqthw2 [ 1 ] > rtP .
DeadZone_End ) { lfrwzvun5e_idx_0 = rtB . om4giqthw2 [ 1 ] - rtP .
DeadZone_End ; } else if ( rtB . om4giqthw2 [ 1 ] >= rtP . DeadZone_Start ) {
lfrwzvun5e_idx_0 = 0.0 ; } else { lfrwzvun5e_idx_0 = rtB . om4giqthw2 [ 1 ] -
rtP . DeadZone_Start ; } if ( rtB . om4giqthw2 [ 2 ] > rtP . DeadZone_End ) {
lfrwzvun5e_idx_1 = rtB . om4giqthw2 [ 2 ] - rtP . DeadZone_End ; } else if (
rtB . om4giqthw2 [ 2 ] >= rtP . DeadZone_Start ) { lfrwzvun5e_idx_1 = 0.0 ; }
else { lfrwzvun5e_idx_1 = rtB . om4giqthw2 [ 2 ] - rtP . DeadZone_Start ; }
nx5egkjgi3 = ( ( nx5egkjgi3 + lfrwzvun5e_idx_0 ) + lfrwzvun5e_idx_1 ) * rtP .
Gain1_Gain ; rtB . kk5fvlgob4 [ 0 ] = rtB . om4giqthw2 [ 0 ] + nx5egkjgi3 ;
rtB . kk5fvlgob4 [ 1 ] = rtB . om4giqthw2 [ 1 ] + nx5egkjgi3 ; rtB .
kk5fvlgob4 [ 2 ] = rtB . om4giqthw2 [ 2 ] + nx5egkjgi3 ; if (
ssIsMajorTimeStep ( rtS ) ) { srUpdateBC ( rtDW . nitdz0i0m1 ) ; } } if ( rtP
. Overmodulation_OverModType == 3.0 ) { nx5egkjgi3 = ( muDoubleScalarMin (
muDoubleScalarMin ( rtB . om4giqthw2 [ 0 ] , rtB . om4giqthw2 [ 1 ] ) , rtB .
om4giqthw2 [ 2 ] ) + muDoubleScalarMax ( muDoubleScalarMax ( rtB . om4giqthw2
[ 0 ] , rtB . om4giqthw2 [ 1 ] ) , rtB . om4giqthw2 [ 2 ] ) ) * rtP .
Gain1_Gain_oucfv1ov0d ; rtB . kve2c32fv4 [ 0 ] = rtB . om4giqthw2 [ 0 ] -
nx5egkjgi3 ; rtB . kve2c32fv4 [ 1 ] = rtB . om4giqthw2 [ 1 ] - nx5egkjgi3 ;
rtB . kve2c32fv4 [ 2 ] = rtB . om4giqthw2 [ 2 ] - nx5egkjgi3 ; if (
ssIsMajorTimeStep ( rtS ) ) { srUpdateBC ( rtDW . b3ha3knqwy ) ; } } if ( rtP
. Overmodulation_OverModType == 1.0 ) { if ( ssIsSampleHit ( rtS , 3 , 0 ) )
{ for ( i = 0 ; i < 3 ; i ++ ) { fljxf0zdcw [ i ] = rtP .
Gain1_Gain_ex3qxbdqxn * ( rtP . Gain3_Gain [ i + 6 ] * rtB . om4giqthw2 [ 2 ]
+ ( rtP . Gain3_Gain [ i + 3 ] * rtB . om4giqthw2 [ 1 ] + rtP . Gain3_Gain [
i ] * rtB . om4giqthw2 [ 0 ] ) ) ; } nx5egkjgi3 = muDoubleScalarAtan2 (
fljxf0zdcw [ 1 ] , fljxf0zdcw [ 0 ] ) ; lfrwzvun5e_idx_0 = rtP . Gain6_Gain *
muDoubleScalarHypot ( fljxf0zdcw [ 0 ] , fljxf0zdcw [ 1 ] ) ; rtB .
khgh0in0mh [ 0 ] = muDoubleScalarSin ( ( nx5egkjgi3 + rtP . Constant_Value [
0 ] ) * rtP . Gain7_Gain ) * lfrwzvun5e_idx_0 + rtB . om4giqthw2 [ 0 ] ; rtB
. khgh0in0mh [ 1 ] = muDoubleScalarSin ( ( nx5egkjgi3 + rtP . Constant_Value
[ 1 ] ) * rtP . Gain7_Gain ) * lfrwzvun5e_idx_0 + rtB . om4giqthw2 [ 1 ] ;
rtB . khgh0in0mh [ 2 ] = muDoubleScalarSin ( ( nx5egkjgi3 + rtP .
Constant_Value [ 2 ] ) * rtP . Gain7_Gain ) * lfrwzvun5e_idx_0 + rtB .
om4giqthw2 [ 2 ] ; } if ( ssIsMajorTimeStep ( rtS ) ) { srUpdateBC ( rtDW .
cpk3jnibze ) ; } } switch ( ( int32_T ) rtP . Overmodulation_OverModType ) {
case 1 : rtB . luiagn315d [ 0 ] = rtB . khgh0in0mh [ 0 ] ; rtB . luiagn315d [
1 ] = rtB . khgh0in0mh [ 1 ] ; rtB . luiagn315d [ 2 ] = rtB . khgh0in0mh [ 2
] ; break ; case 2 : rtB . luiagn315d [ 0 ] = rtB . kk5fvlgob4 [ 0 ] ; rtB .
luiagn315d [ 1 ] = rtB . kk5fvlgob4 [ 1 ] ; rtB . luiagn315d [ 2 ] = rtB .
kk5fvlgob4 [ 2 ] ; break ; default : rtB . luiagn315d [ 0 ] = rtB .
kve2c32fv4 [ 0 ] ; rtB . luiagn315d [ 1 ] = rtB . kve2c32fv4 [ 1 ] ; rtB .
luiagn315d [ 2 ] = rtB . kve2c32fv4 [ 2 ] ; break ; } nx5egkjgi3 = rtDW .
giqv4pyyys ; if ( ssIsSampleHit ( rtS , 3 , 0 ) && ssIsMajorTimeStep ( rtS )
) { if ( rtP . Constant1_Value_finlsj5cwk > 0.0 ) { if ( ! rtDW . iy4nvvktj2
) { if ( ssGetTaskTime ( rtS , 3 ) != ssGetTStart ( rtS ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; } rtDW . lpm5mugxt5 = 1U
; rtDW . nymp3uolcg = 1U ; rtDW . iy4nvvktj2 = true ; } } else { if ( rtDW .
iy4nvvktj2 ) { ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; rtDW .
iy4nvvktj2 = false ; } } } if ( rtDW . iy4nvvktj2 ) { if ( ssIsSampleHit (
rtS , 3 , 0 ) ) { for ( i = 0 ; i < 3 ; i ++ ) { chprme3skc [ i ] = rtP .
Gain1_Gain_mv4azmob4e * ( rtP . Gain3_Gain_fs2dzj0rbm [ i + 6 ] * rtB .
iajsx0ofcp [ 2 ] + ( rtP . Gain3_Gain_fs2dzj0rbm [ i + 3 ] * rtB . iajsx0ofcp
[ 1 ] + rtP . Gain3_Gain_fs2dzj0rbm [ i ] * rtB . iajsx0ofcp [ 0 ] ) ) ; }
flkfnl3dqu ( rtB . e01ed5cv3s , & chprme3skc [ 0 ] , rtB . jnimo5lv0m , & rtB
. flkfnl3dquj , & rtDW . flkfnl3dquj ) ; fiy4znthvv ( rtB . oslgokdckt , &
chprme3skc [ 0 ] , rtB . jnimo5lv0m , & rtB . fiy4znthvv3 , & rtDW .
fiy4znthvv3 ) ; if ( rtB . e01ed5cv3s != 0 ) { rtB . eeusicyxwo [ 0 ] = rtB .
flkfnl3dquj . i0fvut4tar ; rtB . eeusicyxwo [ 1 ] = rtB . flkfnl3dquj .
k5tvpsreru ; } else { rtB . eeusicyxwo [ 0 ] = rtB . fiy4znthvv3 . jgensia2ex
; rtB . eeusicyxwo [ 1 ] = rtB . fiy4znthvv3 . l2faoiogru ; } if ( rtDW .
lpm5mugxt5 != 0 ) { rtB . bzadnafn2c = rtDW . lzgy3px31z ; } else { rtB .
bzadnafn2c = rtP . Integ4_gainval * rtB . eeusicyxwo [ 0 ] + rtDW .
lzgy3px31z ; } if ( nx5egkjgi3 > rtP . Toavoiddivisionbyzero_UpperSat ) {
lfrwzvun5e_idx_0 = rtP . Toavoiddivisionbyzero_UpperSat ; } else if (
nx5egkjgi3 < rtP . Toavoiddivisionbyzero_LowerSat ) { lfrwzvun5e_idx_0 = rtP
. Toavoiddivisionbyzero_LowerSat ; } else { lfrwzvun5e_idx_0 = nx5egkjgi3 ; }
lfrwzvun5e_idx_0 = 1.0 / lfrwzvun5e_idx_0 / 5.0505050505050505e-5 ; P_p =
muDoubleScalarCeil ( lfrwzvun5e_idx_0 ) ; rtB . jvbkza0bug = rtP . Ts_Control
* P_p ; { SimStruct * rts = ssGetSFunction ( rtS , 0 ) ; sfcnOutputs ( rts ,
0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >= rtP . Constant_Value_l4lyjwd23b ) {
P_p = lfrwzvun5e_idx_0 - P_p ; rtB . aggnsxdhid = ( ( rtB . eeusicyxwo [ 0 ]
- rtDW . omufifkqbz ) * P_p * rtP . Gain1_Gain_cmdba5bv2h + rtB . eeusicyxwo
[ 0 ] ) * ( P_p / lfrwzvun5e_idx_0 ) + ( rtB . bzadnafn2c - rtB . chkgbobqc5
) * nx5egkjgi3 ; } else { rtB . aggnsxdhid = rtDW . nndlftph12 ; } if ( rtDW
. nymp3uolcg != 0 ) { rtB . as2m1qy4ku = rtDW . ohydhrpaay ; } else { rtB .
as2m1qy4ku = rtP . Integ4_gainval_fz2uqdiylj * rtB . eeusicyxwo [ 1 ] + rtDW
. ohydhrpaay ; } if ( nx5egkjgi3 > rtP .
Toavoiddivisionbyzero_UpperSat_cysl1cmul3 ) { lfrwzvun5e_idx_0 = rtP .
Toavoiddivisionbyzero_UpperSat_cysl1cmul3 ; } else if ( nx5egkjgi3 < rtP .
Toavoiddivisionbyzero_LowerSat_k1vxw44tbr ) { lfrwzvun5e_idx_0 = rtP .
Toavoiddivisionbyzero_LowerSat_k1vxw44tbr ; } else { lfrwzvun5e_idx_0 =
nx5egkjgi3 ; } lfrwzvun5e_idx_0 = 1.0 / lfrwzvun5e_idx_0 /
5.0505050505050505e-5 ; P_p = muDoubleScalarCeil ( lfrwzvun5e_idx_0 ) ; rtB .
gndfpjrzbt = rtP . Ts_Control * P_p ; { SimStruct * rts = ssGetSFunction (
rtS , 1 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >= rtP
. Constant_Value_jccahizvsu ) { P_p = lfrwzvun5e_idx_0 - P_p ; rtB .
bsrowihmvj = ( ( rtB . eeusicyxwo [ 1 ] - rtDW . eznk4sr03e ) * P_p * rtP .
Gain1_Gain_hetxdadbk5 + rtB . eeusicyxwo [ 1 ] ) * ( P_p / lfrwzvun5e_idx_0 )
+ ( rtB . as2m1qy4ku - rtB . dlvjgbn4i5 ) * nx5egkjgi3 ; } else { rtB .
bsrowihmvj = rtDW . aktl42ci5l ; } P_p = muDoubleScalarHypot ( rtB .
aggnsxdhid , rtB . bsrowihmvj ) ; if ( P_p > rtP . Saturation_UpperSat ) {
P_p = rtP . Saturation_UpperSat ; } else { if ( P_p < rtP .
Saturation_LowerSat ) { P_p = rtP . Saturation_LowerSat ; } } rtB .
puricsu5uo = 1.0 / P_p ; } if ( ssIsMajorTimeStep ( rtS ) ) { srUpdateBC (
rtDW . ntzroniten ) ; } } for ( i = 0 ; i < 3 ; i ++ ) { fq5sy4tkq0 [ i ] =
rtP . Gain1_Gain_op30pyxpek * ( rtP . Gain3_Gain_nwldjxxvlq [ i + 6 ] * rtB .
iajsx0ofcp [ 2 ] + ( rtP . Gain3_Gain_nwldjxxvlq [ i + 3 ] * rtB . iajsx0ofcp
[ 1 ] + rtP . Gain3_Gain_nwldjxxvlq [ i ] * rtB . iajsx0ofcp [ 0 ] ) ) ; }
flkfnl3dqu ( rtB . jc4x0snbqf , & fq5sy4tkq0 [ 0 ] , rtB . jnimo5lv0m , & rtB
. gvjufizpfu , & rtDW . gvjufizpfu ) ; fiy4znthvv ( rtB . fs14jhs4t5 , &
fq5sy4tkq0 [ 0 ] , rtB . jnimo5lv0m , & rtB . h0t1xli1qx , & rtDW .
h0t1xli1qx ) ; if ( rtB . jc4x0snbqf != 0 ) { rtB . azqgqqg4wd [ 0 ] = rtB .
gvjufizpfu . i0fvut4tar ; rtB . azqgqqg4wd [ 1 ] = rtB . gvjufizpfu .
k5tvpsreru ; } else { rtB . azqgqqg4wd [ 0 ] = rtB . h0t1xli1qx . jgensia2ex
; rtB . azqgqqg4wd [ 1 ] = rtB . h0t1xli1qx . l2faoiogru ; } if ( rtDW .
oneka2f33e != 0 ) { rtB . myfgavzlid = rtDW . mwfdpj20rn ; } else { rtB .
myfgavzlid = rtP . Integ4_gainval_lotsoek0sk * rtB . azqgqqg4wd [ 1 ] + rtDW
. mwfdpj20rn ; } if ( nx5egkjgi3 > rtP .
Toavoiddivisionbyzero_UpperSat_fngqu4twkq ) { lfrwzvun5e_idx_0 = rtP .
Toavoiddivisionbyzero_UpperSat_fngqu4twkq ; } else if ( nx5egkjgi3 < rtP .
Toavoiddivisionbyzero_LowerSat_nspiile1b5 ) { lfrwzvun5e_idx_0 = rtP .
Toavoiddivisionbyzero_LowerSat_nspiile1b5 ; } else { lfrwzvun5e_idx_0 =
nx5egkjgi3 ; } lfrwzvun5e_idx_0 = 1.0 / lfrwzvun5e_idx_0 /
5.0505050505050505e-5 ; P_p = muDoubleScalarCeil ( lfrwzvun5e_idx_0 ) ; rtB .
bstj0nioer = rtP . Ts_Control * P_p ; { SimStruct * rts = ssGetSFunction (
rtS , 10 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtP . Constant_Value_kkuzztqyci ) { P_p = lfrwzvun5e_idx_0 - P_p ; rtB .
jlybhz2t0i = ( ( rtB . azqgqqg4wd [ 1 ] - rtDW . m5fkweyhnz ) * P_p * rtP .
Gain1_Gain_afd104gjl4 + rtB . azqgqqg4wd [ 1 ] ) * ( P_p / lfrwzvun5e_idx_0 )
+ ( rtB . myfgavzlid - rtB . ebxa1ugh4c ) * nx5egkjgi3 ; } else { rtB .
jlybhz2t0i = rtDW . oyi34irdce ; } rtB . mhkfvkvx1u = rtB . jlybhz2t0i * rtB
. puricsu5uo ; rtDW . koobssrco0 = ( rtB . mhkfvkvx1u - rtP .
DiscreteDerivative_DenCoef [ 1 ] * rtDW . fmpdeay2dq ) / rtP .
DiscreteDerivative_DenCoef [ 0 ] ; P_p = ( rtP . Discrete_Kp * rtB .
mhkfvkvx1u + rtDW . evobiqhvg0 ) + ( rtP . Discrete_Kd * rtDW . koobssrco0 +
- rtP . Discrete_Kd * rtDW . fmpdeay2dq ) ; if ( P_p > rtP .
Saturation1_UpperSat ) { rtB . acbdmg3uuv = rtP . Saturation1_UpperSat ; }
else if ( P_p < rtP . Saturation1_LowerSat ) { rtB . acbdmg3uuv = rtP .
Saturation1_LowerSat ; } else { rtB . acbdmg3uuv = P_p ; } nx5egkjgi3 = rtP .
Gain10_Gain * rtB . acbdmg3uuv ; P_p = nx5egkjgi3 - rtDW . aswhfhtphz ; if (
P_p > rtP . RateLimiter_RisingLim_c0aabshm22 ) { nx5egkjgi3 = rtDW .
aswhfhtphz + rtP . RateLimiter_RisingLim_c0aabshm22 ; } else { if ( P_p < rtP
. RateLimiter_FallingLim_b13f30fsua ) { nx5egkjgi3 = rtDW . aswhfhtphz + rtP
. RateLimiter_FallingLim_b13f30fsua ; } } rtDW . aswhfhtphz = nx5egkjgi3 ;
rtB . jst3mmutj5 = ( rtP . A11_Gain * rtDW . c3rg43fwng + rtP . A12_Gain *
rtDW . puez01wvqo ) + rtP . B11_Gain * nx5egkjgi3 ; rtB . edk4wtugyj = ( rtP
. A21_Gain * rtDW . c3rg43fwng + rtP . A22_Gain * rtDW . puez01wvqo ) + rtP .
B21_Gain * nx5egkjgi3 ; rtB . pu0odtmlhy = ( rtP . C11_Gain * rtDW .
c3rg43fwng + rtP . C12_Gain * rtDW . puez01wvqo ) + rtP . Duk_Gain *
nx5egkjgi3 ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { if ( rtDW . gu2gnqzjnr
!= 0 ) { nx5egkjgi3 = muDoubleScalarSin ( rtP . SineWave_Freq * ssGetTaskTime
( rtS , 2 ) ) ; lfrwzvun5e_idx_0 = muDoubleScalarCos ( rtP . SineWave_Freq *
ssGetTaskTime ( rtS , 2 ) ) ; rtDW . n3oafbzym3 [ 0 ] = nx5egkjgi3 ; rtDW .
mec1csjpdt [ 0 ] = lfrwzvun5e_idx_0 ; rtDW . n3oafbzym3 [ 1 ] = nx5egkjgi3 ;
rtDW . mec1csjpdt [ 1 ] = lfrwzvun5e_idx_0 ; rtDW . n3oafbzym3 [ 2 ] =
nx5egkjgi3 ; rtDW . mec1csjpdt [ 2 ] = lfrwzvun5e_idx_0 ; rtDW . gu2gnqzjnr =
0 ; } if ( ( int32_T ) rtP . Constant5_Value == 1 ) { fljxf0zdcw [ 0 ] = rtB
. luiagn315d [ 0 ] ; fljxf0zdcw [ 1 ] = rtB . luiagn315d [ 1 ] ; fljxf0zdcw [
2 ] = rtB . luiagn315d [ 2 ] ; } else { fljxf0zdcw [ 0 ] = ( ( rtDW .
n3oafbzym3 [ 0 ] * rtP . SineWave_PCos [ 0 ] + rtDW . mec1csjpdt [ 0 ] * rtP
. SineWave_PSin [ 0 ] ) * rtP . SineWave_HCos [ 0 ] + ( rtDW . mec1csjpdt [ 0
] * rtP . SineWave_PCos [ 0 ] - rtDW . n3oafbzym3 [ 0 ] * rtP . SineWave_PSin
[ 0 ] ) * rtP . SineWave_Hsin [ 0 ] ) * rtP . SineWave_Amp + rtP .
SineWave_Bias ; fljxf0zdcw [ 1 ] = ( ( rtDW . n3oafbzym3 [ 1 ] * rtP .
SineWave_PCos [ 1 ] + rtDW . mec1csjpdt [ 1 ] * rtP . SineWave_PSin [ 1 ] ) *
rtP . SineWave_HCos [ 1 ] + ( rtDW . mec1csjpdt [ 1 ] * rtP . SineWave_PCos [
1 ] - rtDW . n3oafbzym3 [ 1 ] * rtP . SineWave_PSin [ 1 ] ) * rtP .
SineWave_Hsin [ 1 ] ) * rtP . SineWave_Amp + rtP . SineWave_Bias ; fljxf0zdcw
[ 2 ] = ( ( rtDW . n3oafbzym3 [ 2 ] * rtP . SineWave_PCos [ 2 ] + rtDW .
mec1csjpdt [ 2 ] * rtP . SineWave_PSin [ 2 ] ) * rtP . SineWave_HCos [ 2 ] +
( rtDW . mec1csjpdt [ 2 ] * rtP . SineWave_PCos [ 2 ] - rtDW . n3oafbzym3 [ 2
] * rtP . SineWave_PSin [ 2 ] ) * rtP . SineWave_Hsin [ 2 ] ) * rtP .
SineWave_Amp + rtP . SineWave_Bias ; } if ( ( int32_T ) rtP .
Constant4_Value_f2ccycfp1g == 1 ) { lfrwzvun5e_idx_0 = rtB . pgwb1j4u3n ;
lfrwzvun5e_idx_1 = rtB . b3gkmlvhtj ; } else { nx5egkjgi3 = look1_pbinlxpw (
muDoubleScalarRem ( ssGetTaskTime ( rtS , 2 ) + rtP . Constant3_Value , rtP .
Constant1_Value_kqscwqq4vj ) * rtP . uib1_Gain , rtP . uDLookupTable_bp01Data
, rtP . uDLookupTable_tableData , & rtDW . h4meyfugs1 , 2U ) ; nx5egkjgi3 -=
rtP . Constant2_Value ; lfrwzvun5e_idx_0 = ( nx5egkjgi3 + 1.0 ) * 0.5 ;
lfrwzvun5e_idx_1 = ( nx5egkjgi3 - 1.0 ) * 0.5 ; } if ( rtB . jasinaajz3 ) {
nx5egkjgi3 = rtP . Gain_Gain * fljxf0zdcw [ 0 ] ; rtB . iqsu5vb0qp = (
nx5egkjgi3 >= lfrwzvun5e_idx_0 ) - ( nx5egkjgi3 <= lfrwzvun5e_idx_1 ) ; rtB .
esbgstqc1k = ( fljxf0zdcw [ 0 ] >= lfrwzvun5e_idx_0 ) - ( fljxf0zdcw [ 0 ] <=
lfrwzvun5e_idx_1 ) ; if ( ssIsMajorTimeStep ( rtS ) ) { srUpdateBC ( rtDW .
le4iyy3q51 ) ; } } if ( rtB . mjlsry1omh ) { rtB . lhrhljsi0l [ 0 ] = (
fljxf0zdcw [ 0 ] >= lfrwzvun5e_idx_0 ) - ( fljxf0zdcw [ 0 ] <=
lfrwzvun5e_idx_1 ) ; rtB . lhrhljsi0l [ 1 ] = ( fljxf0zdcw [ 1 ] >=
lfrwzvun5e_idx_0 ) - ( fljxf0zdcw [ 1 ] <= lfrwzvun5e_idx_1 ) ; rtB .
lhrhljsi0l [ 2 ] = ( fljxf0zdcw [ 2 ] >= lfrwzvun5e_idx_0 ) - ( fljxf0zdcw [
2 ] <= lfrwzvun5e_idx_1 ) ; if ( ssIsMajorTimeStep ( rtS ) ) { srUpdateBC (
rtDW . b43mt2kku5 ) ; } } if ( ! rtB . jasinaajz3 ) { fljxf0zdcw [ 0 ] = rtB
. lhrhljsi0l [ 0 ] ; fljxf0zdcw [ 1 ] = rtB . lhrhljsi0l [ 1 ] ; fljxf0zdcw [
2 ] = rtB . lhrhljsi0l [ 2 ] ; } else { fljxf0zdcw [ 0 ] = rtB . esbgstqc1k ;
fljxf0zdcw [ 1 ] = rtB . iqsu5vb0qp ; fljxf0zdcw [ 2 ] = rtB . iqsu5vb0qp ; }
fljxf0zdcw [ 0 ] += rtP . Constant2_Value_id2ug4wnj0 ; fljxf0zdcw [ 1 ] +=
rtP . Constant2_Value_id2ug4wnj0 ; switch ( ( int32_T ) fljxf0zdcw [ 0 ] ) {
case 1 : rtB . mi3zbxppas [ 0 ] = rtP . Constant12_Value [ 0 ] ; rtB .
mi3zbxppas [ 1 ] = rtP . Constant12_Value [ 1 ] ; rtB . mi3zbxppas [ 2 ] =
rtP . Constant12_Value [ 2 ] ; rtB . mi3zbxppas [ 3 ] = rtP .
Constant12_Value [ 3 ] ; break ; case 2 : rtB . mi3zbxppas [ 0 ] = rtP .
Constant13_Value [ 0 ] ; rtB . mi3zbxppas [ 1 ] = rtP . Constant13_Value [ 1
] ; rtB . mi3zbxppas [ 2 ] = rtP . Constant13_Value [ 2 ] ; rtB . mi3zbxppas
[ 3 ] = rtP . Constant13_Value [ 3 ] ; break ; default : rtB . mi3zbxppas [ 0
] = rtP . Constant11_Value [ 0 ] ; rtB . mi3zbxppas [ 1 ] = rtP .
Constant11_Value [ 1 ] ; rtB . mi3zbxppas [ 2 ] = rtP . Constant11_Value [ 2
] ; rtB . mi3zbxppas [ 3 ] = rtP . Constant11_Value [ 3 ] ; break ; } switch
( ( int32_T ) fljxf0zdcw [ 1 ] ) { case 1 : rtB . ahenbhgxgu [ 0 ] = rtP .
Constant12_Value [ 0 ] ; rtB . ahenbhgxgu [ 1 ] = rtP . Constant12_Value [ 1
] ; rtB . ahenbhgxgu [ 2 ] = rtP . Constant12_Value [ 2 ] ; rtB . ahenbhgxgu
[ 3 ] = rtP . Constant12_Value [ 3 ] ; break ; case 2 : rtB . ahenbhgxgu [ 0
] = rtP . Constant13_Value [ 0 ] ; rtB . ahenbhgxgu [ 1 ] = rtP .
Constant13_Value [ 1 ] ; rtB . ahenbhgxgu [ 2 ] = rtP . Constant13_Value [ 2
] ; rtB . ahenbhgxgu [ 3 ] = rtP . Constant13_Value [ 3 ] ; break ; default :
rtB . ahenbhgxgu [ 0 ] = rtP . Constant11_Value [ 0 ] ; rtB . ahenbhgxgu [ 1
] = rtP . Constant11_Value [ 1 ] ; rtB . ahenbhgxgu [ 2 ] = rtP .
Constant11_Value [ 2 ] ; rtB . ahenbhgxgu [ 3 ] = rtP . Constant11_Value [ 3
] ; break ; } switch ( ( int32_T ) ( fljxf0zdcw [ 2 ] + rtP .
Constant2_Value_id2ug4wnj0 ) ) { case 1 : rtB . en03atcxhu [ 0 ] = rtP .
Constant12_Value [ 0 ] ; rtB . en03atcxhu [ 1 ] = rtP . Constant12_Value [ 1
] ; rtB . en03atcxhu [ 2 ] = rtP . Constant12_Value [ 2 ] ; rtB . en03atcxhu
[ 3 ] = rtP . Constant12_Value [ 3 ] ; break ; case 2 : rtB . en03atcxhu [ 0
] = rtP . Constant13_Value [ 0 ] ; rtB . en03atcxhu [ 1 ] = rtP .
Constant13_Value [ 1 ] ; rtB . en03atcxhu [ 2 ] = rtP . Constant13_Value [ 2
] ; rtB . en03atcxhu [ 3 ] = rtP . Constant13_Value [ 3 ] ; break ; default :
rtB . en03atcxhu [ 0 ] = rtP . Constant11_Value [ 0 ] ; rtB . en03atcxhu [ 1
] = rtP . Constant11_Value [ 1 ] ; rtB . en03atcxhu [ 2 ] = rtP .
Constant11_Value [ 2 ] ; rtB . en03atcxhu [ 3 ] = rtP . Constant11_Value [ 3
] ; break ; } } if ( ssIsSampleHit ( rtS , 3 , 0 ) ) { P_p = rtP . Ts_Control
* rtP . Fnom * 6.2831853071795862 ; rtB . mqpcovqitm [ 0 ] = ( ( rtB .
jnimo5lv0m + rtP . Constant2_Value_lyii0iggwc [ 0 ] ) + rtP .
Constant_Value_prlpwr0gxs ) + P_p ; rtB . mqpcovqitm [ 1 ] = ( ( rtB .
jnimo5lv0m + rtP . Constant2_Value_lyii0iggwc [ 1 ] ) + rtP .
Constant_Value_prlpwr0gxs ) + P_p ; rtB . mqpcovqitm [ 2 ] = ( ( rtB .
jnimo5lv0m + rtP . Constant2_Value_lyii0iggwc [ 2 ] ) + rtP .
Constant_Value_prlpwr0gxs ) + P_p ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) {
jicqzbt3a1_idx_0 /= rtB . kxfvw5uauv ; jicqzbt3a1_idx_1 /= rtB . kxfvw5uauv ;
rtB . ar3wifnqiv = muDoubleScalarHypot ( jicqzbt3a1_idx_0 , jicqzbt3a1_idx_1
) ; nx5egkjgi3 = muDoubleScalarAtan2 ( jicqzbt3a1_idx_1 , jicqzbt3a1_idx_0 )
; rtB . lpiitgbbyf [ 0 ] = muDoubleScalarSin ( nx5egkjgi3 + rtB . mqpcovqitm
[ 0 ] ) * rtB . ar3wifnqiv ; rtB . lpiitgbbyf [ 1 ] = muDoubleScalarSin (
nx5egkjgi3 + rtB . mqpcovqitm [ 1 ] ) * rtB . ar3wifnqiv ; rtB . lpiitgbbyf [
2 ] = muDoubleScalarSin ( nx5egkjgi3 + rtB . mqpcovqitm [ 2 ] ) * rtB .
ar3wifnqiv ; nx5egkjgi3 = rtB . ozgmksrvpc * rtP . Rs_array_Value + rtB .
bsgixfioey ; rtB . ec4eu1c34c = ( muDoubleScalarExp ( 1.0 / rtB . kqp2cunbno
* nx5egkjgi3 ) - rtP . one_Value ) * rtB . mqwfucjhw5 ; rtB . jjtdt14g1y = (
nx5egkjgi3 / rtP . Rsh_array_Value * bzw3crffge + rtB . ec4eu1c34c ) -
nx5egkjgi3 / rtP . Rsh_array_5Sref_Value ; rtB . oo5we1pw3n = rtP .
donotdeletethisgain_Gain_fzvartv1ey * rtB . hliocuqujf [ 38 ] ; } if (
ssIsSampleHit ( rtS , 3 , 0 ) ) { if ( rtDW . fcwaufktro != 0 ) { rtB .
krgk34ykj1 = rtDW . otc3kwibnn ; } else { rtB . krgk34ykj1 = rtP .
Integ4_gainval_gph3gdfsp4 * rtB . bsgixfioey + rtDW . otc3kwibnn ; } rtB .
mkrkhji0cg = rtP . K1_Value ; { SimStruct * rts = ssGetSFunction ( rtS , 11 )
; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >= rtB .
mkrkhji0cg ) { rtB . k0men4hr1r = ( rtB . krgk34ykj1 - rtB . ckmd25fctl ) *
rtP . K2_Value + ( rtP . Gain1_Gain_lvijrrmj2x * rtB . bsgixfioey - rtP .
Gain_Gain_ikrhtetg1c * rtDW . inc2xczn1j ) ; } else { rtB . k0men4hr1r = rtDW
. hglk0ykmnp ; } if ( rtDW . oaicnmy4cr != 0 ) { rtB . esjg3pcq0t = rtDW .
iyz3blnd0r ; } else { rtB . esjg3pcq0t = rtP . Integ4_gainval_jbhrlhdx2q *
rtB . ozgmksrvpc + rtDW . iyz3blnd0r ; } rtB . l12i0pjkca = rtP .
K1_Value_jbgdaiaown ; { SimStruct * rts = ssGetSFunction ( rtS , 12 ) ;
sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >= rtB .
l12i0pjkca ) { rtB . b11aaunedj = ( rtB . esjg3pcq0t - rtB . ni2s4rtms3 ) *
rtP . K2_Value_ovjoxlqpjc + ( rtP . Gain1_Gain_cygkj45s0o * rtB . ozgmksrvpc
- rtP . Gain_Gain_mrwtozpa1q * rtDW . okoa0ga0es ) ; } else { rtB .
b11aaunedj = rtDW . mlqfus2yxb ; } rtB . a0vdijiybg = rtB . k0men4hr1r * rtB
. b11aaunedj * rtP . Gain_Gain_jivjysmzlr ; } if ( ssIsSampleHit ( rtS , 2 ,
0 ) ) { } if ( ssIsSampleHit ( rtS , 3 , 0 ) ) { if ( rtDW . oi4s4s3b2v != 0
) { rtDW . ev3w2qbtzq = muDoubleScalarSin ( rtP . sinwt_Freq * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . lm3x1szvf2 = muDoubleScalarCos ( rtP . sinwt_Freq *
ssGetTaskTime ( rtS , 3 ) ) ; rtDW . oi4s4s3b2v = 0 ; } rtB . o0xbrdncoc = (
( ( rtDW . ev3w2qbtzq * rtP . sinwt_PCos + rtDW . lm3x1szvf2 * rtP .
sinwt_PSin ) * rtP . sinwt_HCos + ( rtDW . lm3x1szvf2 * rtP . sinwt_PCos -
rtDW . ev3w2qbtzq * rtP . sinwt_PSin ) * rtP . sinwt_Hsin ) * rtP . sinwt_Amp
+ rtP . sinwt_Bias ) * rtB . mtdom0eizr [ 0 ] ; if ( rtDW . d5srsnrycq != 0 )
{ rtB . bzovtsvol2 = rtDW . cv44xoew41 ; } else { rtB . bzovtsvol2 = rtP .
Integ4_gainval_i0v3kwd3pb * rtB . o0xbrdncoc + rtDW . cv44xoew41 ; } rtB .
lvxda1zemz = rtP . K1_Value_mnxxbxkzc3 ; { SimStruct * rts = ssGetSFunction (
rtS , 13 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . lvxda1zemz ) { rtB . ea14lgpbfy = ( rtB . bzovtsvol2 - rtB . d0gp45izbd
) * rtP . K2_Value_ivipcgq0za + ( rtP . Gain1_Gain_gau2kona2s * rtB .
o0xbrdncoc - rtP . Gain_Gain_d5ujhlzr3p * rtDW . amx5pvlykq ) ; } else { rtB
. ea14lgpbfy = rtDW . l1nmdmtivq ; } if ( rtDW . bysaiwysza != 0 ) { rtDW .
fnwtwy11ey = muDoubleScalarSin ( rtP . coswt_Freq * ssGetTaskTime ( rtS , 3 )
) ; rtDW . hjvxevoa1u = muDoubleScalarCos ( rtP . coswt_Freq * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . bysaiwysza = 0 ; } rtB . fu3rpol0ph = ( ( ( rtDW .
fnwtwy11ey * rtP . coswt_PCos + rtDW . hjvxevoa1u * rtP . coswt_PSin ) * rtP
. coswt_HCos + ( rtDW . hjvxevoa1u * rtP . coswt_PCos - rtDW . fnwtwy11ey *
rtP . coswt_PSin ) * rtP . coswt_Hsin ) * rtP . coswt_Amp + rtP . coswt_Bias
) * rtB . mtdom0eizr [ 0 ] ; if ( rtDW . acdm1beais != 0 ) { rtB . lntmbixtgk
= rtDW . c3vwiq1oz1 ; } else { rtB . lntmbixtgk = rtP .
Integ4_gainval_msuhv1zzxl * rtB . fu3rpol0ph + rtDW . c3vwiq1oz1 ; } rtB .
hdh2j1iy05 = rtP . K1_Value_mgtlppjp42 ; { SimStruct * rts = ssGetSFunction (
rtS , 14 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . hdh2j1iy05 ) { rtB . nvqp25eoum = ( rtB . lntmbixtgk - rtB . f1i4vqi2kd
) * rtP . K2_Value_af05yiua5o + ( rtP . Gain1_Gain_cvnj41z3u1 * rtB .
fu3rpol0ph - rtP . Gain_Gain_er1jzlt204 * rtDW . g55lsvsoyn ) ; } else { rtB
. nvqp25eoum = rtDW . bf0dooycmi ; } bzw3crffge = rtP . RadDeg_Gain *
muDoubleScalarAtan2 ( rtB . nvqp25eoum , rtB . ea14lgpbfy ) * rtP .
degrad_Gain ; c4vhn0azip . re = muDoubleScalarHypot ( rtB . ea14lgpbfy , rtB
. nvqp25eoum ) * muDoubleScalarCos ( bzw3crffge ) ; c4vhn0azip . im =
muDoubleScalarHypot ( rtB . ea14lgpbfy , rtB . nvqp25eoum ) *
muDoubleScalarSin ( bzw3crffge ) ; if ( rtDW . fp5gljg1na != 0 ) { rtDW .
bo3u2joydk = muDoubleScalarSin ( rtP . sinwt_Freq_p3beiezqka * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . d2b1q3eao5 = muDoubleScalarCos ( rtP .
sinwt_Freq_p3beiezqka * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . fp5gljg1na = 0 ;
} rtB . hglrrwzja3 = ( ( ( rtDW . bo3u2joydk * rtP . sinwt_PCos_cyveu5wuih +
rtDW . d2b1q3eao5 * rtP . sinwt_PSin_modl3cgbwm ) * rtP .
sinwt_HCos_cafhkcfovv + ( rtDW . d2b1q3eao5 * rtP . sinwt_PCos_cyveu5wuih -
rtDW . bo3u2joydk * rtP . sinwt_PSin_modl3cgbwm ) * rtP .
sinwt_Hsin_e0rgtjbjpx ) * rtP . sinwt_Amp_kei0q2kps2 + rtP .
sinwt_Bias_d20x1us1zk ) * rtB . mtdom0eizr [ 1 ] ; if ( rtDW . i4s40mrg1p !=
0 ) { rtB . f4wsdyxsap = rtDW . is34tmu21f ; } else { rtB . f4wsdyxsap = rtP
. Integ4_gainval_gtzjg23pll * rtB . hglrrwzja3 + rtDW . is34tmu21f ; } rtB .
psu4dh2yoj = rtP . K1_Value_exdwlktlap ; { SimStruct * rts = ssGetSFunction (
rtS , 15 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . psu4dh2yoj ) { rtB . ccqwtokflg = ( rtB . f4wsdyxsap - rtB . on2hc5txnr
) * rtP . K2_Value_jy0nnlmsm2 + ( rtP . Gain1_Gain_bsbbtjbk4d * rtB .
hglrrwzja3 - rtP . Gain_Gain_jq3um4cz4j * rtDW . bgndzrawrf ) ; } else { rtB
. ccqwtokflg = rtDW . jwtcvtmaeo ; } if ( rtDW . f05y3hvfbl != 0 ) { rtDW .
lqckuso1xq = muDoubleScalarSin ( rtP . coswt_Freq_gttzxcelpy * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . jrdmpel4ku = muDoubleScalarCos ( rtP .
coswt_Freq_gttzxcelpy * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . f05y3hvfbl = 0 ;
} rtB . hxvph3vgfk = ( ( ( rtDW . lqckuso1xq * rtP . coswt_PCos_ontpys2zdd +
rtDW . jrdmpel4ku * rtP . coswt_PSin_khvpsemk1b ) * rtP .
coswt_HCos_oz23s2cxn2 + ( rtDW . jrdmpel4ku * rtP . coswt_PCos_ontpys2zdd -
rtDW . lqckuso1xq * rtP . coswt_PSin_khvpsemk1b ) * rtP .
coswt_Hsin_omgodzfzvz ) * rtP . coswt_Amp_nskemowyiu + rtP .
coswt_Bias_jk2fedlc5k ) * rtB . mtdom0eizr [ 1 ] ; if ( rtDW . e4uypnvxm4 !=
0 ) { rtB . aeqligfyi1 = rtDW . crf5cecvju ; } else { rtB . aeqligfyi1 = rtP
. Integ4_gainval_g3uamgfpqb * rtB . hxvph3vgfk + rtDW . crf5cecvju ; } rtB .
bxlm4t4z32 = rtP . K1_Value_g33io5pe2n ; { SimStruct * rts = ssGetSFunction (
rtS , 16 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . bxlm4t4z32 ) { rtB . klgyhsq3yy = ( rtB . aeqligfyi1 - rtB . fj2bifvg5t
) * rtP . K2_Value_ox1txrzba0 + ( rtP . Gain1_Gain_jawtsd2iys * rtB .
hxvph3vgfk - rtP . Gain_Gain_l1yybb0ozh * rtDW . o54g2pguii ) ; } else { rtB
. klgyhsq3yy = rtDW . berzbzjptr ; } bzw3crffge = rtP .
RadDeg_Gain_oothuzb2un * muDoubleScalarAtan2 ( rtB . klgyhsq3yy , rtB .
ccqwtokflg ) * rtP . degrad1_Gain ; ozyauwv5je . re = muDoubleScalarHypot (
rtB . ccqwtokflg , rtB . klgyhsq3yy ) * muDoubleScalarCos ( bzw3crffge ) ;
ozyauwv5je . im = muDoubleScalarHypot ( rtB . ccqwtokflg , rtB . klgyhsq3yy )
* muDoubleScalarSin ( bzw3crffge ) ; if ( rtDW . nzuplyapo4 != 0 ) { rtDW .
pmdevl30t5 = muDoubleScalarSin ( rtP . sinwt_Freq_ibh040bo2d * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . bfqdux1v4y = muDoubleScalarCos ( rtP .
sinwt_Freq_ibh040bo2d * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . nzuplyapo4 = 0 ;
} rtB . hiw0yqrayw = ( ( ( rtDW . pmdevl30t5 * rtP . sinwt_PCos_esk52xey53 +
rtDW . bfqdux1v4y * rtP . sinwt_PSin_ism3nzmned ) * rtP .
sinwt_HCos_lozsdsini2 + ( rtDW . bfqdux1v4y * rtP . sinwt_PCos_esk52xey53 -
rtDW . pmdevl30t5 * rtP . sinwt_PSin_ism3nzmned ) * rtP .
sinwt_Hsin_ltv4wuiu5i ) * rtP . sinwt_Amp_gglawa5b0j + rtP .
sinwt_Bias_ftcgy00q1q ) * rtB . mtdom0eizr [ 2 ] ; if ( rtDW . leakran3jy !=
0 ) { rtB . lqaoyuj05s = rtDW . eyj3xgyyy5 ; } else { rtB . lqaoyuj05s = rtP
. Integ4_gainval_h0bygbtnao * rtB . hiw0yqrayw + rtDW . eyj3xgyyy5 ; } rtB .
a2i2ov02lb = rtP . K1_Value_nofhnhfgqe ; { SimStruct * rts = ssGetSFunction (
rtS , 17 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . a2i2ov02lb ) { rtB . fbyhptghi3 = ( rtB . lqaoyuj05s - rtB . ienudooawn
) * rtP . K2_Value_cheibnriss + ( rtP . Gain1_Gain_lly5kv4kuf * rtB .
hiw0yqrayw - rtP . Gain_Gain_o1r20n5vmz * rtDW . iwb0lp4dat ) ; } else { rtB
. fbyhptghi3 = rtDW . mmleedibcx ; } if ( rtDW . aaw5cbewzs != 0 ) { rtDW .
phb5tgbsae = muDoubleScalarSin ( rtP . coswt_Freq_gp2xaew4z5 * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . cbutkiwxs3 = muDoubleScalarCos ( rtP .
coswt_Freq_gp2xaew4z5 * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . aaw5cbewzs = 0 ;
} rtB . hxdguojmgq = ( ( ( rtDW . phb5tgbsae * rtP . coswt_PCos_apooxuqtce +
rtDW . cbutkiwxs3 * rtP . coswt_PSin_pzecylb5ie ) * rtP .
coswt_HCos_e1lcq0jydt + ( rtDW . cbutkiwxs3 * rtP . coswt_PCos_apooxuqtce -
rtDW . phb5tgbsae * rtP . coswt_PSin_pzecylb5ie ) * rtP .
coswt_Hsin_p0lqeycuo2 ) * rtP . coswt_Amp_ov5trykaai + rtP .
coswt_Bias_caz0b22m5s ) * rtB . mtdom0eizr [ 2 ] ; if ( rtDW . grw0tq5110 !=
0 ) { rtB . eutj1tzkyf = rtDW . iwtvu1xbzm ; } else { rtB . eutj1tzkyf = rtP
. Integ4_gainval_lqadd2jned * rtB . hxdguojmgq + rtDW . iwtvu1xbzm ; } rtB .
pf0efcoh3z = rtP . K1_Value_piw4pfcum5 ; { SimStruct * rts = ssGetSFunction (
rtS , 18 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . pf0efcoh3z ) { rtB . atf0ppnipt = ( rtB . eutj1tzkyf - rtB . hqbivcjz0k
) * rtP . K2_Value_luz1rwbbpa + ( rtP . Gain1_Gain_kl3n41qkg4 * rtB .
hxdguojmgq - rtP . Gain_Gain_fc1p3rfcsm * rtDW . fdgmxn5rpv ) ; } else { rtB
. atf0ppnipt = rtDW . mkbsymk41p ; } bzw3crffge = rtP .
RadDeg_Gain_kam5dohr3p * muDoubleScalarAtan2 ( rtB . atf0ppnipt , rtB .
fbyhptghi3 ) * rtP . degrad2_Gain ; jrcfqdma2u . re = muDoubleScalarHypot (
rtB . fbyhptghi3 , rtB . atf0ppnipt ) * muDoubleScalarCos ( bzw3crffge ) ;
jrcfqdma2u . im = muDoubleScalarHypot ( rtB . fbyhptghi3 , rtB . atf0ppnipt )
* muDoubleScalarSin ( bzw3crffge ) ; gj0ekbre2u ( rtS , rtP .
Constant_Value_gu00guxond , c4vhn0azip , ozyauwv5je , jrcfqdma2u , & rtB .
jr3gbt2o32 , & rtDW . jr3gbt2o32 , & rtP . jr3gbt2o32 ) ; gj0ekbre2u ( rtS ,
rtP . Constant1_Value_d3fgit3q2y , c4vhn0azip , ozyauwv5je , jrcfqdma2u , &
rtB . gj0ekbre2up , & rtDW . gj0ekbre2up , & rtP . gj0ekbre2up ) ; ihqnx345xu
( rtS , rtP . Constant2_Value_dvfzdjynxb , c4vhn0azip , ozyauwv5je ,
jrcfqdma2u , & rtB . ihqnx345xuh , & rtDW . ihqnx345xuh , & rtP . ihqnx345xuh
) ; if ( rtDW . fhmti5fnmk != 0 ) { rtDW . e4oo4zzsto = muDoubleScalarSin (
rtP . sinwt_Freq_g4kja400xj * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . ni0wsgyutj
= muDoubleScalarCos ( rtP . sinwt_Freq_g4kja400xj * ssGetTaskTime ( rtS , 3 )
) ; rtDW . fhmti5fnmk = 0 ; } rtB . jf3zbnuh3n = ( ( ( rtDW . e4oo4zzsto *
rtP . sinwt_PCos_ffxrlya5uh + rtDW . ni0wsgyutj * rtP . sinwt_PSin_mxp5pxyazu
) * rtP . sinwt_HCos_ikm4ke3nt1 + ( rtDW . ni0wsgyutj * rtP .
sinwt_PCos_ffxrlya5uh - rtDW . e4oo4zzsto * rtP . sinwt_PSin_mxp5pxyazu ) *
rtP . sinwt_Hsin_f4ff1jm01o ) * rtP . sinwt_Amp_a5iuycgbj5 + rtP .
sinwt_Bias_o4hfny211d ) * rtB . j40qpekti4 [ 0 ] ; if ( rtDW . ir0a11kmwf !=
0 ) { rtB . f4bvvydmna = rtDW . h4kspo12ff ; } else { rtB . f4bvvydmna = rtP
. Integ4_gainval_gvvjdr1enu * rtB . jf3zbnuh3n + rtDW . h4kspo12ff ; } rtB .
fvjbmcpghe = rtP . K1_Value_ly41q0lfne ; { SimStruct * rts = ssGetSFunction (
rtS , 19 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . fvjbmcpghe ) { rtB . m4cw5jelgb = ( rtB . f4bvvydmna - rtB . nvcc0bievu
) * rtP . K2_Value_peraggcnbk + ( rtP . Gain1_Gain_k45yw5u1ri * rtB .
jf3zbnuh3n - rtP . Gain_Gain_htrn2od5eo * rtDW . ad2sn3k3gj ) ; } else { rtB
. m4cw5jelgb = rtDW . jy2blxntru ; } if ( rtDW . ofkxr3wbpq != 0 ) { rtDW .
gdfrdi2cqi = muDoubleScalarSin ( rtP . coswt_Freq_iohcbhspnz * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . lanv5zwkwv = muDoubleScalarCos ( rtP .
coswt_Freq_iohcbhspnz * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . ofkxr3wbpq = 0 ;
} rtB . e3r3gwb15i = ( ( ( rtDW . gdfrdi2cqi * rtP . coswt_PCos_jgy3xycijn +
rtDW . lanv5zwkwv * rtP . coswt_PSin_ckpub3oin5 ) * rtP .
coswt_HCos_ih2fmjgzvo + ( rtDW . lanv5zwkwv * rtP . coswt_PCos_jgy3xycijn -
rtDW . gdfrdi2cqi * rtP . coswt_PSin_ckpub3oin5 ) * rtP .
coswt_Hsin_ki40gemxaa ) * rtP . coswt_Amp_ph0slmkaz2 + rtP .
coswt_Bias_mxccsalmjx ) * rtB . j40qpekti4 [ 0 ] ; if ( rtDW . bixjjcy32b !=
0 ) { rtB . b4ioytjw1o = rtDW . dbrynerjdo ; } else { rtB . b4ioytjw1o = rtP
. Integ4_gainval_e3hymojevp * rtB . e3r3gwb15i + rtDW . dbrynerjdo ; } rtB .
gmfn04oapy = rtP . K1_Value_odshxihjs1 ; { SimStruct * rts = ssGetSFunction (
rtS , 20 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . gmfn04oapy ) { rtB . g5q20xpjnp = ( rtB . b4ioytjw1o - rtB . dhrszd2dqf
) * rtP . K2_Value_ca4rzsh0ua + ( rtP . Gain1_Gain_h0j53ikn03 * rtB .
e3r3gwb15i - rtP . Gain_Gain_pvpuvbbodz * rtDW . ioxogrdks0 ) ; } else { rtB
. g5q20xpjnp = rtDW . ejhppynn0z ; } bzw3crffge = rtP .
RadDeg_Gain_hfllzvpq3o * muDoubleScalarAtan2 ( rtB . g5q20xpjnp , rtB .
m4cw5jelgb ) * rtP . degrad_Gain_num4oalqxy ; otzzblqr2k . re =
muDoubleScalarHypot ( rtB . m4cw5jelgb , rtB . g5q20xpjnp ) *
muDoubleScalarCos ( bzw3crffge ) ; otzzblqr2k . im = muDoubleScalarHypot (
rtB . m4cw5jelgb , rtB . g5q20xpjnp ) * muDoubleScalarSin ( bzw3crffge ) ; if
( rtDW . lmxa3ssfz4 != 0 ) { rtDW . his1pld4hz = muDoubleScalarSin ( rtP .
sinwt_Freq_pkofylmhnt * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . arh00ntoml =
muDoubleScalarCos ( rtP . sinwt_Freq_pkofylmhnt * ssGetTaskTime ( rtS , 3 ) )
; rtDW . lmxa3ssfz4 = 0 ; } rtB . b24n3f4xp0 = ( ( ( rtDW . his1pld4hz * rtP
. sinwt_PCos_ilquu0arpw + rtDW . arh00ntoml * rtP . sinwt_PSin_bllazdudnu ) *
rtP . sinwt_HCos_c3em0todhq + ( rtDW . arh00ntoml * rtP .
sinwt_PCos_ilquu0arpw - rtDW . his1pld4hz * rtP . sinwt_PSin_bllazdudnu ) *
rtP . sinwt_Hsin_i0nd0ubqqg ) * rtP . sinwt_Amp_lmppjqm2un + rtP .
sinwt_Bias_i50qywn0ii ) * rtB . j40qpekti4 [ 1 ] ; if ( rtDW . lob2ph3yhu !=
0 ) { rtB . pddsnfsfnu = rtDW . dqddocajhg ; } else { rtB . pddsnfsfnu = rtP
. Integ4_gainval_o4m2wju04m * rtB . b24n3f4xp0 + rtDW . dqddocajhg ; } rtB .
ilbet5yid2 = rtP . K1_Value_fascbhcv3t ; { SimStruct * rts = ssGetSFunction (
rtS , 21 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . ilbet5yid2 ) { rtB . bjoxavkoai = ( rtB . pddsnfsfnu - rtB . fbe5ufjhz0
) * rtP . K2_Value_bqbhuqvuc2 + ( rtP . Gain1_Gain_aqpmhpqydp * rtB .
b24n3f4xp0 - rtP . Gain_Gain_c51ds1jfln * rtDW . e2jfcom1xu ) ; } else { rtB
. bjoxavkoai = rtDW . km0mzzn3gj ; } if ( rtDW . kso0zmov5s != 0 ) { rtDW .
birk1htcrc = muDoubleScalarSin ( rtP . coswt_Freq_aek3lx4btw * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . pcj40bs3ff = muDoubleScalarCos ( rtP .
coswt_Freq_aek3lx4btw * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . kso0zmov5s = 0 ;
} rtB . lk335slscy = ( ( ( rtDW . birk1htcrc * rtP . coswt_PCos_oqm4st2je0 +
rtDW . pcj40bs3ff * rtP . coswt_PSin_bdolgwlxdg ) * rtP .
coswt_HCos_cs3y4qtzeb + ( rtDW . pcj40bs3ff * rtP . coswt_PCos_oqm4st2je0 -
rtDW . birk1htcrc * rtP . coswt_PSin_bdolgwlxdg ) * rtP .
coswt_Hsin_fuzcqltv4d ) * rtP . coswt_Amp_aqxb2j5jnu + rtP .
coswt_Bias_frzp530tnq ) * rtB . j40qpekti4 [ 1 ] ; if ( rtDW . fuhanhc4z4 !=
0 ) { rtB . cyaarq5wc5 = rtDW . llg10jep1x ; } else { rtB . cyaarq5wc5 = rtP
. Integ4_gainval_dmsyi0ujpc * rtB . lk335slscy + rtDW . llg10jep1x ; } rtB .
nabvxanmoe = rtP . K1_Value_okiumpl42l ; { SimStruct * rts = ssGetSFunction (
rtS , 22 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . nabvxanmoe ) { rtB . g5azq2cdjs = ( rtB . cyaarq5wc5 - rtB . djupb53czf
) * rtP . K2_Value_gzomggch2d + ( rtP . Gain1_Gain_ck1bqs4xph * rtB .
lk335slscy - rtP . Gain_Gain_nxlojnsi3h * rtDW . kkj5lgh0sd ) ; } else { rtB
. g5azq2cdjs = rtDW . oersb5zkry ; } bzw3crffge = rtP .
RadDeg_Gain_lezhz3zr05 * muDoubleScalarAtan2 ( rtB . g5azq2cdjs , rtB .
bjoxavkoai ) * rtP . degrad1_Gain_gcrkcsbsjy ; m0bkcj42lo . re =
muDoubleScalarHypot ( rtB . bjoxavkoai , rtB . g5azq2cdjs ) *
muDoubleScalarCos ( bzw3crffge ) ; m0bkcj42lo . im = muDoubleScalarHypot (
rtB . bjoxavkoai , rtB . g5azq2cdjs ) * muDoubleScalarSin ( bzw3crffge ) ; if
( rtDW . gqdl0mj0xk != 0 ) { rtDW . l54ofk2ea3 = muDoubleScalarSin ( rtP .
sinwt_Freq_i5xw5disvz * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . bl13n2tmiw =
muDoubleScalarCos ( rtP . sinwt_Freq_i5xw5disvz * ssGetTaskTime ( rtS , 3 ) )
; rtDW . gqdl0mj0xk = 0 ; } rtB . fisyjtz0ao = ( ( ( rtDW . l54ofk2ea3 * rtP
. sinwt_PCos_owldbifmfn + rtDW . bl13n2tmiw * rtP . sinwt_PSin_bs1ts2ayri ) *
rtP . sinwt_HCos_lv5zpnsoyo + ( rtDW . bl13n2tmiw * rtP .
sinwt_PCos_owldbifmfn - rtDW . l54ofk2ea3 * rtP . sinwt_PSin_bs1ts2ayri ) *
rtP . sinwt_Hsin_kfaxcimxsb ) * rtP . sinwt_Amp_oe50flwsvz + rtP .
sinwt_Bias_hpmk1ox4mk ) * rtB . j40qpekti4 [ 2 ] ; if ( rtDW . phos00jdib !=
0 ) { rtB . mxfhajo1pf = rtDW . mxwbciposz ; } else { rtB . mxfhajo1pf = rtP
. Integ4_gainval_lqpuvz5try * rtB . fisyjtz0ao + rtDW . mxwbciposz ; } rtB .
inbgwouqfx = rtP . K1_Value_fl2t5hhsxt ; { SimStruct * rts = ssGetSFunction (
rtS , 23 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . inbgwouqfx ) { rtB . jiw5saclae = ( rtB . mxfhajo1pf - rtB . jrohv3tr0m
) * rtP . K2_Value_fl0eknqnzp + ( rtP . Gain1_Gain_phi4msvodo * rtB .
fisyjtz0ao - rtP . Gain_Gain_a2j2ruaijq * rtDW . mvp44fdh2o ) ; } else { rtB
. jiw5saclae = rtDW . fwzzbi3hli ; } if ( rtDW . bn3v01mqq4 != 0 ) { rtDW .
dpc4svbkri = muDoubleScalarSin ( rtP . coswt_Freq_hhpfztkccd * ssGetTaskTime
( rtS , 3 ) ) ; rtDW . nlvv2lvgnj = muDoubleScalarCos ( rtP .
coswt_Freq_hhpfztkccd * ssGetTaskTime ( rtS , 3 ) ) ; rtDW . bn3v01mqq4 = 0 ;
} rtB . jdnb2gbavk = ( ( ( rtDW . dpc4svbkri * rtP . coswt_PCos_exeoqhe2el +
rtDW . nlvv2lvgnj * rtP . coswt_PSin_pc2mzqmykn ) * rtP .
coswt_HCos_dtrnnwyvoi + ( rtDW . nlvv2lvgnj * rtP . coswt_PCos_exeoqhe2el -
rtDW . dpc4svbkri * rtP . coswt_PSin_pc2mzqmykn ) * rtP .
coswt_Hsin_nt154hil4g ) * rtP . coswt_Amp_ajaph324eb + rtP .
coswt_Bias_er20tnmo5c ) * rtB . j40qpekti4 [ 2 ] ; if ( rtDW . ckxcx1vdsj !=
0 ) { rtB . kmn2nxupno = rtDW . pjo1bnuqfk ; } else { rtB . kmn2nxupno = rtP
. Integ4_gainval_ae24pre0oj * rtB . jdnb2gbavk + rtDW . pjo1bnuqfk ; } rtB .
l1d3sn10xu = rtP . K1_Value_huwvvbdsqd ; { SimStruct * rts = ssGetSFunction (
rtS , 24 ) ; sfcnOutputs ( rts , 0 ) ; } if ( ssGetTaskTime ( rtS , 3 ) >=
rtB . l1d3sn10xu ) { rtB . ltugjcg352 = ( rtB . kmn2nxupno - rtB . mvlgec43se
) * rtP . K2_Value_nuhsjmrzct + ( rtP . Gain1_Gain_f5mqebcvsk * rtB .
jdnb2gbavk - rtP . Gain_Gain_afzupnanth * rtDW . h45pdgkmki ) ; } else { rtB
. ltugjcg352 = rtDW . ldxbwl52qd ; } bzw3crffge = rtP .
RadDeg_Gain_bjvkdbizli * muDoubleScalarAtan2 ( rtB . ltugjcg352 , rtB .
jiw5saclae ) * rtP . degrad2_Gain_fvnkccap1v ; gmxsiixs0p . re =
muDoubleScalarHypot ( rtB . jiw5saclae , rtB . ltugjcg352 ) *
muDoubleScalarCos ( bzw3crffge ) ; gmxsiixs0p . im = muDoubleScalarHypot (
rtB . jiw5saclae , rtB . ltugjcg352 ) * muDoubleScalarSin ( bzw3crffge ) ;
gj0ekbre2u ( rtS , rtP . Constant_Value_fal5u45x5b , otzzblqr2k , m0bkcj42lo
, gmxsiixs0p , & rtB . h1nhkvogk5 , & rtDW . h1nhkvogk5 , & rtP . h1nhkvogk5
) ; gj0ekbre2u ( rtS , rtP . Constant1_Value_gerhfiv2al , otzzblqr2k ,
m0bkcj42lo , gmxsiixs0p , & rtB . drzlaqk2wl , & rtDW . drzlaqk2wl , & rtP .
drzlaqk2wl ) ; ihqnx345xu ( rtS , rtP . Constant2_Value_h0okejydpk ,
otzzblqr2k , m0bkcj42lo , gmxsiixs0p , & rtB . cxxbka35es , & rtDW .
cxxbka35es , & rtP . cxxbka35es ) ; muDoubleScalarSinCos ( rtP . DegRad_Gain
* ( rtP . raddeg1_Gain * muDoubleScalarAtan2 ( rtB . jr3gbt2o32 . pcsvo2nso5
. im , rtB . jr3gbt2o32 . pcsvo2nso5 . re ) - rtP . raddeg1_Gain_joedyuhdkb *
muDoubleScalarAtan2 ( rtB . h1nhkvogk5 . pcsvo2nso5 . im , rtB . h1nhkvogk5 .
pcsvo2nso5 . re ) ) , & bzw3crffge , & nx5egkjgi3 ) ; rtB . mhldlmlbft =
muDoubleScalarHypot ( rtB . jr3gbt2o32 . pcsvo2nso5 . re , rtB . jr3gbt2o32 .
pcsvo2nso5 . im ) * muDoubleScalarHypot ( rtB . h1nhkvogk5 . pcsvo2nso5 . re
, rtB . h1nhkvogk5 . pcsvo2nso5 . im ) * rtP . Gain1_Gain_b0ayny2lnu *
nx5egkjgi3 * rtP . W2kW_Gain ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { if (
ssGetLogOutput ( rtS ) ) { StructLogVar * svar = ( StructLogVar * ) rtDW .
jquzhmx440 . LoggedData [ 0 ] ; LogVar * var = svar -> signals . values ; {
double locTime = ssGetTaskTime ( rtS , 2 ) ; ; rt_UpdateLogVar ( ( LogVar * )
svar -> time , & locTime , 0 ) ; } { real_T up0 [ 1 ] ; up0 [ 0 ] = rtB .
mtdom0eizr [ 0 ] ; rt_UpdateLogVar ( ( LogVar * ) var , up0 , 0 ) ; var = var
-> next ; } { real_T up1 [ 1 ] ; up1 [ 0 ] = rtB . j40qpekti4 [ 0 ] ;
rt_UpdateLogVar ( ( LogVar * ) var , up1 , 0 ) ; } } rtB . kzukygsbbq [ 0 ] =
rtP . donotdeletethisgain_Gain_lcnbsf0lej * rtB . hliocuqujf [ 34 ] * rtP .
Kv1_Gain_oxfuo4itvi ; rtB . kzukygsbbq [ 1 ] = rtP .
donotdeletethisgain_Gain_gvlzyc5wfl * rtB . hliocuqujf [ 35 ] * rtP .
Kv1_Gain_oxfuo4itvi ; rtB . kzukygsbbq [ 2 ] = rtP .
donotdeletethisgain_Gain_mjapajrwub * rtB . hliocuqujf [ 36 ] * rtP .
Kv1_Gain_oxfuo4itvi ; rtB . etkri0io2o [ 0 ] = rtP .
donotdeletethisgain_Gain_ktpkblxqtm * rtB . hliocuqujf [ 43 ] * rtP .
Kv_Gain_gz5snr4zgc ; rtB . etkri0io2o [ 1 ] = rtP .
donotdeletethisgain_Gain_i4321lkuie * rtB . hliocuqujf [ 44 ] * rtP .
Kv_Gain_gz5snr4zgc ; rtB . etkri0io2o [ 2 ] = rtP .
donotdeletethisgain_Gain_jyxp4ll2lp * rtB . hliocuqujf [ 45 ] * rtP .
Kv_Gain_gz5snr4zgc ; rtB . dtnbe2rzvu = ( rtB . kzukygsbbq [ 0 ] * rtB .
etkri0io2o [ 0 ] + rtB . kzukygsbbq [ 1 ] * rtB . etkri0io2o [ 1 ] ) + rtB .
kzukygsbbq [ 2 ] * rtB . etkri0io2o [ 2 ] ; rtB . hxgrahbcuh = ( ( ( rtB .
kzukygsbbq [ 1 ] - rtB . kzukygsbbq [ 2 ] ) * rtB . etkri0io2o [ 0 ] + ( rtB
. kzukygsbbq [ 2 ] - rtB . kzukygsbbq [ 0 ] ) * rtB . etkri0io2o [ 1 ] ) + (
rtB . kzukygsbbq [ 0 ] - rtB . kzukygsbbq [ 1 ] ) * rtB . etkri0io2o [ 2 ] )
* rtP . puV_Gain ; } kyedbo0mxu ( rtS , rtB . nkxqxwgteh , rtB . kzukygsbbq ,
& rtB . kyedbo0mxuc , & rtDW . kyedbo0mxuc , & rtP . kyedbo0mxuc , & rtX .
kyedbo0mxuc , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> kyedbo0mxuc
) ; gllp1vimnh ( rtS , rtB . ocp2ppapt1 , rtB . kzukygsbbq , & rtB .
gllp1vimnhl , & rtDW . gllp1vimnhl , & rtP . gllp1vimnhl , & rtX .
gllp1vimnhl , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> gllp1vimnhl
) ; if ( rtB . ocp2ppapt1 ) { rtB . kuplk1gg1z [ 0 ] = rtB . gllp1vimnhl .
gx0tigur0o [ 0 ] ; rtB . kuplk1gg1z [ 1 ] = rtB . gllp1vimnhl . gx0tigur0o [
1 ] ; rtB . kuplk1gg1z [ 2 ] = rtB . gllp1vimnhl . gx0tigur0o [ 2 ] ; } else
{ rtB . kuplk1gg1z [ 0 ] = rtB . kyedbo0mxuc . me4ylfixyn [ 0 ] ; rtB .
kuplk1gg1z [ 1 ] = rtB . kyedbo0mxuc . me4ylfixyn [ 1 ] ; rtB . kuplk1gg1z [
2 ] = rtB . kyedbo0mxuc . me4ylfixyn [ 2 ] ; } kyedbo0mxu ( rtS , rtB .
emd0mf5yqm , rtB . etkri0io2o , & rtB . oa2z5wykl2 , & rtDW . oa2z5wykl2 , &
rtP . oa2z5wykl2 , & rtX . oa2z5wykl2 , & ( ( XDis * ) ssGetContStateDisabled
( rtS ) ) -> oa2z5wykl2 ) ; gllp1vimnh ( rtS , rtB . fk5vinpzuf , rtB .
etkri0io2o , & rtB . e2mpwvt2ua , & rtDW . e2mpwvt2ua , & rtP . e2mpwvt2ua ,
& rtX . e2mpwvt2ua , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) ->
e2mpwvt2ua ) ; if ( rtB . fk5vinpzuf ) { rtB . ffpomxcrhj [ 0 ] = rtB .
e2mpwvt2ua . gx0tigur0o [ 0 ] ; rtB . ffpomxcrhj [ 1 ] = rtB . e2mpwvt2ua .
gx0tigur0o [ 1 ] ; rtB . ffpomxcrhj [ 2 ] = rtB . e2mpwvt2ua . gx0tigur0o [ 2
] ; } else { rtB . ffpomxcrhj [ 0 ] = rtB . oa2z5wykl2 . me4ylfixyn [ 0 ] ;
rtB . ffpomxcrhj [ 1 ] = rtB . oa2z5wykl2 . me4ylfixyn [ 1 ] ; rtB .
ffpomxcrhj [ 2 ] = rtB . oa2z5wykl2 . me4ylfixyn [ 2 ] ; } gllp1vimnh ( rtS ,
rtB . n3g3acn3kz , rtB . kzukygsbbq , & rtB . jhi4zkfadx , & rtDW .
jhi4zkfadx , & rtP . jhi4zkfadx , & rtX . jhi4zkfadx , & ( ( XDis * )
ssGetContStateDisabled ( rtS ) ) -> jhi4zkfadx ) ; kyedbo0mxu ( rtS , rtB .
oamr1wipoz , rtB . kzukygsbbq , & rtB . mdskmorhmi , & rtDW . mdskmorhmi , &
rtP . mdskmorhmi , & rtX . mdskmorhmi , & ( ( XDis * ) ssGetContStateDisabled
( rtS ) ) -> mdskmorhmi ) ; if ( rtB . n3g3acn3kz ) { fljxf0zdcw [ 0 ] = rtB
. jhi4zkfadx . gx0tigur0o [ 0 ] ; fljxf0zdcw [ 1 ] = rtB . jhi4zkfadx .
gx0tigur0o [ 1 ] ; fljxf0zdcw [ 2 ] = rtB . jhi4zkfadx . gx0tigur0o [ 2 ] ; }
else { fljxf0zdcw [ 0 ] = rtB . mdskmorhmi . me4ylfixyn [ 0 ] ; fljxf0zdcw [
1 ] = rtB . mdskmorhmi . me4ylfixyn [ 1 ] ; fljxf0zdcw [ 2 ] = rtB .
mdskmorhmi . me4ylfixyn [ 2 ] ; } nx5egkjgi3 = fljxf0zdcw [ 0 ] * fljxf0zdcw
[ 0 ] ; rtB . ibnjsgargl [ 0 ] = rtX . f2y4dmvcyy [ 0 ] ; jicqzbt3a1_idx_0 =
fljxf0zdcw [ 1 ] * fljxf0zdcw [ 1 ] ; rtB . ibnjsgargl [ 1 ] = rtX .
f2y4dmvcyy [ 1 ] ; P_p = fljxf0zdcw [ 2 ] * fljxf0zdcw [ 2 ] ; rtB .
ibnjsgargl [ 2 ] = rtX . f2y4dmvcyy [ 2 ] ; { real_T * * uBuffer = ( real_T *
* ) & rtDW . lfqiakux1j . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & rtDW . lfqiakux1j . TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT (
rtS ) ; real_T tMinusDelay ; { int_T i1 ; real_T * y0 = & d3ib2izzs4 [ 0 ] ;
int_T * iw_Tail = & rtDW . drhtugugfl . Tail [ 0 ] ; int_T * iw_Head = & rtDW
. drhtugugfl . Head [ 0 ] ; int_T * iw_Last = & rtDW . drhtugugfl . Last [ 0
] ; int_T * iw_CircularBufSize = & rtDW . drhtugugfl . CircularBufSize [ 0 ]
; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) { tMinusDelay = ( ( rtP .
TransportDelay_Delay > 0.0 ) ? rtP . TransportDelay_Delay : 0.0 ) ;
tMinusDelay = simTime - tMinusDelay ; y0 [ i1 ] = rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , iw_CircularBufSize [ i1 ] , &
iw_Last [ i1 ] , iw_Tail [ i1 ] , iw_Head [ i1 ] , rtP .
TransportDelay_InitOutput , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( rtS ) &&
( ssGetTimeOfLastOutput ( rtS ) == ssGetT ( rtS ) ) ) ) ; tBuffer ++ ;
uBuffer ++ ; } } } if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . o2gpo0dp4u =
rtP . K1_Value_nt1eijy1al ; rtB . mnzoxhxi2g [ 0 ] = rtDW . dpquh1vse1 [ 0 ]
; rtB . mnzoxhxi2g [ 1 ] = rtDW . dpquh1vse1 [ 1 ] ; rtB . mnzoxhxi2g [ 2 ] =
rtDW . dpquh1vse1 [ 2 ] ; } if ( ssGetT ( rtS ) >= rtB . o2gpo0dp4u ) { rtB .
jeu3urzybv [ 0 ] = ( rtB . ibnjsgargl [ 0 ] - d3ib2izzs4 [ 0 ] ) * rtP .
Gain_Gain_evychckliv ; rtB . jeu3urzybv [ 1 ] = ( rtB . ibnjsgargl [ 1 ] -
d3ib2izzs4 [ 1 ] ) * rtP . Gain_Gain_evychckliv ; rtB . jeu3urzybv [ 2 ] = (
rtB . ibnjsgargl [ 2 ] - d3ib2izzs4 [ 2 ] ) * rtP . Gain_Gain_evychckliv ; }
else { rtB . jeu3urzybv [ 0 ] = rtB . mnzoxhxi2g [ 0 ] ; rtB . jeu3urzybv [ 1
] = rtB . mnzoxhxi2g [ 1 ] ; rtB . jeu3urzybv [ 2 ] = rtB . mnzoxhxi2g [ 2 ]
; } lfrwzvun5e_idx_0 = rtB . jeu3urzybv [ 0 ] * rtB . jeu3urzybv [ 0 ] ; dP =
rtB . jeu3urzybv [ 1 ] * rtB . jeu3urzybv [ 1 ] ; jicqzbt3a1_idx_1 = rtB .
jeu3urzybv [ 2 ] * rtB . jeu3urzybv [ 2 ] ; gllp1vimnh ( rtS , rtB .
c5pw4pqcew , rtB . kzukygsbbq , & rtB . blrugcnivv , & rtDW . blrugcnivv , &
rtP . blrugcnivv , & rtX . blrugcnivv , & ( ( XDis * ) ssGetContStateDisabled
( rtS ) ) -> blrugcnivv ) ; kyedbo0mxu ( rtS , rtB . odjvodg0jz , rtB .
kzukygsbbq , & rtB . iks0grgus1 , & rtDW . iks0grgus1 , & rtP . iks0grgus1 ,
& rtX . iks0grgus1 , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) ->
iks0grgus1 ) ; if ( rtB . c5pw4pqcew ) { fljxf0zdcw [ 0 ] = rtB . blrugcnivv
. gx0tigur0o [ 0 ] ; fljxf0zdcw [ 1 ] = rtB . blrugcnivv . gx0tigur0o [ 1 ] ;
fljxf0zdcw [ 2 ] = rtB . blrugcnivv . gx0tigur0o [ 2 ] ; } else { fljxf0zdcw
[ 0 ] = rtB . iks0grgus1 . me4ylfixyn [ 0 ] ; fljxf0zdcw [ 1 ] = rtB .
iks0grgus1 . me4ylfixyn [ 1 ] ; fljxf0zdcw [ 2 ] = rtB . iks0grgus1 .
me4ylfixyn [ 2 ] ; } bzw3crffge = fljxf0zdcw [ 0 ] * fljxf0zdcw [ 0 ] ;
nx5egkjgi3 = ( nx5egkjgi3 - lfrwzvun5e_idx_0 ) - bzw3crffge ; if ( bzw3crffge
> rtP . Saturationtoavoiddivisionby0_UpperSat ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat ; } else { if ( bzw3crffge < rtP .
Saturationtoavoiddivisionby0_LowerSat ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat ; } } bzw3crffge = nx5egkjgi3 /
bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ) { fljxf0zdcw [ 0 ] = rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ; } else if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ) { fljxf0zdcw [ 0 ] =
rtP . Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ; } else { fljxf0zdcw
[ 0 ] = bzw3crffge ; } bzw3crffge = fljxf0zdcw [ 1 ] * fljxf0zdcw [ 1 ] ;
nx5egkjgi3 = ( jicqzbt3a1_idx_0 - dP ) - bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat ; } else { if ( bzw3crffge < rtP .
Saturationtoavoiddivisionby0_LowerSat ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat ; } } bzw3crffge = nx5egkjgi3 /
bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ) { fljxf0zdcw [ 1 ] = rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ; } else if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ) { fljxf0zdcw [ 1 ] =
rtP . Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ; } else { fljxf0zdcw
[ 1 ] = bzw3crffge ; } bzw3crffge = fljxf0zdcw [ 2 ] * fljxf0zdcw [ 2 ] ;
nx5egkjgi3 = ( P_p - jicqzbt3a1_idx_1 ) - bzw3crffge ; if ( bzw3crffge > rtP
. Saturationtoavoiddivisionby0_UpperSat ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat ; } else { if ( bzw3crffge < rtP .
Saturationtoavoiddivisionby0_LowerSat ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat ; } } bzw3crffge = nx5egkjgi3 /
bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ; } else { if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ; } } if ( ssIsMajorTimeStep
( rtS ) ) { if ( rtDW . flcrf2pa4r != 0 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; rtDW . flcrf2pa4r = 0 ;
} rtB . b3h2v3br2u [ 0 ] = muDoubleScalarSqrt ( fljxf0zdcw [ 0 ] ) ; rtB .
b3h2v3br2u [ 1 ] = muDoubleScalarSqrt ( fljxf0zdcw [ 1 ] ) ; rtB . b3h2v3br2u
[ 2 ] = muDoubleScalarSqrt ( bzw3crffge ) ; } else { if ( fljxf0zdcw [ 0 ] <
0.0 ) { rtB . b3h2v3br2u [ 0 ] = - muDoubleScalarSqrt ( muDoubleScalarAbs (
fljxf0zdcw [ 0 ] ) ) ; } else { rtB . b3h2v3br2u [ 0 ] = muDoubleScalarSqrt (
fljxf0zdcw [ 0 ] ) ; } if ( fljxf0zdcw [ 0 ] < 0.0 ) { rtDW . flcrf2pa4r = 1
; } if ( fljxf0zdcw [ 1 ] < 0.0 ) { rtB . b3h2v3br2u [ 1 ] = -
muDoubleScalarSqrt ( muDoubleScalarAbs ( fljxf0zdcw [ 1 ] ) ) ; } else { rtB
. b3h2v3br2u [ 1 ] = muDoubleScalarSqrt ( fljxf0zdcw [ 1 ] ) ; } if (
fljxf0zdcw [ 1 ] < 0.0 ) { rtDW . flcrf2pa4r = 1 ; } if ( bzw3crffge < 0.0 )
{ rtB . b3h2v3br2u [ 2 ] = - muDoubleScalarSqrt ( muDoubleScalarAbs (
bzw3crffge ) ) ; rtDW . flcrf2pa4r = 1 ; } else { rtB . b3h2v3br2u [ 2 ] =
muDoubleScalarSqrt ( bzw3crffge ) ; } } gllp1vimnh ( rtS , rtB . atdnrefilq ,
rtB . etkri0io2o , & rtB . glvlabfrtp , & rtDW . glvlabfrtp , & rtP .
glvlabfrtp , & rtX . glvlabfrtp , & ( ( XDis * ) ssGetContStateDisabled ( rtS
) ) -> glvlabfrtp ) ; kyedbo0mxu ( rtS , rtB . nij3ckqdiv , rtB . etkri0io2o
, & rtB . drtk0acyhp , & rtDW . drtk0acyhp , & rtP . drtk0acyhp , & rtX .
drtk0acyhp , & ( ( XDis * ) ssGetContStateDisabled ( rtS ) ) -> drtk0acyhp )
; if ( rtB . atdnrefilq ) { fljxf0zdcw [ 0 ] = rtB . glvlabfrtp . gx0tigur0o
[ 0 ] ; fljxf0zdcw [ 1 ] = rtB . glvlabfrtp . gx0tigur0o [ 1 ] ; fljxf0zdcw [
2 ] = rtB . glvlabfrtp . gx0tigur0o [ 2 ] ; } else { fljxf0zdcw [ 0 ] = rtB .
drtk0acyhp . me4ylfixyn [ 0 ] ; fljxf0zdcw [ 1 ] = rtB . drtk0acyhp .
me4ylfixyn [ 1 ] ; fljxf0zdcw [ 2 ] = rtB . drtk0acyhp . me4ylfixyn [ 2 ] ; }
nx5egkjgi3 = fljxf0zdcw [ 0 ] * fljxf0zdcw [ 0 ] ; rtB . krzzt4ow11 [ 0 ] =
rtX . nv45bjhe3t [ 0 ] ; jicqzbt3a1_idx_0 = fljxf0zdcw [ 1 ] * fljxf0zdcw [ 1
] ; rtB . krzzt4ow11 [ 1 ] = rtX . nv45bjhe3t [ 1 ] ; P_p = fljxf0zdcw [ 2 ]
* fljxf0zdcw [ 2 ] ; rtB . krzzt4ow11 [ 2 ] = rtX . nv45bjhe3t [ 2 ] ; {
real_T * * uBuffer = ( real_T * * ) & rtDW . chf5u4x2og . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & rtDW . chf5u4x2og . TUbufferPtrs [ 3
] ; real_T simTime = ssGetT ( rtS ) ; real_T tMinusDelay ; { int_T i1 ;
real_T * y0 = & kgv0hwwrkz [ 0 ] ; int_T * iw_Tail = & rtDW . nw5suzqpq2 .
Tail [ 0 ] ; int_T * iw_Head = & rtDW . nw5suzqpq2 . Head [ 0 ] ; int_T *
iw_Last = & rtDW . nw5suzqpq2 . Last [ 0 ] ; int_T * iw_CircularBufSize = &
rtDW . nw5suzqpq2 . CircularBufSize [ 0 ] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) {
tMinusDelay = ( ( rtP . TransportDelay_Delay_jesai05nr3 > 0.0 ) ? rtP .
TransportDelay_Delay_jesai05nr3 : 0.0 ) ; tMinusDelay = simTime - tMinusDelay
; y0 [ i1 ] = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , iw_CircularBufSize [ i1 ] , & iw_Last [ i1 ] , iw_Tail [ i1 ] ,
iw_Head [ i1 ] , rtP . TransportDelay_InitOutput_pykn05w1hn , 0 , ( boolean_T
) ( ssIsMinorTimeStep ( rtS ) && ( ssGetTimeOfLastOutput ( rtS ) == ssGetT (
rtS ) ) ) ) ; tBuffer ++ ; uBuffer ++ ; } } } if ( ssIsSampleHit ( rtS , 1 ,
0 ) ) { rtB . bj5d1sf254 = rtP . K1_Value_n5okklsegz ; rtB . dw3pchqeno [ 0 ]
= rtDW . mvp3sao44j [ 0 ] ; rtB . dw3pchqeno [ 1 ] = rtDW . mvp3sao44j [ 1 ]
; rtB . dw3pchqeno [ 2 ] = rtDW . mvp3sao44j [ 2 ] ; } if ( ssGetT ( rtS ) >=
rtB . bj5d1sf254 ) { rtB . j4eo4uftcs [ 0 ] = ( rtB . krzzt4ow11 [ 0 ] -
kgv0hwwrkz [ 0 ] ) * rtP . Gain_Gain_a1f4gpqc0r ; rtB . j4eo4uftcs [ 1 ] = (
rtB . krzzt4ow11 [ 1 ] - kgv0hwwrkz [ 1 ] ) * rtP . Gain_Gain_a1f4gpqc0r ;
rtB . j4eo4uftcs [ 2 ] = ( rtB . krzzt4ow11 [ 2 ] - kgv0hwwrkz [ 2 ] ) * rtP
. Gain_Gain_a1f4gpqc0r ; } else { rtB . j4eo4uftcs [ 0 ] = rtB . dw3pchqeno [
0 ] ; rtB . j4eo4uftcs [ 1 ] = rtB . dw3pchqeno [ 1 ] ; rtB . j4eo4uftcs [ 2
] = rtB . dw3pchqeno [ 2 ] ; } lfrwzvun5e_idx_0 = rtB . j4eo4uftcs [ 0 ] *
rtB . j4eo4uftcs [ 0 ] ; dP = rtB . j4eo4uftcs [ 1 ] * rtB . j4eo4uftcs [ 1 ]
; jicqzbt3a1_idx_1 = rtB . j4eo4uftcs [ 2 ] * rtB . j4eo4uftcs [ 2 ] ;
gllp1vimnh ( rtS , rtB . p5irdmbgnc , rtB . etkri0io2o , & rtB . daw0ymwmw4 ,
& rtDW . daw0ymwmw4 , & rtP . daw0ymwmw4 , & rtX . daw0ymwmw4 , & ( ( XDis *
) ssGetContStateDisabled ( rtS ) ) -> daw0ymwmw4 ) ; kyedbo0mxu ( rtS , rtB .
cje2imeoha , rtB . etkri0io2o , & rtB . pu1c2mp3al , & rtDW . pu1c2mp3al , &
rtP . pu1c2mp3al , & rtX . pu1c2mp3al , & ( ( XDis * ) ssGetContStateDisabled
( rtS ) ) -> pu1c2mp3al ) ; if ( rtB . p5irdmbgnc ) { fljxf0zdcw [ 0 ] = rtB
. daw0ymwmw4 . gx0tigur0o [ 0 ] ; fljxf0zdcw [ 1 ] = rtB . daw0ymwmw4 .
gx0tigur0o [ 1 ] ; fljxf0zdcw [ 2 ] = rtB . daw0ymwmw4 . gx0tigur0o [ 2 ] ; }
else { fljxf0zdcw [ 0 ] = rtB . pu1c2mp3al . me4ylfixyn [ 0 ] ; fljxf0zdcw [
1 ] = rtB . pu1c2mp3al . me4ylfixyn [ 1 ] ; fljxf0zdcw [ 2 ] = rtB .
pu1c2mp3al . me4ylfixyn [ 2 ] ; } bzw3crffge = fljxf0zdcw [ 0 ] * fljxf0zdcw
[ 0 ] ; nx5egkjgi3 = ( nx5egkjgi3 - lfrwzvun5e_idx_0 ) - bzw3crffge ; if (
bzw3crffge > rtP . Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ) {
bzw3crffge = rtP . Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ; } else
{ if ( bzw3crffge < rtP . Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue )
{ bzw3crffge = rtP . Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue ; } }
bzw3crffge = nx5egkjgi3 / bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ) { fljxf0zdcw [ 0 ] = rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ; } else if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ) { fljxf0zdcw [ 0 ] =
rtP . Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ; } else { fljxf0zdcw
[ 0 ] = bzw3crffge ; } bzw3crffge = fljxf0zdcw [ 1 ] * fljxf0zdcw [ 1 ] ;
nx5egkjgi3 = ( jicqzbt3a1_idx_0 - dP ) - bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ; } else { if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue ; } } bzw3crffge =
nx5egkjgi3 / bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ) { fljxf0zdcw [ 1 ] = rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ; } else if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ) { fljxf0zdcw [ 1 ] =
rtP . Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ; } else { fljxf0zdcw
[ 1 ] = bzw3crffge ; } bzw3crffge = fljxf0zdcw [ 2 ] * fljxf0zdcw [ 2 ] ;
nx5egkjgi3 = ( P_p - jicqzbt3a1_idx_1 ) - bzw3crffge ; if ( bzw3crffge > rtP
. Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ; } else { if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue ; } } bzw3crffge =
nx5egkjgi3 / bzw3crffge ; if ( bzw3crffge > rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ; } else { if ( bzw3crffge <
rtP . Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ) { bzw3crffge = rtP .
Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ; } } if ( ssIsMajorTimeStep
( rtS ) ) { if ( rtDW . mv3mqzgj3o != 0 ) {
ssSetBlockStateForSolverChangedAtMajorStep ( rtS ) ; rtDW . mv3mqzgj3o = 0 ;
} rtB . oryb5w3imi [ 0 ] = muDoubleScalarSqrt ( fljxf0zdcw [ 0 ] ) ; rtB .
oryb5w3imi [ 1 ] = muDoubleScalarSqrt ( fljxf0zdcw [ 1 ] ) ; rtB . oryb5w3imi
[ 2 ] = muDoubleScalarSqrt ( bzw3crffge ) ; } else { if ( fljxf0zdcw [ 0 ] <
0.0 ) { rtB . oryb5w3imi [ 0 ] = - muDoubleScalarSqrt ( muDoubleScalarAbs (
fljxf0zdcw [ 0 ] ) ) ; } else { rtB . oryb5w3imi [ 0 ] = muDoubleScalarSqrt (
fljxf0zdcw [ 0 ] ) ; } if ( fljxf0zdcw [ 0 ] < 0.0 ) { rtDW . mv3mqzgj3o = 1
; } if ( fljxf0zdcw [ 1 ] < 0.0 ) { rtB . oryb5w3imi [ 1 ] = -
muDoubleScalarSqrt ( muDoubleScalarAbs ( fljxf0zdcw [ 1 ] ) ) ; } else { rtB
. oryb5w3imi [ 1 ] = muDoubleScalarSqrt ( fljxf0zdcw [ 1 ] ) ; } if (
fljxf0zdcw [ 1 ] < 0.0 ) { rtDW . mv3mqzgj3o = 1 ; } if ( bzw3crffge < 0.0 )
{ rtB . oryb5w3imi [ 2 ] = - muDoubleScalarSqrt ( muDoubleScalarAbs (
bzw3crffge ) ) ; rtDW . mv3mqzgj3o = 1 ; } else { rtB . oryb5w3imi [ 2 ] =
muDoubleScalarSqrt ( bzw3crffge ) ; } } if ( ssIsSampleHit ( rtS , 2 , 0 ) )
{ for ( i = 0 ; i < 3 ; i ++ ) { rtB . gaih2ptcqs [ i ] = rtP .
Gain7_Gain_i0s03tspqs [ i ] * dmxfhmawdj [ i ] ; rtB . gctmfhb3tj [ i ] = rtP
. Gain8_Gain [ i ] * bfhciemo10 [ i ] ; rtB . hs4xhfhuu4 [ i ] = 0.0 ; rtB .
bkfh2vcfmq [ i ] = 0.0 ; rtB . esekxgdj1v [ i ] = rtP . Gain7_Gain_g2vyun1xyv
[ i ] * cfs3g00azs [ i ] ; rtB . gn420ui3bc [ i ] = rtP .
Gain8_Gain_gygrek3mcm [ i ] * ejdkw0tzmz [ i ] ; rtB . a3oqmvvhoe [ i ] = 0.0
; rtB . bud3z3gi3t [ i ] = 0.0 ; rtB . hs4xhfhuu4 [ i ] += rtP .
phase2modeR_Gain [ i ] * rtB . hliocuqujf [ 21 ] ; rtB . bkfh2vcfmq [ i ] +=
rtP . phase2modeS_Gain [ i ] * rtB . hliocuqujf [ 18 ] ; rtB . a3oqmvvhoe [ i
] += rtP . phase2modeR_Gain_ptaizj5vff [ i ] * rtB . hliocuqujf [ 27 ] ; rtB
. bud3z3gi3t [ i ] += rtP . phase2modeS_Gain_mhoa3los1g [ i ] * rtB .
hliocuqujf [ 24 ] ; rtB . hs4xhfhuu4 [ i ] += rtP . phase2modeR_Gain [ i + 3
] * rtB . hliocuqujf [ 22 ] ; rtB . bkfh2vcfmq [ i ] += rtP .
phase2modeS_Gain [ i + 3 ] * rtB . hliocuqujf [ 19 ] ; rtB . a3oqmvvhoe [ i ]
+= rtP . phase2modeR_Gain_ptaizj5vff [ i + 3 ] * rtB . hliocuqujf [ 28 ] ;
rtB . bud3z3gi3t [ i ] += rtP . phase2modeS_Gain_mhoa3los1g [ i + 3 ] * rtB .
hliocuqujf [ 25 ] ; rtB . hs4xhfhuu4 [ i ] += rtP . phase2modeR_Gain [ i + 6
] * rtB . hliocuqujf [ 23 ] ; rtB . bkfh2vcfmq [ i ] += rtP .
phase2modeS_Gain [ i + 6 ] * rtB . hliocuqujf [ 20 ] ; rtB . a3oqmvvhoe [ i ]
+= rtP . phase2modeR_Gain_ptaizj5vff [ i + 6 ] * rtB . hliocuqujf [ 29 ] ;
rtB . bud3z3gi3t [ i ] += rtP . phase2modeS_Gain_mhoa3los1g [ i + 6 ] * rtB .
hliocuqujf [ 26 ] ; } if ( ssGetLogOutput ( rtS ) ) { StructLogVar * svar = (
StructLogVar * ) rtDW . iek4salud1 . LoggedData ; LogVar * var = svar ->
signals . values ; { double locTime = ssGetTaskTime ( rtS , 2 ) ; ;
rt_UpdateLogVar ( ( LogVar * ) svar -> time , & locTime , 0 ) ; } { real_T
up0 [ 1 ] ; up0 [ 0 ] = rtB . oo5we1pw3n ; rt_UpdateLogVar ( ( LogVar * ) var
, up0 , 0 ) ; } } } if ( ssIsSampleHit ( rtS , 3 , 0 ) ) { } if (
ssIsSampleHit ( rtS , 2 , 0 ) ) { } UNUSED_PARAMETER ( tid ) ; } void
MdlOutputsTID4 ( int_T tid ) { real_T hoovnnwxne ; real_T f0ppcbe0bk ; rtB .
fldwassvbh = rtP . Lff_Gain_m1pxhw4cyl * rtP . Iq_ref_Value ; rtB .
dr1q3vvfez = rtP . Rff_Gain_mplfss1cnd * rtP . Iq_ref_Value ; rtB .
e01ed5cv3s = ( uint8_T ) ( rtP . AlphaBetaZerotodq0_Alignment == rtP .
CompareToConstant_const ) ; rtB . oslgokdckt = ( uint8_T ) ( rtP .
AlphaBetaZerotodq0_Alignment == rtP . CompareToConstant1_const ) ; if (
ssIsMajorTimeStep ( rtS ) ) { srUpdateBC ( rtDW . ntzroniten ) ; } rtB .
jc4x0snbqf = ( uint8_T ) ( rtP . AlphaBetaZerotodq0_Alignment_mnakfv4gd4 ==
rtP . CompareToConstant_const_mjeo4kqpve ) ; rtB . fs14jhs4t5 = ( uint8_T ) (
rtP . AlphaBetaZerotodq0_Alignment_mnakfv4gd4 == rtP .
CompareToConstant1_const_lkgpibc3kf ) ; rtB . pkbwlncsj5 = ( uint8_T ) ( rtP
. AlphaBetaZerotodq0_Alignment_otrg0yyt1a == rtP .
CompareToConstant_const_f3j21z25kv ) ; rtB . hdzltqw2sw = ( uint8_T ) ( rtP .
AlphaBetaZerotodq0_Alignment_otrg0yyt1a == rtP .
CompareToConstant1_const_m1hj2kgrib ) ; rtB . ndqy5pufgt = ( uint8_T ) ( rtP
. AlphaBetaZerotodq0_Alignment_awofvyee55 == rtP .
CompareToConstant_const_ork22hwfpx ) ; rtB . isub4roeyp = ( uint8_T ) ( rtP .
AlphaBetaZerotodq0_Alignment_awofvyee55 == rtP .
CompareToConstant1_const_mofh5awlq5 ) ; f0ppcbe0bk = muDoubleScalarAsin (
muDoubleScalarSin ( rtP . Gain1_Gain_iza01f01um * 0.0 ) ) * rtP . Gain2_Gain
; rtB . jasinaajz3 = ( rtP . Constant2_Value_iqex2uo240 == rtP .
Constant6_Value ) ; rtB . mjlsry1omh = ! rtB . jasinaajz3 ; rtB . pgwb1j4u3n
= f0ppcbe0bk + rtP . Constant1_Value_a4nlxyznsj ; rtB . b3gkmlvhtj =
f0ppcbe0bk - rtP . Constant1_Value_a4nlxyznsj ; rtB . kxfvw5uauv = rtP .
Gain1_Gain_bfibyfcm43 * rtP . Vnom_dc / ( rtP . Vnom_sec * 1.4142135623730951
/ 1.7320508075688772 ) ; rtB . acbfsgne1t = rtP . TempC ; if ( rtB .
acbfsgne1t > rtP . Saturation_UpperSat_gtqfq0ppkv ) { f0ppcbe0bk = rtP .
Saturation_UpperSat_gtqfq0ppkv ; } else if ( rtB . acbfsgne1t < rtP .
Saturation_LowerSat_e1cd25lz4w ) { f0ppcbe0bk = rtP .
Saturation_LowerSat_e1cd25lz4w ; } else { f0ppcbe0bk = rtB . acbfsgne1t ; }
f0ppcbe0bk += rtP . u_K_Value ; rtB . atazzkiqtz = ( ( f0ppcbe0bk - rtP .
Tref_K_Value ) * rtP . alpha_Isc_Gain + rtP . IL_module_Value ) * rtP .
PVArray_Npar ; hoovnnwxne = f0ppcbe0bk / rtP . Tref_K2_Value ; rtB .
mqwfucjhw5 = muDoubleScalarExp ( rtP . EgRef_Value - ( ( f0ppcbe0bk - rtP .
Tref_K1_Value ) * rtP . dEgdT_Gain + rtP . one1_Value ) * rtP . EgRef_Gain /
( rtP . k1_Gain * f0ppcbe0bk ) ) * ( rtP . I0_array_Gain *
muDoubleScalarPower ( hoovnnwxne , 3.0 ) ) ; rtB . kqp2cunbno = rtP .
VT_ref_array_Gain * hoovnnwxne ; rtB . ocp2ppapt1 = ( rtP . RMS_TrueRMS !=
0.0 ) ; rtB . nkxqxwgteh = ! rtB . ocp2ppapt1 ; rtB . fk5vinpzuf = ( rtP .
RMS1_TrueRMS != 0.0 ) ; rtB . emd0mf5yqm = ! rtB . fk5vinpzuf ; rtB .
n3g3acn3kz = ( rtP . RMS1_TrueRMS_kcqxfunc0w != 0.0 ) ; rtB . oamr1wipoz = !
rtB . n3g3acn3kz ; rtB . c5pw4pqcew = ( rtP . RMS2_TrueRMS != 0.0 ) ; rtB .
odjvodg0jz = ! rtB . c5pw4pqcew ; rtB . atdnrefilq = ( rtP .
RMS1_TrueRMS_jywwajkz1m != 0.0 ) ; rtB . nij3ckqdiv = ! rtB . atdnrefilq ;
rtB . p5irdmbgnc = ( rtP . RMS2_TrueRMS_beyrfxdip3 != 0.0 ) ; rtB .
cje2imeoha = ! rtB . p5irdmbgnc ; UNUSED_PARAMETER ( tid ) ; } void MdlUpdate
( int_T tid ) { real_T HoldSine ; if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { {
SimStruct * rts = ssGetSFunction ( rtS , 2 ) ; sfcnUpdate ( rts , 0 ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 3 ) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus (
rts ) != ( NULL ) ) return ; } { SimStruct * rts = ssGetSFunction ( rtS , 4 )
; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return
; } { SimStruct * rts = ssGetSFunction ( rtS , 5 ) ; sfcnUpdate ( rts , 0 ) ;
if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 6 ) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus (
rts ) != ( NULL ) ) return ; } { SimStruct * rts = ssGetSFunction ( rtS , 7 )
; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return
; } { SimStruct * rts = ssGetSFunction ( rtS , 8 ) ; sfcnUpdate ( rts , 0 ) ;
if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } { SimStruct * rts =
ssGetSFunction ( rtS , 9 ) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus (
rts ) != ( NULL ) ) return ; } rtDW . jreglfqlm5 = rtB . jjtdt14g1y ;
HoldSine = rtDW . a1004yerwl ; rtDW . a1004yerwl = rtDW . a1004yerwl * rtP .
SineWaveA_HCos + rtDW . i5irbkj5h1 * rtP . SineWaveA_Hsin ; rtDW . i5irbkj5h1
= rtDW . i5irbkj5h1 * rtP . SineWaveA_HCos - HoldSine * rtP . SineWaveA_Hsin
; HoldSine = rtDW . dqdihrjufk ; rtDW . dqdihrjufk = rtDW . dqdihrjufk * rtP
. SineWaveB_HCos + rtDW . fnoqoswl0l * rtP . SineWaveB_Hsin ; rtDW .
fnoqoswl0l = rtDW . fnoqoswl0l * rtP . SineWaveB_HCos - HoldSine * rtP .
SineWaveB_Hsin ; HoldSine = rtDW . l31gaczqpr ; rtDW . l31gaczqpr = rtDW .
l31gaczqpr * rtP . SineWaveC_HCos + rtDW . eelkc34vvy * rtP . SineWaveC_Hsin
; rtDW . eelkc34vvy = rtDW . eelkc34vvy * rtP . SineWaveC_HCos - HoldSine *
rtP . SineWaveC_Hsin ; { const real_T * As = ( real_T * ) rtDW . o1xie1rqxt .
AS ; const real_T * Bs = ( real_T * ) rtDW . o1xie1rqxt . BS ; real_T * xtmp
= ( real_T * ) rtDW . o1xie1rqxt . XTMP ; real_T accum ; { int_T i1 ; real_T
* xd = & rtDW . hllwh2h1oo [ 0 ] ; for ( i1 = 0 ; i1 < 34 ; i1 ++ ) { accum =
0.0 ; { int_T i2 ; real_T * xd = & rtDW . hllwh2h1oo [ 0 ] ; for ( i2 = 0 ;
i2 < 34 ; i2 ++ ) { accum += * ( As ++ ) * xd [ i2 ] ; } } { int_T i2 ; const
real_T * u0 ; for ( i2 = 0 ; i2 < 18 ; i2 ++ ) { accum += * ( Bs ++ ) * 0.0 ;
} accum += * ( Bs ++ ) * rtB . oq3ipm0wus [ 0 ] ; accum += * ( Bs ++ ) * rtB
. oq3ipm0wus [ 1 ] ; accum += * ( Bs ++ ) * rtB . oq3ipm0wus [ 2 ] ; accum +=
* ( Bs ++ ) * rtB . ntgaxamgwu [ 0 ] ; accum += * ( Bs ++ ) * rtB .
ntgaxamgwu [ 1 ] ; accum += * ( Bs ++ ) * rtB . ntgaxamgwu [ 2 ] ; accum += *
( Bs ++ ) * rtB . hjgv2ztqc5 [ 0 ] ; accum += * ( Bs ++ ) * rtB . hjgv2ztqc5
[ 1 ] ; accum += * ( Bs ++ ) * rtB . hjgv2ztqc5 [ 2 ] ; accum += * ( Bs ++ )
* rtB . leghz2ze52 [ 0 ] ; accum += * ( Bs ++ ) * rtB . leghz2ze52 [ 1 ] ;
accum += * ( Bs ++ ) * rtB . leghz2ze52 [ 2 ] ; accum += * ( Bs ++ ) * rtB .
hoxbun55q5 ; accum += * ( Bs ++ ) * rtB . evntpqggjc ; accum += * ( Bs ++ ) *
rtB . nspdhd04fa ; accum += * ( Bs ++ ) * rtB . fegphm11cq ; accum += * ( Bs
++ ) * rtB . j4jxwn4tn4 ; } xtmp [ i1 ] = accum ; } } { int_T i1 ; real_T *
xd = & rtDW . hllwh2h1oo [ 0 ] ; for ( i1 = 0 ; i1 < 34 ; i1 ++ ) { xd [ i1 ]
= xtmp [ i1 ] ; } } { int_T * gState = ( int_T * ) rtDW . o1xie1rqxt .
G_STATE ; * ( gState ++ ) = ( int_T ) rtB . mi3zbxppas [ 0 ] ; * ( gState ++
) = ( int_T ) rtB . mi3zbxppas [ 1 ] ; * ( gState ++ ) = ( int_T ) rtB .
mi3zbxppas [ 2 ] ; * ( gState ++ ) = ( int_T ) rtB . mi3zbxppas [ 3 ] ; * (
gState ++ ) = ( int_T ) rtP .
zerotogeneratedummydiodegateanddelaysignals_Value ; * ( gState ++ ) = ( int_T
) rtP . zerotogeneratedummydiodegateanddelaysignals_Value ; * ( gState ++ ) =
( int_T ) rtB . ahenbhgxgu [ 0 ] ; * ( gState ++ ) = ( int_T ) rtB .
ahenbhgxgu [ 1 ] ; * ( gState ++ ) = ( int_T ) rtB . ahenbhgxgu [ 2 ] ; * (
gState ++ ) = ( int_T ) rtB . ahenbhgxgu [ 3 ] ; * ( gState ++ ) = ( int_T )
rtP . zerotogeneratedummydiodegateanddelaysignals_Value ; * ( gState ++ ) = (
int_T ) rtP . zerotogeneratedummydiodegateanddelaysignals_Value ; * ( gState
++ ) = ( int_T ) rtB . en03atcxhu [ 0 ] ; * ( gState ++ ) = ( int_T ) rtB .
en03atcxhu [ 1 ] ; * ( gState ++ ) = ( int_T ) rtB . en03atcxhu [ 2 ] ; * (
gState ++ ) = ( int_T ) rtB . en03atcxhu [ 3 ] ; * ( gState ++ ) = ( int_T )
rtP . zerotogeneratedummydiodegateanddelaysignals_Value ; * ( gState ++ ) = (
int_T ) rtP . zerotogeneratedummydiodegateanddelaysignals_Value ; } } } if (
ssIsSampleHit ( rtS , 3 , 0 ) ) { rtDW . m1peilszpy += rtP .
DiscreteTimeIntegrator_gainval * rtB . acbdmg3uuv ; rtDW . bvgqcazfkc = (
int8_T ) rtB . gjbagadbk5 ; rtDW . dnrqy23bwk = rtP . Integrator_gainval *
rtB . ihq1wxrrwv + rtB . ajjwqo0e1v ; rtDW . jzbmotzdka [ 0 ] += rtP .
Integrator_gainval_gmvlo1vh4m * rtB . el5lb42a5h [ 0 ] ; rtDW . jzbmotzdka [
1 ] += rtP . Integrator_gainval_gmvlo1vh4m * rtB . el5lb42a5h [ 1 ] ; rtDW .
jv0y1evb0u [ 0 ] = rtB . lpiitgbbyf [ 0 ] ; rtDW . jv0y1evb0u [ 1 ] = rtB .
lpiitgbbyf [ 1 ] ; rtDW . jv0y1evb0u [ 2 ] = rtB . lpiitgbbyf [ 2 ] ; rtDW .
giqv4pyyys = rtB . pu0odtmlhy ; if ( rtDW . iy4nvvktj2 && ssIsSampleHit ( rtS
, 3 , 0 ) ) { rtDW . lpm5mugxt5 = 0U ; rtDW . lzgy3px31z = rtP .
Integ4_gainval * rtB . eeusicyxwo [ 0 ] + rtB . bzadnafn2c ; { SimStruct *
rts = ssGetSFunction ( rtS , 0 ) ; sfcnUpdate ( rts , 0 ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtDW . omufifkqbz = rtB .
eeusicyxwo [ 0 ] ; rtDW . nndlftph12 = rtB . aggnsxdhid ; rtDW . nymp3uolcg =
0U ; rtDW . ohydhrpaay = rtP . Integ4_gainval_fz2uqdiylj * rtB . eeusicyxwo [
1 ] + rtB . as2m1qy4ku ; { SimStruct * rts = ssGetSFunction ( rtS , 1 ) ;
sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ;
} rtDW . eznk4sr03e = rtB . eeusicyxwo [ 1 ] ; rtDW . aktl42ci5l = rtB .
bsrowihmvj ; } rtDW . oneka2f33e = 0U ; rtDW . mwfdpj20rn = rtP .
Integ4_gainval_lotsoek0sk * rtB . azqgqqg4wd [ 1 ] + rtB . myfgavzlid ; {
SimStruct * rts = ssGetSFunction ( rtS , 10 ) ; sfcnUpdate ( rts , 0 ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtDW . m5fkweyhnz = rtB .
azqgqqg4wd [ 1 ] ; rtDW . oyi34irdce = rtB . jlybhz2t0i ; rtDW . fmpdeay2dq =
rtDW . koobssrco0 ; rtDW . evobiqhvg0 += rtP .
DiscreteTimeIntegrator_gainval_do3kgazujd * rtB . mhkfvkvx1u ; if ( rtDW .
evobiqhvg0 >= rtP . DiscreteTimeIntegrator_UpperSat ) { rtDW . evobiqhvg0 =
rtP . DiscreteTimeIntegrator_UpperSat ; } else { if ( rtDW . evobiqhvg0 <=
rtP . DiscreteTimeIntegrator_LowerSat ) { rtDW . evobiqhvg0 = rtP .
DiscreteTimeIntegrator_LowerSat ; } } rtDW . c3rg43fwng = rtB . jst3mmutj5 ;
rtDW . puez01wvqo = rtB . edk4wtugyj ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) )
{ HoldSine = rtDW . n3oafbzym3 [ 0 ] ; rtDW . n3oafbzym3 [ 0 ] = rtDW .
n3oafbzym3 [ 0 ] * rtP . SineWave_HCos [ 0 ] + rtDW . mec1csjpdt [ 0 ] * rtP
. SineWave_Hsin [ 0 ] ; rtDW . mec1csjpdt [ 0 ] = rtDW . mec1csjpdt [ 0 ] *
rtP . SineWave_HCos [ 0 ] - HoldSine * rtP . SineWave_Hsin [ 0 ] ; HoldSine =
rtDW . n3oafbzym3 [ 1 ] ; rtDW . n3oafbzym3 [ 1 ] = rtDW . n3oafbzym3 [ 1 ] *
rtP . SineWave_HCos [ 1 ] + rtDW . mec1csjpdt [ 1 ] * rtP . SineWave_Hsin [ 1
] ; rtDW . mec1csjpdt [ 1 ] = rtDW . mec1csjpdt [ 1 ] * rtP . SineWave_HCos [
1 ] - HoldSine * rtP . SineWave_Hsin [ 1 ] ; HoldSine = rtDW . n3oafbzym3 [ 2
] ; rtDW . n3oafbzym3 [ 2 ] = rtDW . n3oafbzym3 [ 2 ] * rtP . SineWave_HCos [
2 ] + rtDW . mec1csjpdt [ 2 ] * rtP . SineWave_Hsin [ 2 ] ; rtDW . mec1csjpdt
[ 2 ] = rtDW . mec1csjpdt [ 2 ] * rtP . SineWave_HCos [ 2 ] - HoldSine * rtP
. SineWave_Hsin [ 2 ] ; } if ( ssIsSampleHit ( rtS , 3 , 0 ) ) { rtDW .
fcwaufktro = 0U ; rtDW . otc3kwibnn = rtP . Integ4_gainval_gph3gdfsp4 * rtB .
bsgixfioey + rtB . krgk34ykj1 ; { SimStruct * rts = ssGetSFunction ( rtS , 11
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . inc2xczn1j = rtB . bsgixfioey ; rtDW . hglk0ykmnp = rtB .
k0men4hr1r ; rtDW . oaicnmy4cr = 0U ; rtDW . iyz3blnd0r = rtP .
Integ4_gainval_jbhrlhdx2q * rtB . ozgmksrvpc + rtB . esjg3pcq0t ; { SimStruct
* rts = ssGetSFunction ( rtS , 12 ) ; sfcnUpdate ( rts , 0 ) ; if (
ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtDW . okoa0ga0es = rtB .
ozgmksrvpc ; rtDW . mlqfus2yxb = rtB . b11aaunedj ; HoldSine = rtDW .
ev3w2qbtzq ; rtDW . ev3w2qbtzq = rtDW . ev3w2qbtzq * rtP . sinwt_HCos + rtDW
. lm3x1szvf2 * rtP . sinwt_Hsin ; rtDW . lm3x1szvf2 = rtDW . lm3x1szvf2 * rtP
. sinwt_HCos - HoldSine * rtP . sinwt_Hsin ; rtDW . d5srsnrycq = 0U ; rtDW .
cv44xoew41 = rtP . Integ4_gainval_i0v3kwd3pb * rtB . o0xbrdncoc + rtB .
bzovtsvol2 ; { SimStruct * rts = ssGetSFunction ( rtS , 13 ) ; sfcnUpdate (
rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) ) return ; } rtDW .
amx5pvlykq = rtB . o0xbrdncoc ; rtDW . l1nmdmtivq = rtB . ea14lgpbfy ;
HoldSine = rtDW . fnwtwy11ey ; rtDW . fnwtwy11ey = rtDW . fnwtwy11ey * rtP .
coswt_HCos + rtDW . hjvxevoa1u * rtP . coswt_Hsin ; rtDW . hjvxevoa1u = rtDW
. hjvxevoa1u * rtP . coswt_HCos - HoldSine * rtP . coswt_Hsin ; rtDW .
acdm1beais = 0U ; rtDW . c3vwiq1oz1 = rtP . Integ4_gainval_msuhv1zzxl * rtB .
fu3rpol0ph + rtB . lntmbixtgk ; { SimStruct * rts = ssGetSFunction ( rtS , 14
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . g55lsvsoyn = rtB . fu3rpol0ph ; rtDW . bf0dooycmi = rtB .
nvqp25eoum ; HoldSine = rtDW . bo3u2joydk ; rtDW . bo3u2joydk = rtDW .
bo3u2joydk * rtP . sinwt_HCos_cafhkcfovv + rtDW . d2b1q3eao5 * rtP .
sinwt_Hsin_e0rgtjbjpx ; rtDW . d2b1q3eao5 = rtDW . d2b1q3eao5 * rtP .
sinwt_HCos_cafhkcfovv - HoldSine * rtP . sinwt_Hsin_e0rgtjbjpx ; rtDW .
i4s40mrg1p = 0U ; rtDW . is34tmu21f = rtP . Integ4_gainval_gtzjg23pll * rtB .
hglrrwzja3 + rtB . f4wsdyxsap ; { SimStruct * rts = ssGetSFunction ( rtS , 15
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . bgndzrawrf = rtB . hglrrwzja3 ; rtDW . jwtcvtmaeo = rtB .
ccqwtokflg ; HoldSine = rtDW . lqckuso1xq ; rtDW . lqckuso1xq = rtDW .
lqckuso1xq * rtP . coswt_HCos_oz23s2cxn2 + rtDW . jrdmpel4ku * rtP .
coswt_Hsin_omgodzfzvz ; rtDW . jrdmpel4ku = rtDW . jrdmpel4ku * rtP .
coswt_HCos_oz23s2cxn2 - HoldSine * rtP . coswt_Hsin_omgodzfzvz ; rtDW .
e4uypnvxm4 = 0U ; rtDW . crf5cecvju = rtP . Integ4_gainval_g3uamgfpqb * rtB .
hxvph3vgfk + rtB . aeqligfyi1 ; { SimStruct * rts = ssGetSFunction ( rtS , 16
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . o54g2pguii = rtB . hxvph3vgfk ; rtDW . berzbzjptr = rtB .
klgyhsq3yy ; HoldSine = rtDW . pmdevl30t5 ; rtDW . pmdevl30t5 = rtDW .
pmdevl30t5 * rtP . sinwt_HCos_lozsdsini2 + rtDW . bfqdux1v4y * rtP .
sinwt_Hsin_ltv4wuiu5i ; rtDW . bfqdux1v4y = rtDW . bfqdux1v4y * rtP .
sinwt_HCos_lozsdsini2 - HoldSine * rtP . sinwt_Hsin_ltv4wuiu5i ; rtDW .
leakran3jy = 0U ; rtDW . eyj3xgyyy5 = rtP . Integ4_gainval_h0bygbtnao * rtB .
hiw0yqrayw + rtB . lqaoyuj05s ; { SimStruct * rts = ssGetSFunction ( rtS , 17
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . iwb0lp4dat = rtB . hiw0yqrayw ; rtDW . mmleedibcx = rtB .
fbyhptghi3 ; HoldSine = rtDW . phb5tgbsae ; rtDW . phb5tgbsae = rtDW .
phb5tgbsae * rtP . coswt_HCos_e1lcq0jydt + rtDW . cbutkiwxs3 * rtP .
coswt_Hsin_p0lqeycuo2 ; rtDW . cbutkiwxs3 = rtDW . cbutkiwxs3 * rtP .
coswt_HCos_e1lcq0jydt - HoldSine * rtP . coswt_Hsin_p0lqeycuo2 ; rtDW .
grw0tq5110 = 0U ; rtDW . iwtvu1xbzm = rtP . Integ4_gainval_lqadd2jned * rtB .
hxdguojmgq + rtB . eutj1tzkyf ; { SimStruct * rts = ssGetSFunction ( rtS , 18
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . fdgmxn5rpv = rtB . hxdguojmgq ; rtDW . mkbsymk41p = rtB .
atf0ppnipt ; HoldSine = rtDW . e4oo4zzsto ; rtDW . e4oo4zzsto = rtDW .
e4oo4zzsto * rtP . sinwt_HCos_ikm4ke3nt1 + rtDW . ni0wsgyutj * rtP .
sinwt_Hsin_f4ff1jm01o ; rtDW . ni0wsgyutj = rtDW . ni0wsgyutj * rtP .
sinwt_HCos_ikm4ke3nt1 - HoldSine * rtP . sinwt_Hsin_f4ff1jm01o ; rtDW .
ir0a11kmwf = 0U ; rtDW . h4kspo12ff = rtP . Integ4_gainval_gvvjdr1enu * rtB .
jf3zbnuh3n + rtB . f4bvvydmna ; { SimStruct * rts = ssGetSFunction ( rtS , 19
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . ad2sn3k3gj = rtB . jf3zbnuh3n ; rtDW . jy2blxntru = rtB .
m4cw5jelgb ; HoldSine = rtDW . gdfrdi2cqi ; rtDW . gdfrdi2cqi = rtDW .
gdfrdi2cqi * rtP . coswt_HCos_ih2fmjgzvo + rtDW . lanv5zwkwv * rtP .
coswt_Hsin_ki40gemxaa ; rtDW . lanv5zwkwv = rtDW . lanv5zwkwv * rtP .
coswt_HCos_ih2fmjgzvo - HoldSine * rtP . coswt_Hsin_ki40gemxaa ; rtDW .
bixjjcy32b = 0U ; rtDW . dbrynerjdo = rtP . Integ4_gainval_e3hymojevp * rtB .
e3r3gwb15i + rtB . b4ioytjw1o ; { SimStruct * rts = ssGetSFunction ( rtS , 20
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . ioxogrdks0 = rtB . e3r3gwb15i ; rtDW . ejhppynn0z = rtB .
g5q20xpjnp ; HoldSine = rtDW . his1pld4hz ; rtDW . his1pld4hz = rtDW .
his1pld4hz * rtP . sinwt_HCos_c3em0todhq + rtDW . arh00ntoml * rtP .
sinwt_Hsin_i0nd0ubqqg ; rtDW . arh00ntoml = rtDW . arh00ntoml * rtP .
sinwt_HCos_c3em0todhq - HoldSine * rtP . sinwt_Hsin_i0nd0ubqqg ; rtDW .
lob2ph3yhu = 0U ; rtDW . dqddocajhg = rtP . Integ4_gainval_o4m2wju04m * rtB .
b24n3f4xp0 + rtB . pddsnfsfnu ; { SimStruct * rts = ssGetSFunction ( rtS , 21
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . e2jfcom1xu = rtB . b24n3f4xp0 ; rtDW . km0mzzn3gj = rtB .
bjoxavkoai ; HoldSine = rtDW . birk1htcrc ; rtDW . birk1htcrc = rtDW .
birk1htcrc * rtP . coswt_HCos_cs3y4qtzeb + rtDW . pcj40bs3ff * rtP .
coswt_Hsin_fuzcqltv4d ; rtDW . pcj40bs3ff = rtDW . pcj40bs3ff * rtP .
coswt_HCos_cs3y4qtzeb - HoldSine * rtP . coswt_Hsin_fuzcqltv4d ; rtDW .
fuhanhc4z4 = 0U ; rtDW . llg10jep1x = rtP . Integ4_gainval_dmsyi0ujpc * rtB .
lk335slscy + rtB . cyaarq5wc5 ; { SimStruct * rts = ssGetSFunction ( rtS , 22
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . kkj5lgh0sd = rtB . lk335slscy ; rtDW . oersb5zkry = rtB .
g5azq2cdjs ; HoldSine = rtDW . l54ofk2ea3 ; rtDW . l54ofk2ea3 = rtDW .
l54ofk2ea3 * rtP . sinwt_HCos_lv5zpnsoyo + rtDW . bl13n2tmiw * rtP .
sinwt_Hsin_kfaxcimxsb ; rtDW . bl13n2tmiw = rtDW . bl13n2tmiw * rtP .
sinwt_HCos_lv5zpnsoyo - HoldSine * rtP . sinwt_Hsin_kfaxcimxsb ; rtDW .
phos00jdib = 0U ; rtDW . mxwbciposz = rtP . Integ4_gainval_lqpuvz5try * rtB .
fisyjtz0ao + rtB . mxfhajo1pf ; { SimStruct * rts = ssGetSFunction ( rtS , 23
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . mvp44fdh2o = rtB . fisyjtz0ao ; rtDW . fwzzbi3hli = rtB .
jiw5saclae ; HoldSine = rtDW . dpc4svbkri ; rtDW . dpc4svbkri = rtDW .
dpc4svbkri * rtP . coswt_HCos_dtrnnwyvoi + rtDW . nlvv2lvgnj * rtP .
coswt_Hsin_nt154hil4g ; rtDW . nlvv2lvgnj = rtDW . nlvv2lvgnj * rtP .
coswt_HCos_dtrnnwyvoi - HoldSine * rtP . coswt_Hsin_nt154hil4g ; rtDW .
ckxcx1vdsj = 0U ; rtDW . pjo1bnuqfk = rtP . Integ4_gainval_ae24pre0oj * rtB .
jdnb2gbavk + rtB . kmn2nxupno ; { SimStruct * rts = ssGetSFunction ( rtS , 24
) ; sfcnUpdate ( rts , 0 ) ; if ( ssGetErrorStatus ( rts ) != ( NULL ) )
return ; } rtDW . h45pdgkmki = rtB . jdnb2gbavk ; rtDW . ldxbwl52qd = rtB .
ltugjcg352 ; } gxsaitrvqe ( rtS , & rtB . kyedbo0mxuc , & rtDW . kyedbo0mxuc
, & rtP . kyedbo0mxuc ) ; pwyhxd4z2i ( rtS , & rtB . gllp1vimnhl , & rtDW .
gllp1vimnhl , & rtP . gllp1vimnhl ) ; gxsaitrvqe ( rtS , & rtB . oa2z5wykl2 ,
& rtDW . oa2z5wykl2 , & rtP . oa2z5wykl2 ) ; pwyhxd4z2i ( rtS , & rtB .
e2mpwvt2ua , & rtDW . e2mpwvt2ua , & rtP . e2mpwvt2ua ) ; pwyhxd4z2i ( rtS ,
& rtB . jhi4zkfadx , & rtDW . jhi4zkfadx , & rtP . jhi4zkfadx ) ; gxsaitrvqe
( rtS , & rtB . mdskmorhmi , & rtDW . mdskmorhmi , & rtP . mdskmorhmi ) ; {
real_T * * uBuffer = ( real_T * * ) & rtDW . lfqiakux1j . TUbufferPtrs [ 0 ]
; real_T * * tBuffer = ( real_T * * ) & rtDW . lfqiakux1j . TUbufferPtrs [ 3
] ; real_T simTime = ssGetT ( rtS ) ; rtDW . drhtugugfl . Head [ 0 ] = ( (
rtDW . drhtugugfl . Head [ 0 ] < ( rtDW . drhtugugfl . CircularBufSize [ 0 ]
- 1 ) ) ? ( rtDW . drhtugugfl . Head [ 0 ] + 1 ) : 0 ) ; if ( rtDW .
drhtugugfl . Head [ 0 ] == rtDW . drhtugugfl . Tail [ 0 ] ) { if ( !
rt_TDelayUpdateTailOrGrowBuf ( & rtDW . drhtugugfl . CircularBufSize [ 0 ] ,
& rtDW . drhtugugfl . Tail [ 0 ] , & rtDW . drhtugugfl . Head [ 0 ] , & rtDW
. drhtugugfl . Last [ 0 ] , simTime - rtP . TransportDelay_Delay , tBuffer ,
uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & rtDW . drhtugugfl .
MaxNewBufSize ) ) { ssSetErrorStatus ( rtS , "tdelay memory allocation error"
) ; return ; } } ( * tBuffer ++ ) [ rtDW . drhtugugfl . Head [ 0 ] ] =
simTime ; ( * uBuffer ++ ) [ rtDW . drhtugugfl . Head [ 0 ] ] = rtB .
ibnjsgargl [ 0 ] ; rtDW . drhtugugfl . Head [ 1 ] = ( ( rtDW . drhtugugfl .
Head [ 1 ] < ( rtDW . drhtugugfl . CircularBufSize [ 1 ] - 1 ) ) ? ( rtDW .
drhtugugfl . Head [ 1 ] + 1 ) : 0 ) ; if ( rtDW . drhtugugfl . Head [ 1 ] ==
rtDW . drhtugugfl . Tail [ 1 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( &
rtDW . drhtugugfl . CircularBufSize [ 1 ] , & rtDW . drhtugugfl . Tail [ 1 ]
, & rtDW . drhtugugfl . Head [ 1 ] , & rtDW . drhtugugfl . Last [ 1 ] ,
simTime - rtP . TransportDelay_Delay , tBuffer , uBuffer , ( NULL ) , (
boolean_T ) 0 , false , & rtDW . drhtugugfl . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } } (
* tBuffer ++ ) [ rtDW . drhtugugfl . Head [ 1 ] ] = simTime ; ( * uBuffer ++
) [ rtDW . drhtugugfl . Head [ 1 ] ] = rtB . ibnjsgargl [ 1 ] ; rtDW .
drhtugugfl . Head [ 2 ] = ( ( rtDW . drhtugugfl . Head [ 2 ] < ( rtDW .
drhtugugfl . CircularBufSize [ 2 ] - 1 ) ) ? ( rtDW . drhtugugfl . Head [ 2 ]
+ 1 ) : 0 ) ; if ( rtDW . drhtugugfl . Head [ 2 ] == rtDW . drhtugugfl . Tail
[ 2 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . drhtugugfl .
CircularBufSize [ 2 ] , & rtDW . drhtugugfl . Tail [ 2 ] , & rtDW .
drhtugugfl . Head [ 2 ] , & rtDW . drhtugugfl . Last [ 2 ] , simTime - rtP .
TransportDelay_Delay , tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false
, & rtDW . drhtugugfl . MaxNewBufSize ) ) { ssSetErrorStatus ( rtS ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ rtDW .
drhtugugfl . Head [ 2 ] ] = simTime ; ( * uBuffer ) [ rtDW . drhtugugfl .
Head [ 2 ] ] = rtB . ibnjsgargl [ 2 ] ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . dpquh1vse1 [ 0 ] = rtB . jeu3urzybv [ 0 ] ; rtDW . dpquh1vse1 [ 1
] = rtB . jeu3urzybv [ 1 ] ; rtDW . dpquh1vse1 [ 2 ] = rtB . jeu3urzybv [ 2 ]
; } pwyhxd4z2i ( rtS , & rtB . blrugcnivv , & rtDW . blrugcnivv , & rtP .
blrugcnivv ) ; gxsaitrvqe ( rtS , & rtB . iks0grgus1 , & rtDW . iks0grgus1 ,
& rtP . iks0grgus1 ) ; pwyhxd4z2i ( rtS , & rtB . glvlabfrtp , & rtDW .
glvlabfrtp , & rtP . glvlabfrtp ) ; gxsaitrvqe ( rtS , & rtB . drtk0acyhp , &
rtDW . drtk0acyhp , & rtP . drtk0acyhp ) ; { real_T * * uBuffer = ( real_T *
* ) & rtDW . chf5u4x2og . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & rtDW . chf5u4x2og . TUbufferPtrs [ 3 ] ; real_T simTime = ssGetT (
rtS ) ; rtDW . nw5suzqpq2 . Head [ 0 ] = ( ( rtDW . nw5suzqpq2 . Head [ 0 ] <
( rtDW . nw5suzqpq2 . CircularBufSize [ 0 ] - 1 ) ) ? ( rtDW . nw5suzqpq2 .
Head [ 0 ] + 1 ) : 0 ) ; if ( rtDW . nw5suzqpq2 . Head [ 0 ] == rtDW .
nw5suzqpq2 . Tail [ 0 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( & rtDW .
nw5suzqpq2 . CircularBufSize [ 0 ] , & rtDW . nw5suzqpq2 . Tail [ 0 ] , &
rtDW . nw5suzqpq2 . Head [ 0 ] , & rtDW . nw5suzqpq2 . Last [ 0 ] , simTime -
rtP . TransportDelay_Delay_jesai05nr3 , tBuffer , uBuffer , ( NULL ) , (
boolean_T ) 0 , false , & rtDW . nw5suzqpq2 . MaxNewBufSize ) ) {
ssSetErrorStatus ( rtS , "tdelay memory allocation error" ) ; return ; } } (
* tBuffer ++ ) [ rtDW . nw5suzqpq2 . Head [ 0 ] ] = simTime ; ( * uBuffer ++
) [ rtDW . nw5suzqpq2 . Head [ 0 ] ] = rtB . krzzt4ow11 [ 0 ] ; rtDW .
nw5suzqpq2 . Head [ 1 ] = ( ( rtDW . nw5suzqpq2 . Head [ 1 ] < ( rtDW .
nw5suzqpq2 . CircularBufSize [ 1 ] - 1 ) ) ? ( rtDW . nw5suzqpq2 . Head [ 1 ]
+ 1 ) : 0 ) ; if ( rtDW . nw5suzqpq2 . Head [ 1 ] == rtDW . nw5suzqpq2 . Tail
[ 1 ] ) { if ( ! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . nw5suzqpq2 .
CircularBufSize [ 1 ] , & rtDW . nw5suzqpq2 . Tail [ 1 ] , & rtDW .
nw5suzqpq2 . Head [ 1 ] , & rtDW . nw5suzqpq2 . Last [ 1 ] , simTime - rtP .
TransportDelay_Delay_jesai05nr3 , tBuffer , uBuffer , ( NULL ) , ( boolean_T
) 0 , false , & rtDW . nw5suzqpq2 . MaxNewBufSize ) ) { ssSetErrorStatus (
rtS , "tdelay memory allocation error" ) ; return ; } } ( * tBuffer ++ ) [
rtDW . nw5suzqpq2 . Head [ 1 ] ] = simTime ; ( * uBuffer ++ ) [ rtDW .
nw5suzqpq2 . Head [ 1 ] ] = rtB . krzzt4ow11 [ 1 ] ; rtDW . nw5suzqpq2 . Head
[ 2 ] = ( ( rtDW . nw5suzqpq2 . Head [ 2 ] < ( rtDW . nw5suzqpq2 .
CircularBufSize [ 2 ] - 1 ) ) ? ( rtDW . nw5suzqpq2 . Head [ 2 ] + 1 ) : 0 )
; if ( rtDW . nw5suzqpq2 . Head [ 2 ] == rtDW . nw5suzqpq2 . Tail [ 2 ] ) {
if ( ! rt_TDelayUpdateTailOrGrowBuf ( & rtDW . nw5suzqpq2 . CircularBufSize [
2 ] , & rtDW . nw5suzqpq2 . Tail [ 2 ] , & rtDW . nw5suzqpq2 . Head [ 2 ] , &
rtDW . nw5suzqpq2 . Last [ 2 ] , simTime - rtP .
TransportDelay_Delay_jesai05nr3 , tBuffer , uBuffer , ( NULL ) , ( boolean_T
) 0 , false , & rtDW . nw5suzqpq2 . MaxNewBufSize ) ) { ssSetErrorStatus (
rtS , "tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ rtDW
. nw5suzqpq2 . Head [ 2 ] ] = simTime ; ( * uBuffer ) [ rtDW . nw5suzqpq2 .
Head [ 2 ] ] = rtB . krzzt4ow11 [ 2 ] ; } if ( ssIsSampleHit ( rtS , 1 , 0 )
) { rtDW . mvp3sao44j [ 0 ] = rtB . j4eo4uftcs [ 0 ] ; rtDW . mvp3sao44j [ 1
] = rtB . j4eo4uftcs [ 1 ] ; rtDW . mvp3sao44j [ 2 ] = rtB . j4eo4uftcs [ 2 ]
; } pwyhxd4z2i ( rtS , & rtB . daw0ymwmw4 , & rtDW . daw0ymwmw4 , & rtP .
daw0ymwmw4 ) ; gxsaitrvqe ( rtS , & rtB . pu1c2mp3al , & rtDW . pu1c2mp3al ,
& rtP . pu1c2mp3al ) ; UNUSED_PARAMETER ( tid ) ; } void MdlUpdateTID4 (
int_T tid ) { UNUSED_PARAMETER ( tid ) ; } void MdlDerivatives ( void ) {
XDot * _rtXdot ; _rtXdot = ( ( XDot * ) ssGetdX ( rtS ) ) ; jj4trrgi3n ( &
rtB . kyedbo0mxuc , & rtDW . kyedbo0mxuc , & _rtXdot -> kyedbo0mxuc ) ;
fdfvudi3q4 ( & rtB . gllp1vimnhl , & rtDW . gllp1vimnhl , & _rtXdot ->
gllp1vimnhl ) ; jj4trrgi3n ( & rtB . oa2z5wykl2 , & rtDW . oa2z5wykl2 , &
_rtXdot -> oa2z5wykl2 ) ; fdfvudi3q4 ( & rtB . e2mpwvt2ua , & rtDW .
e2mpwvt2ua , & _rtXdot -> e2mpwvt2ua ) ; fdfvudi3q4 ( & rtB . jhi4zkfadx , &
rtDW . jhi4zkfadx , & _rtXdot -> jhi4zkfadx ) ; jj4trrgi3n ( & rtB .
mdskmorhmi , & rtDW . mdskmorhmi , & _rtXdot -> mdskmorhmi ) ; _rtXdot ->
f2y4dmvcyy [ 0 ] = rtB . kzukygsbbq [ 0 ] ; _rtXdot -> f2y4dmvcyy [ 1 ] = rtB
. kzukygsbbq [ 1 ] ; _rtXdot -> f2y4dmvcyy [ 2 ] = rtB . kzukygsbbq [ 2 ] ;
fdfvudi3q4 ( & rtB . blrugcnivv , & rtDW . blrugcnivv , & _rtXdot ->
blrugcnivv ) ; jj4trrgi3n ( & rtB . iks0grgus1 , & rtDW . iks0grgus1 , &
_rtXdot -> iks0grgus1 ) ; fdfvudi3q4 ( & rtB . glvlabfrtp , & rtDW .
glvlabfrtp , & _rtXdot -> glvlabfrtp ) ; jj4trrgi3n ( & rtB . drtk0acyhp , &
rtDW . drtk0acyhp , & _rtXdot -> drtk0acyhp ) ; _rtXdot -> nv45bjhe3t [ 0 ] =
rtB . etkri0io2o [ 0 ] ; _rtXdot -> nv45bjhe3t [ 1 ] = rtB . etkri0io2o [ 1 ]
; _rtXdot -> nv45bjhe3t [ 2 ] = rtB . etkri0io2o [ 2 ] ; fdfvudi3q4 ( & rtB .
daw0ymwmw4 , & rtDW . daw0ymwmw4 , & _rtXdot -> daw0ymwmw4 ) ; jj4trrgi3n ( &
rtB . pu1c2mp3al , & rtDW . pu1c2mp3al , & _rtXdot -> pu1c2mp3al ) ; } void
MdlProjection ( void ) { } void MdlZeroCrossings ( void ) { ZCV * _rtZCSV ;
_rtZCSV = ( ( ZCV * ) ssGetSolverZcSignalVector ( rtS ) ) ; kqyojj2hz0 ( &
rtB . gllp1vimnhl , & rtDW . gllp1vimnhl , & rtP . gllp1vimnhl , & _rtZCSV ->
gllp1vimnhl ) ; kqyojj2hz0 ( & rtB . e2mpwvt2ua , & rtDW . e2mpwvt2ua , & rtP
. e2mpwvt2ua , & _rtZCSV -> e2mpwvt2ua ) ; kqyojj2hz0 ( & rtB . jhi4zkfadx ,
& rtDW . jhi4zkfadx , & rtP . jhi4zkfadx , & _rtZCSV -> jhi4zkfadx ) ;
kqyojj2hz0 ( & rtB . blrugcnivv , & rtDW . blrugcnivv , & rtP . blrugcnivv ,
& _rtZCSV -> blrugcnivv ) ; kqyojj2hz0 ( & rtB . glvlabfrtp , & rtDW .
glvlabfrtp , & rtP . glvlabfrtp , & _rtZCSV -> glvlabfrtp ) ; kqyojj2hz0 ( &
rtB . daw0ymwmw4 , & rtDW . daw0ymwmw4 , & rtP . daw0ymwmw4 , & _rtZCSV ->
daw0ymwmw4 ) ; } void MdlTerminate ( void ) { { SimStruct * rts =
ssGetSFunction ( rtS , 0 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 1 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 2 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 3 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 4 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 5 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 6 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 7 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 8 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 9 ) ; sfcnTerminate ( rts ) ; } { free ( rtDW .
o1xie1rqxt . AS ) ; free ( rtDW . o1xie1rqxt . BS ) ; free ( rtDW .
o1xie1rqxt . CS ) ; free ( rtDW . o1xie1rqxt . DS ) ; free ( rtDW .
o1xie1rqxt . DX_COL ) ; free ( rtDW . o1xie1rqxt . TMP2 ) ; free ( rtDW .
o1xie1rqxt . BD_COL ) ; free ( rtDW . o1xie1rqxt . TMP1 ) ; free ( rtDW .
o1xie1rqxt . XTMP ) ; free ( rtDW . o1xie1rqxt . G_STATE ) ; free ( rtDW .
o1xie1rqxt . SWITCH_STATUS ) ; free ( rtDW . o1xie1rqxt . SW_CHG ) ; free (
rtDW . o1xie1rqxt . SWITCH_STATUS_INIT ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 10 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 11 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 12 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 13 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 14 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 15 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 16 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 17 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 18 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 19 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 20 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 21 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 22 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 23 ) ; sfcnTerminate ( rts ) ; } { SimStruct * rts =
ssGetSFunction ( rtS , 24 ) ; sfcnTerminate ( rts ) ; } g5flx2hgit ( & rtDW .
kyedbo0mxuc ) ; b0l2el5i05 ( & rtDW . gllp1vimnhl ) ; g5flx2hgit ( & rtDW .
oa2z5wykl2 ) ; b0l2el5i05 ( & rtDW . e2mpwvt2ua ) ; b0l2el5i05 ( & rtDW .
jhi4zkfadx ) ; g5flx2hgit ( & rtDW . mdskmorhmi ) ; { int_T i1 ; void * *
pw_TUbufferPtrs = & rtDW . lfqiakux1j . TUbufferPtrs [ 0 ] ; for ( i1 = 0 ;
i1 < 3 ; i1 ++ ) { rt_TDelayFreeBuf ( pw_TUbufferPtrs [ i1 ] ) ; } }
b0l2el5i05 ( & rtDW . blrugcnivv ) ; g5flx2hgit ( & rtDW . iks0grgus1 ) ;
b0l2el5i05 ( & rtDW . glvlabfrtp ) ; g5flx2hgit ( & rtDW . drtk0acyhp ) ; {
int_T i1 ; void * * pw_TUbufferPtrs = & rtDW . chf5u4x2og . TUbufferPtrs [ 0
] ; for ( i1 = 0 ; i1 < 3 ; i1 ++ ) { rt_TDelayFreeBuf ( pw_TUbufferPtrs [ i1
] ) ; } } b0l2el5i05 ( & rtDW . daw0ymwmw4 ) ; g5flx2hgit ( & rtDW .
pu1c2mp3al ) ; if ( rt_slioCatalogue ( ) != ( NULL ) ) { void * *
slioCatalogueAddr = rt_slioCatalogueAddr ( ) ; rtwSaveDatasetsToMatFile (
rtwGetPointerFromUniquePtr ( rt_slioCatalogue ( ) ) ,
rt_GetMatSigstreamLoggingFileName ( ) ) ; rtwTerminateSlioCatalogue (
slioCatalogueAddr ) ; * slioCatalogueAddr = NULL ; } } void
MdlInitializeSizes ( void ) { ssSetNumContStates ( rtS , 60 ) ;
ssSetNumPeriodicContStates ( rtS , 0 ) ; ssSetNumY ( rtS , 0 ) ; ssSetNumU (
rtS , 0 ) ; ssSetDirectFeedThrough ( rtS , 0 ) ; ssSetNumSampleTimes ( rtS ,
4 ) ; ssSetNumBlocks ( rtS , 990 ) ; ssSetNumBlockIO ( rtS , 397 ) ;
ssSetNumBlockParams ( rtS , 6965 ) ; } void MdlInitializeSampleTimes ( void )
{ ssSetSampleTime ( rtS , 0 , 0.0 ) ; ssSetSampleTime ( rtS , 1 , 0.0 ) ;
ssSetSampleTime ( rtS , 2 , 5.0505050505050507E-6 ) ; ssSetSampleTime ( rtS ,
3 , 5.0505050505050505E-5 ) ; ssSetOffsetTime ( rtS , 0 , 0.0 ) ;
ssSetOffsetTime ( rtS , 1 , 1.0 ) ; ssSetOffsetTime ( rtS , 2 , 0.0 ) ;
ssSetOffsetTime ( rtS , 3 , 0.0 ) ; } void raccel_set_checksum ( ) {
ssSetChecksumVal ( rtS , 0 , 3932948024U ) ; ssSetChecksumVal ( rtS , 1 ,
1333879620U ) ; ssSetChecksumVal ( rtS , 2 , 2973354861U ) ; ssSetChecksumVal
( rtS , 3 , 1201730294U ) ; }
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif
SimStruct * raccel_register_model ( void ) { static struct _ssMdlInfo mdlInfo
; ( void ) memset ( ( char * ) rtS , 0 , sizeof ( SimStruct ) ) ; ( void )
memset ( ( char * ) & mdlInfo , 0 , sizeof ( struct _ssMdlInfo ) ) ;
ssSetMdlInfoPtr ( rtS , & mdlInfo ) ; { static time_T mdlPeriod [
NSAMPLE_TIMES ] ; static time_T mdlOffset [ NSAMPLE_TIMES ] ; static time_T
mdlTaskTimes [ NSAMPLE_TIMES ] ; static int_T mdlTsMap [ NSAMPLE_TIMES ] ;
static int_T mdlSampleHits [ NSAMPLE_TIMES ] ; static boolean_T
mdlTNextWasAdjustedPtr [ NSAMPLE_TIMES ] ; static int_T mdlPerTaskSampleHits
[ NSAMPLE_TIMES * NSAMPLE_TIMES ] ; static time_T mdlTimeOfNextSampleHit [
NSAMPLE_TIMES ] ; { int_T i ; for ( i = 0 ; i < NSAMPLE_TIMES ; i ++ ) {
mdlPeriod [ i ] = 0.0 ; mdlOffset [ i ] = 0.0 ; mdlTaskTimes [ i ] = 0.0 ;
mdlTsMap [ i ] = i ; mdlSampleHits [ i ] = 1 ; } } ssSetSampleTimePtr ( rtS ,
& mdlPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rtS , & mdlOffset [ 0 ] ) ;
ssSetSampleTimeTaskIDPtr ( rtS , & mdlTsMap [ 0 ] ) ; ssSetTPtr ( rtS , &
mdlTaskTimes [ 0 ] ) ; ssSetSampleHitPtr ( rtS , & mdlSampleHits [ 0 ] ) ;
ssSetTNextWasAdjustedPtr ( rtS , & mdlTNextWasAdjustedPtr [ 0 ] ) ;
ssSetPerTaskSampleHitsPtr ( rtS , & mdlPerTaskSampleHits [ 0 ] ) ;
ssSetTimeOfNextSampleHitPtr ( rtS , & mdlTimeOfNextSampleHit [ 0 ] ) ; }
ssSetSolverMode ( rtS , SOLVER_MODE_SINGLETASKING ) ; { ssSetBlockIO ( rtS ,
( ( void * ) & rtB ) ) ; ( void ) memset ( ( ( void * ) & rtB ) , 0 , sizeof
( B ) ) ; } { real_T * x = ( real_T * ) & rtX ; ssSetContStates ( rtS , x ) ;
( void ) memset ( ( void * ) x , 0 , sizeof ( X ) ) ; } { void * dwork = (
void * ) & rtDW ; ssSetRootDWork ( rtS , dwork ) ; ( void ) memset ( dwork ,
0 , sizeof ( DW ) ) ; } { static DataTypeTransInfo dtInfo ; ( void ) memset (
( char_T * ) & dtInfo , 0 , sizeof ( dtInfo ) ) ; ssSetModelMappingInfo ( rtS
, & dtInfo ) ; dtInfo . numDataTypes = 27 ; dtInfo . dataTypeSizes = &
rtDataTypeSizes [ 0 ] ; dtInfo . dataTypeNames = & rtDataTypeNames [ 0 ] ;
dtInfo . BTransTable = & rtBTransTable ; dtInfo . PTransTable = &
rtPTransTable ; dtInfo . dataTypeInfoTable = rtDataTypeInfoTable ; }
PV_Array_Model_InitializeDataMapInfo ( ) ; ssSetIsRapidAcceleratorActive (
rtS , true ) ; ssSetRootSS ( rtS , rtS ) ; ssSetVersion ( rtS ,
SIMSTRUCT_VERSION_LEVEL2 ) ; ssSetModelName ( rtS , "PV_Array_Model" ) ;
ssSetPath ( rtS , "PV_Array_Model" ) ; ssSetTStart ( rtS , 0.0 ) ;
ssSetTFinal ( rtS , 1.5 ) ; { static RTWLogInfo rt_DataLoggingInfo ;
rt_DataLoggingInfo . loggingInterval = NULL ; ssSetRTWLogInfo ( rtS , &
rt_DataLoggingInfo ) ; } { { static int_T rt_LoggedStateWidths [ ] = { 3 , 3
, 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 1 ,
34 , 1 , 1 , 2 , 3 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 } ; static int_T rt_LoggedStateNumDimensions [ ] = { 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
} ; static int_T rt_LoggedStateDimensions [ ] = { 3 , 3 , 3 , 3 , 3 , 3 , 3 ,
3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 3 , 1 , 34 , 1 , 1 , 2 , 3 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1
, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 } ; static
boolean_T rt_LoggedStateIsVarDims [ ] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ; static
BuiltInDTypeId rt_LoggedStateDataTypeIds [ ] = { SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE } ; static int_T rt_LoggedStateComplexSignals [ ] = { 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 } ; static RTWPreprocessingFcnPtr
rt_LoggingStatePreprocessingFcnPtrs [ ] = { ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) } ; static const char_T * rt_LoggedStateLabels [ ] = {
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" ,
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "states" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" ,
"DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" , "DSTATE" } ; static const char_T
* rt_LoggedStateBlockNames [ ] = { "PV_Array_Model/THD/Mean/Model/integrator"
, "PV_Array_Model/THD1/Mean/Model/integrator" ,
"PV_Array_Model/THD1/RMS2/TrueRMS /Mean value/Model/integrator" ,
"PV_Array_Model/THD1/RMS2/RMS /Fourier1/Mean value1/Model/integrator" ,
"PV_Array_Model/THD1/RMS2/RMS /Fourier1/Mean/Model/integrator" ,
"PV_Array_Model/THD1/RMS1/TrueRMS /Mean value/Model/integrator" ,
"PV_Array_Model/THD1/RMS1/RMS /Fourier1/Mean value1/Model/integrator" ,
"PV_Array_Model/THD1/RMS1/RMS /Fourier1/Mean/Model/integrator" ,
"PV_Array_Model/THD/RMS2/TrueRMS /Mean value/Model/integrator" ,
"PV_Array_Model/THD/RMS2/RMS /Fourier1/Mean value1/Model/integrator" ,
"PV_Array_Model/THD/RMS2/RMS /Fourier1/Mean/Model/integrator" ,
"PV_Array_Model/THD/RMS1/TrueRMS /Mean value/Model/integrator" ,
"PV_Array_Model/THD/RMS1/RMS /Fourier1/Mean value1/Model/integrator" ,
"PV_Array_Model/THD/RMS1/RMS /Fourier1/Mean/Model/integrator" ,
"PV_Array_Model/RMS1/TrueRMS /Mean value/Model/integrator" ,
"PV_Array_Model/RMS1/RMS /Fourier1/Mean value1/Model/integrator" ,
"PV_Array_Model/RMS1/RMS /Fourier1/Mean/Model/integrator" ,
"PV_Array_Model/RMS/TrueRMS /Mean value/Model/integrator" ,
"PV_Array_Model/RMS/RMS /Fourier1/Mean value1/Model/integrator" ,
"PV_Array_Model/RMS/RMS /Fourier1/Mean/Model/integrator" ,
"PV_Array_Model/PV Array/Diode Rsh/Unit Delay" ,
"PV_Array_Model/powergui/EquivalentModel1/State-Space" ,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Discrete-Time\nIntegrator"
,
 "PV_Array_Model/Inverter Control/VDC Regulator/PI/Integrator/Discrete/Integrator"
,
 "PV_Array_Model/Inverter Control/Current Regulator/PI/Integrator/Discrete/Integrator"
, "PV_Array_Model/Inverter Control/Unit\nDelay" ,
"PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Unit Delay"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Variable Frequency\nMean value/Model/Integ4"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Variable Frequency\nMean value/Model/Correction\nsubsystem/Unit Delay"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Variable Frequency\nMean value/Model/Unit Delay1"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Discrete/Discrete\nDerivative "
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Discrete/Discrete-Time\nIntegrator"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Second-Order\nFilter/Model/Delay_x1"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Second-Order\nFilter/Model/Delay_x2"
, "PV_Array_Model/Pdc/Mean/Model/Integ4" ,
"PV_Array_Model/Pdc/Mean/Model/Unit Delay" ,
"PV_Array_Model/Pdc/Mean/Model/Unit Delay1" ,
"PV_Array_Model/Pdc/Mean1/Model/Integ4" ,
"PV_Array_Model/Pdc/Mean1/Model/Unit Delay" ,
"PV_Array_Model/Pdc/Mean1/Model/Unit Delay1" ,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean value1/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean value1/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean value1/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean value1/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean value1/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean value1/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean value1/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean value1/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean value1/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean value1/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean value1/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean value1/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean value1/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean value1/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean value1/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean value1/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean value1/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean value1/Model/Unit Delay1"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean/Model/Integ4"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean/Model/Unit Delay"
,
 "PV_Array_Model/Pac/Power\n(Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean/Model/Unit Delay1"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)1/Model/Integ4"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)1/Model/Correction\nsubsystem/Unit Delay"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)1/Model/Unit Delay1"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)2/Model/Integ4"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)2/Model/Correction\nsubsystem/Unit Delay"
,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL\n(3ph)/Model/Automatic Gain\nControl/Positive-Sequence\n(PLL-Driven)/Mean\n(Variable Frequency)2/Model/Unit Delay1"
} ; static const char_T * rt_LoggedStateNames [ ] = { "" , "" , "" , "" , ""
, "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" ,
"" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , ""
, "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" ,
"" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , ""
, "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" , "" }
; static boolean_T rt_LoggedStateCrossMdlRef [ ] = { 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ;
static RTWLogDataTypeConvert rt_RTWLogDataTypeConvert [ ] = { { 0 , SS_DOUBLE
, SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 ,
0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } } ; static RTWLogSignalInfo rt_LoggedStateSignalInfo =
{ 82 , rt_LoggedStateWidths , rt_LoggedStateNumDimensions ,
rt_LoggedStateDimensions , rt_LoggedStateIsVarDims , ( NULL ) , ( NULL ) ,
rt_LoggedStateDataTypeIds , rt_LoggedStateComplexSignals , ( NULL ) ,
rt_LoggingStatePreprocessingFcnPtrs , { rt_LoggedStateLabels } , ( NULL ) , (
NULL ) , ( NULL ) , { rt_LoggedStateBlockNames } , { rt_LoggedStateNames } ,
rt_LoggedStateCrossMdlRef , rt_RTWLogDataTypeConvert } ; static void *
rt_LoggedStateSignalPtrs [ 82 ] ; rtliSetLogXSignalPtrs ( ssGetRTWLogInfo (
rtS ) , ( LogSignalPtrsType ) rt_LoggedStateSignalPtrs ) ;
rtliSetLogXSignalInfo ( ssGetRTWLogInfo ( rtS ) , & rt_LoggedStateSignalInfo
) ; rt_LoggedStateSignalPtrs [ 0 ] = ( void * ) & rtX . f2y4dmvcyy [ 0 ] ;
rt_LoggedStateSignalPtrs [ 1 ] = ( void * ) & rtX . nv45bjhe3t [ 0 ] ;
rt_LoggedStateSignalPtrs [ 2 ] = ( void * ) & rtX . daw0ymwmw4 . mr3ym4eux3 [
0 ] ; rt_LoggedStateSignalPtrs [ 3 ] = ( void * ) & rtX . pu1c2mp3al .
e2h5nin1ya [ 0 ] ; rt_LoggedStateSignalPtrs [ 4 ] = ( void * ) & rtX .
pu1c2mp3al . ndbmdbddqe [ 0 ] ; rt_LoggedStateSignalPtrs [ 5 ] = ( void * ) &
rtX . glvlabfrtp . mr3ym4eux3 [ 0 ] ; rt_LoggedStateSignalPtrs [ 6 ] = ( void
* ) & rtX . drtk0acyhp . e2h5nin1ya [ 0 ] ; rt_LoggedStateSignalPtrs [ 7 ] =
( void * ) & rtX . drtk0acyhp . ndbmdbddqe [ 0 ] ; rt_LoggedStateSignalPtrs [
8 ] = ( void * ) & rtX . blrugcnivv . mr3ym4eux3 [ 0 ] ;
rt_LoggedStateSignalPtrs [ 9 ] = ( void * ) & rtX . iks0grgus1 . e2h5nin1ya [
0 ] ; rt_LoggedStateSignalPtrs [ 10 ] = ( void * ) & rtX . iks0grgus1 .
ndbmdbddqe [ 0 ] ; rt_LoggedStateSignalPtrs [ 11 ] = ( void * ) & rtX .
jhi4zkfadx . mr3ym4eux3 [ 0 ] ; rt_LoggedStateSignalPtrs [ 12 ] = ( void * )
& rtX . mdskmorhmi . e2h5nin1ya [ 0 ] ; rt_LoggedStateSignalPtrs [ 13 ] = (
void * ) & rtX . mdskmorhmi . ndbmdbddqe [ 0 ] ; rt_LoggedStateSignalPtrs [
14 ] = ( void * ) & rtX . e2mpwvt2ua . mr3ym4eux3 [ 0 ] ;
rt_LoggedStateSignalPtrs [ 15 ] = ( void * ) & rtX . oa2z5wykl2 . e2h5nin1ya
[ 0 ] ; rt_LoggedStateSignalPtrs [ 16 ] = ( void * ) & rtX . oa2z5wykl2 .
ndbmdbddqe [ 0 ] ; rt_LoggedStateSignalPtrs [ 17 ] = ( void * ) & rtX .
gllp1vimnhl . mr3ym4eux3 [ 0 ] ; rt_LoggedStateSignalPtrs [ 18 ] = ( void * )
& rtX . kyedbo0mxuc . e2h5nin1ya [ 0 ] ; rt_LoggedStateSignalPtrs [ 19 ] = (
void * ) & rtX . kyedbo0mxuc . ndbmdbddqe [ 0 ] ; rt_LoggedStateSignalPtrs [
20 ] = ( void * ) & rtDW . jreglfqlm5 ; rt_LoggedStateSignalPtrs [ 21 ] = (
void * ) rtDW . hllwh2h1oo ; rt_LoggedStateSignalPtrs [ 22 ] = ( void * ) &
rtDW . m1peilszpy ; rt_LoggedStateSignalPtrs [ 23 ] = ( void * ) & rtDW .
dnrqy23bwk ; rt_LoggedStateSignalPtrs [ 24 ] = ( void * ) rtDW . jzbmotzdka ;
rt_LoggedStateSignalPtrs [ 25 ] = ( void * ) rtDW . jv0y1evb0u ;
rt_LoggedStateSignalPtrs [ 26 ] = ( void * ) & rtDW . giqv4pyyys ;
rt_LoggedStateSignalPtrs [ 27 ] = ( void * ) & rtDW . mwfdpj20rn ;
rt_LoggedStateSignalPtrs [ 28 ] = ( void * ) & rtDW . m5fkweyhnz ;
rt_LoggedStateSignalPtrs [ 29 ] = ( void * ) & rtDW . oyi34irdce ;
rt_LoggedStateSignalPtrs [ 30 ] = ( void * ) & rtDW . fmpdeay2dq ;
rt_LoggedStateSignalPtrs [ 31 ] = ( void * ) & rtDW . evobiqhvg0 ;
rt_LoggedStateSignalPtrs [ 32 ] = ( void * ) & rtDW . c3rg43fwng ;
rt_LoggedStateSignalPtrs [ 33 ] = ( void * ) & rtDW . puez01wvqo ;
rt_LoggedStateSignalPtrs [ 34 ] = ( void * ) & rtDW . otc3kwibnn ;
rt_LoggedStateSignalPtrs [ 35 ] = ( void * ) & rtDW . inc2xczn1j ;
rt_LoggedStateSignalPtrs [ 36 ] = ( void * ) & rtDW . hglk0ykmnp ;
rt_LoggedStateSignalPtrs [ 37 ] = ( void * ) & rtDW . iyz3blnd0r ;
rt_LoggedStateSignalPtrs [ 38 ] = ( void * ) & rtDW . okoa0ga0es ;
rt_LoggedStateSignalPtrs [ 39 ] = ( void * ) & rtDW . mlqfus2yxb ;
rt_LoggedStateSignalPtrs [ 40 ] = ( void * ) & rtDW . cv44xoew41 ;
rt_LoggedStateSignalPtrs [ 41 ] = ( void * ) & rtDW . amx5pvlykq ;
rt_LoggedStateSignalPtrs [ 42 ] = ( void * ) & rtDW . l1nmdmtivq ;
rt_LoggedStateSignalPtrs [ 43 ] = ( void * ) & rtDW . c3vwiq1oz1 ;
rt_LoggedStateSignalPtrs [ 44 ] = ( void * ) & rtDW . g55lsvsoyn ;
rt_LoggedStateSignalPtrs [ 45 ] = ( void * ) & rtDW . bf0dooycmi ;
rt_LoggedStateSignalPtrs [ 46 ] = ( void * ) & rtDW . is34tmu21f ;
rt_LoggedStateSignalPtrs [ 47 ] = ( void * ) & rtDW . bgndzrawrf ;
rt_LoggedStateSignalPtrs [ 48 ] = ( void * ) & rtDW . jwtcvtmaeo ;
rt_LoggedStateSignalPtrs [ 49 ] = ( void * ) & rtDW . crf5cecvju ;
rt_LoggedStateSignalPtrs [ 50 ] = ( void * ) & rtDW . o54g2pguii ;
rt_LoggedStateSignalPtrs [ 51 ] = ( void * ) & rtDW . berzbzjptr ;
rt_LoggedStateSignalPtrs [ 52 ] = ( void * ) & rtDW . eyj3xgyyy5 ;
rt_LoggedStateSignalPtrs [ 53 ] = ( void * ) & rtDW . iwb0lp4dat ;
rt_LoggedStateSignalPtrs [ 54 ] = ( void * ) & rtDW . mmleedibcx ;
rt_LoggedStateSignalPtrs [ 55 ] = ( void * ) & rtDW . iwtvu1xbzm ;
rt_LoggedStateSignalPtrs [ 56 ] = ( void * ) & rtDW . fdgmxn5rpv ;
rt_LoggedStateSignalPtrs [ 57 ] = ( void * ) & rtDW . mkbsymk41p ;
rt_LoggedStateSignalPtrs [ 58 ] = ( void * ) & rtDW . h4kspo12ff ;
rt_LoggedStateSignalPtrs [ 59 ] = ( void * ) & rtDW . ad2sn3k3gj ;
rt_LoggedStateSignalPtrs [ 60 ] = ( void * ) & rtDW . jy2blxntru ;
rt_LoggedStateSignalPtrs [ 61 ] = ( void * ) & rtDW . dbrynerjdo ;
rt_LoggedStateSignalPtrs [ 62 ] = ( void * ) & rtDW . ioxogrdks0 ;
rt_LoggedStateSignalPtrs [ 63 ] = ( void * ) & rtDW . ejhppynn0z ;
rt_LoggedStateSignalPtrs [ 64 ] = ( void * ) & rtDW . dqddocajhg ;
rt_LoggedStateSignalPtrs [ 65 ] = ( void * ) & rtDW . e2jfcom1xu ;
rt_LoggedStateSignalPtrs [ 66 ] = ( void * ) & rtDW . km0mzzn3gj ;
rt_LoggedStateSignalPtrs [ 67 ] = ( void * ) & rtDW . llg10jep1x ;
rt_LoggedStateSignalPtrs [ 68 ] = ( void * ) & rtDW . kkj5lgh0sd ;
rt_LoggedStateSignalPtrs [ 69 ] = ( void * ) & rtDW . oersb5zkry ;
rt_LoggedStateSignalPtrs [ 70 ] = ( void * ) & rtDW . mxwbciposz ;
rt_LoggedStateSignalPtrs [ 71 ] = ( void * ) & rtDW . mvp44fdh2o ;
rt_LoggedStateSignalPtrs [ 72 ] = ( void * ) & rtDW . fwzzbi3hli ;
rt_LoggedStateSignalPtrs [ 73 ] = ( void * ) & rtDW . pjo1bnuqfk ;
rt_LoggedStateSignalPtrs [ 74 ] = ( void * ) & rtDW . h45pdgkmki ;
rt_LoggedStateSignalPtrs [ 75 ] = ( void * ) & rtDW . ldxbwl52qd ;
rt_LoggedStateSignalPtrs [ 76 ] = ( void * ) & rtDW . lzgy3px31z ;
rt_LoggedStateSignalPtrs [ 77 ] = ( void * ) & rtDW . omufifkqbz ;
rt_LoggedStateSignalPtrs [ 78 ] = ( void * ) & rtDW . nndlftph12 ;
rt_LoggedStateSignalPtrs [ 79 ] = ( void * ) & rtDW . ohydhrpaay ;
rt_LoggedStateSignalPtrs [ 80 ] = ( void * ) & rtDW . eznk4sr03e ;
rt_LoggedStateSignalPtrs [ 81 ] = ( void * ) & rtDW . aktl42ci5l ; }
rtliSetLogT ( ssGetRTWLogInfo ( rtS ) , "tout" ) ; rtliSetLogX (
ssGetRTWLogInfo ( rtS ) , "tmp_raccel_xout" ) ; rtliSetLogXFinal (
ssGetRTWLogInfo ( rtS ) , "xFinal" ) ; rtliSetLogVarNameModifier (
ssGetRTWLogInfo ( rtS ) , "none" ) ; rtliSetLogFormat ( ssGetRTWLogInfo ( rtS
) , 2 ) ; rtliSetLogMaxRows ( ssGetRTWLogInfo ( rtS ) , 1000 ) ;
rtliSetLogDecimation ( ssGetRTWLogInfo ( rtS ) , 1 ) ; rtliSetLogY (
ssGetRTWLogInfo ( rtS ) , "" ) ; rtliSetLogYSignalInfo ( ssGetRTWLogInfo (
rtS ) , ( NULL ) ) ; rtliSetLogYSignalPtrs ( ssGetRTWLogInfo ( rtS ) , ( NULL
) ) ; } { static struct _ssStatesInfo2 statesInfo2 ; ssSetStatesInfo2 ( rtS ,
& statesInfo2 ) ; } { static ssPeriodicStatesInfo periodicStatesInfo ;
ssSetPeriodicStatesInfo ( rtS , & periodicStatesInfo ) ; } { static
ssJacobianPerturbationBounds jacobianPerturbationBounds ;
ssSetJacobianPerturbationBounds ( rtS , & jacobianPerturbationBounds ) ; } {
static ssSolverInfo slvrInfo ; static boolean_T contStatesDisabled [ 60 ] ;
static real_T absTol [ 60 ] = { 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 ,
1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 } ; static
uint8_T absTolControl [ 60 ] = { 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U ,
0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U
, 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U ,
0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U , 0U
, 0U , 0U , 0U , 0U } ; static real_T contStateJacPerturbBoundMinVec [ 60 ] ;
static real_T contStateJacPerturbBoundMaxVec [ 60 ] ; static uint8_T
zcAttributes [ 36 ] = { ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL
) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL )
, ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) ,
( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , (
ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , (
ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , (
ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , (
ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , (
ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) , (
ZC_EVENT_ALL ) } ; static ssNonContDerivSigInfo nonContDerivSigInfo [ 2 ] = {
{ 3 * sizeof ( real_T ) , ( char * ) ( & rtB . etkri0io2o [ 0 ] ) , ( NULL )
} , { 3 * sizeof ( real_T ) , ( char * ) ( & rtB . kzukygsbbq [ 0 ] ) , (
NULL ) } } ; { int i ; for ( i = 0 ; i < 60 ; ++ i ) {
contStateJacPerturbBoundMinVec [ i ] = 0 ; contStateJacPerturbBoundMaxVec [ i
] = rtGetInf ( ) ; } } ssSetSolverRelTol ( rtS , 0.001 ) ; ssSetStepSize (
rtS , 0.0 ) ; ssSetMinStepSize ( rtS , 0.0 ) ; ssSetMaxNumMinSteps ( rtS , -
1 ) ; ssSetMinStepViolatedError ( rtS , 0 ) ; ssSetMaxStepSize ( rtS ,
5.0505050505050507E-6 ) ; ssSetSolverMaxOrder ( rtS , - 1 ) ;
ssSetSolverRefineFactor ( rtS , 1 ) ; ssSetOutputTimes ( rtS , ( NULL ) ) ;
ssSetNumOutputTimes ( rtS , 0 ) ; ssSetOutputTimesOnly ( rtS , 0 ) ;
ssSetOutputTimesIndex ( rtS , 0 ) ; ssSetZCCacheNeedsReset ( rtS , 1 ) ;
ssSetDerivCacheNeedsReset ( rtS , 0 ) ; ssSetNumNonContDerivSigInfos ( rtS ,
2 ) ; ssSetNonContDerivSigInfos ( rtS , nonContDerivSigInfo ) ;
ssSetSolverInfo ( rtS , & slvrInfo ) ; ssSetSolverName ( rtS , "ode23t" ) ;
ssSetVariableStepSolver ( rtS , 1 ) ; ssSetSolverConsistencyChecking ( rtS ,
0 ) ; ssSetSolverAdaptiveZcDetection ( rtS , 0 ) ;
ssSetSolverRobustResetMethod ( rtS , 0 ) ; ssSetAbsTolVector ( rtS , absTol )
; ssSetAbsTolControlVector ( rtS , absTolControl ) ;
ssSetSolverAbsTol_Obsolete ( rtS , absTol ) ;
ssSetSolverAbsTolControl_Obsolete ( rtS , absTolControl ) ;
ssSetJacobianPerturbationBoundsMinVec ( rtS , contStateJacPerturbBoundMinVec
) ; ssSetJacobianPerturbationBoundsMaxVec ( rtS ,
contStateJacPerturbBoundMaxVec ) ; ssSetSolverStateProjection ( rtS , 0 ) ;
ssSetSolverMassMatrixType ( rtS , ( ssMatrixType ) 0 ) ;
ssSetSolverMassMatrixNzMax ( rtS , 0 ) ; ssSetModelOutputs ( rtS , MdlOutputs
) ; ssSetModelLogData ( rtS , rt_UpdateTXYLogVars ) ;
ssSetModelLogDataIfInInterval ( rtS , rt_UpdateTXXFYLogVars ) ;
ssSetModelUpdate ( rtS , MdlUpdate ) ; ssSetModelDerivatives ( rtS ,
MdlDerivatives ) ; ssSetSolverZcSignalAttrib ( rtS , zcAttributes ) ;
ssSetSolverNumZcSignals ( rtS , 36 ) ; ssSetModelZeroCrossings ( rtS ,
MdlZeroCrossings ) ; ssSetSolverConsecutiveZCsStepRelTol ( rtS ,
2.8421709430404007E-13 ) ; ssSetSolverMaxConsecutiveZCs ( rtS , 1000 ) ;
ssSetSolverConsecutiveZCsError ( rtS , 2 ) ; ssSetSolverMaskedZcDiagnostic (
rtS , 1 ) ; ssSetSolverIgnoredZcDiagnostic ( rtS , 1 ) ;
ssSetSolverMaxConsecutiveMinStep ( rtS , 1 ) ;
ssSetSolverShapePreserveControl ( rtS , 2 ) ; ssSetTNextTid ( rtS , INT_MIN )
; ssSetTNext ( rtS , rtMinusInf ) ; ssSetSolverNeedsReset ( rtS ) ;
ssSetNumNonsampledZCs ( rtS , 36 ) ; ssSetContStateDisabled ( rtS ,
contStatesDisabled ) ; ssSetSolverMaxConsecutiveMinStep ( rtS , 1 ) ; }
ssSetChecksumVal ( rtS , 0 , 3932948024U ) ; ssSetChecksumVal ( rtS , 1 ,
1333879620U ) ; ssSetChecksumVal ( rtS , 2 , 2973354861U ) ; ssSetChecksumVal
( rtS , 3 , 1201730294U ) ; { static const sysRanDType rtAlwaysEnabled =
SUBSYS_RAN_BC_ENABLE ; static RTWExtModeInfo rt_ExtModeInfo ; static const
sysRanDType * systemRan [ 72 ] ; gblRTWExtModeInfo = & rt_ExtModeInfo ;
ssSetRTWExtModeInfo ( rtS , & rt_ExtModeInfo ) ;
rteiSetSubSystemActiveVectorAddresses ( & rt_ExtModeInfo , systemRan ) ;
systemRan [ 0 ] = & rtAlwaysEnabled ; systemRan [ 1 ] = & rtAlwaysEnabled ;
systemRan [ 2 ] = ( sysRanDType * ) & rtDW . nitdz0i0m1 ; systemRan [ 3 ] = (
sysRanDType * ) & rtDW . b3ha3knqwy ; systemRan [ 4 ] = ( sysRanDType * ) &
rtDW . cpk3jnibze ; systemRan [ 5 ] = ( sysRanDType * ) & rtDW . ntzroniten ;
systemRan [ 6 ] = ( sysRanDType * ) & rtDW . ntzroniten ; systemRan [ 7 ] = (
sysRanDType * ) & rtDW . fiy4znthvv3 . cyyxenso3u ; systemRan [ 8 ] = (
sysRanDType * ) & rtDW . flkfnl3dquj . nevvtwmofe ; systemRan [ 9 ] = (
sysRanDType * ) & rtDW . ntzroniten ; systemRan [ 10 ] = & rtAlwaysEnabled ;
systemRan [ 11 ] = ( sysRanDType * ) & rtDW . h0t1xli1qx . cyyxenso3u ;
systemRan [ 12 ] = ( sysRanDType * ) & rtDW . gvjufizpfu . nevvtwmofe ;
systemRan [ 13 ] = ( sysRanDType * ) & rtDW . gusdfzbcqa . cyyxenso3u ;
systemRan [ 14 ] = ( sysRanDType * ) & rtDW . imgy1z4qes . nevvtwmofe ;
systemRan [ 15 ] = ( sysRanDType * ) & rtDW . bdvvkdwqfm . cyyxenso3u ;
systemRan [ 16 ] = ( sysRanDType * ) & rtDW . ibrtt4ftsl . nevvtwmofe ;
systemRan [ 17 ] = & rtAlwaysEnabled ; systemRan [ 18 ] = ( sysRanDType * ) &
rtDW . le4iyy3q51 ; systemRan [ 19 ] = ( sysRanDType * ) & rtDW . b43mt2kku5
; systemRan [ 20 ] = & rtAlwaysEnabled ; systemRan [ 21 ] = & rtAlwaysEnabled
; systemRan [ 22 ] = & rtAlwaysEnabled ; systemRan [ 23 ] = & rtAlwaysEnabled
; systemRan [ 24 ] = & rtAlwaysEnabled ; systemRan [ 25 ] = & rtAlwaysEnabled
; systemRan [ 26 ] = ( sysRanDType * ) & rtDW . gj0ekbre2up . hrxuy113gk ;
systemRan [ 27 ] = ( sysRanDType * ) & rtDW . jr3gbt2o32 . hrxuy113gk ;
systemRan [ 28 ] = ( sysRanDType * ) & rtDW . ihqnx345xuh . bwjj0lhfrh ;
systemRan [ 29 ] = & rtAlwaysEnabled ; systemRan [ 30 ] = & rtAlwaysEnabled ;
systemRan [ 31 ] = & rtAlwaysEnabled ; systemRan [ 32 ] = & rtAlwaysEnabled ;
systemRan [ 33 ] = & rtAlwaysEnabled ; systemRan [ 34 ] = & rtAlwaysEnabled ;
systemRan [ 35 ] = ( sysRanDType * ) & rtDW . drzlaqk2wl . hrxuy113gk ;
systemRan [ 36 ] = ( sysRanDType * ) & rtDW . h1nhkvogk5 . hrxuy113gk ;
systemRan [ 37 ] = ( sysRanDType * ) & rtDW . cxxbka35es . bwjj0lhfrh ;
systemRan [ 38 ] = & rtAlwaysEnabled ; systemRan [ 39 ] = & rtAlwaysEnabled ;
systemRan [ 40 ] = ( sysRanDType * ) & rtDW . kyedbo0mxuc . gpfaohkthl ;
systemRan [ 41 ] = ( sysRanDType * ) & rtDW . kyedbo0mxuc . gpfaohkthl ;
systemRan [ 42 ] = ( sysRanDType * ) & rtDW . kyedbo0mxuc . gpfaohkthl ;
systemRan [ 43 ] = ( sysRanDType * ) & rtDW . gllp1vimnhl . oqyvh1ylxe ;
systemRan [ 44 ] = ( sysRanDType * ) & rtDW . gllp1vimnhl . oqyvh1ylxe ;
systemRan [ 45 ] = ( sysRanDType * ) & rtDW . oa2z5wykl2 . gpfaohkthl ;
systemRan [ 46 ] = ( sysRanDType * ) & rtDW . oa2z5wykl2 . gpfaohkthl ;
systemRan [ 47 ] = ( sysRanDType * ) & rtDW . oa2z5wykl2 . gpfaohkthl ;
systemRan [ 48 ] = ( sysRanDType * ) & rtDW . e2mpwvt2ua . oqyvh1ylxe ;
systemRan [ 49 ] = ( sysRanDType * ) & rtDW . e2mpwvt2ua . oqyvh1ylxe ;
systemRan [ 50 ] = & rtAlwaysEnabled ; systemRan [ 51 ] = ( sysRanDType * ) &
rtDW . mdskmorhmi . gpfaohkthl ; systemRan [ 52 ] = ( sysRanDType * ) & rtDW
. mdskmorhmi . gpfaohkthl ; systemRan [ 53 ] = ( sysRanDType * ) & rtDW .
mdskmorhmi . gpfaohkthl ; systemRan [ 54 ] = ( sysRanDType * ) & rtDW .
jhi4zkfadx . oqyvh1ylxe ; systemRan [ 55 ] = ( sysRanDType * ) & rtDW .
jhi4zkfadx . oqyvh1ylxe ; systemRan [ 56 ] = ( sysRanDType * ) & rtDW .
iks0grgus1 . gpfaohkthl ; systemRan [ 57 ] = ( sysRanDType * ) & rtDW .
iks0grgus1 . gpfaohkthl ; systemRan [ 58 ] = ( sysRanDType * ) & rtDW .
iks0grgus1 . gpfaohkthl ; systemRan [ 59 ] = ( sysRanDType * ) & rtDW .
blrugcnivv . oqyvh1ylxe ; systemRan [ 60 ] = ( sysRanDType * ) & rtDW .
blrugcnivv . oqyvh1ylxe ; systemRan [ 61 ] = & rtAlwaysEnabled ; systemRan [
62 ] = ( sysRanDType * ) & rtDW . drtk0acyhp . gpfaohkthl ; systemRan [ 63 ]
= ( sysRanDType * ) & rtDW . drtk0acyhp . gpfaohkthl ; systemRan [ 64 ] = (
sysRanDType * ) & rtDW . drtk0acyhp . gpfaohkthl ; systemRan [ 65 ] = (
sysRanDType * ) & rtDW . glvlabfrtp . oqyvh1ylxe ; systemRan [ 66 ] = (
sysRanDType * ) & rtDW . glvlabfrtp . oqyvh1ylxe ; systemRan [ 67 ] = (
sysRanDType * ) & rtDW . pu1c2mp3al . gpfaohkthl ; systemRan [ 68 ] = (
sysRanDType * ) & rtDW . pu1c2mp3al . gpfaohkthl ; systemRan [ 69 ] = (
sysRanDType * ) & rtDW . pu1c2mp3al . gpfaohkthl ; systemRan [ 70 ] = (
sysRanDType * ) & rtDW . daw0ymwmw4 . oqyvh1ylxe ; systemRan [ 71 ] = (
sysRanDType * ) & rtDW . daw0ymwmw4 . oqyvh1ylxe ; rteiSetModelMappingInfoPtr
( ssGetRTWExtModeInfo ( rtS ) , & ssGetModelMappingInfo ( rtS ) ) ;
rteiSetChecksumsPtr ( ssGetRTWExtModeInfo ( rtS ) , ssGetChecksums ( rtS ) )
; rteiSetTPtr ( ssGetRTWExtModeInfo ( rtS ) , ssGetTPtr ( rtS ) ) ; } rtP .
Saturation_UpperSat = rtInf ; rtP . DiscreteTimeIntegrator_UpperSat = rtInf ;
rtP . Saturation1_UpperSat = rtInf ; rtP .
Saturationtoavoiddivisionby0_UpperSat = rtInf ; rtP .
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo = rtInf ; rtP .
Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq = rtInf ; rtP .
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w = rtInf ; rtP . daw0ymwmw4 .
Saturationtoavoidnegativesqrt_UpperSat = rtInf ; rtP . glvlabfrtp .
Saturationtoavoidnegativesqrt_UpperSat = rtInf ; rtP . blrugcnivv .
Saturationtoavoidnegativesqrt_UpperSat = rtInf ; rtP . jhi4zkfadx .
Saturationtoavoidnegativesqrt_UpperSat = rtInf ; rtP . e2mpwvt2ua .
Saturationtoavoidnegativesqrt_UpperSat = rtInf ; rtP . gllp1vimnhl .
Saturationtoavoidnegativesqrt_UpperSat = rtInf ; ssSetNumSFunctions ( rtS ,
25 ) ; { static SimStruct childSFunctions [ 25 ] ; static SimStruct *
childSFunctionPtrs [ 25 ] ; ( void ) memset ( ( void * ) & childSFunctions [
0 ] , 0 , sizeof ( childSFunctions ) ) ; ssSetSFunctions ( rtS , &
childSFunctionPtrs [ 0 ] ) ; { int_T i ; for ( i = 0 ; i < 25 ; i ++ ) {
ssSetSFunction ( rtS , i , & childSFunctions [ i ] ) ; } } { SimStruct * rts
= ssGetSFunction ( rtS , 0 ) ; static time_T sfcnPeriod [ 1 ] ; static time_T
sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void ) memset ( ( void *
) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void ) memset ( ( void * )
sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ; ssSetSampleTimePtr ( rts , &
sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts , & sfcnOffset [ 0 ] ) ;
ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; { static struct _ssBlkInfo2
_blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = & _blkInfo2 ; ssSetBlkInfo2Ptr (
rts , blkInfo2 ) ; } { static struct _ssPortInfo2 _portInfo2 ; struct
_ssPortInfo2 * portInfo2 = & _portInfo2 ; _ssSetBlkInfo2PortInfo2Ptr ( rts ,
portInfo2 ) ; } ssSetMdlInfoPtr ( rts , ssGetMdlInfoPtr ( rtS ) ) ; { static
struct _ssSFcnModelMethods2 methods2 ; ssSetModelMethods2 ( rts , & methods2
) ; } { static struct _ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 (
rts , & methods3 ) ; } { static struct _ssSFcnModelMethods4 methods4 ;
ssSetModelMethods4 ( rts , & methods4 ) ; } { static struct _ssStatesInfo2
statesInfo2 ; static ssPeriodicStatesInfo periodicStatesInfo ; static
ssJacobianPerturbationBounds jacPerturbationBounds ; ssSetStatesInfo2 ( rts ,
& statesInfo2 ) ; ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . bzadnafn2c ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
jvbkza0bug ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . chkgbobqc5
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL (3ph)/Model/Automatic Gain Control/Positive-Sequence (PLL-Driven)/Mean (Variable Frequency)1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size ) ; ssSetSFcnParam ( rts , 1 , (
mxArray * ) rtP . SFunction_P2_Size ) ; ssSetSFcnParam ( rts , 2 , ( mxArray
* ) rtP . SFunction_P3_Size ) ; ssSetSFcnParam ( rts , 3 , ( mxArray * ) rtP
. SFunction_P4_Size ) ; } ssSetRWork ( rts , ( real_T * ) & rtDW . g0q3b3bwh1
) ; ssSetIWork ( rts , ( int_T * ) & rtDW . a4vswgnpt2 ) ; ssSetPWork ( rts ,
( void * * ) & rtDW . by3miegpcz ) ; { static struct _ssDWorkRecord
dWorkRecord [ 3 ] ; static struct _ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ;
ssSetSFcnDWork ( rts , dWorkRecord ) ; ssSetSFcnDWorkAux ( rts ,
dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ; ssSetDWorkWidth ( rts , 0 , 1
) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE ) ; ssSetDWorkComplexSignal (
rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW . g0q3b3bwh1 ) ;
ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1 , SS_INTEGER )
; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts , 1 , & rtDW .
a4vswgnpt2 ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ; ssSetDWorkDataType ( rts , 2
, SS_POINTER ) ; ssSetDWorkComplexSignal ( rts , 2 , 0 ) ; ssSetDWork ( rts ,
2 , & rtDW . by3miegpcz ) ; } sfun_discreteVariableDelay ( rts ) ;
sfcnInitializeSizes ( rts ) ; sfcnInitializeSampleTimes ( rts ) ;
ssSetSampleTime ( rts , 0 , 5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts ,
0 , 0.0 ) ; sfcnTsMap [ 0 ] = 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ;
ssSetInputPortDataType ( rts , 0 , SS_DOUBLE ) ; ssSetInputPortComplexSignal
( rts , 0 , 0 ) ; ssSetInputPortFrameData ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts
, 0 , 0 ) ; ssSetInputPortWidth ( rts , 1 , 1 ) ; ssSetInputPortDataType (
rts , 1 , SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 1 , 0 ) ;
ssSetInputPortFrameData ( rts , 1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 1 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . as2m1qy4ku ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
gndfpjrzbt ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . dlvjgbn4i5
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL (3ph)/Model/Automatic Gain Control/Positive-Sequence (PLL-Driven)/Mean (Variable Frequency)2/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_gcwln5h2fy ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_cja0aqxvcz ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_av3rmjmb5f ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_hngc2st1ax ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . pmrorw1f23 ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. nv33qldkfr ) ; ssSetPWork ( rts , ( void * * ) & rtDW . ni0rxjpduc ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
pmrorw1f23 ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . nv33qldkfr ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . ni0rxjpduc ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 2 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . bkfh2vcfmq ;
sfcnUPtrs [ 1 ] = & rtB . bkfh2vcfmq [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
bkfh2vcfmq [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . o4gd0y3v5b ; sfcnUPtrs [ 1 ] = & rtB . o4gd0y3v5b
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . o4gd0y3v5b [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . g3hcemkbbs )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/14-km Feeder/Model/Discrete Variable Time Delay_V1/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_eb5a0ck1sr ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_a0f30lilzp ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_ihynnzuoja ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_edzxkjjlwn ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . afeqr4soi1 ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. pazrwyzcfm [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . bcokcp0qkj [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
afeqr4soi1 ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . pazrwyzcfm [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . bcokcp0qkj [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 3 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . gaih2ptcqs ;
sfcnUPtrs [ 1 ] = & rtB . gaih2ptcqs [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
gaih2ptcqs [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . dywo4uhhag ; sfcnUPtrs [ 1 ] = & rtB . dywo4uhhag
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . dywo4uhhag [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . atzxigju2m )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/14-km Feeder/Model/Discrete Variable Time Delay_I1/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_oayvoucrma ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_f05fppxlwk ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_hnxggd2vpu ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_kl1dwgtupy ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . eljq3fss0e ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. jtitiatpbl [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . b1c14x4npm [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
eljq3fss0e ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . jtitiatpbl [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . b1c14x4npm [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 4 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . gctmfhb3tj ;
sfcnUPtrs [ 1 ] = & rtB . gctmfhb3tj [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
gctmfhb3tj [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . dywo4uhhag ; sfcnUPtrs [ 1 ] = & rtB . dywo4uhhag
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . dywo4uhhag [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . d0r0bfxxhv )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/14-km Feeder/Model/Discrete Variable Time Delay_I2/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_lxdwb50jvc ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_jijo4kujdd ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_dhi0mtqkva ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_b0bt51nryg ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . merlacocwu ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. ak5hmrpbyv [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . cevsb2sslr [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
merlacocwu ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . ak5hmrpbyv [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . cevsb2sslr [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 5 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . hs4xhfhuu4 ;
sfcnUPtrs [ 1 ] = & rtB . hs4xhfhuu4 [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
hs4xhfhuu4 [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . o4gd0y3v5b ; sfcnUPtrs [ 1 ] = & rtB . o4gd0y3v5b
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . o4gd0y3v5b [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . hu3llba3ca )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/14-km Feeder/Model/Discrete Variable Time Delay_V2/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_ojey0fic3d ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_nkfz30iz0g ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_gsuuzkedwl ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_n44xdy2tit ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . cnqpt10soe ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. m1diazv1pn [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . lbbf2yann5 [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
cnqpt10soe ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . m1diazv1pn [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . lbbf2yann5 [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 6 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . bud3z3gi3t ;
sfcnUPtrs [ 1 ] = & rtB . bud3z3gi3t [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
bud3z3gi3t [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . ft0dn4u15h ; sfcnUPtrs [ 1 ] = & rtB . ft0dn4u15h
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . ft0dn4u15h [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . bk3h34bboy )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/8-km Feeder/Model/Discrete Variable Time Delay_V1/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_iox5d4w0ir ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_glmkyfxahk ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_f4azrwnmre ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_ljktgvqp4f ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . bnggfcupbm ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. cc3bay3gng [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . bsj5iiu43b [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
bnggfcupbm ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . cc3bay3gng [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . bsj5iiu43b [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 7 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . esekxgdj1v ;
sfcnUPtrs [ 1 ] = & rtB . esekxgdj1v [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
esekxgdj1v [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . ekwirjyast ; sfcnUPtrs [ 1 ] = & rtB . ekwirjyast
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . ekwirjyast [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . aj3jiuqdac )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/8-km Feeder/Model/Discrete Variable Time Delay_I1/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_fh43eahwlo ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_jt15ntmltf ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_ptdahzgl4q ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_jnuso5m51u ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . pd03h3b1tv ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. ndlf3lnho1 [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . abc1w5vdl5 [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
pd03h3b1tv ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . ndlf3lnho1 [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . abc1w5vdl5 [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 8 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . gn420ui3bc ;
sfcnUPtrs [ 1 ] = & rtB . gn420ui3bc [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
gn420ui3bc [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . ekwirjyast ; sfcnUPtrs [ 1 ] = & rtB . ekwirjyast
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . ekwirjyast [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . ikd1laxvge )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/8-km Feeder/Model/Discrete Variable Time Delay_I2/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_hvkdhrhi0y ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_jrvfe2qggu ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_ppes2kzqgm ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_la0vmwgdtu ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . i0ikhfczlc ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. piwvjcyewx [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . fawbkmqhtu [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
i0ikhfczlc ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . piwvjcyewx [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . fawbkmqhtu [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 9 ) ; static time_T sfcnPeriod [ 1 ]
; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 3 ] ; sfcnUPtrs [ 0 ] = rtB . a3oqmvvhoe ;
sfcnUPtrs [ 1 ] = & rtB . a3oqmvvhoe [ 1 ] ; sfcnUPtrs [ 2 ] = & rtB .
a3oqmvvhoe [ 2 ] ; ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 0 , 1 ) ;
ssSetInputPortWidth ( rts , 0 , 3 ) ; } { static real_T const * sfcnUPtrs [ 3
] ; sfcnUPtrs [ 0 ] = rtB . ft0dn4u15h ; sfcnUPtrs [ 1 ] = & rtB . ft0dn4u15h
[ 1 ] ; sfcnUPtrs [ 2 ] = & rtB . ft0dn4u15h [ 2 ] ; ssSetInputPortSignalPtrs
( rts , 1 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 1 , 1 ) ; ssSetInputPortWidth ( rts , 1
, 3 ) ; } } { static struct _ssPortOutputs outputPortInfo [ 1 ] ;
ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ] ) ;
_ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 3 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) rtB . nj1powhokr )
) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/8-km Feeder/Model/Discrete Variable Time Delay_V2/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_arlbgtsvtw ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_f22bb0beoq ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_hqvx5dpsdq ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_amoqxoisfs ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . mh2g3u0j0y ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. gzluk1ehd1 [ 0 ] ) ; ssSetPWork ( rts , ( void * * ) & rtDW . eztv3pazqo [
0 ] ) ; { static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
mh2g3u0j0y ) ; ssSetDWorkWidth ( rts , 1 , 3 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . gzluk1ehd1 [ 0 ] ) ; ssSetDWorkWidth ( rts , 2 , 3 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . eztv3pazqo [ 0 ] ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050507E-6 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 2 ; ssSetInputPortWidth ( rts , 0 , 3 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 3 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 3 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 3
) ; ssSetNumPWork ( rts , 3 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 10 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . myfgavzlid ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
bstj0nioer ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . ebxa1ugh4c
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Inverter Control/PLL & Measurements/PLL (3ph)/Model/Variable Frequency Mean value/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_azl4fmid3u ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_dvmzlzudlq ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_fgok1k5lwp ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_nocru43ywt ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . i3mgxdl4vo ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. bhpkupwt0n ) ; ssSetPWork ( rts , ( void * * ) & rtDW . iiu4wr0f5r ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
i3mgxdl4vo ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . bhpkupwt0n ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . iiu4wr0f5r ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 11 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . krgk34ykj1 ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
mkrkhji0cg ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . ckmd25fctl
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/Pdc/Mean/Model/Discrete Variable Time Delay/S-Function" ) ;
if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_ew2pm3b1vb ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_kngh2atg03 ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_la2ztzk3gj ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_gtvcyjcime ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . hyi1vb4nfg ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. hhdyt0quta ) ; ssSetPWork ( rts , ( void * * ) & rtDW . aknzofkqx0 ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
hyi1vb4nfg ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . hhdyt0quta ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . aknzofkqx0 ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 12 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . esjg3pcq0t ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
l12i0pjkca ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . ni2s4rtms3
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
"PV_Array_Model/Pdc/Mean1/Model/Discrete Variable Time Delay/S-Function" ) ;
if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_odjwhpkqrj ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_fmypdjztco ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_oeoze2eg3f ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_bjt1eob1om ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . fztzuxh3fu ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. mzcyzi1mrw ) ; ssSetPWork ( rts , ( void * * ) & rtDW . c4xrejr2sm ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
fztzuxh3fu ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . mzcyzi1mrw ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . c4xrejr2sm ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 13 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . bzovtsvol2 ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
lvxda1zemz ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . d0gp45izbd
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean value1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_ibhpobj5mh ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_hndwfqeer1 ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_kqonwcz2dd ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_gljhqdhvcw ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . m4n3n1vgn4 ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. lunlsisyew ) ; ssSetPWork ( rts , ( void * * ) & rtDW . el4u4uq5nb ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
m4n3n1vgn4 ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . lunlsisyew ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . el4u4uq5nb ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 14 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . lntmbixtgk ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
hdh2j1iy05 ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . f1i4vqi2kd
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer/Fourier_A/Mean/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_pfhrkjciej ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_mhope5jeqx ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_ky0vccwnr2 ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_di5ycaxh3v ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . j3wumogm4s ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. bdjx3fnp20 ) ; ssSetPWork ( rts , ( void * * ) & rtDW . mkajacfjkh ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
j3wumogm4s ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . bdjx3fnp20 ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . mkajacfjkh ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 15 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . f4wsdyxsap ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
psu4dh2yoj ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . on2hc5txnr
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean value1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_ptq3yp4fjx ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_ke5xzzx4kh ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_gp3iwgfnzr ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_fjvrmxlm3b ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . abuos4nqex ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. hssr4w0h5w ) ; ssSetPWork ( rts , ( void * * ) & rtDW . cljmzrmfcy ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
abuos4nqex ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . hssr4w0h5w ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . cljmzrmfcy ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 16 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . aeqligfyi1 ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
bxlm4t4z32 ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . fj2bifvg5t
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer/Fourier_B/Mean/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_cjan5gzh5s ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_jvanv0ykea ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_jgylmcydz1 ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_fcjx3ghw1g ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . ey4kijxpsz ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. l1mpdcz1eb ) ; ssSetPWork ( rts , ( void * * ) & rtDW . ga0gkvh2kb ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
ey4kijxpsz ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . l1mpdcz1eb ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . ga0gkvh2kb ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 17 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . lqaoyuj05s ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
a2i2ov02lb ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . ienudooawn
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean value1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_pejtedqyc4 ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_hlcp1ovx4k ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_ftrxnxhe1f ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_jds5a2cbzo ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . kktgq5zogl ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. pkxv552qte ) ; ssSetPWork ( rts , ( void * * ) & rtDW . bb1zyrk33s ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
kktgq5zogl ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . pkxv552qte ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . bb1zyrk33s ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 18 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . eutj1tzkyf ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
pf0efcoh3z ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . hqbivcjz0k
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer/Fourier_C/Mean/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_n1ajy1bnud ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_e0a1ljwhhh ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_g2ktdcnint ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_ouyv0d4ulf ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . nzmhyhlryd ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. ejb25uem32 ) ; ssSetPWork ( rts , ( void * * ) & rtDW . otbq3qatfg ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
nzmhyhlryd ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . ejb25uem32 ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . otbq3qatfg ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 19 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . f4bvvydmna ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
fvjbmcpghe ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . nvcc0bievu
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean value1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_jukdwcj1bv ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_psnzhais11 ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_l30wnhs0mm ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_ktxkgaz1xj ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . f2lkfuuga3 ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. cyjecukgkv ) ; ssSetPWork ( rts , ( void * * ) & rtDW . kmczgf120y ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
f2lkfuuga3 ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . cyjecukgkv ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . kmczgf120y ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 20 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . b4ioytjw1o ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
gmfn04oapy ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . dhrszd2dqf
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer1/Fourier_A/Mean/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_nmlo2jbnyf ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_lziftoqs2s ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_izfvfjalyr ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_gy1avcz5ha ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . ebzux3ox5l ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. mpcathptum ) ; ssSetPWork ( rts , ( void * * ) & rtDW . ch4lkuxrcg ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
ebzux3ox5l ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . mpcathptum ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . ch4lkuxrcg ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 21 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . pddsnfsfnu ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
ilbet5yid2 ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . fbe5ufjhz0
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean value1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_i4ous05wd0 ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_nomaj1bgdp ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_n4gzbc3msr ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_g2xqkvldm0 ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . fqeweas2gy ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. nvvegqclb5 ) ; ssSetPWork ( rts , ( void * * ) & rtDW . bekhzyaoss ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
fqeweas2gy ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . nvvegqclb5 ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . bekhzyaoss ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 22 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . cyaarq5wc5 ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
nabvxanmoe ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . djupb53czf
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer1/Fourier_B/Mean/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_cvpkxdvatx ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_fwn5zkbns1 ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_pedctzjhmg ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_nydreygcaq ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . pj4mixmqsa ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. gbcxxqxjpd ) ; ssSetPWork ( rts , ( void * * ) & rtDW . hib1rdb5vn ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
pj4mixmqsa ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . gbcxxqxjpd ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . hib1rdb5vn ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 23 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . mxfhajo1pf ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
inbgwouqfx ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . jrohv3tr0m
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean value1/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_pd353sj30r ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_a1y0oaqnrc ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_er4czmynw5 ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_pk24ovu2kv ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . jvbe5wkg5h ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. bb2ftcsloy ) ; ssSetPWork ( rts , ( void * * ) & rtDW . fqibuqwt5r ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
jvbe5wkg5h ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . bb2ftcsloy ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . fqibuqwt5r ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } {
SimStruct * rts = ssGetSFunction ( rtS , 24 ) ; static time_T sfcnPeriod [ 1
] ; static time_T sfcnOffset [ 1 ] ; static int_T sfcnTsMap [ 1 ] ; ( void )
memset ( ( void * ) sfcnPeriod , 0 , sizeof ( time_T ) * 1 ) ; ( void )
memset ( ( void * ) sfcnOffset , 0 , sizeof ( time_T ) * 1 ) ;
ssSetSampleTimePtr ( rts , & sfcnPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rts ,
& sfcnOffset [ 0 ] ) ; ssSetSampleTimeTaskIDPtr ( rts , sfcnTsMap ) ; {
static struct _ssBlkInfo2 _blkInfo2 ; struct _ssBlkInfo2 * blkInfo2 = &
_blkInfo2 ; ssSetBlkInfo2Ptr ( rts , blkInfo2 ) ; } { static struct
_ssPortInfo2 _portInfo2 ; struct _ssPortInfo2 * portInfo2 = & _portInfo2 ;
_ssSetBlkInfo2PortInfo2Ptr ( rts , portInfo2 ) ; } ssSetMdlInfoPtr ( rts ,
ssGetMdlInfoPtr ( rtS ) ) ; { static struct _ssSFcnModelMethods2 methods2 ;
ssSetModelMethods2 ( rts , & methods2 ) ; } { static struct
_ssSFcnModelMethods3 methods3 ; ssSetModelMethods3 ( rts , & methods3 ) ; } {
static struct _ssSFcnModelMethods4 methods4 ; ssSetModelMethods4 ( rts , &
methods4 ) ; } { static struct _ssStatesInfo2 statesInfo2 ; static
ssPeriodicStatesInfo periodicStatesInfo ; static ssJacobianPerturbationBounds
jacPerturbationBounds ; ssSetStatesInfo2 ( rts , & statesInfo2 ) ;
ssSetPeriodicStatesInfo ( rts , & periodicStatesInfo ) ;
ssSetJacobianPerturbationBounds ( rts , & jacPerturbationBounds ) ; } {
static struct _ssPortInputs inputPortInfo [ 2 ] ; _ssSetNumInputPorts ( rts ,
2 ) ; ssSetPortInfoForInputs ( rts , & inputPortInfo [ 0 ] ) ; { static
struct _ssInPortUnit inputPortUnits [ 2 ] ; _ssSetPortInfo2ForInputUnits (
rts , & inputPortUnits [ 0 ] ) ; } ssSetInputPortUnit ( rts , 0 , 0 ) ;
ssSetInputPortUnit ( rts , 1 , 0 ) ; { static struct _ssInPortCoSimAttribute
inputPortCoSimAttribute [ 2 ] ; _ssSetPortInfo2ForInputCoSimAttribute ( rts ,
& inputPortCoSimAttribute [ 0 ] ) ; } ssSetInputPortIsContinuousQuantity (
rts , 0 , 0 ) ; ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; { static
real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB . kmn2nxupno ;
ssSetInputPortSignalPtrs ( rts , 0 , ( InputPtrsType ) & sfcnUPtrs [ 0 ] ) ;
_ssSetInputPortNumDimensions ( rts , 0 , 1 ) ; ssSetInputPortWidth ( rts , 0
, 1 ) ; } { static real_T const * sfcnUPtrs [ 1 ] ; sfcnUPtrs [ 0 ] = & rtB .
l1d3sn10xu ; ssSetInputPortSignalPtrs ( rts , 1 , ( InputPtrsType ) &
sfcnUPtrs [ 0 ] ) ; _ssSetInputPortNumDimensions ( rts , 1 , 1 ) ;
ssSetInputPortWidth ( rts , 1 , 1 ) ; } } { static struct _ssPortOutputs
outputPortInfo [ 1 ] ; ssSetPortInfoForOutputs ( rts , & outputPortInfo [ 0 ]
) ; _ssSetNumOutputPorts ( rts , 1 ) ; { static struct _ssOutPortUnit
outputPortUnits [ 1 ] ; _ssSetPortInfo2ForOutputUnits ( rts , &
outputPortUnits [ 0 ] ) ; } ssSetOutputPortUnit ( rts , 0 , 0 ) ; { static
struct _ssOutPortCoSimAttribute outputPortCoSimAttribute [ 1 ] ;
_ssSetPortInfo2ForOutputCoSimAttribute ( rts , & outputPortCoSimAttribute [ 0
] ) ; } ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; {
_ssSetOutputPortNumDimensions ( rts , 0 , 1 ) ; ssSetOutputPortWidth ( rts ,
0 , 1 ) ; ssSetOutputPortSignal ( rts , 0 , ( ( real_T * ) & rtB . mvlgec43se
) ) ; } } ssSetModelName ( rts , "S-Function" ) ; ssSetPath ( rts ,
 "PV_Array_Model/Pac/Power (Positive-Sequence)/Sequence Analyzer1/Fourier_C/Mean/Model/Discrete Variable Time Delay/S-Function"
) ; if ( ssGetRTModel ( rtS ) == ( NULL ) ) { ssSetParentSS ( rts , rtS ) ;
ssSetRootSS ( rts , ssGetRootSS ( rtS ) ) ; } else { ssSetRTModel ( rts ,
ssGetRTModel ( rtS ) ) ; ssSetParentSS ( rts , ( NULL ) ) ; ssSetRootSS ( rts
, rts ) ; } ssSetVersion ( rts , SIMSTRUCT_VERSION_LEVEL2 ) ; { static
mxArray * sfcnParams [ 4 ] ; ssSetSFcnParamsCount ( rts , 4 ) ;
ssSetSFcnParamsPtr ( rts , & sfcnParams [ 0 ] ) ; ssSetSFcnParam ( rts , 0 ,
( mxArray * ) rtP . SFunction_P1_Size_gt1yv20ls2 ) ; ssSetSFcnParam ( rts , 1
, ( mxArray * ) rtP . SFunction_P2_Size_acjwavlpwb ) ; ssSetSFcnParam ( rts ,
2 , ( mxArray * ) rtP . SFunction_P3_Size_k0opnt4gpv ) ; ssSetSFcnParam ( rts
, 3 , ( mxArray * ) rtP . SFunction_P4_Size_luydhi20rk ) ; } ssSetRWork ( rts
, ( real_T * ) & rtDW . a2wvh3bg4y ) ; ssSetIWork ( rts , ( int_T * ) & rtDW
. at2mehttgr ) ; ssSetPWork ( rts , ( void * * ) & rtDW . poqnsuqeqv ) ; {
static struct _ssDWorkRecord dWorkRecord [ 3 ] ; static struct
_ssDWorkAuxRecord dWorkAuxRecord [ 3 ] ; ssSetSFcnDWork ( rts , dWorkRecord )
; ssSetSFcnDWorkAux ( rts , dWorkAuxRecord ) ; _ssSetNumDWork ( rts , 3 ) ;
ssSetDWorkWidth ( rts , 0 , 1 ) ; ssSetDWorkDataType ( rts , 0 , SS_DOUBLE )
; ssSetDWorkComplexSignal ( rts , 0 , 0 ) ; ssSetDWork ( rts , 0 , & rtDW .
a2wvh3bg4y ) ; ssSetDWorkWidth ( rts , 1 , 1 ) ; ssSetDWorkDataType ( rts , 1
, SS_INTEGER ) ; ssSetDWorkComplexSignal ( rts , 1 , 0 ) ; ssSetDWork ( rts ,
1 , & rtDW . at2mehttgr ) ; ssSetDWorkWidth ( rts , 2 , 1 ) ;
ssSetDWorkDataType ( rts , 2 , SS_POINTER ) ; ssSetDWorkComplexSignal ( rts ,
2 , 0 ) ; ssSetDWork ( rts , 2 , & rtDW . poqnsuqeqv ) ; }
sfun_discreteVariableDelay ( rts ) ; sfcnInitializeSizes ( rts ) ;
sfcnInitializeSampleTimes ( rts ) ; ssSetSampleTime ( rts , 0 ,
5.0505050505050505E-5 ) ; ssSetOffsetTime ( rts , 0 , 0.0 ) ; sfcnTsMap [ 0 ]
= 3 ; ssSetInputPortWidth ( rts , 0 , 1 ) ; ssSetInputPortDataType ( rts , 0
, SS_DOUBLE ) ; ssSetInputPortComplexSignal ( rts , 0 , 0 ) ;
ssSetInputPortFrameData ( rts , 0 , 0 ) ; ssSetInputPortUnit ( rts , 0 , 0 )
; ssSetInputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetInputPortWidth (
rts , 1 , 1 ) ; ssSetInputPortDataType ( rts , 1 , SS_DOUBLE ) ;
ssSetInputPortComplexSignal ( rts , 1 , 0 ) ; ssSetInputPortFrameData ( rts ,
1 , 0 ) ; ssSetInputPortUnit ( rts , 1 , 0 ) ;
ssSetInputPortIsContinuousQuantity ( rts , 1 , 0 ) ; ssSetOutputPortWidth (
rts , 0 , 1 ) ; ssSetOutputPortDataType ( rts , 0 , SS_DOUBLE ) ;
ssSetOutputPortComplexSignal ( rts , 0 , 0 ) ; ssSetOutputPortFrameData ( rts
, 0 , 0 ) ; ssSetOutputPortUnit ( rts , 0 , 0 ) ;
ssSetOutputPortIsContinuousQuantity ( rts , 0 , 0 ) ; ssSetNumIWork ( rts , 1
) ; ssSetNumPWork ( rts , 1 ) ; ssSetNumNonsampledZCs ( rts , 0 ) ;
_ssSetInputPortConnected ( rts , 0 , 1 ) ; _ssSetInputPortConnected ( rts , 1
, 1 ) ; _ssSetOutputPortConnected ( rts , 0 , 1 ) ;
_ssSetOutputPortBeingMerged ( rts , 0 , 0 ) ; ssSetInputPortBufferDstPort (
rts , 0 , - 1 ) ; ssSetInputPortBufferDstPort ( rts , 1 , - 1 ) ; } } return
rtS ; }
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif
const int_T gblParameterTuningTid = 4 ; void MdlOutputsParameterSampleTime (
int_T tid ) { MdlOutputsTID4 ( tid ) ; }
