; Copyright © 2013, 2014, Travis Snoozy
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

.text

#ifdef MCISR_P1
.global P1ISR
P1ISR:
call	#MCISR_P1
clr.b	&__P1IFG
reti
#endif

#ifdef MCISR_P2
.global P2ISR
P2ISR:
call	#MCISR_P2
clr.b	&__P2IFG
reti
#endif

#ifdef MCISR_TA0_CC0
.global TA0ISR
TA0ISR:
call	#MCISR_TA0_CC0
reti
#endif

#ifdef MCISR_TA0_CC1
#define MCISR_TA0
#ifndef MCISR_TA0_CC2
#define MCISR_TA0_CC2 ISR
#endif
#endif

#ifdef MCISR_TA0_CC2
#define MCISR_TA0
#ifndef MCISR_TA0_CC1
#define MCISR_TA0_CC1 ISR
#endif
#endif

#ifdef MCUSI0
#define MCUSI
#endif

#ifdef MCUSI1
#define MCUSI
#endif

#ifdef MCUSICK
#define MCUSI
#endif

#ifdef MCISR_TA0
.global TAISR
TAISR:
add	&TAIV,		PC
jmp	.LTAISR1
jmp	.LTAISR2
.LTAISR1:
call	#MCISR_TA0_CC1
reti
.LTAISR2:
call	#MCISR_TA0_CC2
reti
#endif

.global ISR
ISR:
#ifndef MCISR_P1
.global P1ISR
P1ISR:
#endif
#ifndef MCISR_P2
.global P2ISR
P2ISR:
#endif
#ifndef MCISR_TA0_CC0
.global TA0ISR
TA0ISR:
#endif
#ifndef MCISR_TA0
.global TAISR
TAISR:
#endif
reti

.global PORISR
PORISR:
mov	#(WDTHOLD+WDTPW),&__WDTCTL	; Disable the watchdog timer
mov	#0x0300,	R1		; Set the stack
mov.b	#XCAP_3,	&__BCSCTL3	; LFXT1 - use 32.768KHz with 12.5pF capacitance

#ifdef MCBCSCTL
mov.b	MCBCSCTL,	&__BCSCTL1
#endif
#ifdef MCDCOCTL
mov.b	MCDCOCTL,	&__DCOCTL
#endif

mov.b	#MCP1F1,	&__P1SEL	; Select I/O functions
mov.b	#MCP1F2,	&__P1SEL2	;

mov.b	#MCP2F1,	&__P2SEL	; Select I/O functions
mov.b	#MCP2F2,	&__P2SEL2	;

mov.b	#MCIODIR1,	&__P1DIR	; Set P1 I/O directions
mov.b	#MCIODIR2,	&__P2DIR	; Set P2 I/O directions

mov.b	#MCRESDIR1,	&__P1OUT	; Set output/resistor direction
mov.b	#MCRESDIR2,	&__P2OUT	; Set output/resistor direction

mov.b	#MCRESEN1,	&__P1REN	; Enable resistors
mov.b	#MCRESEN2,	&__P2REN	; Enable resistors

call	#MCISR_POR

mov.b	#MCIRQES1,	&__P1IES	; High-to-low triggers
mov.b	#MCIRQES2,	&__P2IES	; 

#ifdef MCUSI
mov.b	#MCUSI0,	&__USICTL0	; Set up USI
mov.b	#MCUSI1,	&__USICTL1	;
mov.b	#MCUSICK,	&__USICKCTL	;
#endif

#ifdef MCTACCR0
mov.b	#MCTACCR0,	&__TA0CCR0	; Set up Timer_A
#endif
#ifdef MCTACCR1
mov.b	#MCTACCR1,	&__TA0CCR1	;
#endif
#ifdef MCTACCR2
mov.b	#MCTACCR2,	&__TA0CCR2	;
#endif
#ifdef MCTACCTL0
mov.b	#MCTACCTL0,	&__TA0CCTL0	;
#endif
#ifdef MCTACCTL1
mov.b	#MCTACCTL1,	&__TA0CCTL1	;
#endif
#ifdef MCTACCTL2
mov.b	#MCTACCTL2,	&__TA0CCTL2	;
#endif
#ifdef MCTACTL
mov.b	#MCTACTL,	&__TA0CTL	;
#endif

clr.b	&__P1IFG			; Clear flags
clr.b	&__P2IFG

mov.b	#MCIRQEN1,	&__P1IE		; Enable IRQs
mov.b	#MCIRQEN2,	&__P2IE		;

					; I'll write you one very last op if you swear
bis	#(LPM3_bits+GIE),R2		; You'll go the fuck to sleep

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
