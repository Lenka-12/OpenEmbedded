/* Need to put this on function prototypes*/
extern int __io_putchar(int ch);
/* This is for printf during debugging, CMSIS headers are required */
/* Enable SWV and enable port 0*/
int __io_putchar(int ch){
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	ITM->TER |= (1<<0);
	return ITM_SendChar((uint32_t)ch);
}