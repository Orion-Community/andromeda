/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DRIVERS_VGA_TEXT_H
#define __DRIVERS_VGA_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

int vga_text_write(struct vfile* this, char* buf, size_t num);
int vga_text_read(struct vfile*  this, char* buf, size_t num);
int vga_text_seek(struct vfile*  this, size_t num, seek_t from);
int vga_text_flush(struct vfile* this);
int vga_text_close(struct vfile* this);
int vga_text_detect(struct device* this);
int vga_text_attach(struct device* this);
int vga_text_detach(struct device* this);
int vga_text_suspend(struct device* this);
int vga_text_resume(struct device* this);

struct vfile* vga_text_open(struct device *this);

#ifdef __cplusplus
}
#endif

#endif
