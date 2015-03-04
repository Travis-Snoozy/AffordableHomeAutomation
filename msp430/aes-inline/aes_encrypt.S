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
.global c_aes_encrypt
c_aes_encrypt:
push	R4			; 3
push	R5			; 3
push	R6			; 3
push	R7			; 3
push	R8			; 3
push	R9			; 3
push	R10			; 3
mov	R12,		R4	; 1
mov	R13,		R5	; 1
call	#aes_encrypt		; 5
pop	R10			; 2
pop	R9			; 2
pop	R8			; 2
pop	R7			; 2
pop	R6			; 2
pop	R5			; 2
pop	R4			; 2
ret				; 3

.global aes_encrypt
; aes_encrypt
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
; R15=c round constant
aes_encrypt:

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
mov	#1,		R15	; 1 Initialize round constant

.LnextRound:

; AddRoundKey + SubBytes + ShiftRows

mov	@R4,	R9		; 2 Get [1, 0]
xor	@R5,	R9		; 2 AddRoundKey
mov	4(R4),	R10		; 3 Get [5, 4]
xor	4(R5),	R10		; 3 AddRoundKey
mov	8(R4),	R11		; 3 Get [9, 8]
xor	8(R5),	R11		; 3 AddRoundKey


mov	R10,	R12		; 1

				; 14 [5, 0] -> [1, 0]
mac_aes_shift		R12, R9, R14, @R5, R7

mov	12(R4),	R12		; 3 Get [13, 12]
xor	12(R5),	R12		; 3 AddRoundKey

				; 15 [1, 12] -> [13, 12]
mac_aes_shift		R9, R12, R14, 12(R5), R7

				; 15 [13, 8] -> [9, 8]
mac_aes_shift		R12, R11, R14, 8(R5), R7

				; 15 [9, 4] -> [5, 4]
mac_aes_shift		R11, R10, R14, 4(R5), R7

; First two rows done.
; Do the next two rows.

mov	14(R4),	R9		; 3 Get [15, 14]
xor	14(R5),	R9		; 3 AddRoundKey
mov	10(R4),	R10		; 3 Get [11, 10]
xor	10(R5),	R10		; 3 AddRoundKey
mov	6(R4),	R11		; 3 Get [ 7,  6]
xor	6(R5),	R11		; 3 AddRoundKey

mov	R10,	R12		; 1

				; 15 [11, 6] -> [15, 14]
mac_aes_shift		R12, R11, R14, 14(R5), R7

mov	2(R4),	R12		; 3 Get [3, 2]
xor	2(R5),	R12		; 3 AddRoundKey

				; 15 [7, 2] -> [11, 10]
mac_aes_shift		R11, R12, R14, 10(R5), R7

				; 15 [3, 14] -> [7, 6]
mac_aes_shift		R12, R9, R14, 6(R5), R7

				; 15 [15, 10] -> [3, 2]
mac_aes_shift		R9, R10, R14, 2(R5), R7

; ---- Expand key ----
mov	#1,		R11	; 1 Run the expand loop once
call	#aes_expand_key_inline	; 5
; ----    end     ----
; ---- Expand key ----

cmp	#1,		R13	; 1 Is this the final round?
jeq	.LskipMixColumns	; 2 Final round skips mixing

; MixColumns

mov	#4,		R8	; 2 Set the inner loop counter

.LnextColumn:

mov.b	@R5+,		R9	; 2 Save the column data
mov.b	@R5+,		R10	; 2
mov.b	@R5+,		R11	; 2
mov.b	@R5+,		R12	; 2

				; 52
mac_aes_mixcols	R9, R10, R11, R12, R14, R6, R5

dec	R8			; 1
jnz	.LnextColumn		; 2 Next MixColumns loop iteration

sub	#16,		R5	; 2 Reset the data pointer

.LskipMixColumns:

dec	R13			; 1
jnz	.LnextRound		; 2 Next AES round

xor	@R4+,		 0(R5)	; 5
xor	@R4+,		 2(R5)	; 5
xor	@R4+,		 4(R5)	; 5
xor	@R4+,		 6(R5)	; 5
xor	@R4+,		 8(R5)	; 5
xor	@R4+,		10(R5)	; 5
xor	@R4+,		12(R5)	; 5
xor	@R4,		14(R5)	; 5

add	#16,		R1	; 2 Reset the stack

ret				; 3

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
