;;------------------------------------------------------------
;;
;;                    GenoM mode for XEmacs 19 et 20
;;
;;------------------------------------------------------------
;
; KNOWN PROBLEM: 
; Does not work any more with emacs because emacs and xemacs 
; c-mode are now very differents. Contributions well come :-)
;
;------------------------------------------------------------
;     
;  Creation date        : April 1997
;  Date de modification : 
;  File                 : genom-mode.el
;  Author               : Sara Fleury
;     
;  Inspired from:
;
;     /opt/gnu/lib/xemacs-19.14/lisp/modes/cc-mode.el
;     /opt/gnu/lib/xemacs-19.14/lisp/packages/font-lock.el
;     /opt/gnu/lib/xemacs-19.14/lisp/prim/help.el
;     /home/sara/h2/src/modules/genom/emacs/bib-mode.el
;
; To compile : appeler byte-compile-file  depuis XEmacs 
;                (XEmacs-19 pour compatibilité)
;
; Modified the 17/02/98 for compatibility with XEmacs 20:
;     * add init fonction: c-initialize-cc-mode 
;     * add c-make-inherited-keymap
;     * c-mode-menu is now a fonction
;
;------------------------------------------------------------

;------------------------------------------------------------
;
; GLOBAL VARIABLES
;
;------------------------------------------------------------

;; Variables configuration
(defvar genom-comment-length  60 
"Lenght of comment lines (number of minus characters) while creating a 
new module using genom-mode.
To change this value : (setq genom-comment-length <val>)")

(defvar genom-minus-char-string-width genom-comment-length 
"Use genom-comment-length instead.")

(defvar genom-tab-right-column 28 
"Position of the right tab column (field value).
To change its value : (setq genom-tab-right-column <val>)")

(defvar genom-find-file-hook 'find-file-other-window 
"Open file function used by genom-mode. Default: find-file-other-window. 
Can also be set to find-file or find-file-other-frame :
eg : (setq genom-find-file-hook 'find-file)")

(defvar genom-default-codel-name t 
"t or nil. If t (default) a name is proposed for the new codels.")

;------------------------------------------------------------
;
; Emacs 19 - 20 compatibilities
;
;------------------------------------------------------------

; Missing definitions for emacs 19
(defun-when-void c-initialize-cc-mode () "empty declaration for emacs versions less than 20" ())
(defun-when-void c-make-inherited-keymap () "empty declaration for emacs versions less than 20" ())

; Missing definitions for emacs 20
(defun-when-void c-setup-dual-comments (table) "empty declaration for xemacs versions more than 19" ())

;------------------------------------------------------------
;
; Init c-mode
;
;------------------------------------------------------------

;; cc-mode init (emacs 20)
(c-initialize-cc-mode)
(font-lock-mode t)

;------------------------------------------------------------
;
; GenoM Commands Menu
;
;------------------------------------------------------------

(defun genom-mode-menu (modestr)
  "XEmacs menu for GenoM mode. (XEmacs versions : 19 and 20)"
  (let ((m
  '(["New Module"             genom-make-module t]
    ["New Import Structures"  genom-make-import t]
    ["New Request"            genom-make-request t]
    ["New Poster"              genom-make-poster t]
    ["New Execution Task"      genom-make-exec-task t]
    "---"
    ["Refresh .gen"               genom-make-extent t]
    ["Delete Optional Lines"     genom-clean-structure t]
    ["Verify Buffer"             genom-verify-buffer t]
    "---"
    ["Indent Line"             c-indent-command t]
    ["Indent Region"          indent-region (mark)]
    ["Indent Buffer"           genom-indent-buffer t]
    ["Comment Region"    comment-region (mark)]
    "---"
    ("Help      C-c C-h ... "
     ["About GenoM Commands" genom-help-genom-commands t]
     ["About Module" genom-help-module t]
     ["About Requests" genom-help-request t]
     ["About Posters" genom-help-poster t]
     ["About Execution Task" genom-help-exec-task t]
     ["About Import Structures" genom-help-import t]
     ["About Module Generation" genom-help-generate t]
     )
    )))
    (cons modestr m)))


(defvar genom-mode-map ()
  "Keymap used in genom-mode buffers.")

;------------------------------------------------------------
;
; Keys maping for GenoM
;
;------------------------------------------------------------


(if genom-mode-map
    ()
  ;; In Emacs 19, it makes more sense to inherit c-mode-map
  (if (memq 'v19 c-emacs-features)
      ;; XEmacs and Emacs 19 do this differently
      (cond
       ;; XEmacs 19.13
       ((fboundp 'set-keymap-parents)
	(setq genom-mode-map (make-sparse-keymap))
	(set-keymap-parents genom-mode-map c-mode-map))
       ((fboundp 'set-keymap-parent)
	(setq genom-mode-map (make-sparse-keymap))
	(set-keymap-parent genom-mode-map c-mode-map))
       (t (setq genom-mode-map (cons 'keymap c-mode-map)))
       )
    ;; On laisse tomber emacs 18 et on s'occupe d'emacs 20
      ; Do it the hard way for Emacs 18 -- given by JWZ
      ; (setq genom-mode-map (nconc (make-sparse-keymap) c-mode-map)))
    ;; Emacs 20
    (setq genom-mode-map (c-make-inherited-keymap))
    )


  ;; add bindings which are only useful for Genom
  (define-key genom-mode-map "\C-c\C-m" 'genom-make-module)
  (define-key genom-mode-map "\C-c\C-r" 'genom-make-request)
  (define-key genom-mode-map "\C-c\C-p" 'genom-make-poster)
  (define-key genom-mode-map "\C-c\C-e" 'genom-make-exec-task)
  (define-key genom-mode-map "\C-c\C-i" 'genom-make-import)
  (define-key genom-mode-map "\C-c\C-d" 'genom-clean-structure)
  (define-key genom-mode-map "\C-c\C-b" 'genom-indent-buffer)
  (define-key genom-mode-map "\C-c\C-v" 'genom-verify-buffer)
  (define-key genom-mode-map '(control button2) 'genom-make-extent)
  (define-key genom-mode-map "\C-c\C-x" 'genom-make-extent)
;  (define-key genom-mode-map "\C-c\C-?" 'genom-help-for-help)
;  (define-key genom-mode-map genom-help-string 'genom-help-for-help)
  (define-key genom-mode-map "\C-c\C-h" 'genom-help-for-help)
  (define-key genom-mode-map "/"      'c-electric-slash)

  ;; Emacs 19 defines menus in the mode map. This call will return t
  ;; on Emacs 19, otherwise no-op and return nil.
;XXX Apparement ne sert plus (bizard ?) et gène emacs 20
;  (if (memq 'v19 c-emacs-features)
;      (c-mode-fsf-menu "GenoM" genom-mode-map)
;    ())

  ;; Contextual menu (apparement ca se fait tout seul)
;;  (define-key genom-mode-map 'button3 'genom-popup-menu)
)

;------------------------------------------------------------
;
;  C Indentation for GenoM
;
;------------------------------------------------------------

(defconst genom-style
  '((c-basic-offset . 4)
    (c-comment-only-line-offset . (0 . 0))
    (c-offsets-alist . ((statement-block-intro . +)
			(substatement-open     . +)
			(label                 . 4)
			(statement-case-open   . +)
			(statement-cont        . 0)
			(access-label  . 0)
			)))
  "GenoM programming style")


;------------------------------------------------------------
;
; GenoM Mode
;
;------------------------------------------------------------

(defvar genom-mode-hook nil
  "*Hook called by `genom-mode'.")

(defvar genom-mode-abbrev-table nil
  "Abbrev table in use in genom-mode buffers.")
(define-abbrev-table 'genom-mode-abbrev-table ())


(defvar genom-mode-syntax-table nil
  "Syntax table used in genom-mode buffers.")
(if genom-mode-syntax-table
    ()
  (setq genom-mode-syntax-table (make-syntax-table))
  (c-populate-syntax-table genom-mode-syntax-table)
  ;; add extra comment syntax
  ;; nécessaire à emacs 19 mais perturbe emacs 20
  (if (memq 'v19 c-emacs-features)
      (c-setup-dual-comments genom-mode-syntax-table)
    ())
  ;; everyone gets these
  (modify-syntax-entry ?@ "_" genom-mode-syntax-table)
  )


(defun genom-mode ()
  "Major mode for editing GenoM code.
To submit a problem, report to sara@laas.fr

Help : C^c C^h

Configuration : you can change the following variables 

 genom-comment-length     <length> : length of comment lines
 genom-tab-right-column   <pos>    : tab position for right column
 genom-default-codel-name <t/nil>  : if t a default name is proposed for new codel
 genom-find-file-hook    '<func>   : find-file function

 eg: (setq genom-comment-length 60)

The hook variable `genom-mode-hook' is run with no args, if that value
is bound and has a non-nil value.  Also the common hook
`c-mode-common-hook' is run first.

Key bindings:
\\{genom-mode-map}"
  (interactive)
  (kill-all-local-variables)
  (set-syntax-table genom-mode-syntax-table)
  (setq major-mode 'genom-mode
 	mode-name "GenoM"
 	local-abbrev-table genom-mode-abbrev-table)
  (use-local-map genom-mode-map)
  (c-common-init)
  ; add genom menu to the menubar 
  ; (inspiré de easy-menu qui n'existe pas pour xemacs 19)
  (cond ((eq emacs-major-version 19)
	 (add-submenu nil (genom-mode-menu 
			   (concat mode-name " Mode Commands"))))
	((eq emacs-major-version 20)
	 (add-submenu nil (genom-mode-menu mode-name))))

  (setq mode-popup-menu (genom-mode-menu mode-name))
  (setq comment-start "/* "
 	comment-end   " */"
 	comment-multi-line t
 	c-comment-start-regexp comment-start
 	c-baseclass-key nil)
  (c-add-style "genom" genom-style t)
;  (set-variable '*fonctions-sur-au-revoir* '(transforme))
  (run-hooks 'c-mode-common-hook)
  (c-set-style "genom")
  (run-hooks 'genom-mode-hook)
  (setq c-special-indent-hook 'genom-indent-line-c-hook)
  ;; Construit des regions actives pour toutes les lines correspondants
  ;; a des definitions de codels
  (genom-make-extent)
)

;XXX Ne semble indispensable et gène emacs 20
;  (if (memq 'v19 c-emacs-features)
;(setq c-list-of-mode-names (cons "GenoM" c-list-of-mode-names))
;())


;------------------------------------------------------------
;
; Fontify
;
;------------------------------------------------------------
(put 'genom-mode 'font-lock-defaults 
     '((genom-font-lock-keywords
	genom-font-lock-keywords-1
	genom-font-lock-keywords-2)
       nil nil ((?_ . "w")) beginning-of-defun))

(defvar genom-opt-var nil "Regexp that matches the optional variable names ( <opt_var_name> )")
(defvar genom-mand-var nil "Regexp that matches the mandatory variable names( <<mand_var_name>> ) ")
(defvar genom-fields nil "Regexp that matches the field names ( field_name: )")
(defvar genom-codel-fields nil "Regexp that matches the codel field names ( field_name: )")
(defvar genom-types nil "Regexp that matches the genom structure type")
(defvar genom-end-types nil "String that indicates the end of a genom structure declaration")

(defconst genom-font-lock-keywords-1 nil
 "For consideration as a value of `genom-font-lock-keywords'.
This does fairly subdued highlighting.")

(defconst genom-font-lock-keywords-2 nil
 "For consideration as a value of `genom-font-lock-keywords'.
This does fairly subdued highlighting.")

(defconst genom-etc " , ..." "String that indicates an optional list of variables")


(let ((types (concat "request\\|module\\|typedef struct\\|"
		     "import from\\|poster\\|exec_task"))
      (fields (concat
	       "type\\|doc\\|input\\|posters_input\\|input_info\\|output\\|"
	       "interrupt_activity\\|"
               "fail_msg\\|fail_reports\\|"
               "incompatible_with\\|"
	       "resources\\|activity\\|exec_task\\|"
	       "update\\|address\\|number\\|internal_data\\|version\\|"
	       "email\\|uses_cxx\\|lang\\|requires\\|codels_requires\\|data\\|"
	       "period\\|delay\\|priority\\|stack_size"))
      (codel-fields (concat
	       "codel_control\\|"
	       "codel_start\\|codel_end\\|codel_inter\\|"
	       "codel_fail\\|codel_main\\|codel_task_main\\|"
	       "codel_task_start\\|codel_task_end\\|codel_poster_create\\|"
               "c_control_func\\|"
               "c_exec_func_start\\|c_exec_func_end\\|c_exec_func_inter\\|"
               "c_exec_func_fail\\|c_exec_func\\|c_func\\|"
               "c_init_func\\|c_end_func\\|c_create_func"))
      (mand-var (concat
		 "exec-task-name\\|number\\|struct-name\\|name\\|sdi-ref\\|"
		 "file-name"))
      (opt-var (concat
		"name\\|sdi-ref\\|codel\\|number\\|report-name\\|default-value\\|"
		"exec-rqst-name\\|module-name\\|poster-name\\|file-name"))
      (reserved-words (concat 
		       "control\\|exec\\|init\\|user\\|auto\\|all\\|none\\|"
		       "server\\|filter\\|servo_process\\|surveillance\\|"
		       "local\\|sm\\|vme24\\|vme32"))
      )
  (setq genom-codel-fields (concat "\\<\\(" codel-fields "\\)" "\:"))
  (setq genom-fields (concat "\\<\\(" fields "\\|" codel-fields "\\)" "\:"))
  (setq genom-mand-var (concat "<<" "\\("  mand-var "\\)" ">>"))
  (setq genom-opt-var (concat "<" "\\("  opt-var "\\)" ">"))
  (setq genom-types (concat "\\<\\(" types "\\) \+\\w\+ \*{"))
  (setq genom-end-types (concat "} \*\\w*;"))
  (setq genom-font-lock-keywords-1
      (append	
	(list
	 ;;
	 ;; fontify all types classes and type specifiers
	 ;; name des field (attribut)
	 (cons genom-fields 'font-lock-keyword-face)
	 ;; Field obligatoires (valeur)
	 (cons (concat "<<" "\\("  mand-var "\\)" ">>") 'font-lock-string-face)
	 ;; Field optionnels (valeur)
	 (cons  genom-opt-var 'font-lock-doc-string-face)
	 (cons genom-etc 'font-lock-doc-string-face)
	 ;; Debut et fin de structures
	 (cons genom-types 'font-lock-type-face)
	 (cons genom-end-types 'font-lock-type-face)
	 ;; Autres mots reserves
	 (cons (concat "\\<" "\\("  reserved-words "\\)" ";") 
	       'font-lock-keyword-face)
	 (cons "\:\:" 'font-lock-keyword-face)
	 )
	;; Font-lock du C
	c-font-lock-keywords-1))
  (setq genom-font-lock-keywords-2
      (append	
       genom-font-lock-keywords-1
       c-font-lock-keywords-2)
   )
)


(defvar genom-font-lock-keywords genom-font-lock-keywords-1
  "Additional expressions to highlight in Genom mode.")

;------------------------------------------------------------
;
; GenoM Indentation
;
;------------------------------------------------------------

;; To indent the current buffer
(defun genom-indent-buffer ()
  "Indents current buffer"
  (interactive)
  (c-indent-region (point-min) (point-max))
)

;; Cette function d'indentation prend la main apres une indentation "c" 
;; (c-indent-command).
;;
;; Elle indente la partie droite des lines "attribut:   valeur;" 
;; des structures genom (request, poster, ...).
;; L'indentation est sur la colonne genom-tab-right-column
(defun genom-indent-line-c-hook ()
  "
c-special-indent-hook in genom-mode. Indents current line.
The right part of a declaration field is aligned on the column of value 
genom-tab-right-column."
  (let (init-pos-point pos-semi-column)

    ;; Recupere position courante du curseur
    ;; et la position eventuelle du ":" d'un genom-fields ("field:")
    (save-excursion
      (setq init-pos-point (point))
      (beginning-of-line)
      (skip-chars-forward " \t")
      (if (looking-at genom-fields)
	  (setq pos-semi-column (match-end 0)))
      )
    
    ;; On a trouve le ":"
    (if pos-semi-column 
	;; Le curseur est avant le ":"  
	;; on tabule et on remet le curseur a sa position initiale
	(if (< init-pos-point pos-semi-column)
	    (if (genom-indent-right-column pos-semi-column)
		(goto-char init-pos-point))
	  ;; Le curseur est apres le ":"  
	  ;; on tabule et on remet le curseur a une position a peu 
	  ;; pres coherente au dela de la colonne
	  (if (genom-indent-right-column pos-semi-column)
	      (skip-chars-forward "<>a-zA-Z\-;_,"))
	  )

      ;; else no ":" then indent to right all the line
      ;; iff the previous line is a genom field
      ;; For input_info parameters, works well if the first parameter
      ;; is in a line bellow input_info field...
      ;; XXXXX: pb: sets systematically the buffer status as modified !!
      ;; XXXXX      even if there's nothing to do !! Probably a bad
      ;; XXXXX      interaction with the initial c-indent procedure
      (save-excursion
	;; test it is NOT the end of a genom structure
	(beginning-of-line)
	(skip-chars-forward " \t")
	(if (looking-at "}")()
	  (save-excursion
	    ;; test it is the next line of a genom-field line
	    (forward-line -1)
	    (beginning-of-line)
	    (skip-chars-forward " \t")
	    (if (looking-at genom-fields)
		;; then indent needed
		(save-excursion
		  (forward-line 1)
		  (beginning-of-line) 
		  (skip-chars-forward " \t")
		  (if (eq (point) genom-tab-right-column)()
		    (genom-indent-right-column (point))
		    )		  
		  )))))
      )
    )
  )

;; Cette function utilisee par genom-indent-line-c-hook
;; procede a l'indentation sur la colonne genom-tab-right-column
;; a partir de la position passee en argument
;; Retourne t s'il y avait quelque chose a faire ou nil sinon
(defun genom-indent-right-column (pos-indent)
  "
Indents the right column of a genom-field. Used by genom-indent-line-c-hook."
  ; Test s'il y a quelque chose a faire
  (let (current-col)
    (save-excursion
      (goto-char pos-indent)
      (skip-chars-forward " \t" (save-excursion (end-of-line)(point)))
      (setq current-col (current-column)))
    ; Rien a faire
    (if(eq current-col genom-tab-right-column) nil
      ; On indente
      (progn
	(goto-char pos-indent)
	(delete-horizontal-space)
	(indent-to-column genom-tab-right-column)
	t)
      )
    )
  )

;------------------------------------------------------------
;
; Gestion des regions actives (extent) associees aux definitions de codels.
; Permet de chercher et ouvrir le fichier source d'un codel d'un
; clic-souris (bouton2).
;
;------------------------------------------------------------

;; On redefini un keymap local
;; pour pouvoir associer le bouton2 a la function genom-find-code-extent
;; lorsque la souris est dans une region active
(defvar genom-extent-mode-map ()
  "Keymap used in genom-mode buffers.")
(setq genom-extent-mode-map (make-sparse-keymap))
(define-key genom-extent-mode-map 'button2 'genom-find-code-extent)

;; Supprime tous les extent installes par genom-mode
(defun genom-delete-extent ()
  "
Delete all extents made by genom-make-extent"
  
  (let (ext next-ext)
    ;; On recepure le premier extent de la liste
    (setq ext (car (extent-list)))
    (while (extentp ext)
      (progn
	;; Memorise le suivant
	(setq next-ext (next-extent ext))
	;; Si extent "genom" on le supprime
	(if (eq (extent-property ext 'keymap) genom-extent-mode-map)
	    (delete-extent ext))
	;; On passe au suivant
	(setq ext next-ext)
      )
    )
)) 

(defconst genom-parse-codel (concat "\\s \*" genom-codel-fields "\\s \*\\(\\(\\w\\|_\\)\+\\)\\s \*;"))
(defconst genom-parse-request "^request\\s \+\\(\\w\+\\)\\s \+{\*\\s \*")


;; Look For de toutes les lines qui definissent des codels
;; pour en faire autant de regions actives
(defun genom-make-extent ()
  "Makes extent regions for the lines that refere to codels or requests.
Thus a button2 event on those regions will load the source file.
Works only if you keep the original codel file names."

  (interactive)

  ;; Remove previous extents
  (genom-delete-extent)

  ;; Re-built
  (save-excursion

    ;; Build extents associaed to requests
    (goto-char (point-min))
    (while (re-search-forward genom-parse-request (end-of-line) t)
      (let (beg end)
	(setq beg (match-beginning 0))
	(setq end (match-end 0))
	(if (looking-at (concat "[^}]\*" genom-parse-codel))
	    (let (extent)
	      (setq extent (make-extent beg end))
	      (set-extent-property extent 'mouse-face 'highlight)
	      (set-extent-property extent 'keymap genom-extent-mode-map)
	      ))))

    ;; Build extents associated to codels
    (goto-char (point-min))
    (while (re-search-forward genom-parse-codel (end-of-line) t)
      (let (extent)
	(setq extent (make-extent (match-beginning 0) (match-end 0)))
	(set-extent-property extent 'mouse-face 'highlight)
	(set-extent-property extent 'keymap genom-extent-mode-map)
	)))
  )



;; Identifie et ouvre le file codel/*Codels.c qui correspond au codel
;; de la region active dans laquelle on a clique avec button2.
(defun genom-find-code-extent (event)
  "Open the file of the codel on event on the current cursor line (if any).
Works only if you keep the original codel file name."
  (interactive "e")
  
  ;; On retourne dans la bonne fenetre si necessaire
  ;; et on met le cuseur là où on a cliqué
  (select-window (event-window event))
  (goto-char (event-point event))

  (let (objectname taskname startfrom endseparator)

    ;; LOOK FOR NAME DE L'OBJET ET DE LA TACHE
    (save-excursion
      (beginning-of-line)
    
      ;; Si REQUETE
      (if (looking-at genom-parse-request)
	  (progn
	    ;; On recupere son name
	    (setq objectname (buffer-substring (match-beginning 1) 
						(match-end 1) nil))
	    (setq objectname-origin objectname)
	    (setq objectname (concat "/[*]-\*\n[^\n]\*\\b" objectname "\\b"))
	    ;; On recupere le name de la tache
	    (if (looking-at "[^}]\*exec_task:\\s \+\\(\\w\+\\);")
		(setq taskname (buffer-substring (match-beginning 1) 
						 (match-end 1) nil))
	      (setq taskname "CntrlTask"))
	    
	    (setq startfrom nil)
	    (setq endseparator "/[*]-\+")))
      
      ;; sinon, si CODEL
      (if objectname ()
	
	;; On recupere son name et son type
	(if (looking-at genom-parse-codel)
	    (progn
	      ;; name du codel
	      (setq objectname (buffer-substring (match-beginning 2) 
						 (match-end 2) nil))
	      (setq objectname-origin objectname)
	      (setq objectname (concat 
				"\\<\\(ACTIVITY_EVENT\\|STATUS\\)\\W\+\\b" 
				objectname "\\b"))

	      ;; Directement un codel de controle : tache de controle
		(if (string= "c_control_func" (buffer-substring (match-beginning 1) 
						       (match-end 1) nil)) 
		  (setq taskname "CntrlTask")

		  ;; peut-etre avec l'autre nom ...
		  (if (string= "codel_control" (buffer-substring (match-beginning 1) 
								 (match-end 1) nil)) 
		      (setq taskname "CntrlTask")

		    ;; sinon, cherche la tache d'exec
		    (if (re-search-backward genom-types (point-min) t)
			;; C'est une tache d'execution
			(if (looking-at "\\<exec_task\\s \+\\(\\w\+\\)\\s \*")
			    (setq taskname (buffer-substring (match-beginning 1) 
							     (match-end 1) nil))
			  ;; Sinon ca doit etre une requete
			  (if (looking-at "\\<request[^}]\*exec_task:\\s \+\\(\\w\+\\);")
			      (setq taskname (buffer-substring (match-beginning 1) 
							       (match-end 1) nil))
			    )))
		))
	      (setq startfrom "/[*]"))))
      )

    ;; On a le name de la tache (et du codel)
    (if taskname
	(let (filename findres)
	  ;; On en deduit le name du file
	  (setq filename (concat (genom-get-module-name) taskname "Codels.c"))
	  ;; On charge le file et on cherche objectname	    
	  (setq findres (genom-find-file (concat "codels/" filename)
					 objectname))
	  ;; Si on a le file mais pas objectname
	  ;; on cherche object name dans server/codels/<filename>
	  (if (eq findres 1)
	      (if (genom-insert-code filename objectname 
				     startfrom endseparator t)
		  ()(message (concat objectname-origin " not found in " filename". Call genom.")))
	    ))
      
      ;; Pas trouve : bilan
;      (if objectname
;	  (message "Task not found"))
      )
    )
  )

;; Charge un file et cherche la string (optionel)
;; Retourne : 0 si file pas trouve
;;            1 si string pas trouve
;;            2 si string trouve
(defun genom-find-file (filename &optional string)
  "Open file FILENAME using genom-find-file-hook function. Search and center on  STRING (optional)"
  (interactive)
  (if (file-exists-p filename)
      (progn 
	(run-hook-with-args 'genom-find-file-hook filename)
	(if string
	    (progn
	      (goto-char (point-min))
	      (if (re-search-forward string (point-max) t) 
		  (progn
		    (recenter)
		    2)
		(progn
		  (message (concat string "String not found"))
		  1)))))
    (progn
      (message (concat filename " not found"))
      0)
    )
  )

(defun genom-insert-code-request (&optional request filename)
  "To be call from a codel file! Inserts the REQUEST code in the current buffer. 
Default FILENAME to look for the request is the current buffer filename.
The filename must be of the following form : <moduleName><TaskName>Codels.c"
  (interactive)

  ;; Recupere name requete
  (if request ()
    (progn
      (setq request (read-from-minibuffer "Name: " ))
      (if (string= request "")
	  (message "Sorry, I need a name ... bye")
	)))

  ;; On envoie l'insertion
  (if request
      (if (genom-insert-code (if filename filename
			       (file-name-nondirectory (buffer-file-name)))
			     (concat "/[*]-\*\n[^\n]\*\\b" request "\\b")
			     nil
			     "/[*]-\+"
			     nil)
	  ()
	(message (concat request " not found")))
    )
  )

(defun genom-insert-code (filename string startfrom endseparator confirm)
  "
Inserts code related to STRING from FILENAME (usually a codel or a request).
If STARTFROM is nil, the included region starts from the beginning of the 
comment section that precedes STRING.  Otherwise, it starts from the beginning 
of STARTFROM string that precedes STRING. Stops if failed.
If ENDSEPARATOR is nil, the included region ends with the first \"}\" after 
STRING. Otherwise, it ends with the beginning of ENDSEPARATOR string following
STRING if founded, or end of file otherwise. 
If CONFIRM is t, a confirmation is asked in mini-buffer before insertion."
  (let (filebuf beg end bilan)

    ;; File where look for
    (setq filename (concat "../.genom/codels/" filename))
    (if (file-exists-p filename) 
	(progn
	  (setq filebuf (find-file-noselect filename))
	  (goto-char (point-min) filebuf)
	  (setq bilan t))
      (message (concat filename " not found. Call genom.")))
    
    ;; Look for begining of the code
    (if bilan
	(if (re-search-forward string  nil t nil filebuf)
	    (if startfrom
		(save-excursion
		  (setq beg (re-search-backward startfrom nil t nil filebuf)))
	      (save-excursion
		(progn 
		  (setq beg (re-search-backward "/[*]" nil t nil filebuf))
		  (re-search-forward string  nil t nil filebuf))))
	  (message "Code not found. Call genom.")))
    (if beg () (setq bilan nil))
	  
    ;; Look for end
    (if bilan
	;; string terminale a parser
	(if endseparator
	    (progn 
	      (if (re-search-forward endseparator nil t nil filebuf) 
		  (setq end (match-beginning 0))
		;; no separator : goto end of file
		(setq end (point-max filebuf))))
	  ;; defaut string
	  (setq end (re-search-forward "}" nil t nil filebuf))))
    (if end () (setq bilan nil))

    ;; confirme insertion
    (if bilan
	(if confirm
	    (if (string= 
		 (read-from-minibuffer "Not found. Do you want to add it (yes/no) ? [no] " ) "yes") 
		(progn 
		  (setq confirm t) 
		  (goto-char (point-max)))
	      (setq confirm nil))
	  (setq confirm t))
      (setq confirm nil)
      )
    
    ;; Construit et insere le code
    (if confirm
	(let (code)
	  (setq code (buffer-substring beg end filebuf))
	  (insert-string  (concat "\n" code "\n\n"))
	  (re-search-backward string)
	  ))
    
    (kill-buffer filebuf)
    bilan
    )
  )


;;------------------------------------------------------------
;;
;;   Les functions asociees aux commandes GenoM:
;;       .  creation de structures GenoM
;;       .  netoyage des field optionnels
;;       .  help 
;;
;;------------------------------------------------------------


;------------------------------------------------------------
;
; Creation des structures de GenoM
;
;------------------------------------------------------------

;------------------------------------------------------------
; Creation des requetes

(defun genom-make-request ()
  "Creates the canvas for a new request"
  (interactive)
  (let (request-name request-type codel-name)
    (setq request-name (read-from-minibuffer "Request name: " ))
    (if (string= request-name "")
	(message "Sorry, I need a name ... bye")
      (progn
	(setq request-name (genom-upcase-initial request-name))
	(setq codel-name 
	      (if genom-default-codel-name 
		  (concat (genom-get-module-name) request-name) nil)) 
	(setq request-type (read-from-minibuffer 
			    "Type c (control), e (exec) or i (init) : "))
	(if (string= request-type "c")
	    (genom-make-control-request request-name codel-name)
	  (if (string= request-type "e")
	      (genom-make-exec-request request-name codel-name)
	    (if (string= request-type "i")
		(genom-make-init-request request-name codel-name)
	      (message 
	       "Sorry, you must select between c (control), e (exec) or i (init) ... bye"))
	    )
	  )))
    ))

;; Creates the canvas for a new control request 
(defun genom-make-control-request (request-name codel-name)
  "
Creates the canvas for a new control request"
  (genom-insert-struct (concat "
/*  */
request " request-name " {
     doc:                \"<some doc>\";
     type:               control;
     input:              <name>::<sdi-ref>;
     input_info:         <default-value>::\"<doc>\"" genom-etc ";
     output:             <name>::<sdi-ref>; 
     codel_control:     " (if codel-name 
			       (concat codel-name "Cntrl;") "<codel>;") "
     fail_reports:        <report-name>" genom-etc ";
     interrupt_activity:  <exec-rqst-name>" genom-etc ";
};
" )))

;; Creates the canvas for a new init request 
(defun genom-make-init-request (request-name codel-name)
  "
Creates the canvas for a new init request"
  (genom-insert-struct (concat "
/*  */
request " request-name " {
     doc:                \"<some doc>\";
     type:               init;
     exec_task:           <<exec-task-name>>; 
     input:              <name>::<sdi-ref>; 
     input_info:         <default-value>::\"<doc>\"" genom-etc ";
     codel_control:     " (if codel-name 
			       (concat codel-name "Cntrl;") "<codel>;") "
     codel_start:       " (if codel-name 
			       (concat codel-name "Start;") "<codel>;") "
     codel_main:         " (if codel-name 
			       (concat codel-name "Main;") "<codel>;") "
     codel_end:     " (if codel-name 
			       (concat codel-name "End;") "<codel>;") "
     codel_inter:   " (if codel-name 
			       (concat codel-name "Inter;") "<codel>;") "
     fail_reports:          <report-name>" genom-etc "; 
     interrupt_activity:    all;
};
" )))

;; Creates the canvas for a new exec request 
(defun genom-make-exec-request (request-name codel-name)
  "
Creates the canvas for a new exec request"
  (genom-insert-struct (concat "
/*  */
request " request-name " {
     doc:                \"<some doc>\";
     type:                exec;
     exec_task:           <<exec-task-name>>; 
     input:               <name>::<sdi-ref>; 
     input_info:          <default-value>::\"<doc>\"" genom-etc ";
     posters_input:       <struct-name>" genom-etc ";
     output:              <name>::<sdi-ref>; 
     codel_control:      " (if codel-name 
			       (concat codel-name "Cntrl;") "<codel>;") "
     codel_start:   " (if codel-name 
			       (concat codel-name "Start;") "<codel>;") "
     codel_main:         " (if codel-name 
			       (concat codel-name "Main;") "<codel>;") "
     codel_end:     " (if codel-name 
			       (concat codel-name "End;") "<codel>;") "
     codel_inter:   " (if codel-name 
			       (concat codel-name "Inter;") "<codel>;") "
     fail_reports:           <report-name>" genom-etc "; 
     interrupt_activity:    " request-name ", <exec-rqst-name>" genom-etc "; 
};
" 
)))

;------------------------------------------------------------
; Creation des posters

;; Creates the canvas for a new poster
(defun genom-make-poster ()
  "Creates the canvas for a new poster"
  (interactive)
  (let (poster-name poster-type)
    (setq poster-name (read-from-minibuffer "Poster name: " ))
    (if (string= poster-name "")
	(message "Sorry, I need a name ...")
      (progn
	(setq poster-name (genom-upcase-initial poster-name))
	(setq poster-type (read-from-minibuffer "Automatically updated poster (y/n) ?  " ))
	(if (string= poster-type "y")
	    (genom-make-auto-poster poster-name)
	  (if (string= poster-type "n")
	      (genom-make-user-poster poster-name)
	    (message "Sorry, you must select between y or n ... bye")
	    ))
	)
      )))

;; Creates the canvas for a new auto poster
(defun genom-make-auto-poster (poster-name)
  "
Creates the canvas for a new auto poster"
  (genom-insert-struct (concat "
/*  */
poster " poster-name " {
     update:             auto;
     data:               <<name>>::<<sdi-ref>>" genom-etc ";
     exec_task:          <<exec-task-name>>; 
};
" )))

;; Creates the canvas for a new user poster
(defun genom-make-user-poster (poster-name)
  "
Creates the canvas for a new user poster"
  (let (poster-type)
    (setq poster-type (read-from-minibuffer "Data from SDI (y/n) ?  " ))
    (if (string= poster-type "y")
	(genom-make-user-data-poster poster-name)
      (if (string= poster-type "n")
	  (genom-make-user-type-poster poster-name)
	(message "Sorry, you must select between y or n ... bye")
	))
    ))

;; Creates the canvas for a new user data poster
(defun genom-make-user-data-poster (poster-name)
"
Creates the canvas for a new user data poster"
(genom-insert-struct (concat "
/*  */
poster " poster-name " {
     update:             user;
     data:               <<name>>::<<sdi-ref>>" genom-etc ";
     exec_task:          <<exec-task-name>>; 
};
" )))

;; Creates the canvas for a new user type poster
(defun genom-make-user-type-poster (poster-name)
"
Creates the canvas for a new user type poster"
(let (codel-name)
    (setq codel-name 
	  (if genom-default-codel-name 
	      (concat (genom-get-module-name) "CreatePoster"
		      poster-name) "<codel>")) 

    (genom-insert-struct (concat "
/*  */
poster " poster-name " {
     update:             user;
     type:               <<struct-name>>" genom-etc ";
     exec_task:          <<exec-task-name>>; 
     codel_poster_create:      " codel-name ";
};
" ))))

;------------------------------------------------------------
; Creation des taches d'execution

(defun genom-make-exec-task ()
  "Creates the canvas for a new execution task"
  (interactive)
  (let (exec-name codel-name)
    (setq exec-name (read-from-minibuffer "Exec task name: " ))
    (if (string= exec-name "")
	(message "Sorry, I need a name ...")
      (progn
	(setq codel-name 
	      (if genom-default-codel-name 
		  (concat (genom-get-module-name) 
			  (genom-upcase-initial exec-name)) nil)) 
	(genom-insert-struct (concat "
/*  */
exec_task " (genom-upcase-initial exec-name) " {
     period:             <number>;
     delay:              <number>;
     priority:           <<number>>;
     stack_size:         <<number>>;
     posters_input:       <struct-name>" genom-etc ";
     codel_task_start:    " (if codel-name 
				(concat codel-name "Init;") "<codel>;") "
     codel_task_end:      " (if codel-name 
				(concat codel-name "End;") "<codel>;") "
     codel_task_main:     " (if codel-name 
				(concat codel-name "Perm;") "<codel>;") "
     fail_reports:         <report-name>" genom-etc ";
};
" ))))))

;------------------------------------------------------------
; Creation d'un import from

(defun genom-make-import ()
  "Creates the canvas for a new import from"
  (interactive)
  (let (import-name file-name)
    (setq import-name (read-from-minibuffer "Imported module name: " ))
    (if (string= import-name "")
	(message "Sorry, I need a name ...")
      (progn
	(setq file-name
	      (if genom-default-codel-name 
		  (concat import-name "Struct.h") "<file-name>"))
	(genom-insert-struct (concat "
import from " (downcase import-name) " {
#include \"" file-name "\"
};
" ))))))


;------------------------------------------------------------
; Creation d'un module

(defun genom-make-module ()
  "Creates the canvas for a new module"
  (interactive)
  (let (module-name MODULE-NAME minus-char-string file-name)
    (setq module-name (read-from-minibuffer "Module name: " ))
    (if (string= module-name "")
	(message "Sorry, I need a name ...")
      (progn
	(setq module-name (genom-downcase-initial module-name))
	(setq MODULE-NAME (upcase module-name))
	(setq file-name
	      (if genom-default-codel-name 
		  (concat module-name "Struct.h") "<file-name>"))
	(setq minus-char-string (make-string genom-comment-length ?-))
	(genom-insert-struct (concat 
"/*" minus-char-string "
 * 
 *                     --  Module  " MODULE-NAME "  --
 * 
 *  Description:
 *  Creation date : " (current-time-string) " 
 *  Author: " (user-full-name) "
 * 
 *" minus-char-string "*/

module " module-name " {
     number: <<number>>;
     version: \"0.1\";
     email:   <email>;
     requires: <package-or-module>" genom-etc ";
     codels_requires: <package-or-module>" genom-etc ";
     internal_data: " MODULE-NAME "_STR;
     lang:     <\"c++\">;
}; 

/*" minus-char-string "
 *	                   Structures and SDI
 *" minus-char-string "*/

#include \"" file-name "\"

typedef struct " MODULE-NAME "_STR {

} " MODULE-NAME "_STR;

/*" minus-char-string "
 *	                       Requests
 *" minus-char-string "*/
\n
/*" minus-char-string "
 *	                        Posters
 *" minus-char-string "*/
\n
/*" minus-char-string "
 *	                   Execution Tasks
 *" minus-char-string "*/
\n"
))))))

;------------------------------------------------------------
;
; Clean-up and others controls 
;
;------------------------------------------------------------

;------------------------------------------------------------
; Look for module name
; Returns a string : the name of the genom module
(defun genom-get-module-name ()
"
Returns a string : the name of the genom module"
  (save-excursion
    (goto-char (point-min))
    (if (re-search-forward (concat "\\<module \+\\(\\w\+\\) \*{") (point-max) t)
	(buffer-substring (match-beginning 1) (match-end 1) nil)
      nil
      )
    )
  )

;------------------------------------------------------------
; clean-up  optionnels field

(defun genom-verify-buffer ()
  "Verifies that no uninstantiated fields remain"
  (interactive)
  (goto-char (point-min)) 
  (if (re-search-forward genom-mand-var (point-max) t)
    (message (concat "warning: remaining empty mandatory field " (what-line)))
    (progn
      (goto-char (point-min)) 
      (if (re-search-forward genom-opt-var (point-max) t)
	  (message (concat "warning: remaining empty optional field " (what-line)))
	)))
  )

(defun genom-clean-structure ()
  "Deletes non instanciated optional lines of nearest genom structure"
  (interactive)
  (let (begin-region)
    (next-line 1)
    (end-of-line)
    (if (re-search-backward genom-types (point-min) t)
	(progn 
	  (setq begin-region (point))
	  (if (re-search-forward genom-end-types (point-max) t)
	      (progn
		(genom-clean-region begin-region (point))
					; on passe a la structure suivante
		(re-search-forward genom-types (point-max) t))
	    ))))
  )

;; Deletes non instanciated optional lines in region MIN MAX
(defun genom-clean-region (min max)
  "
Deletes non instanciated optional lines in region MIN MAX."
  (goto-char max)
  (while (re-search-backward (concat "[^<]" genom-opt-var "[^>]") min t)
    (genom-delete-lines 1))
  )


;; Deletes NB lines from the cursor position
(defun genom-delete-lines (nb)
  "
Deletes NB lines from the cursor position"
  (save-excursion
    (let (deb)
      (beginning-of-line) 
      (setq deb (point))
      (next-line (or (1- nb) 0))
      (end-of-line)
      (delete-region deb (1+ (point))))))

;; Insert la string en debut de line
;; puis se met en position pour editer le premier field
(defun genom-insert-struct (struct-string)
  "
Inserts STRING (a genom structure) and indents the structure."
    (let (deb)
;      (open-line 1)
;    (forward-char)
      (beginning-of-line)
      (setq deb (point)) 
      (insert-string struct-string)
      (c-indent-region deb (point))
      (goto-char deb)
      (skip-chars-forward "^<")
      )
    )

;------------------------------------------------------------
;
; Functions Utiles
;
;------------------------------------------------------------

;; Converti la premiere lettre en majuscule
;; Voila une function qui existe deja (upcase-initials) mais 
;; qui ne marche pas !!! En effet, elle functionne comme "capitalize" 
;; alors qu' elle est sensee ne pas modifier les caracteres suivants
;; la premiere lettre
(defun genom-upcase-initial (mot)
  "Upcases initial of MOT and returns MOT (upcase-initial doesn't work)."
  (concat
   (upcase (substring mot 0 1))
   (substring mot 1))
  )
(defun genom-downcase-initial (mot)
  "Downcases initial of MOT and returns MOT."
  (concat
   (downcase (substring mot 0 1))
   (substring mot 1))
  )


;------------------------------------------------------------
;
; Help
;
;------------------------------------------------------------

(defvar genom-help-map (let ((map (make-sparse-keymap)))
                   (set-keymap-name map 'genom-help-map)
                   (set-keymap-prompt
                     map (purecopy (gettext "(Type ? for further GenoM options)")))
                   map)
  "Keymap for characters following the Help key.")

;; Les trois lines suivantes permettent de prefixer les commandes de 
;; genom-help-map  par  "\C-c\C-h" en ajoutant cette sequence
;; dans global-map qui est toujours defini quelque soit le mode
;; (voir /opt/gnu/lib/xemacs-19.14/lisp/prim/keydefs.el).
;; Les functions genom-help- seraient alors directement bindees et par
;; consequent, cela eviterait de hardcoder ce prefixe dans tous les 
;; commentaires. Mais c'est genant car ces sequences seraient alors definies 
;; pour toute la session sous emacs quelque soit le mode.
;; Il aurait peut etre falu definir un mode genom-help-mode  au lieu 
;; d'utiliser help-mode (appelle dans with-displaying-help-buffer, 
;; une function de help.el) ...
;(setq genom-help-string "\C-c\C-h")
;(fset 'genom-help-command genom-help-map)
;(define-key global-map genom-help-string 'genom-help-command)

(define-key genom-help-map "?" 'genom-help-for-help)
(define-key genom-help-map "\C-h" 'genom-help-for-help)

(define-key genom-help-map "h" 'genom-help-genom-commands)
(define-key genom-help-map "m" 'genom-help-module)
(define-key genom-help-map "r" 'genom-help-request)
(define-key genom-help-map "p" 'genom-help-poster)
(define-key genom-help-map "e" 'genom-help-exec-task)
(define-key genom-help-map "i" 'genom-help-import)
(define-key genom-help-map "g" 'genom-help-generate)
(define-key genom-help-map "c" 'genom-help-codels)

(define-key genom-help-map "q" 'help-quit)

;------------------------------------------------------------
;  genom-help-for-help 

(defun genom-help-for-help ()
  "Documentation about genom-mode help.
You have typed C-c C-h, the GenoM help.  Type a Help option:

\(Use SPC or DEL to scroll through this text.  
 Type \\<genom-help-map>\\[help-quit] to exit Help.)

\\[genom-help-genom-commands]     Lists the genom-mode commands
\\[genom-help-module]     Help about module 
\\[genom-help-request]     Help about requests
\\[genom-help-poster]     Help about posters
\\[genom-help-exec-task]     Help about execution tasks
\\[genom-help-import]     Help about structures importation
\\[genom-help-generate]     Help about module generation
\\[genom-help-codels]     Help about codels editing
"
    (interactive)
  (let ((help-key (copy-event last-command-event))
	event char)
    (message (gettext "Type h m r i p e c,  or %s again for more help: ")
	     (single-key-description help-key))
    (setq event (next-command-event)
	  char (event-to-character event))
    (if (or (equal event help-key)
	    (eq char ??)
	    (eq 'genom-help-command (key-binding event)))
	(save-window-excursion
	  (switch-to-buffer "*Help*")
	  ;; #### I18N3 should mark buffer as output-translating
	  (delete-other-windows)
	  (let ((buffer-read-only nil))
	    (erase-buffer)
	    (insert (documentation 'genom-help-for-help)))
	  (goto-char (point-min))
	  (while (or (equal event help-key)
		     (eq char ??)
		     (eq 'genom-help-command (key-binding event))
		     (eq char ? )
		     (eq 'scroll-up (key-binding event))
		     (eq char ?\177)
		     (eq 'scroll-down (key-binding event)))
	    (if (or (eq char ? )
		    (eq 'scroll-up (key-binding event)))
		(scroll-up))
	    (if (or (eq char ?\177)
		    (eq 'scroll-down (key-binding event)))
		(scroll-down))
	    ;; write this way for I18N3 snarfing
	    (if (pos-visible-in-window-p (point-max))
		(message "h m r i p e c: ")
	      (message "h m r i p e c or Space to scroll: "))
	    (let ((cursor-in-echo-area t))
	      (setq event (next-command-event event)
		    char (or (event-to-character event) event))))))
    (let ((defn (or (lookup-key genom-help-map (vector event))
 		    (and (numberp char)
 			 (lookup-key genom-help-map (make-string 1 (downcase char)))))))
      (message nil)
      (if defn
 	  (call-interactively defn)
 	(ding)))))

;------------------------------------------------------------
;  genom-help-genom-commands 


(defun genom-help-genom-commands ()
  "Documentation about genom-mode commands"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"button2 (on active regions) : open the source file of the codel 
                               (if you do not have change the original 
                                codels' file name !).

C-c C-m   : creates a new module
C-c C-r   : creates a new request
C-c C-p   : creates a new poster
C-c C-e   : creates a new execution task
C-c C-i   : creates a new \"import from\"
C-c C-b   : indents the buffer
C-c C-v   : verifies that all the fields have been instantiated
C-c C-d   : deletes the optional lines of the nearest GenoM structure
C-c C-h   : help
C-button2 : refreshes active regions associated to the codels

The optional parameters are noted  <params> 
The mandatory parameters are noted  <<params>> 

These functions are also available through the menu bar or the button 3 of the mouse.
")
     ))
  )

;------------------------------------------------------------
;  genom-help-module

(defun genom-help-module ()
  "Documentation about module"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"What is a module ?
------------------
A module is a software entity that allows to integrate your functions in a 
standard structure and with standard interfaces to have access to these 
functions. 

These functions are controlled (ie, started, interrupted and parametrized)
through requests: these requests are the visible part of your
module. They may be used by an operator, by others modules or by a
supervisor to control your module functionalities.
Each request may have an input parameter (a C structure) and may return an 
output parameter (another C structure) at the end of the required service.

Your module may also make public data in posters.

All the data transmited to and from the module (through the requests and the 
posters) are recorded in a database (an internal data structure) named SDI 
(for Structure de Donnees Internes). Thus, this database is a C structure 
made of several substructures.

Your functions are executed by processes named execution tasks. You may have 
one or several execution tasks. 

To sumerize, the description of a module includes 5 parts:
  1. declaration of the module
  2. declaration of the data structures and SDI
  3. declaration of the requests
  4. declaration of the posters
  5. declaration of the execution tasks
 

How to create a new module ?
----------------------------
   1. Type the key sequence: C-c C-r
   2. Enter the module name 
   3. Now you may edit your module:

  * number:
      First of all you have to replace the mandatory field <<number>> by a 
      number to identify your module. To select a free module number (that 
      should be a multiple of 10 and greater than 2000) use the unix function:
      /opt/hilare2/src/errorLib/h2_modules_list that displays the already 
      attributed numbers.

  * SDI and #include:
      Then you will incrementally defined the module SDI (the \"typedef 
      structure\"). In particular, this SDI includes the C structures 
      manipulated by the requests and the posters (ie, their parameters).
      These structures will be defined in C header files (.h) to be include 
      before the SDI description: replace the <file-name> variable by the
      adequate name.

      You may need to use C structures defined by other modules.
      To do that refer to corresponding help (C-c C-h i)

  * Requests:
      The requests are the services offer by your module.
      To add requests refer to corresponding help (C-c C-h r)

  * Posters:
      The posters allow to export data.
      To add a poster refer to corresponding help (C-c C-h p)

  * Execution task:
      Your module needs execution task: the execution context of your 
      functions.
      To add execution task refer to corresponding help (C-c C-h e)


"))))

;------------------------------------------------------------
;  genom-help-request

(defun genom-help-request ()
  "Documentation about request"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"What is a request?
------------------
A module offers a set of services that are executed on requests: the requests 
allow to start a new service. Each request may be associated with an input 
parameter (a C structure). 

Once the service is terminated, one reply is send back to the client (the one 
that has send the request). This reply may be associated with an output
parameter (another C structure). Each reply includes a report. 

There are three types of requests: 
  * the execution requests start an activity (ie, execute one of your program).
  * the control requests control module parameters: they allow to get or to
    modify module parameters or also to interrupt an activity.
  * the init request (no more than one may be defined) is a special control 
    request: it allows to execute an initializing parametrized function at 
    the starting of the module (the module refuses all others requests while 
    this one has not been received and executed correctly).

How to create a new request ?
-----------------------------
   1. Place the cursor where you want to add the new request
   2. Type the key sequence: C-c C-r
   3. Enter the request name 
   4. Enter the request type:  c  for a control request
                               e  for an execution request
                               i  for an init request 
   5. Now you have to instantiate:
        . the mandatory fields (between << and >>), and
        . the needed optional fields (between < and >)

          You can automatically remove the uninstantiated optional 
          fields typing the key sequence: C-c C-d

How to instantiate the fields ?
-------------------------------
  * input:            Input parameter of the request, with:
                          <name>    : A name for this parameter
                          <sdi-ref> : The corresponding data in the SDI
  * output:            Output parameter of the request (same as input)
  * codel_control:    A name of a codel (a C function). This codel is often
                       used to control the validity of the input parameter 
                       before recording it in the SDI. 
  * fail_reports:      The list of all the possible reports (disfunctions).
  * interrupt_activity: The list of the activities (ie, the names of the 
                       corresponding execution requests) that will be 
                       interrupted on the reception of this request. 

                       You can also use one of the two key-words:
                          none  : (default value) no activity to interrupt
                          all   :   all current activities will be interrupted.

                       Remark: Usually (but not systematically) an execution 
                       request is incompatible with itself. For instance 
                       you can not have two motion services at a same time. 
                       In this case do not forget to put itself in this list 
                       (suggested by default).
             

And more fields for the execution requests:
-------------------------------------------
  * codel_start: Name of the corresponding codel (a C function).
                       This codel is the first one which is executed by the 
                       activity. If you do not defined it, then the 
                       c-exec-func codel will be executed first.
  * codel_main:  Name of the corresponding codel.
                       This codel generally corresponds to the main loop.
  * codel_end:   Name of the corresponding codel.
                       This codel generally corresponds to the termination 
                       of the activity (freeing of allocated memory, freeing 
                       connections, ...).
  * codel_inter: Name of the corresponding codel.
                       This codel is very important: it is the last executed 
                       codel (if defined!) when an interruption has occurred. 
                       It often refers to the same codel as the 
                       codel_end codel.
  * codel_fail:  Name of the corresponding codel.
                       This codel is usually dedicated for debugging purpose:
                       after its execution, the module is \"frozen\".
  * exec_task:         Name of the execution task that manages this activity 
                       (ie, that executes the codels).

Example:
--------
request Move {
    type:		    exec; 
    input:		    moveParameters::moveParams;
    output:                 finalPosition::robot.position;
    codel_start:	    demoMoveStart;
    codel_main:	            demoMoveMain;
    codel_end:	            demoMoveEnd;
    codel_inter:	    demoMoveInter;
    exec_task:		    MotionTask;
    interrupt_activity:	    Move, Turn;
    fail_reports:	    BAD_PARAMETERS, BUMPER_STOP, 
			    IMPORTANT_DRIFT;
};

"))))

;------------------------------------------------------------
;  genom-help-poster

(defun genom-help-poster ()
  "Documentation about poster"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"What is a poster ?
------------------
A poster is a shared memory that contains data (a C structure) updated by the
module and readable by all (eg, others modules, a supervisor, graphic
interfaces, ...). 

A poster is managed by one (and only one) execution task of the module.

There are 2 types of posters:
  * The posters \"auto\" are updated automatically each time the corresponding
    execution task wakes up (at each period for periodical tasks).
    Their data are copied from SDI data.
  * The posters \"user\" are updated by codels (managed by the corresponding
    execution task) calling a specific function.
    Their data may be copied from SDI data or may be another C structure 
    filled by the codels.
    
How to create a new poster ?
----------------------------
   1. Put the cursor where you want to add the new poster
   2. Type the key sequence: C-c C-p
   3. Enter the poster name
   4. If this poster is updated automatically (ie, each time the execution 
      task that will manage this poster wakes up), enter 'y' to the next 
      question. Otherwise enter 'n'. 

      If you have enter 'n', then you have to answer another question:
      if the data exported by the poster are a copy from data of the SDI, 
      answer 'y'. Otherwise answer 'n'. 

   5. Now you have to instantiate the mandatory fields (between << and >>):
      see below.

How to instantiate the fields ?
-------------------------------
  * data:       A list of one or more elements (separated by comma), each 
                element having the following form  <<name>>::<<sdi-ref>>  with:
                   <<name>>    : a name for this data
                   <<sdi-ref>> : the corresponding data in the SDI
  * type:       The name of a C structure definition (a \"typedef struct\").
  * exec_task:  The name of the execution task that will manage this poster.

Examples:
---------
/* An \"auto\" poster that comtaines two SDI data copies */
poster RobotState {
    update:		    auto;
    data:                   robotPos::state.position, 
                            robotSpeed::state.speed;
    exec_task:		    CmdTask;
};

/* An \"user\" poster that uses a specific data (not in the SDI) */
poster Obstacles {
    update:		    user;
    type:                   LOCO_OBSTACLES_STR;
    exec_task:		    ModelTask;
};


"))))

;------------------------------------------------------------
;  genom-help-import

(defun genom-help-import ()
  "Documentation about import"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"What is an import ?
-------------------
The requests or the posters of your module may need to use structures that
have been defined by others modules: for that you have to include the
corresponding C header files using the preprocessor directive \#include. 

However, for each structure GenoM produces specific I/O functions. In order
not to produce these functions (already produced by the other module) you must
protect your inclusion directive by an \"import from\". Moreover this 
allows to indicate from which module the file is imported and thus to find out
the I/O functions library.

How to create a new import ?
----------------------------
   1. Place the cursor where you want to add the new import (before the 
      definition of the SDI structure).
   2. Type the key sequence: C-c C-i
   3. Enter the name of the module that the structures are imported from.
   4. Now you have to replace the mandatory fields <<file-name>> which is 
      the name of the C header file that defines the needed structures.
      You may include several files.

      It is recommended to not hardcode the path of this file. 
      To generate your module, the genom option -I allows to specify this path.
      To compile your module, this path must also be defined in your Init.make
      file: the name of the awaited variable that defined this path is the name
      of the \"imported\" module in upcase characters (see example below).

Example:
--------
import from loco {
#include \"locoPostersStruct.h\"
#include \"locoRequestsStruct.h\"
}

To generate:
genom -I/opt/project/hilare/modules/loco  ...

To compile, define in your Init.make file the path variable:
LOCO = /opt/project/hilare/modules/loco
"))))

;------------------------------------------------------------
;  genom-help-exec-task

(defun genom-help-exec-task ()
  "Documentation about execution task"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"What is an execution task ?
---------------------------
The execution tasks are the processes that run your programs: they are the
execution context of the codels. 

How to create a new execution task ?
------------------------------------
   1. Place the cursor where you want to add the new execution task.
   2. Type the key sequence: C-c C-e.
   3. Enter the execution task name.
   4. Now you have to instantiate:
        . the mandatory fields (between << and >>), and
        . the needed optional fields (between < and >)
          You can automatically remove the uninstantiated optional fields 
          typing the key sequence: C-c C-d

How to instantiate the fields ?
-------------------------------
  * period: 
            The period of periodic execution task. Otherwise, remove this 
            field. This period is an integer that corresponds to the number of
            \"tics\" between two waking up. For the LAAS configuration of 
            VxWorks, one tic equals 25 milliseconds.
  * delay:  
            An integer that specifies the wakeup delay (in tics).
            This allows to 
  * priority: 
            An integer between 0 and 255 that defines the absolute priority
            of the process. 0: higher priority, 255: lower priority.
  * stack_size: 
            The size of the stack of the process (number of bytes). 
            You need an important stack if the codels executed by this 
            execution task manipulate large local variables.
            If this stack is not large enough, you will have strong problems!

            How to dimension your stack: 
            ----------------------------
            Define initially an over dimensioned stack (for instance 10000).
            Then run your module under VxWorks and call the checkStack 
            VxWorks commands: it will display sizes used by the task with:
              SIZE:    the allocated stack size, 
              CUR:     current used size, 
              HIGH:    the higher used size from the starting of the task, 
              MARGIN:  the difference between SIZE and HIGH,
                       the OVERFLOW message indicates a too small stack size.
            From these indications you will be able to dimension 
            correctly your stack (the bytes attributed to your stack can not 
            be used by others tasks). 
  * codel_task_start:  
            Name of the corresponding codel.
            This codel is called only once at the starting of the task.
            It is generally used to initialize the SDI with coherent default
            values.
  * codel_task_main:  
            Name of the corresponding codel.
            This codel is called each time the execution task is waked up (at
            each period for a periodic task). It can be assimilated to a 
            permanent activity.
  * fail_reports: 
            The list of all the reports that can be set by the permanent 
            activity (ie, by the codel codel_task_main).

Examples:
---------
/* A task for planning purpose without temporal constraints */
exec_task PlanningTask {
    priority:		    200;     /* Low priority */
    stack_size:		    8000; 
    codel_task_start:	    initSdi;
};

/* A task for motion purpose with hight temporal constraints */
exec_task MotionTask { 
    period:		    5;      /* 5 tics = 25 ms */
    delay:                  0;      /* No delay */
    priority:		    20;     /* High priority */
    stack_size:		    5000; 
    codel_task_start:	    initSdi;          /* Initialization codel */
    codel_task_main:                 computePosition;  /* A permanent activity */
    fail_reports:           CAN_NOT_COMPUTE_POSITION;
}; 


"))))

;------------------------------------------------------------
;  genom-help-generate

(defun genom-help-generate ()
  "Documentation about the generation procedure"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"How to generate your module ?
-----------------------------
(For more informations, refere to the documentation:
     /opt/hilare2/doc/genom/manuel_genom.dvi)

* The first time you generate the module foo:
   1. call genom with the option -i:
           genom -i foo.gen
   2. answer 'y' to the question \"No directory auto, install it ? (y/n)\" 
   3. compile the module with:
           gnumake

  (The path for genom is:
     /usr/local/robots/genom/bin/${MACHTYPE}-${OSTYPE})

* The following times juste call:
     gnumake

"))))

;------------------------------------------------------------
;  genom-help-codels

(defun genom-help-codels ()
  "Documentation about codels"
  (interactive)
  (with-displaying-help-buffer
   (lambda ()
     (princ 
"What is a codel ?
-----------------
(help page in construction ...)

How to write codels  ?
----------------------
(For more informations, refere to the documentation:
     /opt/hilare2/doc/genom/manuel_genom.dvi)


"))))


; LocalWords:  SDI func msg codel codels ie sdi ref genom hilare loco tics init
; LocalWords:  checkStack
