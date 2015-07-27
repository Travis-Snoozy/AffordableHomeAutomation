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

.section __interrupt_vector_3,"ax",@progbits
io1:		.word	P1ISR	; I/O Port 1

.section __interrupt_vector_4,"ax",@progbits
io2:		.word	P2ISR	; I/O Port 2

.section __interrupt_vector_5,"ax",@progbits
usi:		.word	ISR	; USI

.section __interrupt_vector_9,"ax",@progbits
tm1:		.word	TA0ISR	; Timer0_A3 (ACCR0)

.section __interrupt_vector_10,"ax",@progbits
tm2:		.word	TAISR	; Timer0_A3 (ACCR2, ACCR1, TA)

.section __interrupt_vector_11,"ax",@progbits
wdt:		.word	ISR	; Watchdog

.section __interrupt_vector_12,"ax",@progbits
cmp:		.word	ISR	; Comparator

.section __interrupt_vector_15,"ax",@progbits
nmi: 		.word	ISR	; NMI, osc fault, flash violation

.section __reset_vector,"ax",@progbits
por: 		.word	PORISR	; POR, RST, WDT, KEYV

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
