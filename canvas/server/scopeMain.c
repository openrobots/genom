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

/*------------------  Fichier généré automatiquement ------------------*/
/*------------------  Ne pas éditer manuellement !!! ------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <portLib.h>

#include "$module$ScopeLib.h"

void $module$Scope(char *cpu);
BOOL $module$ScopeAll (char *rack, char *cpu, H2_SCOPE_SIG_STR **scopelist,
		       H2_SCOPE_SDI_OR_POSTER sdiOrPoster);

/*----------------------------------------------------------------------
 * Fonction principale interactive pour intaller des signaux sous
 * scope. Les element de la SDI de $module$ sont geres par la
 * fonction $module$ScopeAll passee en parametre
 */

int
main(int argc, char **argv)
{
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "usage: h2scope <rack> [<default_cpu>]\n");
    exit(1);
  }

  if (argc == 2)
    h2scopeIhmTxtMain(argv[1], argv[2], "$module$",
		      (H2_SCOPE_MODULE_FUNC_PT)($module$ScopeAll));
  else
    h2scopeIhmTxtMain(argv[1], argv[2], "$module$",
		      (H2_SCOPE_MODULE_FUNC_PT)($module$ScopeAll));
    

  /* Pour que la fenetre ne disparaisse pas tout de suite */
  printf("Type return again\n");
  getchar();
  exit(0);
} /* main */




