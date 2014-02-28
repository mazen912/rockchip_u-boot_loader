/*
 * This file is derived from crc32.c from the zlib-1.1.3 distribution
 * by Jean-loup Gailly and Mark Adler.
 */

/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef USE_HOSTCC
#include <common.h>
#endif
#include <compiler.h>
#include <u-boot/crc.h>

#if defined(CONFIG_HW_WATCHDOG) || defined(CONFIG_WATCHDOG)
#include <watchdog.h>
#endif
#include "u-boot/zlib.h"

#define local static
#define ZEXPORT	/* empty */

#define tole(x) cpu_to_le32(x)

#ifdef DYNAMIC_CRC_TABLE

local int crc_table_empty = 1;
local uint32_t crc_table[256];
local void make_crc_table OF((void));

/*
  Generate a table for a byte-wise 32-bit CRC calculation on the polynomial:
  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.

  Polynomials over GF(2) are represented in binary, one bit per coefficient,
  with the lowest powers in the most significant bit.  Then adding polynomials
  is just exclusive-or, and multiplying a polynomial by x is a right shift by
  one.  If we call the above polynomial p, and represent a byte as the
  polynomial q, also with the lowest power in the most significant bit (so the
  byte 0xb1 is the polynomial x^7+x^3+x+1), then the CRC is (q*x^32) mod p,
  where a mod b means the remainder after dividing a by b.

  This calculation is done using the shift-register method of multiplying and
  taking the remainder.  The register is initialized to zero, and for each
  incoming bit, x^32 is added mod p to the register if the bit is a one (where
  x^32 mod p is p+x^32 = x^26+...+1), and the register is multiplied mod p by
  x (which is shifting right by one and adding x^32 mod p if the bit shifted
  out is a one).  We start with the highest power (least significant bit) of
  q and repeat for all eight bits of q.

  The table is simply the CRC of all possible eight bit values.  This is all
  the information needed to generate CRC's on data a byte at a time for all
  combinations of CRC register values and incoming bytes.
*/
local void make_crc_table()
{
  uint32_t c;
  int n, k;
  uLong poly;		/* polynomial exclusive-or pattern */
  /* terms of polynomial defining this crc (except x^32): */
  static const Byte p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

  /* make exclusive-or pattern from polynomial (0xedb88320L) */
  poly = 0L;
  for (n = 0; n < sizeof(p)/sizeof(Byte); n++)
    poly |= 1L << (31 - p[n]);

  for (n = 0; n < 256; n++)
  {
    c = (uLong)n;
    for (k = 0; k < 8; k++)
      c = c & 1 ? poly ^ (c >> 1) : c >> 1;
    crc_table[n] = tole(c);
  }
  crc_table_empty = 0;
}
#else
/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */

#ifndef CONFIG_ROCKCHIP
local const uint32_t crc_table[256] = {
tole(0x00000000L), tole(0x77073096L), tole(0xee0e612cL), tole(0x990951baL),
tole(0x076dc419L), tole(0x706af48fL), tole(0xe963a535L), tole(0x9e6495a3L),
tole(0x0edb8832L), tole(0x79dcb8a4L), tole(0xe0d5e91eL), tole(0x97d2d988L),
tole(0x09b64c2bL), tole(0x7eb17cbdL), tole(0xe7b82d07L), tole(0x90bf1d91L),
tole(0x1db71064L), tole(0x6ab020f2L), tole(0xf3b97148L), tole(0x84be41deL),
tole(0x1adad47dL), tole(0x6ddde4ebL), tole(0xf4d4b551L), tole(0x83d385c7L),
tole(0x136c9856L), tole(0x646ba8c0L), tole(0xfd62f97aL), tole(0x8a65c9ecL),
tole(0x14015c4fL), tole(0x63066cd9L), tole(0xfa0f3d63L), tole(0x8d080df5L),
tole(0x3b6e20c8L), tole(0x4c69105eL), tole(0xd56041e4L), tole(0xa2677172L),
tole(0x3c03e4d1L), tole(0x4b04d447L), tole(0xd20d85fdL), tole(0xa50ab56bL),
tole(0x35b5a8faL), tole(0x42b2986cL), tole(0xdbbbc9d6L), tole(0xacbcf940L),
tole(0x32d86ce3L), tole(0x45df5c75L), tole(0xdcd60dcfL), tole(0xabd13d59L),
tole(0x26d930acL), tole(0x51de003aL), tole(0xc8d75180L), tole(0xbfd06116L),
tole(0x21b4f4b5L), tole(0x56b3c423L), tole(0xcfba9599L), tole(0xb8bda50fL),
tole(0x2802b89eL), tole(0x5f058808L), tole(0xc60cd9b2L), tole(0xb10be924L),
tole(0x2f6f7c87L), tole(0x58684c11L), tole(0xc1611dabL), tole(0xb6662d3dL),
tole(0x76dc4190L), tole(0x01db7106L), tole(0x98d220bcL), tole(0xefd5102aL),
tole(0x71b18589L), tole(0x06b6b51fL), tole(0x9fbfe4a5L), tole(0xe8b8d433L),
tole(0x7807c9a2L), tole(0x0f00f934L), tole(0x9609a88eL), tole(0xe10e9818L),
tole(0x7f6a0dbbL), tole(0x086d3d2dL), tole(0x91646c97L), tole(0xe6635c01L),
tole(0x6b6b51f4L), tole(0x1c6c6162L), tole(0x856530d8L), tole(0xf262004eL),
tole(0x6c0695edL), tole(0x1b01a57bL), tole(0x8208f4c1L), tole(0xf50fc457L),
tole(0x65b0d9c6L), tole(0x12b7e950L), tole(0x8bbeb8eaL), tole(0xfcb9887cL),
tole(0x62dd1ddfL), tole(0x15da2d49L), tole(0x8cd37cf3L), tole(0xfbd44c65L),
tole(0x4db26158L), tole(0x3ab551ceL), tole(0xa3bc0074L), tole(0xd4bb30e2L),
tole(0x4adfa541L), tole(0x3dd895d7L), tole(0xa4d1c46dL), tole(0xd3d6f4fbL),
tole(0x4369e96aL), tole(0x346ed9fcL), tole(0xad678846L), tole(0xda60b8d0L),
tole(0x44042d73L), tole(0x33031de5L), tole(0xaa0a4c5fL), tole(0xdd0d7cc9L),
tole(0x5005713cL), tole(0x270241aaL), tole(0xbe0b1010L), tole(0xc90c2086L),
tole(0x5768b525L), tole(0x206f85b3L), tole(0xb966d409L), tole(0xce61e49fL),
tole(0x5edef90eL), tole(0x29d9c998L), tole(0xb0d09822L), tole(0xc7d7a8b4L),
tole(0x59b33d17L), tole(0x2eb40d81L), tole(0xb7bd5c3bL), tole(0xc0ba6cadL),
tole(0xedb88320L), tole(0x9abfb3b6L), tole(0x03b6e20cL), tole(0x74b1d29aL),
tole(0xead54739L), tole(0x9dd277afL), tole(0x04db2615L), tole(0x73dc1683L),
tole(0xe3630b12L), tole(0x94643b84L), tole(0x0d6d6a3eL), tole(0x7a6a5aa8L),
tole(0xe40ecf0bL), tole(0x9309ff9dL), tole(0x0a00ae27L), tole(0x7d079eb1L),
tole(0xf00f9344L), tole(0x8708a3d2L), tole(0x1e01f268L), tole(0x6906c2feL),
tole(0xf762575dL), tole(0x806567cbL), tole(0x196c3671L), tole(0x6e6b06e7L),
tole(0xfed41b76L), tole(0x89d32be0L), tole(0x10da7a5aL), tole(0x67dd4accL),
tole(0xf9b9df6fL), tole(0x8ebeeff9L), tole(0x17b7be43L), tole(0x60b08ed5L),
tole(0xd6d6a3e8L), tole(0xa1d1937eL), tole(0x38d8c2c4L), tole(0x4fdff252L),
tole(0xd1bb67f1L), tole(0xa6bc5767L), tole(0x3fb506ddL), tole(0x48b2364bL),
tole(0xd80d2bdaL), tole(0xaf0a1b4cL), tole(0x36034af6L), tole(0x41047a60L),
tole(0xdf60efc3L), tole(0xa867df55L), tole(0x316e8eefL), tole(0x4669be79L),
tole(0xcb61b38cL), tole(0xbc66831aL), tole(0x256fd2a0L), tole(0x5268e236L),
tole(0xcc0c7795L), tole(0xbb0b4703L), tole(0x220216b9L), tole(0x5505262fL),
tole(0xc5ba3bbeL), tole(0xb2bd0b28L), tole(0x2bb45a92L), tole(0x5cb36a04L),
tole(0xc2d7ffa7L), tole(0xb5d0cf31L), tole(0x2cd99e8bL), tole(0x5bdeae1dL),
tole(0x9b64c2b0L), tole(0xec63f226L), tole(0x756aa39cL), tole(0x026d930aL),
tole(0x9c0906a9L), tole(0xeb0e363fL), tole(0x72076785L), tole(0x05005713L),
tole(0x95bf4a82L), tole(0xe2b87a14L), tole(0x7bb12baeL), tole(0x0cb61b38L),
tole(0x92d28e9bL), tole(0xe5d5be0dL), tole(0x7cdcefb7L), tole(0x0bdbdf21L),
tole(0x86d3d2d4L), tole(0xf1d4e242L), tole(0x68ddb3f8L), tole(0x1fda836eL),
tole(0x81be16cdL), tole(0xf6b9265bL), tole(0x6fb077e1L), tole(0x18b74777L),
tole(0x88085ae6L), tole(0xff0f6a70L), tole(0x66063bcaL), tole(0x11010b5cL),
tole(0x8f659effL), tole(0xf862ae69L), tole(0x616bffd3L), tole(0x166ccf45L),
tole(0xa00ae278L), tole(0xd70dd2eeL), tole(0x4e048354L), tole(0x3903b3c2L),
tole(0xa7672661L), tole(0xd06016f7L), tole(0x4969474dL), tole(0x3e6e77dbL),
tole(0xaed16a4aL), tole(0xd9d65adcL), tole(0x40df0b66L), tole(0x37d83bf0L),
tole(0xa9bcae53L), tole(0xdebb9ec5L), tole(0x47b2cf7fL), tole(0x30b5ffe9L),
tole(0xbdbdf21cL), tole(0xcabac28aL), tole(0x53b39330L), tole(0x24b4a3a6L),
tole(0xbad03605L), tole(0xcdd70693L), tole(0x54de5729L), tole(0x23d967bfL),
tole(0xb3667a2eL), tole(0xc4614ab8L), tole(0x5d681b02L), tole(0x2a6f2b94L),
tole(0xb40bbe37L), tole(0xc30c8ea1L), tole(0x5a05df1bL), tole(0x2d02ef8dL)
};
#else
local const uint32_t crc_table[256] = {
0x00000000,0x04c10db7,0x09821b6e,0x0d4316d9,
0x130436dc,0x17c53b6b,0x1a862db2,0x1e472005,
0x26086db8,0x22c9600f,0x2f8a76d6,0x2b4b7b61,
0x350c5b64,0x31cd56d3,0x3c8e400a,0x384f4dbd,
0x4c10db70,0x48d1d6c7,0x4592c01e,0x4153cda9,
0x5f14edac,0x5bd5e01b,0x5696f6c2,0x5257fb75,
0x6a18b6c8,0x6ed9bb7f,0x639aada6,0x675ba011,
0x791c8014,0x7ddd8da3,0x709e9b7a,0x745f96cd,
0x9821b6e0,0x9ce0bb57,0x91a3ad8e,0x9562a039,
0x8b25803c,0x8fe48d8b,0x82a79b52,0x866696e5,
0xbe29db58,0xbae8d6ef,0xb7abc036,0xb36acd81,
0xad2ded84,0xa9ece033,0xa4aff6ea,0xa06efb5d,
0xd4316d90,0xd0f06027,0xddb376fe,0xd9727b49,
0xc7355b4c,0xc3f456fb,0xceb74022,0xca764d95,
0xf2390028,0xf6f80d9f,0xfbbb1b46,0xff7a16f1,
0xe13d36f4,0xe5fc3b43,0xe8bf2d9a,0xec7e202d,
0x34826077,0x30436dc0,0x3d007b19,0x39c176ae,
0x278656ab,0x23475b1c,0x2e044dc5,0x2ac54072,
0x128a0dcf,0x164b0078,0x1b0816a1,0x1fc91b16,
0x018e3b13,0x054f36a4,0x080c207d,0x0ccd2dca,
0x7892bb07,0x7c53b6b0,0x7110a069,0x75d1adde,
0x6b968ddb,0x6f57806c,0x621496b5,0x66d59b02,
0x5e9ad6bf,0x5a5bdb08,0x5718cdd1,0x53d9c066,
0x4d9ee063,0x495fedd4,0x441cfb0d,0x40ddf6ba,
0xaca3d697,0xa862db20,0xa521cdf9,0xa1e0c04e,
0xbfa7e04b,0xbb66edfc,0xb625fb25,0xb2e4f692,
0x8aabbb2f,0x8e6ab698,0x8329a041,0x87e8adf6,
0x99af8df3,0x9d6e8044,0x902d969d,0x94ec9b2a,
0xe0b30de7,0xe4720050,0xe9311689,0xedf01b3e,
0xf3b73b3b,0xf776368c,0xfa352055,0xfef42de2,
0xc6bb605f,0xc27a6de8,0xcf397b31,0xcbf87686,
0xd5bf5683,0xd17e5b34,0xdc3d4ded,0xd8fc405a,
0x6904c0ee,0x6dc5cd59,0x6086db80,0x6447d637,
0x7a00f632,0x7ec1fb85,0x7382ed5c,0x7743e0eb,
0x4f0cad56,0x4bcda0e1,0x468eb638,0x424fbb8f,
0x5c089b8a,0x58c9963d,0x558a80e4,0x514b8d53,
0x25141b9e,0x21d51629,0x2c9600f0,0x28570d47,
0x36102d42,0x32d120f5,0x3f92362c,0x3b533b9b,
0x031c7626,0x07dd7b91,0x0a9e6d48,0x0e5f60ff,
0x101840fa,0x14d94d4d,0x199a5b94,0x1d5b5623,
0xf125760e,0xf5e47bb9,0xf8a76d60,0xfc6660d7,
0xe22140d2,0xe6e04d65,0xeba35bbc,0xef62560b,
0xd72d1bb6,0xd3ec1601,0xdeaf00d8,0xda6e0d6f,
0xc4292d6a,0xc0e820dd,0xcdab3604,0xc96a3bb3,
0xbd35ad7e,0xb9f4a0c9,0xb4b7b610,0xb076bba7,
0xae319ba2,0xaaf09615,0xa7b380cc,0xa3728d7b,
0x9b3dc0c6,0x9ffccd71,0x92bfdba8,0x967ed61f,
0x8839f61a,0x8cf8fbad,0x81bbed74,0x857ae0c3,
0x5d86a099,0x5947ad2e,0x5404bbf7,0x50c5b640,
0x4e829645,0x4a439bf2,0x47008d2b,0x43c1809c,
0x7b8ecd21,0x7f4fc096,0x720cd64f,0x76cddbf8,
0x688afbfd,0x6c4bf64a,0x6108e093,0x65c9ed24,
0x11967be9,0x1557765e,0x18146087,0x1cd56d30,
0x02924d35,0x06534082,0x0b10565b,0x0fd15bec,
0x379e1651,0x335f1be6,0x3e1c0d3f,0x3add0088,
0x249a208d,0x205b2d3a,0x2d183be3,0x29d93654,
0xc5a71679,0xc1661bce,0xcc250d17,0xc8e400a0,
0xd6a320a5,0xd2622d12,0xdf213bcb,0xdbe0367c,
0xe3af7bc1,0xe76e7676,0xea2d60af,0xeeec6d18,
0xf0ab4d1d,0xf46a40aa,0xf9295673,0xfde85bc4,
0x89b7cd09,0x8d76c0be,0x8035d667,0x84f4dbd0,
0x9ab3fbd5,0x9e72f662,0x9331e0bb,0x97f0ed0c,
0xafbfa0b1,0xab7ead06,0xa63dbbdf,0xa2fcb668,
0xbcbb966d,0xb87a9bda,0xb5398d03,0xb1f880b4
};
#endif
#endif

#if 0
/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
const uint32_t * ZEXPORT get_crc_table()
{
#ifdef DYNAMIC_CRC_TABLE
  if (crc_table_empty) make_crc_table();
#endif
  return (const uint32_t *)crc_table;
}
#endif

/* ========================================================================= */
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define DO_CRC(x) crc = tab[(crc ^ (x)) & 255] ^ (crc >> 8)
# else
#  define DO_CRC(x) crc = tab[((crc >> 24) ^ (x)) & 255] ^ (crc << 8)
# endif

/* ========================================================================= */

/* No ones complement version. JFFS2 (and other things ?)
 * don't use ones compliment in their CRC calculations.
 */
uint32_t ZEXPORT crc32_no_comp(uint32_t crc, const Bytef *buf, uInt len)
{
    const uint32_t *tab = crc_table;
    const uint32_t *b =(const uint32_t *)buf;
    size_t rem_len;
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = cpu_to_le32(crc);
    /* Align it */
    if (((long)b) & 3 && len) {
	 uint8_t *p = (uint8_t *)b;
	 do {
	      DO_CRC(*p++);
	 } while ((--len) && ((long)p)&3);
	 b = (uint32_t *)p;
    }

    rem_len = len & 3;
    len = len >> 2;
    for (--b; len; --len) {
	 /* load data 32 bits wide, xor data 32 bits wide. */
	 crc ^= *++b; /* use pre increment for speed */
	 DO_CRC(0);
	 DO_CRC(0);
	 DO_CRC(0);
	 DO_CRC(0);
    }
    len = rem_len;
    /* And the last few bytes */
    if (len) {
	 uint8_t *p = (uint8_t *)(b + 1) - 1;
	 do {
	      DO_CRC(*++p); /* use pre increment for speed */
	 } while (--len);
    }

    return le32_to_cpu(crc);
}
#undef DO_CRC

uint32_t ZEXPORT crc32 (uint32_t crc, const Bytef *p, uInt len)
{
#ifdef CONFIG_ROCKCHIP
#define DO_CRC(x) crc = tab[((crc >> 24) ^ (x)) & 255] ^ (crc << 8)
    crc = cpu_to_le32(crc);
    const uint32_t *tab = crc_table;
	 do {
	      DO_CRC(*p++);
	 } while (--len);
    return le32_to_cpu(crc);
#undef DO_CRC
#endif
     return crc32_no_comp(crc ^ 0xffffffffL, p, len) ^ 0xffffffffL;
}

/*
 * Calculate the crc32 checksum triggering the watchdog every 'chunk_sz' bytes
 * of input.
 */
uint32_t ZEXPORT crc32_wd (uint32_t crc,
			   const unsigned char *buf,
			   uInt len, uInt chunk_sz)
{
#if defined(CONFIG_HW_WATCHDOG) || defined(CONFIG_WATCHDOG)
	const unsigned char *end, *curr;
	int chunk;

	curr = buf;
	end = buf + len;
	while (curr < end) {
		chunk = end - curr;
		if (chunk > chunk_sz)
			chunk = chunk_sz;
		crc = crc32 (crc, curr, chunk);
		curr += chunk;
		WATCHDOG_RESET ();
	}
#else
	crc = crc32 (crc, buf, len);
#endif

	return crc;
}

void crc32_wd_buf(const unsigned char *input, unsigned int ilen,
		unsigned char *output, unsigned int chunk_sz)
{
	uint32_t crc;

	crc = crc32_wd(0, input, ilen, chunk_sz);
	memcpy(output, &crc, sizeof(crc));
}
