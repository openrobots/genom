/*	$LAAS$ */

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
#ifndef POSTER_XML_LIB_H
#define POSTER_XML_LIB_H

typedef STATUS (* POSTER_XML_FUNC)(FILE *);

typedef struct GENOM_POSTER_XML {
  char *posterName;
  char *posterData;
  POSTER_XML_FUNC posterXMLFunc;
} GENOM_POSTER_XML;

#define POSTER_XML_CMDS {"get", "data"}


extern int getPosterXML(FILE *stream, int argc, char **argv, char **argn, 
			int nbPosterData,
			GENOM_POSTER_XML tabPosterXML[]);

extern int posterListXML(FILE *f, int nbPoster, GENOM_POSTER_XML posterXML[]);

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"


#define BALISE_HEAD "<"
#define BALISE_END ">"
#define END_BALISE "</"

#define BEGIN_BALISE 0
#define BEGIN_BALISE_NEWLINE 1
#define TERMINATE_BALISE 2


extern int xmlBalise(char *balise,int beginEnd,FILE *stream,int level);
extern int xmlHeader(FILE *stream);

#endif /* POSTER_XML_LIB_H */

