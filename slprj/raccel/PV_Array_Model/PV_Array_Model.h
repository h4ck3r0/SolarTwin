#ifndef RTW_HEADER_PV_Array_Model_h_
#define RTW_HEADER_PV_Array_Model_h_
#include <stddef.h>
#include <string.h>
#include <float.h>
#include "rtw_modelmap.h"
#ifndef PV_Array_Model_COMMON_INCLUDES_
#define PV_Array_Model_COMMON_INCLUDES_
#include <stdlib.h>
#include "rtwtypes.h"
#include "simtarget/slSimTgtSigstreamRTW.h"
#include "simtarget/slSimTgtSlioCoreRTW.h"
#include "simtarget/slSimTgtSlioClientsRTW.h"
#include "simtarget/slSimTgtSlioSdiRTW.h"
#include "sigstream_rtw.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "rt_logging.h"
#include "dt_info.h"
#include "ext_work.h"
#endif
#include "PV_Array_Model_types.h"
#include "multiword_types.h"
#include "mwmathutil.h"
#include "rt_look.h"
#include "rt_look1d.h"
#include "rtGetInf.h"
#include "rt_defines.h"
#include "rt_nonfinite.h"
#define MODEL_NAME PV_Array_Model
#define NSAMPLE_TIMES (5) 
#define NINPUTS (0)       
#define NOUTPUTS (0)     
#define NBLOCKIO (397) 
#define NUM_ZC_EVENTS (0) 
#ifndef NCSTATES
#define NCSTATES (60)   
#elif NCSTATES != 60
#error Invalid specification of NCSTATES defined in compiler command
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm) (*rt_dataMapInfoPtr)
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val) (rt_dataMapInfoPtr = &val)
#endif
#ifndef IN_RACCEL_MAIN
#endif
typedef struct { real_T jgensia2ex ; real_T l2faoiogru ; } gj3ohc1kk3 ;
typedef struct { int8_T cyyxenso3u ; } oqxlv1t1or ; typedef struct { real_T
i0fvut4tar ; real_T k5tvpsreru ; } brzkpe5yck ; typedef struct { int8_T
nevvtwmofe ; } np452uldfz ; typedef struct { creal_T pcsvo2nso5 ; }
pij4s3w30k ; typedef struct { int8_T hrxuy113gk ; } baqwe1p1zs ; typedef
struct { creal_T exp35rwk2r ; } pjkmgaolpi ; typedef struct { int8_T
bwjj0lhfrh ; } lkl4gg004l ; typedef struct { creal_T o2kqm2z0ta [ 3 ] ;
real_T k2st2nj1lo [ 3 ] ; real_T pst4sv04lw [ 3 ] ; real_T iw2e3rkjnh ;
real_T nkff3onzga [ 3 ] ; real_T eizdwwjmoe [ 3 ] ; real_T icalmuobxj [ 3 ] ;
real_T pxnysvjhrw [ 3 ] ; real_T cqe0khjtcl ; real_T dekrkdeyqh [ 3 ] ;
real_T hxglutqeuf [ 3 ] ; real_T echkhwo04e [ 3 ] ; real_T pefvlkkdkt [ 3 ] ;
real_T me4ylfixyn [ 3 ] ; real_T hk1ldf3i0k [ 3 ] ; real_T ol2ictkqzg [ 3 ] ;
real_T btlpw3mp4z [ 3 ] ; real_T np40bymzcs [ 3 ] ; } go1ko1exrf ; typedef
struct { real_T fl0yqnilhd [ 3 ] ; real_T h54atg5cgd [ 3 ] ; struct { real_T
modelTStart ; } haoqh40cvy ; struct { real_T modelTStart ; } hwqhavqtmf ;
struct { void * TUbufferPtrs [ 6 ] ; } bdokffy1ag ; struct { void *
TUbufferPtrs [ 6 ] ; } lhvzznxbru ; struct { int_T Tail [ 3 ] ; int_T Head [
3 ] ; int_T Last [ 3 ] ; int_T CircularBufSize [ 3 ] ; int_T MaxNewBufSize ;
} ccgy3ccmth ; struct { int_T Tail [ 3 ] ; int_T Head [ 3 ] ; int_T Last [ 3
] ; int_T CircularBufSize [ 3 ] ; int_T MaxNewBufSize ; } e5x1ldeknm ; int8_T
gpfaohkthl ; boolean_T op4r1m20ns ; } ghzael4l32 ; typedef struct { real_T
e2h5nin1ya [ 3 ] ; real_T ndbmdbddqe [ 3 ] ; } e0vkhe1hid ; typedef struct {
real_T e2h5nin1ya [ 3 ] ; real_T ndbmdbddqe [ 3 ] ; } bvo2qlqek0 ; typedef
struct { boolean_T e2h5nin1ya [ 3 ] ; boolean_T ndbmdbddqe [ 3 ] ; }
mjc13vhdfp ; typedef struct { real_T e2h5nin1ya [ 3 ] ; real_T ndbmdbddqe [ 3
] ; } ftikiempm4 ; typedef struct { real_T e2h5nin1ya [ 3 ] ; real_T
ndbmdbddqe [ 3 ] ; } e1x0yaylwp ; typedef struct { real_T e2h5nin1ya [ 3 ] ;
real_T ndbmdbddqe [ 3 ] ; } p5w0u14fqb ; typedef struct { real_T a3xvaqx1gx [
3 ] ; real_T a52c55k23g [ 3 ] ; real_T lr13frcvpj ; real_T lqyqt2horm [ 3 ] ;
real_T gs4rvks0h4 [ 3 ] ; real_T jeg1ixfqcc [ 3 ] ; real_T d2yyti4lqk [ 3 ] ;
real_T gx0tigur0o [ 3 ] ; real_T mf4drh5hrb [ 3 ] ; real_T idrljgtxlp [ 3 ] ;
} pq3igreacf ; typedef struct { real_T kdzyu5dfoi [ 3 ] ; struct { real_T
modelTStart ; } ako5my0nxx ; struct { void * TUbufferPtrs [ 6 ] ; }
gwwponjnqb ; struct { int_T Tail [ 3 ] ; int_T Head [ 3 ] ; int_T Last [ 3 ]
; int_T CircularBufSize [ 3 ] ; int_T MaxNewBufSize ; } aeinmpqosl ; int_T
lxm3e5s4uc [ 3 ] ; int8_T oqyvh1ylxe ; int8_T hw0rx0e4r4 ; boolean_T
f2xlxmm2rg ; } o3cgxb1nm5 ; typedef struct { real_T mr3ym4eux3 [ 3 ] ; }
h1l3104bn0 ; typedef struct { real_T mr3ym4eux3 [ 3 ] ; } fyvjfwczbw ;
typedef struct { boolean_T mr3ym4eux3 [ 3 ] ; } hlxxn2qtq3 ; typedef struct {
real_T mr3ym4eux3 [ 3 ] ; } dhvw2avfam ; typedef struct { real_T mr3ym4eux3 [
3 ] ; } frubvdvih2 ; typedef struct { real_T mr3ym4eux3 [ 3 ] ; } pynob3xxsj
; typedef struct { real_T huwxhf2bl0 [ 3 ] ; real_T k0flqjs0k3 [ 3 ] ; }
c3runus3ia ; typedef struct { real_T o4gd0y3v5b [ 3 ] ; real_T g3hcemkbbs [ 3
] ; real_T dywo4uhhag [ 3 ] ; real_T atzxigju2m [ 3 ] ; real_T d0r0bfxxhv [ 3
] ; real_T hu3llba3ca [ 3 ] ; real_T oq3ipm0wus [ 3 ] ; real_T ntgaxamgwu [ 3
] ; real_T ft0dn4u15h [ 3 ] ; real_T bk3h34bboy [ 3 ] ; real_T ekwirjyast [ 3
] ; real_T aj3jiuqdac [ 3 ] ; real_T ikd1laxvge [ 3 ] ; real_T nj1powhokr [ 3
] ; real_T hjgv2ztqc5 [ 3 ] ; real_T leghz2ze52 [ 3 ] ; real_T hoxbun55q5 ;
real_T eif00tf41b ; real_T jrxr2bib5p ; real_T evntpqggjc ; real_T nspdhd04fa
; real_T fegphm11cq ; real_T j4jxwn4tn4 ; real_T hliocuqujf [ 46 ] ; real_T
iijvn1pij4 [ 18 ] ; real_T bsgixfioey ; real_T ozgmksrvpc ; real_T j40qpekti4
[ 3 ] ; real_T l4y5mcjvpy ; real_T jnimo5lv0m ; real_T fztulndu2v [ 2 ] ;
real_T crwergbsh2 ; real_T onqodwzqxe ; real_T icx34ifd2r ; real_T ihq1wxrrwv
; real_T ajjwqo0e1v ; real_T ad4rcuki4l ; real_T pbs5aqgeph [ 2 ] ; real_T
adjdefmwjg [ 2 ] ; real_T mtdom0eizr [ 3 ] ; real_T iajsx0ofcp [ 3 ] ; real_T
bhsqp0azai ; real_T j5nk5rin3j ; real_T el5lb42a5h [ 2 ] ; real_T om4giqthw2
[ 3 ] ; real_T luiagn315d [ 3 ] ; real_T azqgqqg4wd [ 2 ] ; real_T myfgavzlid
; real_T bstj0nioer ; real_T ebxa1ugh4c ; real_T jlybhz2t0i ; real_T
mhkfvkvx1u ; real_T acbdmg3uuv ; real_T jst3mmutj5 ; real_T edk4wtugyj ;
real_T pu0odtmlhy ; real_T mi3zbxppas [ 4 ] ; real_T ahenbhgxgu [ 4 ] ;
real_T en03atcxhu [ 4 ] ; real_T mqpcovqitm [ 3 ] ; real_T ar3wifnqiv ;
real_T lpiitgbbyf [ 3 ] ; real_T ec4eu1c34c ; real_T jjtdt14g1y ; real_T
oo5we1pw3n ; real_T krgk34ykj1 ; real_T mkrkhji0cg ; real_T ckmd25fctl ;
real_T k0men4hr1r ; real_T esjg3pcq0t ; real_T l12i0pjkca ; real_T ni2s4rtms3
; real_T b11aaunedj ; real_T a0vdijiybg ; real_T o0xbrdncoc ; real_T
bzovtsvol2 ; real_T lvxda1zemz ; real_T d0gp45izbd ; real_T ea14lgpbfy ;
real_T fu3rpol0ph ; real_T lntmbixtgk ; real_T hdh2j1iy05 ; real_T f1i4vqi2kd
; real_T nvqp25eoum ; real_T hglrrwzja3 ; real_T f4wsdyxsap ; real_T
psu4dh2yoj ; real_T on2hc5txnr ; real_T ccqwtokflg ; real_T hxvph3vgfk ;
real_T aeqligfyi1 ; real_T bxlm4t4z32 ; real_T fj2bifvg5t ; real_T klgyhsq3yy
; real_T hiw0yqrayw ; real_T lqaoyuj05s ; real_T a2i2ov02lb ; real_T
ienudooawn ; real_T fbyhptghi3 ; real_T hxdguojmgq ; real_T eutj1tzkyf ;
real_T pf0efcoh3z ; real_T hqbivcjz0k ; real_T atf0ppnipt ; real_T jf3zbnuh3n
; real_T f4bvvydmna ; real_T fvjbmcpghe ; real_T nvcc0bievu ; real_T
m4cw5jelgb ; real_T e3r3gwb15i ; real_T b4ioytjw1o ; real_T gmfn04oapy ;
real_T dhrszd2dqf ; real_T g5q20xpjnp ; real_T b24n3f4xp0 ; real_T pddsnfsfnu
; real_T ilbet5yid2 ; real_T fbe5ufjhz0 ; real_T bjoxavkoai ; real_T
lk335slscy ; real_T cyaarq5wc5 ; real_T nabvxanmoe ; real_T djupb53czf ;
real_T g5azq2cdjs ; real_T fisyjtz0ao ; real_T mxfhajo1pf ; real_T inbgwouqfx
; real_T jrohv3tr0m ; real_T jiw5saclae ; real_T jdnb2gbavk ; real_T
kmn2nxupno ; real_T l1d3sn10xu ; real_T mvlgec43se ; real_T ltugjcg352 ;
real_T mhldlmlbft ; real_T kzukygsbbq [ 3 ] ; real_T etkri0io2o [ 3 ] ;
real_T dtnbe2rzvu ; real_T hxgrahbcuh ; real_T kuplk1gg1z [ 3 ] ; real_T
ffpomxcrhj [ 3 ] ; real_T ibnjsgargl [ 3 ] ; real_T o2gpo0dp4u ; real_T
mnzoxhxi2g [ 3 ] ; real_T jeu3urzybv [ 3 ] ; real_T b3h2v3br2u [ 3 ] ; real_T
krzzt4ow11 [ 3 ] ; real_T bj5d1sf254 ; real_T dw3pchqeno [ 3 ] ; real_T
j4eo4uftcs [ 3 ] ; real_T oryb5w3imi [ 3 ] ; real_T gaih2ptcqs [ 3 ] ; real_T
gctmfhb3tj [ 3 ] ; real_T hs4xhfhuu4 [ 3 ] ; real_T bkfh2vcfmq [ 3 ] ; real_T
esekxgdj1v [ 3 ] ; real_T gn420ui3bc [ 3 ] ; real_T a3oqmvvhoe [ 3 ] ; real_T
bud3z3gi3t [ 3 ] ; real_T fldwassvbh ; real_T dr1q3vvfez ; real_T pgwb1j4u3n
; real_T b3gkmlvhtj ; real_T kxfvw5uauv ; real_T acbfsgne1t ; real_T
atazzkiqtz ; real_T mqwfucjhw5 ; real_T kqp2cunbno ; real_T lhrhljsi0l [ 3 ]
; real_T iqsu5vb0qp ; real_T esbgstqc1k ; real_T eeusicyxwo [ 2 ] ; real_T
bzadnafn2c ; real_T jvbkza0bug ; real_T chkgbobqc5 ; real_T aggnsxdhid ;
real_T as2m1qy4ku ; real_T gndfpjrzbt ; real_T dlvjgbn4i5 ; real_T bsrowihmvj
; real_T puricsu5uo ; real_T khgh0in0mh [ 3 ] ; real_T kve2c32fv4 [ 3 ] ;
real_T kk5fvlgob4 [ 3 ] ; uint8_T jc4x0snbqf ; uint8_T fs14jhs4t5 ; uint8_T
pkbwlncsj5 ; uint8_T hdzltqw2sw ; uint8_T ndqy5pufgt ; uint8_T isub4roeyp ;
uint8_T e01ed5cv3s ; uint8_T oslgokdckt ; boolean_T gjbagadbk5 ; boolean_T
jasinaajz3 ; boolean_T mjlsry1omh ; boolean_T ocp2ppapt1 ; boolean_T
nkxqxwgteh ; boolean_T fk5vinpzuf ; boolean_T emd0mf5yqm ; boolean_T
n3g3acn3kz ; boolean_T oamr1wipoz ; boolean_T c5pw4pqcew ; boolean_T
odjvodg0jz ; boolean_T atdnrefilq ; boolean_T nij3ckqdiv ; boolean_T
p5irdmbgnc ; boolean_T cje2imeoha ; pq3igreacf daw0ymwmw4 ; go1ko1exrf
pu1c2mp3al ; pq3igreacf glvlabfrtp ; go1ko1exrf drtk0acyhp ; pq3igreacf
blrugcnivv ; go1ko1exrf iks0grgus1 ; pq3igreacf jhi4zkfadx ; go1ko1exrf
mdskmorhmi ; pq3igreacf e2mpwvt2ua ; go1ko1exrf oa2z5wykl2 ; pq3igreacf
gllp1vimnhl ; go1ko1exrf kyedbo0mxuc ; pjkmgaolpi cxxbka35es ; pij4s3w30k
h1nhkvogk5 ; pij4s3w30k drzlaqk2wl ; pjkmgaolpi ihqnx345xuh ; pij4s3w30k
jr3gbt2o32 ; pij4s3w30k gj0ekbre2up ; brzkpe5yck ibrtt4ftsl ; gj3ohc1kk3
bdvvkdwqfm ; brzkpe5yck imgy1z4qes ; gj3ohc1kk3 gusdfzbcqa ; brzkpe5yck
gvjufizpfu ; gj3ohc1kk3 h0t1xli1qx ; brzkpe5yck flkfnl3dquj ; gj3ohc1kk3
fiy4znthvv3 ; } B ; typedef struct { real_T jreglfqlm5 ; real_T hllwh2h1oo [
34 ] ; real_T m1peilszpy ; real_T dnrqy23bwk ; real_T jzbmotzdka [ 2 ] ;
real_T jv0y1evb0u [ 3 ] ; real_T giqv4pyyys ; real_T mwfdpj20rn ; real_T
m5fkweyhnz ; real_T oyi34irdce ; real_T fmpdeay2dq ; real_T evobiqhvg0 ;
real_T c3rg43fwng ; real_T puez01wvqo ; real_T otc3kwibnn ; real_T inc2xczn1j
; real_T hglk0ykmnp ; real_T iyz3blnd0r ; real_T okoa0ga0es ; real_T
mlqfus2yxb ; real_T cv44xoew41 ; real_T amx5pvlykq ; real_T l1nmdmtivq ;
real_T c3vwiq1oz1 ; real_T g55lsvsoyn ; real_T bf0dooycmi ; real_T is34tmu21f
; real_T bgndzrawrf ; real_T jwtcvtmaeo ; real_T crf5cecvju ; real_T
o54g2pguii ; real_T berzbzjptr ; real_T eyj3xgyyy5 ; real_T iwb0lp4dat ;
real_T mmleedibcx ; real_T iwtvu1xbzm ; real_T fdgmxn5rpv ; real_T mkbsymk41p
; real_T h4kspo12ff ; real_T ad2sn3k3gj ; real_T jy2blxntru ; real_T
dbrynerjdo ; real_T ioxogrdks0 ; real_T ejhppynn0z ; real_T dqddocajhg ;
real_T e2jfcom1xu ; real_T km0mzzn3gj ; real_T llg10jep1x ; real_T kkj5lgh0sd
; real_T oersb5zkry ; real_T mxwbciposz ; real_T mvp44fdh2o ; real_T
fwzzbi3hli ; real_T pjo1bnuqfk ; real_T h45pdgkmki ; real_T ldxbwl52qd ;
real_T lzgy3px31z ; real_T omufifkqbz ; real_T nndlftph12 ; real_T ohydhrpaay
; real_T eznk4sr03e ; real_T aktl42ci5l ; real_T ca3dtz0zaw ; real_T
a1004yerwl ; real_T i5irbkj5h1 ; real_T dqdihrjufk ; real_T fnoqoswl0l ;
real_T l31gaczqpr ; real_T eelkc34vvy ; real_T koobssrco0 ; real_T aswhfhtphz
; real_T n3oafbzym3 [ 3 ] ; real_T mec1csjpdt [ 3 ] ; real_T ev3w2qbtzq ;
real_T lm3x1szvf2 ; real_T fnwtwy11ey ; real_T hjvxevoa1u ; real_T bo3u2joydk
; real_T d2b1q3eao5 ; real_T lqckuso1xq ; real_T jrdmpel4ku ; real_T
pmdevl30t5 ; real_T bfqdux1v4y ; real_T phb5tgbsae ; real_T cbutkiwxs3 ;
real_T e4oo4zzsto ; real_T ni0wsgyutj ; real_T gdfrdi2cqi ; real_T lanv5zwkwv
; real_T his1pld4hz ; real_T arh00ntoml ; real_T birk1htcrc ; real_T
pcj40bs3ff ; real_T l54ofk2ea3 ; real_T bl13n2tmiw ; real_T dpc4svbkri ;
real_T nlvv2lvgnj ; real_T dpquh1vse1 [ 3 ] ; real_T mvp3sao44j [ 3 ] ;
real_T k2yf4d0qko ; real_T j0fknedvcm ; real_T h1cmwk4uvu ; real_T afeqr4soi1
; real_T eljq3fss0e ; real_T merlacocwu ; real_T cnqpt10soe ; real_T
bnggfcupbm ; real_T pd03h3b1tv ; real_T i0ikhfczlc ; real_T mh2g3u0j0y ;
real_T i3mgxdl4vo ; real_T hyi1vb4nfg ; real_T fztzuxh3fu ; real_T m4n3n1vgn4
; real_T j3wumogm4s ; real_T abuos4nqex ; real_T ey4kijxpsz ; real_T
kktgq5zogl ; real_T nzmhyhlryd ; real_T f2lkfuuga3 ; real_T ebzux3ox5l ;
real_T fqeweas2gy ; real_T pj4mixmqsa ; real_T jvbe5wkg5h ; real_T a2wvh3bg4y
; struct { real_T modelTStart ; } clnu2343zl ; struct { real_T modelTStart ;
} kfccid5jx1 ; real_T g0q3b3bwh1 ; real_T pmrorw1f23 ; void * bcokcp0qkj [ 3
] ; void * b1c14x4npm [ 3 ] ; void * cevsb2sslr [ 3 ] ; void * lbbf2yann5 [ 3
] ; void * bsj5iiu43b [ 3 ] ; void * abc1w5vdl5 [ 3 ] ; void * fawbkmqhtu [ 3
] ; void * eztv3pazqo [ 3 ] ; struct { void * AS ; void * BS ; void * CS ;
void * DS ; void * DX_COL ; void * BD_COL ; void * TMP1 ; void * TMP2 ; void
* XTMP ; void * SWITCH_STATUS ; void * SWITCH_STATUS_INIT ; void * SW_CHG ;
void * G_STATE ; void * USWLAST ; void * XKM12 ; void * XKP12 ; void * XLAST
; void * ULAST ; void * IDX_SW_CHG ; void * Y_SWITCH ; void * SWITCH_TYPES ;
void * IDX_OUT_SW ; void * SWITCH_TOPO_SAVED_IDX ; void * SWITCH_MAP ; }
o1xie1rqxt ; struct { void * LoggedData [ 2 ] ; } lu5qplaf54 ; struct { void
* LoggedData ; } jlre5nll0u ; void * iiu4wr0f5r ; void * aknzofkqx0 ; void *
c4xrejr2sm ; struct { void * LoggedData [ 3 ] ; } bb5r4yckji ; void *
el4u4uq5nb ; void * mkajacfjkh ; void * cljmzrmfcy ; void * ga0gkvh2kb ; void
* bb1zyrk33s ; void * otbq3qatfg ; void * kmczgf120y ; void * ch4lkuxrcg ;
void * bekhzyaoss ; void * hib1rdb5vn ; void * fqibuqwt5r ; void * poqnsuqeqv
; struct { void * LoggedData ; } cy1zkvxnkp ; struct { void * LoggedData [ 2
] ; } jquzhmx440 ; struct { void * TUbufferPtrs [ 6 ] ; } lfqiakux1j ; struct
{ void * TUbufferPtrs [ 6 ] ; } chf5u4x2og ; struct { void * LoggedData [ 3 ]
; } hosf0kcusx ; struct { void * LoggedData [ 2 ] ; } avraaemnu5 ; struct {
void * LoggedData ; } kkxmjx1dax ; struct { void * LoggedData ; } iek4salud1
; struct { void * LoggedData ; } eugzkgt4ls ; struct { void * LoggedData ; }
bcg2n1qxu5 ; void * by3miegpcz ; void * ni0rxjpduc ; int32_T jyxrya54ck ;
int32_T kc2urh0o2k ; int32_T gwu3cfmboo ; int32_T gu2gnqzjnr ; int32_T
oi4s4s3b2v ; int32_T bysaiwysza ; int32_T fp5gljg1na ; int32_T f05y3hvfbl ;
int32_T nzuplyapo4 ; int32_T aaw5cbewzs ; int32_T fhmti5fnmk ; int32_T
ofkxr3wbpq ; int32_T lmxa3ssfz4 ; int32_T kso0zmov5s ; int32_T gqdl0mj0xk ;
int32_T bn3v01mqq4 ; uint32_T h4meyfugs1 ; int_T pazrwyzcfm [ 3 ] ; int_T
jtitiatpbl [ 3 ] ; int_T ak5hmrpbyv [ 3 ] ; int_T m1diazv1pn [ 3 ] ; int_T
cc3bay3gng [ 3 ] ; int_T ndlf3lnho1 [ 3 ] ; int_T piwvjcyewx [ 3 ] ; int_T
gzluk1ehd1 [ 3 ] ; int_T ph5l2vm5l0 [ 11 ] ; int_T bhpkupwt0n ; int_T
hhdyt0quta ; int_T mzcyzi1mrw ; int_T lunlsisyew ; int_T bdjx3fnp20 ; int_T
hssr4w0h5w ; int_T l1mpdcz1eb ; int_T pkxv552qte ; int_T ejb25uem32 ; int_T
cyjecukgkv ; int_T mpcathptum ; int_T nvvegqclb5 ; int_T gbcxxqxjpd ; int_T
bb2ftcsloy ; int_T at2mehttgr ; struct { int_T Tail [ 3 ] ; int_T Head [ 3 ]
; int_T Last [ 3 ] ; int_T CircularBufSize [ 3 ] ; int_T MaxNewBufSize ; }
drhtugugfl ; struct { int_T Tail [ 3 ] ; int_T Head [ 3 ] ; int_T Last [ 3 ]
; int_T CircularBufSize [ 3 ] ; int_T MaxNewBufSize ; } nw5suzqpq2 ; int_T
a4vswgnpt2 ; int_T nv33qldkfr ; int8_T bvgqcazfkc ; int8_T flcrf2pa4r ;
int8_T mv3mqzgj3o ; int8_T b43mt2kku5 ; int8_T le4iyy3q51 ; int8_T ntzroniten
; int8_T cpk3jnibze ; int8_T b3ha3knqwy ; int8_T nitdz0i0m1 ; uint8_T
oneka2f33e ; uint8_T fcwaufktro ; uint8_T oaicnmy4cr ; uint8_T d5srsnrycq ;
uint8_T acdm1beais ; uint8_T i4s40mrg1p ; uint8_T e4uypnvxm4 ; uint8_T
leakran3jy ; uint8_T grw0tq5110 ; uint8_T ir0a11kmwf ; uint8_T bixjjcy32b ;
uint8_T lob2ph3yhu ; uint8_T fuhanhc4z4 ; uint8_T phos00jdib ; uint8_T
ckxcx1vdsj ; uint8_T lpm5mugxt5 ; uint8_T nymp3uolcg ; boolean_T mktm0brmw5 ;
boolean_T c1tw4ogoog ; boolean_T iy4nvvktj2 ; o3cgxb1nm5 daw0ymwmw4 ;
ghzael4l32 pu1c2mp3al ; o3cgxb1nm5 glvlabfrtp ; ghzael4l32 drtk0acyhp ;
o3cgxb1nm5 blrugcnivv ; ghzael4l32 iks0grgus1 ; o3cgxb1nm5 jhi4zkfadx ;
ghzael4l32 mdskmorhmi ; o3cgxb1nm5 e2mpwvt2ua ; ghzael4l32 oa2z5wykl2 ;
o3cgxb1nm5 gllp1vimnhl ; ghzael4l32 kyedbo0mxuc ; lkl4gg004l cxxbka35es ;
baqwe1p1zs h1nhkvogk5 ; baqwe1p1zs drzlaqk2wl ; lkl4gg004l ihqnx345xuh ;
baqwe1p1zs jr3gbt2o32 ; baqwe1p1zs gj0ekbre2up ; np452uldfz ibrtt4ftsl ;
oqxlv1t1or bdvvkdwqfm ; np452uldfz imgy1z4qes ; oqxlv1t1or gusdfzbcqa ;
np452uldfz gvjufizpfu ; oqxlv1t1or h0t1xli1qx ; np452uldfz flkfnl3dquj ;
oqxlv1t1or fiy4znthvv3 ; } DW ; typedef struct { real_T f2y4dmvcyy [ 3 ] ;
real_T nv45bjhe3t [ 3 ] ; h1l3104bn0 daw0ymwmw4 ; e0vkhe1hid pu1c2mp3al ;
h1l3104bn0 glvlabfrtp ; e0vkhe1hid drtk0acyhp ; h1l3104bn0 blrugcnivv ;
e0vkhe1hid iks0grgus1 ; h1l3104bn0 jhi4zkfadx ; e0vkhe1hid mdskmorhmi ;
h1l3104bn0 e2mpwvt2ua ; e0vkhe1hid oa2z5wykl2 ; h1l3104bn0 gllp1vimnhl ;
e0vkhe1hid kyedbo0mxuc ; } X ; typedef struct { real_T f2y4dmvcyy [ 3 ] ;
real_T nv45bjhe3t [ 3 ] ; fyvjfwczbw daw0ymwmw4 ; bvo2qlqek0 pu1c2mp3al ;
fyvjfwczbw glvlabfrtp ; bvo2qlqek0 drtk0acyhp ; fyvjfwczbw blrugcnivv ;
bvo2qlqek0 iks0grgus1 ; fyvjfwczbw jhi4zkfadx ; bvo2qlqek0 mdskmorhmi ;
fyvjfwczbw e2mpwvt2ua ; bvo2qlqek0 oa2z5wykl2 ; fyvjfwczbw gllp1vimnhl ;
bvo2qlqek0 kyedbo0mxuc ; } XDot ; typedef struct { boolean_T f2y4dmvcyy [ 3 ]
; boolean_T nv45bjhe3t [ 3 ] ; hlxxn2qtq3 daw0ymwmw4 ; mjc13vhdfp pu1c2mp3al
; hlxxn2qtq3 glvlabfrtp ; mjc13vhdfp drtk0acyhp ; hlxxn2qtq3 blrugcnivv ;
mjc13vhdfp iks0grgus1 ; hlxxn2qtq3 jhi4zkfadx ; mjc13vhdfp mdskmorhmi ;
hlxxn2qtq3 e2mpwvt2ua ; mjc13vhdfp oa2z5wykl2 ; hlxxn2qtq3 gllp1vimnhl ;
mjc13vhdfp kyedbo0mxuc ; } XDis ; typedef struct { real_T f2y4dmvcyy [ 3 ] ;
real_T nv45bjhe3t [ 3 ] ; dhvw2avfam daw0ymwmw4 ; ftikiempm4 pu1c2mp3al ;
dhvw2avfam glvlabfrtp ; ftikiempm4 drtk0acyhp ; dhvw2avfam blrugcnivv ;
ftikiempm4 iks0grgus1 ; dhvw2avfam jhi4zkfadx ; ftikiempm4 mdskmorhmi ;
dhvw2avfam e2mpwvt2ua ; ftikiempm4 oa2z5wykl2 ; dhvw2avfam gllp1vimnhl ;
ftikiempm4 kyedbo0mxuc ; } CStateAbsTol ; typedef struct { real_T f2y4dmvcyy
[ 3 ] ; real_T nv45bjhe3t [ 3 ] ; frubvdvih2 daw0ymwmw4 ; e1x0yaylwp
pu1c2mp3al ; frubvdvih2 glvlabfrtp ; e1x0yaylwp drtk0acyhp ; frubvdvih2
blrugcnivv ; e1x0yaylwp iks0grgus1 ; frubvdvih2 jhi4zkfadx ; e1x0yaylwp
mdskmorhmi ; frubvdvih2 e2mpwvt2ua ; e1x0yaylwp oa2z5wykl2 ; frubvdvih2
gllp1vimnhl ; e1x0yaylwp kyedbo0mxuc ; } CXPtMin ; typedef struct { real_T
f2y4dmvcyy [ 3 ] ; real_T nv45bjhe3t [ 3 ] ; pynob3xxsj daw0ymwmw4 ;
p5w0u14fqb pu1c2mp3al ; pynob3xxsj glvlabfrtp ; p5w0u14fqb drtk0acyhp ;
pynob3xxsj blrugcnivv ; p5w0u14fqb iks0grgus1 ; pynob3xxsj jhi4zkfadx ;
p5w0u14fqb mdskmorhmi ; pynob3xxsj e2mpwvt2ua ; p5w0u14fqb oa2z5wykl2 ;
pynob3xxsj gllp1vimnhl ; p5w0u14fqb kyedbo0mxuc ; } CXPtMax ; typedef struct
{ c3runus3ia daw0ymwmw4 ; c3runus3ia glvlabfrtp ; c3runus3ia blrugcnivv ;
c3runus3ia jhi4zkfadx ; c3runus3ia e2mpwvt2ua ; c3runus3ia gllp1vimnhl ; }
ZCV ; typedef struct { rtwCAPI_ModelMappingInfo mmi ; } DataMapInfo ; struct
fjazhlmyos_ { real_T dq_Y0 [ 2 ] ; } ; struct fcezepw4oj_ { real_T dq_Y0 [ 2
] ; } ; struct m3uhrpfyin_ { real_T Gain3_Gain ; creal_T Gain1_Gain [ 3 ] ; }
; struct pneeuxrdyc_ { real_T Gain3_Gain ; } ; struct pcr2y0buvk_ { real_T
Gain_Gain ; real_T Gain_Gain_nx51kbuuf3 ; real_T integrator_IC ; real_T
TransportDelay_Delay ; real_T TransportDelay_InitOutput ; real_T K1_Value ;
real_T Memory_InitialCondition ; real_T integrator_IC_lexuvhoanq ; real_T
TransportDelay_Delay_ftnitesb5l ; real_T TransportDelay_InitOutput_gwyp41kcsd
; real_T K1_Value_ky41retfs4 ; real_T Memory_InitialCondition_iskxsdaa2y ;
real_T sinwt_Amp ; real_T sinwt_Bias ; real_T sinwt_Freq ; real_T sinwt_Phase
; real_T coswt_Amp ; real_T coswt_Bias ; real_T coswt_Freq ; real_T
coswt_Phase ; real_T Gain_Gain_gq3qylxz5y ; } ; struct klg0lcyohc_ { real_T
Gain_Gain ; real_T integrator_IC ; real_T TransportDelay_Delay ; real_T
TransportDelay_InitOutput ; real_T K1_Value ; real_T Memory_InitialCondition
; real_T Saturationtoavoidnegativesqrt_UpperSat ; real_T
Saturationtoavoidnegativesqrt_LowerSat ; } ; struct P_ { real_T Fnom ; real_T
Increment_MPPT ; real_T Ki_Ireg ; real_T Ki_VDCreg ; real_T Kp_Ireg ; real_T
Kp_VDCreg ; real_T LimitL_Ireg ; real_T LimitL_VDCreg ; real_T LimitU_Ireg ;
real_T LimitU_VDCreg ; real_T Pnom ; real_T TempC ; real_T Ts_Control ;
real_T Vnom_dc ; real_T Vnom_prim ; real_T Vnom_sec ; real_T
AlphaBetaZerotodq0_Alignment ; real_T AlphaBetaZerotodq0_Alignment_mnakfv4gd4
; real_T AlphaBetaZerotodq0_Alignment_otrg0yyt1a ; real_T
AlphaBetaZerotodq0_Alignment_awofvyee55 ; real_T Discrete_Init ; real_T
PI_InitialConditionForIntegrator ; real_T
PI_InitialConditionForIntegrator_bmolgnst2b ; real_T Discrete_Kd ; real_T
Discrete_Kp ; real_T PVArray_Npar ; real_T Overmodulation_OverModType ;
real_T RMS_TrueRMS ; real_T RMS1_TrueRMS ; real_T RMS1_TrueRMS_kcqxfunc0w ;
real_T RMS2_TrueRMS ; real_T RMS1_TrueRMS_jywwajkz1m ; real_T
RMS2_TrueRMS_beyrfxdip3 ; real_T CompareToConstant_const ; real_T
CompareToConstant1_const ; real_T CompareToConstant_const_mjeo4kqpve ; real_T
CompareToConstant1_const_lkgpibc3kf ; real_T
CompareToConstant_const_f3j21z25kv ; real_T
CompareToConstant1_const_m1hj2kgrib ; real_T
CompareToConstant_const_ork22hwfpx ; real_T
CompareToConstant1_const_mofh5awlq5 ; real_T Uref_Y0 ; real_T DeadZone_Start
; real_T DeadZone_End ; real_T Gain1_Gain ; real_T Uref_Y0_jnwoqle1jv ;
real_T Gain1_Gain_oucfv1ov0d ; real_T Uref_Y0_dmfzsftjfi ; real_T Gain3_Gain
[ 9 ] ; real_T Gain1_Gain_ex3qxbdqxn ; real_T Gain6_Gain ; real_T Gain7_Gain
; real_T Constant_Value [ 3 ] ; real_T Gain1_Gain_cmdba5bv2h ; real_T
Gain1_Gain_hetxdadbk5 ; real_T Gain_Y0 ; real_T Gain3_Gain_fs2dzj0rbm [ 9 ] ;
real_T Gain1_Gain_mv4azmob4e ; real_T Integ4_gainval ; real_T Integ4_IC ;
real_T Toavoiddivisionbyzero_UpperSat ; real_T Toavoiddivisionbyzero_LowerSat
; real_T SFunction_P1_Size [ 2 ] ; real_T SFunction_P1 ; real_T
SFunction_P2_Size [ 2 ] ; real_T SFunction_P2 ; real_T SFunction_P3_Size [ 2
] ; real_T SFunction_P3 ; real_T SFunction_P4_Size [ 2 ] ; real_T
SFunction_P4 ; real_T UnitDelay_InitialCondition ; real_T
UnitDelay1_InitialCondition ; real_T Integ4_gainval_fz2uqdiylj ; real_T
Integ4_IC_bmrwd2awok ; real_T Toavoiddivisionbyzero_UpperSat_cysl1cmul3 ;
real_T Toavoiddivisionbyzero_LowerSat_k1vxw44tbr ; real_T
SFunction_P1_Size_gcwln5h2fy [ 2 ] ; real_T SFunction_P1_bvoxvxdyd0 ; real_T
SFunction_P2_Size_cja0aqxvcz [ 2 ] ; real_T SFunction_P2_e4zetzg3tb ; real_T
SFunction_P3_Size_av3rmjmb5f [ 2 ] ; real_T SFunction_P3_dzoprv4wqa ; real_T
SFunction_P4_Size_hngc2st1ax [ 2 ] ; real_T SFunction_P4_en5y5az0mt ; real_T
UnitDelay_InitialCondition_jycnfiyaap ; real_T
UnitDelay1_InitialCondition_eoex0bhdpa ; real_T Saturation_UpperSat ; real_T
Saturation_LowerSat ; real_T Constant_Value_l4lyjwd23b ; real_T
Constant_Value_jccahizvsu ; real_T Gain1_Gain_afd104gjl4 ; real_T uib1_Gain ;
real_T uDLookupTable_tableData [ 3 ] ; real_T uDLookupTable_bp01Data [ 3 ] ;
real_T S_Y0 [ 2 ] ; real_T Gain_Gain ; real_T S_Y0_o3ff3poqj5 ; real_T
Gain_Gain_er1jzlt204 ; real_T Gain1_Gain_cvnj41z3u1 ; real_T
Gain_Gain_d5ujhlzr3p ; real_T Gain1_Gain_gau2kona2s ; real_T
Gain_Gain_l1yybb0ozh ; real_T Gain1_Gain_jawtsd2iys ; real_T
Gain_Gain_jq3um4cz4j ; real_T Gain1_Gain_bsbbtjbk4d ; real_T
Gain_Gain_fc1p3rfcsm ; real_T Gain1_Gain_kl3n41qkg4 ; real_T
Gain_Gain_o1r20n5vmz ; real_T Gain1_Gain_lly5kv4kuf ; real_T
Gain_Gain_pvpuvbbodz ; real_T Gain1_Gain_h0j53ikn03 ; real_T
Gain_Gain_htrn2od5eo ; real_T Gain1_Gain_k45yw5u1ri ; real_T
Gain_Gain_nxlojnsi3h ; real_T Gain1_Gain_ck1bqs4xph ; real_T
Gain_Gain_c51ds1jfln ; real_T Gain1_Gain_aqpmhpqydp ; real_T
Gain_Gain_afzupnanth ; real_T Gain1_Gain_f5mqebcvsk ; real_T
Gain_Gain_a2j2ruaijq ; real_T Gain1_Gain_phi4msvodo ; real_T
Gain_Gain_ikrhtetg1c ; real_T Gain1_Gain_lvijrrmj2x ; real_T
Gain_Gain_mrwtozpa1q ; real_T Gain1_Gain_cygkj45s0o ; real_T
Gain_Gain_evychckliv ; real_T Gain_Gain_a1f4gpqc0r ; real_T
Constant_Value_jsrxor425n [ 3 ] ; real_T SFunction_P1_Size_eb5a0ck1sr [ 2 ] ;
real_T SFunction_P1_d4zvpebekt ; real_T SFunction_P2_Size_a0f30lilzp [ 2 ] ;
real_T SFunction_P2_j0up2juwmt ; real_T SFunction_P3_Size_ihynnzuoja [ 2 ] ;
real_T SFunction_P3_n2uuziyjeu [ 36 ] ; real_T SFunction_P4_Size_edzxkjjlwn [
2 ] ; real_T SFunction_P4_l1zkwiwk5m ; real_T uhZ_Gain [ 3 ] ; real_T
Constant1_Value [ 3 ] ; real_T SFunction_P1_Size_oayvoucrma [ 2 ] ; real_T
SFunction_P1_cnuwuclk1h ; real_T SFunction_P2_Size_f05fppxlwk [ 2 ] ; real_T
SFunction_P2_gfvpcxfqr0 ; real_T SFunction_P3_Size_hnxggd2vpu [ 2 ] ; real_T
SFunction_P3_beheipinfh [ 36 ] ; real_T SFunction_P4_Size_kl1dwgtupy [ 2 ] ;
real_T SFunction_P4_pcop4is5he ; real_T Gain6_Gain_cnv0nkvg1u [ 3 ] ; real_T
SFunction_P1_Size_lxdwb50jvc [ 2 ] ; real_T SFunction_P1_pgusngghgc ; real_T
SFunction_P2_Size_jijo4kujdd [ 2 ] ; real_T SFunction_P2_lzrwmhleid ; real_T
SFunction_P3_Size_dhi0mtqkva [ 2 ] ; real_T SFunction_P3_gs3moriuin [ 36 ] ;
real_T SFunction_P4_Size_b0bt51nryg [ 2 ] ; real_T SFunction_P4_hmyubalaq2 ;
real_T SFunction_P1_Size_ojey0fic3d [ 2 ] ; real_T SFunction_P1_e41noykgtc ;
real_T SFunction_P2_Size_nkfz30iz0g [ 2 ] ; real_T SFunction_P2_hjocdlxywu ;
real_T SFunction_P3_Size_gsuuzkedwl [ 2 ] ; real_T SFunction_P3_icxvv3ejvr [
36 ] ; real_T SFunction_P4_Size_n44xdy2tit [ 2 ] ; real_T
SFunction_P4_pn4jfd03mw ; real_T uhZ_Gain_lwvpl3ylzi [ 3 ] ; real_T
Gain3_Gain_otnifcmile [ 3 ] ; real_T mode2phaseS_Gain [ 9 ] ; real_T
Gain4_Gain [ 3 ] ; real_T Gain5_Gain [ 3 ] ; real_T mode2phaseR_Gain [ 9 ] ;
real_T Constant_Value_g4zd2j3sx3 [ 3 ] ; real_T SFunction_P1_Size_iox5d4w0ir
[ 2 ] ; real_T SFunction_P1_iicnccg4gn ; real_T SFunction_P2_Size_glmkyfxahk
[ 2 ] ; real_T SFunction_P2_ejphgkvcyi ; real_T SFunction_P3_Size_f4azrwnmre
[ 2 ] ; real_T SFunction_P3_fz12u3oyfm [ 21 ] ; real_T
SFunction_P4_Size_ljktgvqp4f [ 2 ] ; real_T SFunction_P4_butbfuse0u ; real_T
uhZ_Gain_hoxhm3zthg [ 3 ] ; real_T Constant1_Value_ay2jtmeeno [ 3 ] ; real_T
SFunction_P1_Size_fh43eahwlo [ 2 ] ; real_T SFunction_P1_bd3mjqmvpe ; real_T
SFunction_P2_Size_jt15ntmltf [ 2 ] ; real_T SFunction_P2_nszfpevyas ; real_T
SFunction_P3_Size_ptdahzgl4q [ 2 ] ; real_T SFunction_P3_i0zrk3x3rw [ 21 ] ;
real_T SFunction_P4_Size_jnuso5m51u [ 2 ] ; real_T SFunction_P4_pody3jbyfn ;
real_T Gain6_Gain_mkgiv5ecem [ 3 ] ; real_T SFunction_P1_Size_hvkdhrhi0y [ 2
] ; real_T SFunction_P1_emkjctw1zb ; real_T SFunction_P2_Size_jrvfe2qggu [ 2
] ; real_T SFunction_P2_d4kbmisw22 ; real_T SFunction_P3_Size_ppes2kzqgm [ 2
] ; real_T SFunction_P3_gdam0jgpft [ 21 ] ; real_T
SFunction_P4_Size_la0vmwgdtu [ 2 ] ; real_T SFunction_P4_f4lfm50u10 ; real_T
SFunction_P1_Size_arlbgtsvtw [ 2 ] ; real_T SFunction_P1_d5ympp4ajn ; real_T
SFunction_P2_Size_f22bb0beoq [ 2 ] ; real_T SFunction_P2_bhdci5tynm ; real_T
SFunction_P3_Size_hqvx5dpsdq [ 2 ] ; real_T SFunction_P3_llpcde5pey [ 21 ] ;
real_T SFunction_P4_Size_amoqxoisfs [ 2 ] ; real_T SFunction_P4_eg4qizlweo ;
real_T uhZ_Gain_azgxhb2bie [ 3 ] ; real_T Gain3_Gain_cabqd5r1xu [ 3 ] ;
real_T mode2phaseS_Gain_i4u4vmnkwg [ 9 ] ; real_T Gain4_Gain_awti44fbee [ 3 ]
; real_T Gain5_Gain_f34wg0pxhd [ 3 ] ; real_T mode2phaseR_Gain_hi0adye1ac [ 9
] ; real_T UnitDelay_InitialCondition_mp5gfaiube ; real_T LookUpTable_XData [
12 ] ; real_T LookUpTable_YData [ 12 ] ; real_T RateLimiter_RisingLim ;
real_T RateLimiter_FallingLim ; real_T RateLimiter_IC ; real_T uSref_Gain ;
real_T SineWaveA_Amp ; real_T SineWaveA_Bias ; real_T SineWaveA_Hsin ; real_T
SineWaveA_HCos ; real_T SineWaveA_PSin ; real_T SineWaveA_PCos ; real_T
SineWaveB_Amp ; real_T SineWaveB_Bias ; real_T SineWaveB_Hsin ; real_T
SineWaveB_HCos ; real_T SineWaveB_PSin ; real_T SineWaveB_PCos ; real_T
SineWaveC_Amp ; real_T SineWaveC_Bias ; real_T SineWaveC_Hsin ; real_T
SineWaveC_HCos ; real_T SineWaveC_PSin ; real_T SineWaveC_PCos ; real_T
StateSpace_AS_param [ 1156 ] ; real_T StateSpace_BS_param [ 1190 ] ; real_T
StateSpace_CS_param [ 1564 ] ; real_T StateSpace_DS_param [ 1610 ] ; real_T
StateSpace_X0_param [ 34 ] ; real_T donotdeletethisgain_Gain ; real_T
donotdeletethisgain_Gain_hcm2y02ald ; real_T
donotdeletethisgain_Gain_hd2lurzjs1 ; real_T
donotdeletethisgain_Gain_g0aitc3jds ; real_T
donotdeletethisgain_Gain_j0j522ufey ; real_T Kv_Gain ; real_T
Gain3_Gain_dyow2vdij1 [ 9 ] ; real_T Gain1_Gain_pnfzbhegxe ; real_T
Constant4_Value ; real_T Initial_Value ; real_T
DiscreteTimeIntegrator_gainval ; real_T donotdeletethisgain_Gain_m3rdldqf5v ;
real_T Integrator_gainval ; real_T Integrator_gainval_gmvlo1vh4m ; real_T
donotdeletethisgain_Gain_kb2zb2sa4y ; real_T
donotdeletethisgain_Gain_kt5arp05v1 ; real_T
donotdeletethisgain_Gain_bukv5qr0ht ; real_T Kv1_Gain ; real_T
Gain3_Gain_p1jkth5m13 [ 9 ] ; real_T Gain1_Gain_ozhyrxfmnh ; real_T Rff_Gain
; real_T Lff_Gain ; real_T UnitDelay_InitialCondition_fy3uxswg0s ; real_T
UnitDelay_InitialCondition_hvmmrz5whh ; real_T Constant1_Value_finlsj5cwk ;
real_T Gain3_Gain_nwldjxxvlq [ 9 ] ; real_T Gain1_Gain_op30pyxpek ; real_T
Integ4_gainval_lotsoek0sk ; real_T Integ4_IC_f03w4yglrt ; real_T
Toavoiddivisionbyzero_UpperSat_fngqu4twkq ; real_T
Toavoiddivisionbyzero_LowerSat_nspiile1b5 ; real_T
SFunction_P1_Size_azl4fmid3u [ 2 ] ; real_T SFunction_P1_krpmwxbekp ; real_T
SFunction_P2_Size_dvmzlzudlq [ 2 ] ; real_T SFunction_P2_lji01cpuhb ; real_T
SFunction_P3_Size_fgok1k5lwp [ 2 ] ; real_T SFunction_P3_o5gzkun05y ; real_T
SFunction_P4_Size_nocru43ywt [ 2 ] ; real_T SFunction_P4_al55firpgj ; real_T
UnitDelay_InitialCondition_hw44anb1qk ; real_T
UnitDelay1_InitialCondition_dbtlgsoueo ; real_T DiscreteDerivative_DenCoef [
2 ] ; real_T DiscreteDerivative_InitialStates ; real_T
DiscreteTimeIntegrator_gainval_do3kgazujd ; real_T
DiscreteTimeIntegrator_UpperSat ; real_T DiscreteTimeIntegrator_LowerSat ;
real_T Saturation1_UpperSat ; real_T Saturation1_LowerSat ; real_T
Gain10_Gain ; real_T RateLimiter_RisingLim_c0aabshm22 ; real_T
RateLimiter_FallingLim_b13f30fsua ; real_T RateLimiter_IC_gnpihpl5du ; real_T
Delay_x1_InitialCondition ; real_T A11_Gain ; real_T
Delay_x2_InitialCondition ; real_T A12_Gain ; real_T A21_Gain ; real_T
A22_Gain ; real_T B11_Gain ; real_T B21_Gain ; real_T Duk_Gain ; real_T
C11_Gain ; real_T C12_Gain ; real_T Constant3_Value ; real_T
Constant1_Value_kqscwqq4vj ; real_T Constant2_Value ; real_T SineWave_Amp ;
real_T SineWave_Bias ; real_T SineWave_Freq ; real_T SineWave_Hsin [ 3 ] ;
real_T SineWave_HCos [ 3 ] ; real_T SineWave_PSin [ 3 ] ; real_T
SineWave_PCos [ 3 ] ; real_T donotdeletethisgain_Gain_fzvartv1ey ; real_T
Integ4_gainval_gph3gdfsp4 ; real_T Integ4_IC_mncfk0choj ; real_T K1_Value ;
real_T SFunction_P1_Size_ew2pm3b1vb [ 2 ] ; real_T SFunction_P1_dluuhwrclt ;
real_T SFunction_P2_Size_kngh2atg03 [ 2 ] ; real_T SFunction_P2_nlsoaavoed ;
real_T SFunction_P3_Size_la2ztzk3gj [ 2 ] ; real_T SFunction_P3_f2uibysxdf ;
real_T SFunction_P4_Size_gtvcyjcime [ 2 ] ; real_T SFunction_P4_nmbzrokfyd ;
real_T K2_Value ; real_T UnitDelay_InitialCondition_ma0l54m1mt ; real_T
UnitDelay1_InitialCondition_h0y3tjrtyc ; real_T Integ4_gainval_jbhrlhdx2q ;
real_T Integ4_IC_iz52cymmtl ; real_T K1_Value_jbgdaiaown ; real_T
SFunction_P1_Size_odjwhpkqrj [ 2 ] ; real_T SFunction_P1_lryj5emwqk ; real_T
SFunction_P2_Size_fmypdjztco [ 2 ] ; real_T SFunction_P2_cav5odjyxw ; real_T
SFunction_P3_Size_oeoze2eg3f [ 2 ] ; real_T SFunction_P3_ez4s10ulj2 ; real_T
SFunction_P4_Size_bjt1eob1om [ 2 ] ; real_T SFunction_P4_ejsoxgav0m ; real_T
K2_Value_ovjoxlqpjc ; real_T UnitDelay_InitialCondition_f1pmb4tsqq ; real_T
UnitDelay1_InitialCondition_fmg3kvl22k ; real_T Gain_Gain_jivjysmzlr ; real_T
sinwt_Amp ; real_T sinwt_Bias ; real_T sinwt_Freq ; real_T sinwt_Hsin ;
real_T sinwt_HCos ; real_T sinwt_PSin ; real_T sinwt_PCos ; real_T
Integ4_gainval_i0v3kwd3pb ; real_T Integ4_IC_pitekfoys0 ; real_T
K1_Value_mnxxbxkzc3 ; real_T SFunction_P1_Size_ibhpobj5mh [ 2 ] ; real_T
SFunction_P1_lg5pzzsa2d ; real_T SFunction_P2_Size_hndwfqeer1 [ 2 ] ; real_T
SFunction_P2_phvqmqyobr ; real_T SFunction_P3_Size_kqonwcz2dd [ 2 ] ; real_T
SFunction_P3_h2vg5ev1xh ; real_T SFunction_P4_Size_gljhqdhvcw [ 2 ] ; real_T
SFunction_P4_olzkwupxvh ; real_T K2_Value_ivipcgq0za ; real_T
UnitDelay_InitialCondition_du0hifmua4 ; real_T
UnitDelay1_InitialCondition_bxzmk5e5pe ; real_T coswt_Amp ; real_T coswt_Bias
; real_T coswt_Freq ; real_T coswt_Hsin ; real_T coswt_HCos ; real_T
coswt_PSin ; real_T coswt_PCos ; real_T Integ4_gainval_msuhv1zzxl ; real_T
Integ4_IC_p5hl13nikg ; real_T K1_Value_mgtlppjp42 ; real_T
SFunction_P1_Size_pfhrkjciej [ 2 ] ; real_T SFunction_P1_p5oafid3lr ; real_T
SFunction_P2_Size_mhope5jeqx [ 2 ] ; real_T SFunction_P2_g3zfut0ttz ; real_T
SFunction_P3_Size_ky0vccwnr2 [ 2 ] ; real_T SFunction_P3_ailj2rjtnm ; real_T
SFunction_P4_Size_di5ycaxh3v [ 2 ] ; real_T SFunction_P4_pgdix2di41 ; real_T
K2_Value_af05yiua5o ; real_T UnitDelay_InitialCondition_fluz1hs4rw ; real_T
UnitDelay1_InitialCondition_oxna4lbe4y ; real_T RadDeg_Gain ; real_T
degrad_Gain ; real_T sinwt_Amp_kei0q2kps2 ; real_T sinwt_Bias_d20x1us1zk ;
real_T sinwt_Freq_p3beiezqka ; real_T sinwt_Hsin_e0rgtjbjpx ; real_T
sinwt_HCos_cafhkcfovv ; real_T sinwt_PSin_modl3cgbwm ; real_T
sinwt_PCos_cyveu5wuih ; real_T Integ4_gainval_gtzjg23pll ; real_T
Integ4_IC_ln1mjxwhre ; real_T K1_Value_exdwlktlap ; real_T
SFunction_P1_Size_ptq3yp4fjx [ 2 ] ; real_T SFunction_P1_evblyies3u ; real_T
SFunction_P2_Size_ke5xzzx4kh [ 2 ] ; real_T SFunction_P2_gfwehstk0m ; real_T
SFunction_P3_Size_gp3iwgfnzr [ 2 ] ; real_T SFunction_P3_hc0kiipdev ; real_T
SFunction_P4_Size_fjvrmxlm3b [ 2 ] ; real_T SFunction_P4_py3bmltzky ; real_T
K2_Value_jy0nnlmsm2 ; real_T UnitDelay_InitialCondition_l3h1l0mohd ; real_T
UnitDelay1_InitialCondition_mxloi3dinr ; real_T coswt_Amp_nskemowyiu ; real_T
coswt_Bias_jk2fedlc5k ; real_T coswt_Freq_gttzxcelpy ; real_T
coswt_Hsin_omgodzfzvz ; real_T coswt_HCos_oz23s2cxn2 ; real_T
coswt_PSin_khvpsemk1b ; real_T coswt_PCos_ontpys2zdd ; real_T
Integ4_gainval_g3uamgfpqb ; real_T Integ4_IC_g12toj1ern ; real_T
K1_Value_g33io5pe2n ; real_T SFunction_P1_Size_cjan5gzh5s [ 2 ] ; real_T
SFunction_P1_lz1kvrqslv ; real_T SFunction_P2_Size_jvanv0ykea [ 2 ] ; real_T
SFunction_P2_m4zyizvnqv ; real_T SFunction_P3_Size_jgylmcydz1 [ 2 ] ; real_T
SFunction_P3_hsaxqamxsw ; real_T SFunction_P4_Size_fcjx3ghw1g [ 2 ] ; real_T
SFunction_P4_g2c4lhfo5n ; real_T K2_Value_ox1txrzba0 ; real_T
UnitDelay_InitialCondition_ictydfu5gx ; real_T
UnitDelay1_InitialCondition_ew2tfa0xiv ; real_T RadDeg_Gain_oothuzb2un ;
real_T degrad1_Gain ; real_T sinwt_Amp_gglawa5b0j ; real_T
sinwt_Bias_ftcgy00q1q ; real_T sinwt_Freq_ibh040bo2d ; real_T
sinwt_Hsin_ltv4wuiu5i ; real_T sinwt_HCos_lozsdsini2 ; real_T
sinwt_PSin_ism3nzmned ; real_T sinwt_PCos_esk52xey53 ; real_T
Integ4_gainval_h0bygbtnao ; real_T Integ4_IC_kpvxlhwixo ; real_T
K1_Value_nofhnhfgqe ; real_T SFunction_P1_Size_pejtedqyc4 [ 2 ] ; real_T
SFunction_P1_lxyuxdb3mp ; real_T SFunction_P2_Size_hlcp1ovx4k [ 2 ] ; real_T
SFunction_P2_lvgbmfzfkf ; real_T SFunction_P3_Size_ftrxnxhe1f [ 2 ] ; real_T
SFunction_P3_cll0rk4ayt ; real_T SFunction_P4_Size_jds5a2cbzo [ 2 ] ; real_T
SFunction_P4_cdxkopokc1 ; real_T K2_Value_cheibnriss ; real_T
UnitDelay_InitialCondition_gbfuarhyw0 ; real_T
UnitDelay1_InitialCondition_ajpewtdlxk ; real_T coswt_Amp_ov5trykaai ; real_T
coswt_Bias_caz0b22m5s ; real_T coswt_Freq_gp2xaew4z5 ; real_T
coswt_Hsin_p0lqeycuo2 ; real_T coswt_HCos_e1lcq0jydt ; real_T
coswt_PSin_pzecylb5ie ; real_T coswt_PCos_apooxuqtce ; real_T
Integ4_gainval_lqadd2jned ; real_T Integ4_IC_bcihf3qerl ; real_T
K1_Value_piw4pfcum5 ; real_T SFunction_P1_Size_n1ajy1bnud [ 2 ] ; real_T
SFunction_P1_dkdy2k15zy ; real_T SFunction_P2_Size_e0a1ljwhhh [ 2 ] ; real_T
SFunction_P2_k3kgnekxzq ; real_T SFunction_P3_Size_g2ktdcnint [ 2 ] ; real_T
SFunction_P3_hck5ayelnn ; real_T SFunction_P4_Size_ouyv0d4ulf [ 2 ] ; real_T
SFunction_P4_frxx4pv3d4 ; real_T K2_Value_luz1rwbbpa ; real_T
UnitDelay_InitialCondition_c1mnajnz03 ; real_T
UnitDelay1_InitialCondition_fwlh0v4clc ; real_T RadDeg_Gain_kam5dohr3p ;
real_T degrad2_Gain ; real_T sinwt_Amp_a5iuycgbj5 ; real_T
sinwt_Bias_o4hfny211d ; real_T sinwt_Freq_g4kja400xj ; real_T
sinwt_Hsin_f4ff1jm01o ; real_T sinwt_HCos_ikm4ke3nt1 ; real_T
sinwt_PSin_mxp5pxyazu ; real_T sinwt_PCos_ffxrlya5uh ; real_T
Integ4_gainval_gvvjdr1enu ; real_T Integ4_IC_ljmxfnx2ig ; real_T
K1_Value_ly41q0lfne ; real_T SFunction_P1_Size_jukdwcj1bv [ 2 ] ; real_T
SFunction_P1_pa4jjhehiv ; real_T SFunction_P2_Size_psnzhais11 [ 2 ] ; real_T
SFunction_P2_bytfqwmq3y ; real_T SFunction_P3_Size_l30wnhs0mm [ 2 ] ; real_T
SFunction_P3_n2witgfr1i ; real_T SFunction_P4_Size_ktxkgaz1xj [ 2 ] ; real_T
SFunction_P4_k2rjx01pxg ; real_T K2_Value_peraggcnbk ; real_T
UnitDelay_InitialCondition_d4fsgzspon ; real_T
UnitDelay1_InitialCondition_nwwwd2tqrt ; real_T coswt_Amp_ph0slmkaz2 ; real_T
coswt_Bias_mxccsalmjx ; real_T coswt_Freq_iohcbhspnz ; real_T
coswt_Hsin_ki40gemxaa ; real_T coswt_HCos_ih2fmjgzvo ; real_T
coswt_PSin_ckpub3oin5 ; real_T coswt_PCos_jgy3xycijn ; real_T
Integ4_gainval_e3hymojevp ; real_T Integ4_IC_b4moxyhjbs ; real_T
K1_Value_odshxihjs1 ; real_T SFunction_P1_Size_nmlo2jbnyf [ 2 ] ; real_T
SFunction_P1_f3ypa2zb01 ; real_T SFunction_P2_Size_lziftoqs2s [ 2 ] ; real_T
SFunction_P2_hs0brjtvaq ; real_T SFunction_P3_Size_izfvfjalyr [ 2 ] ; real_T
SFunction_P3_csxyjaoi2n ; real_T SFunction_P4_Size_gy1avcz5ha [ 2 ] ; real_T
SFunction_P4_gtfhpmr5uy ; real_T K2_Value_ca4rzsh0ua ; real_T
UnitDelay_InitialCondition_iu5allnmxd ; real_T
UnitDelay1_InitialCondition_fsrsshkqoc ; real_T RadDeg_Gain_hfllzvpq3o ;
real_T degrad_Gain_num4oalqxy ; real_T sinwt_Amp_lmppjqm2un ; real_T
sinwt_Bias_i50qywn0ii ; real_T sinwt_Freq_pkofylmhnt ; real_T
sinwt_Hsin_i0nd0ubqqg ; real_T sinwt_HCos_c3em0todhq ; real_T
sinwt_PSin_bllazdudnu ; real_T sinwt_PCos_ilquu0arpw ; real_T
Integ4_gainval_o4m2wju04m ; real_T Integ4_IC_etee4cslmc ; real_T
K1_Value_fascbhcv3t ; real_T SFunction_P1_Size_i4ous05wd0 [ 2 ] ; real_T
SFunction_P1_ampffu3m3c ; real_T SFunction_P2_Size_nomaj1bgdp [ 2 ] ; real_T
SFunction_P2_djfrqai1qz ; real_T SFunction_P3_Size_n4gzbc3msr [ 2 ] ; real_T
SFunction_P3_he0khbdhcb ; real_T SFunction_P4_Size_g2xqkvldm0 [ 2 ] ; real_T
SFunction_P4_cbbrphqo2f ; real_T K2_Value_bqbhuqvuc2 ; real_T
UnitDelay_InitialCondition_bh2031guu1 ; real_T
UnitDelay1_InitialCondition_gcar3snp12 ; real_T coswt_Amp_aqxb2j5jnu ; real_T
coswt_Bias_frzp530tnq ; real_T coswt_Freq_aek3lx4btw ; real_T
coswt_Hsin_fuzcqltv4d ; real_T coswt_HCos_cs3y4qtzeb ; real_T
coswt_PSin_bdolgwlxdg ; real_T coswt_PCos_oqm4st2je0 ; real_T
Integ4_gainval_dmsyi0ujpc ; real_T Integ4_IC_lektlq1jie ; real_T
K1_Value_okiumpl42l ; real_T SFunction_P1_Size_cvpkxdvatx [ 2 ] ; real_T
SFunction_P1_akvmkuhaw5 ; real_T SFunction_P2_Size_fwn5zkbns1 [ 2 ] ; real_T
SFunction_P2_dxmnlu0a5n ; real_T SFunction_P3_Size_pedctzjhmg [ 2 ] ; real_T
SFunction_P3_jhuhvxm5vq ; real_T SFunction_P4_Size_nydreygcaq [ 2 ] ; real_T
SFunction_P4_icb2jtdmvm ; real_T K2_Value_gzomggch2d ; real_T
UnitDelay_InitialCondition_nj2mac4354 ; real_T
UnitDelay1_InitialCondition_exqimte31v ; real_T RadDeg_Gain_lezhz3zr05 ;
real_T degrad1_Gain_gcrkcsbsjy ; real_T sinwt_Amp_oe50flwsvz ; real_T
sinwt_Bias_hpmk1ox4mk ; real_T sinwt_Freq_i5xw5disvz ; real_T
sinwt_Hsin_kfaxcimxsb ; real_T sinwt_HCos_lv5zpnsoyo ; real_T
sinwt_PSin_bs1ts2ayri ; real_T sinwt_PCos_owldbifmfn ; real_T
Integ4_gainval_lqpuvz5try ; real_T Integ4_IC_omqg2tqo15 ; real_T
K1_Value_fl2t5hhsxt ; real_T SFunction_P1_Size_pd353sj30r [ 2 ] ; real_T
SFunction_P1_eo2zpwfgcc ; real_T SFunction_P2_Size_a1y0oaqnrc [ 2 ] ; real_T
SFunction_P2_mdi2o1yibz ; real_T SFunction_P3_Size_er4czmynw5 [ 2 ] ; real_T
SFunction_P3_akzr3coq0n ; real_T SFunction_P4_Size_pk24ovu2kv [ 2 ] ; real_T
SFunction_P4_o4gzym1ztl ; real_T K2_Value_fl0eknqnzp ; real_T
UnitDelay_InitialCondition_drj2csvrhz ; real_T
UnitDelay1_InitialCondition_lzsilqa4qy ; real_T coswt_Amp_ajaph324eb ; real_T
coswt_Bias_er20tnmo5c ; real_T coswt_Freq_hhpfztkccd ; real_T
coswt_Hsin_nt154hil4g ; real_T coswt_HCos_dtrnnwyvoi ; real_T
coswt_PSin_pc2mzqmykn ; real_T coswt_PCos_exeoqhe2el ; real_T
Integ4_gainval_ae24pre0oj ; real_T Integ4_IC_fzbpruzddz ; real_T
K1_Value_huwvvbdsqd ; real_T SFunction_P1_Size_gt1yv20ls2 [ 2 ] ; real_T
SFunction_P1_o30oihu332 ; real_T SFunction_P2_Size_acjwavlpwb [ 2 ] ; real_T
SFunction_P2_jub0nilfwr ; real_T SFunction_P3_Size_k0opnt4gpv [ 2 ] ; real_T
SFunction_P3_jyslu0v2kb ; real_T SFunction_P4_Size_luydhi20rk [ 2 ] ; real_T
SFunction_P4_lfuanfgbiu ; real_T K2_Value_nuhsjmrzct ; real_T
UnitDelay_InitialCondition_bp1tsi5gga ; real_T
UnitDelay1_InitialCondition_ovdhgn1wwe ; real_T RadDeg_Gain_bjvkdbizli ;
real_T degrad2_Gain_fvnkccap1v ; real_T Gain1_Gain_b0ayny2lnu ; real_T
raddeg1_Gain ; real_T raddeg1_Gain_joedyuhdkb ; real_T DegRad_Gain ; real_T
W2kW_Gain ; real_T donotdeletethisgain_Gain_lcnbsf0lej ; real_T
donotdeletethisgain_Gain_gvlzyc5wfl ; real_T
donotdeletethisgain_Gain_mjapajrwub ; real_T Kv1_Gain_oxfuo4itvi ; real_T
donotdeletethisgain_Gain_ktpkblxqtm ; real_T
donotdeletethisgain_Gain_i4321lkuie ; real_T
donotdeletethisgain_Gain_jyxp4ll2lp ; real_T Kv_Gain_gz5snr4zgc ; real_T
puV_Gain ; real_T integrator_IC ; real_T TransportDelay_Delay ; real_T
TransportDelay_InitOutput ; real_T K1_Value_nt1eijy1al ; real_T
Memory_InitialCondition ; real_T Saturationtoavoiddivisionby0_UpperSat ;
real_T Saturationtoavoiddivisionby0_LowerSat ; real_T
Saturationtoavoiddivisionby0_UpperSat_gsjjkdpfuo ; real_T
Saturationtoavoiddivisionby0_LowerSat_mkrxhvjga0 ; real_T
integrator_IC_gfcwranmqu ; real_T TransportDelay_Delay_jesai05nr3 ; real_T
TransportDelay_InitOutput_pykn05w1hn ; real_T K1_Value_n5okklsegz ; real_T
Memory_InitialCondition_j5vuqn1rvq ; real_T
Saturationtoavoiddivisionby0_UpperSat_o2lkqggvaq ; real_T
Saturationtoavoiddivisionby0_LowerSat_gipoj3tzue ; real_T
Saturationtoavoiddivisionby0_UpperSat_ewfqz5og5w ; real_T
Saturationtoavoiddivisionby0_LowerSat_cmchd1rqvs ; real_T
Gain7_Gain_i0s03tspqs [ 3 ] ; real_T Gain8_Gain [ 3 ] ; real_T
phase2modeR_Gain [ 9 ] ; real_T phase2modeS_Gain [ 9 ] ; real_T
Gain7_Gain_g2vyun1xyv [ 3 ] ; real_T Gain8_Gain_gygrek3mcm [ 3 ] ; real_T
phase2modeR_Gain_ptaizj5vff [ 9 ] ; real_T phase2modeS_Gain_mhoa3los1g [ 9 ]
; real_T zerotogeneratedummydiodegateanddelaysignals_Value ; real_T
Iq_ref_Value ; real_T Lff_Gain_m1pxhw4cyl ; real_T Rff_Gain_mplfss1cnd ;
real_T Iph_1_Value ; real_T Iph_2_Value ; real_T MPPT_On_Value ; real_T
Constant_Value_kkuzztqyci ; real_T Constant1_Value_a4nlxyznsj ; real_T
Constant2_Value_iqex2uo240 ; real_T Constant4_Value_f2ccycfp1g ; real_T
Constant5_Value ; real_T Constant6_Value ; real_T Gain1_Gain_iza01f01um ;
real_T Gain2_Gain ; real_T Constant11_Value [ 4 ] ; real_T Constant12_Value [
4 ] ; real_T Constant13_Value [ 4 ] ; real_T Constant2_Value_id2ug4wnj0 ;
real_T Constant_Value_prlpwr0gxs ; real_T Constant2_Value_lyii0iggwc [ 3 ] ;
real_T Gain1_Gain_bfibyfcm43 ; real_T u_K_Value ; real_T
Saturation_UpperSat_gtqfq0ppkv ; real_T Saturation_LowerSat_e1cd25lz4w ;
real_T Tref_K_Value ; real_T alpha_Isc_Gain ; real_T IL_module_Value ; real_T
Rs_array_Value ; real_T one1_Value ; real_T Tref_K1_Value ; real_T dEgdT_Gain
; real_T EgRef_Gain ; real_T Tref_K2_Value ; real_T I0_array_Gain ; real_T
EgRef_Value ; real_T k1_Gain ; real_T Rsh_array_Value ; real_T
Rsh_array_5Sref_Value ; real_T VT_ref_array_Gain ; real_T one_Value ; real_T
Constant_Value_gu00guxond ; real_T Constant1_Value_d3fgit3q2y ; real_T
Constant2_Value_dvfzdjynxb ; real_T Constant_Value_fal5u45x5b ; real_T
Constant1_Value_gerhfiv2al ; real_T Constant2_Value_h0okejydpk ; klg0lcyohc
daw0ymwmw4 ; pcr2y0buvk pu1c2mp3al ; klg0lcyohc glvlabfrtp ; pcr2y0buvk
drtk0acyhp ; klg0lcyohc blrugcnivv ; pcr2y0buvk iks0grgus1 ; klg0lcyohc
jhi4zkfadx ; pcr2y0buvk mdskmorhmi ; klg0lcyohc e2mpwvt2ua ; pcr2y0buvk
oa2z5wykl2 ; klg0lcyohc gllp1vimnhl ; pcr2y0buvk kyedbo0mxuc ; pneeuxrdyc
cxxbka35es ; m3uhrpfyin h1nhkvogk5 ; m3uhrpfyin drzlaqk2wl ; pneeuxrdyc
ihqnx345xuh ; m3uhrpfyin jr3gbt2o32 ; m3uhrpfyin gj0ekbre2up ; fcezepw4oj
ibrtt4ftsl ; fjazhlmyos bdvvkdwqfm ; fcezepw4oj imgy1z4qes ; fjazhlmyos
gusdfzbcqa ; fcezepw4oj gvjufizpfu ; fjazhlmyos h0t1xli1qx ; fcezepw4oj
flkfnl3dquj ; fjazhlmyos fiy4znthvv3 ; } ; extern const real_T
PV_Array_Model_RGND ; extern const char * RT_MEMORY_ALLOCATION_ERROR ; extern
B rtB ; extern X rtX ; extern DW rtDW ; extern P rtP ; extern const
rtwCAPI_ModelMappingStaticInfo * PV_Array_Model_GetCAPIStaticMap ( void ) ;
extern SimStruct * const rtS ; extern const int_T gblNumToFiles ; extern
const int_T gblNumFrFiles ; extern const int_T gblNumFrWksBlocks ; extern
rtInportTUtable * gblInportTUtables ; extern const char * gblInportFileName ;
extern const int_T gblNumRootInportBlks ; extern const int_T
gblNumModelInputs ; extern const int_T gblInportDataTypeIdx [ ] ; extern
const int_T gblInportDims [ ] ; extern const int_T gblInportComplex [ ] ;
extern const int_T gblInportInterpoFlag [ ] ; extern const int_T
gblInportContinuous [ ] ; extern const int_T gblParameterTuningTid ; extern
DataMapInfo * rt_dataMapInfoPtr ; extern rtwCAPI_ModelMappingInfo *
rt_modelMapInfoPtr ; void MdlOutputs ( int_T tid ) ; void
MdlOutputsParameterSampleTime ( int_T tid ) ; void MdlUpdate ( int_T tid ) ;
void MdlTerminate ( void ) ; void MdlInitializeSizes ( void ) ; void
MdlInitializeSampleTimes ( void ) ; SimStruct * raccel_register_model ( void
) ;
#endif
