/*
    ASURO robot
    Thorsten 2017-06
*/

#include "mcc_generated_files/mcc.h"

void statusLED(int status)
{
    switch(status)
    {
        case 1:     // green
            LED_green_SetHigh();
            LED_red_SetLow();
            break;
        case 2:     // yellow
            LED_green_SetHigh();
            LED_red_SetHigh();
            break;
        case 3:     // red
            LED_green_SetLow();
            LED_red_SetHigh();
            break;
        default:    // off
            LED_green_SetLow();
            LED_red_SetLow();
            break;
    }
}

int button_push()   //check if button was pressed
{    
    if(Button_GetValue()==0)
    {
        int push_counter=0;
        while(push_counter<=5 && Button_GetValue()==0)  // debouncing
        {
            __delay_ms(1);
            push_counter++;
        } 
        if(push_counter>=5)
        {
            __delay_ms(200);
            return 1;
        }
    }  
    return 0;
}

void motor_FW(int left, int right)  //both motors forward speed
{
    //left motor:
    if(left<=100)
    {
        PWM7_LoadDutyValue(left);
        motorL1_SetLow();
        motorL2_SetHigh();
    }
    
    //right motor:
    if(right<=100)
    {
        PWM6_LoadDutyValue(right);
        motorR1_SetLow();
        motorR2_SetHigh();
    }
}

void motor_BREAK()  // short motor to break
{
    PWM7_LoadDutyValue(0);
    motorL1_SetLow();
    motorL2_SetLow();
    
    PWM6_LoadDutyValue(0);
    motorR1_SetLow();
    motorR2_SetLow();
}

void ASURO_Initialize()   //initialize robot
{
    motor_BREAK();
    LED_board_SetHigh();
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
}

int battery_voltage()
{
    int voltage=ADCC_GetSingleConversion(Battery);
    //if(voltage>500)
        
    return voltage;
}

/*
    Main function
 */

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    ASURO_Initialize();
    
    //variables
    unsigned int speed=0;
    unsigned int status=1;
    unsigned int batteryVoltage;
  
    while (1)
    { 
        //putch('a');
        //EUSART_Write("b");
        printf("\n\rBattery: %i",battery_voltage);
        
        if(button_push())
        {
            status=1;
            statusLED(status);
            
            speed+=20;
            if(speed>250)
                speed=0;
            motor_FW(speed,speed);
            //TMR2_Period8BitSet(speedL); // 0 to 255
            
            //printf("\n\rPR2: %i",speedL);
            LED_board_Toggle();
        }

    }
}
