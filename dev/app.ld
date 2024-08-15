SECTIONS
{
	.text 0x000000 : { *(.text) }
	/* .data 0x1000 : { *(.data) }*/
	.rodata 0x400 : { *(.rodata) }

	PROVIDE(printf = 0x201400);
}
