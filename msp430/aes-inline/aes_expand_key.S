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

.text
; aes_key_expand_inline
; R4=i expanded key start addr
; R7=i substitution box start addr
; R10=c scratch
; R11= i count
; R12=c XOR output
; R14=c scratch
; R15=i round constant
.global aes_expand_key_inline
aes_expand_key_inline:

; Save sbox[ekey[12]] for later.
mov.b	12(R4),		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[12]]
mov.b	@R14,		R10	; 2 => R10.b
swpb	R10			; 1

; Get sbox[ekey[13]]^rcon[next]
mov.b	13(R4),		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[13]]
mov.b	@R14,		R12	; 2 => R12.b
xor.b	R15,		R12	; 1 ^= rcon[next]

rla.b	R15			; 1 Update the round constant
jnc	.LskipRoundConstXor	; 2
xor	R6,		R15	; 2
.LskipRoundConstXor:

; Get sbox[ekey[14]]
mov.b	14(R4),		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[14]]
mov.b	@R14,		R14	; 2 |> R12
swpb	R14			; 1
bis	R14,		R12	; 1

; Save ekey[16-17]
xor	@R4,		R12	; 2 ^=(0-1)
mov	R12,		0(R4)	; 4 => (16-17)

; Get sbox[ekey[15]] into R12 MSB
mov.b	15(R4),		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[15]]
mov.b	@R14,		R12	; 2 => R10.b

mov	R4,		R14	; 1

; Get sbox[ekey[12]] into R12 LSB
bis	R10,		R12	; 1 |> R12

; Save ekey[18-19]
xor	R12,		2(R4)	; 4 ^=(2-3)
				; 0 => (18-19)

xor	@R14+,		4(R4)	; 5 (16-17)
				; 0 ^=(4-5)
				; 0 =>(20-21)

xor	@R14+,		6(R4)	; 5 (18-19)
				; 0 ^=(6-7)
				; 0 =>(22-23)

xor	@R14+,		8(R4)	; 5 (20-21)
				; 0 ^=(8-9)
				; 0 =>(24-25)

xor	@R14+,		10(R4)	; 5 (22-23)
				; 0 ^=(10-11)
				; 0 =>(26-27)

xor	@R14+,		12(R4)	; 5 (24-25)
				; 0 ^=(12-13)
				; 0 =>(28-29)

xor	@R14+,		14(R4)	; 5 (26-27)
				; 0 ^=(14-15)
				; 0 =>(30-31)

dec	R11			; 1
jnz	aes_expand_key_inline	; 2
ret

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
