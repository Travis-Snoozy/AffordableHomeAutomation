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

.text
.global flash_unlock
flash_unlock:
mov	#FWKEY,		&FCTL3
mov	#FWKEY+WRT,	&FCTL1
ret

.global flash_lock
flash_lock:
mov	#FWKEY,		&FCTL1
mov	#FWKEY+LOCK,	&FCTL3
ret

.global flash_erase
flash_erase:
mov	#FWKEY,		&FCTL3
mov	#FWKEY+ERASE,	&FCTL1
mov.b	#0,		0(R12)
mov	#FWKEY+LOCK,	&FCTL3
ret

; vim: set noexpandtab ts=8 sts=0 sw=8 fileencoding=utf-8:
