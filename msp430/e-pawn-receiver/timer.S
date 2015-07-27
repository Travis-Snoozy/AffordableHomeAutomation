; Copyright © 2014, Travis Snoozy
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

.global timer_set
timer_set:
bic	#0x0010,	&TA0CCTL0	; Disable interrupt
mov	R12,		&TA0CCR0	; Set next deadline
bis	#0x0010,	&TA0CCTL0	; Enable interrupt
ret

.global timer_is_set
timer_is_set:
mov	&TA0CCTL0,	R15
and	#0x0010,	R15
ret

.global timer_clear
timer_clear:
bic.b	#0x10,		&TA0CCTL0	; Disable interrupts
ret

.global disable_interrupts
disable_interrupts:
dint
ret

.global enable_interrupts
enable_interrupts:
eint
ret
; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
