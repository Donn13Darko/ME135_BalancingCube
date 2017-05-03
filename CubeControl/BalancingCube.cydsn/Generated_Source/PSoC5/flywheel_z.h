/*******************************************************************************
* File Name: flywheel_z.h
* Version 3.30
*
* Description:
*  Contains the prototypes and constants for the functions available to the
*  PWM user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_PWM_flywheel_z_H)
#define CY_PWM_flywheel_z_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 flywheel_z_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define flywheel_z_Resolution                     (8u)
#define flywheel_z_UsingFixedFunction             (0u)
#define flywheel_z_DeadBandMode                   (0u)
#define flywheel_z_KillModeMinTime                (0u)
#define flywheel_z_KillMode                       (0u)
#define flywheel_z_PWMMode                        (1u)
#define flywheel_z_PWMModeIsCenterAligned         (0u)
#define flywheel_z_DeadBandUsed                   (0u)
#define flywheel_z_DeadBand2_4                    (0u)

#if !defined(flywheel_z_PWMUDB_genblk8_stsreg__REMOVED)
    #define flywheel_z_UseStatus                  (1u)
#else
    #define flywheel_z_UseStatus                  (0u)
#endif /* !defined(flywheel_z_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(flywheel_z_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define flywheel_z_UseControl                 (1u)
#else
    #define flywheel_z_UseControl                 (0u)
#endif /* !defined(flywheel_z_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define flywheel_z_UseOneCompareMode              (0u)
#define flywheel_z_MinimumKillTime                (1u)
#define flywheel_z_EnableMode                     (0u)

#define flywheel_z_CompareMode1SW                 (0u)
#define flywheel_z_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define flywheel_z__B_PWM__DISABLED 0
#define flywheel_z__B_PWM__ASYNCHRONOUS 1
#define flywheel_z__B_PWM__SINGLECYCLE 2
#define flywheel_z__B_PWM__LATCHED 3
#define flywheel_z__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define flywheel_z__B_PWM__DBMDISABLED 0
#define flywheel_z__B_PWM__DBM_2_4_CLOCKS 1
#define flywheel_z__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define flywheel_z__B_PWM__ONE_OUTPUT 0
#define flywheel_z__B_PWM__TWO_OUTPUTS 1
#define flywheel_z__B_PWM__DUAL_EDGE 2
#define flywheel_z__B_PWM__CENTER_ALIGN 3
#define flywheel_z__B_PWM__DITHER 5
#define flywheel_z__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define flywheel_z__B_PWM__LESS_THAN 1
#define flywheel_z__B_PWM__LESS_THAN_OR_EQUAL 2
#define flywheel_z__B_PWM__GREATER_THAN 3
#define flywheel_z__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define flywheel_z__B_PWM__EQUAL 0
#define flywheel_z__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!flywheel_z_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!flywheel_z_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!flywheel_z_PWMModeIsCenterAligned) */
        #if (flywheel_z_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (flywheel_z_UseStatus) */

        /* Backup for Deadband parameters */
        #if(flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_256_CLOCKS || \
            flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(flywheel_z_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (flywheel_z_KillModeMinTime) */

        /* Backup control register */
        #if(flywheel_z_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (flywheel_z_UseControl) */

    #endif /* (!flywheel_z_UsingFixedFunction) */

}flywheel_z_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    flywheel_z_Start(void) ;
void    flywheel_z_Stop(void) ;

#if (flywheel_z_UseStatus || flywheel_z_UsingFixedFunction)
    void  flywheel_z_SetInterruptMode(uint8 interruptMode) ;
    uint8 flywheel_z_ReadStatusRegister(void) ;
#endif /* (flywheel_z_UseStatus || flywheel_z_UsingFixedFunction) */

#define flywheel_z_GetInterruptSource() flywheel_z_ReadStatusRegister()

#if (flywheel_z_UseControl)
    uint8 flywheel_z_ReadControlRegister(void) ;
    void  flywheel_z_WriteControlRegister(uint8 control)
          ;
#endif /* (flywheel_z_UseControl) */

#if (flywheel_z_UseOneCompareMode)
   #if (flywheel_z_CompareMode1SW)
       void    flywheel_z_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (flywheel_z_CompareMode1SW) */
#else
    #if (flywheel_z_CompareMode1SW)
        void    flywheel_z_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (flywheel_z_CompareMode1SW) */
    #if (flywheel_z_CompareMode2SW)
        void    flywheel_z_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (flywheel_z_CompareMode2SW) */
#endif /* (flywheel_z_UseOneCompareMode) */

#if (!flywheel_z_UsingFixedFunction)
    uint8   flywheel_z_ReadCounter(void) ;
    uint8 flywheel_z_ReadCapture(void) ;

    #if (flywheel_z_UseStatus)
            void flywheel_z_ClearFIFO(void) ;
    #endif /* (flywheel_z_UseStatus) */

    void    flywheel_z_WriteCounter(uint8 counter)
            ;
#endif /* (!flywheel_z_UsingFixedFunction) */

void    flywheel_z_WritePeriod(uint8 period)
        ;
uint8 flywheel_z_ReadPeriod(void) ;

#if (flywheel_z_UseOneCompareMode)
    void    flywheel_z_WriteCompare(uint8 compare)
            ;
    uint8 flywheel_z_ReadCompare(void) ;
#else
    void    flywheel_z_WriteCompare1(uint8 compare)
            ;
    uint8 flywheel_z_ReadCompare1(void) ;
    void    flywheel_z_WriteCompare2(uint8 compare)
            ;
    uint8 flywheel_z_ReadCompare2(void) ;
#endif /* (flywheel_z_UseOneCompareMode) */


#if (flywheel_z_DeadBandUsed)
    void    flywheel_z_WriteDeadTime(uint8 deadtime) ;
    uint8   flywheel_z_ReadDeadTime(void) ;
#endif /* (flywheel_z_DeadBandUsed) */

#if ( flywheel_z_KillModeMinTime)
    void flywheel_z_WriteKillTime(uint8 killtime) ;
    uint8 flywheel_z_ReadKillTime(void) ;
#endif /* ( flywheel_z_KillModeMinTime) */

void flywheel_z_Init(void) ;
void flywheel_z_Enable(void) ;
void flywheel_z_Sleep(void) ;
void flywheel_z_Wakeup(void) ;
void flywheel_z_SaveConfig(void) ;
void flywheel_z_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define flywheel_z_INIT_PERIOD_VALUE          (255u)
#define flywheel_z_INIT_COMPARE_VALUE1        (127u)
#define flywheel_z_INIT_COMPARE_VALUE2        (63u)
#define flywheel_z_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    flywheel_z_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    flywheel_z_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    flywheel_z_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    flywheel_z_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define flywheel_z_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  flywheel_z_CTRL_CMPMODE2_SHIFT)
#define flywheel_z_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  flywheel_z_CTRL_CMPMODE1_SHIFT)
#define flywheel_z_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (flywheel_z_UsingFixedFunction)
   #define flywheel_z_PERIOD_LSB              (*(reg16 *) flywheel_z_PWMHW__PER0)
   #define flywheel_z_PERIOD_LSB_PTR          ( (reg16 *) flywheel_z_PWMHW__PER0)
   #define flywheel_z_COMPARE1_LSB            (*(reg16 *) flywheel_z_PWMHW__CNT_CMP0)
   #define flywheel_z_COMPARE1_LSB_PTR        ( (reg16 *) flywheel_z_PWMHW__CNT_CMP0)
   #define flywheel_z_COMPARE2_LSB            (0x00u)
   #define flywheel_z_COMPARE2_LSB_PTR        (0x00u)
   #define flywheel_z_COUNTER_LSB             (*(reg16 *) flywheel_z_PWMHW__CNT_CMP0)
   #define flywheel_z_COUNTER_LSB_PTR         ( (reg16 *) flywheel_z_PWMHW__CNT_CMP0)
   #define flywheel_z_CAPTURE_LSB             (*(reg16 *) flywheel_z_PWMHW__CAP0)
   #define flywheel_z_CAPTURE_LSB_PTR         ( (reg16 *) flywheel_z_PWMHW__CAP0)
   #define flywheel_z_RT1                     (*(reg8 *)  flywheel_z_PWMHW__RT1)
   #define flywheel_z_RT1_PTR                 ( (reg8 *)  flywheel_z_PWMHW__RT1)

#else
   #if (flywheel_z_Resolution == 8u) /* 8bit - PWM */

       #if(flywheel_z_PWMModeIsCenterAligned)
           #define flywheel_z_PERIOD_LSB      (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define flywheel_z_PERIOD_LSB_PTR  ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define flywheel_z_PERIOD_LSB      (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define flywheel_z_PERIOD_LSB_PTR  ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (flywheel_z_PWMModeIsCenterAligned) */

       #define flywheel_z_COMPARE1_LSB        (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define flywheel_z_COMPARE1_LSB_PTR    ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define flywheel_z_COMPARE2_LSB        (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define flywheel_z_COMPARE2_LSB_PTR    ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define flywheel_z_COUNTERCAP_LSB      (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define flywheel_z_COUNTERCAP_LSB_PTR  ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define flywheel_z_COUNTER_LSB         (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define flywheel_z_COUNTER_LSB_PTR     ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define flywheel_z_CAPTURE_LSB         (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define flywheel_z_CAPTURE_LSB_PTR     ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(flywheel_z_PWMModeIsCenterAligned)
               #define flywheel_z_PERIOD_LSB      (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define flywheel_z_PERIOD_LSB_PTR  ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define flywheel_z_PERIOD_LSB      (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define flywheel_z_PERIOD_LSB_PTR  ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (flywheel_z_PWMModeIsCenterAligned) */

            #define flywheel_z_COMPARE1_LSB       (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define flywheel_z_COMPARE1_LSB_PTR   ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define flywheel_z_COMPARE2_LSB       (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define flywheel_z_COMPARE2_LSB_PTR   ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define flywheel_z_COUNTERCAP_LSB     (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define flywheel_z_COUNTERCAP_LSB_PTR ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define flywheel_z_COUNTER_LSB        (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define flywheel_z_COUNTER_LSB_PTR    ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define flywheel_z_CAPTURE_LSB        (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define flywheel_z_CAPTURE_LSB_PTR    ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(flywheel_z_PWMModeIsCenterAligned)
               #define flywheel_z_PERIOD_LSB      (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define flywheel_z_PERIOD_LSB_PTR  ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define flywheel_z_PERIOD_LSB      (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define flywheel_z_PERIOD_LSB_PTR  ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (flywheel_z_PWMModeIsCenterAligned) */

            #define flywheel_z_COMPARE1_LSB       (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define flywheel_z_COMPARE1_LSB_PTR   ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define flywheel_z_COMPARE2_LSB       (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define flywheel_z_COMPARE2_LSB_PTR   ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define flywheel_z_COUNTERCAP_LSB     (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define flywheel_z_COUNTERCAP_LSB_PTR ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define flywheel_z_COUNTER_LSB        (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define flywheel_z_COUNTER_LSB_PTR    ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define flywheel_z_CAPTURE_LSB        (*(reg16 *) flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define flywheel_z_CAPTURE_LSB_PTR    ((reg16 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define flywheel_z_AUX_CONTROLDP1          (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define flywheel_z_AUX_CONTROLDP1_PTR      ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (flywheel_z_Resolution == 8) */

   #define flywheel_z_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define flywheel_z_AUX_CONTROLDP0          (*(reg8 *)  flywheel_z_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define flywheel_z_AUX_CONTROLDP0_PTR      ((reg8 *)   flywheel_z_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (flywheel_z_UsingFixedFunction) */

#if(flywheel_z_KillModeMinTime )
    #define flywheel_z_KILLMODEMINTIME        (*(reg8 *)  flywheel_z_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define flywheel_z_KILLMODEMINTIME_PTR    ((reg8 *)   flywheel_z_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (flywheel_z_KillModeMinTime ) */

#if(flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_256_CLOCKS)
    #define flywheel_z_DEADBAND_COUNT         (*(reg8 *)  flywheel_z_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define flywheel_z_DEADBAND_COUNT_PTR     ((reg8 *)   flywheel_z_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define flywheel_z_DEADBAND_LSB_PTR       ((reg8 *)   flywheel_z_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define flywheel_z_DEADBAND_LSB           (*(reg8 *)  flywheel_z_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (flywheel_z_UsingFixedFunction)
        #define flywheel_z_DEADBAND_COUNT         (*(reg8 *)  flywheel_z_PWMHW__CFG0)
        #define flywheel_z_DEADBAND_COUNT_PTR     ((reg8 *)   flywheel_z_PWMHW__CFG0)
        #define flywheel_z_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << flywheel_z_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define flywheel_z_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define flywheel_z_DEADBAND_COUNT         (*(reg8 *)  flywheel_z_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define flywheel_z_DEADBAND_COUNT_PTR     ((reg8 *)   flywheel_z_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define flywheel_z_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << flywheel_z_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define flywheel_z_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (flywheel_z_UsingFixedFunction) */
#endif /* (flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_256_CLOCKS) */



#if (flywheel_z_UsingFixedFunction)
    #define flywheel_z_STATUS                 (*(reg8 *) flywheel_z_PWMHW__SR0)
    #define flywheel_z_STATUS_PTR             ((reg8 *) flywheel_z_PWMHW__SR0)
    #define flywheel_z_STATUS_MASK            (*(reg8 *) flywheel_z_PWMHW__SR0)
    #define flywheel_z_STATUS_MASK_PTR        ((reg8 *) flywheel_z_PWMHW__SR0)
    #define flywheel_z_CONTROL                (*(reg8 *) flywheel_z_PWMHW__CFG0)
    #define flywheel_z_CONTROL_PTR            ((reg8 *) flywheel_z_PWMHW__CFG0)
    #define flywheel_z_CONTROL2               (*(reg8 *) flywheel_z_PWMHW__CFG1)
    #define flywheel_z_CONTROL3               (*(reg8 *) flywheel_z_PWMHW__CFG2)
    #define flywheel_z_GLOBAL_ENABLE          (*(reg8 *) flywheel_z_PWMHW__PM_ACT_CFG)
    #define flywheel_z_GLOBAL_ENABLE_PTR      ( (reg8 *) flywheel_z_PWMHW__PM_ACT_CFG)
    #define flywheel_z_GLOBAL_STBY_ENABLE     (*(reg8 *) flywheel_z_PWMHW__PM_STBY_CFG)
    #define flywheel_z_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) flywheel_z_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define flywheel_z_BLOCK_EN_MASK          (flywheel_z_PWMHW__PM_ACT_MSK)
    #define flywheel_z_BLOCK_STBY_EN_MASK     (flywheel_z_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define flywheel_z_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define flywheel_z_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define flywheel_z_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define flywheel_z_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define flywheel_z_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define flywheel_z_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define flywheel_z_CTRL_ENABLE            (uint8)((uint8)0x01u << flywheel_z_CTRL_ENABLE_SHIFT)
    #define flywheel_z_CTRL_RESET             (uint8)((uint8)0x01u << flywheel_z_CTRL_RESET_SHIFT)
    #define flywheel_z_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << flywheel_z_CTRL_CMPMODE2_SHIFT)
    #define flywheel_z_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << flywheel_z_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define flywheel_z_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define flywheel_z_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << flywheel_z_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define flywheel_z_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define flywheel_z_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define flywheel_z_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define flywheel_z_STATUS_TC_INT_EN_MASK_SHIFT            (flywheel_z_STATUS_TC_SHIFT - 4u)
    #define flywheel_z_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define flywheel_z_STATUS_CMP1_INT_EN_MASK_SHIFT          (flywheel_z_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define flywheel_z_STATUS_TC              (uint8)((uint8)0x01u << flywheel_z_STATUS_TC_SHIFT)
    #define flywheel_z_STATUS_CMP1            (uint8)((uint8)0x01u << flywheel_z_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define flywheel_z_STATUS_TC_INT_EN_MASK              (uint8)((uint8)flywheel_z_STATUS_TC >> 4u)
    #define flywheel_z_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)flywheel_z_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define flywheel_z_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define flywheel_z_RT1_MASK              (uint8)((uint8)0x03u << flywheel_z_RT1_SHIFT)
    #define flywheel_z_SYNC                  (uint8)((uint8)0x03u << flywheel_z_RT1_SHIFT)
    #define flywheel_z_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define flywheel_z_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << flywheel_z_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define flywheel_z_SYNCDSI_EN            (uint8)((uint8)0x0Fu << flywheel_z_SYNCDSI_SHIFT)


#else
    #define flywheel_z_STATUS                (*(reg8 *)   flywheel_z_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define flywheel_z_STATUS_PTR            ((reg8 *)    flywheel_z_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define flywheel_z_STATUS_MASK           (*(reg8 *)   flywheel_z_PWMUDB_genblk8_stsreg__MASK_REG)
    #define flywheel_z_STATUS_MASK_PTR       ((reg8 *)    flywheel_z_PWMUDB_genblk8_stsreg__MASK_REG)
    #define flywheel_z_STATUS_AUX_CTRL       (*(reg8 *)   flywheel_z_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define flywheel_z_CONTROL               (*(reg8 *)   flywheel_z_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define flywheel_z_CONTROL_PTR           ((reg8 *)    flywheel_z_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define flywheel_z_CTRL_ENABLE_SHIFT      (0x07u)
    #define flywheel_z_CTRL_RESET_SHIFT       (0x06u)
    #define flywheel_z_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define flywheel_z_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define flywheel_z_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define flywheel_z_CTRL_ENABLE            (uint8)((uint8)0x01u << flywheel_z_CTRL_ENABLE_SHIFT)
    #define flywheel_z_CTRL_RESET             (uint8)((uint8)0x01u << flywheel_z_CTRL_RESET_SHIFT)
    #define flywheel_z_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << flywheel_z_CTRL_CMPMODE2_SHIFT)
    #define flywheel_z_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << flywheel_z_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define flywheel_z_STATUS_KILL_SHIFT          (0x05u)
    #define flywheel_z_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define flywheel_z_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define flywheel_z_STATUS_TC_SHIFT            (0x02u)
    #define flywheel_z_STATUS_CMP2_SHIFT          (0x01u)
    #define flywheel_z_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define flywheel_z_STATUS_KILL_INT_EN_MASK_SHIFT          (flywheel_z_STATUS_KILL_SHIFT)
    #define flywheel_z_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (flywheel_z_STATUS_FIFONEMPTY_SHIFT)
    #define flywheel_z_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (flywheel_z_STATUS_FIFOFULL_SHIFT)
    #define flywheel_z_STATUS_TC_INT_EN_MASK_SHIFT            (flywheel_z_STATUS_TC_SHIFT)
    #define flywheel_z_STATUS_CMP2_INT_EN_MASK_SHIFT          (flywheel_z_STATUS_CMP2_SHIFT)
    #define flywheel_z_STATUS_CMP1_INT_EN_MASK_SHIFT          (flywheel_z_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define flywheel_z_STATUS_KILL            (uint8)((uint8)0x00u << flywheel_z_STATUS_KILL_SHIFT )
    #define flywheel_z_STATUS_FIFOFULL        (uint8)((uint8)0x01u << flywheel_z_STATUS_FIFOFULL_SHIFT)
    #define flywheel_z_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << flywheel_z_STATUS_FIFONEMPTY_SHIFT)
    #define flywheel_z_STATUS_TC              (uint8)((uint8)0x01u << flywheel_z_STATUS_TC_SHIFT)
    #define flywheel_z_STATUS_CMP2            (uint8)((uint8)0x01u << flywheel_z_STATUS_CMP2_SHIFT)
    #define flywheel_z_STATUS_CMP1            (uint8)((uint8)0x01u << flywheel_z_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define flywheel_z_STATUS_KILL_INT_EN_MASK            (flywheel_z_STATUS_KILL)
    #define flywheel_z_STATUS_FIFOFULL_INT_EN_MASK        (flywheel_z_STATUS_FIFOFULL)
    #define flywheel_z_STATUS_FIFONEMPTY_INT_EN_MASK      (flywheel_z_STATUS_FIFONEMPTY)
    #define flywheel_z_STATUS_TC_INT_EN_MASK              (flywheel_z_STATUS_TC)
    #define flywheel_z_STATUS_CMP2_INT_EN_MASK            (flywheel_z_STATUS_CMP2)
    #define flywheel_z_STATUS_CMP1_INT_EN_MASK            (flywheel_z_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define flywheel_z_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define flywheel_z_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define flywheel_z_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define flywheel_z_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define flywheel_z_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* flywheel_z_UsingFixedFunction */

#endif  /* CY_PWM_flywheel_z_H */


/* [] END OF FILE */
