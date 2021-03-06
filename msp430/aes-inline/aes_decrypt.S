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

.include "aes_macros.S"

.text
.global c_aes_decrypt
c_aes_decrypt:
push	R4			; 3
push	R5			; 3
push	R6			; 3
push	R7			; 3
push	R8			; 3
push	R9			; 3
push	R10			; 3
mov	R12,		R4	; 1
mov	R13,		R5	; 1
call	#aes_decrypt		; 5
pop	R10			; 2
pop	R9			; 2
pop	R8			; 2
pop	R7			; 2
pop	R6			; 2
pop	R5			; 2
pop	R4			; 2
ret				; 3

.global aes_decrypt
; aes_decrypt
; R4=i expanded key start addr
; R5=i data start addr
; R6=c galois constant
; R7=c substitution box start addr
; R8=c loop 2
; R9=c scratch 1
; R10=c scratch 2
; R11=c scratch 3
; R12=c scratch 4
; R13=c loop 1
; R14=c calculation space
aes_decrypt:

push	14(R4)			; 5 Copy the key to the stack
push	12(R4)			; 5
push	10(R4)			; 5
push	8(R4)			; 5
push	6(R4)			; 5
push	4(R4)			; 5
push	2(R4)			; 5
push	@R4			; 5
mov	R1,		R4	; 1

mov	#0x1b,		R6	; 2 Store the galois XOR constant
mov	#sbox,		R7	; 2 get the sbox start addr
mov	#10,		R13	; 2 Set the outer loop counter
mov	#1,		R15	; 1 Set the round constant
mov	R13,		R11	; 1 We need to also run the key expansion all the way forward
call	#aes_expand_key_inline	; 5

xor	@R4+,		 0(R5)	; 5
xor	@R4+,		 2(R5)	; 5
xor	@R4+,		 4(R5)	; 5
xor	@R4+,		 6(R5)	; 5
xor	@R4+,		 8(R5)	; 5
xor	@R4+,		10(R5)	; 5
xor	@R4+,		12(R5)	; 5
xor	@R4+,		14(R5)	; 5

sub	#16,		R4	; 2

.LnextdRound:

; ---- Expand key ----
bit.b	#1,		R15	; 1 Update the round constant
jz	.LskipRoundConstXor	; 2
xor.b	R6,		R15	; 2
setc				; 1
.LskipRoundConstXor:
rrc.b	R15			; 1
xor	10(R4),		14(R4)	; 6 (26-27)
				; 0 ^=(14-15)
				; 0 =>(30-31)

xor	8(R4),		12(R4)	; 6 (24-25)
				; 0 ^=(12-13)
				; 0 =>(28-29)

xor	6(R4),		10(R4)	; 6 (22-23)
				; 0 ^=(10-11)
				; 0 =>(26-27)

xor	4(R4),		8(R4)	; 6 (20-21)
				; 0 ^=(8-9)
				; 0 =>(24-25)

xor	2(R4),		6(R4)	; 6 (18-19)
				; 0 ^=(6-7)
				; 0 =>(22-23)

xor	@R4,		4(R4)	; 5 (16-17)
				; 0 ^=(4-5)
				; 0 =>(20-21)

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

; Get sbox[ekey[12]] into R12 LSB
bis	R10,		R12	; 1 |> R12
; Save ekey[18-19]
xor	R12,		2(R4)	; 4 ^=(2-3)
				; 0 => (18-19)
; ----    end     ----
; ---- Expand key ----
mov	#rsbox,		R7	; 2 get the sbox start addr

cmp	#10,		R13	; 2 Is this the final round?
jeq	.LskipdMixColumns	; 2 Final round skips mixing

; MixColumns

mov	#4,		R8	; 2 Set the inner loop counter

.LnextdColumn:

mov.b	@R5+,		R9	; 2 Save the column data
mov.b	@R5+,		R10	; 2
mov.b	@R5+,		R11	; 2
mov.b	@R5+,		R12	; 2

mov	R9,		R14	; 1
xor	R11,		R14	; 1
mac_aes_galmult		R14, R6	; 4
mac_aes_galmult		R14, R6	; 4
xor	R14,		R9	; 1
xor	R14,		R11	; 1

mov	R10,		R14	; 1
xor	R12,		R14	; 1
mac_aes_galmult		R14, R6	; 4
mac_aes_galmult		R14, R6	; 4
xor	R14,		R10	; 1
xor	R14,		R12	; 1

				; 52
mac_aes_mixcols		R9, R10, R11, R12, R14, R6, R5

dec	R8			; 1
jnz	.LnextdColumn		; 2

sub	#16,		R5	; 2 Reset the data pointer

.LskipdMixColumns:

; AddRoundKey + SubBytes + ShiftRows

mov	12(R5),	R9	; 3 Get [1, 12]
mov	8(R5),	R10	; 3 Get [13, 8]
mov	4(R5),	R11	; 3 Get [9, 4]

mov	R10,	R12	; 1

			; 18 [13, 12]
mac_aes_unshift	R12, R9, R14, 12(R5), 12(R4), R7

mov	@R5,	R12	; 2 Get [5, 0]

			; 16 [1, 0]
mac_aes_unshift	R9, R12, R14, @R5, @R4, R7

			; 18 [5, 4]
mac_aes_unshift	R12, R11, R14, 4(R5), 4(R4), R7

			; 18 [9, 8]
mac_aes_unshift	R11, R10, R14, 8(R5), 8(R4), R7

; First two rows done.
; Do the next two rows.

mov	6(R5),	R11	; 3 Get [3, 14]
mov	2(R5),	R10	; 3 Get [15, 10]
mov	14(R5),	R9	; 3 Get [11, 6]

mov	R10,	R12	; 1

			; 18 [15, 14]
mac_aes_unshift		R12, R11, R14, 14(R5), 14(R4), R7

mov	10(R5),	R12	; 3 Get [7,  2]

			; 18 [3, 2]
mac_aes_unshift		R11, R12, R14, 2(R5), 2(R4), R7

			; 18 [7, 6]
mac_aes_unshift		R12, R9, R14, 6(R5), 6(R4), R7

			; 18 [11, 10]
mac_aes_unshift		R9, R10, R14, 10(R5), 10(R4), R7

mov	#sbox,		R7	; 2 get the sbox start addr
dec	R13			; 1
jnz	.LnextdRound		; 2 Next AES round

add	#16,		R1	; 2 Reset the stack

ret

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
