7950 // Console input and output.
7951 // Input is from the keyboard or serial port.
7952 // Output is written to the screen and serial port.
7953 
7954 #include "types.h"
7955 #include "defs.h"
7956 #include "param.h"
7957 #include "traps.h"
7958 #include "spinlock.h"
7959 #include "sleeplock.h"
7960 #include "fs.h"
7961 #include "file.h"
7962 #include "memlayout.h"
7963 #include "mmu.h"
7964 #include "proc.h"
7965 #include "x86.h"
7966 
7967 static void consputc(int);
7968 
7969 static int panicked = 0;
7970 
7971 static struct {
7972   struct spinlock lock;
7973   int locking;
7974 } cons;
7975 
7976 static void
7977 printint(int xx, int base, int sign)
7978 {
7979   static char digits[] = "0123456789abcdef";
7980   char buf[16];
7981   int i;
7982   uint x;
7983 
7984   if(sign && (sign = xx < 0))
7985     x = -xx;
7986   else
7987     x = xx;
7988 
7989   i = 0;
7990   do{
7991     buf[i++] = digits[x % base];
7992   }while((x /= base) != 0);
7993 
7994   if(sign)
7995     buf[i++] = '-';
7996 
7997   while(--i >= 0)
7998     consputc(buf[i]);
7999 }
8000 
8001 
8002 
8003 
8004 
8005 
8006 
8007 
8008 
8009 
8010 
8011 
8012 
8013 
8014 
8015 
8016 
8017 
8018 
8019 
8020 
8021 
8022 
8023 
8024 
8025 
8026 
8027 
8028 
8029 
8030 
8031 
8032 
8033 
8034 
8035 
8036 
8037 
8038 
8039 
8040 
8041 
8042 
8043 
8044 
8045 
8046 
8047 
8048 
8049 
8050 // Print to the console. only understands %d, %x, %p, %s.
8051 void
8052 cprintf(char *fmt, ...)
8053 {
8054   int i, c, locking;
8055   uint *argp;
8056   char *s;
8057 
8058   locking = cons.locking;
8059   if(locking)
8060     acquire(&cons.lock);
8061 
8062   if (fmt == 0)
8063     panic("null fmt");
8064 
8065   argp = (uint*)(void*)(&fmt + 1);
8066   for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
8067     if(c != '%'){
8068       consputc(c);
8069       continue;
8070     }
8071     c = fmt[++i] & 0xff;
8072     if(c == 0)
8073       break;
8074     switch(c){
8075     case 'd':
8076       printint(*argp++, 10, 1);
8077       break;
8078     case 'x':
8079     case 'p':
8080       printint(*argp++, 16, 0);
8081       break;
8082     case 's':
8083       if((s = (char*)*argp++) == 0)
8084         s = "(null)";
8085       for(; *s; s++)
8086         consputc(*s);
8087       break;
8088     case '%':
8089       consputc('%');
8090       break;
8091     default:
8092       // Print unknown % sequence to draw attention.
8093       consputc('%');
8094       consputc(c);
8095       break;
8096     }
8097   }
8098 
8099 
8100   if(locking)
8101     release(&cons.lock);
8102 }
8103 
8104 void
8105 panic(char *s)
8106 {
8107   int i;
8108   uint pcs[10];
8109 
8110   cli();
8111   cons.locking = 0;
8112   // use lapiccpunum so that we can call panic from mycpu()
8113   cprintf("lapicid %d: panic: ", lapicid());
8114   cprintf(s);
8115   cprintf("\n");
8116   getcallerpcs(&s, pcs);
8117   for(i=0; i<10; i++)
8118     cprintf(" %p", pcs[i]);
8119   panicked = 1; // freeze other CPU
8120   for(;;)
8121     ;
8122 }
8123 
8124 
8125 
8126 
8127 
8128 
8129 
8130 
8131 
8132 
8133 
8134 
8135 
8136 
8137 
8138 
8139 
8140 
8141 
8142 
8143 
8144 
8145 
8146 
8147 
8148 
8149 
8150 #define BACKSPACE 0x100
8151 #define CRTPORT 0x3d4
8152 static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory
8153 
8154 static void
8155 cgaputc(int c)
8156 {
8157   int pos;
8158 
8159   // Cursor position: col + 80*row.
8160   outb(CRTPORT, 14);
8161   pos = inb(CRTPORT+1) << 8;
8162   outb(CRTPORT, 15);
8163   pos |= inb(CRTPORT+1);
8164 
8165   if(c == '\n')
8166     pos += 80 - pos%80;
8167   else if(c == BACKSPACE){
8168     if(pos > 0) --pos;
8169   } else
8170     crt[pos++] = (c&0xff) | 0x0700;  // black on white
8171 
8172   if(pos < 0 || pos > 25*80)
8173     panic("pos under/overflow");
8174 
8175   if((pos/80) >= 24){  // Scroll up.
8176     memmove(crt, crt+80, sizeof(crt[0])*23*80);
8177     pos -= 80;
8178     memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
8179   }
8180 
8181   outb(CRTPORT, 14);
8182   outb(CRTPORT+1, pos>>8);
8183   outb(CRTPORT, 15);
8184   outb(CRTPORT+1, pos);
8185   crt[pos] = ' ' | 0x0700;
8186 }
8187 
8188 
8189 
8190 
8191 
8192 
8193 
8194 
8195 
8196 
8197 
8198 
8199 
8200 void
8201 consputc(int c)
8202 {
8203   if(panicked){
8204     cli();
8205     for(;;)
8206       ;
8207   }
8208 
8209   if(c == BACKSPACE){
8210     uartputc('\b'); uartputc(' '); uartputc('\b');
8211   } else
8212     uartputc(c);
8213   cgaputc(c);
8214 }
8215 
8216 #define INPUT_BUF 128
8217 struct {
8218   char buf[INPUT_BUF];
8219   uint r;  // Read index
8220   uint w;  // Write index
8221   uint e;  // Edit index
8222 } input;
8223 
8224 #define C(x)  ((x)-'@')  // Control-x
8225 
8226 void
8227 consoleintr(int (*getc)(void))
8228 {
8229   int c, doprocdump = 0;
8230 
8231   acquire(&cons.lock);
8232   while((c = getc()) >= 0){
8233     switch(c){
8234     case C('P'):  // Process listing.
8235       // procdump() locks cons.lock indirectly; invoke later
8236       doprocdump = 1;
8237       break;
8238     case C('U'):  // Kill line.
8239       while(input.e != input.w &&
8240             input.buf[(input.e-1) % INPUT_BUF] != '\n'){
8241         input.e--;
8242         consputc(BACKSPACE);
8243       }
8244       break;
8245     case C('H'): case '\x7f':  // Backspace
8246       if(input.e != input.w){
8247         input.e--;
8248         consputc(BACKSPACE);
8249       }
8250       break;
8251     default:
8252       if(c != 0 && input.e-input.r < INPUT_BUF){
8253         c = (c == '\r') ? '\n' : c;
8254         input.buf[input.e++ % INPUT_BUF] = c;
8255         consputc(c);
8256         if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
8257           input.w = input.e;
8258           wakeup(&input.r);
8259         }
8260       }
8261       break;
8262     }
8263   }
8264   release(&cons.lock);
8265   if(doprocdump) {
8266     procdump();  // now call procdump() wo. cons.lock held
8267   }
8268 }
8269 
8270 int
8271 consoleread(struct inode *ip, char *dst, int n)
8272 {
8273   uint target;
8274   int c;
8275 
8276   iunlock(ip);
8277   target = n;
8278   acquire(&cons.lock);
8279   while(n > 0){
8280     while(input.r == input.w){
8281       if(myproc()->killed){
8282         release(&cons.lock);
8283         ilock(ip);
8284         return -1;
8285       }
8286       sleep(&input.r, &cons.lock);
8287     }
8288     c = input.buf[input.r++ % INPUT_BUF];
8289     if(c == C('D')){  // EOF
8290       if(n < target){
8291         // Save ^D for next time, to make sure
8292         // caller gets a 0-byte result.
8293         input.r--;
8294       }
8295       break;
8296     }
8297     *dst++ = c;
8298     --n;
8299     if(c == '\n')
8300       break;
8301   }
8302   release(&cons.lock);
8303   ilock(ip);
8304 
8305   return target - n;
8306 }
8307 
8308 int
8309 consolewrite(struct inode *ip, char *buf, int n)
8310 {
8311   int i;
8312 
8313   iunlock(ip);
8314   acquire(&cons.lock);
8315   for(i = 0; i < n; i++)
8316     consputc(buf[i] & 0xff);
8317   release(&cons.lock);
8318   ilock(ip);
8319 
8320   return n;
8321 }
8322 
8323 void
8324 consoleinit(void)
8325 {
8326   initlock(&cons.lock, "console");
8327 
8328   devsw[CONSOLE].write = consolewrite;
8329   devsw[CONSOLE].read = consoleread;
8330   cons.locking = 1;
8331 
8332   ioapicenable(IRQ_KBD, 0);
8333 }
8334 
8335 
8336 
8337 
8338 
8339 
8340 
8341 
8342 
8343 
8344 
8345 
8346 
8347 
8348 
8349 
