






























 



 



 
	































 



 



 

	



































 



 



 




 


























 
















 




















 




 
 

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

  






 
 
 


  #pragma system_include

 
 

 

  #pragma system_include














 



 
  typedef _Sizet size_t;

typedef unsigned int __data_size_t;



 
#pragma rtmodel="__dlib_file_descriptor","1"

 

  typedef _Filet FILE;


#pragma language = save
#pragma language = extended

      
         extern FILE __iar_Stdin;
         extern FILE __iar_Stdout;
         extern FILE __iar_Stderr;
      

#pragma language=restore



 
typedef _Fpost fpos_t;


 


   
  
    __intrinsic __nounwind    void   clearerr(FILE *);
    __intrinsic __nounwind    int    fclose(FILE *);
    __intrinsic __nounwind    int    feof(FILE *);
    __intrinsic __nounwind    int    ferror(FILE *);
    __intrinsic __nounwind    int    fflush(FILE *);
    __intrinsic __nounwind    int    fgetc(FILE *);
    __intrinsic __nounwind    int    fgetpos(FILE *restrict, fpos_t *restrict);
    __intrinsic __nounwind    char * fgets(char *restrict, int, FILE *restrict);
     __intrinsic __nounwind  FILE * fopen(const char *restrict, const char *restrict);
     _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int    fprintf(FILE *restrict,
                                   const char *restrict, ...);
    __intrinsic __nounwind    int    fputc(int, FILE *);
    __intrinsic __nounwind    int    fputs(const char *restrict, FILE *restrict);
    __intrinsic __nounwind    size_t fread(void *restrict, size_t, size_t,
                                 FILE *restrict);
     __intrinsic __nounwind  FILE * freopen(const char *restrict,
                                   const char *restrict, FILE *restrict);
     _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind  int    fscanf(FILE *restrict, const char *restrict, ...);
    __intrinsic __nounwind    int    fseek(FILE *, long, int);
    __intrinsic __nounwind    int    fsetpos(FILE *, const fpos_t *);
    __intrinsic __nounwind    long   ftell(FILE *);
    __intrinsic __nounwind    size_t fwrite(const void *restrict, size_t, size_t,
                                  FILE *restrict);
    __intrinsic __nounwind    void   rewind(FILE *);
    __intrinsic __nounwind    void   setbuf(FILE *restrict, char *restrict);
    __intrinsic __nounwind    int    setvbuf(FILE *restrict, char *restrict,
                                   int, size_t);
     __intrinsic __nounwind  char * tmpnam(char *);
     __intrinsic __nounwind  FILE * tmpfile(void);
    __intrinsic __nounwind    int    ungetc(int, FILE *);
     _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int    vfprintf(FILE *restrict, const char *restrict,
                                    __Va_list);
       _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind int   vfscanf(FILE *restrict, const char *restrict,
                                   __Va_list);
      __intrinsic __nounwind FILE *   fdopen(signed char, const char *);
      __intrinsic __nounwind signed char fileno(FILE *);
      __intrinsic __nounwind int      getw(FILE *);
      __intrinsic __nounwind int      putw(int, FILE *);
    __intrinsic __nounwind int        getc(FILE *);
    __intrinsic __nounwind int        putc(int, FILE *);
  


     
    _Pragma("function_effects = no_read(1), always_returns") __intrinsic __nounwind   char * __gets(char *, int);
    _Pragma("function_effects = no_read(1), always_returns")  __intrinsic __nounwind char * gets(char *);
  _Pragma("function_effects = no_write(1), always_returns")    __intrinsic __nounwind    void   perror(const char *);
  _Pragma("function_effects = no_write(1), always_returns")     _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int    printf(const char *restrict, ...);
  _Pragma("function_effects = no_write(1), always_returns")    __intrinsic __nounwind    int    puts(const char *);
  _Pragma("function_effects = no_write(1), always_returns")     _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind  int    scanf(const char *restrict, ...);
  _Pragma("function_effects = no_read(1), no_write(2), always_returns")  _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int    sprintf(char *restrict,
                                              const char *restrict, ...);
  _Pragma("function_effects = no_write(1,2), always_returns")  _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind  int    sscanf(const char *restrict,
                                             const char *restrict, ...);
                                       
  __intrinsic __nounwind                 int    __ungetchar(int);
  _Pragma("function_effects = no_write(1), always_returns")     _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int    vprintf(const char *restrict,
                                              __Va_list);
    _Pragma("function_effects = no_write(1), always_returns")     _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind int vscanf(const char *restrict, __Va_list);
    _Pragma("function_effects = no_write(1,2), always_returns")  _Pragma("__scanf_args") _Pragma("library_default_requirements _Scanf = unknown") __intrinsic __nounwind int vsscanf(const char *restrict,
                                            const char *restrict, __Va_list);
  _Pragma("function_effects = no_read(1), no_write(2), always_returns")   _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int vsprintf(char *restrict,
                                             const char *restrict, __Va_list);
                                 
  _Pragma("function_effects = no_write(1), always_returns") __intrinsic __nounwind size_t   __write_array(const void *, size_t, size_t);
    _Pragma("function_effects = no_read(1), no_write(3), always_returns")  _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int snprintf(char *restrict, size_t,
                                              const char *restrict, ...);
    _Pragma("function_effects = no_read(1), no_write(3), always_returns")  _Pragma("__printf_args") _Pragma("library_default_requirements _Printf = unknown") __intrinsic __nounwind int vsnprintf(char *restrict, size_t,
                                               const char *restrict,
                                               __Va_list);

  __intrinsic __nounwind int                getchar(void);
  __intrinsic __nounwind int                putchar(int);
  
 
  __intrinsic __nounwind int                remove(const char *);
  __intrinsic __nounwind int                rename(const char *, const char *);







 
 
 


  #pragma system_include

 
 

 

  #pragma system_include














 



 
#pragma rtmodel="__dlib_full_locale_support",   "1"

  
    __intrinsic __nounwind size_t __iar_Mbcurmax(void);
  


 



 
  typedef _Wchart wchar_t;

typedef struct
{  
  int quot;
  int rem;
} div_t;

typedef struct
{  
  long quot;
  long rem;
} ldiv_t;

  typedef struct
  {  
    long long quot;
    long long rem;
  } lldiv_t;


 
  
  __intrinsic __nounwind int             atexit(void (*)(void));
    __intrinsic __nounwind          int  at_quick_exit(void (*)(void)) ;
    __intrinsic __noreturn __nounwind void _Exit(int) ;
    __intrinsic __noreturn __nounwind void quick_exit(int) ;
  __intrinsic __noreturn __nounwind void   exit(int);
   __intrinsic __nounwind        char * getenv(const char *);
  __intrinsic __nounwind          int    system(const char *);



               __intrinsic __nounwind void *    aligned_alloc(size_t, size_t);
          __intrinsic __noreturn __nounwind void  abort(void) ;
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind int       abs(int);
               __intrinsic __nounwind void *    calloc(size_t, size_t);
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind div_t     div(int, int);
               __intrinsic __nounwind void      free(void *);
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind long      labs(long);
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind ldiv_t    ldiv(long, long);
    _Pragma("function_effects = no_state, always_returns")   __intrinsic __nounwind long long llabs(long long);
    _Pragma("function_effects = no_state, always_returns")   __intrinsic __nounwind lldiv_t   lldiv(long long, long long);
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind double    fabs(double);
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind float     fabsf(float);
  _Pragma("function_effects = no_state, always_returns")     __intrinsic __nounwind long double fabsl(long double);
               __intrinsic __nounwind void *    malloc(size_t);
  _Pragma("function_effects = no_write(1), always_returns")    __intrinsic __nounwind int       mblen(const char *, size_t);
    _Pragma("function_effects = no_read(1), no_write(2), always_returns")  __intrinsic __nounwind size_t mbstowcs(wchar_t *restrict,
                                                const char *restrict, size_t);
    _Pragma("function_effects = no_read(1), no_write(2), always_returns") __intrinsic __nounwind   int    mbtowc(wchar_t *restrict,
                                              const char *restrict, size_t);
               __intrinsic __nounwind int    rand(void);
               __intrinsic __nounwind void   srand(unsigned int);
               __intrinsic __nounwind void * realloc(void *, size_t);
               __intrinsic __nounwind void * __iar_realloc_in_place(void *, size_t);
  _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind long          strtol(const char *restrict,
                                                 char **restrict, int);
  _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind unsigned long strtoul(const char *, char **, int);
    _Pragma("function_effects = no_read(1), no_write(2), always_returns")  __intrinsic __nounwind size_t wcstombs(char *restrict,
                                               const wchar_t *restrict,
                                               size_t);
    _Pragma("function_effects = no_read(1), always_returns")     __intrinsic __nounwind int    wctomb(char *, wchar_t);
    _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind long long strtoll(const char *, char **, int);
    _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind unsigned long long strtoull(const char *,
                                                          char **, int);



  typedef int _Cmpfun(const void *, const void *);
  _Pragma("function_effects = no_write(1,2), always_returns")  __intrinsic void * bsearch(const void *,
                                                       const void *, size_t,
                                                       size_t, _Cmpfun *);
                __intrinsic void   qsort(void *, size_t, size_t,
                                                     _Cmpfun *);
               __intrinsic void     __qsortbbl(void *, size_t,
                                                          size_t, _Cmpfun *);
  _Pragma("function_effects = no_write(1), always_returns")    __intrinsic __nounwind double             atof(const char *);
  _Pragma("function_effects = no_write(1), always_returns")    __intrinsic __nounwind int                atoi(const char *);
  _Pragma("function_effects = no_write(1), always_returns")    __intrinsic __nounwind long               atol(const char *);
     _Pragma("function_effects = no_write(1), always_returns") __intrinsic __nounwind long long        atoll(const char *);
     _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind float         strtof(const char *restrict,
                                                    char **restrict);
     _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind long double   strtold(const char *, char **);
  _Pragma("function_effects = no_write(1), no_read(2), always_returns") __intrinsic __nounwind double           strtod(const char *restrict,
                                                    char **restrict);
    _Pragma("function_effects = no_state, always_returns")   __intrinsic __nounwind int              __iar_DLib_library_version(void);


  
  #pragma inline=no_body
  int abs(int i)
  {       
    return i < 0 ? -i : i;
  }

  #pragma inline=no_body
  long labs(long i)
  {  
    return i < 0 ? -i : i;
  }

    #pragma inline=no_body
    long long llabs(long long i)
    {  
      return i < 0 ? -i : i;
    }
  



#pragma language=save
#pragma language=extended

  void __iar_table_init3(void __data const * table_start,
                      void __data const * table_end);

#pragma language=restore







 


  #pragma system_include




#pragma inline=forced_no_body
__intrinsic __nounwind
div_t div(int a, int b)
{
  int q = a / b;
  int r = a - (q*b);
  div_t d;
  d.quot = q;
  d.rem = r;
  return d;
}

#pragma inline=forced_no_body
__intrinsic __nounwind
ldiv_t ldiv(long a, long b)
{
  int q = a / b;
  int r = a - (q*b);
  ldiv_t ld;
  ld.quot = q;
  ld.rem = r;
  return ld;
}

    #pragma type_attribute=__value_in_regs
    lldiv_t __aeabi_ldivmod(long long n, long long d);

    #pragma inline=forced_no_body
    __intrinsic __nounwind
    lldiv_t lldiv(long long a, long long b)
    {
      return __aeabi_ldivmod(a, b);
    }








 
 
 

  #pragma system_include

 
 

 

  #pragma system_include














 




 
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

 






















 











 



void load4BytesToTxBuffer(uint8_t *txBuffer, uint8_t opcode, uint32_t address);
void fillArrayPattern(uint8_t * byteArray, uint32_t numBytes, int seedNumber);
void fillArrayConst(uint8_t * byteArray, uint32_t numBytes, int constantNum);
int compareByteArrays(uint8_t *arr1, uint8_t *arr2, uint32_t arrLength);













 










 
void standardflashWaitOnReady();










 
void standardflashSetQEBit();










 
void standardflashClearQEBit();






 
void standardflashWriteEnable();






 
void standardflashWriteDisable();














 
void standardflashReadArrayLowFreq(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes);














 
void standardflashReadArrayHighFreq(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes);












 
void standardflashBytePageProgram(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes);








 
void standardflashBlockErase4K(uint32_t address);








 
void standardflashBlockErase32K(uint32_t address);








 
void standardflashBlockErase64K(uint32_t address);







 
void standardflashChipErase1();







 
void standardflashChipErase2();







 
void standardflashDPD();






 
void standardflashResumeFromDPD();









 
void standardflashReadID(uint8_t *rxBuffer);








 
void standardflashReadMID(uint8_t *rxBuffer);









 
void standardflashWriteSR(uint8_t *txBuffer, uint8_t txNumBytes);

void standardflashWriteSRB1(uint8_t regVal);
void standardflashWriteSRB2(uint8_t regVal);






 
void standardflashWriteEnableVolatileSR();






 
uint8_t standardflashReadSRB1();






 
uint8_t standardflashReadSRB2();













 
void standardflashDualOutputRead(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes);



























 
void standardflashDualIORead(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes, uint8_t readMode, uint8_t modeByteValue);













 
void standardflashQuadOutputRead(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes);


























 
void standardflashQuadIORead(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes, uint8_t readMode, uint8_t modeByteValue);







 
void standardflashContinuousReadModeDualReset();







 
void standardflashContinuousReadModeQuadReset();






 
void standardflashEraseSecurityRegister(uint32_t address);













 
void standardflashProgramSecurityRegisters(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes);












 
void standardflashReadSecurityRegisters(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes);








 
void standardflashResumeFromDPDReadID(uint8_t *rxBuffer);














 
void standardflashQuadPageProgram(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes, uint8_t mode);






 
void standardflashEraseProgramSuspend();






 
void standardflashEraseProgramResume();









 
void standardflashEnableQPI();








 
void standardflashDisableQPI();








 
void standardflashEnableReset();








 
void standardflashReset();






 
void standardflashEnterSecureOTP();






 
void standardflashExitSecuredOTP();


 

















 
 

 


 

















  

 


 



















  

 


 
 

 


 

   
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

 




 






 



 


 






 


 

 


      

 



 
 

 

 

 

    

 

  
  
 


 

 

  
 


 




 


 


 
















 

 


 

















 

 


 











































 



 



 





 



 



 



 




 


 



 







































 



 



 





 




 
typedef enum
{
 
  NonMaskableInt_IRQn         = -14,     
  MemoryManagement_IRQn       = -12,     
  BusFault_IRQn               = -11,     
  UsageFault_IRQn             = -10,     
  SVCall_IRQn                 = -5,      
  DebugMonitor_IRQn           = -4,      
  PendSV_IRQn                 = -2,      
  SysTick_IRQn                = -1,      
 
  WWDG_IRQn                   = 0,       
  PVD_IRQn                    = 1,       
  TAMP_STAMP_IRQn             = 2,       
  RTC_WKUP_IRQn               = 3,       
  FLASH_IRQn                  = 4,       
  RCC_IRQn                    = 5,       
  EXTI0_IRQn                  = 6,       
  EXTI1_IRQn                  = 7,       
  EXTI2_IRQn                  = 8,       
  EXTI3_IRQn                  = 9,       
  EXTI4_IRQn                  = 10,      
  DMA1_Stream0_IRQn           = 11,      
  DMA1_Stream1_IRQn           = 12,      
  DMA1_Stream2_IRQn           = 13,      
  DMA1_Stream3_IRQn           = 14,      
  DMA1_Stream4_IRQn           = 15,      
  DMA1_Stream5_IRQn           = 16,      
  DMA1_Stream6_IRQn           = 17,      
  ADC_IRQn                    = 18,      
  CAN1_TX_IRQn                = 19,      
  CAN1_RX0_IRQn               = 20,      
  CAN1_RX1_IRQn               = 21,      
  CAN1_SCE_IRQn               = 22,      
  EXTI9_5_IRQn                = 23,      
  TIM1_BRK_TIM9_IRQn          = 24,      
  TIM1_UP_TIM10_IRQn          = 25,      
  TIM1_TRG_COM_TIM11_IRQn     = 26,      
  TIM1_CC_IRQn                = 27,      
  TIM2_IRQn                   = 28,      
  TIM3_IRQn                   = 29,      
  TIM4_IRQn                   = 30,      
  I2C1_EV_IRQn                = 31,      
  I2C1_ER_IRQn                = 32,      
  I2C2_EV_IRQn                = 33,      
  I2C2_ER_IRQn                = 34,      
  SPI1_IRQn                   = 35,      
  SPI2_IRQn                   = 36,      
  USART1_IRQn                 = 37,      
  USART2_IRQn                 = 38,      
  USART3_IRQn                 = 39,      
  EXTI15_10_IRQn              = 40,      
  RTC_Alarm_IRQn              = 41,      
  OTG_FS_WKUP_IRQn            = 42,      
  TIM8_BRK_TIM12_IRQn         = 43,      
  TIM8_UP_TIM13_IRQn          = 44,      
  TIM8_TRG_COM_TIM14_IRQn     = 45,      
  TIM8_CC_IRQn                = 46,      
  DMA1_Stream7_IRQn           = 47,      
  FMC_IRQn                    = 48,      
  SDMMC1_IRQn                 = 49,      
  TIM5_IRQn                   = 50,      
  SPI3_IRQn                   = 51,      
  UART4_IRQn                  = 52,      
  UART5_IRQn                  = 53,      
  TIM6_DAC_IRQn               = 54,      
  TIM7_IRQn                   = 55,      
  DMA2_Stream0_IRQn           = 56,      
  DMA2_Stream1_IRQn           = 57,      
  DMA2_Stream2_IRQn           = 58,      
  DMA2_Stream3_IRQn           = 59,      
  DMA2_Stream4_IRQn           = 60,      
  ETH_IRQn                    = 61,      
  ETH_WKUP_IRQn               = 62,      
  OTG_FS_IRQn                 = 67,      
  DMA2_Stream5_IRQn           = 68,      
  DMA2_Stream6_IRQn           = 69,      
  DMA2_Stream7_IRQn           = 70,      
  USART6_IRQn                 = 71,      
  I2C3_EV_IRQn                = 72,      
  I2C3_ER_IRQn                = 73,      
  OTG_HS_EP1_OUT_IRQn         = 74,      
  OTG_HS_EP1_IN_IRQn          = 75,      
  OTG_HS_WKUP_IRQn            = 76,      
  OTG_HS_IRQn                 = 77,      
  RNG_IRQn                    = 80,      
  FPU_IRQn                    = 81,      
  UART7_IRQn                  = 82,      
  UART8_IRQn                  = 83,      
  SPI4_IRQn                   = 84,      
  SPI5_IRQn                   = 85,      
  SAI1_IRQn                   = 87,      
  SAI2_IRQn                   = 91,      
  QUADSPI_IRQn                = 92,      
  LPTIM1_IRQn                 = 93,      
  SDMMC2_IRQn                 = 103,     
} IRQn_Type;



 



 
 




 
















 

  #pragma system_include          
















 




 



 

 




 
















 

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





 










 

 






 

 

 













 



 






 



 
typedef union
{
  struct
  {
    uint32_t _reserved0:16;               
    uint32_t GE:4;                        
    uint32_t _reserved1:7;                
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} APSR_Type;

 









 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:23;               
  } b;                                    
  uint32_t w;                             
} IPSR_Type;

 




 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:1;                
    uint32_t ICI_IT_1:6;                  
    uint32_t GE:4;                        
    uint32_t _reserved1:4;                
    uint32_t T:1;                         
    uint32_t ICI_IT_2:2;                  
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} xPSR_Type;

 













 
typedef union
{
  struct
  {
    uint32_t nPRIV:1;                     
    uint32_t SPSEL:1;                     
    uint32_t FPCA:1;                      
    uint32_t _reserved0:29;               
  } b;                                    
  uint32_t w;                             
} CONTROL_Type;

 



 







 



 
typedef struct
{
  volatile uint32_t ISER[8U];                
        uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];                
        uint32_t RSERVED1[24U];
  volatile uint32_t ISPR[8U];                
        uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];                
        uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];                
        uint32_t RESERVED4[56U];
  volatile uint8_t  IP[240U];                
        uint32_t RESERVED5[644U];
  volatile  uint32_t STIR;                    
}  NVIC_Type;

 

 







 



 
typedef struct
{
  volatile const  uint32_t CPUID;                   
  volatile uint32_t ICSR;                    
  volatile uint32_t VTOR;                    
  volatile uint32_t AIRCR;                   
  volatile uint32_t SCR;                     
  volatile uint32_t CCR;                     
  volatile uint8_t  SHPR[12U];               
  volatile uint32_t SHCSR;                   
  volatile uint32_t CFSR;                    
  volatile uint32_t HFSR;                    
  volatile uint32_t DFSR;                    
  volatile uint32_t MMFAR;                   
  volatile uint32_t BFAR;                    
  volatile uint32_t AFSR;                    
  volatile const  uint32_t ID_PFR[2U];              
  volatile const  uint32_t ID_DFR;                  
  volatile const  uint32_t ID_AFR;                  
  volatile const  uint32_t ID_MFR[4U];              
  volatile const  uint32_t ID_ISAR[5U];             
        uint32_t RESERVED0[1U];
  volatile const  uint32_t CLIDR;                   
  volatile const  uint32_t CTR;                     
  volatile const  uint32_t CCSIDR;                  
  volatile uint32_t CSSELR;                  
  volatile uint32_t CPACR;                   
        uint32_t RESERVED3[93U];
  volatile  uint32_t STIR;                    
        uint32_t RESERVED4[15U];
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
        uint32_t RESERVED5[1U];
  volatile  uint32_t ICIALLU;                 
        uint32_t RESERVED6[1U];
  volatile  uint32_t ICIMVAU;                 
  volatile  uint32_t DCIMVAC;                 
  volatile  uint32_t DCISW;                   
  volatile  uint32_t DCCMVAU;                 
  volatile  uint32_t DCCMVAC;                 
  volatile  uint32_t DCCSW;                   
  volatile  uint32_t DCCIMVAC;                
  volatile  uint32_t DCCISW;                  
        uint32_t RESERVED7[6U];
  volatile uint32_t ITCMCR;                  
  volatile uint32_t DTCMCR;                  
  volatile uint32_t AHBPCR;                  
  volatile uint32_t CACR;                    
  volatile uint32_t AHBSCR;                  
        uint32_t RESERVED8[1U];
  volatile uint32_t ABFSR;                   
} SCB_Type;

 





 










 

 







 



 









 














 



 






 







 






 



 





 


 





 







 


 

 


 


 


 




 




 


 



 



 






 







 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const  uint32_t ICTR;                    
  volatile uint32_t ACTLR;                   
} SCnSCB_Type;

 

 





 







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t LOAD;                    
  volatile uint32_t VAL;                     
  volatile const  uint32_t CALIB;                   
} SysTick_Type;

 




 

 

 



 







 



 
typedef struct
{
  volatile  union
  {
    volatile  uint8_t    u8;                  
    volatile  uint16_t   u16;                 
    volatile  uint32_t   u32;                 
  }  PORT [32U];                          
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;                     
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;                     
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;                     
        uint32_t RESERVED3[29U];
  volatile  uint32_t IWR;                     
  volatile const  uint32_t IRR;                     
  volatile uint32_t IMCR;                    
        uint32_t RESERVED4[43U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
        uint32_t RESERVED5[6U];
  volatile const  uint32_t PID4;                    
  volatile const  uint32_t PID5;                    
  volatile const  uint32_t PID6;                    
  volatile const  uint32_t PID7;                    
  volatile const  uint32_t PID0;                    
  volatile const  uint32_t PID1;                    
  volatile const  uint32_t PID2;                    
  volatile const  uint32_t PID3;                    
  volatile const  uint32_t CID0;                    
  volatile const  uint32_t CID1;                    
  volatile const  uint32_t CID2;                    
  volatile const  uint32_t CID3;                    
} ITM_Type;

 

 









 

 

 

 



   







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t CYCCNT;                  
  volatile uint32_t CPICNT;                  
  volatile uint32_t EXCCNT;                  
  volatile uint32_t SLEEPCNT;                
  volatile uint32_t LSUCNT;                  
  volatile uint32_t FOLDCNT;                 
  volatile const  uint32_t PCSR;                    
  volatile uint32_t COMP0;                   
  volatile uint32_t MASK0;                   
  volatile uint32_t FUNCTION0;               
        uint32_t RESERVED0[1U];
  volatile uint32_t COMP1;                   
  volatile uint32_t MASK1;                   
  volatile uint32_t FUNCTION1;               
        uint32_t RESERVED1[1U];
  volatile uint32_t COMP2;                   
  volatile uint32_t MASK2;                   
  volatile uint32_t FUNCTION2;               
        uint32_t RESERVED2[1U];
  volatile uint32_t COMP3;                   
  volatile uint32_t MASK3;                   
  volatile uint32_t FUNCTION3;               
        uint32_t RESERVED3[981U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
} DWT_Type;

 


















 

 

 

 

 

 

 









   







 



 
typedef struct
{
  volatile const  uint32_t SSPSR;                   
  volatile uint32_t CSPSR;                   
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;                    
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;                    
        uint32_t RESERVED2[131U];
  volatile const  uint32_t FFSR;                    
  volatile uint32_t FFCR;                    
  volatile const  uint32_t FSCR;                    
        uint32_t RESERVED3[759U];
  volatile const  uint32_t TRIGGER;                 
  volatile const  uint32_t FIFO0;                   
  volatile const  uint32_t ITATBCTR2;               
        uint32_t RESERVED4[1U];
  volatile const  uint32_t ITATBCTR0;               
  volatile const  uint32_t FIFO1;                   
  volatile uint32_t ITCTRL;                  
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;                
  volatile uint32_t CLAIMCLR;                
        uint32_t RESERVED7[8U];
  volatile const  uint32_t DEVID;                   
  volatile const  uint32_t DEVTYPE;                 
} TPI_Type;

 

 

 




 


 

 







 


 







 


 

 






 


   







 



 
typedef struct
{
  volatile const  uint32_t TYPE;                    
  volatile uint32_t CTRL;                    
  volatile uint32_t RNR;                     
  volatile uint32_t RBAR;                    
  volatile uint32_t RASR;                    
  volatile uint32_t RBAR_A1;                 
  volatile uint32_t RASR_A1;                 
  volatile uint32_t RBAR_A2;                 
  volatile uint32_t RASR_A2;                 
  volatile uint32_t RBAR_A3;                 
  volatile uint32_t RASR_A3;                 
} MPU_Type;


 



 



 

 



 










 







 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile uint32_t FPCCR;                   
  volatile uint32_t FPCAR;                   
  volatile uint32_t FPDSCR;                  
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
} FPU_Type;

 









 

 




 








 




 

 







 



 
typedef struct
{
  volatile uint32_t DHCSR;                   
  volatile  uint32_t DCRSR;                   
  volatile uint32_t DCRDR;                   
  volatile uint32_t DEMCR;                   
} CoreDebug_Type;

 












 


 













 







 






 






 

 







 

 




 










 


 



 





 





 










 
static inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);              

  reg_value  =  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;                                                    
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));  
  reg_value  =  (reg_value                                   |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U)  );               
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR =  reg_value;
}






 
static inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}







 
static inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}









 
static inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}







 
static inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __iar_builtin_DSB();
    __iar_builtin_ISB();
  }
}









 
static inline uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}







 
static inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}







 
static inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}









 
static inline uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}










 
static inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]                = (uint8_t)((priority << (8U - 4)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 4)) & (uint32_t)0xFFUL);
  }
}










 
static inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]                >> (8U - 4)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 4)));
  }
}












 
static inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority     & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL)))
         );
}












 
static inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority     = (Priority                   ) & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL);
}










 
static inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  vectors[(int32_t)IRQn + 16] = vector;
}









 
static inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return vectors[(int32_t)IRQn + 16];
}





 
__attribute__((__noreturn__)) static inline void __NVIC_SystemReset(void)
{
  __iar_builtin_DSB();                                                          
 
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR  = (uint32_t)((0x5FAUL << 16U)    |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U)    );          
  __iar_builtin_DSB();                                                           

  for(;;)                                                            
  {
    __iar_builtin_no_operation();
  }
}

 

 







 
















 
 
  #pragma system_include          
 







 








   









 
  











                          







  









  











  



 



 



 



 




 
typedef struct {
  uint32_t RBAR; 
  uint32_t RASR; 
} ARM_MPU_Region_t;
    


 
static inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  __iar_builtin_DSB();
  __iar_builtin_ISB();
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);
}


 
static inline void ARM_MPU_Disable(void)
{
  __iar_builtin_DSB();
  __iar_builtin_ISB();
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL  &= ~(1UL );
}



 
static inline void ARM_MPU_ClrRegion(uint32_t rnr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0U;
}




    
static inline void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}





    
static inline void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}





 
static inline void orderedCpy(volatile uint32_t* dst, const uint32_t* __restrict src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i) 
  {
    dst[i] = src[i];
  }
}




 
static inline void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt) 
{
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > 4U) {
    orderedCpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), 4U*rowWordSize);
    table += 4U;
    cnt -= 4U;
  }
  orderedCpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), cnt*rowWordSize);
}



 





 








 
static inline uint32_t SCB_GetFPUType(void)
{
  uint32_t mvfr0;

  mvfr0 = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->MVFR0;
  if      ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x220U)
  {
    return 2U;            
  }
  else if ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x020U)
  {
    return 1U;            
  }
  else
  {
    return 0U;            
  }
}


 



 





 

 





 
static inline void SCB_EnableICache (void)
{
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;                      
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR |=  (uint32_t)(1UL << 17U);   
    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_DisableICache (void)
{
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR &= ~(uint32_t)(1UL << 17U);   
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;                      
    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_InvalidateICache (void)
{
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;
    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_EnableDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    __iar_builtin_DSB();

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR |=  (uint32_t)(1UL << 16U);   

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_DisableDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    __iar_builtin_DSB();

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR &= ~(uint32_t)(1UL << 16U);   
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                       ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_InvalidateDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_CleanDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

     ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
   __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCSW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
static inline void SCB_CleanInvalidateDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;     
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                       ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}







 
static inline void SCB_InvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize)
{
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t)addr;
     int32_t linesize = 32;                 

    __iar_builtin_DSB();

    while (op_size > 0) {
      ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCIMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}







 
static inline void SCB_CleanDCache_by_Addr (uint32_t *addr, int32_t dsize)
{
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 32;                 

    __iar_builtin_DSB();

    while (op_size > 0) {
      ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}







 
static inline void SCB_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize)
{
     int32_t op_size = dsize;
    uint32_t op_addr = (uint32_t) addr;
     int32_t linesize = 32;                 

    __iar_builtin_DSB();

    while (op_size > 0) {
      ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCIMVAC = op_addr;
      op_addr += (uint32_t)linesize;
      op_size -=           linesize;
    }

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}


 



 





 












 
static inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);                                                    
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD  = (uint32_t)(ticks - 1UL);                          
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 4) - 1UL);  
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL   = 0UL;                                              
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL  = (1UL << 2U) |
                   (1UL << 1U)   |
                   (1UL );                          
  return (0UL);                                                      
}


 



 





 

extern volatile int32_t ITM_RxBuffer;                               









 
static inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&       
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL               ) != 0UL)   )      
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __iar_builtin_no_operation();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}







 
static inline int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;                            

  if (ITM_RxBuffer != ((int32_t)0x5AA55AA5U))
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ((int32_t)0x5AA55AA5U);        
  }

  return (ch);
}







 
static inline int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ((int32_t)0x5AA55AA5U))
  {
    return (0);                               
  }
  else
  {
    return (1);                               
  }
}

 









































 



 



   
  


 




 



 




 
  






 
extern uint32_t SystemCoreClock;           

extern const uint8_t  AHBPrescTable[16];     
extern const uint8_t  APBPrescTable[8];      




 



 



 



 



 



 
  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);


 





 
  


   
 



 



 

typedef struct
{
  volatile uint32_t SR;      
  volatile uint32_t CR1;     
  volatile uint32_t CR2;     
  volatile uint32_t SMPR1;   
  volatile uint32_t SMPR2;   
  volatile uint32_t JOFR1;   
  volatile uint32_t JOFR2;   
  volatile uint32_t JOFR3;   
  volatile uint32_t JOFR4;   
  volatile uint32_t HTR;     
  volatile uint32_t LTR;     
  volatile uint32_t SQR1;    
  volatile uint32_t SQR2;    
  volatile uint32_t SQR3;    
  volatile uint32_t JSQR;    
  volatile uint32_t JDR1;    
  volatile uint32_t JDR2;    
  volatile uint32_t JDR3;    
  volatile uint32_t JDR4;    
  volatile uint32_t DR;      
} ADC_TypeDef;

typedef struct
{
  volatile uint32_t CSR;     
  volatile uint32_t CCR;     
  volatile uint32_t CDR;    
 
} ADC_Common_TypeDef;




 

typedef struct
{
  volatile uint32_t TIR;   
  volatile uint32_t TDTR;  
  volatile uint32_t TDLR;  
  volatile uint32_t TDHR;  
} CAN_TxMailBox_TypeDef;



 

typedef struct
{
  volatile uint32_t RIR;   
  volatile uint32_t RDTR;  
  volatile uint32_t RDLR;  
  volatile uint32_t RDHR;  
} CAN_FIFOMailBox_TypeDef;



 

typedef struct
{
  volatile uint32_t FR1;  
  volatile uint32_t FR2;  
} CAN_FilterRegister_TypeDef;



 

typedef struct
{
  volatile uint32_t              MCR;                  
  volatile uint32_t              MSR;                  
  volatile uint32_t              TSR;                  
  volatile uint32_t              RF0R;                 
  volatile uint32_t              RF1R;                 
  volatile uint32_t              IER;                  
  volatile uint32_t              ESR;                  
  volatile uint32_t              BTR;                  
  uint32_t                   RESERVED0[88];        
  CAN_TxMailBox_TypeDef      sTxMailBox[3];        
  CAN_FIFOMailBox_TypeDef    sFIFOMailBox[2];      
  uint32_t                   RESERVED1[12];        
  volatile uint32_t              FMR;                  
  volatile uint32_t              FM1R;                 
  uint32_t                   RESERVED2;            
  volatile uint32_t              FS1R;                 
  uint32_t                   RESERVED3;            
  volatile uint32_t              FFA1R;                
  uint32_t                   RESERVED4;            
  volatile uint32_t              FA1R;                 
  uint32_t                   RESERVED5[8];         
  CAN_FilterRegister_TypeDef sFilterRegister[28];  
} CAN_TypeDef;




 

typedef struct
{
  volatile uint32_t  DR;           
  volatile uint8_t   IDR;          
  uint8_t        RESERVED0;    
  uint16_t       RESERVED1;    
  volatile uint32_t  CR;           
  uint32_t       RESERVED2;    
  volatile uint32_t  INIT;         
  volatile uint32_t  POL;          
} CRC_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;        
  volatile uint32_t SWTRIGR;   
  volatile uint32_t DHR12R1;   
  volatile uint32_t DHR12L1;   
  volatile uint32_t DHR8R1;    
  volatile uint32_t DHR12R2;   
  volatile uint32_t DHR12L2;   
  volatile uint32_t DHR8R2;    
  volatile uint32_t DHR12RD;   
  volatile uint32_t DHR12LD;   
  volatile uint32_t DHR8RD;    
  volatile uint32_t DOR1;      
  volatile uint32_t DOR2;      
  volatile uint32_t SR;        
} DAC_TypeDef;




 

typedef struct
{
  volatile uint32_t IDCODE;   
  volatile uint32_t CR;       
  volatile uint32_t APB1FZ;   
  volatile uint32_t APB2FZ;   
}DBGMCU_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;      
  volatile uint32_t NDTR;    
  volatile uint32_t PAR;     
  volatile uint32_t M0AR;    
  volatile uint32_t M1AR;    
  volatile uint32_t FCR;     
} DMA_Stream_TypeDef;

typedef struct
{
  volatile uint32_t LISR;    
  volatile uint32_t HISR;    
  volatile uint32_t LIFCR;   
  volatile uint32_t HIFCR;   
} DMA_TypeDef;




 

typedef struct
{
  volatile uint32_t IMR;     
  volatile uint32_t EMR;     
  volatile uint32_t RTSR;    
  volatile uint32_t FTSR;    
  volatile uint32_t SWIER;   
  volatile uint32_t PR;      
} EXTI_TypeDef;



 

typedef struct
{
  volatile uint32_t ACR;       
  volatile uint32_t KEYR;      
  volatile uint32_t OPTKEYR;   
  volatile uint32_t SR;        
  volatile uint32_t CR;        
  volatile uint32_t OPTCR;     
  volatile uint32_t OPTCR1;    
  volatile uint32_t OPTCR2;    
} FLASH_TypeDef;





 

typedef struct
{
  volatile uint32_t BTCR[8];     
} FMC_Bank1_TypeDef;



 

typedef struct
{
  volatile uint32_t BWTR[7];     
} FMC_Bank1E_TypeDef;



 

typedef struct
{
  volatile uint32_t PCR;         
  volatile uint32_t SR;          
  volatile uint32_t PMEM;        
  volatile uint32_t PATT;        
  uint32_t      RESERVED0;   
  volatile uint32_t ECCR;        
} FMC_Bank3_TypeDef;



 

typedef struct
{
  volatile uint32_t SDCR[2];         
  volatile uint32_t SDTR[2];         
  volatile uint32_t SDCMR;        
  volatile uint32_t SDRTR;        
  volatile uint32_t SDSR;         
} FMC_Bank5_6_TypeDef;




 

typedef struct
{
  volatile uint32_t MODER;     
  volatile uint32_t OTYPER;    
  volatile uint32_t OSPEEDR;   
  volatile uint32_t PUPDR;     
  volatile uint32_t IDR;       
  volatile uint32_t ODR;       
  volatile uint32_t BSRR;      
  volatile uint32_t LCKR;      
  volatile uint32_t AFR[2];    
} GPIO_TypeDef;



 

typedef struct
{
  volatile uint32_t MEMRMP;        
  volatile uint32_t PMC;           
  volatile uint32_t EXTICR[4];     
  uint32_t      RESERVED[2];   
  volatile uint32_t CMPCR;         
} SYSCFG_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;       
  volatile uint32_t CR2;       
  volatile uint32_t OAR1;      
  volatile uint32_t OAR2;      
  volatile uint32_t TIMINGR;   
  volatile uint32_t TIMEOUTR;  
  volatile uint32_t ISR;       
  volatile uint32_t ICR;       
  volatile uint32_t PECR;      
  volatile uint32_t RXDR;      
  volatile uint32_t TXDR;      
} I2C_TypeDef;



 

typedef struct
{
  volatile uint32_t KR;    
  volatile uint32_t PR;    
  volatile uint32_t RLR;   
  volatile uint32_t SR;    
  volatile uint32_t WINR;  
} IWDG_TypeDef;





 

typedef struct
{
  volatile uint32_t CR1;    
  volatile uint32_t CSR1;   
  volatile uint32_t CR2;    
  volatile uint32_t CSR2;   
} PWR_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t PLLCFGR;        
  volatile uint32_t CFGR;           
  volatile uint32_t CIR;            
  volatile uint32_t AHB1RSTR;       
  volatile uint32_t AHB2RSTR;       
  volatile uint32_t AHB3RSTR;       
  uint32_t      RESERVED0;      
  volatile uint32_t APB1RSTR;       
  volatile uint32_t APB2RSTR;       
  uint32_t      RESERVED1[2];   
  volatile uint32_t AHB1ENR;        
  volatile uint32_t AHB2ENR;        
  volatile uint32_t AHB3ENR;        
  uint32_t      RESERVED2;      
  volatile uint32_t APB1ENR;        
  volatile uint32_t APB2ENR;        
  uint32_t      RESERVED3[2];   
  volatile uint32_t AHB1LPENR;      
  volatile uint32_t AHB2LPENR;      
  volatile uint32_t AHB3LPENR;      
  uint32_t      RESERVED4;      
  volatile uint32_t APB1LPENR;      
  volatile uint32_t APB2LPENR;      
  uint32_t      RESERVED5[2];   
  volatile uint32_t BDCR;           
  volatile uint32_t CSR;            
  uint32_t      RESERVED6[2];   
  volatile uint32_t SSCGR;          
  volatile uint32_t PLLI2SCFGR;     
  volatile uint32_t PLLSAICFGR;     
  volatile uint32_t DCKCFGR1;       
  volatile uint32_t DCKCFGR2;       

} RCC_TypeDef;



 

typedef struct
{
  volatile uint32_t TR;          
  volatile uint32_t DR;          
  volatile uint32_t CR;          
  volatile uint32_t ISR;         
  volatile uint32_t PRER;        
  volatile uint32_t WUTR;        
       uint32_t reserved;    
  volatile uint32_t ALRMAR;      
  volatile uint32_t ALRMBR;      
  volatile uint32_t WPR;         
  volatile uint32_t SSR;         
  volatile uint32_t SHIFTR;      
  volatile uint32_t TSTR;        
  volatile uint32_t TSDR;        
  volatile uint32_t TSSSR;       
  volatile uint32_t CALR;        
  volatile uint32_t TAMPCR;      
  volatile uint32_t ALRMASSR;    
  volatile uint32_t ALRMBSSR;    
  volatile uint32_t OR;          
  volatile uint32_t BKP0R;       
  volatile uint32_t BKP1R;       
  volatile uint32_t BKP2R;       
  volatile uint32_t BKP3R;       
  volatile uint32_t BKP4R;       
  volatile uint32_t BKP5R;       
  volatile uint32_t BKP6R;       
  volatile uint32_t BKP7R;       
  volatile uint32_t BKP8R;       
  volatile uint32_t BKP9R;       
  volatile uint32_t BKP10R;      
  volatile uint32_t BKP11R;      
  volatile uint32_t BKP12R;      
  volatile uint32_t BKP13R;      
  volatile uint32_t BKP14R;      
  volatile uint32_t BKP15R;      
  volatile uint32_t BKP16R;      
  volatile uint32_t BKP17R;      
  volatile uint32_t BKP18R;      
  volatile uint32_t BKP19R;      
  volatile uint32_t BKP20R;      
  volatile uint32_t BKP21R;      
  volatile uint32_t BKP22R;      
  volatile uint32_t BKP23R;      
  volatile uint32_t BKP24R;      
  volatile uint32_t BKP25R;      
  volatile uint32_t BKP26R;      
  volatile uint32_t BKP27R;      
  volatile uint32_t BKP28R;      
  volatile uint32_t BKP29R;      
  volatile uint32_t BKP30R;      
  volatile uint32_t BKP31R;      
} RTC_TypeDef;




 

typedef struct
{
  volatile uint32_t GCR;       
} SAI_TypeDef;

typedef struct
{
  volatile uint32_t CR1;       
  volatile uint32_t CR2;       
  volatile uint32_t FRCR;      
  volatile uint32_t SLOTR;     
  volatile uint32_t IMR;       
  volatile uint32_t SR;        
  volatile uint32_t CLRFR;     
  volatile uint32_t DR;        
} SAI_Block_TypeDef;




 

typedef struct
{
  volatile uint32_t POWER;           
  volatile uint32_t CLKCR;           
  volatile uint32_t ARG;             
  volatile uint32_t CMD;             
  volatile const uint32_t  RESPCMD;         
  volatile const uint32_t  RESP1;           
  volatile const uint32_t  RESP2;           
  volatile const uint32_t  RESP3;           
  volatile const uint32_t  RESP4;           
  volatile uint32_t DTIMER;          
  volatile uint32_t DLEN;            
  volatile uint32_t DCTRL;           
  volatile const uint32_t  DCOUNT;          
  volatile const uint32_t  STA;             
  volatile uint32_t ICR;             
  volatile uint32_t MASK;            
  uint32_t      RESERVED0[2];    
  volatile const uint32_t  FIFOCNT;         
  uint32_t      RESERVED1[13];   
  volatile uint32_t FIFO;            
} SDMMC_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;         
  volatile uint32_t CR2;         
  volatile uint32_t SR;          
  volatile uint32_t DR;          
  volatile uint32_t CRCPR;       
  volatile uint32_t RXCRCR;      
  volatile uint32_t TXCRCR;      
  volatile uint32_t I2SCFGR;     
  volatile uint32_t I2SPR;       
} SPI_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;        
  volatile uint32_t DCR;       
  volatile uint32_t SR;        
  volatile uint32_t FCR;       
  volatile uint32_t DLR;       
  volatile uint32_t CCR;       
  volatile uint32_t AR;        
  volatile uint32_t ABR;       
  volatile uint32_t DR;        
  volatile uint32_t PSMKR;     
  volatile uint32_t PSMAR;     
  volatile uint32_t PIR;       
  volatile uint32_t LPTR;      
} QUADSPI_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;          
  volatile uint32_t CR2;          
  volatile uint32_t SMCR;         
  volatile uint32_t DIER;         
  volatile uint32_t SR;           
  volatile uint32_t EGR;          
  volatile uint32_t CCMR1;        
  volatile uint32_t CCMR2;        
  volatile uint32_t CCER;         
  volatile uint32_t CNT;          
  volatile uint32_t PSC;          
  volatile uint32_t ARR;          
  volatile uint32_t RCR;          
  volatile uint32_t CCR1;         
  volatile uint32_t CCR2;         
  volatile uint32_t CCR3;         
  volatile uint32_t CCR4;         
  volatile uint32_t BDTR;         
  volatile uint32_t DCR;          
  volatile uint32_t DMAR;         
  volatile uint32_t OR;           
  volatile uint32_t CCMR3;        
  volatile uint32_t CCR5;         
  volatile uint32_t CCR6;         

} TIM_TypeDef;



 
typedef struct
{
  volatile uint32_t ISR;       
  volatile uint32_t ICR;       
  volatile uint32_t IER;       
  volatile uint32_t CFGR;      
  volatile uint32_t CR;        
  volatile uint32_t CMP;       
  volatile uint32_t ARR;       
  volatile uint32_t CNT;       
} LPTIM_TypeDef;




 

typedef struct
{
  volatile uint32_t CR1;     
  volatile uint32_t CR2;     
  volatile uint32_t CR3;     
  volatile uint32_t BRR;     
  volatile uint32_t GTPR;    
  volatile uint32_t RTOR;    
  volatile uint32_t RQR;     
  volatile uint32_t ISR;     
  volatile uint32_t ICR;     
  volatile uint32_t RDR;     
  volatile uint32_t TDR;     
} USART_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;    
  volatile uint32_t CFR;   
  volatile uint32_t SR;    
} WWDG_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;   
  volatile uint32_t SR;   
  volatile uint32_t DR;   
} RNG_TypeDef;



 



 
typedef struct
{
 volatile uint32_t GOTGCTL;                
  volatile uint32_t GOTGINT;               
  volatile uint32_t GAHBCFG;               
  volatile uint32_t GUSBCFG;               
  volatile uint32_t GRSTCTL;               
  volatile uint32_t GINTSTS;               
  volatile uint32_t GINTMSK;               
  volatile uint32_t GRXSTSR;               
  volatile uint32_t GRXSTSP;               
  volatile uint32_t GRXFSIZ;               
  volatile uint32_t DIEPTXF0_HNPTXFSIZ;    
  volatile uint32_t HNPTXSTS;              
  uint32_t Reserved30[2];              
  volatile uint32_t GCCFG;                 
  volatile uint32_t CID;                   
  uint32_t  Reserved5[3];              
  volatile uint32_t GHWCFG3;               
  uint32_t  Reserved6;                 
  volatile uint32_t GLPMCFG;               
  uint32_t  Reserved7;                 
  volatile uint32_t GDFIFOCFG;             
  uint32_t  Reserved43[40];            
  volatile uint32_t HPTXFSIZ;              
  volatile uint32_t DIEPTXF[0x0F];         
} USB_OTG_GlobalTypeDef;




 
typedef struct
{
  volatile uint32_t DCFG;             
  volatile uint32_t DCTL;             
  volatile uint32_t DSTS;             
  uint32_t Reserved0C;            
  volatile uint32_t DIEPMSK;          
  volatile uint32_t DOEPMSK;          
  volatile uint32_t DAINT;            
  volatile uint32_t DAINTMSK;         
  uint32_t  Reserved20;           
  uint32_t Reserved9;             
  volatile uint32_t DVBUSDIS;         
  volatile uint32_t DVBUSPULSE;       
  volatile uint32_t DTHRCTL;          
  volatile uint32_t DIEPEMPMSK;       
  volatile uint32_t DEACHINT;         
  volatile uint32_t DEACHMSK;         
  uint32_t Reserved40;            
  volatile uint32_t DINEP1MSK;        
  uint32_t  Reserved44[15];       
  volatile uint32_t DOUTEP1MSK;       
} USB_OTG_DeviceTypeDef;




 
typedef struct
{
  volatile uint32_t DIEPCTL;            
  uint32_t Reserved04;              
  volatile uint32_t DIEPINT;            
  uint32_t Reserved0C;              
  volatile uint32_t DIEPTSIZ;           
  volatile uint32_t DIEPDMA;            
  volatile uint32_t DTXFSTS;            
  uint32_t Reserved18;              
} USB_OTG_INEndpointTypeDef;




 
typedef struct
{
  volatile uint32_t DOEPCTL;        
  uint32_t Reserved04;          
  volatile uint32_t DOEPINT;        
  uint32_t Reserved0C;          
  volatile uint32_t DOEPTSIZ;       
  volatile uint32_t DOEPDMA;        
  uint32_t Reserved18[2];       
} USB_OTG_OUTEndpointTypeDef;




 
typedef struct
{
  volatile uint32_t HCFG;              
  volatile uint32_t HFIR;              
  volatile uint32_t HFNUM;             
  uint32_t Reserved40C;            
  volatile uint32_t HPTXSTS;           
  volatile uint32_t HAINT;             
  volatile uint32_t HAINTMSK;          
} USB_OTG_HostTypeDef;



 
typedef struct
{
  volatile uint32_t HCCHAR;            
  volatile uint32_t HCSPLT;            
  volatile uint32_t HCINT;             
  volatile uint32_t HCINTMSK;          
  volatile uint32_t HCTSIZ;            
  volatile uint32_t HCDMA;             
  uint32_t Reserved[2];            
} USB_OTG_HostChannelTypeDef;


 






 

 

 

 

 
 
 

 
 

 

 




 



 



 



 

  

 

 
 
 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 

 

 

 
 
 
 
 
 
 

 

 



 

 

 

 



 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 


 

 


 

 

 

 

 
 
 
 
 


 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 


 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 


 
 
 
 
 
 

 

 


 

 

 
 
 
 
 
 

 

 

 

 

 
 

 

 

 

 

 

 

 
 
 
 
 
 

 


 
 

 

 


 


 


 

 





 

 

 

 


 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 






 

 
 
 
 
 
 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 

 

 
                     

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 


 







 





 



 

 




 

 


 

 


 
 
 
 
 
 

 



 

 



 

 

 

 

 

 

 

 

 



 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 


 
 
 
 
 
 



 



 


 



 



 



 

 


 



 



 



 

 



 



 



 



 


 


 



 



 



 


 

 
 
 
 
 
 




 





 






 

 

 

 








 





 










 





 


 

 

 

 

 

 

 

 

 



 


 

 

 

 

 

 






 

 


 
 
 
 
 
 

 

 

 








 

 

 

 
 
 
 
 
 


 
 
 

 

 

 

 

 


 

 

 

 

 

 

 
 
 
 
 
 



 





 

 
 
 
 
 
 

 


 

 


 
 
 
 
 
 

 

 




 

 

 


 

 

 


 

 

 

 

 


 

 

 

 

 

 

 

 

 






 






 

 

 

 

 



 



 

 

 

 

 

 

 

 




 

 

 



 





 




 

 

 

 

 

 

 

 

 

 

 

 


 






 



 



 

 


 
 

 




 

 



 


 

 


 

 





 

 

 
 

 


 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
                                                        
 
                                                        
 

 

 

 

 

 

 

 

 

 

 

 

 
	 
 
                                                        
 
                                                        
 
                                                        
 
                                                        
 
                                                         
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 
 

 

 



 



 



 




 



 



 
typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;

typedef enum
{
  SUCCESS = 0U,
  ERROR = !SUCCESS
} ErrorStatus;



 



 










 


















  

 







 



 

 

















 

 


 
 
 



 


 



 






 



 




 



 





 



 


 



 


 



 




 



 








 



 







 



 




 



 




 



 







 




 


 



 



 



 













 



 


 



 








 



 


 



 



 



 


 



 




 
 




 


 



 



 




 



 



 



 












 



 




 



 

 

 



 



 















 




 





 




 


 



 






 



 









 



 


 



 








 




 




 



 



 



 



 



 







 



 




 



 



 



 


 




 



 

 



 


 



 




 



 



 


 

 











 



 



 



 



 



 





 



 









 



 



 



 


 



 


 



 


 



 


 



 


 




 



 

 



 


 



 




 




 








 



 



 



 






 



 




 




 




 



 




 



 



 



 





 



 







 




 


 




 




 




 



 




 



 




 










 































 



 



 



 








 



 





 




 



 






 



 


 



 




 



 






 




 






 



 









 



 







 



 




 



 


 



 


 



 


 



 


 



 


 



 


 



 



 




 
 

  #pragma system_include

 
 

 

  #pragma system_include














 



 


 
  typedef   signed int ptrdiff_t;



    typedef union
    {
      long long _ll;
      long double _ld;
      void *_vp;
    } _Max_align_t;
    typedef _Max_align_t max_align_t;






 

 



   
typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;



 
typedef enum 
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED   = 0x01U  
} HAL_LockTypeDef;

 



















 





 

 



  



 




  


 





   
 
 















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t PLLState;   
 

  uint32_t PLLSource;  
 

  uint32_t PLLM;       
 

  uint32_t PLLN;       
 

  uint32_t PLLP;       
 

  uint32_t PLLQ;       
 

}RCC_PLLInitTypeDef;



 
typedef struct
{
  uint32_t PLLI2SN;    

 

  uint32_t PLLI2SR;    

 

  uint32_t PLLI2SQ;    

 

}RCC_PLLI2SInitTypeDef;



 
typedef struct
{
  uint32_t PLLSAIN;    

 

  uint32_t PLLSAIQ;    

 


  uint32_t PLLSAIP;    

 
}RCC_PLLSAIInitTypeDef;



 
typedef struct
{
  uint32_t PeriphClockSelection; 
 

  RCC_PLLI2SInitTypeDef PLLI2S;  
 

  RCC_PLLSAIInitTypeDef PLLSAI;  
 

  uint32_t PLLI2SDivQ;           

 

  uint32_t PLLSAIDivQ;           

 

  uint32_t PLLSAIDivR;           
 

  uint32_t RTCClockSelection;      
 

  uint32_t I2sClockSelection;      
 

  uint32_t TIMPresSelection;      
 

  uint32_t Sai1ClockSelection;     
 

  uint32_t Sai2ClockSelection;     
 

  uint32_t Usart1ClockSelection; 
 

  uint32_t Usart2ClockSelection; 
 

  uint32_t Usart3ClockSelection; 
 

  uint32_t Uart4ClockSelection;  
 

  uint32_t Uart5ClockSelection;  
 

  uint32_t Usart6ClockSelection;  
 

  uint32_t Uart7ClockSelection;  
 

  uint32_t Uart8ClockSelection;  
 

  uint32_t I2c1ClockSelection;   
 

  uint32_t I2c2ClockSelection;   
 

  uint32_t I2c3ClockSelection;   
 

  uint32_t I2c4ClockSelection;   
 

  uint32_t Lptim1ClockSelection;   
 

  uint32_t CecClockSelection;      
 

  uint32_t Clk48ClockSelection;    
 

  uint32_t Sdmmc1ClockSelection;     
 

  uint32_t Sdmmc2ClockSelection;     
 

}RCC_PeriphCLKInitTypeDef;


 

 


 



 



 




 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 



 



 



 


 



 


 



 


 



 


 





 

 


 






 





 





















 











 







 































 


























 







 





 







 










 






 







 








 




 


 




 







 



 




 










 








 


 








 











 







 









 




 








 






 








 











 







 








 




 

 


























 
 












 



 













 















 






 






 
















 












 















 













 



 


 


 



 



 





 








 






 








 






 








 






 








 






 









 







 









 







 









 







 

 







 







 

 







 







 

 







 







 

 







 







 









 







 









 







 







 





 







 





 







 





 






 





 




 

 


 
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);
HAL_StatusTypeDef HAL_RCCEx_EnablePLLI2S(RCC_PLLI2SInitTypeDef  *PLLI2SInit);
HAL_StatusTypeDef HAL_RCCEx_DisablePLLI2S(void);
HAL_StatusTypeDef HAL_RCCEx_EnablePLLSAI(RCC_PLLSAIInitTypeDef  *PLLSAIInit);
HAL_StatusTypeDef HAL_RCCEx_DisablePLLSAI(void);


 
 


 


 




















 



 



 



 





 



 

  



 



 
typedef struct
{
  uint32_t OscillatorType;       
 

  uint32_t HSEState;             
 

  uint32_t LSEState;             
 
                                          
  uint32_t HSIState;             
 

  uint32_t HSICalibrationValue;   
 
                               
  uint32_t LSIState;             
 

  RCC_PLLInitTypeDef PLL;               

}RCC_OscInitTypeDef;



 
typedef struct
{
  uint32_t ClockType;             
 
  
  uint32_t SYSCLKSource;          
 

  uint32_t AHBCLKDivider;         
 

  uint32_t APB1CLKDivider;        
 

  uint32_t APB2CLKDivider;        
 

}RCC_ClkInitTypeDef;



 

 


 



 


 



 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 
  


 


 




 


 



 


  
  


 


  



 


 





 


 



 


 



 


 



 


 



 


 
  








 
 

 

 


  



 


 
  


 
   
 


 







 
									  




 







 
									  



 







 
									  



 
  






 



 
  






 



   







 


   
  



   



 




 



 




 




 







 







 







 



 
  







 



 








 



 








 


   



  
                                      













 






 


 



  








 


 



  





















 


 



 


















 


 



 



 






















 
                                                   







 






 





 


 



 







 
                            







 









 


 



 








 



 


 



 







 







 














 






 


 
  


  
  














 

                














 



 
  



 











 











 












 












 



 


















 



 
     


 

 

 
 

 



                              
 
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(const RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);


 



 
 
void     HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void     HAL_RCC_EnableCSS(void);
void     HAL_RCC_DisableCSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
void     HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void     HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);

 
void HAL_RCC_NMI_IRQHandler(void);

  
void HAL_RCC_CSSCallback(void);


 



 

 
 
 


 




 
 

 



 


 

 


 
    


   






















 



 



  



 




















 

 


 



 




 

 



 
typedef enum
{
  HAL_EXTI_COMMON_CB_ID          = 0x00U
} EXTI_CallbackIDTypeDef;



 
typedef struct
{
  uint32_t Line;                     
  void (* PendingCallback)(void);    
} EXTI_HandleTypeDef;



 
typedef struct
{
  uint32_t Line;      
 
  uint32_t Mode;      
 
  uint32_t Trigger;   
 
  uint32_t GPIOSel;   

 
} EXTI_ConfigTypeDef;



 

 


 



 


 



 


 



 



 




 



 

 


 



 

 


 


 



 



 



 



 




 

 


 








 

 



 




 
 
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);


 




 
 
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti);



 



 



 



 




















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t Pin;       
 

  uint32_t Mode;      
 

  uint32_t Pull;      
 

  uint32_t Speed;     
 

  uint32_t Alternate;  
 
} GPIO_InitTypeDef;



 
typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
} GPIO_PinState;


 

 



 



 



 









 





 




 


 




 


 



 

 


 






 






 






 






 






 


 

 
















 

 


 



 



 

 

 


 



 
 
 

 


 



 



 



 



 



 



 



 



 



 



 



 



 



 



 
 



 



 

 


 


 

 


 


 
 
 
 


 



 
 



 

 


 


 



 



 


 



 

 


 



 



 



 




 


 



 
 
void  HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void  HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);


 



 
 
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



 



 
 
 
 


 


 

 


 


 

 


 



 



 



 




















  

 


 



 



  

 




 
   


 
typedef struct
{
  uint32_t Channel;              
 

  uint32_t Direction;            

 

  uint32_t PeriphInc;            
 

  uint32_t MemInc;               
 

  uint32_t PeriphDataAlignment;  
 

  uint32_t MemDataAlignment;     
 

  uint32_t Mode;                 


 

  uint32_t Priority;             
 

  uint32_t FIFOMode;             


 

  uint32_t FIFOThreshold;        
 

  uint32_t MemBurst;             



 

  uint32_t PeriphBurst;          



 
}DMA_InitTypeDef;



 
typedef enum
{
  HAL_DMA_STATE_RESET             = 0x00U,   
  HAL_DMA_STATE_READY             = 0x01U,   
  HAL_DMA_STATE_BUSY              = 0x02U,   
  HAL_DMA_STATE_TIMEOUT           = 0x03U,   
  HAL_DMA_STATE_ERROR             = 0x04U,   
  HAL_DMA_STATE_ABORT             = 0x05U,   
}HAL_DMA_StateTypeDef;



 
typedef enum
{
  HAL_DMA_FULL_TRANSFER      = 0x00U,     
  HAL_DMA_HALF_TRANSFER      = 0x01U,     
}HAL_DMA_LevelCompleteTypeDef;



 
typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID          = 0x00U,     
  HAL_DMA_XFER_HALFCPLT_CB_ID      = 0x01U,     
  HAL_DMA_XFER_M1CPLT_CB_ID        = 0x02U,     
  HAL_DMA_XFER_M1HALFCPLT_CB_ID    = 0x03U,     
  HAL_DMA_XFER_ERROR_CB_ID         = 0x04U,     
  HAL_DMA_XFER_ABORT_CB_ID         = 0x05U,     
  HAL_DMA_XFER_ALL_CB_ID           = 0x06U      
}HAL_DMA_CallbackIDTypeDef;



 
typedef struct __DMA_HandleTypeDef
{
  DMA_Stream_TypeDef         *Instance;                                                     

  DMA_InitTypeDef            Init;                                                           

  HAL_LockTypeDef            Lock;                                                            

  volatile HAL_DMA_StateTypeDef  State;                                                         

  void                       *Parent;                                                        

  void                       (* XferCpltCallback)( struct __DMA_HandleTypeDef * hdma);      

  void                       (* XferHalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);  

  void                       (* XferM1CpltCallback)( struct __DMA_HandleTypeDef * hdma);    
  
  void                       (* XferM1HalfCpltCallback)( struct __DMA_HandleTypeDef * hdma);    
  
  void                       (* XferErrorCallback)( struct __DMA_HandleTypeDef * hdma);     
  
  void                       (* XferAbortCallback)( struct __DMA_HandleTypeDef * hdma);       

 volatile uint32_t               ErrorCode;                                                     
  
 uint32_t                    StreamBaseAddress;                                             

 uint32_t                    StreamIndex;                                                   
 
}DMA_HandleTypeDef;



 


 




 




  


 




  


 
        



  


  




  


 




  


  




 


 




  


 




 


  




 


  




 


  




  


  




  


 




 


 




  


 



 
 
 




 












 





 





 

 





 





       





 





 





 













 













 












 












 












 

















 






 


 
















 

 


 



 



  

 



 
   


  
typedef enum
{
  MEMORY0      = 0x00U,     
  MEMORY1      = 0x01U,     

}HAL_DMA_MemoryTypeDef;



 
  
 




 




  



 
  


   

 



 




 

 
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_MultiBufferStart_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMAEx_ChangeMemory(DMA_HandleTypeDef *hdma, uint32_t Address, HAL_DMA_MemoryTypeDef memory);



 


 
  
 



 


   
         
 



 


 



 



 




 




 




 
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma); 
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);


 




 
HAL_StatusTypeDef HAL_DMA_Start (DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout);
void              HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);



  




 
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t             HAL_DMA_GetError(DMA_HandleTypeDef *hdma);


  


  
 



 


  

 



 













  

 



 


 



  



 



   
















  

 


 



 



  
 


 




 
typedef struct
{
  uint8_t                Enable;                
 
  uint8_t                Number;                
 
  uint32_t               BaseAddress;            
  uint8_t                Size;                  
 
  uint8_t                SubRegionDisable;      
          
  uint8_t                TypeExtField;          
                  
  uint8_t                AccessPermission;      
 
  uint8_t                DisableExec;           
 
  uint8_t                IsShareable;           
 
  uint8_t                IsCacheable;           
 
  uint8_t                IsBufferable;          
 
}MPU_Region_InitTypeDef;


 



 

 



 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 
   


 


 



 


 



 


 

 


 
  


 
 
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup);
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_NVIC_SystemReset(void);
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb);


 



 
 
void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_EnableRegion(uint32_t RegionNumber);
void HAL_MPU_DisableRegion(uint32_t RegionNumber);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);
uint32_t HAL_NVIC_GetPriorityGrouping(void);
void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn);
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource);
void HAL_SYSTICK_IRQHandler(void);
void HAL_SYSTICK_Callback(void);


 



 

  
 
 
 


 

















                                                                             



  



 
  

 





















 

 


 



 



 

 


 



 
typedef enum
{
  HAL_CRC_STATE_RESET     = 0x00U,   
  HAL_CRC_STATE_READY     = 0x01U,   
  HAL_CRC_STATE_BUSY      = 0x02U,   
  HAL_CRC_STATE_TIMEOUT   = 0x03U,   
  HAL_CRC_STATE_ERROR     = 0x04U    
} HAL_CRC_StateTypeDef;



 
typedef struct
{
  uint8_t DefaultPolynomialUse;       





 

  uint8_t DefaultInitValueUse;        


 

  uint32_t GeneratingPolynomial;      



 

  uint32_t CRCLength;                 




 

  uint32_t InitValue;                 
 

  uint32_t InputDataInversionMode;    







 

  uint32_t OutputDataInversionMode;   



 
} CRC_InitTypeDef;



 
typedef struct
{
  CRC_TypeDef                 *Instance;    

  CRC_InitTypeDef             Init;         

  HAL_LockTypeDef             Lock;         

  volatile HAL_CRC_StateTypeDef   State;        

  uint32_t InputDataFormat;                










 
} CRC_HandleTypeDef;


 

 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 


 



 

 


 




 





 






 







 






 


 


 


 







 

 
















 

 


 



 



 

 
 


 



 


 



 


 



 

 


 





 





 






 



 

 


 





 

 



 



 
 
HAL_StatusTypeDef HAL_CRCEx_Polynomial_Set(CRC_HandleTypeDef *hcrc, uint32_t Pol, uint32_t PolyLength);
HAL_StatusTypeDef HAL_CRCEx_Input_Data_Reverse(CRC_HandleTypeDef *hcrc, uint32_t InputReverseMode);
HAL_StatusTypeDef HAL_CRCEx_Output_Data_Reverse(CRC_HandleTypeDef *hcrc, uint32_t OutputReverseMode);



 



 



 



 



 


 

 


 
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
HAL_StatusTypeDef HAL_CRC_DeInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc);


 

 


 
uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, const uint32_t pBuffer[], uint32_t BufferLength);
uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, const uint32_t pBuffer[], uint32_t BufferLength);


 

 


 
HAL_CRC_StateTypeDef HAL_CRC_GetState(const CRC_HandleTypeDef *hcrc);


 



 



 



 





















 

 




 

 




 

 



 



 
typedef enum
{
  HAL_DAC_STATE_RESET             = 0x00U,   
  HAL_DAC_STATE_READY             = 0x01U,   
  HAL_DAC_STATE_BUSY              = 0x02U,   
  HAL_DAC_STATE_TIMEOUT           = 0x03U,   
  HAL_DAC_STATE_ERROR             = 0x04U    

} HAL_DAC_StateTypeDef;



 
typedef struct
{
  DAC_TypeDef                 *Instance;      

  volatile HAL_DAC_StateTypeDef   State;          

  HAL_LockTypeDef             Lock;           

  DMA_HandleTypeDef           *DMA_Handle1;   

  DMA_HandleTypeDef           *DMA_Handle2;   

  volatile uint32_t               ErrorCode;      


} DAC_HandleTypeDef;



 
typedef struct
{
  uint32_t DAC_Trigger;                  
 

  uint32_t DAC_OutputBuffer;             
 

} DAC_ChannelConfTypeDef;




 

 



 



 



 



 



 



 



 



 




 



 



 



 





 



 





 



 

 



 




 





 





 




 





 





 








 








 








 








 








 



 

 



 






 

 
















 

 




 

 




 

 



 

 



 



 



 




 

 


 



 



 

 
 



 



 
 

HAL_StatusTypeDef HAL_DACEx_TriangleWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Amplitude);
HAL_StatusTypeDef HAL_DACEx_NoiseWaveGenerate(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Amplitude);

HAL_StatusTypeDef HAL_DACEx_DualStart(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DACEx_DualStop(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DACEx_DualSetValue(DAC_HandleTypeDef *hdac, uint32_t Alignment, uint32_t Data1, uint32_t Data2);
uint32_t HAL_DACEx_DualGetValue(const DAC_HandleTypeDef *hdac);

void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ErrorCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_DMAUnderrunCallbackCh2(DAC_HandleTypeDef *hdac);




 



 



 



 

 
 
void DAC_DMAConvCpltCh2(DMA_HandleTypeDef *hdma);
void DAC_DMAErrorCh2(DMA_HandleTypeDef *hdma);
void DAC_DMAHalfConvCpltCh2(DMA_HandleTypeDef *hdma);



 



 




 



 



 



 
 
HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DAC_DeInit(DAC_HandleTypeDef *hdac);
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac);
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac);



 



 
 
HAL_StatusTypeDef HAL_DAC_Start(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Stop(DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_Start_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel, const uint32_t *pData, uint32_t Length,
                                    uint32_t Alignment);
HAL_StatusTypeDef HAL_DAC_Stop_DMA(DAC_HandleTypeDef *hdac, uint32_t Channel);
void HAL_DAC_IRQHandler(DAC_HandleTypeDef *hdac);
HAL_StatusTypeDef HAL_DAC_SetValue(DAC_HandleTypeDef *hdac, uint32_t Channel, uint32_t Alignment, uint32_t Data);

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac);
void HAL_DAC_DMAUnderrunCallbackCh1(DAC_HandleTypeDef *hdac);




 



 
 
uint32_t HAL_DAC_GetValue(const DAC_HandleTypeDef *hdac, uint32_t Channel);
HAL_StatusTypeDef HAL_DAC_ConfigChannel(DAC_HandleTypeDef *hdac,
                                        const DAC_ChannelConfTypeDef *sConfig, uint32_t Channel);


 



 
 
HAL_DAC_StateTypeDef HAL_DAC_GetState(const DAC_HandleTypeDef *hdac);
uint32_t HAL_DAC_GetError(const DAC_HandleTypeDef *hdac);



 



 



 
void DAC_DMAConvCpltCh1(DMA_HandleTypeDef *hdma);
void DAC_DMAErrorCh1(DMA_HandleTypeDef *hdma);
void DAC_DMAHalfConvCpltCh1(DMA_HandleTypeDef *hdma);


 



 




 





















 

 


 



 



 

 


 



 
typedef enum
{
  FLASH_PROC_NONE = 0U,
  FLASH_PROC_SECTERASE,
  FLASH_PROC_MASSERASE,
  FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;




 
typedef struct
{
  volatile FLASH_ProcedureTypeDef ProcedureOnGoing;    

  volatile uint32_t               NbSectorsToErase;    

  volatile uint8_t                VoltageForErase;     

  volatile uint32_t               Sector;              

  volatile uint32_t               Address;             

  HAL_LockTypeDef             Lock;                

  volatile uint32_t               ErrorCode;           

}FLASH_ProcessTypeDef;



 

 


 




 


 



 


 




 



 




 


 



 


 



 


 



 


 



 

 


 





 





 




 




 





 




 






 








 








 













 












 


 

 















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t TypeErase;   
 


  uint32_t Sector;      
 

  uint32_t NbSectors;   
 

  uint32_t VoltageRange;
 

} FLASH_EraseInitTypeDef;



 
typedef struct
{
  uint32_t OptionType;   
 

  uint32_t WRPState;     
 

  uint32_t WRPSector;    
 

  uint32_t RDPLevel;     
 

  uint32_t BORLevel;     
 

  uint32_t USERConfig;   

 

  uint32_t BootAddr0;    
 

  uint32_t BootAddr1;    
 

  uint32_t PCROPSector;  
 

  uint32_t PCROPRdp;    
 

} FLASH_OBProgramInitTypeDef;



 
 



 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 





 


 



 


 




 


 



 


 




 


 





 


 




 


 



 

 


 





 
 

 

 


 



 
 
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void              HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);



 



 
 
 
 
 


 



 


























 



 

 


 
void FLASH_Erase_Sector(uint32_t Sector, uint8_t VoltageRange);


 



 



 




 


 


 
 
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
 
void HAL_FLASH_IRQHandler(void);
 
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);


 



 
 
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
 
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);


 



 
 
uint32_t HAL_FLASH_GetError(void);
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);


 



 
 
 


 



 
 


 



 



 

 


 



 


 



 

 


 



 



 



 



 





















 

 


 



 



 

 


 




 
typedef struct
{
  uint32_t Timing;              

 

  uint32_t OwnAddress1;         
 

  uint32_t AddressingMode;      
 

  uint32_t DualAddressMode;     
 

  uint32_t OwnAddress2;         
 

  uint32_t OwnAddress2Masks;    

 

  uint32_t GeneralCallMode;     
 

  uint32_t NoStretchMode;       
 

} I2C_InitTypeDef;



 



























 
typedef enum
{
  HAL_I2C_STATE_RESET             = 0x00U,    
  HAL_I2C_STATE_READY             = 0x20U,    
  HAL_I2C_STATE_BUSY              = 0x24U,    
  HAL_I2C_STATE_BUSY_TX           = 0x21U,    
  HAL_I2C_STATE_BUSY_RX           = 0x22U,    
  HAL_I2C_STATE_LISTEN            = 0x28U,    
  HAL_I2C_STATE_BUSY_TX_LISTEN    = 0x29U,   
 
  HAL_I2C_STATE_BUSY_RX_LISTEN    = 0x2AU,   
 
  HAL_I2C_STATE_ABORT             = 0x60U,    

} HAL_I2C_StateTypeDef;



 


















 
typedef enum
{
  HAL_I2C_MODE_NONE               = 0x00U,    
  HAL_I2C_MODE_MASTER             = 0x10U,    
  HAL_I2C_MODE_SLAVE              = 0x20U,    
  HAL_I2C_MODE_MEM                = 0x40U     

} HAL_I2C_ModeTypeDef;



 




 


 




 
typedef struct __I2C_HandleTypeDef
{
  I2C_TypeDef                *Instance;       

  I2C_InitTypeDef            Init;            

  uint8_t                    *pBuffPtr;       

  uint16_t                   XferSize;        

  volatile uint16_t              XferCount;       

  volatile uint32_t              XferOptions;    
 

  volatile uint32_t              PreviousState;   

  HAL_StatusTypeDef(*XferISR)(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources);
   

  DMA_HandleTypeDef          *hdmatx;         

  DMA_HandleTypeDef          *hdmarx;         


  HAL_LockTypeDef            Lock;            

  volatile HAL_I2C_StateTypeDef  State;           

  volatile HAL_I2C_ModeTypeDef   Mode;            

  volatile uint32_t              ErrorCode;       

  volatile uint32_t              AddrEventCount;  

  volatile uint32_t              Devaddress;      

  volatile uint32_t              Memaddress;      

} I2C_HandleTypeDef;



 



 
 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 






 


 



 


 



 

 



 




 














 














 














 























 

















 




 




 




 


 

 
















 

 


 



 



 

 
 


 



 


 



 


 



 

 


 



 

 


 



 
 
HAL_StatusTypeDef HAL_I2CEx_ConfigAnalogFilter(I2C_HandleTypeDef *hi2c, uint32_t AnalogFilter);
HAL_StatusTypeDef HAL_I2CEx_ConfigDigitalFilter(I2C_HandleTypeDef *hi2c, uint32_t DigitalFilter);


 



 
void HAL_I2CEx_EnableFastModePlus(uint32_t ConfigFastModePlus);
void HAL_I2CEx_DisableFastModePlus(uint32_t ConfigFastModePlus);


 



 

 


 



 

 


 




 

 


 
 


 



 



 



 


 



 
 
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);

 


 



 
 
 
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                          uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                         uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                        uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                   uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials,
                                        uint32_t Timeout);

 
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                            uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                      uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                 uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                               uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress);

 
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                              uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                        uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                  uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                 uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);


 



 
 
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c);


 



 
 
HAL_I2C_StateTypeDef HAL_I2C_GetState(const I2C_HandleTypeDef *hi2c);
HAL_I2C_ModeTypeDef  HAL_I2C_GetMode(const I2C_HandleTypeDef *hi2c);
uint32_t             HAL_I2C_GetError(const I2C_HandleTypeDef *hi2c);



 



 

 


 



 

 


 


















 

 


 
 


 



 



 
























 

 


 



 



  

 



 
   


 
typedef struct
{
  uint32_t PVDLevel;   
 

  uint32_t Mode;      
 
}PWR_PVDTypeDef;



 

 


 



  



    
 


 


 



 


 
    


 


 



 


 



 


 



 


 



  
  
 


 










 



















 





 




 




 




 




 




 




 




 





 





 




 




 




 




 



 

 
















 

 


 



 



  

  
 


 


 



 
	


 


  
  


 


 
	


 


 



  
  
 


 

 


 










 











 


 










 










 


 
 


 
 


 
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);

void HAL_PWREx_EnableFlashPowerDown(void);
void HAL_PWREx_DisableFlashPowerDown(void); 
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void); 

void HAL_PWREx_EnableMainRegulatorLowVoltage(void);
void HAL_PWREx_DisableMainRegulatorLowVoltage(void);
void HAL_PWREx_EnableLowRegulatorLowVoltage(void);
void HAL_PWREx_DisableLowRegulatorLowVoltage(void);

HAL_StatusTypeDef HAL_PWREx_EnableOverDrive(void);
HAL_StatusTypeDef HAL_PWREx_DisableOverDrive(void);
HAL_StatusTypeDef HAL_PWREx_EnterUnderDriveSTOPMode(uint32_t Regulator, uint8_t STOPEntry);



 



 
 
 
 
 


 



 


 



 



  



 
  




 


 
  


 
 
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);


 



 
 
 
void HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

 
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinPolarity);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);

 
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

 
void HAL_PWR_PVD_IRQHandler(void);
void HAL_PWR_PVDCallback(void);

 
void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);


 



 

 
 
 


 



 


 



 
 


 



 



 



 



  



 
  





















 

 


 



 





 

 



 



 



 



 
typedef enum
{
  HAL_RNG_STATE_RESET     = 0x00U,   
  HAL_RNG_STATE_READY     = 0x01U,   
  HAL_RNG_STATE_BUSY      = 0x02U,   
  HAL_RNG_STATE_TIMEOUT   = 0x03U,   
  HAL_RNG_STATE_ERROR     = 0x04U    

} HAL_RNG_StateTypeDef;



 



 
typedef struct
{
  RNG_TypeDef                 *Instance;     

  HAL_LockTypeDef             Lock;          

  volatile HAL_RNG_StateTypeDef   State;         

  volatile  uint32_t              ErrorCode;     

  uint32_t                    RandomNumber;  


} RNG_HandleTypeDef;




 



 

 


 



 


 



 


 



 


 



 

 


 




 





 





 










 








 





 





 










 










 



 

 


 



 
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng);

 



 



 
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef
                                 *hrng);     
uint32_t HAL_RNG_GetRandomNumber_IT(RNG_HandleTypeDef
                                    *hrng);  
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef *hrng, uint32_t *random32bit);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_IT(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_ReadLastRandomNumber(const RNG_HandleTypeDef *hrng);

void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng);
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng);
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit);



 



 
HAL_RNG_StateTypeDef HAL_RNG_GetState(const RNG_HandleTypeDef *hrng);
uint32_t             HAL_RNG_GetError(const RNG_HandleTypeDef *hrng);


 



 

 


 




 



 




 


























 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t Mode;                
 

  uint32_t Direction;           
 

  uint32_t DataSize;            
 

  uint32_t CLKPolarity;         
 

  uint32_t CLKPhase;            
 

  uint32_t NSS;                 

 

  uint32_t BaudRatePrescaler;   



 

  uint32_t FirstBit;            
 

  uint32_t TIMode;              
 

  uint32_t CRCCalculation;      
 

  uint32_t CRCPolynomial;       
 

  uint32_t CRCLength;           

 

  uint32_t NSSPMode;            




 
} SPI_InitTypeDef;



 
typedef enum
{
  HAL_SPI_STATE_RESET      = 0x00U,     
  HAL_SPI_STATE_READY      = 0x01U,     
  HAL_SPI_STATE_BUSY       = 0x02U,     
  HAL_SPI_STATE_BUSY_TX    = 0x03U,     
  HAL_SPI_STATE_BUSY_RX    = 0x04U,     
  HAL_SPI_STATE_BUSY_TX_RX = 0x05U,     
  HAL_SPI_STATE_ERROR      = 0x06U,     
  HAL_SPI_STATE_ABORT      = 0x07U      
} HAL_SPI_StateTypeDef;



 
typedef struct __SPI_HandleTypeDef
{
  SPI_TypeDef                *Instance;       

  SPI_InitTypeDef            Init;            

  const uint8_t              *pTxBuffPtr;     

  uint16_t                   TxXferSize;      

  volatile uint16_t              TxXferCount;     

  uint8_t                    *pRxBuffPtr;     

  uint16_t                   RxXferSize;      

  volatile uint16_t              RxXferCount;     

  uint32_t                   CRCSize;         

  void (*RxISR)(struct __SPI_HandleTypeDef *hspi);    

  void (*TxISR)(struct __SPI_HandleTypeDef *hspi);    

  DMA_HandleTypeDef          *hdmatx;         

  DMA_HandleTypeDef          *hdmarx;         

  HAL_LockTypeDef            Lock;            

  volatile HAL_SPI_StateTypeDef  State;           

  volatile uint32_t              ErrorCode;       

} SPI_HandleTypeDef;



 

 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 







 


 








 


 



 


 



 


 



 



 



 


 



 

 


 





 










 










 










 
















 





 





 





 





 





 





 



 

 


 





 





 





 















 









 





 





 




 




 





 





 





 





 





 





 





 





 





 





 





 




 



 

 
















 

 


 



 



 

 
 
 
 


 

 
 


 
HAL_StatusTypeDef HAL_SPIEx_FlushRxFifo(const SPI_HandleTypeDef *hspi);


 



 



 



 




 


 



 
 
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi);

 


 



 
 
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                          uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t Size);
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi);
 
HAL_StatusTypeDef HAL_SPI_Abort(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Abort_IT(SPI_HandleTypeDef *hspi);

void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);


 



 
 
HAL_SPI_StateTypeDef HAL_SPI_GetState(const SPI_HandleTypeDef *hspi);
uint32_t             HAL_SPI_GetError(const SPI_HandleTypeDef *hspi);


 



 



 



 




















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t Prescaler;         
 

  uint32_t CounterMode;       
 

  uint32_t Period;            

 

  uint32_t ClockDivision;     
 

  uint32_t RepetitionCounter;  








 

  uint32_t AutoReloadPreload;  
 
} TIM_Base_InitTypeDef;



 
typedef struct
{
  uint32_t OCMode;        
 

  uint32_t Pulse;         
 

  uint32_t OCPolarity;    
 

  uint32_t OCNPolarity;   

 

  uint32_t OCFastMode;    

 


  uint32_t OCIdleState;   

 

  uint32_t OCNIdleState;  

 
} TIM_OC_InitTypeDef;



 
typedef struct
{
  uint32_t OCMode;        
 

  uint32_t Pulse;         
 

  uint32_t OCPolarity;    
 

  uint32_t OCNPolarity;   

 

  uint32_t OCIdleState;   

 

  uint32_t OCNIdleState;  

 

  uint32_t ICPolarity;    
 

  uint32_t ICSelection;   
 

  uint32_t ICFilter;      
 
} TIM_OnePulse_InitTypeDef;



 
typedef struct
{
  uint32_t  ICPolarity;  
 

  uint32_t ICSelection;  
 

  uint32_t ICPrescaler;  
 

  uint32_t ICFilter;     
 
} TIM_IC_InitTypeDef;



 
typedef struct
{
  uint32_t EncoderMode;   
 

  uint32_t IC1Polarity;   
 

  uint32_t IC1Selection;  
 

  uint32_t IC1Prescaler;  
 

  uint32_t IC1Filter;     
 

  uint32_t IC2Polarity;   
 

  uint32_t IC2Selection;  
 

  uint32_t IC2Prescaler;  
 

  uint32_t IC2Filter;     
 
} TIM_Encoder_InitTypeDef;



 
typedef struct
{
  uint32_t ClockSource;     
 
  uint32_t ClockPolarity;   
 
  uint32_t ClockPrescaler;  
 
  uint32_t ClockFilter;     
 
} TIM_ClockConfigTypeDef;



 
typedef struct
{
  uint32_t ClearInputState;      
 
  uint32_t ClearInputSource;     
 
  uint32_t ClearInputPolarity;   
 
  uint32_t ClearInputPrescaler;  

 
  uint32_t ClearInputFilter;     
 
} TIM_ClearInputConfigTypeDef;





 
typedef struct
{
  uint32_t  MasterOutputTrigger;   
 
  uint32_t  MasterOutputTrigger2;  
 
  uint32_t  MasterSlaveMode;       





 
} TIM_MasterConfigTypeDef;



 
typedef struct
{
  uint32_t  SlaveMode;         
 
  uint32_t  InputTrigger;      
 
  uint32_t  TriggerPolarity;   
 
  uint32_t  TriggerPrescaler;  
 
  uint32_t  TriggerFilter;     
 

} TIM_SlaveConfigTypeDef;





 
typedef struct
{
  uint32_t OffStateRunMode;       

  uint32_t OffStateIDLEMode;      

  uint32_t LockLevel;             

  uint32_t DeadTime;              

  uint32_t BreakState;            

  uint32_t BreakPolarity;         

  uint32_t BreakFilter;           

  uint32_t Break2State;           

  uint32_t Break2Polarity;        

  uint32_t Break2Filter;          


  uint32_t AutomaticOutput;       

} TIM_BreakDeadTimeConfigTypeDef;



 
typedef enum
{
  HAL_TIM_STATE_RESET             = 0x00U,     
  HAL_TIM_STATE_READY             = 0x01U,     
  HAL_TIM_STATE_BUSY              = 0x02U,     
  HAL_TIM_STATE_TIMEOUT           = 0x03U,     
  HAL_TIM_STATE_ERROR             = 0x04U      
} HAL_TIM_StateTypeDef;



 
typedef enum
{
  HAL_TIM_CHANNEL_STATE_RESET             = 0x00U,     
  HAL_TIM_CHANNEL_STATE_READY             = 0x01U,     
  HAL_TIM_CHANNEL_STATE_BUSY              = 0x02U,     
} HAL_TIM_ChannelStateTypeDef;



 
typedef enum
{
  HAL_DMA_BURST_STATE_RESET             = 0x00U,     
  HAL_DMA_BURST_STATE_READY             = 0x01U,     
  HAL_DMA_BURST_STATE_BUSY              = 0x02U,     
} HAL_TIM_DMABurstStateTypeDef;



 
typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1        = 0x01U,     
  HAL_TIM_ACTIVE_CHANNEL_2        = 0x02U,     
  HAL_TIM_ACTIVE_CHANNEL_3        = 0x04U,     
  HAL_TIM_ACTIVE_CHANNEL_4        = 0x08U,     
  HAL_TIM_ACTIVE_CHANNEL_5        = 0x10U,     
  HAL_TIM_ACTIVE_CHANNEL_6        = 0x20U,     
  HAL_TIM_ACTIVE_CHANNEL_CLEARED  = 0x00U      
} HAL_TIM_ActiveChannel;



 
typedef struct
{
  TIM_TypeDef                        *Instance;          
  TIM_Base_InitTypeDef               Init;               
  HAL_TIM_ActiveChannel              Channel;            
  DMA_HandleTypeDef                  *hdma[7];          
 
  HAL_LockTypeDef                    Lock;               
  volatile HAL_TIM_StateTypeDef          State;              
  volatile HAL_TIM_ChannelStateTypeDef   ChannelState[6];    
  volatile HAL_TIM_ChannelStateTypeDef   ChannelNState[4];   
  volatile HAL_TIM_DMABurstStateTypeDef  DMABurstState;      

} TIM_HandleTypeDef;




 
 

 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 


 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 
 

 


 




 





 





 





 







 






 














 














 













 













 






















 






















 















 














 








 






 






 







 






 









 





 






 





 










 








 


















 















 














 













 













 













 

















 

















 








 











 















 








 



 
 

 


 

 


 
 

 


 







































































 
 

 
















 

 


 



 



 

 


 



 

typedef struct
{
  uint32_t IC1Polarity;         
 

  uint32_t IC1Prescaler;        
 

  uint32_t IC1Filter;           
 

  uint32_t Commutation_Delay;   
 
} TIM_HallSensor_InitTypeDef;


 
 

 


 



 


 



 
 

 


 



 
 

 


 



 
 

 


 




 
 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, const TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);

 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);


 




 
 
 
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

 
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

 
HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                          uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
 
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

 
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                           uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
 
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

 
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);


 




 
 
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                              uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                                 uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t  InputTrigger,
                                                  uint32_t  CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
                                                        const TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
                                                const TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef HAL_TIMEx_GroupChannel5(TIM_HandleTypeDef *htim, uint32_t Channels);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);


 




 
 
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_Break2Callback(TIM_HandleTypeDef *htim);


 




 
 
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(const TIM_HandleTypeDef *htim,  uint32_t ChannelN);


 



 
 

 


 
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);


 
 



 



 




 


 




 
 
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim, const uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);


 




 
 
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                       uint16_t Length);
HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, const uint32_t *pData,
                                        uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim, uint32_t OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
 
HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);


 




 
 
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef *htim, const TIM_Encoder_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim);
 
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
 
HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);


 




 
 
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim, const TIM_OC_InitTypeDef *sConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, const TIM_OC_InitTypeDef *sConfig,
                                            uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, const TIM_IC_InitTypeDef *sConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
                                                 uint32_t OutputChannel,  uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim,
                                           const TIM_ClearInputConfigTypeDef *sClearInputConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, const TIM_ClockConfigTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(TIM_HandleTypeDef *htim, const TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                              uint32_t BurstRequestSrc, const uint32_t  *BurstBuffer,
                                              uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc, const uint32_t *BurstBuffer,
                                                   uint32_t BurstLength,  uint32_t DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                             uint32_t BurstRequestSrc, uint32_t  *BurstBuffer, uint32_t  BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                  uint32_t BurstRequestSrc, uint32_t  *BurstBuffer,
                                                  uint32_t  BurstLength, uint32_t  DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim, uint32_t EventSource);
uint32_t HAL_TIM_ReadCapturedValue(const TIM_HandleTypeDef *htim, uint32_t Channel);


 




 
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);

 



 




 
 
HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(const TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(const TIM_HandleTypeDef *htim);

 
HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(const TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(const TIM_HandleTypeDef *htim,  uint32_t Channel);
HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(const TIM_HandleTypeDef *htim);


 



 
 

 


 
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, const TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, const TIM_OC_InitTypeDef *OC_Config);
void TIM_ETR_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ExtTRGPrescaler,
                       uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

void TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_DMAError(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_CCxChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelState);




 
 



 



 















 


 

 



 



  

 
 



 



 
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;


 



 


 



 
   
 


 
  

 




 
                                       


 






 




 
  


 


 
 


 


 
 
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);


 
 
  


 
extern volatile uint32_t uwTick;
extern uint32_t uwTickPrio;
extern HAL_TickFreqTypeDef uwTickFreq;


 
 


  
 
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_DBGMCU_EnableDBGStandbyMode(void);
void HAL_DBGMCU_DisableDBGStandbyMode(void);
void HAL_EnableCompensationCell(void);
void HAL_DisableCompensationCell(void);
void HAL_EnableFMCMemorySwapping(void);
void HAL_DisableFMCMemorySwapping(void);


 



   
 
 


 


 
 


 


 
 
 


  



  
  



















 

 


 



 




 

 
 

 


 

 
 
 
 

 
 


 
 

 
 
 
 
 

 
 

 
 


 
 
 
 

 
 
 

 
 
 

 



 
 
 
 

 
 
 

 
 
 

 

 
 
 
 
 
 
 
 

 

 
 


 
 

 
 

 
 
 
 
 
 

 



 

 
 
 
 


 


 


 








 








 



 


 


 










 
typedef struct
{
  uint32_t CommonClock;                 


 

  uint32_t Multimode;                   


 

  uint32_t MultiDMATransfer;            


 

  uint32_t MultiTwoSamplingDelay;       


 

} LL_ADC_CommonInitTypeDef;




















 
typedef struct
{
  uint32_t Resolution;                  


 

  uint32_t DataAlignment;               


 

  uint32_t SequencersScanMode;          


 

} LL_ADC_InitTypeDef;



















 
typedef struct
{
  uint32_t TriggerSource;               




 

  uint32_t SequencerLength;             



 

  uint32_t SequencerDiscont;            




 

  uint32_t ContinuousMode;              



 

  uint32_t DMATransfer;                 


 

} LL_ADC_REG_InitTypeDef;



















 
typedef struct
{
  uint32_t TriggerSource;               




 

  uint32_t SequencerLength;             



 

  uint32_t SequencerDiscont;            




 

  uint32_t TrigAuto;                    



 

} LL_ADC_INJ_InitTypeDef;



 

 


 




 


 




 


 



 
 
 
 


 



 


 



 
 
 
 
 
 


 



 


 



 


 



 


 



 


 



 



 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 









 
  
 
 
 
 
 
 
 
 
 
 
 
 

 
 
 
 

 
 
 
 



 



 


 


 



 







 






 


 



 





































 





































 














































 




























































 
























 




















































































































 



















 



















 













 










 

















 













 


















 
















 

























 













































 












































 



 



 


 


 



 
 
 
 






























 
static inline uint32_t LL_ADC_DMA_GetRegAddr(ADC_TypeDef *ADCx, uint32_t Register)
{
  uint32_t data_reg_addr = 0U;
  
  if (Register == 0x00000000U)
  {
     
    data_reg_addr = (uint32_t)&(ADCx->DR);
  }
  else  
  {
     
    data_reg_addr = (uint32_t)&(((((ADC_Common_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x2300UL))))->CDR);
  }
  
  return data_reg_addr;
}



 



 












 
static inline void LL_ADC_SetCommonClock(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t CommonClock)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x3UL << (16U))))) | (CommonClock))));
}











 
static inline uint32_t LL_ADC_GetCommonClock(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x3UL << (16U)))));
}




























 
static inline void LL_ADC_SetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x1UL << (23U)) | (0x1UL << (22U))))) | (PathInternal))));
}
















 
static inline uint32_t LL_ADC_GetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x1UL << (23U)) | (0x1UL << (22U)))));
}



 



 













 
static inline void LL_ADC_SetResolution(ADC_TypeDef *ADCx, uint32_t Resolution)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x3UL << (24U))))) | (Resolution))));
}












 
static inline uint32_t LL_ADC_GetResolution(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x3UL << (24U)))));
}











 
static inline void LL_ADC_SetDataAlignment(ADC_TypeDef *ADCx, uint32_t DataAlignment)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (11U))))) | (DataAlignment))));
}










 
static inline uint32_t LL_ADC_GetDataAlignment(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (11U)))));
}





















 
static inline void LL_ADC_SetSequencersScanMode(ADC_TypeDef *ADCx, uint32_t ScanMode)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (8U))))) | (ScanMode))));
}




















 
static inline uint32_t LL_ADC_GetSequencersScanMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (8U)))));
}



 



 






























 
static inline void LL_ADC_REG_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
 
 
 
 
 
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0xFUL << (24U))))) | ((TriggerSource & (0xFUL << (24U)))))));
}

































 
static inline uint32_t LL_ADC_REG_GetTriggerSource(ADC_TypeDef *ADCx)
{
  uint32_t TriggerSource = ((ADCx->CR2) & ((0xFUL << (24U)) | (0x3UL << (28U))));
  
   
   
  uint32_t ShiftExten = ((TriggerSource & (0x3UL << (28U))) >> ((28U) - 2U));
  
   
   
  return ((TriggerSource
           & ((((0x00000000U & (0xFUL << (24U))) >> (4U * 0U)) | (((0xFUL << (24U))) >> (4U * 1U)) | (((0xFUL << (24U))) >> (4U * 2U)) | (((0xFUL << (24U))) >> (4U * 3U))) << ShiftExten) & (0xFUL << (24U)))
          | (((((0x00000000U & (0x3UL << (28U))) >> (4U * 0U)) | ((((0x1UL << (28U)))) >> (4U * 1U)) | ((((0x1UL << (28U)))) >> (4U * 2U)) | ((((0x1UL << (28U)))) >> (4U * 3U))) << ShiftExten) & (0x3UL << (28U)))
         );
}











 
static inline uint32_t LL_ADC_REG_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR2) & ((0x3UL << (28U)))) == (0x00000000U & (0x3UL << (28U))));
}












 
static inline uint32_t LL_ADC_REG_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x3UL << (28U)))));
}

























































 
static inline void LL_ADC_REG_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  (((ADCx->SQR1)) = ((((((ADCx->SQR1))) & (~((0xFUL << (20U))))) | (SequencerNbRanks))));
}























































 
static inline uint32_t LL_ADC_REG_GetSequencerLength(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->SQR1) & ((0xFUL << (20U)))));
}























 
static inline void LL_ADC_REG_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (11U)) | (0x7UL << (13U))))) | (SeqDiscont))));
}


















 
static inline uint32_t LL_ADC_REG_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (11U)) | (0x7UL << (13U)))));
}














































































 
static inline void LL_ADC_REG_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{
   
   
   
   
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SQR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  (((*preg)) = ((((((*preg))) & (~(((0x1FUL << (0U))) << (Rank & (0x0000001FU))))) | ((Channel & ((0x1FUL << (0U)))) << (Rank & (0x0000001FU))))));
}




















































































 
static inline uint32_t LL_ADC_REG_GetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SQR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  return (uint32_t) (((*preg) & (((0x1FUL << (0U))) << (Rank & (0x0000001FU))))
                     >> (Rank & (0x0000001FU))
                    );
}















 
static inline void LL_ADC_REG_SetContinuousMode(ADC_TypeDef *ADCx, uint32_t Continuous)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (1U))))) | (Continuous))));
}












 
static inline uint32_t LL_ADC_REG_GetContinuousMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (1U)))));
}































 
static inline void LL_ADC_REG_SetDMATransfer(ADC_TypeDef *ADCx, uint32_t DMATransfer)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (8U)) | (0x1UL << (9U))))) | (DMATransfer))));
}






























 
static inline uint32_t LL_ADC_REG_GetDMATransfer(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (8U)) | (0x1UL << (9U)))));
}

















 
static inline void LL_ADC_REG_SetFlagEndOfConversion(ADC_TypeDef *ADCx, uint32_t EocSelection)
{
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0x1UL << (10U))))) | (EocSelection))));
}










 
static inline uint32_t LL_ADC_REG_GetFlagEndOfConversion(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x1UL << (10U)))));
}



 



 





























 
static inline void LL_ADC_INJ_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
 
 
 
 
 
  (((ADCx->CR2)) = ((((((ADCx->CR2))) & (~((0xFUL << (16U))))) | ((TriggerSource & (0xFUL << (16U)))))));
}
































 
static inline uint32_t LL_ADC_INJ_GetTriggerSource(ADC_TypeDef *ADCx)
{
  uint32_t TriggerSource = ((ADCx->CR2) & ((0xFUL << (16U)) | (0x3UL << (20U))));
  
   
   
  uint32_t ShiftExten = ((TriggerSource & (0x3UL << (20U))) >> ((20U) - 2U));
  
   
   
  return ((TriggerSource
           & ((((0x00000000U & (0xFUL << (16U))) >> (4U * 0U)) | (((0xFUL << (16U))) >> (4U * 1U)) | (((0xFUL << (16U))) >> (4U * 2U)) | (((0xFUL << (16U))) >> (4U * 3U))) << ShiftExten) & (0xFUL << (16U)))
          | (((((0x00000000U & (0x3UL << (20U))) >> (4U * 0U)) | ((((0x1UL << (20U)))) >> (4U * 1U)) | ((((0x1UL << (20U)))) >> (4U * 2U)) | ((((0x1UL << (20U)))) >> (4U * 3U))) << ShiftExten) & (0x3UL << (20U)))
         );
}











 
static inline uint32_t LL_ADC_INJ_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR2) & ((0x3UL << (20U)))) == (0x00000000U & (0x3UL << (20U))));
}










 
static inline uint32_t LL_ADC_INJ_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR2) & ((0x3UL << (20U)))));
}























 
static inline void LL_ADC_INJ_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((0x3UL << (20U))))) | (SequencerNbRanks))));
}






















 
static inline uint32_t LL_ADC_INJ_GetSequencerLength(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->JSQR) & ((0x3UL << (20U)))));
}













 
static inline void LL_ADC_INJ_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (12U))))) | (SeqDiscont))));
}










 
static inline uint32_t LL_ADC_INJ_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (12U)))));
}















































 
static inline void LL_ADC_INJ_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{
   
   
   
   
  uint32_t tmpreg1 = (((ADCx->JSQR) & ((0x3UL << (20U)))) >> (20U)) + 1U;
  
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~(((0x1FUL << (0U))) << (5U * (uint8_t)(((Rank) + 3U) - (tmpreg1)))))) | ((Channel & ((0x1FUL << (0U)))) << (5U * (uint8_t)(((Rank) + 3U) - (tmpreg1)))))));
}
























































 
static inline uint32_t LL_ADC_INJ_GetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank)
{
  uint32_t tmpreg1 = (((ADCx->JSQR) & ((0x3UL << (20U)))) >> (20U))  + 1U;
  
  return (uint32_t)(((ADCx->JSQR) & (((0x1FUL << (0U))) << (5U * (uint8_t)(((Rank) + 3U) - (tmpreg1)))))
                    >> (5U * (uint8_t)(((Rank) + 3U) - (tmpreg1)))
                   );
}


























 
static inline void LL_ADC_INJ_SetTrigAuto(ADC_TypeDef *ADCx, uint32_t TrigAuto)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~((0x1UL << (10U))))) | (TrigAuto))));
}









 
static inline uint32_t LL_ADC_INJ_GetTrigAuto(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & ((0x1UL << (10U)))));
}

























 
static inline void LL_ADC_INJ_SetOffset(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t OffsetLevel)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JOFR1)) + (((((Rank) & ((0x00000000U | 0x00001000U | 0x00002000U | 0x00003000U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00001000U | 0x00002000U | 0x00003000U))))))) << 2U))));
  
  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (0U))))) | (OffsetLevel))));
}


















 
static inline uint32_t LL_ADC_INJ_GetOffset(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JOFR1)) + (((((Rank) & ((0x00000000U | 0x00001000U | 0x00002000U | 0x00003000U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00001000U | 0x00002000U | 0x00003000U))))))) << 2U))));
  
  return (uint32_t)(((*preg) & ((0xFFFUL << (0U))))
                   );
}



 



 












































































 
static inline void LL_ADC_SetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t SamplingTime)
{
   
   
   
   
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SMPR1)) + (((((Channel) & ((0x00000000U | 0x02000000U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x02000000U))))))) << 2U))));
  
  (((*preg)) = ((((((*preg))) & (~((0x7UL << (0U)) << (((Channel) & (0x01F00000U)) >> (__iar_builtin_CLZ(__iar_builtin_RBIT((0x01F00000U)))))))) | (SamplingTime << (((Channel) & (0x01F00000U)) >> (__iar_builtin_CLZ(__iar_builtin_RBIT((0x01F00000U)))))))));
}
































































 
static inline uint32_t LL_ADC_GetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SMPR1)) + (((((Channel) & ((0x00000000U | 0x02000000U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x02000000U))))))) << 2U))));
  
  return (uint32_t)(((*preg) & ((0x7UL << (0U)) << (((Channel) & (0x01F00000U)) >> (__iar_builtin_CLZ(__iar_builtin_RBIT((0x01F00000U)))))))
                    >> (((Channel) & (0x01F00000U)) >> (__iar_builtin_CLZ(__iar_builtin_RBIT((0x01F00000U)))))
                   );
}



 



 
































































































 
static inline void LL_ADC_SetAnalogWDMonitChannels(ADC_TypeDef *ADCx, uint32_t AWDChannelGroup)
{
  (((ADCx->CR1)) = ((((((ADCx->CR1))) & (~(((0x1UL << (23U)) | (0x1UL << (22U)) | (0x1UL << (9U)) | (0x1FUL << (0U)))))) | (AWDChannelGroup))));
}

























































































 
static inline uint32_t LL_ADC_GetAnalogWDMonitChannels(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CR1) & (((0x1UL << (23U)) | (0x1UL << (22U)) | (0x1UL << (9U)) | (0x1FUL << (0U))))));
}






















 
static inline void LL_ADC_SetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdsHighLow, uint32_t AWDThresholdValue)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->HTR)) + ((AWDThresholdsHighLow) << 2U))));
  
  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (0U))))) | (AWDThresholdValue))));
}














 
static inline uint32_t LL_ADC_GetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDThresholdsHighLow)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->HTR)) + ((AWDThresholdsHighLow) << 2U))));
  
  return (uint32_t)(((*preg) & ((0xFFFUL << (0U)))));
}



 



 


























 
static inline void LL_ADC_SetMultimode(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t Multimode)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x1FUL << (0U))))) | (Multimode))));
}

























 
static inline uint32_t LL_ADC_GetMultimode(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x1FUL << (0U)))));
}












































 
static inline void LL_ADC_SetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiDMATransfer)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x3UL << (14U)) | (0x1UL << (13U))))) | (MultiDMATransfer))));
}











































 
static inline uint32_t LL_ADC_GetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x3UL << (14U)) | (0x1UL << (13U)))));
}





























 
static inline void LL_ADC_SetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiTwoSamplingDelay)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0xFUL << (8U))))) | (MultiTwoSamplingDelay))));
}























 
static inline uint32_t LL_ADC_GetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0xFUL << (8U)))));
}



 


 










 
static inline void LL_ADC_Enable(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) |= ((0x1UL << (0U))));
}






 
static inline void LL_ADC_Disable(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) &= ~((0x1UL << (0U))));
}






 
static inline uint32_t LL_ADC_IsEnabled(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR2) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}



 



 
















 
static inline void LL_ADC_REG_StartConversionSWStart(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) |= ((0x1UL << (30U))));
}
















 
static inline void LL_ADC_REG_StartConversionExtTrig(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  ((ADCx->CR2) |= (ExternalTriggerEdge));
}













 
static inline void LL_ADC_REG_StopConversionExtTrig(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) &= ~((0x3UL << (28U))));
}









 
static inline uint32_t LL_ADC_REG_ReadConversionData32(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}










 
static inline uint16_t LL_ADC_REG_ReadConversionData12(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}










 
static inline uint16_t LL_ADC_REG_ReadConversionData10(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}










 
static inline uint8_t LL_ADC_REG_ReadConversionData8(ADC_TypeDef *ADCx)
{
  return (uint8_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))) & 0x000000FFUL);
}










 
static inline uint8_t LL_ADC_REG_ReadConversionData6(ADC_TypeDef *ADCx)
{
  return (uint8_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))) & 0x000000FFUL);
}





















 
static inline uint32_t LL_ADC_REG_ReadMultiConversionData32(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t ConversionData)
{
  return (uint32_t)(((ADCxy_COMMON->CDR) & ((0xFFFFUL << (16U))))
                    >> (__iar_builtin_CLZ(__iar_builtin_RBIT(ConversionData)))
                   );
}



 



 
















 
static inline void LL_ADC_INJ_StartConversionSWStart(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) |= ((0x1UL << (22U))));
}
















 
static inline void LL_ADC_INJ_StartConversionExtTrig(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  ((ADCx->CR2) |= (ExternalTriggerEdge));
}













 
static inline void LL_ADC_INJ_StopConversionExtTrig(ADC_TypeDef *ADCx)
{
  ((ADCx->CR2) &= ~((0x3UL << (20U))));
}

















 
static inline uint32_t LL_ADC_INJ_ReadConversionData32(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  return (uint32_t)(((*preg) & (((uint16_t)0xFFFFU)))
                   );
}


















 
static inline uint16_t LL_ADC_INJ_ReadConversionData12(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  return (uint16_t)(((*preg) & (((uint16_t)0xFFFFU)))
                   );
}


















 
static inline uint16_t LL_ADC_INJ_ReadConversionData10(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  return (uint16_t)(((*preg) & (((uint16_t)0xFFFFU)))
                   );
}


















 
static inline uint8_t LL_ADC_INJ_ReadConversionData8(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  return (uint8_t)(((*preg) & (((uint16_t)0xFFFFU))) & 0x000000FFUL
                  );
}


















 
static inline uint8_t LL_ADC_INJ_ReadConversionData6(ADC_TypeDef *ADCx, uint32_t Rank)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + (((((Rank) & ((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(((0x00000000U | 0x00000100U | 0x00000200U | 0x00000300U))))))) << 2U))));
  
  return (uint8_t)(((*preg) & (((uint16_t)0xFFFFU))) & 0x000000FFUL
                  );
}



 



 










 
static inline uint32_t LL_ADC_IsActiveFlag_EOCS(ADC_TypeDef *ADCx)
{
  return (((ADCx->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}






 
static inline uint32_t LL_ADC_IsActiveFlag_OVR(ADC_TypeDef *ADCx)
{
  return (((ADCx->SR) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_JEOS(ADC_TypeDef *ADCx)
{
   
   
   
   
  return (((ADCx->SR) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}






 
static inline uint32_t LL_ADC_IsActiveFlag_AWD1(ADC_TypeDef *ADCx)
{
  return (((ADCx->SR) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}










 
static inline void LL_ADC_ClearFlag_EOCS(ADC_TypeDef *ADCx)
{
  ((ADCx->SR) = (~(0x1UL << (1U))));
}






 
static inline void LL_ADC_ClearFlag_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->SR) = (~(0x1UL << (5U))));
}







 
static inline void LL_ADC_ClearFlag_JEOS(ADC_TypeDef *ADCx)
{
   
   
   
   
  ((ADCx->SR) = (~(0x1UL << (2U))));
}






 
static inline void LL_ADC_ClearFlag_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->SR) = (~(0x1UL << (0U))));
}











 
static inline uint32_t LL_ADC_IsActiveFlag_MST_EOCS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((((ADC_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x2000UL))->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}











 
static inline uint32_t LL_ADC_IsActiveFlag_SLV1_EOCS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (9U)))) == ((0x1UL << (9U))));
}











 
static inline uint32_t LL_ADC_IsActiveFlag_SLV2_EOCS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (17U)))) == ((0x1UL << (17U))));
}






 
static inline uint32_t LL_ADC_IsActiveFlag_MST_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_SLV1_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (13U)))) == ((0x1UL << (13U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_SLV2_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (21U)))) == ((0x1UL << (21U))));
}








 
static inline uint32_t LL_ADC_IsActiveFlag_MST_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
   
   
   
   
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_SLV1_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
   
   
   
   
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (10U)))) == ((0x1UL << (10U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_SLV2_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
   
   
   
   
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (18U)))) == ((0x1UL << (18U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_MST_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_SLV1_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (((ADCxy_COMMON->CSR) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}







 
static inline uint32_t LL_ADC_IsActiveFlag_SLV2_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
    return (((ADCxy_COMMON->CSR) & ((0x1UL << (16U)))) == ((0x1UL << (16U))));
}




 



 










 
static inline void LL_ADC_EnableIT_EOCS(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) |= ((0x1UL << (5U))));
}






 
static inline void LL_ADC_EnableIT_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) |= ((0x1UL << (26U))));
}







 
static inline void LL_ADC_EnableIT_JEOS(ADC_TypeDef *ADCx)
{
   
   
   
   
  ((ADCx->CR1) |= ((0x1UL << (7U))));
}






 
static inline void LL_ADC_EnableIT_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) |= ((0x1UL << (6U))));
}










 
static inline void LL_ADC_DisableIT_EOCS(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) &= ~((0x1UL << (5U))));
}






 
static inline void LL_ADC_DisableIT_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) &= ~((0x1UL << (26U))));
}







 
static inline void LL_ADC_DisableIT_JEOS(ADC_TypeDef *ADCx)
{
   
   
   
   
  ((ADCx->CR1) &= ~((0x1UL << (7U))));
}






 
static inline void LL_ADC_DisableIT_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->CR1) &= ~((0x1UL << (6U))));
}











 
static inline uint32_t LL_ADC_IsEnabledIT_EOCS(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR1) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}







 
static inline uint32_t LL_ADC_IsEnabledIT_OVR(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR1) & ((0x1UL << (26U)))) == ((0x1UL << (26U))));
}








 
static inline uint32_t LL_ADC_IsEnabledIT_JEOS(ADC_TypeDef *ADCx)
{
   
   
   
   
  return (((ADCx->CR1) & ((0x1UL << (7U)))) == ((0x1UL << (7U))));
}







 
static inline uint32_t LL_ADC_IsEnabledIT_AWD1(ADC_TypeDef *ADCx)
{
  return (((ADCx->CR1) & ((0x1UL << (6U)))) == ((0x1UL << (6U))));
}



 



 

 
ErrorStatus LL_ADC_CommonDeInit(ADC_Common_TypeDef *ADCxy_COMMON);
ErrorStatus LL_ADC_CommonInit(ADC_Common_TypeDef *ADCxy_COMMON, LL_ADC_CommonInitTypeDef *ADC_CommonInitStruct);
void        LL_ADC_CommonStructInit(LL_ADC_CommonInitTypeDef *ADC_CommonInitStruct);

 
 
ErrorStatus LL_ADC_DeInit(ADC_TypeDef *ADCx);

 
ErrorStatus LL_ADC_Init(ADC_TypeDef *ADCx, LL_ADC_InitTypeDef *ADC_InitStruct);
void        LL_ADC_StructInit(LL_ADC_InitTypeDef *ADC_InitStruct);

 
ErrorStatus LL_ADC_REG_Init(ADC_TypeDef *ADCx, LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct);
void        LL_ADC_REG_StructInit(LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct);

 
ErrorStatus LL_ADC_INJ_Init(ADC_TypeDef *ADCx, LL_ADC_INJ_InitTypeDef *ADC_INJ_InitStruct);
void        LL_ADC_INJ_StructInit(LL_ADC_INJ_InitTypeDef *ADC_INJ_InitStruct);



 



 



 




 



















 

 


 



 




 

 
 

 


 

 
 
 
 
 
 











 

 



 


 


 








 



 


 


 



 
typedef struct
{
  uint32_t TriggerSource;               





 

  uint32_t WaveAutoGeneration;          



 

  uint32_t WaveAutoGenerationConfig;    










 

  uint32_t OutputBuffer;                



 
} LL_DAC_InitTypeDef;



 

 


 




 
 

 



 




 




 



 




 



 


 



 


 



 


 



 


 



 


 



 


 



 
 
 
 


 







 

 
 
 
 
 
 
 
 
 
 
 
 

 
 
 
 
 
 
 
 
 
 
 



 



 

 


 



 







 






 



 



 













 













 











 


















 



 



 


 


 

























 
static inline void LL_DAC_SetTriggerSource(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t TriggerSource)
{
  (((DACx->CR)) = ((((((DACx->CR))) & (~((0x7UL << (3U)) << (DAC_Channel & (0UL | 16UL))))) | (TriggerSource << (DAC_Channel & (0UL | 16UL))))));
}






















 
static inline uint32_t LL_DAC_GetTriggerSource(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return (uint32_t)(((DACx->CR) & ((0x7UL << (3U)) << (DAC_Channel & (0UL | 16UL))))
                    >> (DAC_Channel & (0UL | 16UL))
                   );
}















 
static inline void LL_DAC_SetWaveAutoGeneration(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t WaveAutoGeneration)
{
  (((DACx->CR)) = ((((((DACx->CR))) & (~((0x3UL << (6U)) << (DAC_Channel & (0UL | 16UL))))) | (WaveAutoGeneration << (DAC_Channel & (0UL | 16UL))))));
}














 
static inline uint32_t LL_DAC_GetWaveAutoGeneration(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return (uint32_t)(((DACx->CR) & ((0x3UL << (6U)) << (DAC_Channel & (0UL | 16UL))))
                    >> (DAC_Channel & (0UL | 16UL))
                   );
}





























 
static inline void LL_DAC_SetWaveNoiseLFSR(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t NoiseLFSRMask)
{
  (((DACx->CR)) = ((((((DACx->CR))) & (~((0xFUL << (8U)) << (DAC_Channel & (0UL | 16UL))))) | (NoiseLFSRMask << (DAC_Channel & (0UL | 16UL))))));
}























 
static inline uint32_t LL_DAC_GetWaveNoiseLFSR(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return (uint32_t)(((DACx->CR) & ((0xFUL << (8U)) << (DAC_Channel & (0UL | 16UL))))
                    >> (DAC_Channel & (0UL | 16UL))
                   );
}





























 
static inline void LL_DAC_SetWaveTriangleAmplitude(DAC_TypeDef *DACx, uint32_t DAC_Channel,
                                                     uint32_t TriangleAmplitude)
{
  (((DACx->CR)) = ((((((DACx->CR))) & (~((0xFUL << (8U)) << (DAC_Channel & (0UL | 16UL))))) | (TriangleAmplitude << (DAC_Channel & (0UL | 16UL))))));
}























 
static inline uint32_t LL_DAC_GetWaveTriangleAmplitude(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return (uint32_t)(((DACx->CR) & ((0xFUL << (8U)) << (DAC_Channel & (0UL | 16UL))))
                    >> (DAC_Channel & (0UL | 16UL))
                   );
}













 
static inline void LL_DAC_SetOutputBuffer(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t OutputBuffer)
{
  (((DACx->CR)) = ((((((DACx->CR))) & (~((0x1UL << (1U)) << (DAC_Channel & (0UL | 16UL))))) | (OutputBuffer << (DAC_Channel & (0UL | 16UL))))));
}












 
static inline uint32_t LL_DAC_GetOutputBuffer(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return (uint32_t)(((DACx->CR) & ((0x1UL << (1U)) << (DAC_Channel & (0UL | 16UL))))
                    >> (DAC_Channel & (0UL | 16UL))
                   );
}



 



 












 
static inline void LL_DAC_EnableDMAReq(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->CR) |= ((0x1UL << (12U)) << (DAC_Channel & (0UL | 16UL))));
}












 
static inline void LL_DAC_DisableDMAReq(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->CR) &= ~((0x1UL << (12U)) << (DAC_Channel & (0UL | 16UL))));
}











 
static inline uint32_t LL_DAC_IsDMAReqEnabled(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return ((((DACx->CR) & ((0x1UL << (12U)) << (DAC_Channel & (0UL | 16UL))))
           == ((0x1UL << (12U)) << (DAC_Channel & (0UL | 16UL)))) ? 1UL : 0UL);
}
































 
static inline uint32_t LL_DAC_DMA_GetRegAddr(const DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t Register)
{
   
   
  return ((uint32_t)(((uint32_t *)((uint32_t) ((uint32_t)(&((DACx)->DHR12R1)) + ((((DAC_Channel >> (Register & 0x1FUL)) & 0x0000000FUL)) << 2UL))))));
}


 



 













 
static inline void LL_DAC_Enable(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->CR) |= ((0x1UL << (0U)) << (DAC_Channel & (0UL | 16UL))));
}










 
static inline void LL_DAC_Disable(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->CR) &= ~((0x1UL << (0U)) << (DAC_Channel & (0UL | 16UL))));
}











 
static inline uint32_t LL_DAC_IsEnabled(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return ((((DACx->CR) & ((0x1UL << (0U)) << (DAC_Channel & (0UL | 16UL))))
           == ((0x1UL << (0U)) << (DAC_Channel & (0UL | 16UL)))) ? 1UL : 0UL);
}


















 
static inline void LL_DAC_EnableTrigger(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->CR) |= ((0x1UL << (2U)) << (DAC_Channel & (0UL | 16UL))));
}










 
static inline void LL_DAC_DisableTrigger(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->CR) &= ~((0x1UL << (2U)) << (DAC_Channel & (0UL | 16UL))));
}











 
static inline uint32_t LL_DAC_IsTriggerEnabled(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  return ((((DACx->CR) & ((0x1UL << (2U)) << (DAC_Channel & (0UL | 16UL))))
           == ((0x1UL << (2U)) << (DAC_Channel & (0UL | 16UL)))) ? 1UL : 0UL);
}





















 
static inline void LL_DAC_TrigSWConversion(DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  ((DACx->SWTRIGR) |= ((DAC_Channel & (((0x1UL << (0U))) | ((0x1UL << (1U)))))));
}













 
static inline void LL_DAC_ConvertData12RightAligned(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t Data)
{
  volatile uint32_t *preg = ((uint32_t *)((uint32_t) ((uint32_t)(&(DACx->DHR12R1)) + (((DAC_Channel >> 16UL) & 0x0000000FUL) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (0U))))) | (Data))));
}













 
static inline void LL_DAC_ConvertData12LeftAligned(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t Data)
{
  volatile uint32_t *preg = ((uint32_t *)((uint32_t) ((uint32_t)(&(DACx->DHR12R1)) + (((DAC_Channel >> 20UL) & 0x0000000FUL) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (4U))))) | (Data))));
}













 
static inline void LL_DAC_ConvertData8RightAligned(DAC_TypeDef *DACx, uint32_t DAC_Channel, uint32_t Data)
{
  volatile uint32_t *preg = ((uint32_t *)((uint32_t) ((uint32_t)(&(DACx->DHR12R1)) + (((DAC_Channel >> 24UL) & 0x0000000FUL) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0xFFUL << (0U))))) | (Data))));
}












 
static inline void LL_DAC_ConvertDualData12RightAligned(DAC_TypeDef *DACx, uint32_t DataChannel1,
                                                          uint32_t DataChannel2)
{
  (((DACx->DHR12RD)) = ((((((DACx->DHR12RD))) & (~(((0xFFFUL << (16U)) | (0xFFFUL << (0U)))))) | (((DataChannel2 << (16U)) | DataChannel1)))));
}











 
static inline void LL_DAC_ConvertDualData12LeftAligned(DAC_TypeDef *DACx, uint32_t DataChannel1,
                                                         uint32_t DataChannel2)
{
   
   
   
  (((DACx->DHR12LD)) = ((((((DACx->DHR12LD))) & (~(((0xFFFUL << (20U)) | (0xFFFUL << (4U)))))) | (((DataChannel2 << ((20U) - 4U)) | DataChannel1)))));
}











 
static inline void LL_DAC_ConvertDualData8RightAligned(DAC_TypeDef *DACx, uint32_t DataChannel1,
                                                         uint32_t DataChannel2)
{
  (((DACx->DHR8RD)) = ((((((DACx->DHR8RD))) & (~(((0xFFUL << (8U)) | (0xFFUL << (0U)))))) | (((DataChannel2 << (8U)) | DataChannel1)))));
}















 
static inline uint32_t LL_DAC_RetrieveOutputData(const DAC_TypeDef *DACx, uint32_t DAC_Channel)
{
  volatile uint32_t const *preg = ((uint32_t *)((uint32_t) ((uint32_t)(&(DACx->DOR1)) + (((DAC_Channel >> 28UL) & 0x00000001UL) << 2UL))));

  return (uint16_t) ((*preg) & ((0xFFFUL << (0U))));
}



 



 






 
static inline uint32_t LL_DAC_IsActiveFlag_DMAUDR1(const DAC_TypeDef *DACx)
{
  return ((((DACx->SR) & (((0x1UL << (13U))))) == (((0x1UL << (13U))))) ? 1UL : 0UL);
}







 
static inline uint32_t LL_DAC_IsActiveFlag_DMAUDR2(const DAC_TypeDef *DACx)
{
  return ((((DACx->SR) & (((0x1UL << (29U))))) == (((0x1UL << (29U))))) ? 1UL : 0UL);
}







 
static inline void LL_DAC_ClearFlag_DMAUDR1(DAC_TypeDef *DACx)
{
  ((DACx->SR) = (((0x1UL << (13U)))));
}







 
static inline void LL_DAC_ClearFlag_DMAUDR2(DAC_TypeDef *DACx)
{
  ((DACx->SR) = (((0x1UL << (29U)))));
}




 



 






 
static inline void LL_DAC_EnableIT_DMAUDR1(DAC_TypeDef *DACx)
{
  ((DACx->CR) |= (((0x1UL << (13U)))));
}







 
static inline void LL_DAC_EnableIT_DMAUDR2(DAC_TypeDef *DACx)
{
  ((DACx->CR) |= (((0x1UL << (29U)))));
}







 
static inline void LL_DAC_DisableIT_DMAUDR1(DAC_TypeDef *DACx)
{
  ((DACx->CR) &= ~(((0x1UL << (13U)))));
}







 
static inline void LL_DAC_DisableIT_DMAUDR2(DAC_TypeDef *DACx)
{
  ((DACx->CR) &= ~(((0x1UL << (29U)))));
}







 
static inline uint32_t LL_DAC_IsEnabledIT_DMAUDR1(const DAC_TypeDef *DACx)
{
  return ((((DACx->CR) & (((0x1UL << (13U))))) == (((0x1UL << (13U))))) ? 1UL : 0UL);
}







 
static inline uint32_t LL_DAC_IsEnabledIT_DMAUDR2(const DAC_TypeDef *DACx)
{
  return ((((DACx->CR) & (((0x1UL << (29U))))) == (((0x1UL << (29U))))) ? 1UL : 0UL);
}




 



 

ErrorStatus LL_DAC_DeInit(const DAC_TypeDef *DACx);
ErrorStatus LL_DAC_Init(DAC_TypeDef *DACx, uint32_t DAC_Channel, const LL_DAC_InitTypeDef *DAC_InitStruct);
void        LL_DAC_StructInit(LL_DAC_InitTypeDef *DAC_InitStruct);



 



 



 




 


















 

 


 



 




 

 
 


 
 
static const uint8_t STREAM_OFFSET_TAB[] =
{
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x010UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x028UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x040UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x058UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x070UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x088UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x0A0UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL)),
  (uint8_t)((((0x40000000UL + 0x00020000UL) + 0x6000UL) + 0x0B8UL) - ((0x40000000UL + 0x00020000UL) + 0x6000UL))
};



 

 


 


 


 
 


 
typedef struct
{
  uint32_t PeriphOrM2MSrcAddress;  


 

  uint32_t MemoryOrM2MDstAddress;  


 

  uint32_t Direction;              



 

  uint32_t Mode;                   




 

  uint32_t PeriphOrM2MSrcIncMode;  



 

  uint32_t MemoryOrM2MDstIncMode;  



 

  uint32_t PeriphOrM2MSrcDataSize; 



 

  uint32_t MemoryOrM2MDstDataSize; 



 

  uint32_t NbData;                 




 

  uint32_t Channel;                


 

  uint32_t Priority;               


 
                                        
  uint32_t FIFOMode;               




 

  uint32_t FIFOThreshold;          


 

  uint32_t MemBurst;               





 

  uint32_t PeriphBurst;            





 

} LL_DMA_InitTypeDef;


 
 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 
  


 


   



 


 



 


 
    


 


 



 

 


 



 






 






 


 



 




 





 






 



 



 


 
 

 



 














 
static inline void LL_DMA_EnableStream(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) |= ((0x1UL << (0U))));
}















 
static inline void LL_DMA_DisableStream(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) &= ~((0x1UL << (0U))));
}















 
static inline uint32_t LL_DMA_IsEnabledStream(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}






























 
static inline void LL_DMA_ConfigTransfer(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Configuration)
{
  (((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (6U)) | (0x1UL << (8U)) | (0x1UL << (9U)) | (0x1UL << (10U)) | (0x3UL << (11U)) | (0x3UL << (13U)) | (0x3UL << (16U)) | (0x1UL << (5U))))) | (Configuration))));
}



















 
static inline void LL_DMA_SetDataTransferDirection(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Direction)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (6U))))) | (Direction))));
}


















 
static inline uint32_t LL_DMA_GetDataTransferDirection(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x3UL << (6U)))));
}




















 
static inline void LL_DMA_SetMode(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Mode)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x1UL << (8U)) | (0x1UL << (5U))))) | (Mode))));
}



















 
static inline uint32_t LL_DMA_GetMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (8U)) | (0x1UL << (5U)))));
}


















 
static inline void LL_DMA_SetPeriphIncMode(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t IncrementMode)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x1UL << (9U))))) | (IncrementMode))));
}

















 
static inline uint32_t LL_DMA_GetPeriphIncMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (9U)))));
}


















 
static inline void LL_DMA_SetMemoryIncMode(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t IncrementMode)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x1UL << (10U))))) | (IncrementMode))));
}

















 
static inline uint32_t LL_DMA_GetMemoryIncMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (10U)))));
}



















 
static inline void LL_DMA_SetPeriphSize(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Size)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (11U))))) | (Size))));
}


















 
static inline uint32_t LL_DMA_GetPeriphSize(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x3UL << (11U)))));
}



















 
static inline void LL_DMA_SetMemorySize(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Size)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (13U))))) | (Size))));
}


















 
static inline uint32_t LL_DMA_GetMemorySize(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x3UL << (13U)))));
}


















 
static inline void LL_DMA_SetIncOffsetSize(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t OffsetSize)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x1UL << (15U))))) | (OffsetSize))));
}

















 
static inline uint32_t LL_DMA_GetIncOffsetSize(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (15U)))));
}




















 
static inline void LL_DMA_SetStreamPriorityLevel(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t  Priority)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (16U))))) | (Priority))));
}



















 
static inline uint32_t LL_DMA_GetStreamPriorityLevel(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x3UL << (16U)))));
}


















 
static inline void LL_DMA_SetDataLength(DMA_TypeDef* DMAx, uint32_t Stream, uint32_t NbData)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->NDTR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->NDTR))) & (~((0xFFFFUL << (0U))))) | (NbData))));
}

















 
static inline uint32_t LL_DMA_GetDataLength(DMA_TypeDef* DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->NDTR) & ((0xFFFFUL << (0U)))));
}


































 
static inline void LL_DMA_SetChannelSelection(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Channel)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0xFUL << (25U))))) | (Channel))));
}

































 
static inline uint32_t LL_DMA_GetChannelSelection(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0xFUL << (25U)))));
}




















 
static inline void LL_DMA_SetMemoryBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Mburst)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (23U))))) | (Mburst))));
}



















 
static inline uint32_t LL_DMA_GetMemoryBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x3UL << (23U)))));
}




















 
static inline void LL_DMA_SetPeriphBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Pburst)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x3UL << (21U))))) | (Pburst))));
}



















 
static inline uint32_t LL_DMA_GetPeriphBurstxfer(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x3UL << (21U)))));
}


















 
static inline void LL_DMA_SetCurrentTargetMem(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t CurrentMemory)
{
   (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR))) & (~((0x1UL << (19U))))) | (CurrentMemory))));
}

















 
static inline uint32_t LL_DMA_GetCurrentTargetMem(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (19U)))));
}















 
static inline void LL_DMA_EnableDoubleBufferMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) |= ((0x1UL << (18U))));
}















 
static inline void LL_DMA_DisableDoubleBufferMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) &= ~((0x1UL << (18U))));
}





















 
static inline uint32_t LL_DMA_GetFIFOStatus(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) & ((0x7UL << (3U)))));
}















 
static inline void LL_DMA_DisableFifoMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) &= ~((0x1UL << (2U))));
}















 
static inline void LL_DMA_EnableFifoMode(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) |= ((0x1UL << (2U))));
}




















 
static inline void LL_DMA_SetFIFOThreshold(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Threshold)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR))) & (~((0x3UL << (0U))))) | (Threshold))));
}



















 
static inline uint32_t LL_DMA_GetFIFOThreshold(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) & ((0x3UL << (0U)))));
}
























 
static inline void LL_DMA_ConfigFifo(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t FifoMode, uint32_t FifoThreshold)
{
  (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR)) = ((((((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR))) & (~((0x3UL << (0U))|(0x1UL << (2U))))) | (FifoMode|FifoThreshold))));
}























 
static inline void LL_DMA_ConfigAddresses(DMA_TypeDef* DMAx, uint32_t Stream, uint32_t SrcAddress, uint32_t DstAddress, uint32_t Direction)
{
   
  if (Direction == (0x1UL << (6U)))
  {
    ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M0AR) = (SrcAddress));
    ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->PAR) = (DstAddress));
  }
   
  else
  {
    ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->PAR) = (SrcAddress));
    ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M0AR) = (DstAddress));
  }
}


















 
static inline void LL_DMA_SetMemoryAddress(DMA_TypeDef* DMAx, uint32_t Stream, uint32_t MemoryAddress)
{
  ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M0AR) = (MemoryAddress));
}


















 
static inline void LL_DMA_SetPeriphAddress(DMA_TypeDef* DMAx, uint32_t Stream, uint32_t PeriphAddress)
{
  ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->PAR) = (PeriphAddress));
}
















 
static inline uint32_t LL_DMA_GetMemoryAddress(DMA_TypeDef* DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M0AR)));
}
















 
static inline uint32_t LL_DMA_GetPeriphAddress(DMA_TypeDef* DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->PAR)));
}


















 
static inline void LL_DMA_SetM2MSrcAddress(DMA_TypeDef* DMAx, uint32_t Stream, uint32_t MemoryAddress)
{
  ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->PAR) = (MemoryAddress));
}


















 
static inline void LL_DMA_SetM2MDstAddress(DMA_TypeDef* DMAx, uint32_t Stream, uint32_t MemoryAddress)
  {
    ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M0AR) = (MemoryAddress));
  }
















 
static inline uint32_t LL_DMA_GetM2MSrcAddress(DMA_TypeDef* DMAx, uint32_t Stream)
  {
   return (((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->PAR)));
  }
















 
static inline uint32_t LL_DMA_GetM2MDstAddress(DMA_TypeDef* DMAx, uint32_t Stream)
{
 return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M0AR)));
}
















 
static inline void LL_DMA_SetMemory1Address(DMA_TypeDef *DMAx, uint32_t Stream, uint32_t Address)
{
  ((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M1AR) = (Address));
}















 
static inline uint32_t LL_DMA_GetMemory1Address(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->M1AR);
}



 



 






 
static inline uint32_t LL_DMA_IsActiveFlag_HT0(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (4U))))==((0x1UL << (4U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT1(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (10U))))==((0x1UL << (10U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT2(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (20U))))==((0x1UL << (20U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT3(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (26U))))==((0x1UL << (26U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT4(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (4U))))==((0x1UL << (4U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT5(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (10U))))==((0x1UL << (10U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT6(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (20U))))==((0x1UL << (20U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_HT7(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (26U))))==((0x1UL << (26U))));
} 






 
static inline uint32_t LL_DMA_IsActiveFlag_TC0(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (5U))))==((0x1UL << (5U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC1(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (11U))))==((0x1UL << (11U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC2(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (21U))))==((0x1UL << (21U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC3(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (27U))))==((0x1UL << (27U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC4(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (5U))))==((0x1UL << (5U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC5(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (11U))))==((0x1UL << (11U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC6(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (21U))))==((0x1UL << (21U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TC7(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (27U))))==((0x1UL << (27U))));
} 






 
static inline uint32_t LL_DMA_IsActiveFlag_TE0(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (3U))))==((0x1UL << (3U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE1(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (9U))))==((0x1UL << (9U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE2(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (19U))))==((0x1UL << (19U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE3(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (25U))))==((0x1UL << (25U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE4(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (3U))))==((0x1UL << (3U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE5(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (9U))))==((0x1UL << (9U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE6(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (19U))))==((0x1UL << (19U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_TE7(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (25U))))==((0x1UL << (25U))));
} 






 
static inline uint32_t LL_DMA_IsActiveFlag_DME0(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (2U))))==((0x1UL << (2U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME1(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (8U))))==((0x1UL << (8U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME2(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (18U))))==((0x1UL << (18U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME3(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (24U))))==((0x1UL << (24U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME4(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (2U))))==((0x1UL << (2U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME5(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (8U))))==((0x1UL << (8U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME6(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (18U))))==((0x1UL << (18U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_DME7(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (24U))))==((0x1UL << (24U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE0(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (0U))))==((0x1UL << (0U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE1(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (6U))))==((0x1UL << (6U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE2(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (16U))))==((0x1UL << (16U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE3(DMA_TypeDef *DMAx)
{
  return (((DMAx->LISR) & ((0x1UL << (22U))))==((0x1UL << (22U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE4(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (0U))))==((0x1UL << (0U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE5(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (6U))))==((0x1UL << (6U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE6(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (16U))))==((0x1UL << (16U))));
}






 
static inline uint32_t LL_DMA_IsActiveFlag_FE7(DMA_TypeDef *DMAx)
{
  return (((DMAx->HISR) & ((0x1UL << (22U))))==((0x1UL << (22U))));
}






 
static inline void LL_DMA_ClearFlag_HT0(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (4U))));
}






 
static inline void LL_DMA_ClearFlag_HT1(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (10U))));
}






 
static inline void LL_DMA_ClearFlag_HT2(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (20U))));
}






 
static inline void LL_DMA_ClearFlag_HT3(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (26U))));
}






 
static inline void LL_DMA_ClearFlag_HT4(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (4U))));
}






 
static inline void LL_DMA_ClearFlag_HT5(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (10U))));
}






 
static inline void LL_DMA_ClearFlag_HT6(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (20U))));
}






 
static inline void LL_DMA_ClearFlag_HT7(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (26U))));
}






 
static inline void LL_DMA_ClearFlag_TC0(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (5U))));
}






 
static inline void LL_DMA_ClearFlag_TC1(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (11U))));
}






 
static inline void LL_DMA_ClearFlag_TC2(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (21U))));
}






 
static inline void LL_DMA_ClearFlag_TC3(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (27U))));
}






 
static inline void LL_DMA_ClearFlag_TC4(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (5U))));
}






 
static inline void LL_DMA_ClearFlag_TC5(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (11U))));
}






 
static inline void LL_DMA_ClearFlag_TC6(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (21U))));
}






 
static inline void LL_DMA_ClearFlag_TC7(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (27U))));
}






 
static inline void LL_DMA_ClearFlag_TE0(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (3U))));
}






 
static inline void LL_DMA_ClearFlag_TE1(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (9U))));
}






 
static inline void LL_DMA_ClearFlag_TE2(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (19U))));
}






 
static inline void LL_DMA_ClearFlag_TE3(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (25U))));
}






 
static inline void LL_DMA_ClearFlag_TE4(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (3U))));
}






 
static inline void LL_DMA_ClearFlag_TE5(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (9U))));
}






 
static inline void LL_DMA_ClearFlag_TE6(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (19U))));
}






 
static inline void LL_DMA_ClearFlag_TE7(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (25U))));
}






 
static inline void LL_DMA_ClearFlag_DME0(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (2U))));
}






 
static inline void LL_DMA_ClearFlag_DME1(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (8U))));
}






 
static inline void LL_DMA_ClearFlag_DME2(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (18U))));
}






 
static inline void LL_DMA_ClearFlag_DME3(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (24U))));
}






 
static inline void LL_DMA_ClearFlag_DME4(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (2U))));
}






 
static inline void LL_DMA_ClearFlag_DME5(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (8U))));
}






 
static inline void LL_DMA_ClearFlag_DME6(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (18U))));
}






 
static inline void LL_DMA_ClearFlag_DME7(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (24U))));
}






 
static inline void LL_DMA_ClearFlag_FE0(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (0U))));
}






 
static inline void LL_DMA_ClearFlag_FE1(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (6U))));
}






 
static inline void LL_DMA_ClearFlag_FE2(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (16U))));
}






 
static inline void LL_DMA_ClearFlag_FE3(DMA_TypeDef *DMAx)
{
  ((DMAx->LIFCR) = ((0x1UL << (22U))));
}






 
static inline void LL_DMA_ClearFlag_FE4(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (0U))));
}






 
static inline void LL_DMA_ClearFlag_FE5(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (6U))));
}






 
static inline void LL_DMA_ClearFlag_FE6(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (16U))));
}






 
static inline void LL_DMA_ClearFlag_FE7(DMA_TypeDef *DMAx)
{
  ((DMAx->HIFCR) = ((0x1UL << (22U))));
}



 



 















 
static inline void LL_DMA_EnableIT_HT(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) |= ((0x1UL << (3U))));
}















 
static inline void LL_DMA_EnableIT_TE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) |= ((0x1UL << (2U))));
}















 
static inline void LL_DMA_EnableIT_TC(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) |= ((0x1UL << (4U))));
}















 
static inline void LL_DMA_EnableIT_DME(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) |= ((0x1UL << (1U))));
}















 
static inline void LL_DMA_EnableIT_FE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) |= ((0x1UL << (7U))));
}















 
static inline void LL_DMA_DisableIT_HT(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) &= ~((0x1UL << (3U))));
}















 
static inline void LL_DMA_DisableIT_TE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) &= ~((0x1UL << (2U))));
}















 
static inline void LL_DMA_DisableIT_TC(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) &= ~((0x1UL << (4U))));
}















 
static inline void LL_DMA_DisableIT_DME(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) &= ~((0x1UL << (1U))));
}















 
static inline void LL_DMA_DisableIT_FE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  ((((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) &= ~((0x1UL << (7U))));
}















 
static inline uint32_t LL_DMA_IsEnabledIT_HT(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (3U)))) == (0x1UL << (3U)));
}















 
static inline uint32_t LL_DMA_IsEnabledIT_TE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (2U)))) == (0x1UL << (2U)));
}















 
static inline uint32_t LL_DMA_IsEnabledIT_TC(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (4U)))) == (0x1UL << (4U)));
}















 
static inline uint32_t LL_DMA_IsEnabledIT_DME(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->CR) & ((0x1UL << (1U)))) == (0x1UL << (1U)));
}















 
static inline uint32_t LL_DMA_IsEnabledIT_FE(DMA_TypeDef *DMAx, uint32_t Stream)
{
  return (((((DMA_Stream_TypeDef*)((uint32_t)((uint32_t)DMAx + STREAM_OFFSET_TAB[Stream])))->FCR) & ((0x1UL << (7U)))) == (0x1UL << (7U)));
}



 



 

uint32_t LL_DMA_Init(DMA_TypeDef *DMAx, uint32_t Stream, LL_DMA_InitTypeDef *DMA_InitStruct);
uint32_t LL_DMA_DeInit(DMA_TypeDef *DMAx, uint32_t Stream);
void LL_DMA_StructInit(LL_DMA_InitTypeDef *DMA_InitStruct);



 



 



 




 


















 

 


 



 




 

 
 


 




 
 
 


 


 
 


 



 



 
typedef struct
{
  uint32_t SYSCLK_Frequency;         
  uint32_t HCLK_Frequency;           
  uint32_t PCLK1_Frequency;          
  uint32_t PCLK2_Frequency;          
} LL_RCC_ClocksTypeDef;



 



 

 


 






 








 




 


 




 


 




 


 



 


 



 


 



 


 



 


 



 


 


 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 





 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 






 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 





 


 



 

 


 



 






 





 


 



 












































































 






















































































 
























































































































 












































































 
























































































































 















































































 















 











 











 



 



 

 


 



 





 
static inline void LL_RCC_HSE_EnableCSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (19U))));
}





 
static inline void LL_RCC_HSE_EnableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (18U))));
}





 
static inline void LL_RCC_HSE_DisableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) &= ~((0x1UL << (18U))));
}





 
static inline void LL_RCC_HSE_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (16U))));
}





 
static inline void LL_RCC_HSE_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) &= ~((0x1UL << (16U))));
}





 
static inline uint32_t LL_RCC_HSE_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0x1UL << (17U)))) == ((0x1UL << (17U))));
}



 



 





 
static inline void LL_RCC_HSI_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_HSI_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) &= ~((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_HSI_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}







 
static inline uint32_t LL_RCC_HSI_GetCalibration(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0xFFUL << (8U)))) >> (8U));
}









 
static inline void LL_RCC_HSI_SetCalibTrimming(uint32_t Value)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR))) & (~((0x1FUL << (3U))))) | (Value << (3U)))));
}





 
static inline uint32_t LL_RCC_HSI_GetCalibTrimming(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0x1FUL << (3U)))) >> (3U));
}



 



 





 
static inline void LL_RCC_LSE_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_LSE_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) &= ~((0x1UL << (0U))));
}





 
static inline void LL_RCC_LSE_EnableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) |= ((0x1UL << (2U))));
}





 
static inline void LL_RCC_LSE_DisableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) &= ~((0x1UL << (2U))));
}











 
static inline void LL_RCC_LSE_SetDriveCapability(uint32_t LSEDrive)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR))) & (~((0x3UL << (3U))))) | (LSEDrive))));
}









 
static inline uint32_t LL_RCC_LSE_GetDriveCapability(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) & ((0x3UL << (3U)))));
}





 
static inline uint32_t LL_RCC_LSE_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}



 



 





 
static inline void LL_RCC_LSI_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_LSI_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) &= ~((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_LSI_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}



 



 









 
static inline void LL_RCC_SetSysClkSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((0x3UL << (0U))))) | (Source))));
}








 
static inline uint32_t LL_RCC_GetSysClkSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR) & ((0x3UL << (2U)))));
}















 
static inline void LL_RCC_SetAHBPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((0xFUL << (4U))))) | (Prescaler))));
}











 
static inline void LL_RCC_SetAPB1Prescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((0x7UL << (10U))))) | (Prescaler))));
}











 
static inline void LL_RCC_SetAPB2Prescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((0x7UL << (13U))))) | (Prescaler))));
}














 
static inline uint32_t LL_RCC_GetAHBPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR) & ((0xFUL << (4U)))));
}










 
static inline uint32_t LL_RCC_GetAPB1Prescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR) & ((0x7UL << (10U)))));
}










 
static inline uint32_t LL_RCC_GetAPB2Prescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR) & ((0x7UL << (13U)))));
}



 



 




























 
static inline void LL_RCC_ConfigMCO(uint32_t MCOxSource, uint32_t MCOxPrescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((MCOxSource & 0xFFFF0000U) | (MCOxPrescaler & 0xFFFF0000U)))) | ((MCOxSource << 16U) | (MCOxPrescaler << 16U)))));
}



 



 

























 
static inline void LL_RCC_SetUSARTClockSource(uint32_t USARTxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((USARTxSource >> 16U)))) | ((USARTxSource & 0x0000FFFFU)))));
}

























 
static inline void LL_RCC_SetUARTClockSource(uint32_t UARTxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((UARTxSource >> 16U)))) | ((UARTxSource & 0x0000FFFFU)))));
}























 
static inline void LL_RCC_SetI2CClockSource(uint32_t I2CxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((I2CxSource & 0xFFFF0000U)))) | ((I2CxSource << 16U)))));
}










 
static inline void LL_RCC_SetLPTIMClockSource(uint32_t LPTIMxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((0x3UL << (24U))))) | (LPTIMxSource))));
}

















 
static inline void LL_RCC_SetSAIClockSource(uint32_t SAIxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1))) & (~((SAIxSource & 0xFFFF0000U)))) | ((SAIxSource << 16U)))));
}













 
static inline void LL_RCC_SetSDMMCClockSource(uint32_t SDMMCxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((SDMMCxSource & 0xFFFF0000U)))) | ((SDMMCxSource << 16U)))));
}








 
static inline void LL_RCC_SetCK48MClockSource(uint32_t CK48MxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((0x1UL << (27U))))) | (CK48MxSource))));
}








 
static inline void LL_RCC_SetRNGClockSource(uint32_t RNGxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((0x1UL << (27U))))) | (RNGxSource))));
}








 
static inline void LL_RCC_SetUSBClockSource(uint32_t USBxSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2))) & (~((0x1UL << (27U))))) | (USBxSource))));
}









 
static inline void LL_RCC_SetI2SClockSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((0x1UL << (23U))))) | (Source))));
}































 
static inline uint32_t LL_RCC_GetUSARTClockSource(uint32_t USARTx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (USARTx)) | (USARTx << 16U));
}





























 
static inline uint32_t LL_RCC_GetUARTClockSource(uint32_t UARTx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (UARTx)) | (UARTx << 16U));
}



























 
static inline uint32_t LL_RCC_GetI2CClockSource(uint32_t I2Cx)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (I2Cx)) >> 16U) | I2Cx);
}











 
static inline uint32_t LL_RCC_GetLPTIMClockSource(uint32_t LPTIMx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & ((0x3UL << (24U)))));
}



















 
static inline uint32_t LL_RCC_GetSAIClockSource(uint32_t SAIx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1) & (SAIx)) >> 16U | SAIx);
}















 
static inline uint32_t LL_RCC_GetSDMMCClockSource(uint32_t SDMMCx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (SDMMCx)) >> 16U | SDMMCx);
}









 
static inline uint32_t LL_RCC_GetCK48MClockSource(uint32_t CK48Mx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (CK48Mx)));
}









 
static inline uint32_t LL_RCC_GetRNGClockSource(uint32_t RNGx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (RNGx)));
}









 
static inline uint32_t LL_RCC_GetUSBClockSource(uint32_t USBx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR2) & (USBx)));
}










 
static inline uint32_t LL_RCC_GetI2SClockSource(uint32_t I2Sx)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR) & (I2Sx)));
}





 



 













 
static inline void LL_RCC_SetRTCClockSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR))) & (~((0x3UL << (8U))))) | (Source))));
}









 
static inline uint32_t LL_RCC_GetRTCClockSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) & ((0x3UL << (8U)))));
}





 
static inline void LL_RCC_EnableRTC(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) |= ((0x1UL << (15U))));
}





 
static inline void LL_RCC_DisableRTC(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) &= ~((0x1UL << (15U))));
}





 
static inline uint32_t LL_RCC_IsEnabledRTC(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) & ((0x1UL << (15U)))) == ((0x1UL << (15U))));
}





 
static inline void LL_RCC_ForceBackupDomainReset(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) |= ((0x1UL << (16U))));
}





 
static inline void LL_RCC_ReleaseBackupDomainReset(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->BDCR) &= ~((0x1UL << (16U))));
}





































 
static inline void LL_RCC_SetRTC_HSEPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR))) & (~((0x1FUL << (16U))))) | (Prescaler))));
}




































 
static inline uint32_t LL_RCC_GetRTC_HSEPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CFGR) & ((0x1FUL << (16U)))));
}



 



 








 
static inline void LL_RCC_SetTIMPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1))) & (~((0x1UL << (24U))))) | (Prescaler))));
}







 
static inline uint32_t LL_RCC_GetTIMPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1) & ((0x1UL << (24U)))));
}



 



 





 
static inline void LL_RCC_PLL_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (24U))));
}






 
static inline void LL_RCC_PLL_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) &= ~((0x1UL << (24U))));
}





 
static inline uint32_t LL_RCC_PLL_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0x1UL << (25U)))) == ((0x1UL << (25U))));
}



















































































 
static inline void LL_RCC_PLL_ConfigDomain_SYS(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U)) | (0x3FUL << (0U)) | (0x1FFUL << (6U)) | (0x3UL << (16U))))) | (Source | PLLM | PLLN << (6U) | PLLP))));
}






























































































 
static inline void LL_RCC_PLL_ConfigDomain_48M(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLQ)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U)) | (0x3FUL << (0U)) | (0x1FFUL << (6U)) | (0xFUL << (24U))))) | (Source | PLLM | PLLN << (6U) | PLLQ))));
}









 
static inline void LL_RCC_PLL_SetMainSource(uint32_t PLLSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U))))) | (PLLSource))));
}







 
static inline uint32_t LL_RCC_PLL_GetMainSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR) & ((0x1UL << (22U)))));
}





 
static inline uint32_t LL_RCC_PLL_GetN(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR) & ((0x1FFUL << (6U)))) >>  (6U));
}









 
static inline uint32_t LL_RCC_PLL_GetP(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR) & ((0x3UL << (16U)))));
}




















 
static inline uint32_t LL_RCC_PLL_GetQ(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR) & ((0xFUL << (24U)))));
}




































































 
static inline uint32_t LL_RCC_PLL_GetDivider(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR) & ((0x3FUL << (0U)))));
}













 
static inline void LL_RCC_PLL_ConfigSpreadSpectrum(uint32_t Mod, uint32_t Inc, uint32_t Sel)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR))) & (~((0x1FFFUL << (0U)) | (0x7FFFUL << (13U)) | (0x1UL << (30U))))) | (Mod | (Inc << (13U)) | Sel))));
}





 
static inline uint32_t LL_RCC_PLL_GetPeriodModulation(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR) & ((0x1FFFUL << (0U)))));
}






 
static inline uint32_t LL_RCC_PLL_GetStepIncrementation(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR) & ((0x7FFFUL << (13U)))) >> (13U));
}








 
static inline uint32_t LL_RCC_PLL_GetSpreadSelection(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR) & ((0x1UL << (30U)))));
}





 
static inline void LL_RCC_PLL_SpreadSpectrum_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR) |= ((0x1UL << (31U))));
}





 
static inline void LL_RCC_PLL_SpreadSpectrum_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->SSCGR) &= ~((0x1UL << (31U))));
}



 



 





 
static inline void LL_RCC_PLLI2S_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (26U))));
}





 
static inline void LL_RCC_PLLI2S_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) &= ~((0x1UL << (26U))));
}





 
static inline uint32_t LL_RCC_PLLI2S_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0x1UL << (27U)))) == ((0x1UL << (27U))));
}
































































































































 
static inline void LL_RCC_PLLI2S_ConfigDomain_SAI(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLQ, uint32_t PLLDIVQ)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U)) | (0x3FUL << (0U))))) | (Source | PLLM))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR))) & (~((0x1FFUL << (6U)) | (0xFUL << (24U))))) | (PLLN << (6U) | PLLQ))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1))) & (~((0x1FUL << (0U))))) | (PLLDIVQ))));
}























































































 
static inline void LL_RCC_PLLI2S_ConfigDomain_I2S(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLR)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U)) | (0x3FUL << (0U))))) | (Source | PLLM))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR))) & (~((0x1FFUL << (6U)) | (0x7UL << (28U))))) | (PLLN << (6U) | PLLR))));
}





 
static inline uint32_t LL_RCC_PLLI2S_GetN(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR) & ((0x1FFUL << (6U)))) >> (6U));
}



















 
static inline uint32_t LL_RCC_PLLI2S_GetQ(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR) & ((0xFUL << (24U)))));
}












 
static inline uint32_t LL_RCC_PLLI2S_GetR(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLI2SCFGR) & ((0x7UL << (28U)))));
}







































 
static inline uint32_t LL_RCC_PLLI2S_GetDIVQ(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1) & ((0x1FUL << (0U)))));
}



 



 





 
static inline void LL_RCC_PLLSAI_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) |= ((0x1UL << (28U))));
}





 
static inline void LL_RCC_PLLSAI_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) &= ~((0x1UL << (28U))));
}





 
static inline uint32_t LL_RCC_PLLSAI_IsReady(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CR) & ((0x1UL << (29U)))) == ((0x1UL << (29U))));
}
































































































































 
static inline void LL_RCC_PLLSAI_ConfigDomain_SAI(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLQ, uint32_t PLLDIVQ)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U)) | (0x3FUL << (0U))))) | (Source | PLLM))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR))) & (~((0x1FFUL << (6U)) | (0xFUL << (24U))))) | (PLLN << (6U) | PLLQ))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1))) & (~((0x1FUL << (8U))))) | (PLLDIVQ))));
}




















































































 
static inline void LL_RCC_PLLSAI_ConfigDomain_48M(uint32_t Source, uint32_t PLLM, uint32_t PLLN, uint32_t PLLP)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLCFGR))) & (~((0x1UL << (22U)) | (0x3FUL << (0U))))) | (Source | PLLM))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR))) & (~((0x1FFUL << (6U)) | (0x3UL << (16U))))) | (PLLN << (6U) | PLLP))));
}






 
static inline uint32_t LL_RCC_PLLSAI_GetN(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR) & ((0x1FFUL << (6U)))) >> (6U));
}



















 
static inline uint32_t LL_RCC_PLLSAI_GetQ(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR) & ((0xFUL << (24U)))));
}











 
static inline uint32_t LL_RCC_PLLSAI_GetP(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->PLLSAICFGR) & ((0x3UL << (16U)))));
}






































 
static inline uint32_t LL_RCC_PLLSAI_GetDIVQ(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->DCKCFGR1) & ((0x1FUL << (8U)))));
}




 



 





 
static inline void LL_RCC_ClearFlag_LSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (16U))));
}





 
static inline void LL_RCC_ClearFlag_LSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (17U))));
}





 
static inline void LL_RCC_ClearFlag_HSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (18U))));
}





 
static inline void LL_RCC_ClearFlag_HSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (19U))));
}





 
static inline void LL_RCC_ClearFlag_PLLRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (20U))));
}





 
static inline void LL_RCC_ClearFlag_PLLI2SRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (21U))));
}





 
static inline void LL_RCC_ClearFlag_PLLSAIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (22U))));
}





 
static inline void LL_RCC_ClearFlag_HSECSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (23U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LSIRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LSERDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSIRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSERDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (3U)))) == ((0x1UL << (3U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PLLRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (4U)))) == ((0x1UL << (4U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PLLI2SRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PLLSAIRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (6U)))) == ((0x1UL << (6U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSECSS(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (7U)))) == ((0x1UL << (7U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_IWDGRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (29U)))) == ((0x1UL << (29U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LPWRRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (31U)))) == ((0x1UL << (31U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PINRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (26U)))) == ((0x1UL << (26U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PORRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (27U)))) == ((0x1UL << (27U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_SFTRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (28U)))) == ((0x1UL << (28U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_WWDGRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (30U)))) == ((0x1UL << (30U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_BORRST(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) & ((0x1UL << (25U)))) == ((0x1UL << (25U))));
}





 
static inline void LL_RCC_ClearResetFlags(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CSR) |= ((0x1UL << (24U))));
}



 



 





 
static inline void LL_RCC_EnableIT_LSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (8U))));
}





 
static inline void LL_RCC_EnableIT_LSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (9U))));
}





 
static inline void LL_RCC_EnableIT_HSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (10U))));
}





 
static inline void LL_RCC_EnableIT_HSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (11U))));
}





 
static inline void LL_RCC_EnableIT_PLLRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (12U))));
}





 
static inline void LL_RCC_EnableIT_PLLI2SRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (13U))));
}





 
static inline void LL_RCC_EnableIT_PLLSAIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) |= ((0x1UL << (14U))));
}





 
static inline void LL_RCC_DisableIT_LSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (8U))));
}





 
static inline void LL_RCC_DisableIT_LSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (9U))));
}





 
static inline void LL_RCC_DisableIT_HSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (10U))));
}





 
static inline void LL_RCC_DisableIT_HSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (11U))));
}





 
static inline void LL_RCC_DisableIT_PLLRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (12U))));
}





 
static inline void LL_RCC_DisableIT_PLLI2SRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (13U))));
}





 
static inline void LL_RCC_DisableIT_PLLSAIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) &= ~((0x1UL << (14U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_LSIRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_LSERDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (9U)))) == ((0x1UL << (9U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_HSIRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (10U)))) == ((0x1UL << (10U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_HSERDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (11U)))) == ((0x1UL << (11U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_PLLRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (12U)))) == ((0x1UL << (12U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_PLLI2SRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (13U)))) == ((0x1UL << (13U))));
}





 
static inline uint32_t LL_RCC_IsEnabledIT_PLLSAIRDY(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->CIR) & ((0x1UL << (14U)))) == ((0x1UL << (14U))));
}



 



 
ErrorStatus LL_RCC_DeInit(void);


 



 
void        LL_RCC_GetSystemClocksFreq(LL_RCC_ClocksTypeDef *RCC_Clocks);
uint32_t    LL_RCC_GetUSARTClockFreq(uint32_t USARTxSource);
uint32_t    LL_RCC_GetUARTClockFreq(uint32_t UARTxSource);
uint32_t    LL_RCC_GetI2CClockFreq(uint32_t I2CxSource);
uint32_t    LL_RCC_GetLPTIMClockFreq(uint32_t LPTIMxSource);
uint32_t    LL_RCC_GetSAIClockFreq(uint32_t SAIxSource);
uint32_t    LL_RCC_GetSDMMCClockFreq(uint32_t SDMMCxSource);
uint32_t    LL_RCC_GetRNGClockFreq(uint32_t RNGxSource);
uint32_t    LL_RCC_GetUSBClockFreq(uint32_t USBxSource);
uint32_t    LL_RCC_GetI2SClockFreq(uint32_t I2SxSource);


 



 



 




 



















 

 


 



 




 

 
 
 
 

 
 


 



 


 



 


 



 


 





 


 



 


 



 

 


 



 







 






 


 



 


 


 



 









 
static inline void LL_CRC_ResetCRCCalculationUnit(CRC_TypeDef *CRCx)
{
  ((CRCx->CR) |= ((0x1UL << (0U))));
}











 
static inline void LL_CRC_SetPolynomialSize(CRC_TypeDef *CRCx, uint32_t PolySize)
{
  (((CRCx->CR)) = ((((((CRCx->CR))) & (~((0x3UL << (3U))))) | (PolySize))));
}










 
static inline uint32_t LL_CRC_GetPolynomialSize(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->CR) & ((0x3UL << (3U)))));
}











 
static inline void LL_CRC_SetInputDataReverseMode(CRC_TypeDef *CRCx, uint32_t ReverseMode)
{
  (((CRCx->CR)) = ((((((CRCx->CR))) & (~((0x3UL << (5U))))) | (ReverseMode))));
}










 
static inline uint32_t LL_CRC_GetInputDataReverseMode(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->CR) & ((0x3UL << (5U)))));
}









 
static inline void LL_CRC_SetOutputDataReverseMode(CRC_TypeDef *CRCx, uint32_t ReverseMode)
{
  (((CRCx->CR)) = ((((((CRCx->CR))) & (~((0x1UL << (7U))))) | (ReverseMode))));
}








 
static inline uint32_t LL_CRC_GetOutputDataReverseMode(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->CR) & ((0x1UL << (7U)))));
}










 
static inline void LL_CRC_SetInitialData(CRC_TypeDef *CRCx, uint32_t InitCrc)
{
  ((CRCx->INIT) = (InitCrc));
}








 
static inline uint32_t LL_CRC_GetInitialData(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->INIT)));
}












 
static inline void LL_CRC_SetPolynomialCoef(CRC_TypeDef *CRCx, uint32_t PolynomCoef)
{
  ((CRCx->POL) = (PolynomCoef));
}









 
static inline uint32_t LL_CRC_GetPolynomialCoef(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->POL)));
}



 



 







 
static inline void LL_CRC_FeedData32(CRC_TypeDef *CRCx, uint32_t InData)
{
  ((CRCx->DR) = (InData));
}







 
static inline void LL_CRC_FeedData16(CRC_TypeDef *CRCx, uint16_t InData)
{
  volatile uint16_t *pReg;

  pReg = (volatile uint16_t *)(volatile void *)(&CRCx->DR);                              
  *pReg = InData;
}







 
static inline void LL_CRC_FeedData8(CRC_TypeDef *CRCx, uint8_t InData)
{
  *(uint8_t volatile *)(&CRCx->DR) = (uint8_t) InData;
}






 
static inline uint32_t LL_CRC_ReadData32(const CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->DR)));
}







 
static inline uint16_t LL_CRC_ReadData16(const CRC_TypeDef *CRCx)
{
  return (uint16_t)((CRCx->DR));
}







 
static inline uint8_t LL_CRC_ReadData8(const CRC_TypeDef *CRCx)
{
  return (uint8_t)((CRCx->DR));
}







 
static inline uint8_t LL_CRC_ReadData7(const CRC_TypeDef *CRCx)
{
  return (uint8_t)(((CRCx->DR)) & 0x7FU);
}







 
static inline uint32_t LL_CRC_Read_IDR(CRC_TypeDef *CRCx)
{
  return (uint32_t)(((CRCx->IDR)));
}








 
static inline void LL_CRC_Write_IDR(CRC_TypeDef *CRCx, uint32_t InData)
{
  *((uint8_t volatile *)(&CRCx->IDR)) = (uint8_t) InData;
}


 



 

ErrorStatus LL_CRC_DeInit(const CRC_TypeDef *CRCx);



 



 



 




 


















 

 


 



 




 

 
 


 
static const uint8_t OFFSET_TAB_CCMRx[] =
{
  0x00U,    
  0x00U,    
  0x00U,    
  0x00U,    
  0x04U,    
  0x04U,    
  0x04U,    
  0x3CU,    
  0x3CU     
};

static const uint8_t SHIFT_TAB_OCxx[] =
{
  0U,             
  0U,             
  8U,             
  0U,             
  0U,             
  0U,             
  8U,             
  0U,             
  8U              
};

static const uint8_t SHIFT_TAB_ICxx[] =
{
  0U,             
  0U,             
  8U,             
  0U,             
  0U,             
  0U,             
  8U,             
  0U,             
  0U              
};

static const uint8_t SHIFT_TAB_CCxP[] =
{
  0U,             
  2U,             
  4U,             
  6U,             
  8U,             
  10U,            
  12U,            
  16U,            
  20U             
};

static const uint8_t SHIFT_TAB_OISx[] =
{
  0U,             
  1U,             
  2U,             
  3U,             
  4U,             
  5U,             
  6U,             
  8U,             
  10U             
};


 

 


 


 

 

 




 

 


 












 








 


 


 


 



 
typedef struct
{
  uint16_t Prescaler;         



 

  uint32_t CounterMode;       



 

  uint32_t Autoreload;        






 

  uint32_t ClockDivision;     



 

  uint32_t RepetitionCounter;  











 
} LL_TIM_InitTypeDef;



 
typedef struct
{
  uint32_t OCMode;        



 

  uint32_t OCState;       



 

  uint32_t OCNState;      



 

  uint32_t CompareValue;  



 

  uint32_t OCPolarity;    



 

  uint32_t OCNPolarity;   



 


  uint32_t OCIdleState;   



 

  uint32_t OCNIdleState;  



 
} LL_TIM_OC_InitTypeDef;



 

typedef struct
{

  uint32_t ICPolarity;    



 

  uint32_t ICActiveInput; 



 

  uint32_t ICPrescaler;   



 

  uint32_t ICFilter;      



 
} LL_TIM_IC_InitTypeDef;




 
typedef struct
{
  uint32_t EncoderMode;     



 

  uint32_t IC1Polarity;     



 

  uint32_t IC1ActiveInput;  



 

  uint32_t IC1Prescaler;    



 

  uint32_t IC1Filter;       



 

  uint32_t IC2Polarity;      



 

  uint32_t IC2ActiveInput;  



 

  uint32_t IC2Prescaler;    



 

  uint32_t IC2Filter;       



 

} LL_TIM_ENCODER_InitTypeDef;



 
typedef struct
{

  uint32_t IC1Polarity;        



 

  uint32_t IC1Prescaler;       





 

  uint32_t IC1Filter;          




 

  uint32_t CommutationDelay;   





 
} LL_TIM_HALLSENSOR_InitTypeDef;



 
typedef struct
{
  uint32_t OSSRState;            






 

  uint32_t OSSIState;            






 

  uint32_t LockLevel;            



 

  uint8_t DeadTime;              







 

  uint16_t BreakState;           






 

  uint32_t BreakPolarity;        






 

  uint32_t BreakFilter;          






 

  uint32_t Break2State;          






 

  uint32_t Break2Polarity;        






 

  uint32_t Break2Filter;          






 

  uint32_t AutomaticOutput;      






 
} LL_TIM_BDTR_InitTypeDef;



 

 


 




 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 




 


 



 


 



 


 




 

 


 



 






 






 


 








 











 







 








 









 










 










 




 

 


 



 





 
static inline void LL_TIM_EnableCounter(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) |= ((0x1UL << (0U))));
}






 
static inline void LL_TIM_DisableCounter(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) &= ~((0x1UL << (0U))));
}






 
static inline uint32_t LL_TIM_IsEnabledCounter(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->CR1) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableUpdateEvent(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) &= ~((0x1UL << (1U))));
}






 
static inline void LL_TIM_DisableUpdateEvent(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) |= ((0x1UL << (1U))));
}






 
static inline uint32_t LL_TIM_IsEnabledUpdateEvent(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->CR1) & ((0x1UL << (1U)))) == (uint32_t)RESET) ? 1UL : 0UL);
}
















 
static inline void LL_TIM_SetUpdateSource(TIM_TypeDef *TIMx, uint32_t UpdateSource)
{
  (((TIMx->CR1)) = ((((((TIMx->CR1))) & (~((0x1UL << (2U))))) | (UpdateSource))));
}








 
static inline uint32_t LL_TIM_GetUpdateSource(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CR1) & ((0x1UL << (2U)))));
}









 
static inline void LL_TIM_SetOnePulseMode(TIM_TypeDef *TIMx, uint32_t OnePulseMode)
{
  (((TIMx->CR1)) = ((((((TIMx->CR1))) & (~((0x1UL << (3U))))) | (OnePulseMode))));
}








 
static inline uint32_t LL_TIM_GetOnePulseMode(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CR1) & ((0x1UL << (3U)))));
}



















 
static inline void LL_TIM_SetCounterMode(TIM_TypeDef *TIMx, uint32_t CounterMode)
{
  (((TIMx->CR1)) = ((((((TIMx->CR1))) & (~(((0x1UL << (4U)) | (0x3UL << (5U)))))) | (CounterMode))));
}















 
static inline uint32_t LL_TIM_GetCounterMode(const TIM_TypeDef *TIMx)
{
  uint32_t counter_mode;

  counter_mode = (uint32_t)(((TIMx->CR1) & ((0x3UL << (5U)))));

  if (counter_mode == 0U)
  {
    counter_mode = (uint32_t)(((TIMx->CR1) & ((0x1UL << (4U)))));
  }

  return counter_mode;
}






 
static inline void LL_TIM_EnableARRPreload(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) |= ((0x1UL << (7U))));
}






 
static inline void LL_TIM_DisableARRPreload(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) &= ~((0x1UL << (7U))));
}






 
static inline uint32_t LL_TIM_IsEnabledARRPreload(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->CR1) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}














 
static inline void LL_TIM_SetClockDivision(TIM_TypeDef *TIMx, uint32_t ClockDivision)
{
  (((TIMx->CR1)) = ((((((TIMx->CR1))) & (~((0x3UL << (8U))))) | (ClockDivision))));
}













 
static inline uint32_t LL_TIM_GetClockDivision(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CR1) & ((0x3UL << (8U)))));
}









 
static inline void LL_TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t Counter)
{
  ((TIMx->CNT) = (Counter));
}








 
static inline uint32_t LL_TIM_GetCounter(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CNT)));
}








 
static inline uint32_t LL_TIM_GetDirection(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CR1) & ((0x1UL << (4U)))));
}











 
static inline void LL_TIM_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Prescaler)
{
  ((TIMx->PSC) = (Prescaler));
}






 
static inline uint32_t LL_TIM_GetPrescaler(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->PSC)));
}











 
static inline void LL_TIM_SetAutoReload(TIM_TypeDef *TIMx, uint32_t AutoReload)
{
  ((TIMx->ARR) = (AutoReload));
}








 
static inline uint32_t LL_TIM_GetAutoReload(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->ARR)));
}










 
static inline void LL_TIM_SetRepetitionCounter(TIM_TypeDef *TIMx, uint32_t RepetitionCounter)
{
  ((TIMx->RCR) = (RepetitionCounter));
}








 
static inline uint32_t LL_TIM_GetRepetitionCounter(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->RCR)));
}








 
static inline void LL_TIM_EnableUIFRemap(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) |= ((0x1UL << (11U))));
}






 
static inline void LL_TIM_DisableUIFRemap(TIM_TypeDef *TIMx)
{
  ((TIMx->CR1) &= ~((0x1UL << (11U))));
}





 
static inline uint32_t LL_TIM_IsActiveUIFCPY(const uint32_t Counter)
{
  return (((Counter & (0x1UL << (31U))) == ((0x1UL << (31U)))) ? 1UL : 0UL);
}



 



 










 
static inline void LL_TIM_CC_EnablePreload(TIM_TypeDef *TIMx)
{
  ((TIMx->CR2) |= ((0x1UL << (0U))));
}








 
static inline void LL_TIM_CC_DisablePreload(TIM_TypeDef *TIMx)
{
  ((TIMx->CR2) &= ~((0x1UL << (0U))));
}






 
static inline uint32_t LL_TIM_CC_IsEnabledPreload(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->CR2) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}











 
static inline void LL_TIM_CC_SetUpdate(TIM_TypeDef *TIMx, uint32_t CCUpdateSource)
{
  (((TIMx->CR2)) = ((((((TIMx->CR2))) & (~((0x1UL << (2U))))) | (CCUpdateSource))));
}









 
static inline void LL_TIM_CC_SetDMAReqTrigger(TIM_TypeDef *TIMx, uint32_t DMAReqTrigger)
{
  (((TIMx->CR2)) = ((((((TIMx->CR2))) & (~((0x1UL << (3U))))) | (DMAReqTrigger))));
}








 
static inline uint32_t LL_TIM_CC_GetDMAReqTrigger(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CR2) & ((0x1UL << (3U)))));
}














 
static inline void LL_TIM_CC_SetLockLevel(TIM_TypeDef *TIMx, uint32_t LockLevel)
{
  (((TIMx->BDTR)) = ((((((TIMx->BDTR))) & (~((0x3UL << (8U))))) | (LockLevel))));
}
























 
static inline void LL_TIM_CC_EnableChannel(TIM_TypeDef *TIMx, uint32_t Channels)
{
  ((TIMx->CCER) |= (Channels));
}
























 
static inline void LL_TIM_CC_DisableChannel(TIM_TypeDef *TIMx, uint32_t Channels)
{
  ((TIMx->CCER) &= ~(Channels));
}
























 
static inline uint32_t LL_TIM_CC_IsEnabledChannel(const TIM_TypeDef *TIMx, uint32_t Channels)
{
  return ((((TIMx->CCER) & (Channels)) == (Channels)) ? 1UL : 0UL);
}



 



 
































 
static inline void LL_TIM_OC_ConfigOutput(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Configuration)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) &= ~(((0x3UL << (0U)) << SHIFT_TAB_OCxx[iChannel])));
  (((TIMx->CCER)) = ((((((TIMx->CCER))) & (~(((0x1UL << (1U)) << SHIFT_TAB_CCxP[iChannel])))) | ((Configuration & (0x1UL << (1U))) << SHIFT_TAB_CCxP[iChannel]))));
  (((TIMx->CR2)) = ((((((TIMx->CR2))) & (~(((0x1UL << (8U)) << SHIFT_TAB_OISx[iChannel])))) | ((Configuration & (0x1UL << (8U))) << SHIFT_TAB_OISx[iChannel]))));
}


































 
static inline void LL_TIM_OC_SetMode(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Mode)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  (((*pReg)) = ((((((*pReg))) & (~((((0x1007UL << (4U)) | (0x3UL << (0U))) << SHIFT_TAB_OCxx[iChannel])))) | (Mode << SHIFT_TAB_OCxx[iChannel]))));
}
































 
static inline uint32_t LL_TIM_OC_GetMode(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return (((*pReg) & ((((0x1007UL << (4U)) | (0x3UL << (0U))) << SHIFT_TAB_OCxx[iChannel]))) >> SHIFT_TAB_OCxx[iChannel]);
}



























 
static inline void LL_TIM_OC_SetPolarity(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Polarity)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  (((TIMx->CCER)) = ((((((TIMx->CCER))) & (~(((0x1UL << (1U)) << SHIFT_TAB_CCxP[iChannel])))) | (Polarity << SHIFT_TAB_CCxP[iChannel]))));
}


























 
static inline uint32_t LL_TIM_OC_GetPolarity(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  return (((TIMx->CCER) & (((0x1UL << (1U)) << SHIFT_TAB_CCxP[iChannel]))) >> SHIFT_TAB_CCxP[iChannel]);
}































 
static inline void LL_TIM_OC_SetIdleState(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t IdleState)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  (((TIMx->CR2)) = ((((((TIMx->CR2))) & (~(((0x1UL << (8U)) << SHIFT_TAB_OISx[iChannel])))) | (IdleState << SHIFT_TAB_OISx[iChannel]))));
}


























 
static inline uint32_t LL_TIM_OC_GetIdleState(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  return (((TIMx->CR2) & (((0x1UL << (8U)) << SHIFT_TAB_OISx[iChannel]))) >> SHIFT_TAB_OISx[iChannel]);
}



















 
static inline void LL_TIM_OC_EnableFast(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) |= (((0x1UL << (2U)) << SHIFT_TAB_OCxx[iChannel])));

}


















 
static inline void LL_TIM_OC_DisableFast(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) &= ~(((0x1UL << (2U)) << SHIFT_TAB_OCxx[iChannel])));

}


















 
static inline uint32_t LL_TIM_OC_IsEnabledFast(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  uint32_t bitfield = (0x1UL << (2U)) << SHIFT_TAB_OCxx[iChannel];
  return ((((*pReg) & (bitfield)) == bitfield) ? 1UL : 0UL);
}


















 
static inline void LL_TIM_OC_EnablePreload(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) |= (((0x1UL << (3U)) << SHIFT_TAB_OCxx[iChannel])));
}


















 
static inline void LL_TIM_OC_DisablePreload(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) &= ~(((0x1UL << (3U)) << SHIFT_TAB_OCxx[iChannel])));
}


















 
static inline uint32_t LL_TIM_OC_IsEnabledPreload(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  uint32_t bitfield = (0x1UL << (3U)) << SHIFT_TAB_OCxx[iChannel];
  return ((((*pReg) & (bitfield)) == bitfield) ? 1UL : 0UL);
}





















 
static inline void LL_TIM_OC_EnableClear(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) |= (((0x1UL << (7U)) << SHIFT_TAB_OCxx[iChannel])));
}




















 
static inline void LL_TIM_OC_DisableClear(TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  ((*pReg) &= ~(((0x1UL << (7U)) << SHIFT_TAB_OCxx[iChannel])));
}






















 
static inline uint32_t LL_TIM_OC_IsEnabledClear(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  uint32_t bitfield = (0x1UL << (7U)) << SHIFT_TAB_OCxx[iChannel];
  return ((((*pReg) & (bitfield)) == bitfield) ? 1UL : 0UL);
}











 
static inline void LL_TIM_OC_SetDeadTime(TIM_TypeDef *TIMx, uint32_t DeadTime)
{
  (((TIMx->BDTR)) = ((((((TIMx->BDTR))) & (~((0xFFUL << (0U))))) | (DeadTime))));
}












 
static inline void LL_TIM_OC_SetCompareCH1(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  ((TIMx->CCR1) = (CompareValue));
}












 
static inline void LL_TIM_OC_SetCompareCH2(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  ((TIMx->CCR2) = (CompareValue));
}












 
static inline void LL_TIM_OC_SetCompareCH3(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  ((TIMx->CCR3) = (CompareValue));
}












 
static inline void LL_TIM_OC_SetCompareCH4(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  ((TIMx->CCR4) = (CompareValue));
}









 
static inline void LL_TIM_OC_SetCompareCH5(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  (((TIMx->CCR5)) = ((((((TIMx->CCR5))) & (~((0xFFFFFFFFUL << (0U))))) | (CompareValue))));
}









 
static inline void LL_TIM_OC_SetCompareCH6(TIM_TypeDef *TIMx, uint32_t CompareValue)
{
  ((TIMx->CCR6) = (CompareValue));
}











 
static inline uint32_t LL_TIM_OC_GetCompareCH1(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR1)));
}











 
static inline uint32_t LL_TIM_OC_GetCompareCH2(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR2)));
}











 
static inline uint32_t LL_TIM_OC_GetCompareCH3(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR3)));
}











 
static inline uint32_t LL_TIM_OC_GetCompareCH4(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR4)));
}








 
static inline uint32_t LL_TIM_OC_GetCompareCH5(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR5) & ((0xFFFFFFFFUL << (0U)))));
}








 
static inline uint32_t LL_TIM_OC_GetCompareCH6(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR6)));
}















 
static inline void LL_TIM_SetCH5CombinedChannels(TIM_TypeDef *TIMx, uint32_t GroupCH5)
{
  (((TIMx->CCR5)) = ((((((TIMx->CCR5))) & (~(((0x1UL << (31U)) | (0x1UL << (30U)) | (0x1UL << (29U)))))) | (GroupCH5))));
}



 



 


































 
static inline void LL_TIM_IC_Config(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Configuration)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  (((*pReg)) = ((((((*pReg))) & (~((((0xFUL << (4U)) | (0x3UL << (2U)) | (0x3UL << (0U))) << SHIFT_TAB_ICxx[iChannel])))) | (((Configuration >> 16U) & ((0xFUL << (4U)) | (0x3UL << (2U)) | (0x3UL << (0U)))) << SHIFT_TAB_ICxx[iChannel]))));
  (((TIMx->CCER)) = ((((((TIMx->CCER))) & (~((((0x1UL << (3U)) | (0x1UL << (1U))) << SHIFT_TAB_CCxP[iChannel])))) | ((Configuration & ((0x1UL << (3U)) | (0x1UL << (1U)))) << SHIFT_TAB_CCxP[iChannel]))));
}


















 
static inline void LL_TIM_IC_SetActiveInput(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICActiveInput)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  (((*pReg)) = ((((((*pReg))) & (~((((0x3UL << (0U))) << SHIFT_TAB_ICxx[iChannel])))) | ((ICActiveInput >> 16U) << SHIFT_TAB_ICxx[iChannel]))));
}

















 
static inline uint32_t LL_TIM_IC_GetActiveInput(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return ((((*pReg) & ((((0x3UL << (0U))) << SHIFT_TAB_ICxx[iChannel]))) >> SHIFT_TAB_ICxx[iChannel]) << 16U);
}



















 
static inline void LL_TIM_IC_SetPrescaler(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICPrescaler)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  (((*pReg)) = ((((((*pReg))) & (~((((0x3UL << (2U))) << SHIFT_TAB_ICxx[iChannel])))) | ((ICPrescaler >> 16U) << SHIFT_TAB_ICxx[iChannel]))));
}


















 
static inline uint32_t LL_TIM_IC_GetPrescaler(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return ((((*pReg) & ((((0x3UL << (2U))) << SHIFT_TAB_ICxx[iChannel]))) >> SHIFT_TAB_ICxx[iChannel]) << 16U);
}































 
static inline void LL_TIM_IC_SetFilter(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICFilter)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  (((*pReg)) = ((((((*pReg))) & (~((((0xFUL << (4U))) << SHIFT_TAB_ICxx[iChannel])))) | ((ICFilter >> 16U) << SHIFT_TAB_ICxx[iChannel]))));
}






























 
static inline uint32_t LL_TIM_IC_GetFilter(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  const volatile uint32_t *pReg = (volatile uint32_t *)((uint32_t)((uint32_t)(&TIMx->CCMR1) + OFFSET_TAB_CCMRx[iChannel]));
  return ((((*pReg) & ((((0xFUL << (4U))) << SHIFT_TAB_ICxx[iChannel]))) >> SHIFT_TAB_ICxx[iChannel]) << 16U);
}






















 
static inline void LL_TIM_IC_SetPolarity(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ICPolarity)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  (((TIMx->CCER)) = ((((((TIMx->CCER))) & (~((((0x1UL << (3U)) | (0x1UL << (1U))) << SHIFT_TAB_CCxP[iChannel])))) | (ICPolarity << SHIFT_TAB_CCxP[iChannel]))));
}





















 
static inline uint32_t LL_TIM_IC_GetPolarity(const TIM_TypeDef *TIMx, uint32_t Channel)
{
  uint8_t iChannel = (((Channel) == (0x1UL << (0U))) ? 0U : ((Channel) == (0x1UL << (2U))) ? 1U : ((Channel) == (0x1UL << (4U))) ? 2U : ((Channel) == (0x1UL << (6U))) ? 3U : ((Channel) == (0x1UL << (8U))) ? 4U : ((Channel) == (0x1UL << (10U))) ? 5U : ((Channel) == (0x1UL << (12U))) ? 6U : ((Channel) == (0x1UL << (16U))) ? 7U : 8U);
  return (((TIMx->CCER) & ((((0x1UL << (3U)) | (0x1UL << (1U))) << SHIFT_TAB_CCxP[iChannel]))) >>
          SHIFT_TAB_CCxP[iChannel]);
}








 
static inline void LL_TIM_IC_EnableXORCombination(TIM_TypeDef *TIMx)
{
  ((TIMx->CR2) |= ((0x1UL << (7U))));
}








 
static inline void LL_TIM_IC_DisableXORCombination(TIM_TypeDef *TIMx)
{
  ((TIMx->CR2) &= ~((0x1UL << (7U))));
}








 
static inline uint32_t LL_TIM_IC_IsEnabledXORCombination(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->CR2) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}











 
static inline uint32_t LL_TIM_IC_GetCaptureCH1(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR1)));
}











 
static inline uint32_t LL_TIM_IC_GetCaptureCH2(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR2)));
}











 
static inline uint32_t LL_TIM_IC_GetCaptureCH3(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR3)));
}











 
static inline uint32_t LL_TIM_IC_GetCaptureCH4(const TIM_TypeDef *TIMx)
{
  return (uint32_t)(((TIMx->CCR4)));
}



 



 








 
static inline void LL_TIM_EnableExternalClock(TIM_TypeDef *TIMx)
{
  ((TIMx->SMCR) |= ((0x1UL << (14U))));
}








 
static inline void LL_TIM_DisableExternalClock(TIM_TypeDef *TIMx)
{
  ((TIMx->SMCR) &= ~((0x1UL << (14U))));
}








 
static inline uint32_t LL_TIM_IsEnabledExternalClock(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SMCR) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}



















 
static inline void LL_TIM_SetClockSource(TIM_TypeDef *TIMx, uint32_t ClockSource)
{
  (((TIMx->SMCR)) = ((((((TIMx->SMCR))) & (~((0x10007UL << (0U)) | (0x1UL << (14U))))) | (ClockSource))));
}












 
static inline void LL_TIM_SetEncoderMode(TIM_TypeDef *TIMx, uint32_t EncoderMode)
{
  (((TIMx->SMCR)) = ((((((TIMx->SMCR))) & (~((0x10007UL << (0U))))) | (EncoderMode))));
}



 



 
















 
static inline void LL_TIM_SetTriggerOutput(TIM_TypeDef *TIMx, uint32_t TimerSynchronization)
{
  (((TIMx->CR2)) = ((((((TIMx->CR2))) & (~((0x7UL << (4U))))) | (TimerSynchronization))));
}

























 
static inline void LL_TIM_SetTriggerOutput2(TIM_TypeDef *TIMx, uint32_t ADCSynchronization)
{
  (((TIMx->CR2)) = ((((((TIMx->CR2))) & (~((0xFUL << (20U))))) | (ADCSynchronization))));
}














 
static inline void LL_TIM_SetSlaveMode(TIM_TypeDef *TIMx, uint32_t SlaveMode)
{
  (((TIMx->SMCR)) = ((((((TIMx->SMCR))) & (~((0x10007UL << (0U))))) | (SlaveMode))));
}

















 
static inline void LL_TIM_SetTriggerInput(TIM_TypeDef *TIMx, uint32_t TriggerInput)
{
  (((TIMx->SMCR)) = ((((((TIMx->SMCR))) & (~((0x7UL << (4U))))) | (TriggerInput))));
}








 
static inline void LL_TIM_EnableMasterSlaveMode(TIM_TypeDef *TIMx)
{
  ((TIMx->SMCR) |= ((0x1UL << (7U))));
}








 
static inline void LL_TIM_DisableMasterSlaveMode(TIM_TypeDef *TIMx)
{
  ((TIMx->SMCR) &= ~((0x1UL << (7U))));
}








 
static inline uint32_t LL_TIM_IsEnabledMasterSlaveMode(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SMCR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}



































 
static inline void LL_TIM_ConfigETR(TIM_TypeDef *TIMx, uint32_t ETRPolarity, uint32_t ETRPrescaler,
                                      uint32_t ETRFilter)
{
  (((TIMx->SMCR)) = ((((((TIMx->SMCR))) & (~((0x1UL << (15U)) | (0x3UL << (12U)) | (0xFUL << (8U))))) | (ETRPolarity | ETRPrescaler | ETRFilter))));
}



 



 







 
static inline void LL_TIM_EnableBRK(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) |= ((0x1UL << (12U))));
}








 
static inline void LL_TIM_DisableBRK(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) &= ~((0x1UL << (12U))));
}





























 
static inline void LL_TIM_ConfigBRK(TIM_TypeDef *TIMx, uint32_t BreakPolarity,
                                      uint32_t BreakFilter)
{
  (((TIMx->BDTR)) = ((((((TIMx->BDTR))) & (~((0x1UL << (13U)) | (0xFUL << (16U))))) | (BreakPolarity | BreakFilter))));
}








 
static inline void LL_TIM_EnableBRK2(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) |= ((0x1UL << (24U))));
}








 
static inline void LL_TIM_DisableBRK2(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) &= ~((0x1UL << (24U))));
}





























 
static inline void LL_TIM_ConfigBRK2(TIM_TypeDef *TIMx, uint32_t Break2Polarity, uint32_t Break2Filter)
{
  (((TIMx->BDTR)) = ((((((TIMx->BDTR))) & (~((0x1UL << (25U)) | (0xFUL << (20U))))) | (Break2Polarity | Break2Filter))));
}















 
static inline void LL_TIM_SetOffStates(TIM_TypeDef *TIMx, uint32_t OffStateIdle, uint32_t OffStateRun)
{
  (((TIMx->BDTR)) = ((((((TIMx->BDTR))) & (~((0x1UL << (10U)) | (0x1UL << (11U))))) | (OffStateIdle | OffStateRun))));
}








 
static inline void LL_TIM_EnableAutomaticOutput(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) |= ((0x1UL << (14U))));
}








 
static inline void LL_TIM_DisableAutomaticOutput(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) &= ~((0x1UL << (14U))));
}








 
static inline uint32_t LL_TIM_IsEnabledAutomaticOutput(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->BDTR) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}










 
static inline void LL_TIM_EnableAllOutputs(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) |= ((0x1UL << (15U))));
}










 
static inline void LL_TIM_DisableAllOutputs(TIM_TypeDef *TIMx)
{
  ((TIMx->BDTR) &= ~((0x1UL << (15U))));
}








 
static inline uint32_t LL_TIM_IsEnabledAllOutputs(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->BDTR) & ((0x1UL << (15U)))) == ((0x1UL << (15U)))) ? 1UL : 0UL);
}



 



 





















































 
static inline void LL_TIM_ConfigDMABurst(TIM_TypeDef *TIMx, uint32_t DMABurstBaseAddress, uint32_t DMABurstLength)
{
  (((TIMx->DCR)) = ((((((TIMx->DCR))) & (~(((0x1FUL << (8U)) | (0x1FUL << (0U)))))) | ((DMABurstBaseAddress | DMABurstLength)))));
}



 



 





































 
static inline void LL_TIM_SetRemap(TIM_TypeDef *TIMx, uint32_t Remap)
{
  (((TIMx->OR)) = ((((((TIMx->OR))) & (~((Remap >> 16U)))) | ((Remap & 0x0000FFFFU)))));
}



 



 





 
static inline void LL_TIM_ClearFlag_UPDATE(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (0U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_UPDATE(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC1(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (1U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_CC1(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC2(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (2U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_CC2(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC3(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (3U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_CC3(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC4(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (4U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_CC4(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC5(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (16U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_CC5(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (16U)))) == ((0x1UL << (16U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC6(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (17U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_CC6(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (17U)))) == ((0x1UL << (17U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_COM(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (5U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_COM(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_TRIG(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (6U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_TRIG(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_BRK(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (7U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_BRK(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_BRK2(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (8U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_BRK2(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC1OVR(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (9U)))));
}







 
static inline uint32_t LL_TIM_IsActiveFlag_CC1OVR(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC2OVR(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (10U)))));
}







 
static inline uint32_t LL_TIM_IsActiveFlag_CC2OVR(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC3OVR(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (11U)))));
}







 
static inline uint32_t LL_TIM_IsActiveFlag_CC3OVR(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (11U)))) == ((0x1UL << (11U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_CC4OVR(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (12U)))));
}







 
static inline uint32_t LL_TIM_IsActiveFlag_CC4OVR(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (12U)))) == ((0x1UL << (12U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_ClearFlag_SYSBRK(TIM_TypeDef *TIMx)
{
  ((TIMx->SR) = (~((0x1UL << (13U)))));
}






 
static inline uint32_t LL_TIM_IsActiveFlag_SYSBRK(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->SR) & ((0x1UL << (13U)))) == ((0x1UL << (13U)))) ? 1UL : 0UL);
}



 



 





 
static inline void LL_TIM_EnableIT_UPDATE(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (0U))));
}






 
static inline void LL_TIM_DisableIT_UPDATE(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (0U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_UPDATE(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_CC1(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (1U))));
}






 
static inline void LL_TIM_DisableIT_CC1(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (1U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_CC1(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_CC2(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (2U))));
}






 
static inline void LL_TIM_DisableIT_CC2(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (2U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_CC2(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_CC3(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (3U))));
}






 
static inline void LL_TIM_DisableIT_CC3(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (3U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_CC3(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_CC4(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (4U))));
}






 
static inline void LL_TIM_DisableIT_CC4(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (4U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_CC4(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_COM(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (5U))));
}






 
static inline void LL_TIM_DisableIT_COM(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (5U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_COM(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_TRIG(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (6U))));
}






 
static inline void LL_TIM_DisableIT_TRIG(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (6U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_TRIG(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableIT_BRK(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (7U))));
}






 
static inline void LL_TIM_DisableIT_BRK(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (7U))));
}






 
static inline uint32_t LL_TIM_IsEnabledIT_BRK(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}



 



 





 
static inline void LL_TIM_EnableDMAReq_UPDATE(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (8U))));
}






 
static inline void LL_TIM_DisableDMAReq_UPDATE(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (8U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_UPDATE(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableDMAReq_CC1(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (9U))));
}






 
static inline void LL_TIM_DisableDMAReq_CC1(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (9U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_CC1(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableDMAReq_CC2(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (10U))));
}






 
static inline void LL_TIM_DisableDMAReq_CC2(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (10U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_CC2(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableDMAReq_CC3(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (11U))));
}






 
static inline void LL_TIM_DisableDMAReq_CC3(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (11U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_CC3(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (11U)))) == ((0x1UL << (11U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableDMAReq_CC4(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (12U))));
}






 
static inline void LL_TIM_DisableDMAReq_CC4(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (12U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_CC4(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (12U)))) == ((0x1UL << (12U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableDMAReq_COM(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (13U))));
}






 
static inline void LL_TIM_DisableDMAReq_COM(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (13U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_COM(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (13U)))) == ((0x1UL << (13U)))) ? 1UL : 0UL);
}






 
static inline void LL_TIM_EnableDMAReq_TRIG(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) |= ((0x1UL << (14U))));
}






 
static inline void LL_TIM_DisableDMAReq_TRIG(TIM_TypeDef *TIMx)
{
  ((TIMx->DIER) &= ~((0x1UL << (14U))));
}






 
static inline uint32_t LL_TIM_IsEnabledDMAReq_TRIG(const TIM_TypeDef *TIMx)
{
  return ((((TIMx->DIER) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}



 



 





 
static inline void LL_TIM_GenerateEvent_UPDATE(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (0U))));
}






 
static inline void LL_TIM_GenerateEvent_CC1(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (1U))));
}






 
static inline void LL_TIM_GenerateEvent_CC2(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (2U))));
}






 
static inline void LL_TIM_GenerateEvent_CC3(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (3U))));
}






 
static inline void LL_TIM_GenerateEvent_CC4(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (4U))));
}






 
static inline void LL_TIM_GenerateEvent_COM(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (5U))));
}






 
static inline void LL_TIM_GenerateEvent_TRIG(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (6U))));
}






 
static inline void LL_TIM_GenerateEvent_BRK(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (7U))));
}






 
static inline void LL_TIM_GenerateEvent_BRK2(TIM_TypeDef *TIMx)
{
  ((TIMx->EGR) |= ((0x1UL << (8U))));
}



 



 

ErrorStatus LL_TIM_DeInit(const TIM_TypeDef *TIMx);
void LL_TIM_StructInit(LL_TIM_InitTypeDef *TIM_InitStruct);
ErrorStatus LL_TIM_Init(TIM_TypeDef *TIMx, const LL_TIM_InitTypeDef *TIM_InitStruct);
void LL_TIM_OC_StructInit(LL_TIM_OC_InitTypeDef *TIM_OC_InitStruct);
ErrorStatus LL_TIM_OC_Init(TIM_TypeDef *TIMx, uint32_t Channel, const LL_TIM_OC_InitTypeDef *TIM_OC_InitStruct);
void LL_TIM_IC_StructInit(LL_TIM_IC_InitTypeDef *TIM_ICInitStruct);
ErrorStatus LL_TIM_IC_Init(TIM_TypeDef *TIMx, uint32_t Channel, const LL_TIM_IC_InitTypeDef *TIM_IC_InitStruct);
void LL_TIM_ENCODER_StructInit(LL_TIM_ENCODER_InitTypeDef *TIM_EncoderInitStruct);
ErrorStatus LL_TIM_ENCODER_Init(TIM_TypeDef *TIMx, const LL_TIM_ENCODER_InitTypeDef *TIM_EncoderInitStruct);
void LL_TIM_HALLSENSOR_StructInit(LL_TIM_HALLSENSOR_InitTypeDef *TIM_HallSensorInitStruct);
ErrorStatus LL_TIM_HALLSENSOR_Init(TIM_TypeDef *TIMx, const LL_TIM_HALLSENSOR_InitTypeDef *TIM_HallSensorInitStruct);
void LL_TIM_BDTR_StructInit(LL_TIM_BDTR_InitTypeDef *TIM_BDTRInitStruct);
ErrorStatus LL_TIM_BDTR_Init(TIM_TypeDef *TIMx, const LL_TIM_BDTR_InitTypeDef *TIM_BDTRInitStruct);


 



 



 




 


































 

 


 



 




 

 
 
 
 
 
 


 



 


 



 


 



 


 



 


 



 


 



 

 
 


 



 





















































 
static inline void LL_AHB1_GRP1_EnableClock(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) & (Periphs));
  (void)tmpreg;
}





















































 
static inline uint32_t LL_AHB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) & (Periphs)) == Periphs);
}





















































 
static inline void LL_AHB1_GRP1_DisableClock(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1ENR) &= ~(Periphs));
}










































 
static inline void LL_AHB1_GRP1_ForceReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1RSTR) |= (Periphs));
}










































 
static inline void LL_AHB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1RSTR) &= ~(Periphs));
}





























































 
static inline void LL_AHB1_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1LPENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1LPENR) & (Periphs));
  (void)tmpreg;
}





























































 
static inline void LL_AHB1_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB1LPENR) &= ~(Periphs));
}



 



 





















 
static inline void LL_AHB2_GRP1_EnableClock(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2ENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2ENR) & (Periphs));
  (void)tmpreg;
}





















 
static inline uint32_t LL_AHB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2ENR) & (Periphs)) == Periphs);
}





















 
static inline void LL_AHB2_GRP1_DisableClock(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2ENR) &= ~(Periphs));
}






















 
static inline void LL_AHB2_GRP1_ForceReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2RSTR) |= (Periphs));
}






















 
static inline void LL_AHB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2RSTR) &= ~(Periphs));
}





















 
static inline void LL_AHB2_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2LPENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2LPENR) & (Periphs));
  (void)tmpreg;
}





















 
static inline void LL_AHB2_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB2LPENR) &= ~(Periphs));
}



 



 











 
static inline void LL_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3ENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3ENR) & (Periphs));
  (void)tmpreg;
}











 
static inline uint32_t LL_AHB3_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3ENR) & (Periphs)) == Periphs);
}











 
static inline void LL_AHB3_GRP1_DisableClock(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3ENR) &= ~(Periphs));
}












 
static inline void LL_AHB3_GRP1_ForceReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3RSTR) |= (Periphs));
}












 
static inline void LL_AHB3_GRP1_ReleaseReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3RSTR) &= ~(Periphs));
}











 
static inline void LL_AHB3_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3LPENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3LPENR) & (Periphs));
  (void)tmpreg;
}











 
static inline void LL_AHB3_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->AHB3LPENR) &= ~(Periphs));
}



 



 





































































 
static inline void LL_APB1_GRP1_EnableClock(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1ENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1ENR) & (Periphs));
  (void)tmpreg;
}





































































 
static inline uint32_t LL_APB1_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1ENR) & (Periphs)) == Periphs);
}





































































 
static inline void LL_APB1_GRP1_DisableClock(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1ENR) &= ~(Periphs));
}



































































 
static inline void LL_APB1_GRP1_ForceReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1RSTR) |= (Periphs));
}



































































 
static inline void LL_APB1_GRP1_ReleaseReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1RSTR) &= ~(Periphs));
}





































































 
static inline void LL_APB1_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1LPENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1LPENR) & (Periphs));
  (void)tmpreg;
}





































































 
static inline void LL_APB1_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB1LPENR) &= ~(Periphs));
}



 



 























































 
static inline void LL_APB2_GRP1_EnableClock(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2ENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2ENR) & (Periphs));
  (void)tmpreg;
}























































 
static inline uint32_t LL_APB2_GRP1_IsEnabledClock(uint32_t Periphs)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2ENR) & (Periphs)) == Periphs);
}























































 
static inline void LL_APB2_GRP1_DisableClock(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2ENR) &= ~(Periphs));
}




















































 
static inline void LL_APB2_GRP1_ForceReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2RSTR) |= (Periphs));
}




















































 
static inline void LL_APB2_GRP1_ReleaseReset(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2RSTR) &= ~(Periphs));
}





















































 
static inline void LL_APB2_GRP1_EnableClockLowPower(uint32_t Periphs)
{
  volatile uint32_t tmpreg;
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2LPENR) |= (Periphs));
   
  tmpreg = ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2LPENR) & (Periphs));
  (void)tmpreg;
}





















































 
static inline void LL_APB2_GRP1_DisableClockLowPower(uint32_t Periphs)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3800UL))->APB2LPENR) &= ~(Periphs));
}



 



 



 




 
































 

 


 



 




 

 
 

 


 



 

 

 
 


 



 


 






 


 



 


 



 


 



 


 


 


 



 


 



 


 



 


 



 


 



 

 

 


 



 







 
static inline void LL_SYSCFG_EnableFMCMemorySwapping(void)
{
  ((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->MEMRMP) |= ((0x1UL << (10U))));
}







 
static inline void LL_SYSCFG_DisableFMCMemorySwapping(void)
{
  ((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->MEMRMP) &= ~((0x3UL << (10U))));
}







 
static inline void LL_SYSCFG_EnableCompensationCell(void)
{
  ((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->CMPCR) |= ((0x1UL << (0U))));
}







 
static inline void LL_SYSCFG_DisableCompensationCell(void)
{
  ((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->CMPCR) &= ~((0x1UL << (0U))));
}





 
static inline uint32_t LL_SYSCFG_IsActiveFlag_CMPCR(void)
{
  return (((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->CMPCR) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}










 
static inline uint32_t LL_SYSCFG_GetRemapMemoryBoot(void)
{
  return (uint32_t)(((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->MEMRMP) & ((0x1UL << (0U)))));
}




















 
static inline void LL_SYSCFG_EnableFastModePlus(uint32_t ConfigFastModePlus)
{
  ((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->PMC) |= (ConfigFastModePlus));
}
















 
static inline void LL_SYSCFG_DisableFastModePlus(uint32_t ConfigFastModePlus)
{
  ((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->PMC) &= ~(ConfigFastModePlus));
}








































 
static inline void LL_SYSCFG_SetEXTISource(uint32_t Port, uint32_t Line)
{
  (((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->EXTICR[Line & 0xFFU])) = ((((((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->EXTICR[Line & 0xFFU]))) & (~((Line >> 16U)))) | (Port << (__iar_builtin_CLZ(__iar_builtin_RBIT((Line >> 16U))))))));
}





































 
static inline uint32_t LL_SYSCFG_GetEXTISource(uint32_t Line)
{
  return (uint32_t)(((((SYSCFG_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3800UL))->EXTICR[Line & 0xFFU]) & ((Line >> 16U))) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(Line >> 16U))));
}




 



 








 
static inline uint32_t LL_DBGMCU_GetDeviceID(void)
{
  return (uint32_t)(((((DBGMCU_TypeDef *) 0xE0042000UL)->IDCODE) & ((0xFFFUL << (0U)))));
}







 
static inline uint32_t LL_DBGMCU_GetRevisionID(void)
{
  return (uint32_t)(((((DBGMCU_TypeDef *) 0xE0042000UL)->IDCODE) & ((0xFFFFUL << (16U)))) >> (16U));
}





 
static inline void LL_DBGMCU_EnableDBGSleepMode(void)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) |= ((0x1UL << (0U))));
}





 
static inline void LL_DBGMCU_DisableDBGSleepMode(void)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) &= ~((0x1UL << (0U))));
}





 
static inline void LL_DBGMCU_EnableDBGStopMode(void)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) |= ((0x1UL << (1U))));
}





 
static inline void LL_DBGMCU_DisableDBGStopMode(void)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) &= ~((0x1UL << (1U))));
}





 
static inline void LL_DBGMCU_EnableDBGStandbyMode(void)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) |= ((0x1UL << (2U))));
}





 
static inline void LL_DBGMCU_DisableDBGStandbyMode(void)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) &= ~((0x1UL << (2U))));
}












 
static inline void LL_DBGMCU_SetTracePinAssignment(uint32_t PinAssignment)
{
  (((((DBGMCU_TypeDef *) 0xE0042000UL)->CR)) = ((((((((DBGMCU_TypeDef *) 0xE0042000UL)->CR))) & (~((0x1UL << (5U)) | (0x3UL << (6U))))) | (PinAssignment))));
}











 
static inline uint32_t LL_DBGMCU_GetTracePinAssignment(void)
{
  return (uint32_t)(((((DBGMCU_TypeDef *) 0xE0042000UL)->CR) & ((0x1UL << (5U)) | (0x3UL << (6U)))));
}















































 
static inline void LL_DBGMCU_APB1_GRP1_FreezePeriph(uint32_t Periphs)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->APB1FZ) |= (Periphs));
}














































 
static inline void LL_DBGMCU_APB1_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->APB1FZ) &= ~(Periphs));
}

















 
static inline void LL_DBGMCU_APB2_GRP1_FreezePeriph(uint32_t Periphs)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->APB2FZ) |= (Periphs));
}

















 
static inline void LL_DBGMCU_APB2_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  ((((DBGMCU_TypeDef *) 0xE0042000UL)->APB2FZ) &= ~(Periphs));
}


 



 






















 
static inline void LL_FLASH_SetLatency(uint32_t Latency)
{
  (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR)) = ((((((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR))) & (~((0xFUL << (0U))))) | (Latency))));
}





















 
static inline uint32_t LL_FLASH_GetLatency(void)
{
  return (uint32_t)(((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) & ((0xFUL << (0U)))));
}





 
static inline void LL_FLASH_EnablePrefetch(void)
{
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) |= ((0x1UL << (8U))));
}





 
static inline void LL_FLASH_DisablePrefetch(void)
{
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) &= ~((0x1UL << (8U))));
}





 
static inline uint32_t LL_FLASH_IsPrefetchEnabled(void)
{
  return (((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}







 
static inline void LL_FLASH_EnableART(void)
{
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) |= ((0x1UL << (9U))));
}





 
static inline void LL_FLASH_DisableART(void)
{
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) &= ~((0x1UL << (9U))));
}





 
static inline void LL_FLASH_EnableARTReset(void)
{
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) |= ((0x1UL << (11U))));
}





 
static inline void LL_FLASH_DisableARTReset(void)
{
  ((((FLASH_TypeDef *) ((0x40000000UL + 0x00020000UL) + 0x3C00UL))->ACR) &= ~((0x1UL << (11U))));
}



 



 



 




 




















 

 


 



 




 

 
 
 
 


 


 
 


 
typedef struct
{

  uint32_t Line_0_31;           
 

  FunctionalState LineCommand;  
 

  uint8_t Mode;                 
 

  uint8_t Trigger;              
 
} LL_EXTI_InitTypeDef;



 

 


 



 






 



 


 



 



 






 

 


 



 






 





 


 




 



 


 


 





































 
static inline void LL_EXTI_EnableIT_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->IMR) |= (ExtiLine));
}





































 
static inline void LL_EXTI_DisableIT_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->IMR) &= ~(ExtiLine));
}






































 
static inline uint32_t LL_EXTI_IsEnabledIT_0_31(uint32_t ExtiLine)
{
  return (((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->IMR) & (ExtiLine)) == (ExtiLine));
}




 



 


































 
static inline void LL_EXTI_EnableEvent_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->EMR) |= (ExtiLine));

}



































 
static inline void LL_EXTI_DisableEvent_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->EMR) &= ~(ExtiLine));
}



































 
static inline uint32_t LL_EXTI_IsEnabledEvent_0_31(uint32_t ExtiLine)
{
  return (((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->EMR) & (ExtiLine)) == (ExtiLine));

}




 



 




































 
static inline void LL_EXTI_EnableRisingTrig_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->RTSR) |= (ExtiLine));

}





































 
static inline void LL_EXTI_DisableRisingTrig_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->RTSR) &= ~(ExtiLine));

}






























 
static inline uint32_t LL_EXTI_IsEnabledRisingTrig_0_31(uint32_t ExtiLine)
{
  return (((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->RTSR) & (ExtiLine)) == (ExtiLine));
}




 



 




































 
static inline void LL_EXTI_EnableFallingTrig_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->FTSR) |= (ExtiLine));
}




































 
static inline void LL_EXTI_DisableFallingTrig_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->FTSR) &= ~(ExtiLine));
}






























 
static inline uint32_t LL_EXTI_IsEnabledFallingTrig_0_31(uint32_t ExtiLine)
{
  return (((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->FTSR) & (ExtiLine)) == (ExtiLine));
}




 



 


































 
static inline void LL_EXTI_GenerateSWI_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->SWIER) |= (ExtiLine));
}




 



 































 
static inline uint32_t LL_EXTI_IsActiveFlag_0_31(uint32_t ExtiLine)
{
  return (((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->PR) & (ExtiLine)) == (ExtiLine));
}
































 
static inline uint32_t LL_EXTI_ReadFlag_0_31(uint32_t ExtiLine)
{
  return (uint32_t)(((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->PR) & (ExtiLine)));
}
































 
static inline void LL_EXTI_ClearFlag_0_31(uint32_t ExtiLine)
{
  ((((EXTI_TypeDef *) ((0x40000000UL + 0x00010000UL) + 0x3C00UL))->PR) = (ExtiLine));
}




 



 

uint32_t LL_EXTI_Init(LL_EXTI_InitTypeDef *EXTI_InitStruct);
uint32_t LL_EXTI_DeInit(void);
void LL_EXTI_StructInit(LL_EXTI_InitTypeDef *EXTI_InitStruct);




 



 



 




 



















 

 


 



 




 

 
 
 

 


 



 
typedef struct
{
  uint32_t TransferDirection;       



 

  uint32_t Mode;                    



 

  uint32_t DataWidth;               



 

  uint32_t ClockPolarity;           



 

  uint32_t ClockPhase;              



 

  uint32_t NSS;                     




 

  uint32_t BaudRate;                






 

  uint32_t BitOrder;                



 

  uint32_t CRCCalculation;          



 

  uint32_t CRCPoly;                 



 

} LL_SPI_InitTypeDef;



 

 


 




 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 

 


 



 







 






 


 



 

 


 



 






 
static inline void LL_SPI_Enable(SPI_TypeDef *SPIx)
{
  ((SPIx->CR1) |= ((0x1UL << (6U))));
}







 
static inline void LL_SPI_Disable(SPI_TypeDef *SPIx)
{
  ((SPIx->CR1) &= ~((0x1UL << (6U))));
}






 
static inline uint32_t LL_SPI_IsEnabled(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR1) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}











 
static inline void LL_SPI_SetMode(SPI_TypeDef *SPIx, uint32_t Mode)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (2U)) | (0x1UL << (8U))))) | (Mode))));
}









 
static inline uint32_t LL_SPI_GetMode(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x1UL << (2U)) | (0x1UL << (8U)))));
}










 
static inline void LL_SPI_SetStandard(SPI_TypeDef *SPIx, uint32_t Standard)
{
  (((SPIx->CR2)) = ((((((SPIx->CR2))) & (~((0x1UL << (4U))))) | (Standard))));
}








 
static inline uint32_t LL_SPI_GetStandard(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR2) & ((0x1UL << (4U)))));
}











 
static inline void LL_SPI_SetClockPhase(SPI_TypeDef *SPIx, uint32_t ClockPhase)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (0U))))) | (ClockPhase))));
}








 
static inline uint32_t LL_SPI_GetClockPhase(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x1UL << (0U)))));
}











 
static inline void LL_SPI_SetClockPolarity(SPI_TypeDef *SPIx, uint32_t ClockPolarity)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (1U))))) | (ClockPolarity))));
}








 
static inline uint32_t LL_SPI_GetClockPolarity(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x1UL << (1U)))));
}
















 
static inline void LL_SPI_SetBaudRatePrescaler(SPI_TypeDef *SPIx, uint32_t BaudRate)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x7UL << (3U))))) | (BaudRate))));
}














 
static inline uint32_t LL_SPI_GetBaudRatePrescaler(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x7UL << (3U)))));
}










 
static inline void LL_SPI_SetTransferBitOrder(SPI_TypeDef *SPIx, uint32_t BitOrder)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (7U))))) | (BitOrder))));
}








 
static inline uint32_t LL_SPI_GetTransferBitOrder(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x1UL << (7U)))));
}















 
static inline void LL_SPI_SetTransferDirection(SPI_TypeDef *SPIx, uint32_t TransferDirection)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (10U)) | (0x1UL << (15U)) | (0x1UL << (14U))))) | (TransferDirection))));
}












 
static inline uint32_t LL_SPI_GetTransferDirection(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x1UL << (10U)) | (0x1UL << (15U)) | (0x1UL << (14U)))));
}




















 
static inline void LL_SPI_SetDataWidth(SPI_TypeDef *SPIx, uint32_t DataWidth)
{
  (((SPIx->CR2)) = ((((((SPIx->CR2))) & (~((0xFUL << (8U))))) | (DataWidth))));
}



















 
static inline uint32_t LL_SPI_GetDataWidth(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR2) & ((0xFUL << (8U)))));
}









 
static inline void LL_SPI_SetRxFIFOThreshold(SPI_TypeDef *SPIx, uint32_t Threshold)
{
  (((SPIx->CR2)) = ((((((SPIx->CR2))) & (~((0x1UL << (12U))))) | (Threshold))));
}








 
static inline uint32_t LL_SPI_GetRxFIFOThreshold(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR2) & ((0x1UL << (12U)))));
}



 



 







 
static inline void LL_SPI_EnableCRC(SPI_TypeDef *SPIx)
{
  ((SPIx->CR1) |= ((0x1UL << (13U))));
}







 
static inline void LL_SPI_DisableCRC(SPI_TypeDef *SPIx)
{
  ((SPIx->CR1) &= ~((0x1UL << (13U))));
}







 
static inline uint32_t LL_SPI_IsEnabledCRC(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR1) & ((0x1UL << (13U)))) == ((0x1UL << (13U)))) ? 1UL : 0UL);
}










 
static inline void LL_SPI_SetCRCWidth(SPI_TypeDef *SPIx, uint32_t CRCLength)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (11U))))) | (CRCLength))));
}








 
static inline uint32_t LL_SPI_GetCRCWidth(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR1) & ((0x1UL << (11U)))));
}







 
static inline void LL_SPI_SetCRCNext(SPI_TypeDef *SPIx)
{
  ((SPIx->CR1) |= ((0x1UL << (12U))));
}







 
static inline void LL_SPI_SetCRCPolynomial(SPI_TypeDef *SPIx, uint32_t CRCPoly)
{
  ((SPIx->CRCPR) = ((uint16_t)CRCPoly));
}






 
static inline uint32_t LL_SPI_GetCRCPolynomial(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CRCPR)));
}






 
static inline uint32_t LL_SPI_GetRxCRC(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->RXCRCR)));
}






 
static inline uint32_t LL_SPI_GetTxCRC(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->TXCRCR)));
}



 



 












 
static inline void LL_SPI_SetNSSMode(SPI_TypeDef *SPIx, uint32_t NSS)
{
  (((SPIx->CR1)) = ((((((SPIx->CR1))) & (~((0x1UL << (9U))))) | (NSS))));
  (((SPIx->CR2)) = ((((((SPIx->CR2))) & (~((0x1UL << (2U))))) | (((uint32_t)(NSS >> 16U))))));
}










 
static inline uint32_t LL_SPI_GetNSSMode(const SPI_TypeDef *SPIx)
{
  uint32_t Ssm  = (((SPIx->CR1) & ((0x1UL << (9U)))));
  uint32_t Ssoe = (((SPIx->CR2) & ((0x1UL << (2U)))) << 16U);
  return (Ssm | Ssoe);
}







 
static inline void LL_SPI_EnableNSSPulseMgt(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) |= ((0x1UL << (3U))));
}







 
static inline void LL_SPI_DisableNSSPulseMgt(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) &= ~((0x1UL << (3U))));
}







 
static inline uint32_t LL_SPI_IsEnabledNSSPulse(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR2) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}



 



 






 
static inline uint32_t LL_SPI_IsActiveFlag_RXNE(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsActiveFlag_TXE(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsActiveFlag_CRCERR(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsActiveFlag_MODF(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsActiveFlag_OVR(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}













 
static inline uint32_t LL_SPI_IsActiveFlag_BSY(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsActiveFlag_FRE(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}










 
static inline uint32_t LL_SPI_GetRxFIFOLevel(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->SR) & ((0x3UL << (9U)))));
}










 
static inline uint32_t LL_SPI_GetTxFIFOLevel(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->SR) & ((0x3UL << (11U)))));
}






 
static inline void LL_SPI_ClearFlag_CRCERR(SPI_TypeDef *SPIx)
{
  ((SPIx->SR) &= ~((0x1UL << (4U))));
}








 
static inline void LL_SPI_ClearFlag_MODF(SPI_TypeDef *SPIx)
{
  volatile uint32_t tmpreg_sr;
  tmpreg_sr = SPIx->SR;
  (void) tmpreg_sr;
  ((SPIx->CR1) &= ~((0x1UL << (6U))));
}








 
static inline void LL_SPI_ClearFlag_OVR(const SPI_TypeDef *SPIx)
{
  volatile uint32_t tmpreg;
  tmpreg = SPIx->DR;
  (void) tmpreg;
  tmpreg = SPIx->SR;
  (void) tmpreg;
}







 
static inline void LL_SPI_ClearFlag_FRE(const SPI_TypeDef *SPIx)
{
  volatile uint32_t tmpreg;
  tmpreg = SPIx->SR;
  (void) tmpreg;
}



 



 








 
static inline void LL_SPI_EnableIT_ERR(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) |= ((0x1UL << (5U))));
}






 
static inline void LL_SPI_EnableIT_RXNE(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) |= ((0x1UL << (6U))));
}






 
static inline void LL_SPI_EnableIT_TXE(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) |= ((0x1UL << (7U))));
}








 
static inline void LL_SPI_DisableIT_ERR(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) &= ~((0x1UL << (5U))));
}






 
static inline void LL_SPI_DisableIT_RXNE(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) &= ~((0x1UL << (6U))));
}






 
static inline void LL_SPI_DisableIT_TXE(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) &= ~((0x1UL << (7U))));
}






 
static inline uint32_t LL_SPI_IsEnabledIT_ERR(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR2) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsEnabledIT_RXNE(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR2) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_SPI_IsEnabledIT_TXE(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR2) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}



 



 






 
static inline void LL_SPI_EnableDMAReq_RX(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) |= ((0x1UL << (0U))));
}






 
static inline void LL_SPI_DisableDMAReq_RX(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) &= ~((0x1UL << (0U))));
}






 
static inline uint32_t LL_SPI_IsEnabledDMAReq_RX(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR2) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline void LL_SPI_EnableDMAReq_TX(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) |= ((0x1UL << (1U))));
}






 
static inline void LL_SPI_DisableDMAReq_TX(SPI_TypeDef *SPIx)
{
  ((SPIx->CR2) &= ~((0x1UL << (1U))));
}






 
static inline uint32_t LL_SPI_IsEnabledDMAReq_TX(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->CR2) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}









 
static inline void LL_SPI_SetDMAParity_RX(SPI_TypeDef *SPIx, uint32_t Parity)
{
  (((SPIx->CR2)) = ((((((SPIx->CR2))) & (~((0x1UL << (13U))))) | ((Parity << (13U))))));
}








 
static inline uint32_t LL_SPI_GetDMAParity_RX(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR2) & ((0x1UL << (13U)))) >> (13U));
}









 
static inline void LL_SPI_SetDMAParity_TX(SPI_TypeDef *SPIx, uint32_t Parity)
{
  (((SPIx->CR2)) = ((((((SPIx->CR2))) & (~((0x1UL << (14U))))) | ((Parity << (14U))))));
}








 
static inline uint32_t LL_SPI_GetDMAParity_TX(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->CR2) & ((0x1UL << (14U)))) >> (14U));
}






 
static inline uint32_t LL_SPI_DMA_GetRegAddr(const SPI_TypeDef *SPIx)
{
  return (uint32_t) &(SPIx->DR);
}



 



 






 
static inline uint8_t LL_SPI_ReceiveData8(SPI_TypeDef *SPIx)
{
  return (*((volatile uint8_t *)&SPIx->DR));
}






 
static inline uint16_t LL_SPI_ReceiveData16(SPI_TypeDef *SPIx)
{
  return (uint16_t)(((SPIx->DR)));
}







 
static inline void LL_SPI_TransmitData8(SPI_TypeDef *SPIx, uint8_t TxData)
{
  *((volatile uint8_t *)&SPIx->DR) = TxData;
}







 
static inline void LL_SPI_TransmitData16(SPI_TypeDef *SPIx, uint16_t TxData)
{
  SPIx->DR = TxData;
}



 


 

ErrorStatus LL_SPI_DeInit(const SPI_TypeDef *SPIx);
ErrorStatus LL_SPI_Init(SPI_TypeDef *SPIx, LL_SPI_InitTypeDef *SPI_InitStruct);
void        LL_SPI_StructInit(LL_SPI_InitTypeDef *SPI_InitStruct);



 


 



 



 

 
 
 

 


 



 

typedef struct
{
  uint32_t Mode;                    


 

  uint32_t Standard;                


 


  uint32_t DataFormat;              


 


  uint32_t MCLKOutput;              


 


  uint32_t AudioFreq;               



 


  uint32_t ClockPolarity;           


 

} LL_I2S_InitTypeDef;



 

 


 




 


 




 


 



 


 



 


 



 


 



 


 



 


 




 


 



 



 



 

 


 



 







 






 


 



 


 



 



 







 
static inline void LL_I2S_Enable(SPI_TypeDef *SPIx)
{
  ((SPIx->I2SCFGR) |= ((0x1UL << (11U)) | (0x1UL << (10U))));
}






 
static inline void LL_I2S_Disable(SPI_TypeDef *SPIx)
{
  ((SPIx->I2SCFGR) &= ~((0x1UL << (11U)) | (0x1UL << (10U))));
}






 
static inline uint32_t LL_I2S_IsEnabled(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->I2SCFGR) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}












 
static inline void LL_I2S_SetDataFormat(SPI_TypeDef *SPIx, uint32_t DataFormat)
{
  (((SPIx->I2SCFGR)) = ((((((SPIx->I2SCFGR))) & (~((0x3UL << (1U)) | (0x1UL << (0U))))) | (DataFormat))));
}











 
static inline uint32_t LL_I2S_GetDataFormat(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->I2SCFGR) & ((0x3UL << (1U)) | (0x1UL << (0U)))));
}









 
static inline void LL_I2S_SetClockPolarity(SPI_TypeDef *SPIx, uint32_t ClockPolarity)
{
  ((SPIx->I2SCFGR) |= (ClockPolarity));
}








 
static inline uint32_t LL_I2S_GetClockPolarity(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->I2SCFGR) & ((0x1UL << (3U)))));
}













 
static inline void LL_I2S_SetStandard(SPI_TypeDef *SPIx, uint32_t Standard)
{
  (((SPIx->I2SCFGR)) = ((((((SPIx->I2SCFGR))) & (~((0x3UL << (4U)) | (0x1UL << (7U))))) | (Standard))));
}












 
static inline uint32_t LL_I2S_GetStandard(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->I2SCFGR) & ((0x3UL << (4U)) | (0x1UL << (7U)))));
}











 
static inline void LL_I2S_SetTransferMode(SPI_TypeDef *SPIx, uint32_t Mode)
{
  (((SPIx->I2SCFGR)) = ((((((SPIx->I2SCFGR))) & (~((0x3UL << (8U))))) | (Mode))));
}










 
static inline uint32_t LL_I2S_GetTransferMode(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->I2SCFGR) & ((0x3UL << (8U)))));
}







 
static inline void LL_I2S_SetPrescalerLinear(SPI_TypeDef *SPIx, uint8_t PrescalerLinear)
{
  (((SPIx->I2SPR)) = ((((((SPIx->I2SPR))) & (~((0xFFUL << (0U))))) | (PrescalerLinear))));
}






 
static inline uint32_t LL_I2S_GetPrescalerLinear(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->I2SPR) & ((0xFFUL << (0U)))));
}









 
static inline void LL_I2S_SetPrescalerParity(SPI_TypeDef *SPIx, uint32_t PrescalerParity)
{
  (((SPIx->I2SPR)) = ((((((SPIx->I2SPR))) & (~((0x1UL << (8U))))) | (PrescalerParity << 8U))));
}








 
static inline uint32_t LL_I2S_GetPrescalerParity(const SPI_TypeDef *SPIx)
{
  return (uint32_t)(((SPIx->I2SPR) & ((0x1UL << (8U)))) >> 8U);
}






 
static inline void LL_I2S_EnableMasterClock(SPI_TypeDef *SPIx)
{
  ((SPIx->I2SPR) |= ((0x1UL << (9U))));
}






 
static inline void LL_I2S_DisableMasterClock(SPI_TypeDef *SPIx)
{
  ((SPIx->I2SPR) &= ~((0x1UL << (9U))));
}






 
static inline uint32_t LL_I2S_IsEnabledMasterClock(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->I2SPR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}






 
static inline void LL_I2S_EnableAsyncStart(SPI_TypeDef *SPIx)
{
  ((SPIx->I2SCFGR) |= ((0x1UL << (12U))));
}






 
static inline void LL_I2S_DisableAsyncStart(SPI_TypeDef *SPIx)
{
  ((SPIx->I2SCFGR) &= ~((0x1UL << (12U))));
}






 
static inline uint32_t LL_I2S_IsEnabledAsyncStart(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->I2SCFGR) & ((0x1UL << (12U)))) == ((0x1UL << (12U)))) ? 1UL : 0UL);
}



 



 






 
static inline uint32_t LL_I2S_IsActiveFlag_RXNE(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsActiveFlag_RXNE(SPIx);
}






 
static inline uint32_t LL_I2S_IsActiveFlag_TXE(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsActiveFlag_TXE(SPIx);
}






 
static inline uint32_t LL_I2S_IsActiveFlag_BSY(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsActiveFlag_BSY(SPIx);
}






 
static inline uint32_t LL_I2S_IsActiveFlag_OVR(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsActiveFlag_OVR(SPIx);
}






 
static inline uint32_t LL_I2S_IsActiveFlag_UDR(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_I2S_IsActiveFlag_FRE(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsActiveFlag_FRE(SPIx);
}









 
static inline uint32_t LL_I2S_IsActiveFlag_CHSIDE(const SPI_TypeDef *SPIx)
{
  return ((((SPIx->SR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}






 
static inline void LL_I2S_ClearFlag_OVR(SPI_TypeDef *SPIx)
{
  LL_SPI_ClearFlag_OVR(SPIx);
}






 
static inline void LL_I2S_ClearFlag_UDR(const SPI_TypeDef *SPIx)
{
  volatile uint32_t tmpreg;
  tmpreg = SPIx->SR;
  (void)tmpreg;
}






 
static inline void LL_I2S_ClearFlag_FRE(const SPI_TypeDef *SPIx)
{
  LL_SPI_ClearFlag_FRE(SPIx);
}



 



 







 
static inline void LL_I2S_EnableIT_ERR(SPI_TypeDef *SPIx)
{
  LL_SPI_EnableIT_ERR(SPIx);
}






 
static inline void LL_I2S_EnableIT_RXNE(SPI_TypeDef *SPIx)
{
  LL_SPI_EnableIT_RXNE(SPIx);
}






 
static inline void LL_I2S_EnableIT_TXE(SPI_TypeDef *SPIx)
{
  LL_SPI_EnableIT_TXE(SPIx);
}







 
static inline void LL_I2S_DisableIT_ERR(SPI_TypeDef *SPIx)
{
  LL_SPI_DisableIT_ERR(SPIx);
}






 
static inline void LL_I2S_DisableIT_RXNE(SPI_TypeDef *SPIx)
{
  LL_SPI_DisableIT_RXNE(SPIx);
}






 
static inline void LL_I2S_DisableIT_TXE(SPI_TypeDef *SPIx)
{
  LL_SPI_DisableIT_TXE(SPIx);
}






 
static inline uint32_t LL_I2S_IsEnabledIT_ERR(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsEnabledIT_ERR(SPIx);
}






 
static inline uint32_t LL_I2S_IsEnabledIT_RXNE(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsEnabledIT_RXNE(SPIx);
}






 
static inline uint32_t LL_I2S_IsEnabledIT_TXE(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsEnabledIT_TXE(SPIx);
}



 



 






 
static inline void LL_I2S_EnableDMAReq_RX(SPI_TypeDef *SPIx)
{
  LL_SPI_EnableDMAReq_RX(SPIx);
}






 
static inline void LL_I2S_DisableDMAReq_RX(SPI_TypeDef *SPIx)
{
  LL_SPI_DisableDMAReq_RX(SPIx);
}






 
static inline uint32_t LL_I2S_IsEnabledDMAReq_RX(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsEnabledDMAReq_RX(SPIx);
}






 
static inline void LL_I2S_EnableDMAReq_TX(SPI_TypeDef *SPIx)
{
  LL_SPI_EnableDMAReq_TX(SPIx);
}






 
static inline void LL_I2S_DisableDMAReq_TX(SPI_TypeDef *SPIx)
{
  LL_SPI_DisableDMAReq_TX(SPIx);
}






 
static inline uint32_t LL_I2S_IsEnabledDMAReq_TX(const SPI_TypeDef *SPIx)
{
  return LL_SPI_IsEnabledDMAReq_TX(SPIx);
}



 



 






 
static inline uint16_t LL_I2S_ReceiveData16(SPI_TypeDef *SPIx)
{
  return LL_SPI_ReceiveData16(SPIx);
}







 
static inline void LL_I2S_TransmitData16(SPI_TypeDef *SPIx, uint16_t TxData)
{
  LL_SPI_TransmitData16(SPIx, TxData);
}



 



 

ErrorStatus LL_I2S_DeInit(const SPI_TypeDef *SPIx);
ErrorStatus LL_I2S_Init(SPI_TypeDef *SPIx,const LL_I2S_InitTypeDef *I2S_InitStruct);
void        LL_I2S_StructInit(LL_I2S_InitTypeDef *I2S_InitStruct);
void        LL_I2S_ConfigPrescaler(SPI_TypeDef *SPIx, uint32_t PrescalerLinear, uint32_t PrescalerParity);



 



 



 




 


































 

 


 



 



 

 
 

 

 

 
 


 



 


 



 


 




 


 



 


 



 


 



 


 



 

 


 



 


 



 


 



 


 



 


 


 

 

 


 



 






 
static inline uint32_t LL_SYSTICK_IsActiveCounterFlag(void)
{
  return ((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL & (1UL << 16U)) == ((1UL << 16U)));
}








 
static inline void LL_SYSTICK_SetClkSource(uint32_t Source)
{
  if (Source == (1UL << 2U))
  {
    ((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL) |= ((1UL << 2U)));
  }
  else
  {
    ((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL) &= ~((1UL << 2U)));
  }
}







 
static inline uint32_t LL_SYSTICK_GetClkSource(void)
{
  return ((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL) & ((1UL << 2U)));
}





 
static inline void LL_SYSTICK_EnableIT(void)
{
  ((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL) |= ((1UL << 1U)));
}





 
static inline void LL_SYSTICK_DisableIT(void)
{
  ((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL) &= ~((1UL << 1U)));
}





 
static inline uint32_t LL_SYSTICK_IsEnabledIT(void)
{
  return (((((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL) & ((1UL << 1U))) == ((1UL << 1U)));
}



 



 





 
static inline void LL_LPM_EnableSleep(void)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR) &= ~(((uint32_t)(1UL << 2U))));
}





 
static inline void LL_LPM_EnableDeepSleep(void)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR) |= (((uint32_t)(1UL << 2U))));
}







 
static inline void LL_LPM_EnableSleepOnExit(void)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR) |= (((uint32_t)(1UL << 1U))));
}





 
static inline void LL_LPM_DisableSleepOnExit(void)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR) &= ~(((uint32_t)(1UL << 1U))));
}






 
static inline void LL_LPM_EnableEventOnPend(void)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR) |= (((uint32_t)(1UL << 4U))));
}






 
static inline void LL_LPM_DisableEventOnPend(void)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SCR) &= ~(((uint32_t)(1UL << 4U))));
}



 



 









 
static inline void LL_HANDLER_EnableFault(uint32_t Fault)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR) |= (Fault));
}









 
static inline void LL_HANDLER_DisableFault(uint32_t Fault)
{
   
  ((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR) &= ~(Fault));
}



 



 





 
static inline uint32_t LL_CPUID_GetImplementer(void)
{
  return (uint32_t)(((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CPUID) & ((0xFFUL << 24U))) >> 24U);
}





 
static inline uint32_t LL_CPUID_GetVariant(void)
{
  return (uint32_t)(((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CPUID) & ((0xFUL << 20U))) >> 20U);
}





 
static inline uint32_t LL_CPUID_GetConstant(void)
{
  return (uint32_t)(((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CPUID) & ((0xFUL << 16U))) >> 16U);
}





 
static inline uint32_t LL_CPUID_GetParNo(void)
{
  return (uint32_t)(((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CPUID) & ((0xFFFUL << 4U))) >> 4U);
}





 
static inline uint32_t LL_CPUID_GetRevision(void)
{
  return (uint32_t)(((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CPUID) & ((0xFUL ))) >> 0U);
}



 



 










 
static inline void LL_MPU_Enable(uint32_t Options)
{
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL) = (((1UL ) | Options)));
   
  __iar_builtin_DSB();
   
  __iar_builtin_ISB();
}





 
static inline void LL_MPU_Disable(void)
{
   
  __iar_builtin_DMB();
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL) = (0U));
}





 
static inline uint32_t LL_MPU_IsEnabled(void)
{
  return (((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL) & ((1UL ))) == ((1UL )));
}














 
static inline void LL_MPU_EnableRegion(uint32_t Region)
{
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR) = (Region));
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR) |= ((1UL )));
}






































 
static inline void LL_MPU_ConfigRegion(uint32_t Region, uint32_t SubRegionDisable, uint32_t Address, uint32_t Attributes)
{
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR) = (Region));
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR) = ((Address & 0xFFFFFFE0U)));
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR) = (((1UL ) | Attributes | (SubRegionDisable << 8U))));
}















 
static inline void LL_MPU_DisableRegion(uint32_t Region)
{
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR) = (Region));
   
  ((((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR) &= ~((1UL )));
}



 



 



 



 































 

 


 



 



 

 
 

 


 

 



 



 



 



 

 


 


 
 


 


 
typedef struct
{
  uint32_t PLLM;   



 

  uint32_t PLLN;   



 

  uint32_t PLLP;   



 
} LL_UTILS_PLLInitTypeDef;



 
typedef struct
{
  uint32_t AHBCLKDivider;         



 

  uint32_t APB1CLKDivider;        



 

  uint32_t APB2CLKDivider;        



 

} LL_UTILS_ClkInitTypeDef;



 

 


 



 


 



 


 



 

 

 


 



 




 
static inline uint32_t LL_GetUID_Word0(void)
{
  return (uint32_t)(((*((uint32_t *)0x1FF07A10UL))));
}




 
static inline uint32_t LL_GetUID_Word1(void)
{
  return (uint32_t)(((*((uint32_t *)(0x1FF07A10UL + 4U)))));
}




 
static inline uint32_t LL_GetUID_Word2(void)
{
  return (uint32_t)(((*((uint32_t *)(0x1FF07A10UL + 8U)))));
}






 
static inline uint32_t LL_GetFlashSize(void)
{
  return (uint32_t)(((*((uint32_t *)0x1FF07A22UL))) & 0xFFFFU);
}










 
static inline uint32_t LL_GetPackageType(void)
{
  return (uint32_t)(((*((uint32_t *)0x1FF07BF0UL))) & 0x0700U);
}



 



 








 
static inline void LL_InitTick(uint32_t HCLKFrequency, uint32_t Ticks)
{
   
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD  = (uint32_t)((HCLKFrequency / Ticks) - 1UL);   
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL   = 0UL;                                        
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL  = (1UL << 2U) |
                   (1UL );                    
}

void        LL_Init1msTick(uint32_t HCLKFrequency);
void        LL_mDelay(uint32_t Delay);



 



 

void        LL_SetSystemCoreClock(uint32_t HCLKFrequency);
ErrorStatus LL_SetFlashLatency(uint32_t HCLK_Frequency);
ErrorStatus LL_PLL_ConfigSystemClock_HSI(LL_UTILS_PLLInitTypeDef *UTILS_PLLInitStruct,
                                         LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct);
ErrorStatus LL_PLL_ConfigSystemClock_HSE(uint32_t HSEFrequency, uint32_t HSEBypass,
                                         LL_UTILS_PLLInitTypeDef *UTILS_PLLInitStruct, LL_UTILS_ClkInitTypeDef *UTILS_ClkInitStruct);



 



 



 



 



















 

 


 



 




 

 
 
 
 
 
 


 




 



 




 



 
  


 


 



 


 



 


 



 


 



 


 
  


 
 


 



 






 





 


 



 
 


 



 













 
static inline void LL_PWR_EnableUnderDriveMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x3UL << (18U))));
}





 
static inline void LL_PWR_DisableUnderDriveMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x3UL << (18U))));
}





 
static inline uint32_t LL_PWR_IsEnabledUnderDriveMode(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x3UL << (18U)))) == ((0x3UL << (18U))));
}





 
static inline void LL_PWR_EnableOverDriveSwitching(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (17U))));
}





 
static inline void LL_PWR_DisableOverDriveSwitching(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (17U))));
}





 
static inline uint32_t LL_PWR_IsEnabledOverDriveSwitching(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (17U)))) == ((0x1UL << (17U))));
}





 
static inline void LL_PWR_EnableOverDriveMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (16U))));
}





 
static inline void LL_PWR_DisableOverDriveMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (16U))));
}





 
static inline uint32_t LL_PWR_IsEnabledOverDriveMode(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (16U)))) == ((0x1UL << (16U))));
}









 
static inline void LL_PWR_SetRegulVoltageScaling(uint32_t VoltageScaling)
{
  (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1)) = ((((((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1))) & (~((0x3UL << (14U))))) | (VoltageScaling))));
}








 
static inline uint32_t LL_PWR_GetRegulVoltageScaling(void)
{
  return (uint32_t)(((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x3UL << (14U)))));
}





 
static inline void LL_PWR_EnableMainRegulatorDeepSleepUDMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (11U))));
}





 
static inline void LL_PWR_DisableMainRegulatorDeepSleepUDMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (11U))));
}





 
static inline uint32_t LL_PWR_IsEnabledMainRegulatorDeepSleepUDMode(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (11U)))) == ((0x1UL << (11U))));
}





 
static inline void LL_PWR_EnableLowPowerRegulatorDeepSleepUDMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (10U))));
}





 
static inline void LL_PWR_DisableLowPowerRegulatorDeepSleepUDMode(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (10U))));
}





 
static inline uint32_t LL_PWR_IsEnabledLowPowerRegulatorDeepSleepUDMode(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (10U)))) == ((0x1UL << (10U))));
}





 
static inline void LL_PWR_EnableFlashPowerDown(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (9U))));
}





 
static inline void LL_PWR_DisableFlashPowerDown(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (9U))));
}





 
static inline uint32_t LL_PWR_IsEnabledFlashPowerDown(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (9U)))) == ((0x1UL << (9U))));
}





 
static inline void LL_PWR_EnableBkUpAccess(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (8U))));
}





 
static inline void LL_PWR_DisableBkUpAccess(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (8U))));
}





 
static inline uint32_t LL_PWR_IsEnabledBkUpAccess(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}










 
static inline void LL_PWR_EnableBkUpRegulator(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) |= ((0x1UL << (9U))));
}





 
static inline void LL_PWR_DisableBkUpRegulator(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) &= ~((0x1UL << (9U))));
}





 
static inline uint32_t LL_PWR_IsEnabledBkUpRegulator(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (9U)))) == ((0x1UL << (9U))));
}








 
static inline void LL_PWR_SetRegulModeDS(uint32_t RegulMode)
{
  (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1)) = ((((((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1))) & (~((0x1UL << (0U))))) | (RegulMode))));
}







 
static inline uint32_t LL_PWR_GetRegulModeDS(void)
{
  return (uint32_t)(((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (0U)))));
}















 
static inline void LL_PWR_SetPowerMode(uint32_t PDMode)
{
  (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1)) = ((((((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1))) & (~(((0x1UL << (1U)) | (0x1UL << (0U)) | (0x1UL << (9U)) | (0x1UL << (10U)) | (0x1UL << (11U)))))) | (PDMode))));
}














 
static inline uint32_t LL_PWR_GetPowerMode(void)
{
  return (uint32_t)(((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & (((0x1UL << (1U)) | (0x1UL << (0U)) | (0x1UL << (9U)) | (0x1UL << (10U)) | (0x1UL << (11U))))));
}














 
static inline void LL_PWR_SetPVDLevel(uint32_t PVDLevel)
{
  (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1)) = ((((((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1))) & (~((0x7UL << (5U))))) | (PVDLevel))));
}













 
static inline uint32_t LL_PWR_GetPVDLevel(void)
{
  return (uint32_t)(((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x7UL << (5U)))));
}





 
static inline void LL_PWR_EnablePVD(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (4U))));
}





 
static inline void LL_PWR_DisablePVD(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) &= ~((0x1UL << (4U))));
}





 
static inline uint32_t LL_PWR_IsEnabledPVD(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) & ((0x1UL << (4U)))) == ((0x1UL << (4U))));
}

















 
static inline void LL_PWR_EnableWakeUpPin(uint32_t WakeUpPin)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) |= (WakeUpPin));
}

















 
static inline void LL_PWR_DisableWakeUpPin(uint32_t WakeUpPin)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) &= ~(WakeUpPin));
}

















 
static inline uint32_t LL_PWR_IsEnabledWakeUpPin(uint32_t WakeUpPin)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & (WakeUpPin)) == (WakeUpPin));
}

















 
static inline void LL_PWR_SetWakeUpPinPolarityLow(uint32_t WakeUpPin)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) |= (WakeUpPin));
}

















 
static inline void LL_PWR_SetWakeUpPinPolarityHigh(uint32_t WakeUpPin)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) &= ~(WakeUpPin));
}

















 
static inline uint32_t LL_PWR_IsWakeUpPinPolarityLow(uint32_t WakeUpPin)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) & (WakeUpPin)) == (WakeUpPin));
}







 
static inline void LL_PWR_EnableInternalWakeUp(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) |= ((0x1UL << (8U))));
}





 
static inline void LL_PWR_DisableInternalWakeUp(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) &= ~((0x1UL << (8U))));
}





 
static inline uint32_t LL_PWR_IsEnabledInternalWakeUp(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (8U)))) == ((0x1UL << (8U))));
}



 



 





 
static inline uint32_t LL_PWR_IsActiveFlag_WU6(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & ((0x1UL << (5U)))) == ((0x1UL << (5U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_WU5(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & ((0x1UL << (4U)))) == ((0x1UL << (4U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_WU4(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & ((0x1UL << (3U)))) == ((0x1UL << (3U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_WU3(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_WU2(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_WU1(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR2) & ((0x1UL << (0U)))) == ((0x1UL << (0U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_SB(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (1U)))) == ((0x1UL << (1U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_PVDO(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (2U)))) == ((0x1UL << (2U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_BRR(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (3U)))) == ((0x1UL << (3U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_VOS(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (14U)))) == ((0x1UL << (14U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_OD(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (16U)))) == ((0x1UL << (16U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_ODSW(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x1UL << (17U)))) == ((0x1UL << (17U))));
}





 
static inline uint32_t LL_PWR_IsActiveFlag_UD(void)
{
  return (((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) & ((0x3UL << (18U)))) == ((0x3UL << (18U))));
}





 
static inline void LL_PWR_ClearFlag_SB(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR1) |= ((0x1UL << (3U))));
}





 
static inline void LL_PWR_ClearFlag_WU6(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) = ((0x1UL << (5U))));
}





 
static inline void LL_PWR_ClearFlag_WU5(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) = ((0x1UL << (4U))));
}





 
static inline void LL_PWR_ClearFlag_WU4(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) = ((0x1UL << (3U))));
}





 
static inline void LL_PWR_ClearFlag_WU3(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) = ((0x1UL << (2U))));
}





 
static inline void LL_PWR_ClearFlag_WU2(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) = ((0x1UL << (1U))));
}





 
static inline void LL_PWR_ClearFlag_WU1(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CR2) = ((0x1UL << (0U))));
}





 
static inline void LL_PWR_ClearFlag_UD(void)
{
  ((((PWR_TypeDef *) (0x40000000UL + 0x7000UL))->CSR1) = ((0x3UL << (18U))));
}



 
ErrorStatus LL_PWR_DeInit(void);


 



 



 



 




 



















 

 


 



 




 

 
 
 
 
 

 
 


 





 


 




 


 



 

 


 



 







 






 


 



 


 


 


 






 
static inline void LL_RNG_Enable(RNG_TypeDef *RNGx)
{
  ((RNGx->CR) |= ((0x1UL << (2U))));
}






 
static inline void LL_RNG_Disable(RNG_TypeDef *RNGx)
{
  ((RNGx->CR) &= ~((0x1UL << (2U))));
}






 
static inline uint32_t LL_RNG_IsEnabled(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->CR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}



 



 






 
static inline uint32_t LL_RNG_IsActiveFlag_DRDY(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->SR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_RNG_IsActiveFlag_CECS(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->SR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_RNG_IsActiveFlag_SECS(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->SR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_RNG_IsActiveFlag_CEIS(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->SR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_RNG_IsActiveFlag_SEIS(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->SR) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline void LL_RNG_ClearFlag_CEIS(RNG_TypeDef *RNGx)
{
  ((RNGx->SR) = (~(0x1UL << (5U))));
}






 
static inline void LL_RNG_ClearFlag_SEIS(RNG_TypeDef *RNGx)
{
  ((RNGx->SR) = (~(0x1UL << (6U))));
}



 



 







 
static inline void LL_RNG_EnableIT(RNG_TypeDef *RNGx)
{
  ((RNGx->CR) |= ((0x1UL << (3U))));
}







 
static inline void LL_RNG_DisableIT(RNG_TypeDef *RNGx)
{
  ((RNGx->CR) &= ~((0x1UL << (3U))));
}







 
static inline uint32_t LL_RNG_IsEnabledIT(const RNG_TypeDef *RNGx)
{
  return ((((RNGx->CR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}



 



 






 
static inline uint32_t LL_RNG_ReadRandData32(const RNG_TypeDef *RNGx)
{
  return (uint32_t)(((RNGx->DR)));
}



 



 
ErrorStatus LL_RNG_DeInit(const RNG_TypeDef *RNGx);



 



 



 




 



















 

 


 



 




 

 
 
 
 
 
 


 




 


 



 


 



 

 


 


 






 






 


 



 

 


 



 









 
static inline void LL_WWDG_Enable(WWDG_TypeDef *WWDGx)
{
  ((WWDGx->CR) |= ((0x1UL << (7U))));
}






 
static inline uint32_t LL_WWDG_IsEnabled(const WWDG_TypeDef *WWDGx)
{
  return ((((WWDGx->CR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}











 
static inline void LL_WWDG_SetCounter(WWDG_TypeDef *WWDGx, uint32_t Counter)
{
  (((WWDGx->CR)) = ((((((WWDGx->CR))) & (~((0x7FUL << (0U))))) | (Counter))));
}






 
static inline uint32_t LL_WWDG_GetCounter(const WWDG_TypeDef *WWDGx)
{
  return (((WWDGx->CR) & ((0x7FUL << (0U)))));
}













 
static inline void LL_WWDG_SetPrescaler(WWDG_TypeDef *WWDGx, uint32_t Prescaler)
{
  (((WWDGx->CFR)) = ((((((WWDGx->CFR))) & (~((0x3UL << (7U))))) | (Prescaler))));
}










 
static inline uint32_t LL_WWDG_GetPrescaler(const WWDG_TypeDef *WWDGx)
{
  return (((WWDGx->CFR) & ((0x3UL << (7U)))));
}
















 
static inline void LL_WWDG_SetWindow(WWDG_TypeDef *WWDGx, uint32_t Window)
{
  (((WWDGx->CFR)) = ((((((WWDGx->CFR))) & (~((0x7FUL << (0U))))) | (Window))));
}






 
static inline uint32_t LL_WWDG_GetWindow(const WWDG_TypeDef *WWDGx)
{
  return (((WWDGx->CFR) & ((0x7FUL << (0U)))));
}



 



 








 
static inline uint32_t LL_WWDG_IsActiveFlag_EWKUP(const WWDG_TypeDef *WWDGx)
{
  return ((((WWDGx->SR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline void LL_WWDG_ClearFlag_EWKUP(WWDG_TypeDef *WWDGx)
{
  ((WWDGx->SR) = (~(0x1UL << (0U))));
}



 



 







 
static inline void LL_WWDG_EnableIT_EWKUP(WWDG_TypeDef *WWDGx)
{
  ((WWDGx->CFR) |= ((0x1UL << (9U))));
}






 
static inline uint32_t LL_WWDG_IsEnabledIT_EWKUP(const WWDG_TypeDef *WWDGx)
{
  return ((((WWDGx->CFR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}



 



 



 




 


















 

 


 



 




 

 
 
 
 


 



 

 


 



 
typedef struct
{
  uint32_t Pin;          
 

  uint32_t Mode;         


 

  uint32_t Speed;        


 

  uint32_t OutputType;   


 

  uint32_t Pull;         


 

  uint32_t Alternate;    


 
} LL_GPIO_InitTypeDef;



 

 


 



 


 



 


 



 


 



 


 



 


 



 


 



 

 


 



 







 






 


 



 

 


 



 






























 
static inline void LL_GPIO_SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
{
  (((GPIOx->MODER)) = ((((((GPIOx->MODER))) & (~(((0x3UL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U))))) | ((Mode << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U))))));
}





























 
static inline uint32_t LL_GPIO_GetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->MODER) & (((0x3UL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U)))) >> ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U));
}





























 
static inline void LL_GPIO_SetPinOutputType(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t OutputType)
{
  (((GPIOx->OTYPER)) = ((((((GPIOx->OTYPER))) & (~(PinMask))) | ((PinMask * OutputType)))));
}





























 
static inline uint32_t LL_GPIO_GetPinOutputType(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->OTYPER) & (Pin)) >> (__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))));
}
































 
static inline void LL_GPIO_SetPinSpeed(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t  Speed)
{
  (((GPIOx->OSPEEDR)) = ((((((GPIOx->OSPEEDR))) & (~(((0x3UL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U))))) | ((Speed << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U))))));
}































 
static inline uint32_t LL_GPIO_GetPinSpeed(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->OSPEEDR) & (((0x3UL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U)))) >> ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U));
}




























 
static inline void LL_GPIO_SetPinPull(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Pull)
{
  (((GPIOx->PUPDR)) = ((((((GPIOx->PUPDR))) & (~(((0x3UL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U))))) | ((Pull << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U))))));
}



























 
static inline uint32_t LL_GPIO_GetPinPull(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->PUPDR) & (((0x3UL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U)))) >> ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 2U));
}


































 
static inline void LL_GPIO_SetAFPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate)
{
  (((GPIOx->AFR[0])) = ((((((GPIOx->AFR[0]))) & (~(((0xFUL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 4U))))) | ((Alternate << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 4U))))));
}































 
static inline uint32_t LL_GPIO_GetAFPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->AFR[0]) & (((0xFUL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 4U)))) >> ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin))) * 4U));
}


































 
static inline void LL_GPIO_SetAFPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate)
{
  (((GPIOx->AFR[1])) = ((((((GPIOx->AFR[1]))) & (~(((0xFUL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin >> 8U))) * 4U))))) | ((Alternate << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin >> 8U))) * 4U))))));
}
































 
static inline uint32_t LL_GPIO_GetAFPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->AFR[1]) & (((0xFUL << (0U)) << ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin >> 8U))) * 4U)))) >> ((__iar_builtin_CLZ(__iar_builtin_RBIT(Pin >> 8U))) * 4U));
}






























 
static inline void LL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  volatile uint32_t temp;
  ((GPIOx->LCKR) = ((0x1UL << (16U)) | PinMask));
  ((GPIOx->LCKR) = (PinMask));
  ((GPIOx->LCKR) = ((0x1UL << (16U)) | PinMask));
  temp = ((GPIOx->LCKR));
  (void) temp;
}
























 
static inline uint32_t LL_GPIO_IsPinLocked(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (((GPIOx->LCKR) & (PinMask)) == (PinMask));
}






 
static inline uint32_t LL_GPIO_IsAnyPinLocked(GPIO_TypeDef *GPIOx)
{
  return (((GPIOx->LCKR) & ((0x1UL << (16U)))) == ((0x1UL << (16U))));
}



 



 






 
static inline uint32_t LL_GPIO_ReadInputPort(GPIO_TypeDef *GPIOx)
{
  return (uint32_t)(((GPIOx->IDR)));
}
























 
static inline uint32_t LL_GPIO_IsInputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (((GPIOx->IDR) & (PinMask)) == (PinMask));
}







 
static inline void LL_GPIO_WriteOutputPort(GPIO_TypeDef *GPIOx, uint32_t PortValue)
{
  ((GPIOx->ODR) = (PortValue));
}






 
static inline uint32_t LL_GPIO_ReadOutputPort(GPIO_TypeDef *GPIOx)
{
  return (uint32_t)(((GPIOx->ODR)));
}
























 
static inline uint32_t LL_GPIO_IsOutputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return (((GPIOx->ODR) & (PinMask)) == (PinMask));
}
























 
static inline void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  ((GPIOx->BSRR) = (PinMask));
}
























 
static inline void LL_GPIO_ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  ((GPIOx->BSRR) = ((PinMask << 16)));
}
























 
static inline void LL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  uint32_t odr = ((GPIOx->ODR));
  ((GPIOx->BSRR) = (((odr & PinMask) << 16u) | (~odr & PinMask)));
}



 



 

ErrorStatus LL_GPIO_DeInit(GPIO_TypeDef *GPIOx);
ErrorStatus LL_GPIO_Init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct);
void        LL_GPIO_StructInit(LL_GPIO_InitTypeDef *GPIO_InitStruct);



 



 



 



 



















 

 


 



 




 

 
 

 


 


 
 


 


 

 


 



 
typedef struct
{

  uint32_t BaudRate;                  


 

  uint32_t DataWidth;                 



 

  uint32_t StopBits;                  



 

  uint32_t Parity;                    



 

  uint32_t TransferDirection;         



 

  uint32_t HardwareFlowControl;       



 

  uint32_t OverSampling;              



 

} LL_USART_InitTypeDef;



 
typedef struct
{
  uint32_t ClockOutput;               




 

  uint32_t ClockPolarity;             




 

  uint32_t ClockPhase;                




 

  uint32_t LastBitClockPulse;         





 

} LL_USART_ClockInitTypeDef;



 

 


 




 


 




 


 




 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 

 


 



 







 






 


 



 







 







 



 



 

 



 



 






 
static inline void LL_USART_Enable(USART_TypeDef *USARTx)
{
  ((USARTx->CR1) |= ((0x1UL << (0U))));
}









 
static inline void LL_USART_Disable(USART_TypeDef *USARTx)
{
  ((USARTx->CR1) &= ~((0x1UL << (0U))));
}






 
static inline uint32_t LL_USART_IsEnabled(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline void LL_USART_EnableDirectionRx(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (2U)));
}






 
static inline void LL_USART_DisableDirectionRx(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (2U)));
}






 
static inline void LL_USART_EnableDirectionTx(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (3U)));
}






 
static inline void LL_USART_DisableDirectionTx(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (3U)));
}













 
static inline void LL_USART_SetTransferDirection(USART_TypeDef *USARTx, uint32_t TransferDirection)
{
  ATOMIC_MODIFY_REG(USARTx->CR1, (0x1UL << (2U)) | (0x1UL << (3U)), TransferDirection);
}











 
static inline uint32_t LL_USART_GetTransferDirection(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x1UL << (2U)) | (0x1UL << (3U)))));
}














 
static inline void LL_USART_SetParity(USART_TypeDef *USARTx, uint32_t Parity)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x1UL << (9U)) | (0x1UL << (10U))))) | (Parity))));
}










 
static inline uint32_t LL_USART_GetParity(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x1UL << (9U)) | (0x1UL << (10U)))));
}









 
static inline void LL_USART_SetWakeUpMethod(USART_TypeDef *USARTx, uint32_t Method)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x1UL << (11U))))) | (Method))));
}








 
static inline uint32_t LL_USART_GetWakeUpMethod(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x1UL << (11U)))));
}











 
static inline void LL_USART_SetDataWidth(USART_TypeDef *USARTx, uint32_t DataWidth)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x10001UL << (12U))))) | (DataWidth))));
}










 
static inline uint32_t LL_USART_GetDataWidth(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x10001UL << (12U)))));
}






 
static inline void LL_USART_EnableMuteMode(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (13U)));
}






 
static inline void LL_USART_DisableMuteMode(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (13U)));
}






 
static inline uint32_t LL_USART_IsEnabledMuteMode(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (13U)))) == ((0x1UL << (13U)))) ? 1UL : 0UL);
}









 
static inline void LL_USART_SetOverSampling(USART_TypeDef *USARTx, uint32_t OverSampling)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x1UL << (15U))))) | (OverSampling))));
}








 
static inline uint32_t LL_USART_GetOverSampling(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x1UL << (15U)))));
}











 
static inline void LL_USART_SetLastClkPulseOutput(USART_TypeDef *USARTx, uint32_t LastBitClockPulse)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (8U))))) | (LastBitClockPulse))));
}











 
static inline uint32_t LL_USART_GetLastClkPulseOutput(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (8U)))));
}











 
static inline void LL_USART_SetClockPhase(USART_TypeDef *USARTx, uint32_t ClockPhase)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (9U))))) | (ClockPhase))));
}










 
static inline uint32_t LL_USART_GetClockPhase(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (9U)))));
}











 
static inline void LL_USART_SetClockPolarity(USART_TypeDef *USARTx, uint32_t ClockPolarity)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (10U))))) | (ClockPolarity))));
}










 
static inline uint32_t LL_USART_GetClockPolarity(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (10U)))));
}























 
static inline void LL_USART_ConfigClock(USART_TypeDef *USARTx, uint32_t Phase, uint32_t Polarity, uint32_t LBCPOutput)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (9U)) | (0x1UL << (10U)) | (0x1UL << (8U))))) | (Phase | Polarity | LBCPOutput))));
}








 
static inline void LL_USART_EnableSCLKOutput(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) |= ((0x1UL << (11U))));
}








 
static inline void LL_USART_DisableSCLKOutput(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) &= ~((0x1UL << (11U))));
}








 
static inline uint32_t LL_USART_IsEnabledSCLKOutput(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR2) & ((0x1UL << (11U)))) == ((0x1UL << (11U)))) ? 1UL : 0UL);
}











 
static inline void LL_USART_SetStopBitsLength(USART_TypeDef *USARTx, uint32_t StopBits)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x3UL << (12U))))) | (StopBits))));
}










 
static inline uint32_t LL_USART_GetStopBitsLength(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x3UL << (12U)))));
}



























 
static inline void LL_USART_ConfigCharacter(USART_TypeDef *USARTx, uint32_t DataWidth, uint32_t Parity,
                                              uint32_t StopBits)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x1UL << (9U)) | (0x1UL << (10U)) | (0x10001UL << (12U))))) | (Parity | DataWidth))));
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x3UL << (12U))))) | (StopBits))));
}









 
static inline void LL_USART_SetTXRXSwap(USART_TypeDef *USARTx, uint32_t SwapConfig)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (15U))))) | (SwapConfig))));
}








 
static inline uint32_t LL_USART_GetTXRXSwap(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (15U)))));
}









 
static inline void LL_USART_SetRXPinLevel(USART_TypeDef *USARTx, uint32_t PinInvMethod)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (16U))))) | (PinInvMethod))));
}








 
static inline uint32_t LL_USART_GetRXPinLevel(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (16U)))));
}









 
static inline void LL_USART_SetTXPinLevel(USART_TypeDef *USARTx, uint32_t PinInvMethod)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (17U))))) | (PinInvMethod))));
}








 
static inline uint32_t LL_USART_GetTXPinLevel(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (17U)))));
}











 
static inline void LL_USART_SetBinaryDataLogic(USART_TypeDef *USARTx, uint32_t DataLogic)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (18U))))) | (DataLogic))));
}








 
static inline uint32_t LL_USART_GetBinaryDataLogic(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (18U)))));
}











 
static inline void LL_USART_SetTransferBitOrder(USART_TypeDef *USARTx, uint32_t BitOrder)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (19U))))) | (BitOrder))));
}










 
static inline uint32_t LL_USART_GetTransferBitOrder(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (19U)))));
}








 
static inline void LL_USART_EnableAutoBaudRate(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) |= ((0x1UL << (20U))));
}








 
static inline void LL_USART_DisableAutoBaudRate(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) &= ~((0x1UL << (20U))));
}








 
static inline uint32_t LL_USART_IsEnabledAutoBaud(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR2) & ((0x1UL << (20U)))) == ((0x1UL << (20U)))) ? 1UL : 0UL);
}













 
static inline void LL_USART_SetAutoBaudRateMode(USART_TypeDef *USARTx, uint32_t AutoBaudRateMode)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x3UL << (21U))))) | (AutoBaudRateMode))));
}












 
static inline uint32_t LL_USART_GetAutoBaudRateMode(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x3UL << (21U)))));
}






 
static inline void LL_USART_EnableRxTimeout(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) |= ((0x1UL << (23U))));
}






 
static inline void LL_USART_DisableRxTimeout(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) &= ~((0x1UL << (23U))));
}






 
static inline uint32_t LL_USART_IsEnabledRxTimeout(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR2) & ((0x1UL << (23U)))) == ((0x1UL << (23U)))) ? 1UL : 0UL);
}























 
static inline void LL_USART_ConfigNodeAddress(USART_TypeDef *USARTx, uint32_t AddressLen, uint32_t NodeAddress)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0xFFUL << (24U)) | (0x1UL << (4U))))) | ((uint32_t)(AddressLen | (NodeAddress << (24U)))))));
}










 
static inline uint32_t LL_USART_GetNodeAddress(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0xFFUL << (24U)))) >> (24U));
}








 
static inline uint32_t LL_USART_GetNodeAddressLen(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (4U)))));
}








 
static inline void LL_USART_EnableRTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (8U))));
}








 
static inline void LL_USART_DisableRTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (8U))));
}








 
static inline void LL_USART_EnableCTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (9U))));
}








 
static inline void LL_USART_DisableCTSHWFlowCtrl(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (9U))));
}














 
static inline void LL_USART_SetHWFlowCtrl(USART_TypeDef *USARTx, uint32_t HardwareFlowControl)
{
  (((USARTx->CR3)) = ((((((USARTx->CR3))) & (~((0x1UL << (8U)) | (0x1UL << (9U))))) | (HardwareFlowControl))));
}













 
static inline uint32_t LL_USART_GetHWFlowCtrl(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR3) & ((0x1UL << (8U)) | (0x1UL << (9U)))));
}






 
static inline void LL_USART_EnableOneBitSamp(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (11U))));
}






 
static inline void LL_USART_DisableOneBitSamp(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (11U))));
}






 
static inline uint32_t LL_USART_IsEnabledOneBitSamp(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (11U)))) == ((0x1UL << (11U)))) ? 1UL : 0UL);
}






 
static inline void LL_USART_EnableOverrunDetect(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (12U))));
}






 
static inline void LL_USART_DisableOverrunDetect(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (12U))));
}






 
static inline uint32_t LL_USART_IsEnabledOverrunDetect(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (12U)))) != (0x1UL << (12U))) ? 1UL : 0UL);
}
















 
static inline void LL_USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t OverSampling,
                                          uint32_t BaudRate)
{
  uint32_t usartdiv;
  uint32_t brrtemp;

  if (OverSampling == (0x1UL << (15U)))
  {
    usartdiv = (uint16_t)(((((PeriphClk)*2U) + ((BaudRate)/2U))/(BaudRate)));
    brrtemp = usartdiv & 0xFFF0U;
    brrtemp |= (uint16_t)((usartdiv & (uint16_t)0x000FU) >> 1U);
    USARTx->BRR = brrtemp;
  }
  else
  {
    USARTx->BRR = (uint16_t)((((PeriphClk) + ((BaudRate)/2U))/(BaudRate)));
  }
}













 
static inline uint32_t LL_USART_GetBaudRate(const USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t OverSampling)
{
  uint32_t usartdiv;
  uint32_t brrresult = 0x0U;

  usartdiv = USARTx->BRR;

  if (usartdiv == 0U)
  {
     
  }
  else if (OverSampling == (0x1UL << (15U)))
  {
    usartdiv = (uint16_t)((usartdiv & 0xFFF0U) | ((usartdiv & 0x0007U) << 1U)) ;
    if (usartdiv != 0U)
    {
      brrresult = (PeriphClk * 2U) / usartdiv;
    }
  }
  else
  {
    if ((usartdiv & 0xFFFFU) != 0U)
    {
      brrresult = PeriphClk / usartdiv;
    }
  }
  return (brrresult);
}







 
static inline void LL_USART_SetRxTimeout(USART_TypeDef *USARTx, uint32_t Timeout)
{
  (((USARTx->RTOR)) = ((((((USARTx->RTOR))) & (~((0xFFFFFFUL << (0U))))) | (Timeout))));
}






 
static inline uint32_t LL_USART_GetRxTimeout(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->RTOR) & ((0xFFFFFFUL << (0U)))));
}







 
static inline void LL_USART_SetBlockLength(USART_TypeDef *USARTx, uint32_t BlockLength)
{
  (((USARTx->RTOR)) = ((((((USARTx->RTOR))) & (~((0xFFUL << (24U))))) | (BlockLength << (24U)))));
}






 
static inline uint32_t LL_USART_GetBlockLength(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->RTOR) & ((0xFFUL << (24U)))) >> (24U));
}



 



 








 
static inline void LL_USART_EnableIrda(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (1U))));
}








 
static inline void LL_USART_DisableIrda(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (1U))));
}








 
static inline uint32_t LL_USART_IsEnabledIrda(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}











 
static inline void LL_USART_SetIrdaPowerMode(USART_TypeDef *USARTx, uint32_t PowerMode)
{
  (((USARTx->CR3)) = ((((((USARTx->CR3))) & (~((0x1UL << (2U))))) | (PowerMode))));
}










 
static inline uint32_t LL_USART_GetIrdaPowerMode(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR3) & ((0x1UL << (2U)))));
}










 
static inline void LL_USART_SetIrdaPrescaler(USART_TypeDef *USARTx, uint32_t PrescalerValue)
{
  (((USARTx->GTPR)) = ((((((USARTx->GTPR))) & (~((0xFFUL << (0U))))) | ((uint16_t)PrescalerValue))));
}









 
static inline uint32_t LL_USART_GetIrdaPrescaler(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->GTPR) & ((0xFFUL << (0U)))));
}



 



 








 
static inline void LL_USART_EnableSmartcardNACK(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (4U))));
}








 
static inline void LL_USART_DisableSmartcardNACK(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (4U))));
}








 
static inline uint32_t LL_USART_IsEnabledSmartcardNACK(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}








 
static inline void LL_USART_EnableSmartcard(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (5U))));
}








 
static inline void LL_USART_DisableSmartcard(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (5U))));
}








 
static inline uint32_t LL_USART_IsEnabledSmartcard(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}














 
static inline void LL_USART_SetSmartcardAutoRetryCount(USART_TypeDef *USARTx, uint32_t AutoRetryCount)
{
  (((USARTx->CR3)) = ((((((USARTx->CR3))) & (~((0x7UL << (17U))))) | (AutoRetryCount << (17U)))));
}








 
static inline uint32_t LL_USART_GetSmartcardAutoRetryCount(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR3) & ((0x7UL << (17U)))) >> (17U));
}










 
static inline void LL_USART_SetSmartcardPrescaler(USART_TypeDef *USARTx, uint32_t PrescalerValue)
{
  (((USARTx->GTPR)) = ((((((USARTx->GTPR))) & (~((0xFFUL << (0U))))) | ((uint16_t)PrescalerValue))));
}









 
static inline uint32_t LL_USART_GetSmartcardPrescaler(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->GTPR) & ((0xFFUL << (0U)))));
}










 
static inline void LL_USART_SetSmartcardGuardTime(USART_TypeDef *USARTx, uint32_t GuardTime)
{
  (((USARTx->GTPR)) = ((((((USARTx->GTPR))) & (~((0xFFUL << (8U))))) | ((uint16_t)(GuardTime << (8U))))));
}









 
static inline uint32_t LL_USART_GetSmartcardGuardTime(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->GTPR) & ((0xFFUL << (8U)))) >> (8U));
}



 



 








 
static inline void LL_USART_EnableHalfDuplex(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (3U))));
}








 
static inline void LL_USART_DisableHalfDuplex(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (3U))));
}








 
static inline uint32_t LL_USART_IsEnabledHalfDuplex(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}



 



 











 
static inline void LL_USART_SetLINBrkDetectionLen(USART_TypeDef *USARTx, uint32_t LINBDLength)
{
  (((USARTx->CR2)) = ((((((USARTx->CR2))) & (~((0x1UL << (5U))))) | (LINBDLength))));
}










 
static inline uint32_t LL_USART_GetLINBrkDetectionLen(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR2) & ((0x1UL << (5U)))));
}








 
static inline void LL_USART_EnableLIN(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) |= ((0x1UL << (14U))));
}








 
static inline void LL_USART_DisableLIN(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) &= ~((0x1UL << (14U))));
}








 
static inline uint32_t LL_USART_IsEnabledLIN(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR2) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}



 



 









 
static inline void LL_USART_SetDEDeassertionTime(USART_TypeDef *USARTx, uint32_t Time)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x1FUL << (16U))))) | (Time << (16U)))));
}








 
static inline uint32_t LL_USART_GetDEDeassertionTime(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x1FUL << (16U)))) >> (16U));
}









 
static inline void LL_USART_SetDEAssertionTime(USART_TypeDef *USARTx, uint32_t Time)
{
  (((USARTx->CR1)) = ((((((USARTx->CR1))) & (~((0x1FUL << (21U))))) | (Time << (21U)))));
}








 
static inline uint32_t LL_USART_GetDEAssertionTime(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR1) & ((0x1FUL << (21U)))) >> (21U));
}








 
static inline void LL_USART_EnableDEMode(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (14U))));
}








 
static inline void LL_USART_DisableDEMode(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (14U))));
}








 
static inline uint32_t LL_USART_IsEnabledDEMode(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}











 
static inline void LL_USART_SetDESignalPolarity(USART_TypeDef *USARTx, uint32_t Polarity)
{
  (((USARTx->CR3)) = ((((((USARTx->CR3))) & (~((0x1UL << (15U))))) | (Polarity))));
}










 
static inline uint32_t LL_USART_GetDESignalPolarity(const USART_TypeDef *USARTx)
{
  return (uint32_t)(((USARTx->CR3) & ((0x1UL << (15U)))));
}



 



 

























 
static inline void LL_USART_ConfigAsyncMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (14U)) | (0x1UL << (11U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (5U)) | (0x1UL << (1U)) | (0x1UL << (3U)))));
}



























 
static inline void LL_USART_ConfigSyncMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (14U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (5U)) | (0x1UL << (1U)) | (0x1UL << (3U)))));
   
  ((USARTx->CR2) |= ((0x1UL << (11U))));
}





























 
static inline void LL_USART_ConfigLINMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (11U)) | (0x3UL << (12U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (1U)) | (0x1UL << (5U)) | (0x1UL << (3U)))));
   
  ((USARTx->CR2) |= ((0x1UL << (14U))));
}



























 
static inline void LL_USART_ConfigHalfDuplexMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (14U)) | (0x1UL << (11U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (5U)) | (0x1UL << (1U)))));
   
  ((USARTx->CR3) |= ((0x1UL << (3U))));
}





























 
static inline void LL_USART_ConfigSmartcardMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (14U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (1U)) | (0x1UL << (3U)))));
   
   
  ((USARTx->CR2) |= (((0x1UL << (12U)) | (0x2UL << (12U)) | (0x1UL << (11U)))));
   
  ((USARTx->CR3) |= ((0x1UL << (5U))));
}





























 
static inline void LL_USART_ConfigIrdaMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (14U)) | (0x1UL << (11U)) | (0x3UL << (12U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (5U)) | (0x1UL << (3U)))));
   
  ((USARTx->CR3) |= ((0x1UL << (1U))));
}



























 
static inline void LL_USART_ConfigMultiProcessMode(USART_TypeDef *USARTx)
{
  


 
  ((USARTx->CR2) &= ~(((0x1UL << (14U)) | (0x1UL << (11U)))));
  ((USARTx->CR3) &= ~(((0x1UL << (5U)) | (0x1UL << (3U)) | (0x1UL << (1U)))));
}



 



 






 
static inline uint32_t LL_USART_IsActiveFlag_PE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_FE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_NE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_ORE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_IDLE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_RXNE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_TC(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_TXE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsActiveFlag_LBD(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsActiveFlag_nCTS(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsActiveFlag_CTS(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_RTO(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (11U)))) == ((0x1UL << (11U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsActiveFlag_EOB(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (12U)))) == ((0x1UL << (12U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsActiveFlag_ABRE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsActiveFlag_ABR(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (15U)))) == ((0x1UL << (15U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_BUSY(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (16U)))) == ((0x1UL << (16U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_CM(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (17U)))) == ((0x1UL << (17U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_SBK(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (18U)))) == ((0x1UL << (18U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_RWU(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (19U)))) == ((0x1UL << (19U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsActiveFlag_TEACK(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (21U)))) == ((0x1UL << (21U)))) ? 1UL : 0UL);
}

 





 
static inline uint32_t LL_USART_IsActiveFlag_TCBGT(const USART_TypeDef *USARTx)
{
  return ((((USARTx->ISR) & ((0x1UL << (25U)))) == ((0x1UL << (25U)))) ? 1UL : 0UL);
}






 
static inline void LL_USART_ClearFlag_PE(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (0U))));
}






 
static inline void LL_USART_ClearFlag_FE(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (1U))));
}






 
static inline void LL_USART_ClearFlag_NE(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (2U))));
}






 
static inline void LL_USART_ClearFlag_ORE(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (3U))));
}






 
static inline void LL_USART_ClearFlag_IDLE(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (4U))));
}






 
static inline void LL_USART_ClearFlag_TC(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (6U))));
}

 





 
static inline void LL_USART_ClearFlag_TCBGT(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (7U))));
}








 
static inline void LL_USART_ClearFlag_LBD(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (8U))));
}








 
static inline void LL_USART_ClearFlag_nCTS(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (9U))));
}






 
static inline void LL_USART_ClearFlag_RTO(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (11U))));
}








 
static inline void LL_USART_ClearFlag_EOB(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (12U))));
}






 
static inline void LL_USART_ClearFlag_CM(USART_TypeDef *USARTx)
{
  ((USARTx->ICR) = ((0x1UL << (17U))));
}



 



 






 
static inline void LL_USART_EnableIT_IDLE(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (4U)));
}






 
static inline void LL_USART_EnableIT_RXNE(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (5U)));
}






 
static inline void LL_USART_EnableIT_TC(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (6U)));
}






 
static inline void LL_USART_EnableIT_TXE(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (7U)));
}






 
static inline void LL_USART_EnableIT_PE(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (8U)));
}






 
static inline void LL_USART_EnableIT_CM(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (14U)));
}






 
static inline void LL_USART_EnableIT_RTO(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (26U)));
}








 
static inline void LL_USART_EnableIT_EOB(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR1, (0x1UL << (27U)));
}








 
static inline void LL_USART_EnableIT_LBD(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) |= ((0x1UL << (6U))));
}










 
static inline void LL_USART_EnableIT_ERROR(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR3, (0x1UL << (0U)));
}








 
static inline void LL_USART_EnableIT_CTS(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR3, (0x1UL << (10U)));
}

 







 
static inline void LL_USART_EnableIT_TCBGT(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR3, (0x1UL << (24U)));
}






 
static inline void LL_USART_DisableIT_IDLE(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (4U)));
}






 
static inline void LL_USART_DisableIT_RXNE(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (5U)));
}






 
static inline void LL_USART_DisableIT_TC(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (6U)));
}






 
static inline void LL_USART_DisableIT_TXE(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (7U)));
}






 
static inline void LL_USART_DisableIT_PE(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (8U)));
}






 
static inline void LL_USART_DisableIT_CM(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (14U)));
}






 
static inline void LL_USART_DisableIT_RTO(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (26U)));
}








 
static inline void LL_USART_DisableIT_EOB(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR1, (0x1UL << (27U)));
}








 
static inline void LL_USART_DisableIT_LBD(USART_TypeDef *USARTx)
{
  ((USARTx->CR2) &= ~((0x1UL << (6U))));
}










 
static inline void LL_USART_DisableIT_ERROR(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR3, (0x1UL << (0U)));
}








 
static inline void LL_USART_DisableIT_CTS(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR3, (0x1UL << (10U)));
}

 







 
static inline void LL_USART_DisableIT_TCBGT(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR3, (0x1UL << (24U)));
}






 
static inline uint32_t LL_USART_IsEnabledIT_IDLE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsEnabledIT_RXNE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1U : 0U);
}






 
static inline uint32_t LL_USART_IsEnabledIT_TC(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsEnabledIT_TXE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1U : 0U);
}






 
static inline uint32_t LL_USART_IsEnabledIT_PE(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsEnabledIT_CM(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (14U)))) == ((0x1UL << (14U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsEnabledIT_RTO(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (26U)))) == ((0x1UL << (26U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsEnabledIT_EOB(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR1) & ((0x1UL << (27U)))) == ((0x1UL << (27U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsEnabledIT_LBD(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR2) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline uint32_t LL_USART_IsEnabledIT_ERROR(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}








 
static inline uint32_t LL_USART_IsEnabledIT_CTS(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}

 







 
static inline uint32_t LL_USART_IsEnabledIT_TCBGT(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (24U)))) == ((0x1UL << (24U)))) ? 1UL : 0UL);
}



 



 






 
static inline void LL_USART_EnableDMAReq_RX(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR3, (0x1UL << (6U)));
}






 
static inline void LL_USART_DisableDMAReq_RX(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR3, (0x1UL << (6U)));
}






 
static inline uint32_t LL_USART_IsEnabledDMAReq_RX(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}






 
static inline void LL_USART_EnableDMAReq_TX(USART_TypeDef *USARTx)
{
  ATOMIC_SET_BIT(USARTx->CR3, (0x1UL << (7U)));
}






 
static inline void LL_USART_DisableDMAReq_TX(USART_TypeDef *USARTx)
{
  ATOMIC_CLEAR_BIT(USARTx->CR3, (0x1UL << (7U)));
}






 
static inline uint32_t LL_USART_IsEnabledDMAReq_TX(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}






 
static inline void LL_USART_EnableDMADeactOnRxErr(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) |= ((0x1UL << (13U))));
}






 
static inline void LL_USART_DisableDMADeactOnRxErr(USART_TypeDef *USARTx)
{
  ((USARTx->CR3) &= ~((0x1UL << (13U))));
}






 
static inline uint32_t LL_USART_IsEnabledDMADeactOnRxErr(const USART_TypeDef *USARTx)
{
  return ((((USARTx->CR3) & ((0x1UL << (13U)))) == ((0x1UL << (13U)))) ? 1UL : 0UL);
}










 
static inline uint32_t LL_USART_DMA_GetRegAddr(const USART_TypeDef *USARTx, uint32_t Direction)
{
  uint32_t data_reg_addr;

  if (Direction == 0x00000000U)
  {
     
    data_reg_addr = (uint32_t) &(USARTx->TDR);
  }
  else
  {
     
    data_reg_addr = (uint32_t) &(USARTx->RDR);
  }

  return data_reg_addr;
}



 



 






 
static inline uint8_t LL_USART_ReceiveData8(const USART_TypeDef *USARTx)
{
  return (uint8_t)(((USARTx->RDR) & ((0x1FFUL << (0U)))) & 0xFFU);
}






 
static inline uint16_t LL_USART_ReceiveData9(const USART_TypeDef *USARTx)
{
  return (uint16_t)(((USARTx->RDR) & ((0x1FFUL << (0U)))));
}







 
static inline void LL_USART_TransmitData8(USART_TypeDef *USARTx, uint8_t Value)
{
  USARTx->TDR = Value;
}







 
static inline void LL_USART_TransmitData9(USART_TypeDef *USARTx, uint16_t Value)
{
  USARTx->TDR = (uint16_t)(Value & 0x1FFUL);
}



 



 








 
static inline void LL_USART_RequestAutoBaudRate(USART_TypeDef *USARTx)
{
  ((USARTx->RQR) |= ((uint16_t)(0x1UL << (0U))));
}






 
static inline void LL_USART_RequestBreakSending(USART_TypeDef *USARTx)
{
  ((USARTx->RQR) |= ((uint16_t)(0x1UL << (1U))));
}






 
static inline void LL_USART_RequestEnterMuteMode(USART_TypeDef *USARTx)
{
  ((USARTx->RQR) |= ((uint16_t)(0x1UL << (2U))));
}








 
static inline void LL_USART_RequestRxDataFlush(USART_TypeDef *USARTx)
{
  ((USARTx->RQR) |= ((uint16_t)(0x1UL << (3U))));
}








 
static inline void LL_USART_RequestTxDataFlush(USART_TypeDef *USARTx)
{
  ((USARTx->RQR) |= ((uint16_t)(0x1UL << (4U))));
}



 



 
ErrorStatus LL_USART_DeInit(const USART_TypeDef *USARTx);
ErrorStatus LL_USART_Init(USART_TypeDef *USARTx, const LL_USART_InitTypeDef *USART_InitStruct);
void        LL_USART_StructInit(LL_USART_InitTypeDef *USART_InitStruct);
ErrorStatus LL_USART_ClockInit(USART_TypeDef *USARTx, const LL_USART_ClockInitTypeDef *USART_ClockInitStruct);
void        LL_USART_ClockStructInit(LL_USART_ClockInitTypeDef *USART_ClockInitStruct);


 



 



 




 




 
 

 

 



typedef  struct  rem_buf {
    unsigned char used;
    unsigned char data[16];                          
    unsigned char len;                                
} REM_BUF;                                        



 
 












 

 
 

 

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

 
void Error_Handler(void);



void eeprom_init();
uint8_t eeprom_read_byte(uint16_t eeprom_address);
void eeprom_write_byte(uint16_t eeprom_address, unsigned char value);


 

 

 
 

 



 





uint8_t txStandardflashInternalBuffer[4096];






void SPI_Delay(uint32_t delayTime);
uint8_t SPI1_RW(uint8_t txByte);



void SPI_Exchange(uint8_t *txBuffer,
				  uint32_t txNumBytes,
				  uint8_t *rxBuffer,
				  uint32_t rxNumBytes,
				  uint32_t dummyNumBytes);



void load4BytesToTxBuffer(uint8_t *txBuffer, uint8_t opcode, uint32_t address)
{
	txBuffer[0] = opcode;
	txBuffer[1] = (uint8_t) (address >> 16);
	txBuffer[2] = (uint8_t) (address >> 8);
	txBuffer[3] = (uint8_t) address;
} 




void fillArrayPattern(uint8_t * byteArray, uint32_t numBytes, int seedNumber)
{
	for(int i = 0; i < numBytes; i++)
	{
		byteArray[i] = (uint8_t)(seedNumber+i);
	}
}

void fillArrayConst(uint8_t * byteArray, uint32_t numBytes, int constantNum)
{
	for(int i = 0; i < numBytes; i++)
	{
		byteArray[i] = (uint8_t)(constantNum);
	}
} 





int compareByteArrays(uint8_t *arr1, uint8_t *arr2, uint32_t arrLength)
{
	uint32_t numErrors = 0;
	for(int i = 0; i < arrLength; i++)
	{
		if(arr1[i] != arr2[i])
		{
			numErrors++;
		}
	}
	return (numErrors == 0) ? 1 : 0;
} 










void standardflashWaitOnReady()
{
	uint8_t SRArray[2] = {0, 0};
	do
	{
		SRArray[0] = standardflashReadSRB1();
		SPI_Delay(10);
	}
	while(SRArray[0] & (1<<0));
}

void standardflashSetQEBit()
{
	uint8_t SRArray[2] = {0, 0};
	
	SRArray[0] = standardflashReadSRB1();
	SRArray[1] = standardflashReadSRB2();
	
	SRArray[1] |= (1 << 1);
	
	standardflashWriteEnable();
	
	
	standardflashWriteSR(SRArray, 2);
}

void standardflashClearQEBit()
{
	uint8_t SRArray[2] = {0, 0};
	
	SRArray[0] = standardflashReadSRB1();
	SRArray[1] = standardflashReadSRB2();
	
	SRArray[1] &= ~(1 << 1);
	
	standardflashWriteEnable();
	
	
	standardflashWriteSR(SRArray, 2);
}

void standardflashWriteEnable()
{
	txStandardflashInternalBuffer[0] = 0x06;

	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);

}

void standardflashWriteDisable()
{
	txStandardflashInternalBuffer[0] = 0x04;

        SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);

}

void standardflashReadArrayLowFreq(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x03, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, rxBuffer, rxNumBytes, 0);
}

void standardflashReadArrayHighFreq(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x0B, address);

	SPI_Exchange(txStandardflashInternalBuffer, 4, rxBuffer, rxNumBytes, 1);

}

void standardflashBytePageProgram(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x02, address);
	
	uint32_t totalBytes = txNumBytes + 4;

	for(uint32_t j = 0; j < txNumBytes; j++)
	{
		txStandardflashInternalBuffer[j+4] = txBuffer[j];
	}

	SPI_Exchange(txStandardflashInternalBuffer, totalBytes, 0, 0, 0);

}

void standardflashBlockErase4K(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x20, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, 0, 0, 0);
}

void standardflashBlockErase32K(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x52, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, 0, 0, 0);
}

void standardflashBlockErase64K(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0xD8, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, 0, 0, 0);
}

void standardflashChipErase1()
{
	txStandardflashInternalBuffer[0] = 0x60;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashChipErase2()
{
	txStandardflashInternalBuffer[0] = 0xC7;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashDPD()
{
	txStandardflashInternalBuffer[0] = 0xB9;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashResumeFromDPD()
{
	txStandardflashInternalBuffer[0] = 0xAB;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashReadID(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = 0x90;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 2, 3);
}

void standardflashReadMID(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = 0x9F;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 3, 0);
}

void standardflashWriteSR(uint8_t *txBuffer, uint8_t txNumBytes)
{
	txStandardflashInternalBuffer[0] = 0x01;
	txStandardflashInternalBuffer[1] = txBuffer[0];
	if(txNumBytes > 1)
	{
		txStandardflashInternalBuffer[2] = txBuffer[1];
	}
	SPI_Exchange(txStandardflashInternalBuffer, txNumBytes+1, 0, 0, 0);
}


void standardflashWriteSRB1(uint8_t regVal)
{
	txStandardflashInternalBuffer[0] = 0x01;
	txStandardflashInternalBuffer[1] = regVal;
	SPI_Exchange(txStandardflashInternalBuffer, 2, 0, 0, 0);
}

void standardflashWriteSRB2(uint8_t regVal)
{
	txStandardflashInternalBuffer[0] = 0x31;
	txStandardflashInternalBuffer[1] = regVal;
	SPI_Exchange(txStandardflashInternalBuffer, 2, 0, 0, 0);
}

void standardflashWriteEnableVolatileSR()
{
	txStandardflashInternalBuffer[0] = 0x50;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

uint8_t standardflashReadSRB1()
{
	uint8_t regVal = 0;
	txStandardflashInternalBuffer[0] = 0x05;
	SPI_Exchange(txStandardflashInternalBuffer, 1, &regVal, 1, 0);
	return regVal;
}

uint8_t standardflashReadSRB2()
{
	uint8_t regVal = 0;
	txStandardflashInternalBuffer[0] = 0x35;
	SPI_Exchange(txStandardflashInternalBuffer, 1, &regVal, 1, 0);
	return regVal;
}








void standardflashEraseSecurityRegister(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x44, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, 0, 0, 0);
}

void standardflashProgramSecurityRegisters(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x42, address);
	
	uint32_t totalBytes = txNumBytes + 4;

	for(uint32_t j = 0; j < txNumBytes; j++)
	{
		txStandardflashInternalBuffer[j+4] = txBuffer[j];
	}

	SPI_Exchange(txStandardflashInternalBuffer, totalBytes, 0, 0, 0);
}

void standardflashReadSecurityRegisters(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, 0x48, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, rxBuffer, rxNumBytes, 1);
}

void standardflashResumeFromDPDReadID(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = 0xAB;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 1, 3);
}





void standardflashEraseProgramSuspend()
{
	txStandardflashInternalBuffer[0] = (uint8_t) 0x75;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashEraseProgramResume()
{
	txStandardflashInternalBuffer[0] = (uint8_t) 0x7A;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}





void standardflashEnableReset()
{
	txStandardflashInternalBuffer[0] = (uint8_t) 0x66;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashReset()
{
	txStandardflashInternalBuffer[0] = (uint8_t) 0x99;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashEnterSecureOTP()
{
	txStandardflashInternalBuffer[0] = (uint8_t) 0xB1;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}

void standardflashExitSecuredOTP()
{
	txStandardflashInternalBuffer[0] = (uint8_t) 0xC1;
	SPI_Exchange(txStandardflashInternalBuffer, 1, 0, 0, 0);
}



