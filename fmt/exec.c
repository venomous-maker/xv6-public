6650 #include "types.h"
6651 #include "param.h"
6652 #include "memlayout.h"
6653 #include "mmu.h"
6654 #include "proc.h"
6655 #include "defs.h"
6656 #include "x86.h"
6657 #include "elf.h"
6658 
6659 int
6660 exec(char *path, char **argv)
6661 {
6662   char *s, *last;
6663   int i, off;
6664   uint argc, sz, sp, ustack[3+MAXARG+1];
6665   struct elfhdr elf;
6666   struct inode *ip;
6667   struct proghdr ph;
6668   pde_t *pgdir, *oldpgdir;
6669   struct proc *curproc = myproc();
6670 
6671   begin_op();
6672 
6673   if((ip = namei(path)) == 0){
6674     end_op();
6675     cprintf("exec: fail\n");
6676     return -1;
6677   }
6678   ilock(ip);
6679   pgdir = 0;
6680 
6681   // Check ELF header
6682   if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
6683     goto bad;
6684   if(elf.magic != ELF_MAGIC)
6685     goto bad;
6686 
6687   if((pgdir = setupkvm()) == 0)
6688     goto bad;
6689 
6690   // Load program into memory.
6691   sz = 0;
6692   for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
6693     if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
6694       goto bad;
6695     if(ph.type != ELF_PROG_LOAD)
6696       continue;
6697     if(ph.memsz < ph.filesz)
6698       goto bad;
6699     if(ph.vaddr + ph.memsz < ph.vaddr)
6700       goto bad;
6701     if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
6702       goto bad;
6703     if(ph.vaddr % PGSIZE != 0)
6704       goto bad;
6705     if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
6706       goto bad;
6707   }
6708   iunlockput(ip);
6709   end_op();
6710   ip = 0;
6711 
6712   // Allocate two pages at the next page boundary.
6713   // Make the first inaccessible.  Use the second as the user stack.
6714   sz = PGROUNDUP(sz);
6715   if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
6716     goto bad;
6717   clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
6718   sp = sz;
6719 
6720   // Push argument strings, prepare rest of stack in ustack.
6721   for(argc = 0; argv[argc]; argc++) {
6722     if(argc >= MAXARG)
6723       goto bad;
6724     sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
6725     if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
6726       goto bad;
6727     ustack[3+argc] = sp;
6728   }
6729   ustack[3+argc] = 0;
6730 
6731   ustack[0] = 0xffffffff;  // fake return PC
6732   ustack[1] = argc;
6733   ustack[2] = sp - (argc+1)*4;  // argv pointer
6734 
6735   sp -= (3+argc+1) * 4;
6736   if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
6737     goto bad;
6738 
6739   // Save program name for debugging.
6740   for(last=s=path; *s; s++)
6741     if(*s == '/')
6742       last = s+1;
6743   safestrcpy(curproc->name, last, sizeof(curproc->name));
6744 
6745   // Commit to the user image.
6746   oldpgdir = curproc->pgdir;
6747   curproc->pgdir = pgdir;
6748   curproc->sz = sz;
6749   curproc->tf->eip = elf.entry;  // main
6750   curproc->tf->esp = sp;
6751   switchuvm(curproc);
6752   freevm(oldpgdir);
6753   return 0;
6754 
6755  bad:
6756   if(pgdir)
6757     freevm(pgdir);
6758   if(ip){
6759     iunlockput(ip);
6760     end_op();
6761   }
6762   return -1;
6763 }
6764 
6765 
6766 
6767 
6768 
6769 
6770 
6771 
6772 
6773 
6774 
6775 
6776 
6777 
6778 
6779 
6780 
6781 
6782 
6783 
6784 
6785 
6786 
6787 
6788 
6789 
6790 
6791 
6792 
6793 
6794 
6795 
6796 
6797 
6798 
6799 
