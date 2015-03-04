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

.section .vectors,"ax",@progbits
vector16:	.word	ISR	; 0xFFE0
vector15:	.word	ISR
io1:		.word	P2ISR	; I/O Port 1
io2:		.word	P2ISR	; I/O Port 2
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
