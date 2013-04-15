/*
 *   The openLoader project - Intel (/AMD) CPU features.
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#define OL_CPUID_AVAILABLE 0x1
#define OL_CPUID_TEST_BIT 0x200000

#ifndef CPU_H
#define	CPU_H

#ifdef	__cplusplus
extern "C"
{
#endif
  extern mutex_t cpu_lock;

  typedef uint8_t ol_lock_t;

  struct ol_gen_regs
  {
    uint32_t eax, ebx, ecx, edx;
  } __attribute__((packed));
  typedef struct ol_gen_regs *ol_gen_registers_t;

  typedef struct ol_cpu_model
  {
    uint32_t vendor_id, family;
    const uint8_t *model_name;
  } *ol_cpu_model_t;

  typedef struct ol_cpu
  {
    /* model info*/
    ol_cpu_model_t cpu_models[4];
    char* vendor;

    /* CPU device info */
    uint8_t flags;
    uint64_t bus_width;

    void (*lock)(mutex_t*);
    void (*unlock)(mutex_t*);

  } *ol_cpu_t;
#if 0
  /* CPU feature functions */
  static int
  ol_cpuid_available(ol_cpu_t);
#endif
  void
  ol_cpu_init(ol_cpu_t);

  ol_gen_registers_t
  ol_cpuid(uint32_t func);

  uint32_t
  ol_get_eflags(void);

  void
  ol_set_eflags(uint32_t);
#if 0
  /* LOCKS */
  static void /* lock spin lock */
  ol_mutex_lock(mutex_t*);

  static void /* spin release */
  ol_mutex_release(mutex_t*);

  /* CPUID */
  static ol_gen_registers_t
  __ol_cpuid(volatile ol_gen_registers_t);

  /*
   * Inside function to write to a Model Specific Register
   */
  static uint64_t
  __read_msr(uint32_t);

  /*
   * Inside function to read a Model Specific Register
   */
  static void
  __write_msr(uint32_t, uint64_t);
#endif
  /*
   * Read a model specific register.
   */
  uint64_t
  cpu_read_msr(uint32_t);

  /*
   * Write a value to a model specific register.
   */
  void
  cpu_write_msr(uint32_t, uint64_t);

  extern volatile ol_cpu_t cpus;

#ifdef	__cplusplus
}
#endif

#endif	/* CPU_H */

