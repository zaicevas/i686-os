/* The bootloader will start execution at the symbol designated as the entry point. */
ENTRY(_start)
 
SECTIONS
{
	/* Begin putting sections at 1 Megabyte, because memory below 1 Megabyte is reserved for other x86-related things */
	. = 1M;
 
	/* We align all sections in the executable at multiples of 4 Kilobytes (4K). This will become useful later in development when we add paging */
 
	/* First put the multiboot header, as it's required to be near the start of the executable otherwise the bootloader won't find it */
	/* The Multiboot header is Read-Only data, so we can put it in a '.rodata' section. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
	}
 
	/* Executable code */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}
