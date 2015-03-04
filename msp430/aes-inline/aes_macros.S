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

; Use the substitution array starting at (sbox) to translate (io) into a new value.
; (io) must be a register.
; If all inputs are registers:
; 3 cycles.
.macro	mac_aes_sbox	sb, io
add	\sb,	\io	; 1
mov.b	@\io,	\io	; 2
.endm

; Perform a Galios field multiplication on (io). (const) must be 0x1b.
; If all inputs are registers:
; 4 cycles.
.macro	mac_aes_galmult	io, const
rla.b	\io			; 1
jnc	$+4			; 2
xor	\const,		\io	; 1
.endm

; For AES, mix a single value in a column.
; If (o) is an n(r), and all other inputs are registers:
; 13 cycles.
.macro	mac_aes_mixcol	a, b, c, d, scratch, const, o
mov		\a,		\scratch	; 1
xor		\b,		\scratch	; 1
mac_aes_galmult	\scratch,	\const		; 4
xor		\b,		\scratch	; 1
xor		\c,		\scratch	; 1
xor		\d,		\scratch	; 1
mov.b		\scratch,	\o		; 4
.endm

; For AES, mix a single column in a block.
; (o) must be a register (it will be indexed from -4 to -1).
; If all inputs are registers:
; 52 cycles.
.macro	mac_aes_mixcols	a, b, c, d, scratch, const, o
mac_aes_mixcol	\a, \b, \c, \d, \scratch, \const, -4(\o)	; 13
mac_aes_mixcol	\b, \c, \d, \a, \scratch, \const, -3(\o)	; 13
mac_aes_mixcol	\c, \d, \a, \b, \scratch, \const, -2(\o)	; 13
mac_aes_mixcol	\d, \a, \b, \c, \scratch, \const, -1(\o)	; 13
.endm

; Isolate the LSB of (i), and substitute the result into (o) (via (sb)).
; (o) must be a register.
; If all inputs are registers:
; 4 cycles.
.macro mac_aes_isub_lsb	i, o, sb
mov.b		\i,	\o	; 1
mac_aes_sbox	\sb,	\o	; 3
.endm

; Isolate the MSB of (i), and substitute the result into (o) (via (sb)).
; (i) will be swapped from its original state.
; (o) must be a register.
; If all inputs are registers:
; 5 cycles.
.macro mac_aes_isub_msb	i, o, sb
swpb			\i
mac_aes_isub_lsb	\i, \o, \sb
.endm

; Extract an MSB and LSB, substitute, combine, and write
; (msb) will contain only its MSB; LSB will be 0.
; (msb) must be a register.
; (scratch) must be a register.
; If (o) is an n(r), and all other inputs are registers:
; 15 cycles.
.macro mac_aes_shift	msb, lsb, scratch, o, sb
mac_aes_isub_msb	\msb, \msb, \sb			; 5
mac_aes_isub_lsb	\lsb, \scratch, \sb		; 4
swpb			\msb				; 1
bis			\msb,		\scratch	; 1
mov			\scratch,	\o		; 4
.endm

; Extract an MSB and LSB, substitute, combine, addroundkey, and write
; (msb) will contain only its MSB; LSB will be 0.
; (msb) must be a register.
; (scratch) must be a register.
; If (o) and (k) are both n(r), and all other inputs are registers:
; 18 cycles.
.macro mac_aes_unshift	msb, lsb, scratch, o, k, sb
mac_aes_isub_msb	\msb, \msb, \sb			; 5
mac_aes_isub_lsb	\lsb, \scratch, \sb		; 4
swpb			\msb				; 1
bis			\msb,		\scratch	; 1
xor			\k,		\scratch	; 3
mov			\scratch,	\o		; 4
.endm

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
