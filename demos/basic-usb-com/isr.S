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
;SCK	P1.0_/| |\_P2.6	N/C
;SSN1	P1.1_/| |\_P2.7	N/C
;SSN2	P1.2_/| |\_TEST	N/C
;CE1	P1.3_/| |\_RST	10k -> VCC
;MISO	P1.4_/| |\_P1.7	N/C
;IRQ	P1.5_/| |\_P1.6	N/C
;MOSI0	P2.0_/| |\_P2.5	Diagnostic button
;MOSI1	P2.1_/| |\_P2.4	Diagnostic LED
;MOSI2	P2.2_/| |\_P2.3	MOSI3

; SS1		-> O: RFM70 (SPI slave select)
; SS2		-> O: FT220X (SPI slave select)
; MOSI0		-> I/O: SPI MOSI, MIOSIO[0] (FT220X)
; MOSI1-3	-> I/O: MIOSIO[1-3] (FT220X)
; SCK		-> O: SPI clock
; MISO		-> I: SPI MISO
; IRQ		-> I: RFM70 interrupt line
; CE1		-> O: RFM70 chip enable

; The FT220X must be configured to NOT drive its buffer status
; when de-selected.

; The MOSI0-3 pins must start in output mode.


.text
.global debug_wait
debug_wait:
bis.b	#0x10,		&__P2OUT	; Turn on the LED
bis	#0x00D8,	R2		; Go to sleep.
ret

.global IOISR
IOISR:
bic.b	#0x10,		&__P2OUT	; Turn off the LED
call	#firmware_io_isr
clr.b	&__P2IFG
clr.b	&__P1IFG
bic	#0x00D0, 	0(R1)		; Stay awake after return.
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

mov.b	#0x0F,		&__P1DIR	; Set P1 I/O directions
mov.b	#0x1F,		&__P2DIR	; Set P2 I/O directions

mov.b	#0x26,		&__P1OUT	; Set output/resistor direction
mov.b	#0x00,		&__P2OUT	; Set output/resistor direction

mov.b	#0x20,		&__P1REN	; Enable resistors
mov.b	#0x20,		&__P2REN	; Enable resistors

mov.b	#0x20,		&__P1IES	; High-to-low wireless IRQ trigger
mov.b	#0x20,		&__P2IES	; High-to-low debug trigger
clr.b	&__P1IFG			; Clear flags
clr.b	&__P2IFG
mov.b	#0x20,		&__P1IE		; Enable wireless IRQ + switch IRQs
mov.b	#0x20,		&__P2IE		; Enable switch IRQ

call	#firmware_main
nowhere:
					; I'll write you one very last op if you swear
bis	#0x00D8,	R2		; You'll go the fuck to sleep
jmp	nowhere

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
