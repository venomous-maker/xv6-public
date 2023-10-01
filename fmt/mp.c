7250 // Multiprocessor support
7251 // Search memory for MP description structures.
7252 // http://developer.intel.com/design/pentium/datashts/24201606.pdf
7253 
7254 #include "types.h"
7255 #include "defs.h"
7256 #include "param.h"
7257 #include "memlayout.h"
7258 #include "mp.h"
7259 #include "x86.h"
7260 #include "mmu.h"
7261 #include "proc.h"
7262 
7263 struct cpu cpus[NCPU];
7264 int ncpu;
7265 uchar ioapicid;
7266 
7267 static uchar
7268 sum(uchar *addr, int len)
7269 {
7270   int i, sum;
7271 
7272   sum = 0;
7273   for(i=0; i<len; i++)
7274     sum += addr[i];
7275   return sum;
7276 }
7277 
7278 // Look for an MP structure in the len bytes at addr.
7279 static struct mp*
7280 mpsearch1(uint a, int len)
7281 {
7282   uchar *e, *p, *addr;
7283 
7284   addr = P2V(a);
7285   e = addr+len;
7286   for(p = addr; p < e; p += sizeof(struct mp))
7287     if(memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
7288       return (struct mp*)p;
7289   return 0;
7290 }
7291 
7292 
7293 
7294 
7295 
7296 
7297 
7298 
7299 
7300 // Search for the MP Floating Pointer Structure, which according to the
7301 // spec is in one of the following three locations:
7302 // 1) in the first KB of the EBDA;
7303 // 2) in the last KB of system base memory;
7304 // 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
7305 static struct mp*
7306 mpsearch(void)
7307 {
7308   uchar *bda;
7309   uint p;
7310   struct mp *mp;
7311 
7312   bda = (uchar *) P2V(0x400);
7313   if((p = ((bda[0x0F]<<8)| bda[0x0E]) << 4)){
7314     if((mp = mpsearch1(p, 1024)))
7315       return mp;
7316   } else {
7317     p = ((bda[0x14]<<8)|bda[0x13])*1024;
7318     if((mp = mpsearch1(p-1024, 1024)))
7319       return mp;
7320   }
7321   return mpsearch1(0xF0000, 0x10000);
7322 }
7323 
7324 // Search for an MP configuration table.  For now,
7325 // don't accept the default configurations (physaddr == 0).
7326 // Check for correct signature, calculate the checksum and,
7327 // if correct, check the version.
7328 // To do: check extended table checksum.
7329 static struct mpconf*
7330 mpconfig(struct mp **pmp)
7331 {
7332   struct mpconf *conf;
7333   struct mp *mp;
7334 
7335   if((mp = mpsearch()) == 0 || mp->physaddr == 0)
7336     return 0;
7337   conf = (struct mpconf*) P2V((uint) mp->physaddr);
7338   if(memcmp(conf, "PCMP", 4) != 0)
7339     return 0;
7340   if(conf->version != 1 && conf->version != 4)
7341     return 0;
7342   if(sum((uchar*)conf, conf->length) != 0)
7343     return 0;
7344   *pmp = mp;
7345   return conf;
7346 }
7347 
7348 
7349 
7350 void
7351 mpinit(void)
7352 {
7353   uchar *p, *e;
7354   int ismp;
7355   struct mp *mp;
7356   struct mpconf *conf;
7357   struct mpproc *proc;
7358   struct mpioapic *ioapic;
7359 
7360   if((conf = mpconfig(&mp)) == 0)
7361     panic("Expect to run on an SMP");
7362   ismp = 1;
7363   lapic = (uint*)conf->lapicaddr;
7364   for(p=(uchar*)(conf+1), e=(uchar*)conf+conf->length; p<e; ){
7365     switch(*p){
7366     case MPPROC:
7367       proc = (struct mpproc*)p;
7368       if(ncpu < NCPU) {
7369         cpus[ncpu].apicid = proc->apicid;  // apicid may differ from ncpu
7370         ncpu++;
7371       }
7372       p += sizeof(struct mpproc);
7373       continue;
7374     case MPIOAPIC:
7375       ioapic = (struct mpioapic*)p;
7376       ioapicid = ioapic->apicno;
7377       p += sizeof(struct mpioapic);
7378       continue;
7379     case MPBUS:
7380     case MPIOINTR:
7381     case MPLINTR:
7382       p += 8;
7383       continue;
7384     default:
7385       ismp = 0;
7386       break;
7387     }
7388   }
7389   if(!ismp)
7390     panic("Didn't find a suitable machine");
7391 
7392   if(mp->imcrp){
7393     // Bochs doesn't support IMCR, so this doesn't run on Bochs.
7394     // But it would on real hardware.
7395     outb(0x22, 0x70);   // Select IMCR
7396     outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
7397   }
7398 }
7399 
