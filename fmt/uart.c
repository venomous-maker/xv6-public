8350 // Intel 8250 serial port (UART).
8351 
8352 #include "types.h"
8353 #include "defs.h"
8354 #include "param.h"
8355 #include "traps.h"
8356 #include "spinlock.h"
8357 #include "sleeplock.h"
8358 #include "fs.h"
8359 #include "file.h"
8360 #include "mmu.h"
8361 #include "proc.h"
8362 #include "x86.h"
8363 
8364 #define COM1    0x3f8
8365 
8366 static int uart;    // is there a uart?
8367 
8368 void
8369 uartinit(void)
8370 {
8371   char *p;
8372 
8373   // Turn off the FIFO
8374   outb(COM1+2, 0);
8375 
8376   // 9600 baud, 8 data bits, 1 stop bit, parity off.
8377   outb(COM1+3, 0x80);    // Unlock divisor
8378   outb(COM1+0, 115200/9600);
8379   outb(COM1+1, 0);
8380   outb(COM1+3, 0x03);    // Lock divisor, 8 data bits.
8381   outb(COM1+4, 0);
8382   outb(COM1+1, 0x01);    // Enable receive interrupts.
8383 
8384   // If status is 0xFF, no serial port.
8385   if(inb(COM1+5) == 0xFF)
8386     return;
8387   uart = 1;
8388 
8389   // Acknowledge pre-existing interrupt conditions;
8390   // enable interrupts.
8391   inb(COM1+2);
8392   inb(COM1+0);
8393   ioapicenable(IRQ_COM1, 0);
8394 
8395   // Announce that we're here.
8396   for(p="xv6...\n"; *p; p++)
8397     uartputc(*p);
8398 }
8399 
8400 void
8401 uartputc(int c)
8402 {
8403   int i;
8404 
8405   if(!uart)
8406     return;
8407   for(i = 0; i < 128 && !(inb(COM1+5) & 0x20); i++)
8408     microdelay(10);
8409   outb(COM1+0, c);
8410 }
8411 
8412 static int
8413 uartgetc(void)
8414 {
8415   if(!uart)
8416     return -1;
8417   if(!(inb(COM1+5) & 0x01))
8418     return -1;
8419   return inb(COM1+0);
8420 }
8421 
8422 void
8423 uartintr(void)
8424 {
8425   consoleintr(uartgetc);
8426 }
8427 
8428 
8429 
8430 
8431 
8432 
8433 
8434 
8435 
8436 
8437 
8438 
8439 
8440 
8441 
8442 
8443 
8444 
8445 
8446 
8447 
8448 
8449 
