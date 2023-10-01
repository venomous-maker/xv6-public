7650 // The I/O APIC manages hardware interrupts for an SMP system.
7651 // http://www.intel.com/design/chipsets/datashts/29056601.pdf
7652 // See also picirq.c.
7653 
7654 #include "types.h"
7655 #include "defs.h"
7656 #include "traps.h"
7657 
7658 #define IOAPIC  0xFEC00000   // Default physical address of IO APIC
7659 
7660 #define REG_ID     0x00  // Register index: ID
7661 #define REG_VER    0x01  // Register index: version
7662 #define REG_TABLE  0x10  // Redirection table base
7663 
7664 // The redirection table starts at REG_TABLE and uses
7665 // two registers to configure each interrupt.
7666 // The first (low) register in a pair contains configuration bits.
7667 // The second (high) register contains a bitmask telling which
7668 // CPUs can serve that interrupt.
7669 #define INT_DISABLED   0x00010000  // Interrupt disabled
7670 #define INT_LEVEL      0x00008000  // Level-triggered (vs edge-)
7671 #define INT_ACTIVELOW  0x00002000  // Active low (vs high)
7672 #define INT_LOGICAL    0x00000800  // Destination is CPU id (vs APIC ID)
7673 
7674 volatile struct ioapic *ioapic;
7675 
7676 // IO APIC MMIO structure: write reg, then read or write data.
7677 struct ioapic {
7678   uint reg;
7679   uint pad[3];
7680   uint data;
7681 };
7682 
7683 static uint
7684 ioapicread(int reg)
7685 {
7686   ioapic->reg = reg;
7687   return ioapic->data;
7688 }
7689 
7690 static void
7691 ioapicwrite(int reg, uint data)
7692 {
7693   ioapic->reg = reg;
7694   ioapic->data = data;
7695 }
7696 
7697 
7698 
7699 
7700 void
7701 ioapicinit(void)
7702 {
7703   int i, id, maxintr;
7704 
7705   ioapic = (volatile struct ioapic*)IOAPIC;
7706   maxintr = (ioapicread(REG_VER) >> 16) & 0xFF;
7707   id = ioapicread(REG_ID) >> 24;
7708   if(id != ioapicid)
7709     cprintf("ioapicinit: id isn't equal to ioapicid; not a MP\n");
7710 
7711   // Mark all interrupts edge-triggered, active high, disabled,
7712   // and not routed to any CPUs.
7713   for(i = 0; i <= maxintr; i++){
7714     ioapicwrite(REG_TABLE+2*i, INT_DISABLED | (T_IRQ0 + i));
7715     ioapicwrite(REG_TABLE+2*i+1, 0);
7716   }
7717 }
7718 
7719 void
7720 ioapicenable(int irq, int cpunum)
7721 {
7722   // Mark interrupt edge-triggered, active high,
7723   // enabled, and routed to the given cpunum,
7724   // which happens to be that cpu's APIC ID.
7725   ioapicwrite(REG_TABLE+2*irq, T_IRQ0 + irq);
7726   ioapicwrite(REG_TABLE+2*irq+1, cpunum << 24);
7727 }
7728 
7729 
7730 
7731 
7732 
7733 
7734 
7735 
7736 
7737 
7738 
7739 
7740 
7741 
7742 
7743 
7744 
7745 
7746 
7747 
7748 
7749 
