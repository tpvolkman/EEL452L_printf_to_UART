/**
 * Main program.
 */
 
#include "main.h"
#include "sysclock.h"
#include "uart.h"

bool user_button_pressed = FALSE;

int main(void) 
{
	bool flashLED = TRUE;
	
	// Configure the SysTick to operate at 1 mS
	SysTickConfigure();
	
	// Configure the System Clocks
	SysClockConfigure();
	
	// Enable GPIO Port A Clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOA;

	// Enable GPIO Port C Clock 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOC;
	
	// Initialize GPIOA Port, Pin 5 - On-Board LED
	// Set GPIO Port A, Bit 5 as a General Purpose Output
	GPIOA->MODER.bit11 = 0;
	GPIOA->MODER.bit10 = 1;
	
	// Initialize GPIOC Port, Pin 13 - On-Board User Button
	// Set GPI0 Port C, Bit 13 for Input
	GPIOC->MODER.bit27 = 0;
	GPIOC->MODER.bit26 = 0;
	
	// Set GPIO Port C, Bit 13 for Pull-Up
	GPIOC->PUPDR.bit27 = 0;
	GPIOC->PUPDR.bit26 = 1;

	// Enable GPIO13 Interrupt for GPIO Port C
	SYSCFG->EXTICR4 |= 0x00000020;
	
	// Set up Interrupt Handler for Interrupt Pin 13, Port C
	EXTI->FTSR &= 0xFFFFDFFF;
	EXTI->RTSR |= 0x00002000;
	EXTI->EMR &= 0xFFFFDFFF;
	EXTI->IMR |= 0x00002000; 
	
	// Enable Interrupt 40 in Nested Vector Interrupt Controller
	NVIC_EnableIRQ( EXTI15_10_IRQn );
	
	GPIOA->ODR.bit5 = 0;		// Turn OFF LED
	
	stdout_init();
	
	while (1)
	{	
		if ( flashLED == TRUE )
		{
			if ( GPIOA->IDR.bit5 == 0 )
			{
				printf( "Internal LED On\r\n" );
				GPIOA->ODR.bit5 = 1;
			}
			else
			{
				printf( "Internal LED Off\r\n" );
				GPIOA->ODR.bit5 = 0;
			}

			Delay( 500 );
		}
		
		if ( user_button_pressed == TRUE )
		{
			if ( flashLED == TRUE )
			{
				GPIOA->ODR.bit5 = 0;
				flashLED = FALSE;
			}
			else
			{
				flashLED = TRUE;
			}
			user_button_pressed = FALSE;
		}
	}
}
