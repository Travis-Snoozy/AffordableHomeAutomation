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
ISR:
reti

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

mov.b	#0x40,		&__P1OUT	; Set output/resistor direction
mov.b	#0xC0,		&__P2OUT	; Set output/resistor direction

mov.b	#0x40,		&__P1REN	; Enable resistors
mov.b	#0xC0,		&__P2REN	; Enable resistors

mov	#spi_odata,	R15		; Set up our SPI data
mov.b	#0x50,		0(R15)
mov.b	#0x53,		1(R15)
mov.b	#0xFF,		2(R15)
mov.b	#0x08,		3(R15)
mov.b	#0xFF,		4(R15)
mov.b	#0xFF,		5(R15)
mov.b	#0xFF,		6(R15)
mov.b	#0xFF,		7(R15)

mov	#spi_idata,	R15
mov.b	#0xAA,		0(R15)
mov.b	#0x55,		1(R15)
mov.b	#0xAA,		2(R15)
mov.b	#0x55,		3(R15)
mov.b	#0xAA,		4(R15)
mov.b	#0x55,		5(R15)
mov.b	#0xAA,		6(R15)
mov.b	#0xFF,		7(R15)


mov	#0,		R15
call	#spi_start

mov	#spi_odata+2,	R15
mov	#spi_idata+2,	R14
mov	#1,		R13
call	#spi_write

call	#spi_stop

bankswitch:

mov	#0,		R15		; Call spi_start(0)
call	#spi_start

mov	#spi_odata,	R15		; Call spi_write(spi_odata, spi_idata, 2)
mov	#spi_idata,	R14
mov	#2,		R13
call	#spi_write

call	#spi_stop

mov	#0,		R15
call	#spi_start

mov	#spi_odata+2,	R15
mov	#spi_idata+2,	R14
mov	#1,		R13
call	#spi_write

call	#spi_stop


mov	#spi_idata+2,	R15
mov	@R15,		R15
and	#0x80,		R15
cmp	#0x80,		R15
jeq	banksuccess
bis.b	#0x10,		&__P2OUT	; Turn on the "retried" light.
jmp	bankswitch


banksuccess:
mov	#0,		R15
call	#spi_start

mov	#spi_odata+3,	R15
mov	#spi_idata+3,	R14
mov	#5,		R13
call	#spi_write

call	#spi_stop


mov	#0,		R15
call	#spi_start

mov	#spi_odata,	R15
mov	#spi_idata,	R14
mov	#2,		R13
call	#spi_write

call	#spi_stop


mov	#spi_idata,	R15
;cmp.b	#0x00,		2(R15)
;jne	wrong
cmp.b	#0x63,		4(R15)
jne	wrong
cmp.b	#0x00,		5(R15)
jne	wrong
cmp.b	#0x00,		6(R15)
jne	wrong
cmp.b	#0x00,		7(R15)
jne	wrong


bis.b	#0x08,		&__P2OUT	; Turn on the success light.
jmp	done


wrong:
bis.b	#0x20,		&__P2OUT	; Turn on the failure light.

done:
					; I'll write you one very last op if you swear
bis	#0x00D8,	R2		; You'll go the fuck to sleep

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
