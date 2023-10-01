4450 // Buffer cache.
4451 //
4452 // The buffer cache is a linked list of buf structures holding
4453 // cached copies of disk block contents.  Caching disk blocks
4454 // in memory reduces the number of disk reads and also provides
4455 // a synchronization point for disk blocks used by multiple processes.
4456 //
4457 // Interface:
4458 // * To get a buffer for a particular disk block, call bread.
4459 // * After changing buffer data, call bwrite to write it to disk.
4460 // * When done with the buffer, call brelse.
4461 // * Do not use the buffer after calling brelse.
4462 // * Only one process at a time can use a buffer,
4463 //     so do not keep them longer than necessary.
4464 //
4465 // The implementation uses two state flags internally:
4466 // * B_VALID: the buffer data has been read from the disk.
4467 // * B_DIRTY: the buffer data has been modified
4468 //     and needs to be written to disk.
4469 
4470 #include "types.h"
4471 #include "defs.h"
4472 #include "param.h"
4473 #include "spinlock.h"
4474 #include "sleeplock.h"
4475 #include "fs.h"
4476 #include "buf.h"
4477 
4478 struct {
4479   struct spinlock lock;
4480   struct buf buf[NBUF];
4481 
4482   // Linked list of all buffers, through prev/next.
4483   // head.next is most recently used.
4484   struct buf head;
4485 } bcache;
4486 
4487 void
4488 binit(void)
4489 {
4490   struct buf *b;
4491 
4492   initlock(&bcache.lock, "bcache");
4493 
4494 
4495 
4496 
4497 
4498 
4499 
4500   // Create linked list of buffers
4501   bcache.head.prev = &bcache.head;
4502   bcache.head.next = &bcache.head;
4503   for(b = bcache.buf; b < bcache.buf+NBUF; b++){
4504     b->next = bcache.head.next;
4505     b->prev = &bcache.head;
4506     initsleeplock(&b->lock, "buffer");
4507     bcache.head.next->prev = b;
4508     bcache.head.next = b;
4509   }
4510 }
4511 
4512 // Look through buffer cache for block on device dev.
4513 // If not found, allocate a buffer.
4514 // In either case, return locked buffer.
4515 static struct buf*
4516 bget(uint dev, uint blockno)
4517 {
4518   struct buf *b;
4519 
4520   acquire(&bcache.lock);
4521 
4522   // Is the block already cached?
4523   for(b = bcache.head.next; b != &bcache.head; b = b->next){
4524     if(b->dev == dev && b->blockno == blockno){
4525       b->refcnt++;
4526       release(&bcache.lock);
4527       acquiresleep(&b->lock);
4528       return b;
4529     }
4530   }
4531 
4532   // Not cached; recycle an unused buffer.
4533   // Even if refcnt==0, B_DIRTY indicates a buffer is in use
4534   // because log.c has modified it but not yet committed it.
4535   for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
4536     if(b->refcnt == 0 && (b->flags & B_DIRTY) == 0) {
4537       b->dev = dev;
4538       b->blockno = blockno;
4539       b->flags = 0;
4540       b->refcnt = 1;
4541       release(&bcache.lock);
4542       acquiresleep(&b->lock);
4543       return b;
4544     }
4545   }
4546   panic("bget: no buffers");
4547 }
4548 
4549 
4550 // Return a locked buf with the contents of the indicated block.
4551 struct buf*
4552 bread(uint dev, uint blockno)
4553 {
4554   struct buf *b;
4555 
4556   b = bget(dev, blockno);
4557   if((b->flags & B_VALID) == 0) {
4558     iderw(b);
4559   }
4560   return b;
4561 }
4562 
4563 // Write b's contents to disk.  Must be locked.
4564 void
4565 bwrite(struct buf *b)
4566 {
4567   if(!holdingsleep(&b->lock))
4568     panic("bwrite");
4569   b->flags |= B_DIRTY;
4570   iderw(b);
4571 }
4572 
4573 // Release a locked buffer.
4574 // Move to the head of the MRU list.
4575 void
4576 brelse(struct buf *b)
4577 {
4578   if(!holdingsleep(&b->lock))
4579     panic("brelse");
4580 
4581   releasesleep(&b->lock);
4582 
4583   acquire(&bcache.lock);
4584   b->refcnt--;
4585   if (b->refcnt == 0) {
4586     // no one is waiting for it.
4587     b->next->prev = b->prev;
4588     b->prev->next = b->next;
4589     b->next = bcache.head.next;
4590     b->prev = &bcache.head;
4591     bcache.head.next->prev = b;
4592     bcache.head.next = b;
4593   }
4594 
4595   release(&bcache.lock);
4596 }
4597 
4598 
4599 
4600 // Blank page.
4601 
4602 
4603 
4604 
4605 
4606 
4607 
4608 
4609 
4610 
4611 
4612 
4613 
4614 
4615 
4616 
4617 
4618 
4619 
4620 
4621 
4622 
4623 
4624 
4625 
4626 
4627 
4628 
4629 
4630 
4631 
4632 
4633 
4634 
4635 
4636 
4637 
4638 
4639 
4640 
4641 
4642 
4643 
4644 
4645 
4646 
4647 
4648 
4649 
