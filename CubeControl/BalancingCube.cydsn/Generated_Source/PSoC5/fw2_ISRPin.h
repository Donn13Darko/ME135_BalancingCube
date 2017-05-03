/*******************************************************************************
* File Name: fw2_ISRPin.h  
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

#if !defined(CY_PINS_fw2_ISRPin_H) /* Pins fw2_ISRPin_H */
#define CY_PINS_fw2_ISRPin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "fw2_ISRPin_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 fw2_ISRPin__PORT == 15 && ((fw2_ISRPin__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    fw2_ISRPin_Write(uint8 value);
void    fw2_ISRPin_SetDriveMode(uint8 mode);
uint8   fw2_ISRPin_ReadDataReg(void);
uint8   fw2_ISRPin_Read(void);
void    fw2_ISRPin_SetInterruptMode(uint16 position, uint16 mode);
uint8   fw2_ISRPin_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the fw2_ISRPin_SetDriveMode() function.
     *  @{
     */
        #define fw2_ISRPin_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define fw2_ISRPin_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define fw2_ISRPin_DM_RES_UP          PIN_DM_RES_UP
        #define fw2_ISRPin_DM_RES_DWN         PIN_DM_RES_DWN
        #define fw2_ISRPin_DM_OD_LO           PIN_DM_OD_LO
        #define fw2_ISRPin_DM_OD_HI           PIN_DM_OD_HI
        #define fw2_ISRPin_DM_STRONG          PIN_DM_STRONG
        #define fw2_ISRPin_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define fw2_ISRPin_MASK               fw2_ISRPin__MASK
#define fw2_ISRPin_SHIFT              fw2_ISRPin__SHIFT
#define fw2_ISRPin_WIDTH              1u

/* Interrupt constants */
#if defined(fw2_ISRPin__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in fw2_ISRPin_SetInterruptMode() function.
     *  @{
     */
        #define fw2_ISRPin_INTR_NONE      (uint16)(0x0000u)
        #define fw2_ISRPin_INTR_RISING    (uint16)(0x0001u)
        #define fw2_ISRPin_INTR_FALLING   (uint16)(0x0002u)
        #define fw2_ISRPin_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define fw2_ISRPin_INTR_MASK      (0x01u) 
#endif /* (fw2_ISRPin__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define fw2_ISRPin_PS                     (* (reg8 *) fw2_ISRPin__PS)
/* Data Register */
#define fw2_ISRPin_DR                     (* (reg8 *) fw2_ISRPin__DR)
/* Port Number */
#define fw2_ISRPin_PRT_NUM                (* (reg8 *) fw2_ISRPin__PRT) 
/* Connect to Analog Globals */                                                  
#define fw2_ISRPin_AG                     (* (reg8 *) fw2_ISRPin__AG)                       
/* Analog MUX bux enable */
#define fw2_ISRPin_AMUX                   (* (reg8 *) fw2_ISRPin__AMUX) 
/* Bidirectional Enable */                                                        
#define fw2_ISRPin_BIE                    (* (reg8 *) fw2_ISRPin__BIE)
/* Bit-mask for Aliased Register Access */
#define fw2_ISRPin_BIT_MASK               (* (reg8 *) fw2_ISRPin__BIT_MASK)
/* Bypass Enable */
#define fw2_ISRPin_BYP                    (* (reg8 *) fw2_ISRPin__BYP)
/* Port wide control signals */                                                   
#define fw2_ISRPin_CTL                    (* (reg8 *) fw2_ISRPin__CTL)
/* Drive Modes */
#define fw2_ISRPin_DM0                    (* (reg8 *) fw2_ISRPin__DM0) 
#define fw2_ISRPin_DM1                    (* (reg8 *) fw2_ISRPin__DM1)
#define fw2_ISRPin_DM2                    (* (reg8 *) fw2_ISRPin__DM2) 
/* Input Buffer Disable Override */
#define fw2_ISRPin_INP_DIS                (* (reg8 *) fw2_ISRPin__INP_DIS)
/* LCD Common or Segment Drive */
#define fw2_ISRPin_LCD_COM_SEG            (* (reg8 *) fw2_ISRPin__LCD_COM_SEG)
/* Enable Segment LCD */
#define fw2_ISRPin_LCD_EN                 (* (reg8 *) fw2_ISRPin__LCD_EN)
/* Slew Rate Control */
#define fw2_ISRPin_SLW                    (* (reg8 *) fw2_ISRPin__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define fw2_ISRPin_PRTDSI__CAPS_SEL       (* (reg8 *) fw2_ISRPin__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define fw2_ISRPin_PRTDSI__DBL_SYNC_IN    (* (reg8 *) fw2_ISRPin__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define fw2_ISRPin_PRTDSI__OE_SEL0        (* (reg8 *) fw2_ISRPin__PRTDSI__OE_SEL0) 
#define fw2_ISRPin_PRTDSI__OE_SEL1        (* (reg8 *) fw2_ISRPin__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define fw2_ISRPin_PRTDSI__OUT_SEL0       (* (reg8 *) fw2_ISRPin__PRTDSI__OUT_SEL0) 
#define fw2_ISRPin_PRTDSI__OUT_SEL1       (* (reg8 *) fw2_ISRPin__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define fw2_ISRPin_PRTDSI__SYNC_OUT       (* (reg8 *) fw2_ISRPin__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(fw2_ISRPin__SIO_CFG)
    #define fw2_ISRPin_SIO_HYST_EN        (* (reg8 *) fw2_ISRPin__SIO_HYST_EN)
    #define fw2_ISRPin_SIO_REG_HIFREQ     (* (reg8 *) fw2_ISRPin__SIO_REG_HIFREQ)
    #define fw2_ISRPin_SIO_CFG            (* (reg8 *) fw2_ISRPin__SIO_CFG)
    #define fw2_ISRPin_SIO_DIFF           (* (reg8 *) fw2_ISRPin__SIO_DIFF)
#endif /* (fw2_ISRPin__SIO_CFG) */

/* Interrupt Registers */
#if defined(fw2_ISRPin__INTSTAT)
    #define fw2_ISRPin_INTSTAT            (* (reg8 *) fw2_ISRPin__INTSTAT)
    #define fw2_ISRPin_SNAP               (* (reg8 *) fw2_ISRPin__SNAP)
    
	#define fw2_ISRPin_0_INTTYPE_REG 		(* (reg8 *) fw2_ISRPin__0__INTTYPE)
#endif /* (fw2_ISRPin__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_fw2_ISRPin_H */


/* [] END OF FILE */
