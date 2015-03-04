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

; "Factory setting" values for device's keys, IVs, and config.
.section .rodata
.balign 512, 0xFF
.global expanded_key
expanded_key:
.skip 176, 0xFF
.global hash_key
hash_key:
.skip 16, 0xFF
.global key_segment
key_segment:
.skip 1, 0xFF
.balign 512, 0xFF
.global safe_iv_tx
safe_iv_tx:
.skip 512, 0xFF
.global safe_iv_rx
safe_iv_rx:
.skip 512, 0xFF

.section .infob,"ax",@progbits
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0
.byte 0
.byte 0
.byte 0xFF

.section .infoc,"ax",@progbits
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0
.byte 0
.byte 0
.byte 0xFF

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
