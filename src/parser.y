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

/***
 *** Ge'ne'rateur de modules h2
 ***
 *** Analyseur Syntaxique
 ***
 *** Matthieu Herrb & Sara Fleury -- Juillet 1993
 ***/

%{
#include "config.h"
__RCSID("$LAAS$");

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>

#include "genom.h"
#include "makeGen.h"
#include "userMakeGen.h"
#include "typeGen.h"
#include "errorGen.h"
#include "reportsGen.h"
#include "msgLibGen.h"
#include "printGen.h"
#include "endianGen.h"
#include "scanGen.h"
#include "scopeGen.h"
#include "posterLibGen.h"
#include "headerGen.h"
#include "cntrlTaskGen.h"
#include "execTaskGen.h"
#include "moduleInitGen.h"
#include "essayGen.h"
#include "initGen.h"

#include "propiceGen.h"

#include "tclGen.h"
#include "tclClientGen.h"

#include "userCntrlCodelsGen.h"
#include "userExecCodelsGen.h"
#include "userStartGen.h"

#include "spyServerLibGen.h"
#include "spyStructGenProto.h"
#include "spyDescriptionGenProto.h"

#include "configureGen.h"

/* compile time configuration */
const char *includeDir = INCLUDEDIR;
const char *libraryDir = LIBRARYDIR;
const char *protoDir = CANVASDIR;

extern int yylex(void);

#define YYDEBUG 1
#define YYERROR_VERBOSE

#ifdef LEXDEBUG
#  define DEBUG(x) printf x
#else
#  define DEBUG(x)
#endif

#define MAXDIM 10		/* nombre max de dimensions d'un tableau */

int keyword = 1;
int num_ligne = 1;
char nomfic[MAXPATHLEN];        /* nom du fichier en cours de traitement */
FILE *sortie;
int enumValue = 0;

static int import_flag = 0;

static void yyerror(char *s);

MODULE_STR *module;
RQST_LIST *requetes = NULL;
int nbRequest = 0;
int initRequest = 0;
int maxRqstSize = 0, maxReplySize = 0;
EXEC_TASK_LIST *taches = NULL;
int nbExecTask = 0;
DCL_NOM_LIST *typedefs = NULL;
TYPE_LIST *types = NULL;
POSTER_LIST *posters = NULL;
ID_LIST *includeFiles = NULL; /* Fichiers inclus dans .gen */
ID_LIST *allIncludeFiles = NULL; /* Tous les fichiers inclus (trouves par cpp) */
ID_LIST *externLibs = NULL;
ID_LIST *externPathMacro = NULL;

#define MAX_CPP_OPT 20

char *cppOptions[MAX_CPP_OPT];
int nCppOptions = 0;

%}

/*----------------------------------------------------------------------*/

/**
 ** La grammaire proprement dite
 **/

%start liste_declaration_top

%union {
    int                  ival;
    double               dval;
    ID_LIST 		*idList;
    ID_STR 		*idStr;
    RQST_STR 		*rqstStr;
    RQST_AV_STR 	*rqstAV;
    RQST_LIST 		*rqstList;
    MODULE_STR 		*moduleStr;
    MODULE_AV_STR 	*moduleAV;
    EXEC_TASK_STR 	*taskStr;
    EXEC_TASK_AV_STR 	*taskAV;
    EXEC_TASK_LIST 	*taskList;
    TYPE_STR 		*typeStr;
    DCL_NOM_STR         *dclNomStr;
    DCL_NOM_LIST        *dclNomList;
    TYPE_LIST 		*typeList;
    STR_REF_STR 	*strRefStr;
    STR_REF_LIST        *strRefList;
    POSTER_LIST         *posterList;
    RQST_INPUT_INFO_LIST *rqstInputInfoList;
}

%token <idStr> IDENTIFICATEUR QUOTED_STRING
%token <ival> CONSTANTE_ENTIERE
%token <dval> CONSTANTE_FLOTTANTE
%token <ival>  AUTO STATIC EXTERN REGISTER TYPEDEF
%token <ival> CHAR SHORT INT LONG UNSIGNED FLOAT DOUBLE
%token <ival> STRUCT UNION ENUM
%token <ival> MEMBER N_WHITE

%token <ival> MODULE REQUEST EXEC_TASK IMPORT_TYPE FROM
%token <ival> INTERNAL_DATA
%token <ival> NUMBER MAX_RQST_SIZE MAX_REPLY_SIZE
%token <ival> TYPE INPUT OUTPUT C_CONTROL_FUNC C_EXEC_FUNC INCOMPATIBLE_WITH
%token <ival> C_EXEC_FUNC_START C_EXEC_FUNC_END
%token <ival> C_EXEC_FUNC_FAIL C_EXEC_FUNC_INTER
%token <ival> PERIOD C_INIT_FUNC C_END_FUNC PRIORITY STACK_SIZE C_FUNC
%token <ival> FAIL_MSG RESOURCES T_DELAY
%token <ival> CONTROL EXEC ALL G_NONE INIT
%token <ival> POSTER DATA UPDATE ADDRESS 
%token <idStr>  POSTER_LOCAL POSTER_SM_MEM POSTER_VME24 POSTER_VME32
%token <ival> USER
%token <ival> C_CREATE_FUNC
%token <ival> CS_CLIENT_FROM POSTER_CLIENT_FROM
%token <ival> RQST_NUM RQST_DOC RQST_INPUT_INFO

%type <ival> error 

%type <idStr> identificateur quoted_string
%type <ival> expression_constante
%type <dval> expression_constante_flottante
%type <ival> liste_declaration_top declaration_top
%type <dclNomList> liste_declaration_de_typedef
%type <rqstList> liste_declaration_de_requete
%type <taskList> liste_declaration_de_tache
%type <moduleStr> declaration_de_module
%type <rqstStr> declaration_de_requete 
%type <taskStr> declaration_de_tache

%type <moduleStr> liste_av_module attributs_de_module 
%type <moduleAV> paire_av_module av_module
%type <rqstStr> attributs_de_requete liste_av_requete 
%type <rqstAV> paire_av_requete av_requete
%type <ival> type_requete
%type <rqstList> liste_requete liste_requete_simple
%type <strRefStr> ref_membre_struct 
%type <idList> sdi_ref membre_sdi 
%type <idList> liste_echecs liste_ressources
%type <rqstInputInfoList> liste_input_info input_info

%type <posterList> liste_declaration_de_posters declaration_de_poster
%type <idStr> bus_space

%type <taskStr> attributs_de_tache liste_av_tache 
%type <taskAV> paire_av_tache av_tache
%type <ival> valeur_periode
%type <idList> liste_modules liste_posters
%type <strRefList> liste_ref_membre_struct
%type <typeList> liste_indicateur_de_type

%type <dclNomList> declaration_de_typedef
%type <typeStr> indicateur_de_declaration 
%type <typeStr> indicateur_de_type type_complexe indicateur_structure_union
%type <typeStr> indicateur_enum
%type <typeStr> type_simple type_entier
%type <ival> type_entier_simple type_flottant attributs_type_entier
%type <dclNomList> liste_elt_enum 
%type <dclNomList> declaration_struct  liste_decl_struct 
%type <dclNomStr> mem_struct elt_enum
%type <dclNomList> liste_mem_struct 
%type <dclNomList> liste_decl;
%type <dclNomStr>  elt_declarable
%type <ival> indicateur_classe_memorisation 
%type <idStr>  nom_typedef 

%right '?' ':'
%left ','
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQUAL NOTEQUAL
%left '<' '>' LEQ GEQ
%left LSH RSH
%left '+' '-'
%left '*' '/' '%'
%right UNARY

%%

/*
 * Un fichier contient une liste de declarations
 */

liste_declaration_top: declaration_top
    | declaration_top liste_declaration_top 
    | error liste_declaration_top
    ;

declaration_top: declaration_de_module ';' { $$ = 0; }
    | liste_declaration_de_typedef { typedefs = $1; $$ = 0; }
    | liste_declaration_de_requete { requetes = $1; $$ = 0; }
    | liste_declaration_de_tache { taches = $1; $$ = 0; }
    | liste_declaration_de_posters { posters = $1; $$ = 0; }
    ;

liste_declaration_de_typedef: declaration_de_typedef 
	{ $$ = $1;  
	  typedefs = $$; }
    | liste_declaration_de_typedef declaration_de_typedef  
	{ DCL_NOM_LIST *t;
	  if ($1 == NULL) {
              $$ = $2;
	      typedefs = $$;
	  } else {
	      for (t=$1; t->next != NULL; t=t->next);
	      t->next = $2;
	      $$ = $1;
	     typedefs = $$;
	  }
        }
;

liste_declaration_de_requete: declaration_de_requete ';'
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->rqst = $1; 
	  $$->name = $1->name;
	  $$->flags = 0;
	  $$->next = NULL; }
    | declaration_de_requete ';' liste_declaration_de_requete    
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->rqst = $1;
	  $$->name = $1->name;
	  $$->flags = 0; 
	  $$->next = $3; }
;

liste_declaration_de_tache: declaration_de_tache ';'
	{ $$ = STR_ALLOC(EXEC_TASK_LIST);
	  $$->exec_task = $1;
	  $$->next = NULL; }
    | declaration_de_tache ';' liste_declaration_de_tache 
	{ $$ = STR_ALLOC(EXEC_TASK_LIST);
	  $$->exec_task = $1;
	  $$->next = $3; }
;

liste_declaration_de_posters: declaration_de_poster ';'
	{ $$ = $1; } 
    | declaration_de_poster ';' liste_declaration_de_posters
        { $1->next = $3;
          $$ = $1; }
;

/*----------------------------------------------------------------------*/

declaration_de_module: MODULE identificateur attributs_de_module  
        { $3->name = $2; module = $3; $$ = $3; }
;

attributs_de_module: '{' liste_av_module '}' { $$ = $2; }
;

liste_av_module: paire_av_module
        { $$ = STR_ALLOC(MODULE_STR);
          $$ = ajout_av_module($1, $$); 
	  free($1); } 
    | paire_av_module liste_av_module 
	{ $$ = ajout_av_module($1, $2); 
	  free($1); }
    ;

paire_av_module: av_module ';' ;

av_module: INTERNAL_DATA ':' indicateur_de_type
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.internal_data = $3; }
    | NUMBER ':' expression_constante
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.number = $3; }
    | MAX_RQST_SIZE ':' expression_constante
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.max_rqst_size = $3; }
    | MAX_REPLY_SIZE ':' expression_constante
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.max_reply_size = $3; }
    ;

/*----------------------------------------------------------------------*/

declaration_de_requete: REQUEST identificateur attributs_de_requete  
    { $3->name = $2; $$ = $3; }
;

attributs_de_requete: '{' liste_av_requete '}' { $$ = $2; } 
;

liste_av_requete: paire_av_requete 
	{ $$ = STR_ALLOC(RQST_STR);
          $$->rqst_num = -1;
	  $$ = ajout_av_requete($1, $$); 
	  free($1); }
    | paire_av_requete liste_av_requete 
	{ $$ = ajout_av_requete($1, $2); 
	  free($1); }
    ;

paire_av_requete: av_requete ';' { $$ = $1; }
;	

av_requete: TYPE {keyword = 0;} ':' type_requete
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; $$->value.type = $4; keyword = 1;
	  if($4==INIT) {
	    initRequest++;
	    if(initRequest>1) 
	      fprintf(stderr, "%s: error: too many init requests.\n", nomfic);}}
    | INPUT ':' ref_membre_struct
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.input = $3; }
    | OUTPUT ':' ref_membre_struct
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.output = $3; }
    | C_CONTROL_FUNC ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_control_func = $3; }
    | C_EXEC_FUNC ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_exec_func = $3; }
    | C_EXEC_FUNC_START ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_exec_func_start = $3; }
    | C_EXEC_FUNC_END ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_exec_func_end = $3; }
    | C_EXEC_FUNC_FAIL ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_exec_func_fail = $3; }
    | C_EXEC_FUNC_INTER ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_exec_func_inter = $3; }
    | INCOMPATIBLE_WITH ':' liste_requete
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.incompatible_with = $3; }
    | EXEC_TASK ':' identificateur
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.exec_task_name = $3; }
    | FAIL_MSG ':' liste_echecs
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.fail_msg = $3; }
    | RESOURCES ':' liste_ressources
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.resource_list = $3; }
    | RQST_NUM ':' expression_constante
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.rqst_num = $3; }
    | RQST_DOC ':' quoted_string
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.doc = $3; }
    | RQST_INPUT_INFO ':' liste_input_info
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.input_info = $3; }
    ;

type_requete: EXEC
    | CONTROL
    | INIT
    ;

ref_membre_struct: indicateur_de_type 
        { TYPE_STR *t = trouve_type($1);
	  if (t == NULL) {
	    fprintf(stderr, "%s:%d: warning: type inconnu %s\n",
		    nomfic, num_ligne, $1->name);
	  } else if ($1 != t) {
	    free($1->name);
	    free($1->filename);
	    free($1);
	    markUsed(t);
	    $1 = t;
	  }
	}
	sdi_ref elt_declarable
        {
	  fprintf(stderr, "%s:%d: warning syntaxe obsolete. "
		  "Utilisez `variable::sdi_ref'\n", nomfic, num_ligne);
	  $$ = STR_ALLOC(STR_REF_STR);
	  $4->type = $1;
	  $$->dcl_nom = $4;
	  $$->sdi_ref = $3; }
    | elt_declarable MEMBER sdi_ref
        {
	  $$ = STR_ALLOC(STR_REF_STR);
	  $$->sdi_ref = $3;
	  $$->dcl_nom = $1;
	}
;

sdi_ref:  membre_sdi
	{ $$ = $1; }
    |  ALL 
	{ $$ = NULL; }
    ;

membre_sdi: identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = NULL; }
    |  identificateur '.' membre_sdi
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = $3; }
    ;

liste_requete: G_NONE { $$ = NULL; }
    | ALL 
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->name = NULL;	
	  $$->rqst = NULL;
	  $$->flags = $1;
	  $$->next = NULL; }
    | liste_requete_simple
    ;

liste_requete_simple: identificateur 
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->name = $1;
	  $$->rqst = NULL;
	  $$->flags = 0;
	  $$->next = NULL; }
    | identificateur ',' liste_requete_simple
	{  $$ = STR_ALLOC(RQST_LIST);
	  $$->name = $1;
	  $$->rqst = NULL;
	  $$->flags = 0;
	  $$->next = $3; }
    ;

liste_echecs: identificateur
	{ $$ = STR_ALLOC(ID_LIST); 
	  $$->name = $1;
	  $$->next = NULL; } 
    | identificateur ',' liste_echecs
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = $3; }
    ;

liste_ressources: identificateur
	{ $$ = STR_ALLOC(ID_LIST); 
	  $$->name = $1;
	  $$->next = NULL; } 
    | identificateur ',' liste_ressources
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = $3; }
    ;

liste_input_info: input_info
    { $$ = $1;}
    | input_info ',' liste_input_info
    { $$ = $1;
      $$->next = $3;}
    ;

input_info: expression_constante MEMBER quoted_string
    { $$ = STR_ALLOC(RQST_INPUT_INFO_LIST);
      $$->default_val.i_val = $1;
      $$->doc = $3;
      $$->type = INT;
      $$->next = NULL;}
    | expression_constante_flottante MEMBER quoted_string
    { $$ = STR_ALLOC(RQST_INPUT_INFO_LIST);
      $$->default_val.d_val = $1;
      $$->doc = $3;
      $$->type = DOUBLE;
      $$->next = NULL;}
    | quoted_string MEMBER quoted_string
    { $$ = STR_ALLOC(RQST_INPUT_INFO_LIST);
      $$->default_val.str_val = $1;
      $$->doc = $3;
      $$->type = CHAR;
      $$->next = NULL;}
    ;

quoted_string:
    QUOTED_STRING 
    {$$ = $1;}
    | QUOTED_STRING  quoted_string
      { $1 = xrealloc($1, strlen($1) + strlen($2) + 1);
        strcat($1, $2);
	free($2);
        $$ = $1;
      }
   ;

/*----------------------------------------------------------------------*/

declaration_de_tache: EXEC_TASK identificateur attributs_de_tache  
	{ $3->name = $2; $$ = $3; }
;

attributs_de_tache: '{' liste_av_tache '}' { $$ = $2; }
;

liste_av_tache: paire_av_tache
	{ $$ = STR_ALLOC(EXEC_TASK_STR);
	  $$ = ajout_av_tache($1, $$); 
	  free($1); }
    | paire_av_tache liste_av_tache
	{ $$ = ajout_av_tache($1, $2); 
	  free($1); }
    ;

paire_av_tache: av_tache ';' ;

av_tache: PERIOD ':' valeur_periode
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.period = $3; }
    | T_DELAY ':' valeur_periode
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.delay = $3; }
    | PRIORITY ':' expression_constante
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.priority = $3; }
    | STACK_SIZE ':' expression_constante
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.stack_size = $3; }
    | C_INIT_FUNC ':' identificateur
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_init_func = $3; }
    | C_END_FUNC ':' identificateur
      { $$ = STR_ALLOC(EXEC_TASK_AV_STR);
        $$->attribut = $1;
        $$->value.c_end_func = $3; }
    | C_FUNC ':' identificateur
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.c_func = $3; }
    | CS_CLIENT_FROM ':' liste_modules
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.cs_client_from = $3; }
    | POSTER_CLIENT_FROM ':' liste_posters
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.poster_client_from = $3; }
    | RESOURCES ':' liste_ressources
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.resource_list = $3; }
    | FAIL_MSG ':' liste_echecs
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.fail_msg = $3; }
    ;


valeur_periode: expression_constante
    | G_NONE { $$ = 0; }
    ;

liste_ref_membre_struct: ref_membre_struct
        { $$ = STR_ALLOC(STR_REF_LIST);
	  $$->str_ref = $1;
	  $$->next = NULL; }
    | ref_membre_struct ',' liste_ref_membre_struct
        { $$ = STR_ALLOC(STR_REF_LIST);
	  $$->str_ref = $1;
	  $$->next = $3;
         }
;

liste_modules: identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = NULL; }
    | liste_modules ',' identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $3;
	  $$->next = $1; }
    ;

liste_posters: identificateur MEMBER identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  free($3);
	  $$->next = NULL; }
    | liste_posters ',' identificateur MEMBER identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $3;
	  free($5);
	  $$->next = $1; }
    ;

/*----------------------------------------------------------------------*/

declaration_de_poster: POSTER identificateur '{' 
                 UPDATE ':' AUTO ';'
                 DATA ':' liste_ref_membre_struct ';'
		 EXEC_TASK ':' identificateur ';'
              '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = $10;
	  $$->type = NULL;
	  $$->types = NULL;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->address = NULL; 
	  $$->create_func = NULL; }
    | POSTER identificateur '{' 
                 UPDATE ':' USER ';'
                 TYPE ':' liste_indicateur_de_type ';'
		 EXEC_TASK ':' identificateur ';'
              '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = NULL;
	  $$->type = NULL;
	  $$->types = $10;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->address = NULL; 
	  $$->create_func = NULL; }
    | POSTER identificateur '{' 
                 UPDATE ':' USER ';'
                 DATA ':' liste_ref_membre_struct ';'
		 EXEC_TASK ':' identificateur ';'
              '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = $10;
	  $$->type = NULL;
	  $$->types = NULL;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->address = NULL; 
          $$->create_func = NULL; }
   | POSTER identificateur '{'
                 UPDATE ':' USER ';'
                 TYPE ':' liste_indicateur_de_type ';'
		 EXEC_TASK ':' identificateur ';'
                 ADDRESS ':' bus_space MEMBER expression_constante ';'
      '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = NULL;
	  $$->type = NULL;
	  $$->types = $10;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->bus_space = $18;
	  $$->address = (void *)$20; 
	  $$->create_func = NULL; }
   | POSTER identificateur '{'
                 UPDATE ':' USER ';'
                 TYPE ':' liste_indicateur_de_type ';'
		 EXEC_TASK ':' identificateur ';'
                 C_CREATE_FUNC ':' identificateur ';'
      '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = NULL;
	  $$->type = NULL;
	  $$->types = $10;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->bus_space = 0;
	  $$->address = NULL;
	  $$->create_func = $18; }
    ;

bus_space:
	POSTER_LOCAL { $$ = "POSTER_LOCAL_MEM"; }
        | POSTER_SM_MEM { $$ = "POSTER_SM_MEM"; } 
        | POSTER_VME24 { $$ = "POSTER_VME_A24"; } 
        | POSTER_VME32 { $$ = "POSTER_VME_A32"; } 
        ;

/*----------------------------------------------------------------------*/

declaration_de_typedef: 
    TYPEDEF indicateur_de_declaration liste_decl ';' 
	{ DCL_NOM_LIST *l;
	  for (l=$3; l!= NULL; l=l->next) {
	      l->dcl_nom->type = $2;
	      /* Identification des types STRING et ARRAY */
	      if(IS_STRING(l->dcl_nom)) {
		l->dcl_nom->flags = STRING;
		/* Est-ce une bonne idee ? */
		/* l->dcl_nom->ndimensions--;
		l->dcl_nom->max_str_len = l->dcl_nom->dimensions[l->dcl_nom->ndimensions]; */
		printf ("type STRING: %s\n", l->dcl_nom->name);
		fprintf(stderr, "%s:%d: warning: forme de déclaration de "
			"typedef pour %s non encore autorisée.\n", 
			nomfic, num_ligne, l->dcl_nom->name); 
	      }
	      if(IS_ARRAY(l->dcl_nom)) {
		l->dcl_nom->flags |= ARRAY;
		printf ("type ARRAY: %s\n", 
			l->dcl_nom->name);
		fprintf(stderr, "%s:%d: warning: forme de déclaration de "
			"typedef pour %s non encore autorisée.\n", 
			nomfic, num_ligne, l->dcl_nom->name);
	      }
	  }
	  $$ = $3; 
        }
    | IMPORT_TYPE { import_flag = 1; } '{' liste_declaration_de_typedef '}'  ';'
        { DCL_NOM_LIST *l;
	  for (l = $4; l!= NULL; l=l->next) {
	      l->dcl_nom->type->flags |= TYPE_IMPORT;
	  }
	  import_flag = 0;
	  $$ = $4; 
         }
| IMPORT_TYPE FROM identificateur { import_flag = 1; }
       '{' liste_declaration_de_typedef '}'  ';'
        { DCL_NOM_LIST *l;
	  ID_LIST *ld;
	  for (l = $6; l!= NULL; l=l->next) {
	      l->dcl_nom->type->flags |= TYPE_IMPORT;
	  }
	  ld = STR_ALLOC(ID_LIST);
	  ld->name = $3;
	  ld->next = externLibs;
	  externLibs = ld;
	  import_flag = 0;
	  $$ = $6; 
         } 
;

indicateur_de_declaration:
    indicateur_de_type
          { $$ = $1; }
    /* | indicateur_de_type indicateur_de_declaration 
          { $$ = $1; } */
    | indicateur_classe_memorisation indicateur_de_declaration
          { $$ = $2; } 
    ;

indicateur_classe_memorisation:
    AUTO
    | STATIC
    | EXTERN
    | REGISTER
    ;

liste_indicateur_de_type: indicateur_de_type
    { $$ = STR_ALLOC(TYPE_LIST); 
      $$->next = NULL; $$->type = $1; }
    | indicateur_de_type ',' liste_indicateur_de_type
    { $$ = STR_ALLOC(TYPE_LIST); 
      $$->next = $3; $$->type = $1; }
;

indicateur_de_type: 
     type_simple 
     | type_complexe
     ;

type_simple:
    type_entier
    | type_flottant
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = NULL;
	  $$->used = 0;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    ;

type_flottant:
    FLOAT
    | DOUBLE
    ;

type_entier:
    type_entier_simple
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = NULL;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    | attributs_type_entier type_entier_simple
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $2;
	  $$->flags |= $1;
	  $$->name = NULL;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    | attributs_type_entier
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = INT;
	  $$->flags |= $1;
	  $$->name = NULL;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    ;

type_entier_simple:
    CHAR 
    | INT 
    ;

attributs_type_entier:
    SHORT { $$ = SHORT_INT; }
    | LONG { $$ = LONG_INT; }
    | UNSIGNED { $$ = UNSIGNED_TYPE; }
    | UNSIGNED SHORT { $$ = UNSIGNED_TYPE | SHORT_INT; } 
    | UNSIGNED LONG { $$ = UNSIGNED_TYPE | LONG_INT; }
    ;

type_complexe:
    indicateur_structure_union 
	{ $$ = $1; }
    | indicateur_enum
        { $$ = $1; }
    | nom_typedef 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = TYPEDEF;
	  $$->name = $1;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    ;

/*
 * Les elements declares ou les noms dans un typedef
 */
liste_decl:
    elt_declarable 
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->next = NULL; $$->dcl_nom = $1; }
    | elt_declarable ',' liste_decl 
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->next = $3; $$->dcl_nom = $1; }
    ;

/*
 * La declaration de structures et d'unions
 */
indicateur_structure_union:
    STRUCT '{' liste_decl_struct '}' 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = new_name();
	  $$->members = $3; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }

    /* Structure nommee */
    | STRUCT identificateur '{' liste_decl_struct '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->members = $4;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }

    /* reference a une structure non definie */
    | STRUCT identificateur 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }

    /* Union sans nom */
    | UNION '{' liste_decl_struct '}' 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = new_name();
	  $$->members = $3; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }

    /* union non definie */
    | UNION identificateur 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  $$->members = NULL; }
    
    /* union nommee */
    | UNION identificateur '{' liste_decl_struct '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->members = $4; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }
    ;

liste_decl_struct:
    declaration_struct
    | declaration_struct liste_decl_struct
	{ DCL_NOM_LIST *l;
	  for (l=$1; l!= NULL && l->next != NULL; l=l->next)
	      ;
	  $$ = $1; l->next = $2; 
        }
    ;

declaration_struct:
    indicateur_de_type liste_mem_struct ';' 
        { /* Association variable/type */
	  DCL_NOM_LIST *l;
	  TYPE_STR *t = trouve_type($1);

	  /* Pas trouve le type correspondant */
	  if (t == NULL) {
	      fprintf(stderr, "%s:%d: warning: type inconnu %s\n",
		      nomfic, num_ligne, $1->name);
	  } else if ($1 != t) {
	      free($1->name);
	      free($1->filename);
	      free($1);
	      markUsed(t);
	      $1 = t;
	  }
	  for (l = $2; l != NULL; l = l->next) {
	      l->dcl_nom->type = $1;

	      /* Identification des ARRAY et des STRING */
	      if(IS_STRING(l->dcl_nom)) {
		l->dcl_nom->flags = STRING;
		/* l->dcl_nom->ndimensions--;
		l->dcl_nom->max_str_len = l->dcl_nom->dimensions[l->dcl_nom->ndimensions]; */
	      }
	      if(IS_ARRAY(l->dcl_nom)) {
		l->dcl_nom->flags |= ARRAY;
	      }

	  } /* for */
          $$ = $2; 
        }
;

liste_mem_struct:
    mem_struct 
        { $$ = STR_ALLOC(DCL_NOM_LIST);
          $$->next = NULL; $$->dcl_nom = $1; }
    | mem_struct ',' liste_mem_struct
        { $$ = STR_ALLOC(DCL_NOM_LIST);
          $$->next = $3; $$->dcl_nom = $1; }
    ;

/*
 * Ici on analyse un membre de structure:
 */
mem_struct:
    elt_declarable
    | elt_declarable ':' expression_constante
    ;


elt_declarable:
    identificateur 
	{ $$ = STR_ALLOC(DCL_NOM_STR);
	  $$->name = $1;
	  $$->type = NULL;
	  $$->pointeur = 0;
	  $$->ndimensions = 0;
	  $$->dimensions = NULL;
	}
    | '(' elt_declarable ')'  
	{ $$ = $2; }
    | '*' elt_declarable 
	{$2->pointeur++; $$=$2; 
	fprintf(stderr, "%s:%d: WARNING: pointer %s not recommended in SDI\n",
		nomfic, num_ligne, $2->name);}
    | elt_declarable '(' ')'
    | elt_declarable '[' ']' 
	{ $1->pointeur++; $$ = $1; }
    | elt_declarable '[' expression_constante ']' 
	{ $1->dimensions = ($1->dimensions == NULL ?
	      xalloc(sizeof(int)) :
	      xrealloc($1->dimensions, ($1->ndimensions + 1) * sizeof(int)));
	  $1->dimensions[($1->ndimensions)++] = $3; 
          $$ = $1; }
    ;

nom_typedef: 
    identificateur 
    ;

/*
 * Analyse des enum
 */
indicateur_enum:
    ENUM '{' liste_elt_enum '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = ENUM;
	  $$->name = new_name();
	  $$->members = $3;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
	}
    | ENUM identificateur '{' liste_elt_enum '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = ENUM;
	  $$->name = $2;
	  $$->members = $4;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
	}
    ;

liste_elt_enum:
    elt_enum
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->dcl_nom = $1; 
	  $$->next = NULL;
	  /* premier element */
	  if ($1->flags == ENUM_NO_VALUE) {
	      $1->pointeur = 0;
	      $1->flags = ENUM_VALUE;
	  }
	  enumValue = $1->pointeur + 1;
	}
    | liste_elt_enum ',' elt_enum
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->dcl_nom = $3;
	  $$->next = $1; 
	  if ($3->flags == ENUM_NO_VALUE) {
	      $3->pointeur = enumValue;
	      $3->flags = ENUM_VALUE;
	  }
	  enumValue = $3->pointeur + 1;
	}
    ;

elt_enum:
    identificateur
 	{ $$ = STR_ALLOC(DCL_NOM_STR); 
          $$->name = $1;
	  $$->type = NULL;
	  $$->pointeur = 0;
	  $$->ndimensions = 0;
	  $$->dimensions = NULL; 
	  $$->flags = ENUM_NO_VALUE; }
    | identificateur '=' expression_constante
	{ $$ = STR_ALLOC(DCL_NOM_STR);
          $$->name = $1;
	  $$->type = NULL;
	  $$->pointeur = $3;
	  $$->ndimensions = 0;
	  $$->dimensions = NULL; 
	  $$->flags = ENUM_VALUE; }

    ;
/*
 * Analyse des expressions constantes 
 */

expression_constante:
      '-' expression_constante %prec UNARY
	{ $$ = - $2; }
    | '!' expression_constante %prec UNARY
	{ $$ = ! $2; }
    | '+' expression_constante %prec UNARY
        { $$ = $2; }
    | '~' expression_constante %prec UNARY
	{ $$ = ~ $2; }
    | '(' expression_constante ')' 
   	{ $$ = $2; }
    | expression_constante '*' expression_constante
  	{ $$ = $1 * $3; }
    | expression_constante '/' expression_constante
	{ $$ = $1 / $3; }
    | expression_constante '%' expression_constante
	{ $$ = $1 % $3; }
    | expression_constante '+' expression_constante
	{ $$ = $1 + $3; }
    | expression_constante '-' expression_constante
	{ $$ = $1 - $3; }
    | expression_constante LSH expression_constante
	{ $$ = $1 << $3; }
    | expression_constante RSH expression_constante
  	{ $$ = $1 >> $3; }
    | expression_constante EQUAL expression_constante
	{ $$ = ($1 == $3); }
    | expression_constante NOTEQUAL expression_constante
  	{ $$ = ($1 != $3); }
    | expression_constante LEQ expression_constante
 	{ $$ = ($1 <= $3); }
    | expression_constante GEQ expression_constante
 	{ $$ = ($1 >= $3); }
    | expression_constante '<' expression_constante
	{ $$ = ($1 < $3); }
    | expression_constante '>' expression_constante
	{ $$ = ($1 > $3); }
    | expression_constante '&' expression_constante
	{ $$ = $1 & $3; }
    | expression_constante '^' expression_constante
	{ $$ = $1 ^ $3; }
    | expression_constante '|' expression_constante
	{ $$ = $1 | $3; }
    | expression_constante AND expression_constante
	{ $$ = $1 && $3; }
    | expression_constante OR expression_constante
	{ $$ = $1 || $3; }
    | expression_constante '?' expression_constante ':' expression_constante
	{ $$ = $1 ? $3 : $5; }
    | identificateur
        { if (trouve_value($1, &($$))) {
	    free($1);
	} else {
	    fprintf(stderr, "%s: %d: undefined symbol %s\n",
		    nomfic, num_ligne, $1);
	    $$=0;
	}
	}
    | CONSTANTE_ENTIERE
 	{ $$ = $1; }
    ;


expression_constante_flottante:
     '-' expression_constante_flottante %prec UNARY
	{ $$ = - $2; }
    | '!' expression_constante_flottante %prec UNARY
	{ $$ = ! $2; }
    | '+' expression_constante_flottante %prec UNARY
        { $$ = $2; }
    | '(' expression_constante_flottante ')' 
   	{ $$ = $2; }
    | expression_constante_flottante '*' expression_constante_flottante
  	{ $$ = $1 * $3; }
    | expression_constante_flottante '/' expression_constante_flottante
	{ $$ = $1 / $3; }
    | expression_constante_flottante '+' expression_constante_flottante
	{ $$ = $1 + $3; }
    | expression_constante_flottante '-' expression_constante_flottante
	{ $$ = $1 - $3; }
    | expression_constante_flottante EQUAL expression_constante_flottante
	{ $$ = ($1 == $3); }
    | expression_constante_flottante NOTEQUAL expression_constante_flottante
  	{ $$ = ($1 != $3); }
    | expression_constante_flottante LEQ expression_constante_flottante
 	{ $$ = ($1 <= $3); }
    | expression_constante_flottante GEQ expression_constante_flottante
 	{ $$ = ($1 >= $3); }
    | expression_constante_flottante '<' expression_constante_flottante
	{ $$ = ($1 < $3); }
    | expression_constante_flottante '>' expression_constante_flottante
	{ $$ = ($1 > $3); }
    | expression_constante_flottante AND expression_constante_flottante
	{ $$ = $1 && $3; }
    | expression_constante_flottante OR expression_constante_flottante
	{ $$ = $1 || $3; }
    | expression_constante_flottante '?' expression_constante_flottante ':' expression_constante_flottante
	{ $$ = $1 ? $3 : $5; }
    | CONSTANTE_FLOTTANTE
 	{ $$ = $1; }
    |  expression_constante
        { $$ =  $1; }
    ;

identificateur:
     IDENTIFICATEUR { $$ = $1; }
    | TYPE { $$ = "type"; }
    ;

%%
/*--------------------------------------------------------------------*/

/***
 *** Le code C
 ***/

int yydebug;
static char *callCpp(char *nomFic, char *cppOptions[]);

/**
 ** Programme principal
 **/

int
main(int argc, char **argv)
{
    extern FILE *yyin;
    char nomout[MAXPATHLEN];
    char cmdout[MAXPATHLEN];
    char pathmacro[MAXPATHLEN];
    char path[MAXPATHLEN];
    char *equal;
    
    char *nomTemp;
    char opt;
    int errFlag;
    int noExecFlag = 0;
    extern char *optarg;
    int nopt;
    int status, fatalError;

    char cwd[MAXPATHLEN];
    static const char *autoDir = "auto";
    static const char *upDir = "../";
    char chDir[10] = "";              /* upDir ou "" */
    char yesFlag;
    static const char *extention = ".gen";

    int genIfChange = 0;
    int genTcl = 0;
    int genPropice = 0;
    int genSpy = 0;
    int upToDate;
    struct stat statfile, statstamp, statgen;
    static const char *nomstamp = ".gen_stamp";
    FILE *stamp;
    ID_LIST *il;
    char *genfile;
    int installUserPart = 0;
    char *cmdLine=NULL;
    int i;

    char codelsDir[MAXPATHLEN] = "codels";

    static const char *usage = 
      "Usage: \n  genom [-i] [-c] [-d] [-n] [-p protoDir] [-s] [-t] "
      "[-u codelsDir] [-x]\n\t[-Ddefine] [-Ipath] [-Jvar=path] module[.gen]\n"
      "with:\n"
      "     -D: compilation flags\n"
      "     -I: path for included file\n"
      "     -J: make variable path name + path for included file (eg, -JFLAT=$(FLAT)\n"
      "     -i: installs codels part (new module)\n"
      "     -s: makes a spy server for this module\n"
      "     -c: generates if changes only  \n"
      "     -u: specifies the name of the codels directory\n"
      "     -d: debug \n"
      "     -n: produces \".pl\" without execution  \n"
      "     -p: changes the path for prototype files (canvas) \n"
      "     -t: produces on-board tcl client code\n"
      "     -x: produces propice interfaces\n\n"
      "     --includes: print path to libgenom includes\n";


    /* remember cwd */
    if (getcwd(cwd, MAXPATHLEN) == 0) {
       perror("couldn't compute current working directory");
       exit(2);
    }

    /* 
     * Parsing des arguments 
     */

    errFlag = 0;
    memset(cppOptions, 0, sizeof(cppOptions));
    nopt = argc;

    while ((opt = getopt(argc, argv, "D:I:J:dnp:ciu:txs-:")) != -1) {

	switch (opt) {
	  case 'D':
	    sprintf(nomout, "-%c%s", opt, optarg);
	    bufcat(&cmdLine, "-D%s ", optarg);
	    if (nCppOptions < MAX_CPP_OPT - 1) {
		cppOptions[nCppOptions++] = strdup(nomout);
	    } else {
		fprintf(stderr, "%s: too many options for cpp\n", argv[0]);
		exit(-1);
	    }
	    break;
	  case 'I':
	    sprintf(nomout, "-%c%s", opt, optarg);
	    bufcat(&cmdLine, "-I%s ", optarg);
	    if (nCppOptions < MAX_CPP_OPT - 1) {
		cppOptions[nCppOptions++] = strdup(nomout);
	    } else {
		fprintf(stderr, "%s: too many options for cpp\n", argv[0]);
		exit(-1);
	    }
	    break;
	  case 'J':
	    if (!(equal = strstr(optarg, "=")))
	      {
		fprintf(stderr, "%s: bad format for option -J\n", argv[0]);
		exit(-1);
	      }
	    bufcat(&cmdLine, "-J%s ", optarg);

	    /* separation between macro and path */
	    strcpy(path, equal+1);
	    strncpy(pathmacro, optarg, equal-optarg);
	    pathmacro[equal-optarg+1] = '\0';

	    /* make option for cpp with the path */
	    sprintf(nomout, "-I%s", path);
	    if (nCppOptions < MAX_CPP_OPT - 1) {
		cppOptions[nCppOptions++] = strdup(nomout);
	    } else {
		fprintf(stderr, "%s: too many options for cpp\n", argv[0]);
		exit(-1);
	    }

	    /* keep macro name for makefiles */
	    il = STR_ALLOC(ID_LIST);
	    il->name = strdup(pathmacro);
	    il->next = externPathMacro;
	    externPathMacro = il;

	    break;
	  case 's':
	    genSpy = 1;
	    break;
	  case 'd':
	    yydebug = 1;
	    break;
	  case 'n':
	    noExecFlag = 1;
	    break;
	  case 'p':
	    bufcat(&cmdLine, "-p%s ", optarg);
	    protoDir = strdup(optarg);
	    break;
	  case 'c':
	    genIfChange = 1;
	    break;
	  case 'i':
	    installUserPart = 1;
	    break;
	  case 'u':
	    bufcat(&cmdLine, "-u%s ", optarg);
	    sprintf (codelsDir, "%s", optarg);
	    break;
	  case 't':
	    genTcl = 1;
	    break;
	  case 'x':
	    genPropice = 1;
	    break;

	  case '-':
	     if (!strcmp("includes", optarg)) {
		puts(includeDir);
		exit(0);
	     } else if (!strcmp("libraries", optarg)) {
		puts(libraryDir);
		exit(0);
	     }

	     /* FALLTROUGH */
	  case '?':
	    errFlag++;
	    break;
	    
	}  /* switch */
	nopt--;
    } /* while */

    fprintf(stderr, 
	    "Module Generator GenoM\n"
	    "Copyright (C) LAAS/CNRS 1994-2003\n");

    if (errFlag) {
      fprintf(stderr, usage);
      exit(2);
    }

    /* make sure we've an input file */
    if (nopt == 0) {
	fprintf(stderr, "%s: error: no file specified\n", argv[0]);
	fprintf(stderr, usage);
	exit (2);
    } 

    /* Enregistrer le nom du fichier a analyser */
    bufcat(&cmdLine, "%s", argv[argc - 1]);
    strcpy(nomfic, argv[argc - 1]);

    /* add .gen extension if missing */
    if (strcmp(nomfic + (strlen(nomfic) - strlen(extention)), 
	       extention) != 0) {
	strcat(nomfic, extention);
    }

    /* Traitement des options de cpp */
    for (i = 0; i<nCppOptions; i++) {
	if (cppOptions[i][0] == '-' && cppOptions[i][1] == 'I') {
	    if (cppOptions[i][2] != '/' && chDir[0] != '\0') {
		/* chemin relatif et on a changé de répertoire */
		sprintf(nomout, "-I%s%s", chDir, &cppOptions[i][2]);
		free(cppOptions[i]);
		cppOptions[i] = strdup(nomout);

	    }
	}
    } /* for */
    cppOptions[nCppOptions] = NULL;
    nomTemp = callCpp(nomfic, cppOptions);
    genfile = strdup(nomfic);

    /*
     * Analyse du fichier de description
     */

    if ((yyin = fopen(nomTemp, "r")) == NULL) {
	perror(nomTemp);
	exit(2);
    }
    if (yyparse() == 1) {
	exit(2);
    }
    if (yydebug == 0) {
	unlink(nomTemp);
    }
    if (module == NULL) {
	fprintf(stderr, "\"%s\": Error: can't find a MODULE declaration\n", 
	       nomfic);
	exit(2);
    }
    if (yynerrs != 0) {
      fprintf(stderr, "Parsing errors.\n");
      exit(2);
    }

    /* change to auto directory */
    if (strlen(cwd) < strlen(autoDir) ||
	(strcmp(cwd + strlen(cwd) - strlen(autoDir), autoDir) != 0)) {
	
	/* mkdir autoDir */
	if (stat(autoDir, &statfile) == -1 
	    || ((statfile.st_mode & S_IFDIR) == 0)) {
	    fprintf(stdout, "No directory %s, install it ? (y/n) ", autoDir);
	    fscanf(stdin, "%c", &yesFlag);
	    if (yesFlag != 'y') {
		exit(0);
	    }
	    if (mkdir (autoDir, 0775) != 0) {
		perror ("mkdir auto");
		exit(2);
	    }
	}

	/* chdir autoDir */
	fprintf (stderr, "Entering directory `%s'\n", autoDir);
	if (chdir (autoDir) == -1) {
	    perror ("chdir auto");
	    exit(2);
	}
	strcpy(chDir, upDir);
    }

    /* 
     * Faut-il regenerer ? 
     */

    if(genIfChange && fopen(nomstamp, "r") != NULL) {
       strcpy(nomfic, chDir);
       strcat(nomfic, genfile);
       if (stat(nomfic, &statfile) < 0) {
	  perror(nomfic);
	  exit(2);
       }
       /* argv[0] doesn't contain full path */
       if (stat(argv[0], &statgen) < 0) {
	  statgen.st_mtime = 0;
       }
       if (stat(nomstamp, &statstamp) == 0) {
	  /* Fichier .gen a jour */
	  if (statstamp.st_mtime > statfile.st_mtime && 
	      statstamp.st_mtime > statgen.st_mtime) {
	     upToDate = 1;

	     /* Test date fichiers inclus dans .gen */
	     for (il = allIncludeFiles; il != NULL; il = il->next) {
		printf ("allIncludeFiles : %s \n", il->name);
		stat(il->name, &statfile);
		if (statstamp.st_mtime < statfile.st_mtime) {
		   upToDate = 0;
		   break;
		}
	     }

	     if (upToDate) {
		fprintf (stderr, "Your module is up to date.\n");
		exit(0);
	     }
	  }
       }
    }
    unlink(nomstamp);


    /*
     * Contruction des donnees
     */

    upCaseNames();

    resolveRequests();
    resolveTypes();
    resolveTasks();
    construitIncludeList();

    /*
     * Generation du fichier perl
     */

    /* En-tete du fichier */
    sprintf(nomout, "./%s.pl", module->name);
    if ((sortie = fopen(nomout, "w")) == NULL) {
        perror(argv[0]);
        exit(2);
    }

    fprintf(sortie, "#!/usr/bin/env perl\n");
    fprintf(sortie, "# Generateur du module %s\n\n", module->name);

    /* Variables generales pour perl */
    fprintf(sortie, "$module=\"%s\";\n", module->name);
    fprintf(sortie, "$genPropice=\"%d\";\n", genPropice);
    fprintf(sortie, "$genSpy=\"%d\";\n\n", genSpy);

    /* Debut du fichier perl */
    script_do(sortie, protoDir, "auto/start.pl");

    /* Le module */
    fatalError = 0;
    fatalError |= (makeGen(sortie,genfile,genTcl,genPropice,genSpy) != 0);
    fatalError |= (typeGen(sortie) != 0);
    fatalError |= (errorGen(sortie) != 0);
    fatalError |= (msgLibGen(sortie) != 0);
    fatalError |= (endianGen(sortie) != 0);
    fatalError |= (printGen(sortie) != 0);
    fatalError |= (scanGen(sortie) != 0);
    fatalError |= (scopeGen(sortie) != 0);
    fatalError |= (headerGen(sortie) != 0);
    fatalError |= (cntrlTaskGen(sortie) != 0);
    fatalError |= (execTaskGen(sortie) != 0);
    fatalError |= (moduleInitGen(sortie) != 0);
    fatalError |= (essayGen(sortie) != 0);
    fatalError |= (posterLibGen(sortie) != 0);
    fatalError |= (initGen(sortie) != 0);
    fatalError |= (reportsGen(sortie) != 0);
    fatalError |= (configureGen(sortie,
				codelsDir, cmdLine, argv[0], genfile, cwd,
				genTcl, genPropice, genSpy) != 0);

    /****** par Laurent ******/
    if (genSpy)
    {
     fatalError |= (spyDescriptionGen(sortie) != 0);
     fatalError |= (spyStructGen(sortie) != 0);
     fatalError |= (spyServerLibGen(sortie) != 0);
    }
    /******             ******/
    if (genPropice)
      fatalError |= (propiceGen(sortie) != 0); 
    if (genTcl) {
       fatalError |= (tclGen(sortie) != 0);
       fatalError |= (tclClientGen(sortie) != 0);
    }

    /* La partie user */
    fatalError |= (userMakeGen(sortie, codelsDir, cmdLine) != 0);
    fatalError |= (userExecCodelsGen(sortie) != 0);
    fatalError |= (userCntrlCodelsGen(sortie) != 0);
    fatalError |= (userStartGen(sortie, codelsDir) != 0);

    /* On termine */
    fprintf(sortie, "$codelsDir=\"%s\";\n\n", codelsDir);
    fprintf(sortie, "$installUserPart=\"%d\";\n\n", installUserPart);
    script_do(sortie, protoDir, "auto/end.pl");
    fclose(sortie);


    if (!fatalError) {
	/* 
	 * Execution de perl 
	 */
	
	if (chmod(nomout, 0755) < 0) {
	    perror("chmod");
	    exit(1);
	}
	if (!noExecFlag) {
	     sprintf(cmdout, "perl %s\n", nomout);
	     fprintf(stderr,"%s", cmdout);
	     status = system(cmdout);
	    unlink(nomout);
	} else {
	    fprintf(stderr, "File %s is written\n", nomout);
	    status = 0;
	}
    
	/* Mise a jour du fichier stamp */
	if (status == 0) {
	    stamp = fopen(nomstamp, "w");
	    fclose(stamp);

	    fprintf(stderr, "Done.\n");
	    exit(0);
	} else {
	    /* erreur durant l'execution du script */
	    perror ("Fatal error");
	    exit(2);
	}
	    
    } else {
	/* Erreur detecte'e pendant la generation */
	fprintf(stderr, "No code generated.\n");
	unlink(nomout);
	exit (2);
    }
    /* on ne devrait pas arriver la` */
    return(2);
} /* main */


/*------------------------------------------------------------------------*/

/**
 ** Traitement des erreurs lors du parsing
 **/
static void 
yyerror(char *s)
{
    fprintf(stderr, "\"%s\", line %d: %s\n", nomfic, num_ligne, s);
} /* yyerror */

/*----------------------------------------------------------------------*/

/**
 ** Appel du preprocesseur C 
 **/

#ifndef STDINCPP
# define STDINCPP "gcc -E -"
#endif /* STDINCPP */

static char *
callCpp(char *nomFic, char *cppOptions[])
{
    char *tmpName;
    char *cppArg[MAX_CPP_OPT + 3], *cpp;
    int in, out, i, j, status;
    
    /* open input and output files */
    in = open(nomFic, O_RDONLY, 0);
    if (in < 0) {
       fprintf(stderr, "cannot open %s for reading: ", nomFic);
       perror("");
       exit(2);
    }

    tmpName = strdup("/tmp/genomXXXXXX");
    /* use a safe temporary file */
    out = mkstemp(tmpName);
    if (out < 0) {
       fprintf(stderr, "genom: cannot open %s for writing: ", tmpName);
       perror("");
       exit(2);
    }
   
    /* build the argv array: split cpp into argvs and copy options */
    cpp = strdup(STDINCPP);
    if (!cpp) {
       perror("genom: cannot exec cpp");
       exit(2);
    }

    for(i=0; *cpp != '\0'; i++) {
       if (i > MAX_CPP_OPT) {
	  fputs("too many options to cpp\n", stderr);
	  free(cppArg[0]);
	  exit(2);
       }

       cppArg[i] = cpp;
       cpp += strcspn(cpp, " \t");
       if (*cpp != '\0') {
	  *cpp = '\0';
	  cpp++;
	  cpp += strspn(cpp, " \t");
       }
    }

    for(j=0; cppOptions[j] != NULL;) {
       if (i > MAX_CPP_OPT) {
	  fputs("too many options to cpp\n", stderr);
	  free(cppArg[0]);
	  exit(2);
       }

       cppArg[i++] = cppOptions[j++];
    }

    /* cpp output goes to stdout */
    cppArg[i] = NULL;

    if (fork() == 0) {
	    /* read stdin from nomFic */
	    if (dup2(in, fileno(stdin)) < 0) {
		    fprintf(stderr, "genom: cannot redirect cpp stdin: %s\n",
			strerror(errno));
		    exit(2);
	    }

	    /* redirect stdout to tmpName */
	    if (dup2(out, fileno(stdout)) < 0) {
		    fprintf(stderr, "genom: cannot redirect cpp output: %s\n",
			strerror(errno));
		    exit(2);
	    }
	    execvp(cppArg[0], cppArg);
    }
    free(cppArg[0]);
    wait(&status);
    if (! WIFEXITED(status) || WEXITSTATUS(status) != 0) {
	    fprintf(stderr, "genom: cpp failure\n");
	    exit(2);
    }
    return(tmpName);
} /* callCpp */
    
/*----------------------------------------------------------------------*/
