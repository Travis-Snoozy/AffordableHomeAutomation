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

.text
.global c_gcm_galois_mult
c_gcm_galois_mult:
push	R4				; 3
push	R5				; 3
push	R6				; 3
push	R7				; 3
push	R8				; 3
push	R9				; 3
push	R10				; 3
call	#gcm_galois_mult		; 5
pop	R10				; 2
pop	R9				; 2
pop	R8				; 2
pop	R7				; 2
pop	R6				; 2
pop	R5				; 2
pop	R4				; 2
ret					; 3

.global gcm_galois_mult
; R4=c Calc (MSW)
; R5=c Calc
; R6=c Calc
; R7=c Calc
; R8=c Calc
; R9=c Calc
; R10=c Calc
; R11=c Calc (LSW)
; R12=i Dst
; R13=i Src
; R14=c Loop counter
; R15=c Current src value
gcm_galois_mult:

mov	14(R13),	R4		; 3 Copy the eighth word (LSW) of the const data
swpb	R4				; 1 Swap the data (it's in memory as big-endian)
push	R4				; 3 Save the copy to our stack.
mov	12(R13),	R4		; 3 Seventh word
swpb	R4				; 1
push	R4				; 3
mov	10(R13),	R4		; 3 Sixth word
swpb	R4				; 1
push	R4				; 3
mov	8(R13),		R4		; 3 Fifth word
swpb	R4				; 1
push	R4				; 3
mov	6(R13),		R4		; 3 Fourth word
swpb	R4				; 1
push	R4				; 3
mov	4(R13),		R4		; 3 Third word
swpb	R4				; 1
push	R4				; 3
mov	2(R13),		R4		; 3 Second word
swpb	R4				; 1
push	R4				; 3
mov	@R13,		R4		; 2 First word (MSW)
swpb	R4				; 1
push	R4				; 3
mov	R1,		R13		; 1 Copy the current stack position

clr	R4				; 1 Zero the destination registers
clr	R5				; 1
clr	R6				; 1
clr	R7				; 1
clr	R8				; 1
clr	R9				; 1
clr	R10				; 1
clr	R11				; 1

mov	#128,		R14		; 2 Set up the bit counter
jmp	.LloadSrc			; 2 Bootstrap -- load the first src byte.
.Lloop:					; 0
					;   --- Important! ---
clrc					; 1 Carry needs to be clear for subsequent loops; we want
                                        ;   only one sentinal.
					;   ---            ---
.LloopBypass:				; 0
rlc	R15				; 1 Shift the next bit up for testing
jnz	.LbyteNotFinished		; 2 If we have bits still set (i.e., sentinal), keep processing the byte.
					;   Else we're full zero, the byte is done; get a new one
.LloadSrc:				; 0
mov	@R12+,		R15		; 2 Load the next byte from the dest.
swpb	R15				; 1 Swap the bytes
					;   --- Important! ---
setc					; 1 Carry needs to be set so we get a sentinal on RLC.
					;   ---            ---
jmp	.LloopBypass			; 2 Lock & load with the new byte.
.LbyteNotFinished:			; 0
jnc	.LsrcUnset			; 2 If the bit that was shifted out (high bit) was unset, bypass XOR.
xor	@R13+,		R4		; 2 XOR our current value into the dest
xor	@R13+,		R5		; 2
xor	@R13+,		R6		; 2
xor	@R13+,		R7		; 2
xor	@R13+,		R8		; 2
xor	@R13+,		R9		; 2
xor	@R13+,		R10		; 2
xor	@R13+,		R11		; 2
sub	#16,		R13		; 2 Reset the pointer
clrc					; 1 Clear the carry bit to prep for MATH!
					;   (Carry bit will already be clear if the jump to LsrcUnset was taken)
.LsrcUnset:				; 0
rrc	@R13+				; 3 Rotate the whole big number to the right.
rrc	@R13+				; 3
rrc	@R13+				; 3
rrc	@R13+				; 3
rrc	@R13+				; 3
rrc	@R13+				; 3
rrc	@R13+				; 3
rrc	@R13+				; 3
jnc	.LskipRXor			; 2 If the LSB was unset, bypass the R-constant XOR.
xor	#0xE100,	-16(R13)	; 5 XOR in the R constant
.LskipRXor:				; 0
sub	#16,		R13		; 2 Reset the pointer
dec	R14				; 1 Decrement the loop.
jnz	.Lloop				; 2 If not, carry on.
swpb	R4				; 1 Reset the byte order before saving
swpb	R5				; 1
swpb	R6				; 1
swpb	R7				; 1
swpb	R8				; 1
swpb	R9				; 1
swpb	R10				; 1
swpb	R11				; 1
mov	R4,		-16(R12)	; 4 Save the results back to the destination
mov	R5,		-14(R12)	; 4
mov	R6,		-12(R12)	; 4
mov	R7,		-10(R12)	; 4
mov	R8,		-8(R12)		; 4
mov	R9,		-6(R12)		; 4
mov	R10,		-4(R12)		; 4
mov	R11,		-2(R12)		; 4
add	#16,		R1		; 2 Reset the stack pointer
ret					; 3

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
