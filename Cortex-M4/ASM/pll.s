.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb
.global _pll_config

/* some RCC registers  */
.equ RCC_BASE,                   0x40023800
.equ RCC_CR ,                    0x00000000
.equ RCC_PLLCFGR,                0x00000004
// prototype extern void _pll_config(uint8_t M, uint16_t N, uint8_t P, uint8_t Q);
.section .text._pll_config
 .type _pll_config, %function
_pll_config:
		// prepare stack for inputs
		push {r7}
		subs sp,#16
		adds r7,sp,#0
		// push inputs to the stack
		str r0,[r7,#12]
		str r1,[r7,#8]
		str r2,[r7,#4]
		str r3,[r7,#0]
		ldr r0, =RCC_BASE
		ldr r1, =RCC_PLLCFGR
		// configure PLL
		ldr r2,[r0,r1]
		// clear PLL
		ands r2,#0x00000000
		// set M
		ldr r3,[r7,#12]
		orrs r2,r2,r3
		// set N
		ldr r3,[r7,#8]
		lsls r3,r3,#6
		orrs r2,r2,r3
		// set P
		ldr r3,[r7,#4]
		lsls r3,r3,#0x10
		orrs r2,r2,r3
		// set Q
		ldr r3,[r7,#0]
		lsls r3,r3,#0x18
		orrs r2,r2,r3
		// set HSE as PLL clock source
		ldr r3,=0x01
		lsls r3,r3,#0x16
		orrs r2,r2,r3
		str r2,[r0,r1]
		// enable PLL and HSE
		ldr r1, =RCC_CR
		ldr r2, [r0,r1]
		ldr r3,=0x1
		// enable HSE bit 16
		lsls r3,r3,#0x10
		orrs r2,r2,r3
		str r2,[r0,r1]
		lsls r3,#0x11
/* wait for HSERDY flag to go high*/
_hserdy:
		ldr r2,[r0,r1]
		ands r2,r2,r3
		cmp r2,r3
		bne _hserdy
		// PLLON bit 24 of CR
		ldr r3,=0x01
		ldr r2, [r0,r1]
		lsls r3,r3,#0x18
		orrs r2,r2,r3
		str r2,[r0,r1]
		lsls r3,#0x19
/* wait for PLLRDY to go high */
_pllrdy:
		ldr r2,[r0,r1]
		ands r2,r2,r3
		cmp r2,r3
		bne _pllrdy
		// configure SYSCLKSRC
		nop
		// back to callee stack frame
		adds r7,#16
		mov  sp, r7
		pop  {r7}
		//return to callee
		bx   lr
		.align
		.end
