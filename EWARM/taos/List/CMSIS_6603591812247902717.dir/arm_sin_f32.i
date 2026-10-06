








 
















 






 
















 











































































































































 




 








 









 



 






































































 



 



 



 


 






 



 

 




 




 
















 

  #pragma system_include          


 
 

  #pragma system_include

 
 

 

  #pragma system_include














 


 
 


  #pragma system_include

 



 

 

 

 
#pragma rtmodel = "__dlib_version", "6"

 


 



























 


  #pragma system_include

 
 
 


  #pragma system_include

 

 

 

 

   

 
 


   #pragma system_include






 




 


 


 


 

 


 

 

 

 

 

 

 

 

 

 
















 



















 











 























 





 



 










 














 













 








 













 













 















 











 








 








 






 





 












 





 













 






 


   


  







 







 




 






 




 




 













 

   




 







 







 







 










 





 

















 


 


 













 

   


 


 



 

 

 
  typedef unsigned int _Wchart;
  typedef unsigned int _Wintt;

 

 
typedef unsigned int     _Sizet;

 
typedef signed char   __int8_t;
typedef unsigned char  __uint8_t;
typedef signed short int   __int16_t;
typedef unsigned short int  __uint16_t;
typedef signed int   __int32_t;
typedef unsigned int  __uint32_t;
   typedef signed long long int   __int64_t;
   typedef unsigned long long int  __uint64_t;
typedef signed int   __intptr_t;
typedef unsigned int  __uintptr_t;

 
typedef struct _Mbstatet
{  
    unsigned int _Wchar;   
    unsigned int _State;   

} _Mbstatet;

 

 
  typedef struct __va_list __Va_list;


    typedef struct __FILE _Filet;

 
typedef struct
{
    long long _Off;     
  _Mbstatet _Wstate;
} _Fpost;


 

 
  
   
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Malloc(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Stream(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Debug(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_StaticGuard(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Malloc(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Stream(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Debug(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_StaticGuard(void);

      _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Lockfilelock(_Filet *);
      _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlockfilelock(_Filet *);

  typedef void *__iar_Rmtx;

  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Initdynamiclock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Dstdynamiclock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Lockdynamiclock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlockdynamiclock(__iar_Rmtx *);

  






 


 
  typedef signed char          int8_t;
  typedef unsigned char        uint8_t;

  typedef signed short int         int16_t;
  typedef unsigned short int       uint16_t;

  typedef signed int         int32_t;
  typedef unsigned int       uint32_t;

  typedef signed long long int         int64_t;
  typedef unsigned long long int       uint64_t;


 
typedef signed char      int_least8_t;
typedef unsigned char    uint_least8_t;

typedef signed short int     int_least16_t;
typedef unsigned short int   uint_least16_t;

typedef signed int     int_least32_t;
typedef unsigned int   uint_least32_t;

 
  typedef signed long long int   int_least64_t;
  typedef unsigned long long int uint_least64_t;



 
typedef signed int       int_fast8_t;
typedef unsigned int     uint_fast8_t;

typedef signed int      int_fast16_t;
typedef unsigned int    uint_fast16_t;

typedef signed int      int_fast32_t;
typedef unsigned int    uint_fast32_t;

  typedef signed long long int    int_fast64_t;
  typedef unsigned long long int  uint_fast64_t;

 
typedef signed long long int          intmax_t;
typedef unsigned long long int        uintmax_t;


 
typedef signed int          intptr_t;
typedef unsigned int        uintptr_t;

 
typedef int __data_intptr_t; typedef unsigned int __data_uintptr_t;

 






















 











 














 




 



 

 




 
















 

  #pragma system_include          


 

 




 


 




 
















 





 
 




 





















#pragma system_include






 

 















#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic uint16_t __iar_uint16_read(void const *ptr)
{
  return *(__packed uint16_t*)(ptr);
}
#pragma language=restore


#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic void __iar_uint16_write(void const *ptr, uint16_t val)
{
  *(__packed uint16_t*)(ptr) = val;;
}
#pragma language=restore

#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic uint32_t __iar_uint32_read(void const *ptr)
{
  return *(__packed uint32_t*)(ptr);
}
#pragma language=restore

#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic void __iar_uint32_write(void const *ptr, uint32_t val)
{
  *(__packed uint32_t*)(ptr) = val;;
}
#pragma language=restore

#pragma language=save
#pragma language=extended
__packed struct  __iar_u32 { uint32_t v; };
#pragma language=restore















 



  #pragma system_include



 


 


#pragma language=save
#pragma language=extended

__intrinsic __nounwind void    __iar_builtin_no_operation(void);

__intrinsic __nounwind void    __iar_builtin_disable_interrupt(void);
__intrinsic __nounwind void    __iar_builtin_enable_interrupt(void);

typedef unsigned int __istate_t;

__intrinsic __nounwind __istate_t __iar_builtin_get_interrupt_state(void);
__intrinsic __nounwind void __iar_builtin_set_interrupt_state(__istate_t);

 
__intrinsic __nounwind unsigned int __iar_builtin_get_PSR( void );
__intrinsic __nounwind unsigned int __iar_builtin_get_IPSR( void );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_MSP( void );
__intrinsic __nounwind void         __iar_builtin_set_MSP( unsigned int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_PSP( void );
__intrinsic __nounwind void         __iar_builtin_set_PSP( unsigned int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_PRIMASK( void );
__intrinsic __nounwind void         __iar_builtin_set_PRIMASK( unsigned int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_CONTROL( void );
__intrinsic __nounwind void         __iar_builtin_set_CONTROL( unsigned int );

 
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_FAULTMASK( void );
__intrinsic __nounwind void         __iar_builtin_set_FAULTMASK(unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_BASEPRI( void );
__intrinsic __nounwind void         __iar_builtin_set_BASEPRI( unsigned int );

 
__intrinsic __nounwind void __iar_builtin_disable_irq(void);
__intrinsic __nounwind void __iar_builtin_enable_irq(void);

__intrinsic __nounwind void __iar_builtin_disable_fiq(void);
__intrinsic __nounwind void __iar_builtin_enable_fiq(void);


 

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SWP( unsigned int, volatile unsigned int * );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned char __iar_builtin_SWPB( unsigned char, volatile unsigned char * );

typedef unsigned int __ul;
typedef unsigned int __iar_builtin_uint;


 

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void __iar_builtin_CDP (unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRd, unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opc2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void __iar_builtin_CDP2(unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRd, unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opc2);

 
__intrinsic __nounwind void          __iar_builtin_MCR( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1, __iar_builtin_uint src,
                                unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );
__intrinsic __nounwind unsigned int __iar_builtin_MRC( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1,
                                unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );
__intrinsic __nounwind void          __iar_builtin_MCR2( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1, __iar_builtin_uint src,
                                 unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );
__intrinsic __nounwind unsigned int __iar_builtin_MRC2( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1,
                                 unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );

__intrinsic __nounwind void __iar_builtin_MCRR (unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned long long src, unsigned __constrange(0,15) CRm);
__intrinsic __nounwind void __iar_builtin_MCRR2(unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned long long src, unsigned __constrange(0,15) CRm);

__intrinsic __nounwind unsigned long long __iar_builtin_MRRC (unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRm);
__intrinsic __nounwind unsigned long long __iar_builtin_MRRC2(unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRm);

 
__intrinsic __nounwind void __iar_builtin_LDC  ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);
__intrinsic __nounwind void __iar_builtin_LDCL ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);
__intrinsic __nounwind void __iar_builtin_LDC2 ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);
__intrinsic __nounwind void __iar_builtin_LDC2L( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);

 
__intrinsic __nounwind void __iar_builtin_STC  ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);
__intrinsic __nounwind void __iar_builtin_STCL ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);
__intrinsic __nounwind void __iar_builtin_STC2 ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);
__intrinsic __nounwind void __iar_builtin_STC2L( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);

 
__intrinsic __nounwind void __iar_builtin_LDC_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                              unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_LDCL_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_LDC2_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_LDC2L_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                                unsigned __constrange(0,255) option);

 
__intrinsic __nounwind void __iar_builtin_STC_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                              unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_STCL_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_STC2_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_STC2L_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                                unsigned __constrange(0,255) option);


 
__intrinsic __nounwind unsigned int       __iar_builtin_rsr(__spec_string const char * special_register);
__intrinsic __nounwind unsigned long long __iar_builtin_rsr64(__spec_string const char * special_register);
__intrinsic __nounwind void*              __iar_builtin_rsrp(__spec_string const char * special_register);

 
__intrinsic __nounwind void __iar_builtin_wsr(__spec_string const char * special_register, unsigned int value);
__intrinsic __nounwind void __iar_builtin_wsr64(__spec_string const char * special_register, unsigned long long value);
__intrinsic __nounwind void __iar_builtin_wsrp(__spec_string const char * special_register, const void *value);

 
__intrinsic __nounwind unsigned int __iar_builtin_get_APSR( void );
__intrinsic __nounwind void         __iar_builtin_set_APSR( unsigned int );

 
__intrinsic __nounwind unsigned int __iar_builtin_get_CPSR( void );
__intrinsic __nounwind void         __iar_builtin_set_CPSR( unsigned int );

 
__intrinsic __nounwind unsigned int __iar_builtin_get_FPSCR( void );
__intrinsic __nounwind void __iar_builtin_set_FPSCR( unsigned int );

 
 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CLZ(unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_ROR(unsigned int, unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_RRX(unsigned int);

 
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QADD( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QDADD( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QSUB( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QDSUB( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QDOUBLE( signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int        __iar_builtin_QFlag( void );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int   __iar_builtin_acle_QFlag(void);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void  __iar_builtin_set_QFlag(int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void  __iar_builtin_ignore_QFlag(void);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int         __iar_builtin_QCFlag( void );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void __iar_builtin_reset_QC_flag( void );

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_SMUL( signed short, signed short );

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_REV( unsigned int );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_REVSH( short );

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_REV16( unsigned int );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_RBIT( unsigned int );

__intrinsic __nounwind unsigned char  __iar_builtin_LDREXB( volatile unsigned char const * );
__intrinsic __nounwind unsigned short __iar_builtin_LDREXH( volatile unsigned short const * );
__intrinsic __nounwind unsigned int  __iar_builtin_LDREX ( volatile unsigned int const * );
__intrinsic __nounwind unsigned long long __iar_builtin_LDREXD( volatile unsigned long long const * );

__intrinsic __nounwind unsigned int  __iar_builtin_STREXB( unsigned char, volatile unsigned char * );
__intrinsic __nounwind unsigned int  __iar_builtin_STREXH( unsigned short, volatile unsigned short * );
__intrinsic __nounwind unsigned int  __iar_builtin_STREX ( unsigned int, volatile unsigned int * );
__intrinsic __nounwind unsigned int  __iar_builtin_STREXD( unsigned long long, volatile unsigned long long * );

__intrinsic __nounwind void __iar_builtin_CLREX( void );

__intrinsic __nounwind void __iar_builtin_SEV( void );
__intrinsic __nounwind void __iar_builtin_WFE( void );
__intrinsic __nounwind void __iar_builtin_WFI( void );
__intrinsic __nounwind void __iar_builtin_YIELD( void );

__intrinsic __nounwind void __iar_builtin_PLI( volatile void const * );
__intrinsic __nounwind void __iar_builtin_PLD( volatile void const * );

__intrinsic __nounwind void __iar_builtin_PLIx( volatile void const *, unsigned int __constrange(0,2), unsigned int __constrange(0,1));
__intrinsic __nounwind void __iar_builtin_PLDx( volatile void const *, unsigned int __constrange(0, 1), unsigned int __constrange(0, 2), unsigned int __constrange(0, 1));
__intrinsic __nounwind void __iar_builtin_PLDW( volatile void const * );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int   __iar_builtin_SSAT     (signed int val, unsigned int __constrange( 1, 32 ) sat );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAT     (signed int val, unsigned int __constrange( 0, 31 ) sat );

 
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SEL( unsigned int op1, unsigned int op2 );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SADD8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SADD16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSUB8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSUB16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SADDSUBX (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSUBADDX (unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHADD8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHADD16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSUB8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSUB16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHADDSUBX(unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSUBADDX(unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QADD8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QADD16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSUB8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSUB16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QADDSUBX (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSUBADDX (unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UADD8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UADD16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USUB8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USUB16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UADDSUBX (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USUBADDX (unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHADD8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHADD16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSUB8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSUB16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHADDSUBX(unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSUBADDX(unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQADD8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQADD16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSUB8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSUB16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQADDSUBX(unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSUBADDX(unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAD8(unsigned int x, unsigned int y );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USADA8(unsigned int x, unsigned int y,
                                   unsigned int acc );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSAT16   (unsigned int pair,
                                      unsigned int __constrange( 1, 16 ) sat );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAT16   (unsigned int pair,
                                      unsigned int __constrange( 0, 15 ) sat );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUAD (unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUSD (unsigned int x, unsigned int y);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUADX(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUSDX(unsigned int x, unsigned int y);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLAD (unsigned int x, unsigned int y, int sum);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLSD (unsigned int x, unsigned int y, int sum);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLADX(unsigned int x, unsigned int y, int sum);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLSDX(unsigned int x, unsigned int y, int sum);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALD (unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALDX(unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLSLD (unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLSLDX(unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_PKHBT(unsigned int x,
                                  unsigned int y,
                                  unsigned __constrange(0,31) count);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_PKHTB(unsigned int x,
                                  unsigned int y,
                                  unsigned __constrange(0,32) count);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLABB(unsigned int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLABT(unsigned int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLATB(unsigned int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLATT(unsigned int x, unsigned int y, int acc);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLAWB(int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLAWT(int x, unsigned int y, int acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLA (int x, int y, int acc);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLAR(int x, int y, int acc);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLS (int x, int y, int acc);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLSR(int x, int y, int acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMUL (int x, int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMULR(int x, int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULBB(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULBT(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULTB(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULTT(unsigned int x, unsigned int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULWB(int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULWT(int x, unsigned int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SXTAB (int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SXTAH (int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTAB (unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTAH (unsigned int x, unsigned int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned long long __iar_builtin_UMAAL(unsigned int x,
                                       unsigned int y,
                                       unsigned int a,
                                       unsigned int b);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALBB(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALBT(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALTB(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALTT(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTB16(unsigned int x);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTAB16(unsigned int acc, unsigned int x);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SXTB16(unsigned int x);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SXTAB16(unsigned int acc, unsigned int x);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSAX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHASX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSAX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSAX(unsigned int, unsigned int);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHASX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSAX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSAX(unsigned int, unsigned int);

 
__intrinsic __nounwind void __iar_builtin_DMB(void);
__intrinsic __nounwind void __iar_builtin_DSB(void);
__intrinsic __nounwind void __iar_builtin_ISB(void);
__intrinsic __nounwind void __iar_builtin_DMBx(unsigned int __constrange(1, 15));
__intrinsic __nounwind void __iar_builtin_DSBx(unsigned int __constrange(1, 15));
__intrinsic __nounwind void __iar_builtin_ISBx(unsigned int __constrange(1, 15));

 
__intrinsic __nounwind unsigned int __iar_builtin_TT(unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_TTT(unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_TTA(unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_TTAT(unsigned int);

__intrinsic __nounwind unsigned int __iar_builtin_get_SB(void);
__intrinsic __nounwind void __iar_builtin_set_SB(unsigned int);

__intrinsic __nounwind unsigned int __get_LR(void);
__intrinsic __nounwind void __set_LR(unsigned int);

__intrinsic __nounwind unsigned int __get_SP(void);
__intrinsic __nounwind void __set_SP(unsigned int);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VSQRT_F32(float x);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VSQRT_F64(double x);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFMA_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFMS_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFNMA_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFNMS_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFMA_F64(double x, double y, double z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFMS_F64(double x, double y, double z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFNMA_F64(double x, double y, double z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFNMS_F64(double x, double y, double z);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32B(unsigned int crc, unsigned char data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32H(unsigned int crc, unsigned short data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32W(unsigned int crc, unsigned int data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32CB(unsigned int crc, unsigned char data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32CH(unsigned int crc, unsigned short data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32CW(unsigned int crc, unsigned int data);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VMAXNM_F32(float a, float b);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VMINNM_F32(float a, float b);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VMAXNM_F64(double a, double b);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VMINNM_F64(double a, double b);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTA_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTM_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTN_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTP_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTX_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTR_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTZ_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTA_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTM_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTN_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTP_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTX_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTR_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTZ_F64(double a);

#pragma language=restore









    

    



    
    


    







  _Pragma("inline=forced") __intrinsic int16_t __REVSH(int16_t val)
  {
    return (int16_t) __iar_builtin_REVSH(val);
  }












  _Pragma("inline=forced") __intrinsic uint8_t __LDRBT(volatile uint8_t *addr)
  {
    uint32_t res;
    __asm("LDRBT %0, [%1]" : "=r" (res) : "r" (addr) : "memory");
    return ((uint8_t)res);
  }

  _Pragma("inline=forced") __intrinsic uint16_t __LDRHT(volatile uint16_t *addr)
  {
    uint32_t res;
    __asm("LDRHT %0, [%1]" : "=r" (res) : "r" (addr) : "memory");
    return ((uint16_t)res);
  }

  _Pragma("inline=forced") __intrinsic uint32_t __LDRT(volatile uint32_t *addr)
  {
    uint32_t res;
    __asm("LDRT %0, [%1]" : "=r" (res) : "r" (addr) : "memory");
    return res;
  }

  _Pragma("inline=forced") __intrinsic void __STRBT(uint8_t value, volatile uint8_t *addr)
  {
    __asm("STRBT %1, [%0]" : : "r" (addr), "r" ((uint32_t)value) : "memory");
  }

  _Pragma("inline=forced") __intrinsic void __STRHT(uint16_t value, volatile uint16_t *addr)
  {
    __asm("STRHT %1, [%0]" : : "r" (addr), "r" ((uint32_t)value) : "memory");
  }

  _Pragma("inline=forced") __intrinsic void __STRT(uint32_t value, volatile uint32_t *addr)
  {
    __asm("STRT %1, [%0]" : : "r" (addr), "r" (value) : "memory");
  }




#pragma diag_default=Pe940
#pragma diag_default=Pe177





 








 
 

  #pragma system_include

 
 

 

  #pragma system_include














 


 
 


  #pragma system_include

 
 

 

  #pragma system_include














 



 
  typedef _Sizet size_t;

typedef unsigned int __data_size_t;











 


  #pragma system_include


  





 


  




 




  


 


  #pragma inline=forced_no_body
  _Pragma("function_effects = no_state, no_read(1), no_write(2), returns 1, always_returns") __intrinsic __nounwind void * memcpy(void * _D, const void * _S, size_t _N)
  {
    __aeabi_memcpy(_D, _S, _N);
    return _D;
  }

  #pragma inline=forced_no_body
  _Pragma("function_effects = no_state, no_read(1), no_write(2), returns 1, always_returns") __intrinsic __nounwind void * memmove(void * _D, const void * _S, size_t _N)
  {
    __aeabi_memmove(_D, _S, _N);
    return _D;
  }

  #pragma inline=forced_no_body
  _Pragma("function_effects = no_state, no_read(1), returns 1, always_returns") __intrinsic __nounwind void * memset(void * _D, int _C, size_t _N)
  {
    __aeabi_memset(_D, _N, _C);
    return _D;
  }




 

 

 

  _Pragma("function_effects = no_state, no_write(1,2), always_returns")   __intrinsic __nounwind   int       memcmp(const void *, const void *,
                                                   size_t);
  _Pragma("function_effects = no_state, no_read(1), no_write(2), returns 1, always_returns")  __intrinsic __nounwind void *    memcpy(void *restrict,
                                                   const void *restrict,
                                                   size_t);
  _Pragma("function_effects = no_state, no_read(1), no_write(2), returns 1, always_returns")  __intrinsic __nounwind void *    memmove(void *, const void *,
                                                    size_t);
  _Pragma("function_effects = no_state, no_read(1), returns 1, always_returns")     __intrinsic __nounwind void *    memset(void *, int, size_t);
  _Pragma("function_effects = no_state, no_write(2), returns 1, always_returns")     __intrinsic __nounwind char *    strcat(char *restrict,
                                                   const char *restrict);
  _Pragma("function_effects = no_state, no_write(1,2), always_returns")   __intrinsic __nounwind   int       strcmp(const char *, const char *);
  _Pragma("function_effects = no_write(1,2), always_returns")     __intrinsic __nounwind   int       strcoll(const char *, const char *);
  _Pragma("function_effects = no_state, no_read(1), no_write(2), returns 1, always_returns")  __intrinsic __nounwind char *    strcpy(char *restrict,
                                                   const char *restrict);
  _Pragma("function_effects = no_state, no_write(1,2), always_returns")   __intrinsic __nounwind   size_t    strcspn(const char *, const char *);
                    __intrinsic __nounwind char *    strerror(int);
  _Pragma("function_effects = no_state, no_write(1), always_returns")      __intrinsic __nounwind   size_t    strlen(const char *);
  _Pragma("function_effects = no_state, no_write(2), returns 1, always_returns")     __intrinsic __nounwind char *    strncat(char *restrict,
                                                    const char *restrict,
                                                    size_t);
  _Pragma("function_effects = no_state, no_write(1,2), always_returns")   __intrinsic __nounwind   int       strncmp(const char *, const char *,
                                                    size_t);
  _Pragma("function_effects = no_state, no_read(1), no_write(2), returns 1, always_returns")  __intrinsic __nounwind char *    strncpy(char *restrict,
                                                    const char *restrict,
                                                    size_t);
  _Pragma("function_effects = no_state, no_write(1,2), always_returns")   __intrinsic __nounwind   size_t    strspn(const char *, const char *);
  _Pragma("function_effects = no_write(2), always_returns")         __intrinsic __nounwind char *    strtok(char *restrict,
                                                   const char *restrict);
  _Pragma("function_effects = no_write(2), always_returns")        __intrinsic __nounwind   size_t    strxfrm(char *restrict,
                                                    const char *restrict,
                                                    size_t);
    _Pragma("function_effects = no_write(1), always_returns")      __intrinsic __nounwind   char *    strdup(const char *);
    _Pragma("function_effects = no_write(1,2), always_returns")   __intrinsic __nounwind   int       strcasecmp(const char *,
                                                       const char *);
    _Pragma("function_effects = no_write(1,2), always_returns")   __intrinsic __nounwind   int       strncasecmp(const char *,
                                                        const char *, size_t);
    _Pragma("function_effects = no_state, no_write(2), always_returns")    __intrinsic __nounwind   char *    strtok_r(char *, const char *,
                                                     char **);
    _Pragma("function_effects = no_state, no_write(1), always_returns")     __intrinsic __nounwind size_t    strnlen(char const *, size_t);


  _Pragma("function_effects = no_state, no_write(1), always_returns")    __intrinsic __nounwind void *memchr(const void *_S, int _C, size_t _N);
  _Pragma("function_effects = no_state, no_write(1), always_returns")    __intrinsic __nounwind char *strchr(const char *_S, int _C);
  _Pragma("function_effects = no_state, no_write(1,2), always_returns") __intrinsic __nounwind char *strpbrk(const char *_S, const char *_P);
  _Pragma("function_effects = no_state, no_write(1), always_returns")    __intrinsic __nounwind char *strrchr(const char *_S, int _C);
  _Pragma("function_effects = no_state, no_write(1,2), always_returns") __intrinsic __nounwind char *strstr(const char *_S, const char *_P);






 
 
 

  #pragma system_include

 
 

 

  #pragma system_include














 




 

  #pragma system_include




 

 
 




 

 
 




 

 
 


 



 
typedef unsigned int __iar_FlagUType;
typedef signed int  __iar_FlagSType;

typedef signed int  __iar_ExpType;













 




 



#pragma diag_suppress = Pe549 









 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isfinite32(float _X)
{
    signed int _Ix = __iar_fp2bits32(_X);
    return ((_Ix << 1) >> (23 + 1)) + 1;
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isfinite64(double _X)
{
    signed long long int _Ix = __iar_fp2bits64(_X);
    return ((_Ix << 1) >> (52 + 1)) + 1;
}


 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnan32(float _X)
{
    signed int _Ix = __iar_fp2bits32(_X) << 1;
    return (_Ix >> (23 + 1)) + 1 ? 0 : (_Ix << (31 - 23));
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnan64(double _X)
{
    signed long long int _Ix = __iar_fp2bits64(_X);
    return ((_Ix << 1) >> (52 + 1)) + 1
           ? 0
           : ((_Ix << (64 - 52)) != 0);
}




 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_iszero32(float _X)
{
    unsigned int _Ix = __iar_fp2bits32(_X);
    return (_Ix << 1) == 0;
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_iszero64(double _X)
{
    unsigned long long int _Ix = __iar_fp2bits64(_X);
    return (_Ix & ~(1ULL << 63)) == 0;
}





 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isinf32(float _X)
{
    signed int _Ix = __iar_fp2bits32(_X);
    return ((_Ix << 1) >> (23 + 1)) + 1
           ? 0
           : ((_Ix << (32 - 23)) == 0);
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isinf64(double _X)
{
    signed int _Ix = __iar_fpgethi64(_X);
    return ((_Ix << 1) >> (52 - 31)) + 1 
             ? 0 
             : ((_Ix << (64 - 52)) == 0);
}





 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_issubnormal32(float _X)
{
    unsigned int _Ix = __iar_fp2bits32(_X) & ~(1 << 31);
    return (_Ix != 0) && (_Ix < (1 << 23));
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_issubnormal64(double _X)
{
    unsigned long long int _Ix = __iar_fp2bits64(_X) & ~(1ULL << 63);
    return (_Ix != 0) && (_Ix < (1ULL << 52));
}




 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnormal32(float _X)
{
  signed int _Exp = ((signed int)
                          (__iar_fp2bits32(_X) << 1) >> (23 + 1));
  return ((_Exp + 1) >> 1);
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnormal64(double _X)
{
    signed int _Exp = 
      ((signed int)(__iar_fpgethi64(_X) << 1) >> (52 - 31));
    return ((_Exp + 1) >> 1);
}




 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_signbit32(float _X)
{
  unsigned int _Ix = __iar_fp2bits32(_X);
  return (_Ix >> 31);
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_signbit64(double _X)
{
  unsigned long long int _Ix = __iar_fp2bits64(_X);
  return (_Ix >> 63);
}





  typedef float float_t;
  typedef double double_t;



   
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       acos(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       asin(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       atan(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       atan2(double, double);
  _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind      double       ceil(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       cos(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       cosh(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       exp(double);
  _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind      double       fabs(double);
  _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind      double       floor(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       fmod(double, double);
  _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind   double       frexp(double, int *);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       ldexp(double, int);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       log(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       log10(double);
  _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind   double       modf(double, double *);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       pow(double, double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       sin(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       sinh(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       sqrt(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       tan(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       tanh(double);

    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       acosh(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       asinh(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       atanh(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       cbrt(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       copysign(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       erf(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       erfc(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       expm1(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       exp2(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       fdim(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       fma(double, double, double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       fmax(double, double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       fmin(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       hypot(double, double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    int          ilogb(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       lgamma(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llrint(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llround(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   double       log1p(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   double       log2(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   double       logb(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   long         lrint(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   long         lround(double);
    _Pragma("function_effects = no_state, always_returns")  __intrinsic __nounwind   double       nan(const char *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       nearbyint(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       nextafter(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       nexttoward(double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       remainder(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       remquo(double, double, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       rint(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       round(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       scalbn(double, int);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       scalbln(double, long);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       tgamma(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       trunc(double);

     
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        acosf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        acoshf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        asinf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        asinhf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        atanf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        atanhf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        atan2f(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        ceilf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        coshf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        cosf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        cbrtf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        copysignf(float, float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        erff(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        erfcf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        expf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        expm1f(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        exp2f(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fabsf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fdimf(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        floorf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        fmaf(float, float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fmaxf(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fminf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        fmodf(float, float);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind float        frexpf(float, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        hypotf(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    int          ilogbf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        ldexpf(float, int);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        lgammaf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llrintf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llroundf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        logbf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        logf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        log1pf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        log2f(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        log10f(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lrintf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lroundf(float);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind float        modff(float, float *);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        nanf(const char *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        nearbyintf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        nextafterf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        nexttowardf(float, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        powf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        remainderf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        remquof(float, float, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        rintf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        roundf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        scalbnf(float, int);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        scalblnf(float, long);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        sinf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        sinhf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        sqrtf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        tanf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        tanhf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        tgammaf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        truncf(float);

     
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  acoshl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  acosl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  asinhl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  asinl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  atanl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  atanhl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  atan2l(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  ceill(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  cbrtl(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  copysignl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  coshl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  cosl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  erfl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  erfcl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  expl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  expm1l(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  exp2l(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fabsl(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fdiml(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  floorl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  fmal(long double, long double,
                                               long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fmaxl(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fminl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  fmodl(long double, long double);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind long double  frexpl(long double, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  hypotl(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    int          ilogbl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  ldexpl(long double, int);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  lgammal(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llrintl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llroundl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  logbl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  logl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  log1pl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  log10l(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  log2l(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lrintl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lroundl(long double);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind long double  modfl(long double, long double *);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  nanl(const char *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  nearbyintl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  nextafterl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  nexttowardl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  powl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  remainderl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  remquol(long double, long double,
                                                  int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  rintl(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  roundl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  scalbnl(long double, int);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  scalblnl(long double, long);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  sinhl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  sinl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  sqrtl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  tanl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  tanhl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  tgammal(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  truncl(long double);

   
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_cos_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_exp_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_log_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_log10_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_log2_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_pow_medium(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_sin_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_tan_medium(double);

    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_cos_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_exp_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_log_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_log10_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_log2_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_pow_mediumf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_sin_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_tan_mediumf(float);

    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_cos_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_exp_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_log_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_log10_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_log2_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_pow_mediuml(long double,
                                                           long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_sin_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_tan_mediuml(long double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_cos_accurate(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_pow_accurate(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_sin_accurate(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_tan_accurate(double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_cos_accuratef(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_pow_accuratef(float, float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_sin_accuratef(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_tan_accuratef(float);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_cos_accuratel(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_pow_accuratel(long double,
                                                              long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_sin_accuratel(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_tan_accuratel(long double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_cos_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_exp_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_log_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_log10_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_log2_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_pow_small(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_sin_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_tan_small(double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_cos_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_exp_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_log_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_log10_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_log2_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_pow_smallf(float, float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_sin_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_tan_smallf(float);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_cos_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_exp_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_log_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_log10_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_log2_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_pow_smalll(long double,
                                                         long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_sin_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_tan_smalll(long double);




   
  enum __FPclass { __kInfinity, __kNan, __kFinite, __kDenorm, __kZero };

  #pragma inline=forced
  __intrinsic int __iar_FPclassify32(float x)
  {
    if (!__iar_isfinite32(x))
    {
      if (__iar_isnan32(x))
      {
        return __kNan;
      }
      return __kInfinity;
    }
    if (__iar_iszero32(x))
    {
      return __kZero;
    }
    if (__iar_issubnormal32(x))
    {
      return __kDenorm;
    }
    return __kFinite;
  }

    #pragma inline=forced
    __intrinsic int __iar_FPclassify64(double x)
    {
      if (!__iar_isfinite64(x))
      {
        if (__iar_isnan64(x))
        {
          return __kNan;
        }
        return __kInfinity;
      }
      if (__iar_iszero64(x))
      {
        return __kZero;
      }
      if (__iar_issubnormal64(x))
      {
        return __kDenorm;
      }
      return __kFinite;
    }

   





   




    #pragma inline
    __iar_FlagSType __isnormalf(float _Left)
    {       
      return !__iar_isnan32(_Left) && __iar_isnormal32(_Left);
    }
    #pragma inline
    __iar_FlagSType __isnormal(double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isnormal64(_Left);
    }
    #pragma inline
    __iar_FlagSType __isnormall(long double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isnormal64(_Left);
    }
    #pragma inline
     __iar_FlagSType __isinff(float _Left)
    {       
      return !__iar_isnan32(_Left) && __iar_isinf32(_Left);
    }
    #pragma inline
     __iar_FlagSType __isinf(double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isinf64(_Left);
    }
    #pragma inline
     __iar_FlagSType __isinfl(long double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isinf64(_Left);
    }




    #pragma inline
    __iar_FlagSType __isunorderedf(float _x, float _y)
    {
      return __iar_isnan32(_x) || __iar_isnan32(_y);
    }
    #pragma inline
    __iar_FlagSType __isunordered(double _x, double _y)
    {
      return __iar_isnan64(_x) || __iar_isnan64(_y);
    }
    #pragma inline
    __iar_FlagSType __isunorderedl(long double _x, long double _y)
    {
      return __iar_isnan64(_x) || __iar_isnan64(_y);
    }
    #pragma inline
    __iar_FlagSType __islessgreaterf(float _x, float _y)
    {
      if (__isunorderedf(_x, _y))
        return 0;
      return _x < _y || _x > _y;
    }
    #pragma inline
    __iar_FlagSType __islessgreater(double _x, double _y)
    {
      if (__isunordered(_x, _y))
        return 0;
      return _x < _y || _x > _y;
    }
    #pragma inline
    __iar_FlagSType __islessgreaterl(long double _x, long double _y)
    {
      if (__isunorderedl(_x, _y))
        return 0;
      return _x < _y || _x > _y;
    }







 


  

 


  

 


  

 
   
   

  

 

  

 

  typedef enum
  {
    ARM_MATH_SUCCESS = 0,                 
    ARM_MATH_ARGUMENT_ERROR = -1,         
    ARM_MATH_LENGTH_ERROR = -2,           
    ARM_MATH_SIZE_MISMATCH = -3,          
    ARM_MATH_NANINF = -4,                 
    ARM_MATH_SINGULAR = -5,               
    ARM_MATH_TEST_FAILURE = -6            
  } arm_status;

  

 
  typedef int8_t q7_t;

  

 
  typedef int16_t q15_t;

  

 
  typedef int32_t q31_t;

  

 
  typedef int64_t q63_t;

  

 
  typedef float float32_t;

  

 
  typedef double float64_t;

  

 




   

 



  

 
   static inline q31_t clip_q63_to_q31(
  q63_t x)
  {
    return ((q31_t) (x >> 32) != ((q31_t) x >> 31)) ?
      ((0x7FFFFFFF ^ ((q31_t) (x >> 63)))) : (q31_t) x;
  }

  

 
   static inline q15_t clip_q63_to_q15(
  q63_t x)
  {
    return ((q31_t) (x >> 32) != ((q31_t) x >> 31)) ?
      ((0x7FFF ^ ((q15_t) (x >> 63)))) : (q15_t) (x >> 15);
  }

  

 
   static inline q7_t clip_q31_to_q7(
  q31_t x)
  {
    return ((q31_t) (x >> 24) != ((q31_t) x >> 23)) ?
      ((0x7F ^ ((q7_t) (x >> 31)))) : (q7_t) x;
  }

  

 
   static inline q15_t clip_q31_to_q15(
  q31_t x)
  {
    return ((q31_t) (x >> 16) != ((q31_t) x >> 15)) ?
      ((0x7FFF ^ ((q15_t) (x >> 31)))) : (q15_t) x;
  }

  

 

   static inline q63_t mult32x64(
  q63_t x,
  q31_t y)
  {
    return ((((q63_t) (x & 0x00000000FFFFFFFF) * y) >> 32) +
            (((q63_t) (x >> 32) * y)));
  }

  

 

   static inline uint32_t arm_recip_q31(
  q31_t in,
  q31_t * dst,
  q31_t * pRecipTable)
  {
    q31_t out;
    uint32_t tempVal;
    uint32_t index, i;
    uint32_t signBits;

    if (in > 0)
    {
      signBits = ((uint32_t) (__iar_builtin_CLZ( in) - 1));
    }
    else
    {
      signBits = ((uint32_t) (__iar_builtin_CLZ(-in) - 1));
    }

     
    in = (in << signBits);

     
    index = (uint32_t)(in >> 24);
    index = (index & 0x0000003F);

     
    out = pRecipTable[index];

     
     
    for (i = 0U; i < 2U; i++)
    {
      tempVal = (uint32_t) (((q63_t) in * out) >> 31);
      tempVal = 0x7FFFFFFFu - tempVal;
       
       
      out = clip_q63_to_q31(((q63_t) out * tempVal) >> 30);
    }

     
    *dst = out;

     
    return (signBits + 1U);
  }


  

 
   static inline uint32_t arm_recip_q15(
  q15_t in,
  q15_t * dst,
  q15_t * pRecipTable)
  {
    q15_t out = 0;
    uint32_t tempVal = 0;
    uint32_t index = 0, i = 0;
    uint32_t signBits = 0;

    if (in > 0)
    {
      signBits = ((uint32_t)(__iar_builtin_CLZ( in) - 17));
    }
    else
    {
      signBits = ((uint32_t)(__iar_builtin_CLZ(-in) - 17));
    }

     
    in = (in << signBits);

     
    index = (uint32_t)(in >>  8);
    index = (index & 0x0000003F);

     
    out = pRecipTable[index];

     
     
    for (i = 0U; i < 2U; i++)
    {
      tempVal = (uint32_t) (((q31_t) in * out) >> 15);
      tempVal = 0x7FFFu - tempVal;
       
      out = (q15_t) (((q31_t) out * tempVal) >> 14);
       
    }

     
    *dst = out;

     
    return (signBits + 1);
  }




 


  

 
  typedef struct
  {
    uint16_t numTaps;         
    q7_t *pState;             
    q7_t *pCoeffs;            
  } arm_fir_instance_q7;

  

 
  typedef struct
  {
    uint16_t numTaps;          
    q15_t *pState;             
    q15_t *pCoeffs;            
  } arm_fir_instance_q15;

  

 
  typedef struct
  {
    uint16_t numTaps;          
    q31_t *pState;             
    q31_t *pCoeffs;            
  } arm_fir_instance_q31;

  

 
  typedef struct
  {
    uint16_t numTaps;      
    float32_t *pState;     
    float32_t *pCoeffs;    
  } arm_fir_instance_f32;


  





 
  void arm_fir_q7(
  const arm_fir_instance_q7 * S,
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  






 
  void arm_fir_init_q7(
  arm_fir_instance_q7 * S,
  uint16_t numTaps,
  q7_t * pCoeffs,
  q7_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_q15(
  const arm_fir_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_fir_fast_q15(
  const arm_fir_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  








 
  arm_status arm_fir_init_q15(
  arm_fir_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_q31(
  const arm_fir_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_fir_fast_q31(
  const arm_fir_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  






 
  void arm_fir_init_q31(
  arm_fir_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_f32(
  const arm_fir_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  






 
  void arm_fir_init_f32(
  arm_fir_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);


  

 
  typedef struct
  {
    int8_t numStages;         
    q15_t *pState;            
    q15_t *pCoeffs;           
    int8_t postShift;         
  } arm_biquad_casd_df1_inst_q15;

  

 
  typedef struct
  {
    uint32_t numStages;       
    q31_t *pState;            
    q31_t *pCoeffs;           
    uint8_t postShift;        
  } arm_biquad_casd_df1_inst_q31;

  

 
  typedef struct
  {
    uint32_t numStages;       
    float32_t *pState;        
    float32_t *pCoeffs;       
  } arm_biquad_casd_df1_inst_f32;


  





 
  void arm_biquad_cascade_df1_q15(
  const arm_biquad_casd_df1_inst_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  






 
  void arm_biquad_cascade_df1_init_q15(
  arm_biquad_casd_df1_inst_q15 * S,
  uint8_t numStages,
  q15_t * pCoeffs,
  q15_t * pState,
  int8_t postShift);


  





 
  void arm_biquad_cascade_df1_fast_q15(
  const arm_biquad_casd_df1_inst_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cascade_df1_q31(
  const arm_biquad_casd_df1_inst_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cascade_df1_fast_q31(
  const arm_biquad_casd_df1_inst_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  






 
  void arm_biquad_cascade_df1_init_q31(
  arm_biquad_casd_df1_inst_q31 * S,
  uint8_t numStages,
  q31_t * pCoeffs,
  q31_t * pState,
  int8_t postShift);


  





 
  void arm_biquad_cascade_df1_f32(
  const arm_biquad_casd_df1_inst_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cascade_df1_init_f32(
  arm_biquad_casd_df1_inst_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);


  

 
  typedef struct
  {
    uint16_t numRows;      
    uint16_t numCols;      
    float32_t *pData;      
  } arm_matrix_instance_f32;


  

 
  typedef struct
  {
    uint16_t numRows;      
    uint16_t numCols;      
    float64_t *pData;      
  } arm_matrix_instance_f64;

  

 
  typedef struct
  {
    uint16_t numRows;      
    uint16_t numCols;      
    q15_t *pData;          
  } arm_matrix_instance_q15;

  

 
  typedef struct
  {
    uint16_t numRows;      
    uint16_t numCols;      
    q31_t *pData;          
  } arm_matrix_instance_q31;


  






 
  arm_status arm_mat_add_f32(
  const arm_matrix_instance_f32 * pSrcA,
  const arm_matrix_instance_f32 * pSrcB,
  arm_matrix_instance_f32 * pDst);


  






 
  arm_status arm_mat_add_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst);


  






 
  arm_status arm_mat_add_q31(
  const arm_matrix_instance_q31 * pSrcA,
  const arm_matrix_instance_q31 * pSrcB,
  arm_matrix_instance_q31 * pDst);


  






 
  arm_status arm_mat_cmplx_mult_f32(
  const arm_matrix_instance_f32 * pSrcA,
  const arm_matrix_instance_f32 * pSrcB,
  arm_matrix_instance_f32 * pDst);


  






 
  arm_status arm_mat_cmplx_mult_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst,
  q15_t * pScratch);


  






 
  arm_status arm_mat_cmplx_mult_q31(
  const arm_matrix_instance_q31 * pSrcA,
  const arm_matrix_instance_q31 * pSrcB,
  arm_matrix_instance_q31 * pDst);


  





 
  arm_status arm_mat_trans_f32(
  const arm_matrix_instance_f32 * pSrc,
  arm_matrix_instance_f32 * pDst);


  





 
  arm_status arm_mat_trans_q15(
  const arm_matrix_instance_q15 * pSrc,
  arm_matrix_instance_q15 * pDst);


  





 
  arm_status arm_mat_trans_q31(
  const arm_matrix_instance_q31 * pSrc,
  arm_matrix_instance_q31 * pDst);


  






 
  arm_status arm_mat_mult_f32(
  const arm_matrix_instance_f32 * pSrcA,
  const arm_matrix_instance_f32 * pSrcB,
  arm_matrix_instance_f32 * pDst);


  







 
  arm_status arm_mat_mult_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst,
  q15_t * pState);


  







 
  arm_status arm_mat_mult_fast_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst,
  q15_t * pState);


  






 
  arm_status arm_mat_mult_q31(
  const arm_matrix_instance_q31 * pSrcA,
  const arm_matrix_instance_q31 * pSrcB,
  arm_matrix_instance_q31 * pDst);


  






 
  arm_status arm_mat_mult_fast_q31(
  const arm_matrix_instance_q31 * pSrcA,
  const arm_matrix_instance_q31 * pSrcB,
  arm_matrix_instance_q31 * pDst);


  






 
  arm_status arm_mat_sub_f32(
  const arm_matrix_instance_f32 * pSrcA,
  const arm_matrix_instance_f32 * pSrcB,
  arm_matrix_instance_f32 * pDst);


  






 
  arm_status arm_mat_sub_q15(
  const arm_matrix_instance_q15 * pSrcA,
  const arm_matrix_instance_q15 * pSrcB,
  arm_matrix_instance_q15 * pDst);


  






 
  arm_status arm_mat_sub_q31(
  const arm_matrix_instance_q31 * pSrcA,
  const arm_matrix_instance_q31 * pSrcB,
  arm_matrix_instance_q31 * pDst);


  






 
  arm_status arm_mat_scale_f32(
  const arm_matrix_instance_f32 * pSrc,
  float32_t scale,
  arm_matrix_instance_f32 * pDst);


  







 
  arm_status arm_mat_scale_q15(
  const arm_matrix_instance_q15 * pSrc,
  q15_t scaleFract,
  int32_t shift,
  arm_matrix_instance_q15 * pDst);


  







 
  arm_status arm_mat_scale_q31(
  const arm_matrix_instance_q31 * pSrc,
  q31_t scaleFract,
  int32_t shift,
  arm_matrix_instance_q31 * pDst);


  





 
  void arm_mat_init_q31(
  arm_matrix_instance_q31 * S,
  uint16_t nRows,
  uint16_t nColumns,
  q31_t * pData);


  





 
  void arm_mat_init_q15(
  arm_matrix_instance_q15 * S,
  uint16_t nRows,
  uint16_t nColumns,
  q15_t * pData);


  





 
  void arm_mat_init_f32(
  arm_matrix_instance_f32 * S,
  uint16_t nRows,
  uint16_t nColumns,
  float32_t * pData);



  

 
  typedef struct
  {
    q15_t A0;            
    q31_t A1;            
    q15_t state[3];      
    q15_t Kp;            
    q15_t Ki;            
    q15_t Kd;            
  } arm_pid_instance_q15;

  

 
  typedef struct
  {
    q31_t A0;             
    q31_t A1;             
    q31_t A2;             
    q31_t state[3];       
    q31_t Kp;             
    q31_t Ki;             
    q31_t Kd;             
  } arm_pid_instance_q31;

  

 
  typedef struct
  {
    float32_t A0;           
    float32_t A1;           
    float32_t A2;           
    float32_t state[3];     
    float32_t Kp;           
    float32_t Ki;           
    float32_t Kd;           
  } arm_pid_instance_f32;



  



 
  void arm_pid_init_f32(
  arm_pid_instance_f32 * S,
  int32_t resetStateFlag);


  


 
  void arm_pid_reset_f32(
  arm_pid_instance_f32 * S);


  



 
  void arm_pid_init_q31(
  arm_pid_instance_q31 * S,
  int32_t resetStateFlag);


  


 

  void arm_pid_reset_q31(
  arm_pid_instance_q31 * S);


  



 
  void arm_pid_init_q15(
  arm_pid_instance_q15 * S,
  int32_t resetStateFlag);


  


 
  void arm_pid_reset_q15(
  arm_pid_instance_q15 * S);


  

 
  typedef struct
  {
    uint32_t nValues;            
    float32_t x1;                
    float32_t xSpacing;          
    float32_t *pYData;           
  } arm_linear_interp_instance_f32;

  

 
  typedef struct
  {
    uint16_t numRows;    
    uint16_t numCols;    
    float32_t *pData;    
  } arm_bilinear_interp_instance_f32;

   

 
  typedef struct
  {
    uint16_t numRows;    
    uint16_t numCols;    
    q31_t *pData;        
  } arm_bilinear_interp_instance_q31;

   

 
  typedef struct
  {
    uint16_t numRows;    
    uint16_t numCols;    
    q15_t *pData;        
  } arm_bilinear_interp_instance_q15;

   

 
  typedef struct
  {
    uint16_t numRows;    
    uint16_t numCols;    
    q7_t *pData;         
  } arm_bilinear_interp_instance_q7;


  





 
  void arm_mult_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize);


  





 
  void arm_mult_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_mult_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_mult_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint16_t fftLen;                  
    uint8_t ifftFlag;                 
    uint8_t bitReverseFlag;           
    q15_t *pTwiddle;                  
    uint16_t *pBitRevTable;           
    uint16_t twidCoefModifier;        
    uint16_t bitRevFactor;            
  } arm_cfft_radix2_instance_q15;

 
  arm_status arm_cfft_radix2_init_q15(
  arm_cfft_radix2_instance_q15 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

 
  void arm_cfft_radix2_q15(
  const arm_cfft_radix2_instance_q15 * S,
  q15_t * pSrc);


  

 
  typedef struct
  {
    uint16_t fftLen;                  
    uint8_t ifftFlag;                 
    uint8_t bitReverseFlag;           
    q15_t *pTwiddle;                  
    uint16_t *pBitRevTable;           
    uint16_t twidCoefModifier;        
    uint16_t bitRevFactor;            
  } arm_cfft_radix4_instance_q15;

 
  arm_status arm_cfft_radix4_init_q15(
  arm_cfft_radix4_instance_q15 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

 
  void arm_cfft_radix4_q15(
  const arm_cfft_radix4_instance_q15 * S,
  q15_t * pSrc);

  

 
  typedef struct
  {
    uint16_t fftLen;                  
    uint8_t ifftFlag;                 
    uint8_t bitReverseFlag;           
    q31_t *pTwiddle;                  
    uint16_t *pBitRevTable;           
    uint16_t twidCoefModifier;        
    uint16_t bitRevFactor;            
  } arm_cfft_radix2_instance_q31;

 
  arm_status arm_cfft_radix2_init_q31(
  arm_cfft_radix2_instance_q31 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

 
  void arm_cfft_radix2_q31(
  const arm_cfft_radix2_instance_q31 * S,
  q31_t * pSrc);

  

 
  typedef struct
  {
    uint16_t fftLen;                  
    uint8_t ifftFlag;                 
    uint8_t bitReverseFlag;           
    q31_t *pTwiddle;                  
    uint16_t *pBitRevTable;           
    uint16_t twidCoefModifier;        
    uint16_t bitRevFactor;            
  } arm_cfft_radix4_instance_q31;

 
  void arm_cfft_radix4_q31(
  const arm_cfft_radix4_instance_q31 * S,
  q31_t * pSrc);

 
  arm_status arm_cfft_radix4_init_q31(
  arm_cfft_radix4_instance_q31 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

  

 
  typedef struct
  {
    uint16_t fftLen;                    
    uint8_t ifftFlag;                   
    uint8_t bitReverseFlag;             
    float32_t *pTwiddle;                
    uint16_t *pBitRevTable;             
    uint16_t twidCoefModifier;          
    uint16_t bitRevFactor;              
    float32_t onebyfftLen;              
  } arm_cfft_radix2_instance_f32;

 
  arm_status arm_cfft_radix2_init_f32(
  arm_cfft_radix2_instance_f32 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

 
  void arm_cfft_radix2_f32(
  const arm_cfft_radix2_instance_f32 * S,
  float32_t * pSrc);

  

 
  typedef struct
  {
    uint16_t fftLen;                    
    uint8_t ifftFlag;                   
    uint8_t bitReverseFlag;             
    float32_t *pTwiddle;                
    uint16_t *pBitRevTable;             
    uint16_t twidCoefModifier;          
    uint16_t bitRevFactor;              
    float32_t onebyfftLen;              
  } arm_cfft_radix4_instance_f32;

 
  arm_status arm_cfft_radix4_init_f32(
  arm_cfft_radix4_instance_f32 * S,
  uint16_t fftLen,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

 
  void arm_cfft_radix4_f32(
  const arm_cfft_radix4_instance_f32 * S,
  float32_t * pSrc);

  

 
  typedef struct
  {
    uint16_t fftLen;                    
    const q15_t *pTwiddle;              
    const uint16_t *pBitRevTable;       
    uint16_t bitRevLength;              
  } arm_cfft_instance_q15;

void arm_cfft_q15(
    const arm_cfft_instance_q15 * S,
    q15_t * p1,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag);

  

 
  typedef struct
  {
    uint16_t fftLen;                    
    const q31_t *pTwiddle;              
    const uint16_t *pBitRevTable;       
    uint16_t bitRevLength;              
  } arm_cfft_instance_q31;

void arm_cfft_q31(
    const arm_cfft_instance_q31 * S,
    q31_t * p1,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag);

  

 
  typedef struct
  {
    uint16_t fftLen;                    
    const float32_t *pTwiddle;          
    const uint16_t *pBitRevTable;       
    uint16_t bitRevLength;              
  } arm_cfft_instance_f32;

  void arm_cfft_f32(
  const arm_cfft_instance_f32 * S,
  float32_t * p1,
  uint8_t ifftFlag,
  uint8_t bitReverseFlag);

  

 
  typedef struct
  {
    uint32_t fftLenReal;                       
    uint8_t ifftFlagR;                         
    uint8_t bitReverseFlagR;                   
    uint32_t twidCoefRModifier;                
    q15_t *pTwiddleAReal;                      
    q15_t *pTwiddleBReal;                      
    const arm_cfft_instance_q15 *pCfft;        
  } arm_rfft_instance_q15;

  arm_status arm_rfft_init_q15(
  arm_rfft_instance_q15 * S,
  uint32_t fftLenReal,
  uint32_t ifftFlagR,
  uint32_t bitReverseFlag);

  void arm_rfft_q15(
  const arm_rfft_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst);

  

 
  typedef struct
  {
    uint32_t fftLenReal;                         
    uint8_t ifftFlagR;                           
    uint8_t bitReverseFlagR;                     
    uint32_t twidCoefRModifier;                  
    q31_t *pTwiddleAReal;                        
    q31_t *pTwiddleBReal;                        
    const arm_cfft_instance_q31 *pCfft;          
  } arm_rfft_instance_q31;

  arm_status arm_rfft_init_q31(
  arm_rfft_instance_q31 * S,
  uint32_t fftLenReal,
  uint32_t ifftFlagR,
  uint32_t bitReverseFlag);

  void arm_rfft_q31(
  const arm_rfft_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst);

  

 
  typedef struct
  {
    uint32_t fftLenReal;                         
    uint16_t fftLenBy2;                          
    uint8_t ifftFlagR;                           
    uint8_t bitReverseFlagR;                     
    uint32_t twidCoefRModifier;                      
    float32_t *pTwiddleAReal;                    
    float32_t *pTwiddleBReal;                    
    arm_cfft_radix4_instance_f32 *pCfft;         
  } arm_rfft_instance_f32;

  arm_status arm_rfft_init_f32(
  arm_rfft_instance_f32 * S,
  arm_cfft_radix4_instance_f32 * S_CFFT,
  uint32_t fftLenReal,
  uint32_t ifftFlagR,
  uint32_t bitReverseFlag);

  void arm_rfft_f32(
  const arm_rfft_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst);

  

 
typedef struct
  {
    arm_cfft_instance_f32 Sint;       
    uint16_t fftLenRFFT;              
    float32_t * pTwiddleRFFT;         
  } arm_rfft_fast_instance_f32 ;

arm_status arm_rfft_fast_init_f32 (
   arm_rfft_fast_instance_f32 * S,
   uint16_t fftLen);

void arm_rfft_fast_f32(
  arm_rfft_fast_instance_f32 * S,
  float32_t * p, float32_t * pOut,
  uint8_t ifftFlag);

  

 
  typedef struct
  {
    uint16_t N;                           
    uint16_t Nby2;                        
    float32_t normalize;                  
    float32_t *pTwiddle;                  
    float32_t *pCosFactor;                
    arm_rfft_instance_f32 *pRfft;         
    arm_cfft_radix4_instance_f32 *pCfft;  
  } arm_dct4_instance_f32;


  








 
  arm_status arm_dct4_init_f32(
  arm_dct4_instance_f32 * S,
  arm_rfft_instance_f32 * S_RFFT,
  arm_cfft_radix4_instance_f32 * S_CFFT,
  uint16_t N,
  uint16_t Nby2,
  float32_t normalize);


  




 
  void arm_dct4_f32(
  const arm_dct4_instance_f32 * S,
  float32_t * pState,
  float32_t * pInlineBuffer);


  

 
  typedef struct
  {
    uint16_t N;                           
    uint16_t Nby2;                        
    q31_t normalize;                      
    q31_t *pTwiddle;                      
    q31_t *pCosFactor;                    
    arm_rfft_instance_q31 *pRfft;         
    arm_cfft_radix4_instance_q31 *pCfft;  
  } arm_dct4_instance_q31;


  








 
  arm_status arm_dct4_init_q31(
  arm_dct4_instance_q31 * S,
  arm_rfft_instance_q31 * S_RFFT,
  arm_cfft_radix4_instance_q31 * S_CFFT,
  uint16_t N,
  uint16_t Nby2,
  q31_t normalize);


  




 
  void arm_dct4_q31(
  const arm_dct4_instance_q31 * S,
  q31_t * pState,
  q31_t * pInlineBuffer);


  

 
  typedef struct
  {
    uint16_t N;                           
    uint16_t Nby2;                        
    q15_t normalize;                      
    q15_t *pTwiddle;                      
    q15_t *pCosFactor;                    
    arm_rfft_instance_q15 *pRfft;         
    arm_cfft_radix4_instance_q15 *pCfft;  
  } arm_dct4_instance_q15;


  








 
  arm_status arm_dct4_init_q15(
  arm_dct4_instance_q15 * S,
  arm_rfft_instance_q15 * S_RFFT,
  arm_cfft_radix4_instance_q15 * S_CFFT,
  uint16_t N,
  uint16_t Nby2,
  q15_t normalize);


  




 
  void arm_dct4_q15(
  const arm_dct4_instance_q15 * S,
  q15_t * pState,
  q15_t * pInlineBuffer);


  





 
  void arm_add_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);


  





 
  void arm_add_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize);


  





 
  void arm_add_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_add_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_sub_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t blockSize);


  





 
  void arm_sub_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  uint32_t blockSize);


  





 
  void arm_sub_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_sub_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_scale_f32(
  float32_t * pSrc,
  float32_t scale,
  float32_t * pDst,
  uint32_t blockSize);


  






 
  void arm_scale_q7(
  q7_t * pSrc,
  q7_t scaleFract,
  int8_t shift,
  q7_t * pDst,
  uint32_t blockSize);


  






 
  void arm_scale_q15(
  q15_t * pSrc,
  q15_t scaleFract,
  int8_t shift,
  q15_t * pDst,
  uint32_t blockSize);


  






 
  void arm_scale_q31(
  q31_t * pSrc,
  q31_t scaleFract,
  int8_t shift,
  q31_t * pDst,
  uint32_t blockSize);


  




 
  void arm_abs_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  




 
  void arm_abs_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  




 
  void arm_abs_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  




 
  void arm_abs_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_dot_prod_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  uint32_t blockSize,
  float32_t * result);


  





 
  void arm_dot_prod_q7(
  q7_t * pSrcA,
  q7_t * pSrcB,
  uint32_t blockSize,
  q31_t * result);


  





 
  void arm_dot_prod_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  uint32_t blockSize,
  q63_t * result);


  





 
  void arm_dot_prod_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  uint32_t blockSize,
  q63_t * result);


  





 
  void arm_shift_q7(
  q7_t * pSrc,
  int8_t shiftBits,
  q7_t * pDst,
  uint32_t blockSize);


  





 
  void arm_shift_q15(
  q15_t * pSrc,
  int8_t shiftBits,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_shift_q31(
  q31_t * pSrc,
  int8_t shiftBits,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_offset_f32(
  float32_t * pSrc,
  float32_t offset,
  float32_t * pDst,
  uint32_t blockSize);


  





 
  void arm_offset_q7(
  q7_t * pSrc,
  q7_t offset,
  q7_t * pDst,
  uint32_t blockSize);


  





 
  void arm_offset_q15(
  q15_t * pSrc,
  q15_t offset,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_offset_q31(
  q31_t * pSrc,
  q31_t offset,
  q31_t * pDst,
  uint32_t blockSize);


  




 
  void arm_negate_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  




 
  void arm_negate_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  




 
  void arm_negate_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  




 
  void arm_negate_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  




 
  void arm_copy_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  




 
  void arm_copy_q7(
  q7_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  




 
  void arm_copy_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  




 
  void arm_copy_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  




 
  void arm_fill_f32(
  float32_t value,
  float32_t * pDst,
  uint32_t blockSize);


  




 
  void arm_fill_q7(
  q7_t value,
  q7_t * pDst,
  uint32_t blockSize);


  




 
  void arm_fill_q15(
  q15_t value,
  q15_t * pDst,
  uint32_t blockSize);


  




 
  void arm_fill_q31(
  q31_t value,
  q31_t * pDst,
  uint32_t blockSize);









 
  void arm_conv_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst);


  








 
  void arm_conv_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);









 
  void arm_conv_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst);


  






 
  void arm_conv_fast_q15(
          q15_t * pSrcA,
          uint32_t srcALen,
          q15_t * pSrcB,
          uint32_t srcBLen,
          q15_t * pDst);


  








 
  void arm_conv_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);


  






 
  void arm_conv_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);


  






 
  void arm_conv_fast_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);


    








 
  void arm_conv_opt_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);


  






 
  void arm_conv_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst);


  









 
  arm_status arm_conv_partial_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);


  











 
  arm_status arm_conv_partial_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2);


  









 
  arm_status arm_conv_partial_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);


  









 
  arm_status arm_conv_partial_fast_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);


  











 
  arm_status arm_conv_partial_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2);


  









 
  arm_status arm_conv_partial_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);


  









 
  arm_status arm_conv_partial_fast_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);


  











 
  arm_status arm_conv_partial_opt_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints,
  q15_t * pScratch1,
  q15_t * pScratch2);












 
  arm_status arm_conv_partial_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  uint32_t firstIndex,
  uint32_t numPoints);


  

 
  typedef struct
  {
    uint8_t M;                   
    uint16_t numTaps;            
    q15_t *pCoeffs;              
    q15_t *pState;               
  } arm_fir_decimate_instance_q15;

  

 
  typedef struct
  {
    uint8_t M;                   
    uint16_t numTaps;            
    q31_t *pCoeffs;              
    q31_t *pState;               
  } arm_fir_decimate_instance_q31;

  

 
  typedef struct
  {
    uint8_t M;                   
    uint16_t numTaps;            
    float32_t *pCoeffs;          
    float32_t *pState;           
  } arm_fir_decimate_instance_f32;


  





 
  void arm_fir_decimate_f32(
  const arm_fir_decimate_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  









 
  arm_status arm_fir_decimate_init_f32(
  arm_fir_decimate_instance_f32 * S,
  uint16_t numTaps,
  uint8_t M,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_decimate_q15(
  const arm_fir_decimate_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_fir_decimate_fast_q15(
  const arm_fir_decimate_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  









 
  arm_status arm_fir_decimate_init_q15(
  arm_fir_decimate_instance_q15 * S,
  uint16_t numTaps,
  uint8_t M,
  q15_t * pCoeffs,
  q15_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_decimate_q31(
  const arm_fir_decimate_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);

  





 
  void arm_fir_decimate_fast_q31(
  arm_fir_decimate_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  









 
  arm_status arm_fir_decimate_init_q31(
  arm_fir_decimate_instance_q31 * S,
  uint16_t numTaps,
  uint8_t M,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint8_t L;                       
    uint16_t phaseLength;            
    q15_t *pCoeffs;                  
    q15_t *pState;                   
  } arm_fir_interpolate_instance_q15;

  

 
  typedef struct
  {
    uint8_t L;                       
    uint16_t phaseLength;            
    q31_t *pCoeffs;                  
    q31_t *pState;                   
  } arm_fir_interpolate_instance_q31;

  

 
  typedef struct
  {
    uint8_t L;                      
    uint16_t phaseLength;           
    float32_t *pCoeffs;             
    float32_t *pState;              
  } arm_fir_interpolate_instance_f32;


  





 
  void arm_fir_interpolate_q15(
  const arm_fir_interpolate_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  









 
  arm_status arm_fir_interpolate_init_q15(
  arm_fir_interpolate_instance_q15 * S,
  uint8_t L,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_interpolate_q31(
  const arm_fir_interpolate_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  









 
  arm_status arm_fir_interpolate_init_q31(
  arm_fir_interpolate_instance_q31 * S,
  uint8_t L,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  uint32_t blockSize);


  





 
  void arm_fir_interpolate_f32(
  const arm_fir_interpolate_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  









 
  arm_status arm_fir_interpolate_init_f32(
  arm_fir_interpolate_instance_f32 * S,
  uint8_t L,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint8_t numStages;        
    q63_t *pState;            
    q31_t *pCoeffs;           
    uint8_t postShift;        
  } arm_biquad_cas_df1_32x64_ins_q31;


  




 
  void arm_biquad_cas_df1_32x64_q31(
  const arm_biquad_cas_df1_32x64_ins_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cas_df1_32x64_init_q31(
  arm_biquad_cas_df1_32x64_ins_q31 * S,
  uint8_t numStages,
  q31_t * pCoeffs,
  q63_t * pState,
  uint8_t postShift);


  

 
  typedef struct
  {
    uint8_t numStages;          
    float32_t *pState;          
    float32_t *pCoeffs;         
  } arm_biquad_cascade_df2T_instance_f32;

  

 
  typedef struct
  {
    uint8_t numStages;          
    float32_t *pState;          
    float32_t *pCoeffs;         
  } arm_biquad_cascade_stereo_df2T_instance_f32;

  

 
  typedef struct
  {
    uint8_t numStages;          
    float64_t *pState;          
    float64_t *pCoeffs;         
  } arm_biquad_cascade_df2T_instance_f64;


  





 
  void arm_biquad_cascade_df2T_f32(
  const arm_biquad_cascade_df2T_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cascade_stereo_df2T_f32(
  const arm_biquad_cascade_stereo_df2T_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cascade_df2T_f64(
  const arm_biquad_cascade_df2T_instance_f64 * S,
  float64_t * pSrc,
  float64_t * pDst,
  uint32_t blockSize);


  





 
  void arm_biquad_cascade_df2T_init_f32(
  arm_biquad_cascade_df2T_instance_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);


  





 
  void arm_biquad_cascade_stereo_df2T_init_f32(
  arm_biquad_cascade_stereo_df2T_instance_f32 * S,
  uint8_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);


  





 
  void arm_biquad_cascade_df2T_init_f64(
  arm_biquad_cascade_df2T_instance_f64 * S,
  uint8_t numStages,
  float64_t * pCoeffs,
  float64_t * pState);


  

 
  typedef struct
  {
    uint16_t numStages;                   
    q15_t *pState;                        
    q15_t *pCoeffs;                       
  } arm_fir_lattice_instance_q15;

  

 
  typedef struct
  {
    uint16_t numStages;                   
    q31_t *pState;                        
    q31_t *pCoeffs;                       
  } arm_fir_lattice_instance_q31;

  

 
  typedef struct
  {
    uint16_t numStages;                   
    float32_t *pState;                    
    float32_t *pCoeffs;                   
  } arm_fir_lattice_instance_f32;


  





 
  void arm_fir_lattice_init_q15(
  arm_fir_lattice_instance_q15 * S,
  uint16_t numStages,
  q15_t * pCoeffs,
  q15_t * pState);


  





 
  void arm_fir_lattice_q15(
  const arm_fir_lattice_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  





 
  void arm_fir_lattice_init_q31(
  arm_fir_lattice_instance_q31 * S,
  uint16_t numStages,
  q31_t * pCoeffs,
  q31_t * pState);


  





 
  void arm_fir_lattice_q31(
  const arm_fir_lattice_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);








 
  void arm_fir_lattice_init_f32(
  arm_fir_lattice_instance_f32 * S,
  uint16_t numStages,
  float32_t * pCoeffs,
  float32_t * pState);


  





 
  void arm_fir_lattice_f32(
  const arm_fir_lattice_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint16_t numStages;                   
    q15_t *pState;                        
    q15_t *pkCoeffs;                      
    q15_t *pvCoeffs;                      
  } arm_iir_lattice_instance_q15;

  

 
  typedef struct
  {
    uint16_t numStages;                   
    q31_t *pState;                        
    q31_t *pkCoeffs;                      
    q31_t *pvCoeffs;                      
  } arm_iir_lattice_instance_q31;

  

 
  typedef struct
  {
    uint16_t numStages;                   
    float32_t *pState;                    
    float32_t *pkCoeffs;                  
    float32_t *pvCoeffs;                  
  } arm_iir_lattice_instance_f32;


  





 
  void arm_iir_lattice_f32(
  const arm_iir_lattice_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  







 
  void arm_iir_lattice_init_f32(
  arm_iir_lattice_instance_f32 * S,
  uint16_t numStages,
  float32_t * pkCoeffs,
  float32_t * pvCoeffs,
  float32_t * pState,
  uint32_t blockSize);


  





 
  void arm_iir_lattice_q31(
  const arm_iir_lattice_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  







 
  void arm_iir_lattice_init_q31(
  arm_iir_lattice_instance_q31 * S,
  uint16_t numStages,
  q31_t * pkCoeffs,
  q31_t * pvCoeffs,
  q31_t * pState,
  uint32_t blockSize);


  





 
  void arm_iir_lattice_q15(
  const arm_iir_lattice_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);










 
  void arm_iir_lattice_init_q15(
  arm_iir_lattice_instance_q15 * S,
  uint16_t numStages,
  q15_t * pkCoeffs,
  q15_t * pvCoeffs,
  q15_t * pState,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint16_t numTaps;     
    float32_t *pState;    
    float32_t *pCoeffs;   
    float32_t mu;         
  } arm_lms_instance_f32;


  







 
  void arm_lms_f32(
  const arm_lms_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pRef,
  float32_t * pOut,
  float32_t * pErr,
  uint32_t blockSize);


  







 
  void arm_lms_init_f32(
  arm_lms_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  float32_t mu,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint16_t numTaps;     
    q15_t *pState;        
    q15_t *pCoeffs;       
    q15_t mu;             
    uint32_t postShift;   
  } arm_lms_instance_q15;


  








 
  void arm_lms_init_q15(
  arm_lms_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  q15_t mu,
  uint32_t blockSize,
  uint32_t postShift);


  







 
  void arm_lms_q15(
  const arm_lms_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pRef,
  q15_t * pOut,
  q15_t * pErr,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint16_t numTaps;     
    q31_t *pState;        
    q31_t *pCoeffs;       
    q31_t mu;             
    uint32_t postShift;   
  } arm_lms_instance_q31;


  







 
  void arm_lms_q31(
  const arm_lms_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pRef,
  q31_t * pOut,
  q31_t * pErr,
  uint32_t blockSize);


  








 
  void arm_lms_init_q31(
  arm_lms_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  q31_t mu,
  uint32_t blockSize,
  uint32_t postShift);


  

 
  typedef struct
  {
    uint16_t numTaps;      
    float32_t *pState;     
    float32_t *pCoeffs;    
    float32_t mu;          
    float32_t energy;      
    float32_t x0;          
  } arm_lms_norm_instance_f32;


  







 
  void arm_lms_norm_f32(
  arm_lms_norm_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pRef,
  float32_t * pOut,
  float32_t * pErr,
  uint32_t blockSize);


  







 
  void arm_lms_norm_init_f32(
  arm_lms_norm_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  float32_t mu,
  uint32_t blockSize);


  

 
  typedef struct
  {
    uint16_t numTaps;      
    q31_t *pState;         
    q31_t *pCoeffs;        
    q31_t mu;              
    uint8_t postShift;     
    q31_t *recipTable;     
    q31_t energy;          
    q31_t x0;              
  } arm_lms_norm_instance_q31;


  







 
  void arm_lms_norm_q31(
  arm_lms_norm_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pRef,
  q31_t * pOut,
  q31_t * pErr,
  uint32_t blockSize);


  








 
  void arm_lms_norm_init_q31(
  arm_lms_norm_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  q31_t mu,
  uint32_t blockSize,
  uint8_t postShift);


  

 
  typedef struct
  {
    uint16_t numTaps;      
    q15_t *pState;         
    q15_t *pCoeffs;        
    q15_t mu;              
    uint8_t postShift;     
    q15_t *recipTable;     
    q15_t energy;          
    q15_t x0;              
  } arm_lms_norm_instance_q15;


  







 
  void arm_lms_norm_q15(
  arm_lms_norm_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pRef,
  q15_t * pOut,
  q15_t * pErr,
  uint32_t blockSize);


  








 
  void arm_lms_norm_init_q15(
  arm_lms_norm_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  q15_t mu,
  uint32_t blockSize,
  uint8_t postShift);


  






 
  void arm_correlate_f32(
  float32_t * pSrcA,
  uint32_t srcALen,
  float32_t * pSrcB,
  uint32_t srcBLen,
  float32_t * pDst);


   







 
  void arm_correlate_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch);


  






 

  void arm_correlate_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst);


  






 

  void arm_correlate_fast_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst);


  







 
  void arm_correlate_fast_opt_q15(
  q15_t * pSrcA,
  uint32_t srcALen,
  q15_t * pSrcB,
  uint32_t srcBLen,
  q15_t * pDst,
  q15_t * pScratch);


  






 
  void arm_correlate_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);


  






 
  void arm_correlate_fast_q31(
  q31_t * pSrcA,
  uint32_t srcALen,
  q31_t * pSrcB,
  uint32_t srcBLen,
  q31_t * pDst);


 








 
  void arm_correlate_opt_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst,
  q15_t * pScratch1,
  q15_t * pScratch2);


  






 
  void arm_correlate_q7(
  q7_t * pSrcA,
  uint32_t srcALen,
  q7_t * pSrcB,
  uint32_t srcBLen,
  q7_t * pDst);


  

 
  typedef struct
  {
    uint16_t numTaps;              
    uint16_t stateIndex;           
    float32_t *pState;             
    float32_t *pCoeffs;            
    uint16_t maxDelay;             
    int32_t *pTapDelay;            
  } arm_fir_sparse_instance_f32;

  

 
  typedef struct
  {
    uint16_t numTaps;              
    uint16_t stateIndex;           
    q31_t *pState;                 
    q31_t *pCoeffs;                
    uint16_t maxDelay;             
    int32_t *pTapDelay;            
  } arm_fir_sparse_instance_q31;

  

 
  typedef struct
  {
    uint16_t numTaps;              
    uint16_t stateIndex;           
    q15_t *pState;                 
    q15_t *pCoeffs;                
    uint16_t maxDelay;             
    int32_t *pTapDelay;            
  } arm_fir_sparse_instance_q15;

  

 
  typedef struct
  {
    uint16_t numTaps;              
    uint16_t stateIndex;           
    q7_t *pState;                  
    q7_t *pCoeffs;                 
    uint16_t maxDelay;             
    int32_t *pTapDelay;            
  } arm_fir_sparse_instance_q7;


  






 
  void arm_fir_sparse_f32(
  arm_fir_sparse_instance_f32 * S,
  float32_t * pSrc,
  float32_t * pDst,
  float32_t * pScratchIn,
  uint32_t blockSize);


  








 
  void arm_fir_sparse_init_f32(
  arm_fir_sparse_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);


  






 
  void arm_fir_sparse_q31(
  arm_fir_sparse_instance_q31 * S,
  q31_t * pSrc,
  q31_t * pDst,
  q31_t * pScratchIn,
  uint32_t blockSize);


  








 
  void arm_fir_sparse_init_q31(
  arm_fir_sparse_instance_q31 * S,
  uint16_t numTaps,
  q31_t * pCoeffs,
  q31_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);


  







 
  void arm_fir_sparse_q15(
  arm_fir_sparse_instance_q15 * S,
  q15_t * pSrc,
  q15_t * pDst,
  q15_t * pScratchIn,
  q31_t * pScratchOut,
  uint32_t blockSize);


  








 
  void arm_fir_sparse_init_q15(
  arm_fir_sparse_instance_q15 * S,
  uint16_t numTaps,
  q15_t * pCoeffs,
  q15_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);


  







 
  void arm_fir_sparse_q7(
  arm_fir_sparse_instance_q7 * S,
  q7_t * pSrc,
  q7_t * pDst,
  q7_t * pScratchIn,
  q31_t * pScratchOut,
  uint32_t blockSize);


  








 
  void arm_fir_sparse_init_q7(
  arm_fir_sparse_instance_q7 * S,
  uint16_t numTaps,
  q7_t * pCoeffs,
  q7_t * pState,
  int32_t * pTapDelay,
  uint16_t maxDelay,
  uint32_t blockSize);


  




 
  void arm_sin_cos_f32(
  float32_t theta,
  float32_t * pSinVal,
  float32_t * pCosVal);


  




 
  void arm_sin_cos_q31(
  q31_t theta,
  q31_t * pSinVal,
  q31_t * pCosVal);


  




 
  void arm_cmplx_conj_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples);

  




 
  void arm_cmplx_conj_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);


  




 
  void arm_cmplx_conj_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);


  




 
  void arm_cmplx_mag_squared_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples);


  




 
  void arm_cmplx_mag_squared_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);


  




 
  void arm_cmplx_mag_squared_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);


 

 

  






















































 

  


 

  




 
   static inline float32_t arm_pid_f32(
  arm_pid_instance_f32 * S,
  float32_t in)
  {
    float32_t out;

     
    out = (S->A0 * in) +
      (S->A1 * S->state[0]) + (S->A2 * S->state[1]) + (S->state[2]);

     
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

     
    return (out);

  }

  












 
   static inline q31_t arm_pid_q31(
  arm_pid_instance_q31 * S,
  q31_t in)
  {
    q63_t acc;
    q31_t out;

     
    acc = (q63_t) S->A0 * in;

     
    acc += (q63_t) S->A1 * S->state[0];

     
    acc += (q63_t) S->A2 * S->state[1];

     
    out = (q31_t) (acc >> 31U);

     
    out += S->state[2];

     
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

     
    return (out);
  }


  













 
   static inline q15_t arm_pid_q15(
  arm_pid_instance_q15 * S,
  q15_t in)
  {
    q63_t acc;
    q15_t out;

    int32_t __packed *vstate;

     

     
    acc = (q31_t) __iar_builtin_SMUAD((uint32_t)S->A0, (uint32_t)in);

     
    vstate = ((int32_t __packed *)(S->state));
    acc = (q63_t)__iar_builtin_SMLALD((uint32_t)S->A1, (uint32_t)*vstate, (uint64_t)acc);

     
    acc += (q31_t) S->state[2] << 15;

     
    out = (q15_t) (__iar_builtin_SSAT((acc >> 15), 16));

     
    S->state[1] = S->state[0];
    S->state[0] = in;
    S->state[2] = out;

     
    return (out);
  }

  

 


  





 
  arm_status arm_mat_inverse_f32(
  const arm_matrix_instance_f32 * src,
  arm_matrix_instance_f32 * dst);


  





 
  arm_status arm_mat_inverse_f64(
  const arm_matrix_instance_f64 * src,
  arm_matrix_instance_f64 * dst);



  

 

  



















 

  


 

  






 
   static inline void arm_clarke_f32(
  float32_t Ia,
  float32_t Ib,
  float32_t * pIalpha,
  float32_t * pIbeta)
  {
     
    *pIalpha = Ia;

     
    *pIbeta = ((float32_t) 0.57735026919 * Ia + (float32_t) 1.15470053838 * Ib);
  }


  











 
   static inline void arm_clarke_q31(
  q31_t Ia,
  q31_t Ib,
  q31_t * pIalpha,
  q31_t * pIbeta)
  {
    q31_t product1, product2;                     

     
    *pIalpha = Ia;

     
    product1 = (q31_t) (((q63_t) Ia * 0x24F34E8B) >> 30);

     
    product2 = (q31_t) (((q63_t) Ib * 0x49E69D16) >> 30);

     
    *pIbeta = __iar_builtin_QADD(product1, product2);
  }

  

 

  




 
  void arm_q7_to_q31(
  q7_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);



  

 

  













 

  


 

   





 
   static inline void arm_inv_clarke_f32(
  float32_t Ialpha,
  float32_t Ibeta,
  float32_t * pIa,
  float32_t * pIb)
  {
     
    *pIa = Ialpha;

     
    *pIb = -0.5f * Ialpha + 0.8660254039f * Ibeta;
  }


  











 
   static inline void arm_inv_clarke_q31(
  q31_t Ialpha,
  q31_t Ibeta,
  q31_t * pIa,
  q31_t * pIb)
  {
    q31_t product1, product2;                     

     
    *pIa = Ialpha;

     
    product1 = (q31_t) (((q63_t) (Ialpha) * (0x40000000)) >> 31);

     
    product2 = (q31_t) (((q63_t) (Ibeta) * (0x6ED9EBA1)) >> 31);

     
    *pIb = __iar_builtin_QSUB(product2, product1);
  }

  

 

  




 
  void arm_q7_to_q15(
  q7_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);



  

 

  





















 

  


 

  










 
   static inline void arm_park_f32(
  float32_t Ialpha,
  float32_t Ibeta,
  float32_t * pId,
  float32_t * pIq,
  float32_t sinVal,
  float32_t cosVal)
  {
     
    *pId = Ialpha * cosVal + Ibeta * sinVal;

     
    *pIq = -Ialpha * sinVal + Ibeta * cosVal;
  }


  













 
   static inline void arm_park_q31(
  q31_t Ialpha,
  q31_t Ibeta,
  q31_t * pId,
  q31_t * pIq,
  q31_t sinVal,
  q31_t cosVal)
  {
    q31_t product1, product2;                     
    q31_t product3, product4;                     

     
    product1 = (q31_t) (((q63_t) (Ialpha) * (cosVal)) >> 31);

     
    product2 = (q31_t) (((q63_t) (Ibeta) * (sinVal)) >> 31);


     
    product3 = (q31_t) (((q63_t) (Ialpha) * (sinVal)) >> 31);

     
    product4 = (q31_t) (((q63_t) (Ibeta) * (cosVal)) >> 31);

     
    *pId = __iar_builtin_QADD(product1, product2);

     
    *pIq = __iar_builtin_QSUB(product4, product3);
  }

  

 

  




 
  void arm_q7_to_float(
  q7_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  

 

  














 

  


 

   







 
   static inline void arm_inv_park_f32(
  float32_t Id,
  float32_t Iq,
  float32_t * pIalpha,
  float32_t * pIbeta,
  float32_t sinVal,
  float32_t cosVal)
  {
     
    *pIalpha = Id * cosVal - Iq * sinVal;

     
    *pIbeta = Id * sinVal + Iq * cosVal;
  }


  













 
   static inline void arm_inv_park_q31(
  q31_t Id,
  q31_t Iq,
  q31_t * pIalpha,
  q31_t * pIbeta,
  q31_t sinVal,
  q31_t cosVal)
  {
    q31_t product1, product2;                     
    q31_t product3, product4;                     

     
    product1 = (q31_t) (((q63_t) (Id) * (cosVal)) >> 31);

     
    product2 = (q31_t) (((q63_t) (Iq) * (sinVal)) >> 31);


     
    product3 = (q31_t) (((q63_t) (Id) * (sinVal)) >> 31);

     
    product4 = (q31_t) (((q63_t) (Iq) * (cosVal)) >> 31);

     
    *pIalpha = __iar_builtin_QSUB(product1, product2);

     
    *pIbeta = __iar_builtin_QADD(product4, product3);
  }

  

 


  




 
  void arm_q31_to_float(
  q31_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);

  

 

  





























 

  


 

  





 
   static inline float32_t arm_linear_interp_f32(
  arm_linear_interp_instance_f32 * S,
  float32_t x)
  {
    float32_t y;
    float32_t x0, x1;                             
    float32_t y0, y1;                             
    float32_t xSpacing = S->xSpacing;             
    int32_t i;                                    
    float32_t *pYData = S->pYData;                

     
    i = (int32_t) ((x - S->x1) / xSpacing);

    if (i < 0)
    {
       
      y = pYData[0];
    }
    else if ((uint32_t)i >= S->nValues)
    {
       
      y = pYData[S->nValues - 1];
    }
    else
    {
       
      x0 = S->x1 +  i      * xSpacing;
      x1 = S->x1 + (i + 1) * xSpacing;

       
      y0 = pYData[i];
      y1 = pYData[i + 1];

       
      y = y0 + (x - x0) * ((y1 - y0) / (x1 - x0));

    }

     
    return (y);
  }


   











 
   static inline q31_t arm_linear_interp_q31(
  q31_t * pYData,
  q31_t x,
  uint32_t nValues)
  {
    q31_t y;                                      
    q31_t y0, y1;                                 
    q31_t fract;                                  
    int32_t index;                                

     
     
     
    index = ((x & (q31_t)0xFFF00000) >> 20);

    if (index >= (int32_t)(nValues - 1))
    {
      return (pYData[nValues - 1]);
    }
    else if (index < 0)
    {
      return (pYData[0]);
    }
    else
    {
       
       
      fract = (x & 0x000FFFFF) << 11;

       
      y0 = pYData[index];
      y1 = pYData[index + 1];

       
      y = ((q31_t) ((q63_t) y0 * (0x7FFFFFFF - fract) >> 32));

       
      y += ((q31_t) (((q63_t) y1 * fract) >> 32));

       
      return (y << 1U);
    }
  }


  











 
   static inline q15_t arm_linear_interp_q15(
  q15_t * pYData,
  q31_t x,
  uint32_t nValues)
  {
    q63_t y;                                      
    q15_t y0, y1;                                 
    q31_t fract;                                  
    int32_t index;                                

     
     
     
    index = ((x & (int32_t)0xFFF00000) >> 20);

    if (index >= (int32_t)(nValues - 1))
    {
      return (pYData[nValues - 1]);
    }
    else if (index < 0)
    {
      return (pYData[0]);
    }
    else
    {
       
       
      fract = (x & 0x000FFFFF);

       
      y0 = pYData[index];
      y1 = pYData[index + 1];

       
      y = ((q63_t) y0 * (0xFFFFF - fract));

       
      y += ((q63_t) y1 * (fract));

       
      return (q15_t) (y >> 20);
    }
  }


  










 
   static inline q7_t arm_linear_interp_q7(
  q7_t * pYData,
  q31_t x,
  uint32_t nValues)
  {
    q31_t y;                                      
    q7_t y0, y1;                                  
    q31_t fract;                                  
    uint32_t index;                               

     
     
     
    if (x < 0)
    {
      return (pYData[0]);
    }
    index = (x >> 20) & 0xfff;

    if (index >= (nValues - 1))
    {
      return (pYData[nValues - 1]);
    }
    else
    {
       
       
      fract = (x & 0x000FFFFF);

       
      y0 = pYData[index];
      y1 = pYData[index + 1];

       
      y = ((y0 * (0xFFFFF - fract)));

       
      y += (y1 * fract);

       
      return (q7_t) (y >> 20);
     }
  }

  

 

  



 
  float32_t arm_sin_f32(
  float32_t x);


  



 
  q31_t arm_sin_q31(
  q31_t x);


  



 
  q15_t arm_sin_q15(
  q15_t x);


  



 
  float32_t arm_cos_f32(
  float32_t x);


  



 
  q31_t arm_cos_q31(
  q31_t x);


  



 
  q15_t arm_cos_q15(
  q15_t x);


  

 


  

















 


  


 

  





 
   static inline arm_status arm_sqrt_f32(
  float32_t in,
  float32_t * pOut)
  {
    if (in >= 0.0f)
    {

      __asm("VSQRT.F32 %0,%1" : "=t"(*pOut) : "t"(in));

      return (ARM_MATH_SUCCESS);
    }
    else
    {
      *pOut = 0.0f;
      return (ARM_MATH_ARGUMENT_ERROR);
    }
  }


  





 
  arm_status arm_sqrt_q31(
  q31_t in,
  q31_t * pOut);


  





 
  arm_status arm_sqrt_q15(
  q15_t in,
  q15_t * pOut);

  

 


  

 
   static inline void arm_circularWrite_f32(
  int32_t * circBuffer,
  int32_t L,
  uint16_t * writeOffset,
  int32_t bufferInc,
  const int32_t * src,
  int32_t srcInc,
  uint32_t blockSize)
  {
    uint32_t i = 0U;
    int32_t wOffset;

    
 
    wOffset = *writeOffset;

     
    i = blockSize;

    while (i > 0U)
    {
       
      circBuffer[wOffset] = *src;

       
      src += srcInc;

       
      wOffset += bufferInc;
      if (wOffset >= L)
        wOffset -= L;

       
      i--;
    }

     
    *writeOffset = (uint16_t)wOffset;
  }



  

 
   static inline void arm_circularRead_f32(
  int32_t * circBuffer,
  int32_t L,
  int32_t * readOffset,
  int32_t bufferInc,
  int32_t * dst,
  int32_t * dst_base,
  int32_t dst_length,
  int32_t dstInc,
  uint32_t blockSize)
  {
    uint32_t i = 0U;
    int32_t rOffset, dst_end;

    
 
    rOffset = *readOffset;
    dst_end = (int32_t) (dst_base + dst_length);

     
    i = blockSize;

    while (i > 0U)
    {
       
      *dst = circBuffer[rOffset];

       
      dst += dstInc;

      if (dst == (int32_t *) dst_end)
      {
        dst = dst_base;
      }

       
      rOffset += bufferInc;

      if (rOffset >= L)
      {
        rOffset -= L;
      }

       
      i--;
    }

     
    *readOffset = rOffset;
  }


  

 
   static inline void arm_circularWrite_q15(
  q15_t * circBuffer,
  int32_t L,
  uint16_t * writeOffset,
  int32_t bufferInc,
  const q15_t * src,
  int32_t srcInc,
  uint32_t blockSize)
  {
    uint32_t i = 0U;
    int32_t wOffset;

    
 
    wOffset = *writeOffset;

     
    i = blockSize;

    while (i > 0U)
    {
       
      circBuffer[wOffset] = *src;

       
      src += srcInc;

       
      wOffset += bufferInc;
      if (wOffset >= L)
        wOffset -= L;

       
      i--;
    }

     
    *writeOffset = (uint16_t)wOffset;
  }


  

 
   static inline void arm_circularRead_q15(
  q15_t * circBuffer,
  int32_t L,
  int32_t * readOffset,
  int32_t bufferInc,
  q15_t * dst,
  q15_t * dst_base,
  int32_t dst_length,
  int32_t dstInc,
  uint32_t blockSize)
  {
    uint32_t i = 0;
    int32_t rOffset, dst_end;

    
 
    rOffset = *readOffset;

    dst_end = (int32_t) (dst_base + dst_length);

     
    i = blockSize;

    while (i > 0U)
    {
       
      *dst = circBuffer[rOffset];

       
      dst += dstInc;

      if (dst == (q15_t *) dst_end)
      {
        dst = dst_base;
      }

       
      rOffset += bufferInc;

      if (rOffset >= L)
      {
        rOffset -= L;
      }

       
      i--;
    }

     
    *readOffset = rOffset;
  }


  

 
   static inline void arm_circularWrite_q7(
  q7_t * circBuffer,
  int32_t L,
  uint16_t * writeOffset,
  int32_t bufferInc,
  const q7_t * src,
  int32_t srcInc,
  uint32_t blockSize)
  {
    uint32_t i = 0U;
    int32_t wOffset;

    
 
    wOffset = *writeOffset;

     
    i = blockSize;

    while (i > 0U)
    {
       
      circBuffer[wOffset] = *src;

       
      src += srcInc;

       
      wOffset += bufferInc;
      if (wOffset >= L)
        wOffset -= L;

       
      i--;
    }

     
    *writeOffset = (uint16_t)wOffset;
  }


  

 
   static inline void arm_circularRead_q7(
  q7_t * circBuffer,
  int32_t L,
  int32_t * readOffset,
  int32_t bufferInc,
  q7_t * dst,
  q7_t * dst_base,
  int32_t dst_length,
  int32_t dstInc,
  uint32_t blockSize)
  {
    uint32_t i = 0;
    int32_t rOffset, dst_end;

    
 
    rOffset = *readOffset;

    dst_end = (int32_t) (dst_base + dst_length);

     
    i = blockSize;

    while (i > 0U)
    {
       
      *dst = circBuffer[rOffset];

       
      dst += dstInc;

      if (dst == (q7_t *) dst_end)
      {
        dst = dst_base;
      }

       
      rOffset += bufferInc;

      if (rOffset >= L)
      {
        rOffset -= L;
      }

       
      i--;
    }

     
    *readOffset = rOffset;
  }


  




 
  void arm_power_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult);


  




 
  void arm_power_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);


  




 
  void arm_power_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q63_t * pResult);


  




 
  void arm_power_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);


  




 
  void arm_mean_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q7_t * pResult);


  




 
  void arm_mean_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);


  




 
  void arm_mean_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);


  




 
  void arm_mean_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);


  




 
  void arm_var_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);


  




 
  void arm_var_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);


  




 
  void arm_var_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);


  




 
  void arm_rms_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);


  




 
  void arm_rms_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);


  




 
  void arm_rms_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);


  




 
  void arm_std_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult);


  




 
  void arm_std_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult);


  




 
  void arm_std_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult);


  




 
  void arm_cmplx_mag_f32(
  float32_t * pSrc,
  float32_t * pDst,
  uint32_t numSamples);


  




 
  void arm_cmplx_mag_q31(
  q31_t * pSrc,
  q31_t * pDst,
  uint32_t numSamples);


  




 
  void arm_cmplx_mag_q15(
  q15_t * pSrc,
  q15_t * pDst,
  uint32_t numSamples);


  






 
  void arm_cmplx_dot_prod_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  uint32_t numSamples,
  q31_t * realResult,
  q31_t * imagResult);


  






 
  void arm_cmplx_dot_prod_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  uint32_t numSamples,
  q63_t * realResult,
  q63_t * imagResult);


  






 
  void arm_cmplx_dot_prod_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  uint32_t numSamples,
  float32_t * realResult,
  float32_t * imagResult);


  





 
  void arm_cmplx_mult_real_q15(
  q15_t * pSrcCmplx,
  q15_t * pSrcReal,
  q15_t * pCmplxDst,
  uint32_t numSamples);


  





 
  void arm_cmplx_mult_real_q31(
  q31_t * pSrcCmplx,
  q31_t * pSrcReal,
  q31_t * pCmplxDst,
  uint32_t numSamples);


  





 
  void arm_cmplx_mult_real_f32(
  float32_t * pSrcCmplx,
  float32_t * pSrcReal,
  float32_t * pCmplxDst,
  uint32_t numSamples);


  





 
  void arm_min_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q7_t * result,
  uint32_t * index);


  





 
  void arm_min_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult,
  uint32_t * pIndex);


  





 
  void arm_min_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult,
  uint32_t * pIndex);


  





 
  void arm_min_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult,
  uint32_t * pIndex);








 
  void arm_max_q7(
  q7_t * pSrc,
  uint32_t blockSize,
  q7_t * pResult,
  uint32_t * pIndex);








 
  void arm_max_q15(
  q15_t * pSrc,
  uint32_t blockSize,
  q15_t * pResult,
  uint32_t * pIndex);








 
  void arm_max_q31(
  q31_t * pSrc,
  uint32_t blockSize,
  q31_t * pResult,
  uint32_t * pIndex);








 
  void arm_max_f32(
  float32_t * pSrc,
  uint32_t blockSize,
  float32_t * pResult,
  uint32_t * pIndex);


  





 
  void arm_cmplx_mult_cmplx_q15(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  uint32_t numSamples);


  





 
  void arm_cmplx_mult_cmplx_q31(
  q31_t * pSrcA,
  q31_t * pSrcB,
  q31_t * pDst,
  uint32_t numSamples);


  





 
  void arm_cmplx_mult_cmplx_f32(
  float32_t * pSrcA,
  float32_t * pSrcB,
  float32_t * pDst,
  uint32_t numSamples);


  




 
  void arm_float_to_q31(
  float32_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  




 
  void arm_float_to_q15(
  float32_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  




 
  void arm_float_to_q7(
  float32_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  




 
  void arm_q31_to_q15(
  q31_t * pSrc,
  q15_t * pDst,
  uint32_t blockSize);


  




 
  void arm_q31_to_q7(
  q31_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  




 
  void arm_q15_to_float(
  q15_t * pSrc,
  float32_t * pDst,
  uint32_t blockSize);


  




 
  void arm_q15_to_q31(
  q15_t * pSrc,
  q31_t * pDst,
  uint32_t blockSize);


  




 
  void arm_q15_to_q7(
  q15_t * pSrc,
  q7_t * pDst,
  uint32_t blockSize);


  

 

  

















































 

  


 


  






 
   static inline float32_t arm_bilinear_interp_f32(
  const arm_bilinear_interp_instance_f32 * S,
  float32_t X,
  float32_t Y)
  {
    float32_t out;
    float32_t f00, f01, f10, f11;
    float32_t *pData = S->pData;
    int32_t xIndex, yIndex, index;
    float32_t xdiff, ydiff;
    float32_t b1, b2, b3, b4;

    xIndex = (int32_t) X;
    yIndex = (int32_t) Y;

     
     
    if (xIndex < 0 || xIndex > (S->numRows - 1) || yIndex < 0 || yIndex > (S->numCols - 1))
    {
      return (0);
    }

     
    index = (xIndex - 1) + (yIndex - 1) * S->numCols;


     
    f00 = pData[index];
    f01 = pData[index + 1];

     
    index = (xIndex - 1) + (yIndex) * S->numCols;


     
    f10 = pData[index];
    f11 = pData[index + 1];

     
    b1 = f00;
    b2 = f01 - f00;
    b3 = f10 - f00;
    b4 = f00 - f01 - f10 + f11;

     
    xdiff = X - xIndex;

     
    ydiff = Y - yIndex;

     
    out = b1 + b2 * xdiff + b3 * ydiff + b4 * xdiff * ydiff;

     
    return (out);
  }


  






 
   static inline q31_t arm_bilinear_interp_q31(
  arm_bilinear_interp_instance_q31 * S,
  q31_t X,
  q31_t Y)
  {
    q31_t out;                                    
    q31_t acc = 0;                                
    q31_t xfract, yfract;                         
    q31_t x1, x2, y1, y2;                         
    int32_t rI, cI;                               
    q31_t *pYData = S->pData;                     
    uint32_t nCols = S->numCols;                  

     
     
     
    rI = ((X & (q31_t)0xFFF00000) >> 20);

     
     
     
    cI = ((Y & (q31_t)0xFFF00000) >> 20);

     
     
    if (rI < 0 || rI > (S->numRows - 1) || cI < 0 || cI > (S->numCols - 1))
    {
      return (0);
    }

     
     
    xfract = (X & 0x000FFFFF) << 11U;

     
    x1 = pYData[(rI) + (int32_t)nCols * (cI)    ];
    x2 = pYData[(rI) + (int32_t)nCols * (cI) + 1];

     
     
    yfract = (Y & 0x000FFFFF) << 11U;

     
    y1 = pYData[(rI) + (int32_t)nCols * (cI + 1)    ];
    y2 = pYData[(rI) + (int32_t)nCols * (cI + 1) + 1];

     
    out = ((q31_t) (((q63_t) x1  * (0x7FFFFFFF - xfract)) >> 32));
    acc = ((q31_t) (((q63_t) out * (0x7FFFFFFF - yfract)) >> 32));

     
    out = ((q31_t) ((q63_t) x2 * (0x7FFFFFFF - yfract) >> 32));
    acc += ((q31_t) ((q63_t) out * (xfract) >> 32));

     
    out = ((q31_t) ((q63_t) y1 * (0x7FFFFFFF - xfract) >> 32));
    acc += ((q31_t) ((q63_t) out * (yfract) >> 32));

     
    out = ((q31_t) ((q63_t) y2 * (xfract) >> 32));
    acc += ((q31_t) ((q63_t) out * (yfract) >> 32));

     
    return ((q31_t)(acc << 2));
  }


  





 
   static inline q15_t arm_bilinear_interp_q15(
  arm_bilinear_interp_instance_q15 * S,
  q31_t X,
  q31_t Y)
  {
    q63_t acc = 0;                                
    q31_t out;                                    
    q15_t x1, x2, y1, y2;                         
    q31_t xfract, yfract;                         
    int32_t rI, cI;                               
    q15_t *pYData = S->pData;                     
    uint32_t nCols = S->numCols;                  

     
     
     
    rI = ((X & (q31_t)0xFFF00000) >> 20);

     
     
     
    cI = ((Y & (q31_t)0xFFF00000) >> 20);

     
     
    if (rI < 0 || rI > (S->numRows - 1) || cI < 0 || cI > (S->numCols - 1))
    {
      return (0);
    }

     
     
    xfract = (X & 0x000FFFFF);

     
    x1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI)    ];
    x2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI) + 1];

     
     
    yfract = (Y & 0x000FFFFF);

     
    y1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1)    ];
    y2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1) + 1];

     

     
     
    out = (q31_t) (((q63_t) x1 * (0xFFFFF - xfract)) >> 4U);
    acc = ((q63_t) out * (0xFFFFF - yfract));

     
    out = (q31_t) (((q63_t) x2 * (0xFFFFF - yfract)) >> 4U);
    acc += ((q63_t) out * (xfract));

     
    out = (q31_t) (((q63_t) y1 * (0xFFFFF - xfract)) >> 4U);
    acc += ((q63_t) out * (yfract));

     
    out = (q31_t) (((q63_t) y2 * (xfract)) >> 4U);
    acc += ((q63_t) out * (yfract));

     
     
    return ((q15_t)(acc >> 36));
  }


  





 
   static inline q7_t arm_bilinear_interp_q7(
  arm_bilinear_interp_instance_q7 * S,
  q31_t X,
  q31_t Y)
  {
    q63_t acc = 0;                                
    q31_t out;                                    
    q31_t xfract, yfract;                         
    q7_t x1, x2, y1, y2;                          
    int32_t rI, cI;                               
    q7_t *pYData = S->pData;                      
    uint32_t nCols = S->numCols;                  

     
     
     
    rI = ((X & (q31_t)0xFFF00000) >> 20);

     
     
     
    cI = ((Y & (q31_t)0xFFF00000) >> 20);

     
     
    if (rI < 0 || rI > (S->numRows - 1) || cI < 0 || cI > (S->numCols - 1))
    {
      return (0);
    }

     
     
    xfract = (X & (q31_t)0x000FFFFF);

     
    x1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI)    ];
    x2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI) + 1];

     
     
    yfract = (Y & (q31_t)0x000FFFFF);

     
    y1 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1)    ];
    y2 = pYData[((uint32_t)rI) + nCols * ((uint32_t)cI + 1) + 1];

     
    out = ((x1 * (0xFFFFF - xfract)));
    acc = (((q63_t) out * (0xFFFFF - yfract)));

     
    out = ((x2 * (0xFFFFF - yfract)));
    acc += (((q63_t) out * (xfract)));

     
    out = ((y1 * (0xFFFFF - xfract)));
    acc += (((q63_t) out * (yfract)));

     
    out = ((y2 * (yfract)));
    acc += (((q63_t) out * (xfract)));

     
    return ((q7_t)(acc >> 40));
  }

  

 


 

 

 

 

 

 


   

   

   

   




 






 









 
















 



extern const uint16_t armBitRevTable[1024];
extern const q15_t armRecipTableQ15[64];
extern const q31_t armRecipTableQ31[64];
extern const float32_t twiddleCoef_16[32];
extern const float32_t twiddleCoef_32[64];
extern const float32_t twiddleCoef_64[128];
extern const float32_t twiddleCoef_128[256];
extern const float32_t twiddleCoef_256[512];
extern const float32_t twiddleCoef_512[1024];
extern const float32_t twiddleCoef_1024[2048];
extern const float32_t twiddleCoef_2048[4096];
extern const float32_t twiddleCoef_4096[8192];
extern const q31_t twiddleCoef_16_q31[24];
extern const q31_t twiddleCoef_32_q31[48];
extern const q31_t twiddleCoef_64_q31[96];
extern const q31_t twiddleCoef_128_q31[192];
extern const q31_t twiddleCoef_256_q31[384];
extern const q31_t twiddleCoef_512_q31[768];
extern const q31_t twiddleCoef_1024_q31[1536];
extern const q31_t twiddleCoef_2048_q31[3072];
extern const q31_t twiddleCoef_4096_q31[6144];
extern const q15_t twiddleCoef_16_q15[24];
extern const q15_t twiddleCoef_32_q15[48];
extern const q15_t twiddleCoef_64_q15[96];
extern const q15_t twiddleCoef_128_q15[192];
extern const q15_t twiddleCoef_256_q15[384];
extern const q15_t twiddleCoef_512_q15[768];
extern const q15_t twiddleCoef_1024_q15[1536];
extern const q15_t twiddleCoef_2048_q15[3072];
extern const q15_t twiddleCoef_4096_q15[6144];
extern const float32_t twiddleCoef_rfft_32[32];
extern const float32_t twiddleCoef_rfft_64[64];
extern const float32_t twiddleCoef_rfft_128[128];
extern const float32_t twiddleCoef_rfft_256[256];
extern const float32_t twiddleCoef_rfft_512[512];
extern const float32_t twiddleCoef_rfft_1024[1024];
extern const float32_t twiddleCoef_rfft_2048[2048];
extern const float32_t twiddleCoef_rfft_4096[4096];

 

extern const uint16_t armBitRevIndexTable16[((uint16_t)20)];
extern const uint16_t armBitRevIndexTable32[((uint16_t)48)];
extern const uint16_t armBitRevIndexTable64[((uint16_t)56)];
extern const uint16_t armBitRevIndexTable128[((uint16_t)208)];
extern const uint16_t armBitRevIndexTable256[((uint16_t)440)];
extern const uint16_t armBitRevIndexTable512[((uint16_t)448)];
extern const uint16_t armBitRevIndexTable1024[((uint16_t)1800)];
extern const uint16_t armBitRevIndexTable2048[((uint16_t)3808)];
extern const uint16_t armBitRevIndexTable4096[((uint16_t)4032)];

 

extern const uint16_t armBitRevIndexTable_fixed_16[((uint16_t)12)];
extern const uint16_t armBitRevIndexTable_fixed_32[((uint16_t)24)];
extern const uint16_t armBitRevIndexTable_fixed_64[((uint16_t)56)];
extern const uint16_t armBitRevIndexTable_fixed_128[((uint16_t)112)];
extern const uint16_t armBitRevIndexTable_fixed_256[((uint16_t)240)];
extern const uint16_t armBitRevIndexTable_fixed_512[((uint16_t)480)];
extern const uint16_t armBitRevIndexTable_fixed_1024[((uint16_t)992)];
extern const uint16_t armBitRevIndexTable_fixed_2048[((uint16_t)1984)];
extern const uint16_t armBitRevIndexTable_fixed_4096[((uint16_t)4032)];

 
extern const float32_t sinTable_f32[512 + 1];
extern const q31_t sinTable_q31[512 + 1];
extern const q15_t sinTable_q15[512 + 1];




 























 




 





 

float32_t arm_sin_f32(
  float32_t x)
{
  float32_t sinVal, fract, in;                            
  uint16_t index;                                         
  float32_t a, b;                                         
  int32_t n;
  float32_t findex;

   
  if ((x < 0.0f) && (x >= -1.9e-7f)) {
     return x;
  }

   
   
  in = x * 0.159154943092f;

   
  n = (int32_t) in;

   
  if (x < 0.0f)
  {
    n--;
  }

   
  in = in - (float32_t) n;

   
  findex = (float32_t) 512 * in;

  index = ((uint16_t)findex) & 0x1ff;

   
  fract = findex - (float32_t) index;

   
  a = sinTable_f32[index];
  b = sinTable_f32[index+1];

   
  sinVal = (1.0f-fract)*a + fract*b;

   
  return (sinVal);
}



 
