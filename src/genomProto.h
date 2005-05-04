/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
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
#ifndef H_GENOMPROTO
#define H_GENOMPROTO

extern MODULE_STR * ajout_av_module ( MODULE_AV_STR *av, MODULE_STR *module );
extern RQST_STR * ajout_av_requete ( RQST_AV_STR *av, RQST_STR *rqst );
extern EXEC_TASK_STR * ajout_av_tache ( EXEC_TASK_AV_STR *av, EXEC_TASK_STR *task );
extern void ajout_type ( TYPE_STR *t );
extern void bufcat ( char **buf, char *fmt, ... );
extern int bufcatIfNotIn ( char **buf, char *fmt, ... );
extern void cat_begin ( FILE *out );
extern void cat_end ( FILE *out );
extern void construitIncludeList ( void );
extern TYPE_STR * copy_type ( TYPE_STR *t );
extern void dcl_nom_decl ( DCL_NOM_STR *n, char **pType, char **pVar );
extern char * genSizeof ( DCL_NOM_STR *n );
extern void markUsed ( TYPE_STR *t );
extern char * new_name ( void );
extern char * nom_type ( TYPE_STR *type );
extern char * nom_type1 ( TYPE_STR *type );
extern void print_sed_subst(FILE *f, const char *from, const char *fmt, ...);
extern int reentrant ( RQST_STR *r );
extern void resolveRequests ( void );
extern void resolveTasks ( void );
extern void resolveTypes ( void );
extern void script_close ( FILE *out, const char *newName, ... );
extern void script_do ( FILE *out, const char *dir, const char *doFileName );
extern void script_open ( FILE *out );
extern void script_close_exec(FILE *out, const char *newName,...);
extern void subst_begin ( FILE *out, const char *protoName );
extern void subst_end ( FILE *out );
extern int taille_obj ( DCL_NOM_STR *t );
extern int trouve_abort_num ( void );
extern EXEC_TASK_STR * trouve_exec_task ( char *name );
extern int trouve_members ( TYPE_STR *t );
extern RQST_STR * trouve_requete ( char *name );
extern POSTERS_INPUT_LIST * trouve_poster_input(char *name);
extern DCL_NOM_STR * trouve_sdi_ref ( STR_REF_STR *ref );
extern TYPE_STR * trouve_type ( TYPE_STR *t );
extern DCL_NOM_STR * trouve_typedef ( TYPE_STR *t );
extern int trouve_value ( char *symbole, int *pVal );
extern void upCaseNames ( void );
extern void upCaseArguments ( void );
extern void * xalloc ( size_t n );
extern void * xrealloc ( void *p, size_t n );

#endif /* H_GENOMPROTO */
