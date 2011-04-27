int kmain()
{
	char * screen = (char *)0xB8000;
	*screen = 0x70;
	*(screen+1) = '=';
	while(1);
	return 0;
}