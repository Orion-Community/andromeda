/*
 * This is the inital boot image loaded from
 * grub. This will call the actual kernel
 * from where-ever it might be. From disk
 * (hope-not) of from gzip image in memory
 * (would be alot better).
 */

#include <text.h>

void announce()
{
	textInit();
	println("Compressed kernel loaded");
	println("Decompressing the kernel");
}

int kmain(/* boot data , boot data , gzipped kernel*/)
{
	announce();
	//installInterruptVectorTable();
	//initPaging();
	//exec(decompress(gzipped kernel));

	for (;;); // Prevent the CPU from
		  // doing stupid things.
	return 0;
}
