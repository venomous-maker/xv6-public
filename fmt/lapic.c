7400 // The local APIC manages internal (non-I/O) interrupts.
7401 // See Chapter 8 & Appendix C of Intel processor manual volume 3.
7402 
7403 #include "param.h"
7404 #include "types.h"
7405 #include "defs.h"
7406 #include "date.h"
7407 #include "memlayout.h"
7408 #include "traps.h"
7409 #include "mmu.h"
7410 #include "x86.h"
7411 
7412 // Local APIC registers, divided by 4 for use as uint[] indices.
7413 #define ID      (0x0020/4)   // ID
7414 #define VER     (0x0030/4)   // Version
7415 #define TPR     (0x0080/4)   // Task Priority
7416 #define EOI     (0x00B0/4)   // EOI
7417 #define SVR     (0x00F0/4)   // Spurious Interrupt Vector
7418   #define ENABLE     0x00000100   // Unit Enable
7419 #define ESR     (0x0280/4)   // Error Status
7420 #define ICRLO   (0x0300/4)   // Interrupt Command
7421   #define INIT       0x00000500   // INIT/RESET
7422   #define STARTUP    0x00000600   // Startup IPI
7423   #define DELIVS     0x00001000   // Delivery status
7424   #define ASSERT     0x00004000   // Assert interrupt (vs deassert)
7425   #define DEASSERT   0x00000000
7426   #define LEVEL      0x00008000   // Level triggered
7427   #define BCAST      0x00080000   // Send to all APICs, including self.
7428   #define BUSY       0x00001000
7429   #define FIXED      0x00000000
7430 #define ICRHI   (0x0310/4)   // Interrupt Command [63:32]
7431 #define TIMER   (0x0320/4)   // Local Vector Table 0 (TIMER)
7432   #define X1         0x0000000B   // divide counts by 1
7433   #define PERIODIC   0x00020000   // Periodic
7434 #define PCINT   (0x0340/4)   // Performance Counter LVT
7435 #define LINT0   (0x0350/4)   // Local Vector Table 1 (LINT0)
7436 #define LINT1   (0x0360/4)   // Local Vector Table 2 (LINT1)
7437 #define ERROR   (0x0370/4)   // Local Vector Table 3 (ERROR)
7438   #define MASKED     0x00010000   // Interrupt masked
7439 #define TICR    (0x0380/4)   // Timer Initial Count
7440 #define TCCR    (0x0390/4)   // Timer Current Count
7441 #define TDCR    (0x03E0/4)   // Timer Divide Configuration
7442 
7443 volatile uint *lapic;  // Initialized in mp.c
7444 
7445 
7446 
7447 
7448 
7449 
7450 static void
7451 lapicw(int index, int value)
7452 {
7453   lapic[index] = value;
7454   lapic[ID];  // wait for write to finish, by reading
7455 }
7456 
7457 void
7458 lapicinit(void)
7459 {
7460   if(!lapic)
7461     return;
7462 
7463   // Enable local APIC; set spurious interrupt vector.
7464   lapicw(SVR, ENABLE | (T_IRQ0 + IRQ_SPURIOUS));
7465 
7466   // The timer repeatedly counts down at bus frequency
7467   // from lapic[TICR] and then issues an interrupt.
7468   // If xv6 cared more about precise timekeeping,
7469   // TICR would be calibrated using an external time source.
7470   lapicw(TDCR, X1);
7471   lapicw(TIMER, PERIODIC | (T_IRQ0 + IRQ_TIMER));
7472   lapicw(TICR, 10000000);
7473 
7474   // Disable logical interrupt lines.
7475   lapicw(LINT0, MASKED);
7476   lapicw(LINT1, MASKED);
7477 
7478   // Disable performance counter overflow interrupts
7479   // on machines that provide that interrupt entry.
7480   if(((lapic[VER]>>16) & 0xFF) >= 4)
7481     lapicw(PCINT, MASKED);
7482 
7483   // Map error interrupt to IRQ_ERROR.
7484   lapicw(ERROR, T_IRQ0 + IRQ_ERROR);
7485 
7486   // Clear error status register (requires back-to-back writes).
7487   lapicw(ESR, 0);
7488   lapicw(ESR, 0);
7489 
7490   // Ack any outstanding interrupts.
7491   lapicw(EOI, 0);
7492 
7493   // Send an Init Level De-Assert to synchronise arbitration ID's.
7494   lapicw(ICRHI, 0);
7495   lapicw(ICRLO, BCAST | INIT | LEVEL);
7496   while(lapic[ICRLO] & DELIVS)
7497     ;
7498 
7499 
7500   // Enable interrupts on the APIC (but not on the processor).
7501   lapicw(TPR, 0);
7502 }
7503 
7504 int
7505 lapicid(void)
7506 {
7507   if (!lapic)
7508     return 0;
7509   return lapic[ID] >> 24;
7510 }
7511 
7512 // Acknowledge interrupt.
7513 void
7514 lapiceoi(void)
7515 {
7516   if(lapic)
7517     lapicw(EOI, 0);
7518 }
7519 
7520 // Spin for a given number of microseconds.
7521 // On real hardware would want to tune this dynamically.
7522 void
7523 microdelay(int us)
7524 {
7525 }
7526 
7527 #define CMOS_PORT    0x70
7528 #define CMOS_RETURN  0x71
7529 
7530 // Start additional processor running entry code at addr.
7531 // See Appendix B of MultiProcessor Specification.
7532 void
7533 lapicstartap(uchar apicid, uint addr)
7534 {
7535   int i;
7536   ushort *wrv;
7537 
7538   // "The BSP must initialize CMOS shutdown code to 0AH
7539   // and the warm reset vector (DWORD based at 40:67) to point at
7540   // the AP startup code prior to the [universal startup algorithm]."
7541   outb(CMOS_PORT, 0xF);  // offset 0xF is shutdown code
7542   outb(CMOS_PORT+1, 0x0A);
7543   wrv = (ushort*)P2V((0x40<<4 | 0x67));  // Warm reset vector
7544   wrv[0] = 0;
7545   wrv[1] = addr >> 4;
7546 
7547 
7548 
7549 
7550   // "Universal startup algorithm."
7551   // Send INIT (level-triggered) interrupt to reset other CPU.
7552   lapicw(ICRHI, apicid<<24);
7553   lapicw(ICRLO, INIT | LEVEL | ASSERT);
7554   microdelay(200);
7555   lapicw(ICRLO, INIT | LEVEL);
7556   microdelay(100);    // should be 10ms, but too slow in Bochs!
7557 
7558   // Send startup IPI (twice!) to enter code.
7559   // Regular hardware is supposed to only accept a STARTUP
7560   // when it is in the halted state due to an INIT.  So the second
7561   // should be ignored, but it is part of the official Intel algorithm.
7562   // Bochs complains about the second one.  Too bad for Bochs.
7563   for(i = 0; i < 2; i++){
7564     lapicw(ICRHI, apicid<<24);
7565     lapicw(ICRLO, STARTUP | (addr>>12));
7566     microdelay(200);
7567   }
7568 }
7569 
7570 #define CMOS_STATA   0x0a
7571 #define CMOS_STATB   0x0b
7572 #define CMOS_UIP    (1 << 7)        // RTC update in progress
7573 
7574 #define SECS    0x00
7575 #define MINS    0x02
7576 #define HOURS   0x04
7577 #define DAY     0x07
7578 #define MONTH   0x08
7579 #define YEAR    0x09
7580 
7581 static uint
7582 cmos_read(uint reg)
7583 {
7584   outb(CMOS_PORT,  reg);
7585   microdelay(200);
7586 
7587   return inb(CMOS_RETURN);
7588 }
7589 
7590 static void
7591 fill_rtcdate(struct rtcdate *r)
7592 {
7593   r->second = cmos_read(SECS);
7594   r->minute = cmos_read(MINS);
7595   r->hour   = cmos_read(HOURS);
7596   r->day    = cmos_read(DAY);
7597   r->month  = cmos_read(MONTH);
7598   r->year   = cmos_read(YEAR);
7599 }
7600 // qemu seems to use 24-hour GWT and the values are BCD encoded
7601 void
7602 cmostime(struct rtcdate *r)
7603 {
7604   struct rtcdate t1, t2;
7605   int sb, bcd;
7606 
7607   sb = cmos_read(CMOS_STATB);
7608 
7609   bcd = (sb & (1 << 2)) == 0;
7610 
7611   // make sure CMOS doesn't modify time while we read it
7612   for(;;) {
7613     fill_rtcdate(&t1);
7614     if(cmos_read(CMOS_STATA) & CMOS_UIP)
7615         continue;
7616     fill_rtcdate(&t2);
7617     if(memcmp(&t1, &t2, sizeof(t1)) == 0)
7618       break;
7619   }
7620 
7621   // convert
7622   if(bcd) {
7623 #define    CONV(x)     (t1.x = ((t1.x >> 4) * 10) + (t1.x & 0xf))
7624     CONV(second);
7625     CONV(minute);
7626     CONV(hour  );
7627     CONV(day   );
7628     CONV(month );
7629     CONV(year  );
7630 #undef     CONV
7631   }
7632 
7633   *r = t1;
7634   r->year += 2000;
7635 }
7636 
7637 
7638 
7639 
7640 
7641 
7642 
7643 
7644 
7645 
7646 
7647 
7648 
7649 
