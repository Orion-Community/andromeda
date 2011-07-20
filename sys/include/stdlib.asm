;
;    Get the system memory map from the bios and store it in a global constant.
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

%ifndef __STDLIB_H

regs:
	eax_t dd 0
	ebx_t dd 0
	ecx_t dd 0
	edx_t dd 0
	esi_t dd 0
	edi_t dd 0
	ebp_t dd 0
	esp_t dd 0
	
segs:
	ds_t dw 0
	cs_t dw 0
	es_t dw 0
	fs_t dw 0
	gs_t dw 0
	ss_t dw 0

%endif