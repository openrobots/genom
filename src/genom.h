
/* 
 * Copyright (c) 1993-2006,2009 LAAS/CNRS
 * Matthieu Herrb - Tue Jul 13 1993
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
#ifndef _GENOM_H
#define _GENOM_H

#include <sys/types.h>

#define MAX_POSTERS 16		/* Keep coherent with modules.h */

#define STR_ALLOC(s) ((s *)xalloc(sizeof(s)))


/***
 *** Definitions for the internal representation of a Genom module
 ***/


/* Identifier */
typedef char ID_STR;

/* List of identifiers */
typedef struct id_list {
    ID_STR *name;
    ID_STR *NAME;			/* ALL CAPS version of the name */
    struct id_list *next;
} ID_LIST;

/* Definition of a declared name (right side part of the declaration */
typedef struct {
    char *name;
    struct type_str *type;
    int pointeur;      /* counts the "*" */
    int *dimensions;
    int ndimensions;
  /*    int max_str_len;   */
    int flags;         /* ENUM_VALUE, ENUM_NO_VALUE, ARRAY, STRING */
} DCL_NOM_STR;

/* List of declared names */
typedef struct dcl_nom_list {
    DCL_NOM_STR *dcl_nom;
    struct dcl_nom_list *next;
} DCL_NOM_LIST;

/* Type descriptor (left side part of the declaration */
typedef struct type_str {
    int type;
    ID_STR *name;
    DCL_NOM_LIST *members;
    int used;         /* Flag indicating use */
    int linenum;
    char *filename;
    int flags;         /* See below */
} TYPE_STR;

/* Flags for data types */
#define    TYPE_INTERNAL_DATA 0x01 /* This is the type of the fids */
#define    TYPE_POSTER        0x02 /* This is the type of a poster */
#define    TYPE_IMPORT        0x04 /* imported from anoter module */
#define    ENUM_NO_VALUE      0x08 /* enum element without a declared value */
#define    ENUM_VALUE         0x10 /* enum element with a declared value */
#define    SHORT_INT          0x20 /* 'short' */
#define    LONG_INT           0x40 /* 'long' */
#define    UNSIGNED_TYPE      0x80 /* 'unsigned' */

#define    TYPE_GENOM         0x100 /* type defined in the .gen file */


#define    ARRAY              0x200 /* array (of a type declared above) */
#define    STRING             0x400 /* array of chars  */
#define    POINTER            0x600
#define    LONG_LONG_INT      0x800 /* long long */

/* POINTER */
#define IS_POINTER(n) ((n)->pointeur==0 ? 0==1:1==1)

/* STRING or array of  STRINGs
   Currently all arrays of chars are considered as strings.
   It would be better to have an explicit string type of some kind. */ 
#define IS_STRING(n)   (!((n)->type->flags & UNSIGNED_TYPE) && \
                        (n)->type->type==CHAR && (n)->ndimensions!=0 && \
			!IS_POINTER(n) ? 1==1:0==1)
/* #define IS_STRING(n)   ((n)->type->type==CHAR && (n)->ndimensions==1 && \ */
/* 			!IS_POINTER(n) ? 1==1:0==1) */

/* ARRAY (except strings) */
#define IS_ARRAY(n)    ((n)->ndimensions!=0 && \
		       ((n)->ndimensions!=1 || !IS_STRING(n)) ? 1==1:0==1)

/* List of types */
typedef struct type_list {
    TYPE_STR *type;
    struct type_list *next;
} TYPE_LIST;

/* List of requests */
typedef struct rqst_list {
    ID_STR *name;
    int flags;
    struct rqst_str *rqst;
    struct rqst_list *next;
} RQST_LIST;

/* Reference to a member of a structure */
typedef struct {
    DCL_NOM_STR *dcl_nom;
    ID_LIST *sdi_ref;
} STR_REF_STR;

/* List of reference to structure members */
typedef struct str_ref_list {
    STR_REF_STR *str_ref;
    struct str_ref_list *next;
} STR_REF_LIST;


/* List of "information" data about a variable */
typedef struct rqst_input_info_list {
    int type;   /* INT, DOUBLE ou CHAR */
    union { 
      int i_val;
      double d_val;
      ID_STR *str_val;
    } default_val;
    ID_STR *doc;
    struct rqst_input_info_list *next;
} RQST_INPUT_INFO_LIST;
    
/* Description of an execution task */
typedef struct exec_task_str {
    ID_STR *name;
    ID_STR *NAME;
    int num;
    int period;
    int delay;
    int priority;
    int stack_size;
    ID_STR *codel_task_start;
    ID_STR *codel_task_end;
    ID_STR *codel_task_main;
    ID_STR *codel_task_main2;
    ID_STR *codel_task_wait;
    TYPE_LIST *posters_input_types;
    ID_LIST *cs_client_from;
    ID_LIST *poster_client_from;
    ID_LIST *resource_list;
    ID_LIST *fail_reports;
} EXEC_TASK_STR;

/* List of attribute/values for an execution task */
typedef struct {
    int attribut;
    union {
	int period;
	int delay;
	int priority;
	int stack_size;
	ID_STR *codel_task_start;
	ID_STR *codel_task_end;
        ID_STR *codel_task_main;
        ID_STR *codel_task_main2;
	ID_STR *codel_task_wait;
        TYPE_LIST *posters_input_types;
	ID_STR *poster_name;
	STR_REF_LIST *poster_data;
	ID_LIST *cs_client_from;
	ID_LIST *poster_client_from;
	ID_LIST *resource_list;
	ID_LIST *fail_reports;
    } value;
} EXEC_TASK_AV_STR;

/* List of execution tasks */
typedef struct exec_task_list {
    EXEC_TASK_STR *exec_task;
    struct exec_task_list *next;
} EXEC_TASK_LIST;

/* Description of a request */
typedef struct rqst_str {
    ID_STR *name;
    ID_STR *NAME;
    int type;
    int num;
    STR_REF_STR *input;
    TYPE_LIST *posters_input_types;
    STR_REF_STR *output;
    ID_STR *codel_control;
    ID_STR *codel_main;
    ID_STR *codel_start;
    ID_STR *codel_end;
    ID_STR *codel_fail;
    ID_STR *codel_inter;
    RQST_LIST *interrupt_activity;
    ID_STR *exec_task_name;
    EXEC_TASK_STR *exec_task;
    ID_LIST *fail_reports;
    ID_LIST *resource_list;
    int activity;
    int rqst_num;
    ID_STR *doc;
    RQST_INPUT_INFO_LIST *input_info; 
} RQST_STR;

/* Attribute/value pair for a request */
typedef struct {
    int attribut;
    union {
	int type;
	STR_REF_STR *input;
        TYPE_LIST *posters_input_types;
	STR_REF_STR *output;
	ID_STR *codel_control;
	ID_STR *codel_main;
	ID_STR *codel_start;
	ID_STR *codel_end;
	ID_STR *codel_fail;
	ID_STR *codel_inter;
	RQST_LIST *interrupt_activity;
	ID_STR *exec_task_name;
	ID_LIST *fail_reports;
	ID_LIST *resource_list;
	int activity;
	int rqst_num;
        ID_STR *doc;
	RQST_INPUT_INFO_LIST *input_info; 
    } value;
} RQST_AV_STR;

/* Libtool version */
typedef struct LIBTOOL_VERSION {
    int current, revision, age;
} LIBTOOL_VERSION;

/* Module language */
typedef enum MODULE_LANG {
  MODULE_LANG_DEFAULT,
  MODULE_LANG_C,
  MODULE_LANG_CXX
} MODULE_LANG;

/* Description of a module */
typedef struct {
    ID_STR *name;
    ID_STR *NAME;
    EXEC_TASK_LIST *exec_tasks;
    RQST_LIST *requests;
    TYPE_STR *internal_data;
    int number;
    ID_LIST *codel_files;
    ID_STR *email;
    ID_STR *version;
    LIBTOOL_VERSION *iface_version;
    MODULE_LANG lang;
    int clkrate;
} MODULE_STR;


/* Attribute/value pairs for a module */
typedef struct {
    int attribut;
    union {
	TYPE_STR *internal_data;
	int number;
	ID_LIST *codel_files;
        ID_STR *email;
        ID_STR *version;
        LIBTOOL_VERSION *iface_version; /* interface versionning */
        MODULE_LANG lang;
        int clkrate;
    } value;
} MODULE_AV_STR;

/* Description of a poster */
typedef struct POSTER_LIST {
    ID_STR  *name;
    ID_STR *NAME;
    int update;
    EXEC_TASK_STR *exec_task;
    STR_REF_LIST *data;
    DCL_NOM_LIST *types;
    TYPE_STR *type;
    void *address;
    char *bus_space;
    ID_STR *codel_poster_create;
    struct POSTER_LIST *next;
} POSTER_LIST;


/* List of references to posters used as input */
typedef struct POSTERS_INPUT_LIST {
    ID_STR  *name;
    ID_STR *NAME;
    TYPE_LIST *types;
    TYPE_STR *type;
    STR_REF_LIST *data;
    struct POSTERS_INPUT_LIST *next;
} POSTERS_INPUT_LIST;

/*----------------------------------------------------------------------*/
/* Prototypes of  external functions */
extern void genom_get_requires(char* filename, char* cppOptions[]);
extern ID_LIST* push_back(ID_LIST* list, ID_LIST* item);
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
extern void copy_script(FILE *, const char *);
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
extern int trouve_value ( char *symbole, long *pVal );
extern void upCaseNames ( void );
extern void upCaseArguments ( void );
extern void * xalloc ( size_t n );
extern void * xrealloc ( void *p, size_t n );
extern const char *	langFileExt(MODULE_LANG lang);

/*----------------------------------------------------------------------*/

/* Global variables */
extern int keyword;	
extern int num_ligne;
extern char nomfic[];
extern char *genfile;

extern MODULE_STR *module;
extern DCL_NOM_LIST *typedefs;
extern TYPE_LIST *types;
extern RQST_LIST *requetes;
extern int nbRequest, maxRqstSize, maxReplySize;
extern EXEC_TASK_LIST *taches;
extern int nbExecTask;
extern ID_LIST *includeFiles;
extern ID_LIST *allIncludeFiles;
extern ID_LIST *imports;            /* modules listed in import from */
extern ID_LIST *requires;    /* packages listed in -P and requires statements */
extern ID_LIST *codels_requires;    /* packages listed in codels_requires statements */
extern ID_LIST *externPath;         /* all the path from -I */
extern POSTER_LIST *posters;
extern POSTERS_INPUT_LIST *posters_input;
extern int initRequest;

extern const char *protoDir;
extern char *cppOptions[];
extern int nCppOptions;

extern const char genomVersion[];

extern int verbose;

#endif
