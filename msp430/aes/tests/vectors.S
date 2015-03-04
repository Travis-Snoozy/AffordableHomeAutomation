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
POR:
mov	#0x5A80,	&__WDTCTL	; Disable the watchdog timer
mov	#0x0300,	R1		; Set the stack
mov.b	#0x20,		&__BCSCTL3	; LFXT1 - use VLO for ACLK (no xtal)
call	#firmware_main
					; I'll write you one very last op if you swear
bis	#0x00D8,	R2		; You'll go the fuck to sleep
ISR:
reti

.section __interrupt_vector_16,"ax",@progbits
.word	ISR	; 0xFFE0

.section __interrupt_vector_17,"ax",@progbits
.word	ISR

.section __interrupt_vector_18,"ax",@progbits
.word	ISR	; I/O Port 1

.section __interrupt_vector_19,"ax",@progbits
.word	ISR	; I/O Port 2

.section __interrupt_vector_20,"ax",@progbits
.word	ISR	; USI

.section __interrupt_vector_21,"ax",@progbits
.word	ISR

.section __interrupt_vector_22,"ax",@progbits
.word	ISR

.section __interrupt_vector_23,"ax",@progbits
.word	ISR

.section __interrupt_vector_24,"ax",@progbits
.word	ISR	; Timer0_A3 (ACCR0)

.section __interrupt_vector_25,"ax",@progbits
.word	ISR	; Timer0_A3 (ACCR2, ACCR1, TA)

.section __interrupt_vector_26,"ax",@progbits
.word	ISR	; Watchdog

.section __interrupt_vector_27,"ax",@progbits
.word	ISR	; Comparator

.section __interrupt_vector_28,"ax",@progbits
.word	ISR

.section __interrupt_vector_29,"ax",@progbits
.word	ISR

.section __interrupt_vector_30,"ax",@progbits
.word	ISR	; NMI, osc fault, flash violation

.section __interrupt_vector_31,"ax",@progbits
.word	POR	; POR, RST, WDT, KEYV

;.section .vectors,"ax",@progbits
;vector16:	.word	ISR	; 0xFFE0
;vector15:	.word	ISR
;io1:		.word	ISR	; I/O Port 1
;io2:		.word	ISR	; I/O Port 2
;usi:		.word	ISR	; USI
;vector11:	.word	ISR
;vector10:	.word	ISR
;vector9:	.word	ISR
;tm1:		.word	ISR	; Timer0_A3 (ACCR0)
;tm2:		.word	ISR	; Timer0_A3 (ACCR2, ACCR1, TA)
;wdt:		.word	ISR	; Watchdog
;cmp:		.word	ISR	; Comparator
;vector4:	.word	ISR
;vector3:	.word	ISR
;nmi: 		.word	ISR	; NMI, osc fault, flash violation
;por: 		.word	POR	; POR, RST, WDT, KEYV

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
