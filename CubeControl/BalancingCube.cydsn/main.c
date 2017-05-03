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
#define SLEEP_PAUSE          1000

#define x_motion_ISR         (1u)
#define y_motion_ISR         (2u)
#define z_motion_ISR         (3u)

/* Function prototypes. */
CY_ISR_PROTO (blueRX_ISR);
CY_ISR_PROTO (accelUP_ISR);
CY_ISR_PROTO (motion_handler);
CY_ISR_PROTO (rpm_handler);

void update_motion ();

/* Global Variables. */
uint8 errorStatus = (0u);

int16_t ax, ay, az;
int16_t gx, gy, gz;

bool jx, jy, jz;
int16_t rpmx, rpmy, rpmz;

uint8_t timeout = 50;
const uint8_t buff_len = 64;
char buff[buff_len];
uint8_t baud = 2;

/*** MAIN FUNCTION ***/
int main(void)
{   
    /* Enable the Global interrupt. */
    CyGlobalIntEnable;
    
    /* Start I2C Communication Line. */
    I2CM_Start ();
    
    /* Start UART Communication. */
    UART_Start ();
    CyDelay (BLUE_PAUSE);
	while (config_blue (baud, BLUE_PAUSE) != baud)
    {
        CyDelay (BLUE_PAUSE);
    }
    
    /* Setup & Connect to MPU6050. */
    MPU6050_init ();
	MPU6050_initialize ();
    while (!MPU6050_testConnection ())
    {
        CyDelay (MPU6050_PAUSE);
    }
   
    /* Setup UART (hardware) interrupt. */
    blueRX_ISR_StartEx (blueRX_ISR);
    blueRX_ISR_SetPriority (LOW_PRIORITY);
    
    /* Setup accel (timer) interrupt. */
    accelUP_ISR_StartEx (accelUP_ISR);
    accelUP_ISR_SetPriority (HIGH_PRIORITY);
    
    /* Setup x motion (software) interrupt. */
    CyIntSetVector (x_motion_ISR, motion_handler);
    CyIntSetPriority (x_motion_ISR, DEFAULT_PRIORITY);
    CyIntEnable (x_motion_ISR);
    
    /* Setup y motion (software) interrupt. */
    CyIntSetVector (y_motion_ISR, motion_handler);
    CyIntSetPriority (y_motion_ISR, DEFAULT_PRIORITY);
    CyIntEnable (y_motion_ISR);
    
    /* Setup z motion (software) interrupt. */
    CyIntSetVector (z_motion_ISR, motion_handler);
    CyIntSetPriority (z_motion_ISR, DEFAULT_PRIORITY);
    CyIntEnable (z_motion_ISR);
    
    /* Setup flywheel PWM and clocks. */
    flywheel_clock_Enable ();
    fw1_PWM_Start ();
    fw2_PWM_Start ();
    fw3_PWM_Start ();
    
    /* Setup flywheel RPM ISRs. */
    fw1_RPM_ISR_StartEx (rpm_handler);
    fw2_RPM_ISR_StartEx (rpm_handler);
    fw3_RPM_ISR_StartEx (rpm_handler);
    fw1_RPM_ISR_SetPriority (LOW_PRIORITY);
    fw2_RPM_ISR_SetPriority (LOW_PRIORITY);
    fw3_RPM_ISR_SetPriority (LOW_PRIORITY);
    
    /* Start accel timer. */
    accel_timer_Start ();

    for(;;)
    {
        CyDelay (SLEEP_PAUSE);
    }
}

/*** FUNCTION SECTION ***/


/*** ISR SECTION ***/

CY_ISR (blueRX_ISR)
{    
    uint8 rxStatus, rxData;           
    
    do
    {
        /* Read receiver status register */
        rxStatus = UART_RXSTATUS_REG;

        if ((rxStatus & (UART_RX_STS_BREAK      | UART_RX_STS_PAR_ERROR |
                        UART_RX_STS_STOP_ERROR | UART_RX_STS_OVERRUN)) != 0u)
        {
            /* ERROR handling. */
            errorStatus |= rxStatus & ( UART_RX_STS_BREAK      | UART_RX_STS_PAR_ERROR | 
                                        UART_RX_STS_STOP_ERROR | UART_RX_STS_OVERRUN);
        }
        
        if ((rxStatus & UART_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read data from the RX data register */
            rxData = UART_RXDATA_REG;
            if(errorStatus == 0u)
            {
                /* Send data backward */
                UART_TXDATA_REG = rxData;
            }
        }
    } while ((rxStatus & UART_RX_STS_FIFO_NOTEMPTY) != 0u);
    
    /* Clear blueRX ISR for next interrupt. */
    blueRX_ISR_ClearPending ();
}

CY_ISR (accelUP_ISR)
{    
    /* Clear sticky timer TC by reading register. */
    accel_timer_STATUS;
    
    /* Update accel & gyro values. */
    MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    /* Set interrupts to handle x, y, and z motion. */
    CyIntSetPending (x_motion_ISR);
    CyIntSetPending (y_motion_ISR);
    CyIntSetPending (z_motion_ISR);
    
    /* Clear accel ISR. */
    accelUP_ISR_ClearPending ();
}

CY_ISR (motion_handler)
{
    /* Handle motion cases. */
    if (CyIntGetState (x_motion_ISR))
    {
        /* React to x motion. */
        
        /* Clear x interrupt. */
        CyIntClearPending (x_motion_ISR);
    } else if (CyIntGetState (y_motion_ISR))
    {
        /* React to y motion. */
        
        /* Clear y interrupt. */
        CyIntClearPending (y_motion_ISR);
    } else if (CyIntGetState (z_motion_ISR))
    {
        /* React to z motion. */
        
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
        fw1_RPM_ISR_ClearPending ();
    } else if (fw2_RPM_ISR_GetState ())
    {
        /* Update fw2 RPM. */
        rpmy++;
        
        /* Clear fw2 interrupt. */
        fw2_RPM_ISR_ClearPending ();
    }else if (fw3_RPM_ISR_GetState ())
    {
        /* Update fw3 RPM. */
        rpmz++;
        
        /* Clear fw3 interrupt. */
        fw3_RPM_ISR_ClearPending ();
    } else
    {
        /* Raise an error. */
    }
}

/* [] END OF FILE */
