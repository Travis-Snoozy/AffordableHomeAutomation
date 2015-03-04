; Copyright © 2013, Travis Snoozy
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
;CS	P1.0_/| |\_P2.6	Switch
;CSN	P1.1_/| |\_P2.7	Switch
;SCK	P1.2_/| |\_TEST	N/C
;MOSI	P1.3_/| |\_RST	10k -> VCC
;MISO	P1.4_/| |\_P1.7	N/C
;IRQ	P1.5_/| |\_P1.6	Switch
;N/C	P2.0_/| |\_P2.5	LED
;N/C	P2.1_/| |\_P2.4	LED
;N/C	P2.2_/| |\_P2.3	LED

.text
.global P2ISR
P2ISR:
call	#firmware_io_isr
clr.b	&__P2IFG
clr.b	&__P1IFG
.global ISR
ISR:
reti

.global PORISR
PORISR:
mov	#0x5A80,	&__WDTCTL	; Disable the watchdog timer
mov	#0x0300,	R1		; Set the stack
mov.b	#0x20,		&__BCSCTL3	; LFXT1 - use VLO for ACLK (no xtal)

clr.b	&__P1SEL			; Select I/O only
clr.b	&__P1SEL2			;

clr.b	&__P2SEL			; Select I/O only
clr.b	&__P2SEL2			;

mov.b	#0x8F,		&__P1DIR	; Set P1 I/O directions
mov.b	#0x3F,		&__P2DIR	; Set P2 I/O directions

mov.b	#0x60,		&__P1OUT	; Set output/resistor direction
mov.b	#0xC0,		&__P2OUT	; Set output/resistor direction

mov.b	#0x60,		&__P1REN	; Enable resistors
mov.b	#0xC0,		&__P2REN	; Enable resistors

call	#firmware_main

mov.b	#0x20,		&__P1IES	; High-to-low wireless IRQ trigger
clr.b	&__P1IFG			; Clear flags
clr.b	&__P2IFG
mov.b	#0x60,		&__P1IE		; Enable wireless IRQ + switch IRQs
mov.b	#0xC0,		&__P2IE		; Enable switch IRQ

					; I'll write you one very last op if you swear
bis	#0x00D8,	R2		; You'll go the fuck to sleep

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
