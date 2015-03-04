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
.global c_aes_expand_key
c_aes_expand_key:
push	R4			; 3
push	R7			; 3
push	R10			; 3
push	R11			; 3
mov	R12,		R14	; 1
mov	R13,		R4	; 1
call	#aes_expand_key		; 5
pop	R11			; 2
pop	R10			; 2
pop	R7			; 2
pop	R4			; 2
ret

.global aes_expand_key
; aes_expand_key
; R4=c expanded key start addr
; R7=c substitution box start addr
; R10=c scratch
; R11=c expanded key index 2 (=R4+12)
; R12=c XOR output
; R13=c iterator (=10)
; R14=c key start addr; c SBox index (=R7+offset)
; R15=c round constant

aes_expand_key:

mov	R4,		R11	; 1 set up the second expansion index
add	#12,		R11	; 2
mov	#sbox,		R7	; 2 get the SBox address
mov	#1,		R15	; 1 Initialize round constant
mov	#10,		R13	; 2 Set up loop

; Copy key[0-15] to ekey[0-15]
mov	@R14+,		0(R4)	; 5
mov	@R14+,		2(R4)	; 5
mov	@R14+,		4(R4)	; 5
mov	@R14+,		6(R4)	; 5
mov	@R14+,		8(R4)	; 5
mov	@R14+,		10(R4)	; 5
mov	@R14+,		12(R4)	; 5
mov	@R14+,		14(R4)	; 5

.LexpandRound:

; Save sbox[ekey[12]] for later.
mov.b	@R11+,		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[12]]
mov.b	@R14,		R10	; 2 => R10.b
swpb	R10			; 1

; Get sbox[ekey[13]]^rcon[next]
mov.b	@R11+,		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[13]]
mov.b	@R14,		R12	; 2 => R12.b
xor.b	R15,		R12	; 1 ^= rcon[next]

rla.b	R15			; 1 Update the round constant
jnc	.LskipRoundConstXor	; 2
xor	#0x1b,		R15	; 2
.LskipRoundConstXor:

; Get sbox[ekey[14]]
mov.b	@R11+,		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[14]]
mov.b	@R14,		R14	; 2 |> R12
swpb	R14			; 1
bis	R14,		R12	; 1

; Save ekey[16-17]
xor	@R4+,		R12	; 2 ^=(0-1)
mov	R12,		14(R4)	; 4 => (16-17)

; Get sbox[ekey[15]] into R12 MSB
mov.b	@R11+,		R14	; 2 
add	R7,		R14	; 1 sbox[ekey[15]]
mov.b	@R14,		R12	; 2 => R10.b

; Get sbox[ekey[12]] into R12 LSB
bis	R10,		R12	; 1 |> R12

; Save ekey[18-19]
xor	@R4+,		R12	; 2 ^=(2-3)
mov	R12,		14(R4)	; 4 => (18-19)

mov	@R11+,		R12 	; 2 (16-17)
xor	@R4+,		R12 	; 2 ^=(4-5)
mov	R12,		14(R4)	; 4 =>(20-21)

mov	@R11+,		R12 	; 2 (18-19)
xor	@R4+,		R12 	; 2 ^=(6-7)
mov	R12,		14(R4)	; 4 =>(22-23)

mov	@R11+,		R12 	; 2 (20-21)
xor	@R4+,		R12 	; 2 ^=(8-9)
mov	R12,		14(R4)	; 4 =>(24-25)

mov	@R11+,		R12 	; 2 (22-23)
xor	@R4+,		R12 	; 2 ^=(10-11)
mov	R12,		14(R4)	; 4 =>(26-27)

mov	@R11+,		R12 	; 2 (24-25)
xor	@R4+,		R12 	; 2 ^=(12-13)
mov	R12,		14(R4)	; 4 =>(28-29)

mov	@R11+,		R12 	; 2 (26-27)
xor	@R4+,		R12 	; 2 ^=(14-15)
mov	R12,		14(R4)	; 4 =>(30-31)

dec	R13			; 1
jnz	.LexpandRound		; 2

ret

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
