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

;MSP430G2412
;Batt+	VCC _/| |\_GND	Batt-
;N/C	P1.0_/| |\_P2.6	Switch
;N/C	P1.1_/| |\_P2.7	Switch
;N/C	P1.2_/| |\_TEST	N/C
;N/C	P1.3_/| |\_RST	10k -> VCC
;N/C	P1.4_/| |\_P1.7	N/C
;N/C	P1.5_/| |\_P1.6	Switch
;N/C	P2.0_/| |\_P2.5	LED
;N/C	P2.1_/| |\_P2.4	LED
;N/C	P2.2_/| |\_P2.3	LED

; P1.0:	NSNNNNNN
; Dir : 10111111
; Out :	01000000
; Res : 01000000
; Pie : 01000000
; Pies: 0x000000
; Pif : 00000000
; P2.0:	SSLLLNNN
; Dir : 00111111
; Out : 11xxx000
; Res : 11000000
; Pie : 11000000
; Pies: xx000000
; Pif : 00000000

.text
.global PORISR
PORISR:
mov	#0x5A80,	&__WDTCTL	; Disable the watchdog timer
mov	#0x0300,	R1		; Set the stack
mov.b	#0x20,		&__BCSCTL3	; LFXT1 - use VLO for ACLK (no xtal)

clr.b	&__P1SEL			; Select I/O only
clr.b	&__P1SEL2			;

clr.b	&__P2SEL			; Select I/O only
clr.b	&__P2SEL2			;

mov.b	#0xBF,		&__P1DIR	; Set P1 I/O directions
mov.b	#0x3F,		&__P2DIR	; Set P2 I/O directions

mov.b	#0x40,		&__P1OUT	; Set output/resistor direction
mov.b	#0xC0,		&__P2OUT	; Set output/resistor direction

mov.b	#0x40,		&__P1REN	; Enable resistors
mov.b	#0xC0,		&__P2REN	; Enable resistors

mov.b	#0x40,		R5		; Mask
and.b	&__P1IN,	R5		;
mov.b	R5,		&__P1IES	; Edge select

mov.b	#0xC0,		R6		; Mask
and.b	&__P2IN,	R6		;
mov.b	R6,		&__P2IES	; Edge select

rra.b	R5				; Build up the LED states
bis.b	R5,		R6		;
rrc.b	R6				; Prev. rra guarantees C is clear
rrc.b	R6				; Prev. rrc guarantees C is clear
bic.b	R6,		&__P2OUT	; Clear bits (input inverted)
inv.b	R6				; Invert
bic.b	#0xC7,		R6		; Re-Mask
bis.b	R6,		&__P2OUT	; Set bits (input inverted)

clr.b	&__P1IFG			; Clear flags
clr.b	&__P2IFG			; Clear flags

mov.b	#0x40,		&__P1IE		; Enable interrupts
mov.b	#0xC0,		&__P2IE		; Enable interrupts

					; I'll write you one very last op if you swear
bis	#0x00D8,	R2		; You'll go the fuck to sleep




.global Switch_ISR
Switch_ISR:

					; Debounce
mov	#0x0010,	R7		; Set up a count...
loop:
dec	R7				; Start a new pass
jz	done				; If we have no more passes, finish
cmp	&__P1IN,	R5		; Does P1 match its previous state?
jz	port2				; Yes, carry on.
mov	&__P1IN,	R5		; No, store its current value...
jmp	Switch_ISR			; And start over.
port2:					; 
cmp	&__P2IN,	R6		; Does P2 match its previous state?
jz	loop				; Yes, carry on.
mov	&__P2IN,	R6		; No, store its current value...
jmp	Switch_ISR			; And start over
done:


and.b	#0x40,		R5		; Mask
mov.b	R5,		&__P1IES	; Edge select

and.b	#0xC0,		R6		; Mask
mov.b	R6,		&__P2IES	; Edge select

rra.b	R5				; Build up the LED states
bis.b	R5,		R6		;
rrc.b	R6				; Prev. rra guarantees C is clear
rrc.b	R6				; Prev. rrc guarantees C is clear
bic.b	R6,		&__P2OUT	; Clear bits (input inverted)
inv.b	R6				; Invert
bic.b	#0xC7,		R6		; Re-Mask
bis.b	R6,		&__P2OUT	; Set bits (input inverted)



clr.b	&__P1IFG			; Clear flags
clr.b	&__P2IFG			; Clear flags
.global ISR
ISR:
reti


; Set up the interrupt vectors

.section .vectors,"ax",@progbits
vector16:	.word	ISR	; 0xFFE0
vector15:	.word	ISR
io1:		.word	Switch_ISR	; I/O Port 1
io2:		.word	Switch_ISR	; I/O Port 2
usi:		.word	ISR	; USI
vector11:	.word	ISR
vector10:	.word	ISR
vector9:	.word	ISR
tm1:		.word	ISR	; Timer0_A3 (ACCR0)
tm2:		.word	ISR	; Timer0_A3 (ACCR2, ACCR1, TA)
wdt:		.word	ISR	; Watchdog
cmp:		.word	ISR	; Comparator
vector4:	.word	ISR
vector3:	.word	ISR
nmi: 		.word	ISR	; NMI, osc fault, flash violation
por: 		.word	PORISR	; POR, RST, WDT, KEYV

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
