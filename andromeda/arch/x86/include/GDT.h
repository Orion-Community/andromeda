#ifndef __GDT_H
#define __GDT_H

// Here goes the GDT entry data structure
struct gdtEntry
{
  unsigned int limitLow : 16; // Hold the lower part of the limit
  unsigned int baseLow : 24; // Holds the lower part of the base address of the segment.
  
  unsigned int type : 5; // Indicates the type of segment, code, data, executable, ...
  unsigned int pl : 2; // Holds the privilige ring, 0 for kernel level, 3 for user level, others aren't implemented.
  unsigned int present : 1; // Indicates the presence in memory (I think this should always be true in our case).
  unsigned int limitHigh : 4; //Holds the higher part of the limit.
  unsigned int attributes : 3; // Don't know what this does.
  unsigned int granularity : 1; // Indicates the way the limit needs to be read.
  unsigned int baseHigh : 8; // Holds the higher part of the base address of the segment.
}__attribute__((packed)); // To tell the compiler that these fields should be packed toggether(1)
typedef struct gdtEntry gdtEntry_t;

// Here comes the GDT pointer structure
struct gdtPtr
{
  unsigned int limit : 8; // Indicate where the table ends
  unsigned int baseAddr : 32; // Indicate where the table starts
}__attribute__((packed)); // To tell the compiler that these fields should be packed together(1)
typedef struct gdtPtr gdt_t;
// The load gdt instruction
extern void lgdt(gdt_t*);

#endif

/*
(1) Compilers often do some form of optimalisation, and this can be done by giving each field it's
    own byte. We don't want this. We want something which is in the form that the CPU know what we
    want. That's why pack the bytes together.
*/