;
;    The interrupt request header.
;    Copyright (C) 2011 Michel Megens
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

%macro isa_irq 1
[GLOBAL irq%1]
[EXTERN cIRQ%1]
irq%1:
        cli	; no interrupts while we handle this one
        push cIRQ%1
        mov eax, %1
        push eax
        jmp irqStub
%endmacro

[EXTERN do_irq]
%macro irq 1
[GLOBAL irq%1]
irq%1:
  cli
  push do_irq
  mov eax, %1
  push eax
  jmp irqStub
%endmacro


isa_irq 0
isa_irq 1
isa_irq 2
isa_irq 3
isa_irq 4
isa_irq 5
isa_irq 6
isa_irq 7
isa_irq 8
isa_irq 9
isa_irq 10
isa_irq 11
isa_irq 12
isa_irq 13
isa_irq 14
isa_irq 15
%if 0
irq 16
irq 17
irq 18
irq 19
irq 20
irq 21
irq 22
irq 23
irq 24
irq 25
irq 26
irq 27
irq 28
irq 29
irq 30
irq 31
irq 32
irq 33
irq 34
irq 35
irq 36
irq 37
irq 38
irq 39
irq 40
irq 41
irq 42
irq 43
irq 44
irq 45
irq 46
irq 47
irq 48
irq 49
irq 50
irq 51
irq 52
irq 53
irq 54
irq 55
irq 56
irq 57
irq 58
irq 59
irq 60
irq 61
irq 62
irq 63
irq 64
irq 65
irq 66
irq 67
irq 68
irq 69
irq 70
irq 71
irq 72
irq 73
irq 74
irq 75
irq 76
irq 77
irq 78
irq 79
irq 80
irq 81
irq 82
irq 83
irq 84
irq 85
irq 86
irq 87
irq 88
irq 89
irq 90
irq 91
irq 92
irq 93
irq 94
irq 95
irq 96
irq 97
irq 98
irq 99
irq 100
irq 101
irq 102
irq 103
irq 104
irq 105
irq 106
irq 107
irq 108
irq 109
irq 110
irq 111
irq 112
irq 113
irq 114
irq 115
irq 116
irq 117
irq 118
irq 119
irq 120
irq 121
irq 122
irq 123
irq 124
irq 125
irq 126
irq 127
irq 128
irq 129
irq 130
irq 131
irq 132
irq 133
irq 134
irq 135
irq 136
irq 137
irq 138
irq 139
irq 140
irq 141
irq 142
irq 143
irq 144
irq 145
irq 146
irq 147
irq 148
irq 149
irq 150
irq 151
irq 152
irq 153
irq 154
irq 155
irq 156
irq 157
irq 158
irq 159
irq 160
irq 161
irq 162
irq 163
irq 164
irq 165
irq 166
irq 167
irq 168
irq 169
irq 170
irq 171
irq 172
irq 173
irq 174
irq 175
irq 176
irq 177
irq 178
irq 179
irq 180
irq 181
irq 182
irq 183
irq 184
irq 185
irq 186
irq 187
irq 188
irq 189
irq 190
irq 191
irq 192
irq 193
irq 194
irq 195
irq 196
irq 197
irq 198
irq 199
irq 200
irq 201
irq 202
irq 203
irq 204
irq 205
irq 206
irq 207
irq 208
irq 209
irq 210
irq 211
irq 212
irq 213
irq 214
irq 215
irq 216
irq 217
irq 218
irq 219
irq 220
irq 221
irq 222
irq 223
%endif

; [GLOBAL systemcall]
; [EXTERN proberam]
; systemcall:
; 	cli
; 	push proberam
; 	jmp irqStub

irqStub:
  pop eax
	pushad
; 	push ebp
	mov ebp, esp
	mov eax, [ebp+32]

	mov dx, ds
	push edx
	mov dx, 0x10	; kernel ring

	mov ds, dx
	mov es, dx
	mov fs, dx
	mov gs, dx

	push esp

	call eax

	pop esp

	pop edx
	mov ds, dx
	mov es, dx
	mov gs, dx
	mov gs, dx

	popad
; 	pop ebp
	add esp, 4	; pop func pointer
	iret	; interrupt return

; [EXTERN test_func]
[EXTERN putc]
[GLOBAL gen_irq_stub]
gen_irq_stub:
        cli
        pushad
        xor edx, edx
        mov dx, ds
        push edx

        mov dx, 0x10
        mov ds, dx
        mov es, dx
        mov fs, dx
        mov gs, dx


        call label
        label:
        pop eax

        push dword [eax + (irq_num_addr - label)]
        call [eax + (irq_fp - label)]
        add esp, 4

        pop edx

        mov ds, dx
        mov es, dx
        mov fs, dx
        mov gs, dx
        sti
        popad
        iret
irq_num_addr:  dd 0
irq_fp:  dd 0
[GLOBAL __end_of_irq_stub]
__end_of_irq_stub:

[GLOBAL exec_addr]
exec_addr:
  push ebp
  mov ebp, esp
  mov eax, [ebp+8]
  call eax
  pop ebp
  ret


[GLOBAL trigger_soft_irq30]
trigger_soft_irq30:
  pushad
  int 0x20
  popad
  ret

%ifdef IRG_DBG
[GLOBAL trigger_soft_irq31]
trigger_soft_irq31:
  pushad
  int 0x31
  popad
  ret
%endif
