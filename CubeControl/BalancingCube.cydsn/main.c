/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdint.h>
#include <stdio.h>
#include "I2C.h"
#include "blue.h"
#include "MPU6050.h"

/* Various definitions defined structures. */
/* Priority ranges from
    0-7 on PSOC 5LP
    0-3 on PSOC 4 */
#define DEFAULT_PRIORITY     (5u)
#define LOW_PRIORITY         (7u)
#define MEDIUM_PRIORITY      (3u)
#define HIGH_PRIORITY        (1u)

#define I2C_SLAVE_ADDRESS    0x68

#define MPU6050_PAUSE        500
#define BLUE_PAUSE           600
#define SLEEP_PAUSE          250
#define SERVO_PAUSE          250
#define MOTOR_PAUSE          500

#define x_motion_ISR         (1u)
#define y_motion_ISR         (2u)
#define z_motion_ISR         (3u)

#define PIN_OFF              (0u)
#define PIN_ON               (1u)

/* Function prototypes. */
CY_ISR_PROTO (blueRX_ISR);
CY_ISR_PROTO (accelUP_ISR);
CY_ISR_PROTO (motion_handler);
CY_ISR_PROTO (rpm_handler);

/* Global Variables. */
uint8 errorStatus = (0u);

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t tx, ty, tz;

uint16_t PWM_PERIOD = 4800;
uint16_t PWM_ZERO = 0;

uint16_t m_pwmx, m_pwmy, m_pwmz;
uint16_t MOTOR_FULL_REVERSE = 125;
uint16_t MOTOR_STOP = 290;
uint16_t MOTOR_FULL_FORWARD = 415;

uint16_t s_pwmx, s_pwmy, s_pwmz;

uint16_t SERVO_N90 = 3870;
uint16_t SERVO_ZERO = 4320;
uint16_t SERVO_P90 = 4770;

bool jx, jy, jz;
uint16_t rpmx, rpmy, rpmz;

uint8_t blueBuff_timeout = 50;
const size_t blueBuff_len = 64;
uint8_t baud = 2;

/* ax ay az gx gy gz rpmx rpmy rpmz */
const size_t updateBuff_len = 18;
const char* updateBlue_STR = "%d %d %d %d %d %d %d %d %d";

/*** MAIN FUNCTION ***/
int main(void)
{   
    /* Enable the Global interrupt. */
    CyGlobalIntEnable;
    
    /* Start I2C Communication Line. */
    I2CM_Start ();
    
    /* Start UART Communication. */
    UART_Start ();
	while (config_blue (baud, BLUE_PAUSE) != baud)
    {
        CyDelay (BLUE_PAUSE);
    }
    
    /* Setup & Connect to MPU6050. */
    //MPU6050_init ();
	//MPU6050_initialize ();
    //while (!MPU6050_testConnection ())
    //{
    //    CyDelay (MPU6050_PAUSE);
    //}
    
    /* Set desired state to current state. */
    //MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    tx = gx;
    ty = gy;
    tz = gz;
   
    /* Setup UART (hardware) interrupt. */
    blueRX_ISR_StartEx (blueRX_ISR);
    blueRX_ISR_SetPriority (LOW_PRIORITY);
    
    /* Setup accel (timer) interrupt. */
    accelUP_ISR_StartEx (accelUP_ISR);
    accelUP_ISR_SetPriority (HIGH_PRIORITY);
    
    /* Setup x motion (software) interrupt. */
    CyIntSetVector (x_motion_ISR, motion_handler);
    CyIntSetPriority (x_motion_ISR, MEDIUM_PRIORITY);
    CyIntEnable (x_motion_ISR);
    
    /* Setup y motion (software) interrupt. */
    CyIntSetVector (y_motion_ISR, motion_handler);
    CyIntSetPriority (y_motion_ISR, MEDIUM_PRIORITY);
    CyIntEnable (y_motion_ISR);
    
    /* Setup z motion (software) interrupt. */
    CyIntSetVector (z_motion_ISR, motion_handler);
    CyIntSetPriority (z_motion_ISR, MEDIUM_PRIORITY);
    CyIntEnable (z_motion_ISR);
    
    /* Setup flywheel PWM, clocks, and calibration. */
    flywheel_clock_Enable ();
    fw1_PWM_Start ();
    fw2_PWM_Start ();
    fw3_PWM_Start ();
    CyDelay (MOTOR_PAUSE);
    
    fw1_PWM_WritePeriod (PWM_PERIOD);
    fw3_PWM_WritePeriod (PWM_PERIOD);
    fw3_PWM_WritePeriod (PWM_PERIOD);
    CyDelay (MOTOR_PAUSE);
    
    /* Setup Motor PWM Compare Values (Duty Cycle). */
    fw1_PWM_WriteCompare1 (PWM_ZERO);
    fw2_PWM_WriteCompare1 (PWM_ZERO);
    fw3_PWM_WriteCompare1 (PWM_ZERO);
    CyDelay (MOTOR_PAUSE);
    fw1_PWM_WriteCompare1 (MOTOR_STOP);
    fw2_PWM_WriteCompare1 (MOTOR_STOP);
    fw3_PWM_WriteCompare1 (MOTOR_STOP);
    CyDelay (MOTOR_PAUSE);
    
    /* Setup Servo PWM Compare Values (Duty Cycle). */
    fw1_PWM_WriteCompare2 (PWM_ZERO);
    fw2_PWM_WriteCompare2 (PWM_ZERO);
    fw3_PWM_WriteCompare2 (PWM_ZERO);
    CyDelay (SERVO_PAUSE);
    fw1_PWM_WriteCompare2 (SERVO_N90);
    fw2_PWM_WriteCompare2 (SERVO_ZERO);
    fw3_PWM_WriteCompare2 (SERVO_N90);
    CyDelay (SERVO_PAUSE);
    
    /* Setup flywheel RPM ISRs. */
    //fw1_RPM_ISR_StartEx (rpm_handler);
    //fw2_RPM_ISR_StartEx (rpm_handler);
    //fw3_RPM_ISR_StartEx (rpm_handler);
    //fw1_RPM_ISR_SetPriority (LOW_PRIORITY);
    //fw2_RPM_ISR_SetPriority (LOW_PRIORITY);
    //fw3_RPM_ISR_SetPriority (LOW_PRIORITY);
    
    /* Turn on flywheel LEDS. */
    //RPM_LED_PIN_Write (PIN_ON);
    
    /* Start accel timer. */
    //accel_timer_Start ();

    char updateBuff[updateBuff_len];
    uint16_t i = MOTOR_STOP;
    for(;;)
    {
        //fw2_PWM_WriteCompare1 (i);
        //i -= 10;
        fw2_PWM_WriteCompare1 (i);
        
        i -= 10;
        
        if (i == (MOTOR_STOP - 70))
        {
            fw2_PWM_WriteCompare1 (MOTOR_STOP);
            fw2_PWM_WriteCompare2 (SERVO_N90);
        }
        //snprintf (updateBuff, updateBuff_len, updateBlue_STR, 
        //    ax, ay, az, gx, gy, gz, rpmx, rpmy, rpmz);
        //WriteTxData (updateBuff, updateBuff_len);
    }
}

/*** FUNCTION SECTION ***/


/*** ISR SECTION ***/

CY_ISR (blueRX_ISR)
{    
    /* Read RX Data. */
    char blueBuff[blueBuff_len];
    ReadRxData (blueBuff, blueBuff_len, blueBuff_timeout);
    
    /* Act on RX Data. */
    
    
    /* Respond to RX Data. */
    WriteTxData (blueBuff, blueBuff_len);
    
    /* Clear blueRX ISR for next interrupt. */
    blueRX_ISR_ClearPending ();
}

CY_ISR (accelUP_ISR)
{    
    /* Clear sticky timer TC by reading register. */
    accel_timer_STATUS;
    
    /* Clear accel ISR. */
    accelUP_ISR_ClearPending ();
    
    /* Update accel & gyro values. */
    MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    /* Set interrupts to handle x, y, and z motion. */
    CyIntSetPending (x_motion_ISR);
    CyIntSetPending (y_motion_ISR);
    CyIntSetPending (z_motion_ISR);
}

CY_ISR (motion_handler)
{
    int16_t val = MOTOR_STOP;
    
    /* Handle motion cases. */
    if (CyIntGetState (x_motion_ISR))
    {
        /* React to x motion. */
        if (tx < gx)
        {
            val = MOTOR_STOP + ((MOTOR_FULL_FORWARD - MOTOR_STOP) * (tx / gx) + 10);
        } else if (tx > gx)
        {
            val = MOTOR_STOP - ((MOTOR_STOP - MOTOR_FULL_REVERSE) * (1 - (tx / gx)) + 10);
        }
            
        fw1_PWM_WriteCompare1 (val);
        
        /* Clear x interrupt. */
        CyIntClearPending (x_motion_ISR);
    } else if (CyIntGetState (y_motion_ISR))
    {
        /* React to y motion. */
        if (ty < gy)
        {
            val = MOTOR_STOP + ((MOTOR_FULL_FORWARD - MOTOR_STOP) * (ty / gy) + 10);
        } else if (ty > gy)
        {
            val = MOTOR_STOP - ((MOTOR_STOP - MOTOR_FULL_REVERSE) * (1 - (ty / gy)) + 10);
        }
            
        fw2_PWM_WriteCompare1 (val);
        
        /* Clear y interrupt. */
        CyIntClearPending (y_motion_ISR);
    } else if (CyIntGetState (z_motion_ISR))
    {
        /* React to z motion. */
        if (tz < gz)
        {
            val = MOTOR_STOP + ((MOTOR_FULL_FORWARD - MOTOR_STOP) * (tz / gz) + 10);
        } else if (tz > gz)
        {
            val = MOTOR_STOP - ((MOTOR_STOP - MOTOR_FULL_REVERSE) * (1 - (tz / gz)) + 10);
        }
            
        fw3_PWM_WriteCompare1 (val);
        
        /* Clear z interrupt. */
        CyIntClearPending (z_motion_ISR);
    } else
    {
        /* Raise an error. */
    }
}

CY_ISR (rpm_handler)
{
    /* Handle increment cases. */
    if (fw1_RPM_ISR_GetState())
    {
        /* Update fw1 RPM. */
        rpmx++;
        
        /* Clear fw1 interrupt. */
        fw1_RPM_ISRPin_ClearInterrupt ();
        fw1_RPM_ISR_ClearPending ();
    } else if (fw2_RPM_ISR_GetState ())
    {
        /* Update fw2 RPM. */
        rpmy++;
        
        /* Clear fw2 interrupt. */
        fw2_RPM_ISRPin_ClearInterrupt ();
        fw2_RPM_ISR_ClearPending ();
    }else if (fw3_RPM_ISR_GetState ())
    {
        /* Update fw3 RPM. */
        rpmz++;
        
        /* Clear fw3 interrupt. */
        fw3_RPM_ISRPin_ClearInterrupt ();
        fw3_RPM_ISR_ClearPending ();
    } else
    {
        /* Raise an error. */
    }
}

/* [] END OF FILE */
