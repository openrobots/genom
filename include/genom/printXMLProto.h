
/* 
 * Copyright (c) 2003 LAAS/CNRS                       --  Fri Sep  5 2003
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
#ifndef H_PRINTXMLPROTO
#define H_PRINTXMLPROTO

extern int printXML_char ( FILE *out, char *name, char *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_unsigned_char ( FILE *out, char *name, unsigned char *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_short_int ( FILE *out, char *name, short int *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_int ( FILE *out, char *name, int *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_long_int ( FILE *out, char *name, long int *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_unsigned_short_int ( FILE *out, char *name, unsigned short *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_unsigned_int ( FILE *out, char *name, unsigned int *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_unsigned_long_int ( FILE *out, char *name, unsigned long *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_float ( FILE *out, char *name, float *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_double ( FILE *out, char *name, double *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_addr ( FILE *out, char *name, void **x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_string ( FILE *out, char *name, char *x, int indent, int nDim, int *dims, FILE *in );
extern int printXML_string_len ( FILE *out, char *name, char *x, int max_str_len, int indent, int nDim, int *dims, FILE *in );
extern int fprintfBuf(FILE *out, const char *fmt, ...);

#endif /* H_PRINTXMLPROTO */
