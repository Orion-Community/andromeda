;
;    Header file for the master boot record.
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

%ifndef __H_MASTERBOOT
%define __H_MASTERBOOT

; here is the mbr loaded into by the bios
%define LOADOFF 0x7c00

; the mbr will migrate itself to this segment:offset address
%define BUFSEG 0x0
%define BUFOFF 0x500
%endif
