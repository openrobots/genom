/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
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

#define MAX_POSTERS 16		/* Verifier coherence avec modules.h */

#define STR_ALLOC(s) ((s *)xalloc(sizeof(s)))


/***
 *** De'finitions de la representation interne d'un module pour genom
 ***/


/* Description d'un identificateur */
typedef char ID_STR;

/* Liste d'identificateurs */
typedef struct id_list {
    ID_STR *name;
    ID_STR *NAME;
    struct id_list *next;
} ID_LIST;

/* Description d'un nom déclaré (partie droite de la déclaration) */
typedef struct {
    char *name;
    struct type_str *type;
    int pointeur;      /* Compte les "*" */
    int *dimensions;
    int ndimensions;
  /*    int max_str_len;   */
    int flags;         /* ENUM_VALUE, ENUM_NO_VALUE, ARRAY, STRING */
} DCL_NOM_STR;

/* Liste de noms de'clare's */
typedef struct dcl_nom_list {
    DCL_NOM_STR *dcl_nom;
    struct dcl_nom_list *next;
} DCL_NOM_LIST;

/* Descripteur de type (partie gauche de la déclaration) */
typedef struct type_str {
    int type;
    ID_STR *name;
    DCL_NOM_LIST *members;
    int used;         /* Indique si utilise */
    int linenum;
    char *filename;
    int flags;         /* Voir ci-dessous */
} TYPE_STR;

typedef struct LIBTOOL_VERSION {
    int current, revision, age;
} LIBTOOL_VERSION;

/* Flags des types */
#define    TYPE_INTERNAL_DATA 0x01 /* Ce type est celui de la SDI */
#define    TYPE_POSTER        0x02 /* Ce type est celui d'un poster */
#define    TYPE_IMPORT        0x04 /* Ce type est importe' d'un autre module */
#define    ENUM_NO_VALUE      0x08 /* element d'une enum sans valeur */
#define    ENUM_VALUE         0x10 /* element d'une enume avec valeur */
#define    SHORT_INT          0x20 /* type 'short' */
#define    LONG_INT           0x40 /* type 'long' */
#define    UNSIGNED_TYPE      0x80 /* type 'unsigned' */

#define    TYPE_GENOM         0x100 /* type defini dans le .gen */


#define    ARRAY              0x200 /* les tableaux (de type ci-dessus) */
#define    STRING             0x400 /* tableau de char */
#define    POINTER            0x600

/* POINTEUR */
#define IS_POINTER(n) ((n)->pointeur==0 ? 0==1:1==1)

/* STRING ou tableau de STRING 
   (actuellement tout tableau de char est une STRING,
   mais ce serait plus propre (et plus souple) de definir 
   explicitement un type string). */ 
#define IS_STRING(n)   (!((n)->type->flags & UNSIGNED_TYPE) && \
                        (n)->type->type==CHAR && (n)->ndimensions!=0 && \
			!IS_POINTER(n) ? 1==1:0==1)
/* #define IS_STRING(n)   ((n)->type->type==CHAR && (n)->ndimensions==1 && \ */
/* 			!IS_POINTER(n) ? 1==1:0==1) */

/* Tableau (sauf simple STRING) */
#define IS_ARRAY(n)    ((n)->ndimensions!=0 && \
		       ((n)->ndimensions!=1 || !IS_STRING(n)) ? 1==1:0==1)

/* Liste de types */
typedef struct type_list {
    TYPE_STR *type;
    struct type_list *next;
} TYPE_LIST;

/* Liste de requetes */
typedef struct rqst_list {
    ID_STR *name;
    int flags;
    struct rqst_str *rqst;
    struct rqst_list *next;
} RQST_LIST;

/* Refe'rence a un membre de structure */
typedef struct {
    DCL_NOM_STR *dcl_nom;
    ID_LIST *sdi_ref;
} STR_REF_STR;

typedef struct str_ref_list {
    STR_REF_STR *str_ref;
    struct str_ref_list *next;
} STR_REF_LIST;


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
    
/* Description d'une ta^che d'exe'cution */
typedef struct exec_task_str {
    ID_STR *name;
    ID_STR *NAME;
    int num;
    int period;
    int delay;
    int priority;
    int stack_size;
    ID_STR *c_init_func;
    ID_STR *c_end_func;
    ID_STR *c_func;
    TYPE_LIST *posters_input_types;
    ID_LIST *cs_client_from;
    ID_LIST *poster_client_from;
    ID_LIST *resource_list;
    ID_LIST *fail_msg;
} EXEC_TASK_STR;

typedef struct {
    int attribut;
    union {
	int period;
	int delay;
	int priority;
	int stack_size;
	ID_STR *c_init_func;
	ID_STR *c_end_func;
        ID_STR *c_func;
        TYPE_LIST *posters_input_types;
	ID_STR *poster_name;
	STR_REF_LIST *poster_data;
	ID_LIST *cs_client_from;
	ID_LIST *poster_client_from;
	ID_LIST *resource_list;
	ID_LIST *fail_msg;
    } value;
} EXEC_TASK_AV_STR;

/* Liste de taches d'exection */
typedef struct exec_task_list {
    EXEC_TASK_STR *exec_task;
    struct exec_task_list *next;
} EXEC_TASK_LIST;

/* Description d'une requete */
typedef struct rqst_str {
    ID_STR *name;
    ID_STR *NAME;
    int type;
    int num;
    STR_REF_STR *input;
    TYPE_LIST *posters_input_types;
    STR_REF_STR *output;
    ID_STR *c_control_func;
    ID_STR *c_exec_func;
    ID_STR *c_exec_func_start;
    ID_STR *c_exec_func_end;
    ID_STR *c_exec_func_fail;
    ID_STR *c_exec_func_inter;
    RQST_LIST *incompatible_with;
    ID_STR *exec_task_name;
    EXEC_TASK_STR *exec_task;
    ID_LIST *fail_msg;
    ID_LIST *resource_list;
    int activity;
    int rqst_num;
    ID_STR *doc;
    RQST_INPUT_INFO_LIST *input_info; 
} RQST_STR;

typedef struct {
    int attribut;
    union {
	int type;
	STR_REF_STR *input;
        TYPE_LIST *posters_input_types;
	STR_REF_STR *output;
	ID_STR *c_control_func;
	ID_STR *c_exec_func;
	ID_STR *c_exec_func_start;
	ID_STR *c_exec_func_end;
	ID_STR *c_exec_func_fail;
	ID_STR *c_exec_func_inter;
	RQST_LIST *incompatible_with;
	ID_STR *exec_task_name;
	ID_LIST *fail_msg;
	ID_LIST *resource_list;
	int activity;
	int rqst_num;
        ID_STR *doc;
	RQST_INPUT_INFO_LIST *input_info; 
    } value;
} RQST_AV_STR;

/* Description d'un module */
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
    int use_cxx;
} MODULE_STR;


typedef struct {
    int attribut;
    union {
	TYPE_STR *internal_data;
	int number;
	ID_LIST *codel_files;
        ID_STR *email;
        ID_STR *version;
        LIBTOOL_VERSION *iface_version;
        int use_cxx;
    } value;
} MODULE_AV_STR;

/* Description d'un poster */
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
    ID_STR *create_func;
    struct POSTER_LIST *next;
} POSTER_LIST;

extern const char *protoDir;

/* Description d'un poster attendu en entree */
typedef struct POSTERS_INPUT_LIST {
    ID_STR  *name;
    ID_STR *NAME;
    TYPE_LIST *types;
    TYPE_STR *type;
    STR_REF_LIST *data;
    struct POSTERS_INPUT_LIST *next;
} POSTERS_INPUT_LIST;

extern void genom_get_requires(char* filename, char* cppOptions[]);
extern ID_LIST* push_back(ID_LIST* list, ID_LIST* item);

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
extern ID_LIST *requires;    /* packages listed in -P and require statements */
extern ID_LIST *externPath;         /* all the path from -I */
extern POSTER_LIST *posters;
extern POSTERS_INPUT_LIST *posters_input;
extern int initRequest;

extern char *cppOptions[];
extern int nCppOptions;

extern const char genomVersion[];
extern const char genomMajor[];
extern const char genomMinor[];

extern int verbose;

#include "genomProto.h"

#endif
