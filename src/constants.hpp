#pragma once

#define PAGESIZE 8192



#define SEGMENT_BITS 16
#define PAGE_BITS 40
#define SLOT_BITS 8

#define SEGMENT_MASK ((2L << SEGMENT_BITS) - 1) << (PAGE_BITS + SLOT_BITS)
#define PAGE_MASK ((2L << PAGE_BITS) - 1) << SLOT_BITS;
#define SLOT_MASK ((2L << SLOT_BITS) - 1);
#define SEGMENTPAGE_MASK SEGMENT_MASK | PAGE_MASK

#define NUMBER_OF_SEGMENTS 1L << SEGMENT_BITS
#define PAGES_PER_SEGMENT 1L << PAGE_BITS
#define SLOTS_PER_PAGE  1L << SLOT_BITS

/*

TID:

     SEGMENT                        PAGE                        SLOT
|---------------|--------------------------------------------|--------|
0-------8-------16-------24-------32-------40-------48-------56-------64

*/
