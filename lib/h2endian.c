/*	$LAAS$ */

/* 
 * Copyright (c) 2003-2004 LAAS/CNRS
 * Sara Fleury, David Bonnafous -  Thu Jan 9 2003
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include "genom-config.h"
__RCSID("$LAAS$");

#ifdef VXWORKS
#  include <vxWorks.h>
#endif

#include <stdio.h>

#include "genom/printScan.h"

#include "h2endianness.h"
#include "genom/h2endian.h"

/***
 *** SWAP big endian <-> little endian
 ***/


void endianswap_char (char *thechar, int nDim, int *dims)
{}

void endianswap_string (char *thechar, int nDim, int *dims)
{}

void endianswap_unsigned_char (unsigned char *x, int nDim, int *dims)
{}

void endianswap_double (double *x, int nDim, int *dims)
{
#if 1
  endianswap_longlong((long long *)x, nDim, dims);
#else
  FOR_EACH_elt(nDim,dims)
    endianswap_buffer((char *)(x+elt), sizeof(double));
  END_FOR
#endif
}

void endianswap_longlong(long long *thelonglong, int nDim, int *dims)
{
  FOR_EACH_elt(nDim,dims)
    endianswap_longlong_elt(thelonglong+elt);
  END_FOR
}

void endianswap_longlong_elt(long long *thelonglong)
{
  register unsigned char *c;

  c = (unsigned char *)thelonglong;

#ifdef LITTLE_ENDIAN
  /* big -> little */
  *thelonglong = ((long long int)c[0] << 56 | (long long int)c[1] << 48 |
		  (long long int)c[2] << 40 | (long long int)c[3] << 32 |
		  (long long int)c[4] << 24 | (long long int)c[5] << 16 |
		  (long long int)c[6] << 8 | (long long int)c[7]);
#else
  *thelonglong = ((long long int)c[7] << 56 | (long long int)c[6] << 48 |
		  (long long int)c[5] << 40 | (long long int)c[4] << 32 |
		  (long long int)c[3] << 24 | (long long int)c[2] << 16 |
		  (long long int)c[1] << 8  | (long long int)c[0]);
#endif
}

void endianswap_long_long_int(long long *thelonglong, int nDim, int *dims)
{
	endianswap_longlong(thelonglong, nDim, dims);
}

void endianswap_unsigned_long_long_int(long long *thelonglong, int nDim, int *dims)
{
	endianswap_longlong(thelonglong, nDim, dims);
}

void endianswap_short_int(short int *theshort, int nDim, int *dims)
{
  FOR_EACH_elt(nDim,dims)
    endianswap_short_int_elt(theshort+elt);
  END_FOR
}

void endianswap_short_int_elt(short int *theshort)
{
  register unsigned char *c;
  short int res;

  c = (unsigned char *)theshort;

#ifdef LITTLE_ENDIAN
  /* big -> little */
  res = c[0] << 8 | c[1];
#else
  res = c[1] << 8 | c[0];
#endif
  *theshort = res;
}

void endianswap_unsigned_short_int(unsigned short int *theshort, 
			       int nDim, int *dims)
{
  endianswap_short_int((short int *)theshort, nDim, dims);
}

void endianswap_unsigned_int (unsigned int *theint, int nDim, int *dims)
{
  endianswap_int ((int *)theint, nDim, dims);
}

void endianswap_int (int *theint, int nDim, int *dims)
{
  FOR_EACH_elt(nDim,dims)
    endianswap_int_elt (theint+elt);
  END_FOR
}

void endianswap_int_elt (int *theint)
{
  register unsigned char *c;
  int res;

  c = (unsigned char *)theint;

#ifdef LITTLE_ENDIAN
  /* big -> little */
  res = c[0] << 24 | c[1] << 16 | c[2] << 8 | c[3];
#else
  /* little -> big */
  res = c[3] << 24 | c[2] << 16 | c[1] << 8 | c[0];
#endif
  *theint = res;
}

void endianswap_long_int (long int *theint, int nDim, int *dims)
{
  FOR_EACH_elt(nDim,dims)
    endianswap_long_int_elt (theint+elt);
  END_FOR
}

void endianswap_long_int_elt (long int *theint)
{
  register unsigned char *c;
  int res;

  c = (unsigned char *)theint;

#ifdef LITTLE_ENDIAN
  res = c[0] << 24 | c[1] << 16 | c[2] << 8 | c[3];
#else
  res = c[3] << 24 | c[2] << 16 | c[1] << 8 | c[0];
#endif
  *theint = res;
}

void endianswap_unsigned_long_int (unsigned long int *theint, int nDim, int *dims)
{
  endianswap_long_int ((long int *)theint, nDim, dims);
}

void endianswap_float(float *thefloat, int nDim, int *dims)
{
  endianswap_int((int *)thefloat, nDim, dims);
}


void endianswap_buffer(char *buf, unsigned char bufsize)
{
  register unsigned char tampon;
  register unsigned char *bs;		/* buffer start */
  register unsigned char *be;		/* buffer end */

  bs = (unsigned char *)&buf[0];
  be = (unsigned char *)&buf[bufsize-1];
  while ( bs < be ) {
    tampon = *be;
    *be = *bs;
    *bs = tampon;

    bs++, be--;
  }
}
