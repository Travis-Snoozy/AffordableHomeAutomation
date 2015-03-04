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

; Raise RFM70 CE for at least 10us, triggering it to
; transmit the next packet in the TX queue (if one is
; present).
; This routine assumes that the MSR430G is using its
; calibrated 16MHz frequency settings, and that it has
; a maximum ±6% deviation from that frequency.
.global rfm70_pulse
rfm70_pulse:
; 3 * 55 = 165 cycles
; 170 cycles @ 16MHz + 6% = 10us
;            @ 16MHz - 6% = 11.3us
mov	#55,			R15
bis	#RFM70_B_ENABLE,	&RFM70_A_ENABLE
.ldelay:
dec	R15					; 1
jnz	.ldelay					; 2
bic	#RFM70_B_ENABLE,	&RFM70_A_ENABLE ; 5
ret
; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
