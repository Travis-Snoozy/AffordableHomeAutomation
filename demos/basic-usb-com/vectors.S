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

;.section .vectors,"ax",@progbits
;vector16:	.word	ISR	; 0xFFE0
;vector15:	.word	ISR

.section __interrupt_vector_18,"ax",@progbits
io1:		.word	IOISR	; I/O Port 1

.section __interrupt_vector_19,"ax",@progbits
io2:		.word	IOISR	; I/O Port 2

.section __interrupt_vector_20,"ax",@progbits
usi:		.word	ISR	; USI

;vector11:	.word	ISR
;vector10:	.word	ISR
;vector9:	.word	ISR

.section __interrupt_vector_24,"ax",@progbits
tm1:		.word	ISR	; Timer0_A3 (ACCR0)

.section __interrupt_vector_25,"ax",@progbits
tm2:		.word	ISR	; Timer0_A3 (ACCR2, ACCR1, TA)

.section __interrupt_vector_26,"ax",@progbits
wdt:		.word	ISR	; Watchdog

.section __interrupt_vector_27,"ax",@progbits
cmp:		.word	ISR	; Comparator

;vector4:	.word	ISR
;vector3:	.word	ISR

.section __interrupt_vector_30,"ax",@progbits
nmi: 		.word	ISR	; NMI, osc fault, flash violation
.section __interrupt_vector_31,"ax",@progbits
por: 		.word	PORISR	; POR, RST, WDT, KEYV

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
