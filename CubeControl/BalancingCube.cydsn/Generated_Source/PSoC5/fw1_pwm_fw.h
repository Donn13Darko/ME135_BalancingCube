/*******************************************************************************
* File Name: fw1_pwm_fw.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_fw1_pwm_fw_H) /* Pins fw1_pwm_fw_H */
#define CY_PINS_fw1_pwm_fw_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "fw1_pwm_fw_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 fw1_pwm_fw__PORT == 15 && ((fw1_pwm_fw__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    fw1_pwm_fw_Write(uint8 value);
void    fw1_pwm_fw_SetDriveMode(uint8 mode);
uint8   fw1_pwm_fw_ReadDataReg(void);
uint8   fw1_pwm_fw_Read(void);
void    fw1_pwm_fw_SetInterruptMode(uint16 position, uint16 mode);
uint8   fw1_pwm_fw_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the fw1_pwm_fw_SetDriveMode() function.
     *  @{
     */
        #define fw1_pwm_fw_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define fw1_pwm_fw_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define fw1_pwm_fw_DM_RES_UP          PIN_DM_RES_UP
        #define fw1_pwm_fw_DM_RES_DWN         PIN_DM_RES_DWN
        #define fw1_pwm_fw_DM_OD_LO           PIN_DM_OD_LO
        #define fw1_pwm_fw_DM_OD_HI           PIN_DM_OD_HI
        #define fw1_pwm_fw_DM_STRONG          PIN_DM_STRONG
        #define fw1_pwm_fw_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define fw1_pwm_fw_MASK               fw1_pwm_fw__MASK
#define fw1_pwm_fw_SHIFT              fw1_pwm_fw__SHIFT
#define fw1_pwm_fw_WIDTH              1u

/* Interrupt constants */
#if defined(fw1_pwm_fw__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in fw1_pwm_fw_SetInterruptMode() function.
     *  @{
     */
        #define fw1_pwm_fw_INTR_NONE      (uint16)(0x0000u)
        #define fw1_pwm_fw_INTR_RISING    (uint16)(0x0001u)
        #define fw1_pwm_fw_INTR_FALLING   (uint16)(0x0002u)
        #define fw1_pwm_fw_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define fw1_pwm_fw_INTR_MASK      (0x01u) 
#endif /* (fw1_pwm_fw__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define fw1_pwm_fw_PS                     (* (reg8 *) fw1_pwm_fw__PS)
/* Data Register */
#define fw1_pwm_fw_DR                     (* (reg8 *) fw1_pwm_fw__DR)
/* Port Number */
#define fw1_pwm_fw_PRT_NUM                (* (reg8 *) fw1_pwm_fw__PRT) 
/* Connect to Analog Globals */                                                  
#define fw1_pwm_fw_AG                     (* (reg8 *) fw1_pwm_fw__AG)                       
/* Analog MUX bux enable */
#define fw1_pwm_fw_AMUX                   (* (reg8 *) fw1_pwm_fw__AMUX) 
/* Bidirectional Enable */                                                        
#define fw1_pwm_fw_BIE                    (* (reg8 *) fw1_pwm_fw__BIE)
/* Bit-mask for Aliased Register Access */
#define fw1_pwm_fw_BIT_MASK               (* (reg8 *) fw1_pwm_fw__BIT_MASK)
/* Bypass Enable */
#define fw1_pwm_fw_BYP                    (* (reg8 *) fw1_pwm_fw__BYP)
/* Port wide control signals */                                                   
#define fw1_pwm_fw_CTL                    (* (reg8 *) fw1_pwm_fw__CTL)
/* Drive Modes */
#define fw1_pwm_fw_DM0                    (* (reg8 *) fw1_pwm_fw__DM0) 
#define fw1_pwm_fw_DM1                    (* (reg8 *) fw1_pwm_fw__DM1)
#define fw1_pwm_fw_DM2                    (* (reg8 *) fw1_pwm_fw__DM2) 
/* Input Buffer Disable Override */
#define fw1_pwm_fw_INP_DIS                (* (reg8 *) fw1_pwm_fw__INP_DIS)
/* LCD Common or Segment Drive */
#define fw1_pwm_fw_LCD_COM_SEG            (* (reg8 *) fw1_pwm_fw__LCD_COM_SEG)
/* Enable Segment LCD */
#define fw1_pwm_fw_LCD_EN                 (* (reg8 *) fw1_pwm_fw__LCD_EN)
/* Slew Rate Control */
#define fw1_pwm_fw_SLW                    (* (reg8 *) fw1_pwm_fw__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define fw1_pwm_fw_PRTDSI__CAPS_SEL       (* (reg8 *) fw1_pwm_fw__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define fw1_pwm_fw_PRTDSI__DBL_SYNC_IN    (* (reg8 *) fw1_pwm_fw__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define fw1_pwm_fw_PRTDSI__OE_SEL0        (* (reg8 *) fw1_pwm_fw__PRTDSI__OE_SEL0) 
#define fw1_pwm_fw_PRTDSI__OE_SEL1        (* (reg8 *) fw1_pwm_fw__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define fw1_pwm_fw_PRTDSI__OUT_SEL0       (* (reg8 *) fw1_pwm_fw__PRTDSI__OUT_SEL0) 
#define fw1_pwm_fw_PRTDSI__OUT_SEL1       (* (reg8 *) fw1_pwm_fw__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define fw1_pwm_fw_PRTDSI__SYNC_OUT       (* (reg8 *) fw1_pwm_fw__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(fw1_pwm_fw__SIO_CFG)
    #define fw1_pwm_fw_SIO_HYST_EN        (* (reg8 *) fw1_pwm_fw__SIO_HYST_EN)
    #define fw1_pwm_fw_SIO_REG_HIFREQ     (* (reg8 *) fw1_pwm_fw__SIO_REG_HIFREQ)
    #define fw1_pwm_fw_SIO_CFG            (* (reg8 *) fw1_pwm_fw__SIO_CFG)
    #define fw1_pwm_fw_SIO_DIFF           (* (reg8 *) fw1_pwm_fw__SIO_DIFF)
#endif /* (fw1_pwm_fw__SIO_CFG) */

/* Interrupt Registers */
#if defined(fw1_pwm_fw__INTSTAT)
    #define fw1_pwm_fw_INTSTAT            (* (reg8 *) fw1_pwm_fw__INTSTAT)
    #define fw1_pwm_fw_SNAP               (* (reg8 *) fw1_pwm_fw__SNAP)
    
	#define fw1_pwm_fw_0_INTTYPE_REG 		(* (reg8 *) fw1_pwm_fw__0__INTTYPE)
#endif /* (fw1_pwm_fw__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_fw1_pwm_fw_H */


/* [] END OF FILE */
