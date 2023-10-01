9350 /* Simple linker script for the JOS kernel.
9351    See the GNU ld 'info' manual ("info ld") to learn the syntax. */
9352 
9353 OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
9354 OUTPUT_ARCH(i386)
9355 ENTRY(_start)
9356 
9357 SECTIONS
9358 {
9359 	/* Link the kernel at this address: "." means the current address */
9360         /* Must be equal to KERNLINK */
9361 	. = 0x80100000;
9362 
9363 	.text : AT(0x100000) {
9364 		*(.text .stub .text.* .gnu.linkonce.t.*)
9365 	}
9366 
9367 	PROVIDE(etext = .);	/* Define the 'etext' symbol to this value */
9368 
9369 	.rodata : {
9370 		*(.rodata .rodata.* .gnu.linkonce.r.*)
9371 	}
9372 
9373 	/* Include debugging information in kernel memory */
9374 	.stab : {
9375 		PROVIDE(__STAB_BEGIN__ = .);
9376 		*(.stab);
9377 		PROVIDE(__STAB_END__ = .);
9378 	}
9379 
9380 	.stabstr : {
9381 		PROVIDE(__STABSTR_BEGIN__ = .);
9382 		*(.stabstr);
9383 		PROVIDE(__STABSTR_END__ = .);
9384 	}
9385 
9386 	/* Adjust the address for the data segment to the next page */
9387 	. = ALIGN(0x1000);
9388 
9389 	/* Conventionally, Unix linkers provide pseudo-symbols
9390 	 * etext, edata, and end, at the end of the text, data, and bss.
9391 	 * For the kernel mapping, we need the address at the beginning
9392 	 * of the data section, but that's not one of the conventional
9393 	 * symbols, because the convention started before there was a
9394 	 * read-only rodata section between text and data. */
9395 	PROVIDE(data = .);
9396 
9397 
9398 
9399 
9400 	/* The data segment */
9401 	.data : {
9402 		*(.data)
9403 	}
9404 
9405 	PROVIDE(edata = .);
9406 
9407 	.bss : {
9408 		*(.bss)
9409 	}
9410 
9411 	PROVIDE(end = .);
9412 
9413 	/DISCARD/ : {
9414 		*(.eh_frame .note.GNU-stack)
9415 	}
9416 }
9417 
9418 
9419 
9420 
9421 
9422 
9423 
9424 
9425 
9426 
9427 
9428 
9429 
9430 
9431 
9432 
9433 
9434 
9435 
9436 
9437 
9438 
9439 
9440 
9441 
9442 
9443 
9444 
9445 
9446 
9447 
9448 
9449 
