#ifndef __DEF_TIMERS
#define __DEF_TIMERS

volatile QWORD getTick;
volatile QWORD temporary_tmr1_value;

#define mGetTick()                  ((!(temporary_tmr1_value = TMR1) | mT1GetIntFlag())?(temporary_tmr1_value + (getTick += (mT1GetIntFlag() << 16)) + (mT1GetIntFlag() = 0)):(temporary_tmr1_value + getTick))
#define mTickCompare(var)           (mGetTick() - var)
#define TICK_INIT                   (0ul)
#define TICK_0                      (0ul)
        
#if (PERIPHERAL_FREQ == 48000000L)
#define TICK_1US                    (6ul)
#define TICK_10US                   (60ul)
#define TICK_100US                  (600ul)
#define TICK_500US                  (3000ul)
#define TICK_1MS                    (6000ul)
#define TICK_2MS                    (12000ul)
#define TICK_3MS                    (18000ul)
#define TICK_4MS                    (24000ul)
#define TICK_5MS                    (30000ul)
#define TICK_6MS                    (36000ul)
#define TICK_8MS                    (48000ul)
#define TICK_10MS                   (60000ul)
#define TICK_20MS                   (120000ul)
#define TICK_30MS                   (180000ul)
#define TICK_40MS                   (240000ul)
#define TICK_50MS                   (300000ul)
#define TICK_60MS                   (360000ul)
#define TICK_70MS                   (420000ul)
#define TICK_80MS                   (480000ul)
#define TICK_90MS                   (540000ul)
#define TICK_100MS                  (600000ul)
#define TICK_200MS                  (1200000ul)
#define TICK_300MS                  (1800000ul)
#define TICK_400MS                  (2400000ul)
#define TICK_500MS                  (3000000ul)
#define TICK_1S                     (6000000ul)
#define TICK_1_5S                   (9000000ul)
#define TICK_2S                     (12000000ul)
#define TICK_3S                     (18000000ul)
#define TICK_4S                     (24000000ul)
#define TICK_5S                     (30000000ul)
#define TICK_10S                    (60000000ul)
#elif (PERIPHERAL_FREQ == 80000000L)
#define TICK_1US                    (10ul)
#define TICK_10US                   (100ul)
#define TICK_100US                  (1000ul)
#define TICK_300US                  (3000ul)
#define TICK_500US                  (5000ul)
#define TICK_1MS                    (10000ul)
#define TICK_2MS                    (20000ul)
#define TICK_3MS                    (30000ul)
#define TICK_4MS                    (40000ul)
#define TICK_5MS                    (50000ul)
#define TICK_6MS                    (60000ul)
#define TICK_8MS                    (80000ul)
#define TICK_10MS                   (100000ul)
#define TICK_20MS                   (200000ul)
#define TICK_30MS                   (300000ul)
#define TICK_40MS                   (400000ul)
#define TICK_50MS                   (500000ul)
#define TICK_60MS                   (600000ul)
#define TICK_70MS                   (700000ul)
#define TICK_80MS                   (800000ul)
#define TICK_90MS                   (900000ul)
#define TICK_100MS                  (1000000ul)
#define TICK_200MS                  (2000000ul)
#define TICK_300MS                  (3000000ul)
#define TICK_400MS                  (4000000ul)
#define TICK_500MS                  (5000000ul)
#define TICK_1S                     (10000000ul)
#define TICK_1_5S                   (15000000ul)
#define TICK_2S                     (20000000ul)
#define TICK_3S                     (30000000ul)
#define TICK_4S                     (40000000ul)
#define TICK_5S                     (50000000ul)
#define TICK_10S                    (100000000ul)
#else
#error "timer.h - value not define for getTick"
#endif

#define T_ON                        T1_ON               /* Timer ON */
#define T_OFF                       T1_OFF              /* Timer OFF */

#define T_IDLE_STOP                 T1_IDLE_STOP        /* stop during idle */
#define T_IDLE_CON                  T1_IDLE_CON         /* operate during idle */

#define T_TMWDIS_ON                 T1_TMWDIS_ON        /* Asynchronous Write Disable */
#define T_TMWDIS_OFF                T1_TMWDIS_OFF

#define T_GATE_ON                   T1_GATE_ON          /* Timer Gate accumulation mode ON */
#define T_GATE_OFF                  T1_GATE_OFF         /* Timer Gate accumulation mode OFF */

#define T_SYNC_EXT_ON               T1_SYNC_EXT_ON      /* Synch external clk input */
#define T_SYNC_EXT_OFF              T1_SYNC_EXT_OFF

#define T_SOURCE_EXT                T1_SOURCE_EXT       /* External clock source */
#define T_SOURCE_INT                T1_SOURCE_INT       /* Internal clock source */

#define T_32BIT_MODE_ON             T2_32BIT_MODE_ON    /* Enable 32-bit mode */
#define T_32BIT_MODE_OFF            T2_32BIT_MODE_OFF   /* Separate 16-bit timer */

// ----------------------------------------
// ---------- Defines of typedef ----------
// ----------------------------------------
typedef enum {
    TIMER1 = 0,
    TIMER2,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER_NUMBER_OF_MODULES   // Number of available TMR modules.
} TIMER_MODULE;
// ----------------------------------------
typedef union {
  struct {
    unsigned :1;
    unsigned TCS:1;
    unsigned :1;
    unsigned T32:1;
    unsigned TCKPS:3;
    unsigned TGATE:1;
    unsigned :5;
    unsigned SIDL:1;
    unsigned :1;
    unsigned TON:1;
  };
  struct {
    unsigned :4;
    unsigned TCKPS0:1;
    unsigned TCKPS1:1;
    unsigned TCKPS2:1;
  };
  struct {
    unsigned :13;
    unsigned TSIDL:1;
    unsigned :1;
    unsigned TON:1;
  };
  struct {
    unsigned w:32;
  };
} __TxCONbits_t;
// ----------------------------------------
typedef struct {
    union {
	volatile UINT32 TxCON;
	volatile __TxCONbits_t TxCONbits;
    };
    volatile UINT32 TxCONCLR;
    volatile UINT32 TxCONSET;
    volatile UINT32 TxCONINV;

    volatile UINT32 TMRx;
    volatile UINT32 TMRxCLR;
    volatile UINT32 TMRxSET;
    volatile UINT32 TMRxINV;

    volatile UINT32 PRx;
    volatile UINT32 PRxCLR;
    volatile UINT32 PRxSET;
    volatile UINT32 PRxINV;
} TIMER_REGISTERS;
// ----------------------------------------

void TimerInit2345xUs(TIMER_MODULE mTimerModule, DWORD config, double periodUs);
double TimerGetPeriodUs(TIMER_MODULE mTimerModule);
#define TimerInit2345xHz(mTimerModule, config, freqHz)  (TimerInit2345xUs(mTimerModule, config, (double)(1000000.0/freqHz)))
#define TimerGetFrequence(mTimerModule) (double)(1000000.0/TimerGetPeriodUs(mTimerModule));
	
#endif
