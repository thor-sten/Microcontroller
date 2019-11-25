//
//	Digsys2 project: Digital voltmeter
//

// Set configure bits
#include <p24f16ka102.h>
//_FOSCSEL(FNOSC_FRCDIV_PLL & IESO_OFF)
_FOSC(FCKSM_CSECMD & POSCMOD_NONE & OSCIOFNC_ON)
_FWDT(FWDTEN_OFF)

// Initialise registers and ports
#define U_BRG ((36000000/16)/115200-1); //baud rate for UART (115200 baud)
#define U_MODE 0x8000 //enable UART1
#define U_STA 0x1400 //enable transmission and receiving for UART1

void init_registers(void)
{
	TRISA = 0x2B;	//define pins of portA as output (=0, RA7 down to RA0). Hex to binary: 0010 1011
	TRISB = 0x0387; //same for portB: 0000 0011 1000 0111

	T1CON = 0x8030; //timer1
	T2CON = 0x8030; //timer2

	I2C1CONbits.I2CEN = 0;
	ODCB = 0;
} //init_registers

void initU1(void)  
{
	U1BRG 	= U_BRG;
	U1MODE 	= U_MODE;
	U1STA 	= U_STA;
} //initU1

//test UART
char putU1(char c)
{
	while (U2STAbits.UTXBF); //whait while Tx buffer full
	U2TXREG = c;
	return c;
}
char getU1(void)
{
char read;
	while (!U2STAbits.URXDA);
	read = U2RXREG;
	_RB3 = 0;
	if (read == 'x') //LED2 illuminates if a "x" is received
	{
		_RB3 = 1;
		T1CON = 0x8030;
		TMR1 = 0;
		while (TMR1 < 32768);
		_RB3 = 0;
	}
	return read;
} //test UART

// A/D conversion
void initADC(void)
{
	unsigned amask;
	amask = 0xFFFC; 	//all analog inputs except AN1 and AN0 deactivated
	AD1PCFG = amask;
	AD1CON1 = 0x80E2; // automatic conversion after end of sampling
	AD1CON2	= 0x0604;	// AVDD/AVSS, set AD1IF after every two samples
	AD1CON3 = 0x0F00;	// Sample time 15 Tad, Tad = Tcy/2
	AD1CSSL = 0x0002;	//Select analog channel 1 and 2
	AD1CON1bits.ADON = 1;	// turn ADC ON
} // initADC

//
// Functions
//
unsigned ReadADC1(void)
{
	int *pointer;
	IFS0bits.AD1IF = 0;			// clear ADC interrupt flag
	AD1CON1bits.ASAM = 1;		// Set sampling to begin immediately after last conversion completes.
	while (!IFS0bits.AD1IF);	// wait for conversion to finish
	pointer = &ADC1BUF1;		// point to the ADC1 buffer
	//pointer = &ADC1BUF0;		// point to the ADC0 buffer
	return (unsigned) *pointer;
}

// Single digits
void digit(int dig)
{
	switch (dig)
	{	
	case 0:
		_RB14=1; _RA7=1; _RB10=1; _RB13=1; _RB15=1; _RB12=1; _RA6=0; asm ("nop"); _RB10=1; asm ("nop"); _RB15=1;
		break;
	case 1:
		_RB14=0; _RA7=1; _RB10=1; _RB13=0; _RB15=0; _RB12=0; _RA6=0; asm ("nop"); _RB10=1; asm ("nop"); _RB15=0;
		break;
	case 2:
		_RB14=1; _RA7=1; _RB10=0; _RB13=1; _RB15=1; _RB12=0; _RA6=1; asm ("nop"); _RB10=0; asm ("nop"); _RB15=1;
		break;
	case 3:
		_RB14=1; _RA7=1; _RB10=1; _RB13=1; _RB15=0; _RB12=0; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=0;
		break;
	case 4:
		_RB14=0; _RA7=1; _RB10=1; _RB13=0; _RB15=0; _RB12=1; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=0;
		break;
	case 5:
		_RB14=1; _RA7=0; _RB10=1; _RB13=1; _RB15=0; _RB12=1; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=0;
		break;
	case 6:
		_RB14=1; _RA7=0; _RB10=1; _RB13=1; _RB15=1; _RB12=1; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=1;
		break;
	case 7:
		_RB14=1; _RA7=1; _RB10=1; _RB13=0; _RB15=0; _RB12=0; _RA6=0; asm ("nop"); _RB10=1; asm ("nop"); _RB15=0;
		break;
	case 8:
		_RB14=1; _RA7=1; _RB10=1; _RB13=1; _RB15=1; _RB12=1; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=1;
		break;
	case 9:
		_RB14=1; _RA7=1; _RB10=1; _RB13=1; _RB15=0; _RB12=1; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=0;
		break;
	case 10: //E
		_RB14=1; _RA7=0; _RB10=0; _RB13=1; _RB15=1; _RB12=1; _RA6=1; asm ("nop"); _RB10=1; asm ("nop"); _RB15=1;
		break;
	case 11: //minus
		_RB14=0; _RA7=0; _RB10=0; _RB13=0; _RB15=0; _RB12=0; _RA6=1; asm ("nop"); _RB10=0; asm ("nop"); _RB15=0;
		break; 
	} //switch
} //disp

// Three digit display
void display(int num)
{
	int firstdigit, seconddigit, thirddigit;
	int mode=1; //DP position: 0= no dp, 1 = V, 2 = negative V, 3 = V

	// Adress numbers to certain digits
	// 100 = 1V -> 1 = 10mV
	if (num<2 && num>-2) //to keep zero when nothing measured
	{
		mode = 0;
		firstdigit = 0;
		seconddigit = 0;
		thirddigit = 0;
	}
	else if(num<0) //negative
	{
		mode = 2;
		firstdigit = 11; //minus
		seconddigit = num/-100;
		thirddigit = (num-firstdigit*100)/-10;
	}
	else if(num>999) //overload
	{
		thirddigit = 10; //E
	}
	else if(num>99)
	{
		mode = 1; //V
		firstdigit = num/100;
		seconddigit = (num-firstdigit*100)/10;
		thirddigit = (num-firstdigit*100)%10; //rest of division
	}
	else if(num>9)
	{
		mode = 3; //mV
		firstdigit = num/10;
		seconddigit = num%10;
		thirddigit = 0;
	}
	else if (num>=0)
	{
		mode = 3; //mV
		firstdigit = 0;
		seconddigit = num;
		thirddigit = 0;
	}
// Write numbers to display
	
	TMR2=0;
	while (TMR2<10000) //to slow down the display
	{
	// first digit (left to right: RA4, RB4, RB5)
	_RB5=0;	_RB4=0;	_RA4=1;	_RB5=0;
	digit(firstdigit);
	if(mode == 1) {_RB11=1;} //decimal point
	else {_RB11=0;} 
	TMR1=0;
	//mV LED
	if (mode==3)
	{
		_RB6 = 1;
		while(TMR1<50); //LED dimming
		_RB6 = 0;
	}
	while(TMR1<250); //delay for LCD
		
	//second digit
	_RB5=0;	_RB4=1;	_RA4=0;
	digit(seconddigit);
	if(mode == 2) {_RB11=1;} //decimal point
	else {_RB11=0;}
	while(TMR1<500);

	//third digit	
	_RB5=1;	_RB4=0;	_RA4=0;	_RB5=1;
	digit(thirddigit);
	if(mode == 3) {_RB11=1;} //decimal point
	else {_RB11=0;}
	while(TMR1<750);
	}//while
}

///////////////////////////////////////////////// Main

int main()
{
// Initialise
init_registers();
initU1();
initADC();

// Variables
int button; 
int buttonStatus=0; //bool?
char letter;
//unsigned dispValue;
int readValue, dispValue, refVoltage;
int counter;
float calFactor = 1.97; //6.5V = about 300 counts

while(1)
{
// read UART
	if (U2STAbits.URXDA)
	{
		letter=getU1();
		putU1(letter);
	}

// Button pushed?
	button = _RA3; //read button pin
	if ((button == 0) && (buttonStatus == 0)) //button pressed
	{
		_RB3 = 1; //switch button LED on
		buttonStatus = 1;
		readValue = ReadADC1(); //read analog input
		refVoltage = readValue*calFactor; //save reference voltage
		TMR2=0;	while(TMR2<5000); //wait to avoid switching off unwanted
	}//if
	else if ((button == 0) && (buttonStatus == 1))
	{
		_RB3 = 0; //switch button LED off
		buttonStatus = 0;
		refVoltage = 0;	// reset reference voltage
		TMR2=0;	while(TMR2<5000);
	}//if

// Read analog input	
	readValue=0;
	for(counter=1; counter<=10; counter++) //averaging
	{
		readValue = readValue + ReadADC1();
	}
	dispValue = readValue/10*calFactor-2; // average * calibration factor - offset

// Display voltage
	if (buttonStatus == 1) //reference voltage mode
	{
		dispValue = dispValue - refVoltage;
	}
	display(dispValue);

	//count up to 999:
	//TMR2=0;
	//if(dispValue>999)	dispValue=0;
	//while(TMR2<2000)	{display(dispValue);}	dispValue++;
	
} //while
} //main
