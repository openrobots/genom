%	$LAAS$

% Copyright (c) 2000-2003 Anthony Mallet              --  Thu Oct 19 2000
% All rights reserved.
%
% Redistribution  and  use in source   and binary forms,  with or without
% modification, are permitted provided that  the following conditions are
% met:
%
%   1. Redistributions  of  source code must  retain  the above copyright
%      notice, this list of conditions and the following disclaimer.
%   2. Redistributions in binary form must  reproduce the above copyright
%      notice,  this list of  conditions and  the following disclaimer in
%      the  documentation   and/or  other  materials   provided with  the
%      distribution.
%
% THIS SOFTWARE IS PROVIDED BY THE  AUTHOR AND CONTRIBUTORS ``AS IS'' AND
% ANY  EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES   OF MERCHANTABILITY AND  FITNESS  FOR  A PARTICULAR
% PURPOSE ARE DISCLAIMED.  IN NO  EVENT SHALL THE AUTHOR OR  CONTRIBUTORS
% BE LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING,  BUT  NOT LIMITED TO, PROCUREMENT  OF
% SUBSTITUTE  GOODS OR SERVICES;  LOSS   OF  USE,  DATA, OR PROFITS;   OR
% BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
% WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
% OTHERWISE) ARISING IN ANY WAY OUT OF THE  USE OF THIS SOFTWARE, EVEN IF
% ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{show}[2001/06/20]

\gdef\showbgcolor{white}
\gdef\showbgcolorwhite{white}
\gdef\showbgcolorblack{black}
\gdef\showbgcolorsmooth{smooth}

\DeclareOption{black}{\gdef\showbgcolor{black}}
\DeclareOption{white}{\gdef\showbgcolor{white}}
\DeclareOption{smooth}{\gdef\showbgcolor{smooth}}

% Other options goes to the slides class
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{slides}}
\ProcessOptions
\LoadClass[titlepage]{slides}
\RequirePackage{aeguill,color}

\ifx\pdfoutput\@undefined\else\ifnum\pdfoutput=1
   \def\set@page@color{%
   \global\let\current@page@color\current@color%
   \expandafter\ifx\csname begindvi@color\endcsname\relax%
   \gdef\begindvi@color{%
   \pdfliteral{q \current@page@color\space 1 i -1000 -1000 2000 2000 re f Q}%
   \gdef\@begindvi{\begindvi@color}}%
   \AtBeginDocument{%
      \AtBeginDvi{\begindvi@color}%
      }%
   \fi%
   }
\fi\fi

% a stupid hack for a stupid xdvi and a stupid landscape option
% (or for a stupid me)
\AtBeginDocument{
   \AtBeginDvi{\special{papersize=\the\paperwidth,\the\paperheight}}
}


% Colors ----------------------------------------------------------------

\ifx\showbgcolor\showbgcolorblack
   \pagecolor{black}
   \definecolor{color@title}{rgb}{.7,.7,.9}
   \definecolor{color@backtitle}{rgb}{0.2,0.2,.2}

   \definecolor{color@progress}{rgb}{0.4,0.4,.4}
   \definecolor{color@progressback}{rgb}{0.2,0.2,.2}
   \definecolor{color@progresshback}{rgb}{.2,.4,.6}
   \definecolor{color@progresstback}{rgb}{.7,.7,.9}
   \definecolor{color@progressnback}{rgb}{.7,.7,.9}

   \definecolor{color@bulle}{rgb}{1.0,0.7,0.3}
   \definecolor{color@bulle@1}{rgb}{.9,.1,.1}
   \definecolor{color@bulle@2}{rgb}{.9,.1,.1}
   \definecolor{color@bulle@3}{rgb}{.9,.1,.1}
   \definecolor{color@bulle@4}{rgb}{.9,.1,.1}

   \definecolor{color@back}{rgb}{0.2,0.2,.2}
   \definecolor{color@back@1}{rgb}{1.0,.95,.7}
   \definecolor{color@back@2}{rgb}{1.0,.95,.7}
   \definecolor{color@back@3}{rgb}{1.0,.95,.7}
   \definecolor{color@back@4}{rgb}{1.0,.95,.7}

   \definecolor{color@text}{rgb}{1,1,1}
   \definecolor{color@text@1}{rgb}{.0,.0,.0}
   \definecolor{color@text@2}{rgb}{.0,.0,.0}
   \definecolor{color@text@3}{rgb}{.0,.0,.0}
   \definecolor{color@text@4}{rgb}{.0,.0,.0}
\else\ifx\showbgcolor\showbgcolorwhite
   \definecolor{color@title}{rgb}{.1,.1,.9}
   \definecolor{color@backtitle}{rgb}{1.,1.,.8}

   \definecolor{color@progress}{rgb}{.4,.4,.9}
   \definecolor{color@progressback}{rgb}{0.8,1.,.8}
   \definecolor{color@progresshback}{rgb}{.5,1.,.5}
   \definecolor{color@progresstback}{rgb}{.1,.1,.1}
   \definecolor{color@progressnback}{rgb}{.5,.5,.5}

   \definecolor{color@bulle}{rgb}{1.0,0.7,0.3}
   \definecolor{color@bulle@1}{rgb}{.9,.1,.1}
   \definecolor{color@bulle@2}{rgb}{.9,.1,.1}
   \definecolor{color@bulle@3}{rgb}{.9,.1,.1}
   \definecolor{color@bulle@4}{rgb}{.9,.1,.1}

   \definecolor{color@back}{rgb}{1.,.95,.7}
   \definecolor{color@back@1}{rgb}{1.0,.95,.7}
   \definecolor{color@back@2}{rgb}{1.0,.95,.7}
   \definecolor{color@back@3}{rgb}{1.0,.95,.7}
   \definecolor{color@back@4}{rgb}{1.0,.95,.7}

   \definecolor{color@text}{rgb}{.0,.0,.0}
   \definecolor{color@text@1}{rgb}{.0,.0,.0}
   \definecolor{color@text@2}{rgb}{.0,.0,.0}
   \definecolor{color@text@3}{rgb}{.0,.0,.0}
   \definecolor{color@text@4}{rgb}{.0,.0,.0}
\else%smooth
   \RequirePackage[bgadd]{pp4/background}

   \definecolor{color@title}{rgb}{.4,.4,.9}
   \definecolor{color@backtitle}{rgb}{0.8,0.9,1.}

   \definecolor{color@progress}{rgb}{0.3,0.3,0.3}
   \definecolor{color@progressback}{rgb}{0.8,1.,1.}
   \definecolor{color@progresshback}{rgb}{.2,.8,1.0}
   \definecolor{color@progresstback}{rgb}{.1,.1,.1}
   \definecolor{color@progressnback}{rgb}{.5,.5,.5}

   \definecolor{color@bulle}{rgb}{1.0,0.5,0.3}
   \definecolor{color@bulle@1}{rgb}{1.0,0.5,0.3}
   \definecolor{color@bulle@2}{rgb}{1.0,0.5,0.3}
   \definecolor{color@bulle@3}{rgb}{1.0,0.5,0.3}
   \definecolor{color@bulle@4}{rgb}{1.0,0.5,0.3}

   \definecolor{color@back}{rgb}{0.2,0.2,.2}
   \definecolor{color@back@1}{rgb}{1.0,.95,.7}
   \definecolor{color@back@2}{rgb}{1.0,.95,.7}
   \definecolor{color@back@3}{rgb}{1.0,.95,.7}
   \definecolor{color@back@4}{rgb}{1.0,.95,.7}

   \definecolor{color@text}{rgb}{.0,.0,.0}
   \definecolor{color@text@1}{rgb}{.0,.0,.0}
   \definecolor{color@text@2}{rgb}{.0,.0,.0}
   \definecolor{color@text@3}{rgb}{.0,.0,.0}
   \definecolor{color@text@4}{rgb}{.0,.0,.0}

   \definecolor{bg}{rgb}{0.7,0.7,0.9}
   \definecolor{bglight}{rgb}{0.95,0.95,1.0}
\fi\fi

\def\invisibleshow#1{
   \definecolor{color@bulle@#1}{rgb}{1.0,1.0,1.0}
   \definecolor{color@back@#1}{rgb}{1.0,1.0,1.0}
   \definecolor{color@text@#1}{rgb}{1.0,1.0,1.0}
}

\color{color@text}

% Title page ------------------------------------------------------------

\gdef\@context{\@latex@error{No \noexpand \context given}\@ehc}
\long\gdef\context#1{\long\gdef\@context{#1}}

\def\maketitle{
   \vspace*{50pt}\color{color@text}
   {\centering
      \@context\par
      \color{color@title}\vskip10pt\hrule
      {\large\sc\bf\@title\par}
      \vskip30pt\hrule
   }
   \color{color@text}{\raggedleft\@author\par}
   \hfill{\small\@date}
   \vfill
   \begin{center}\ifx\showbgcolor\showbgcolorblack
      \docfig[width=5cm]{LAASblack.gif}
   \else
      \docfig[width=5cm]{LAAScoul.jpg}
   \fi\end{center}
   \if@titlepage\break\fi
   \pagestyle{slide}
   \ifx\showbgcolor\showbgcolorsmooth\vpagecolor[bg]{bglight}\fi
}


% Page style ------------------------------------------------------------

% overall dimensions
\voffset-1in
\hoffset0in
\headheight0pt
\headsep0pt
\footskip5mm
\topmargin5mm
\textheight195mm

\def\ps@slide{
   \def\@oddhead{}
   \def\@evenhead{}
   \def\@evenfoot{}
   \def\@oddfoot{%
      \fontsize{10pt}{10pt}\selectfont%
      \parbox[c]{0.95\textwidth}{%
	 \ifshowprogressbar\show@makeprogressbar{0.97\textwidth}\fi}%
      \parbox[c]{0.03\textwidth}{%
	 \hfill\color{color@title}\bf\tiny\ifshowprogressbar\theslide\fi}%
   }%
}

\newenvironment{bitemize}[1]{%
   \def\labelitemi{${\color{#1}\m@th\bullet}$}%
   \def\labelitemii{${\color{#1}\m@th\cdot}$}%
   \ifnum\@itemdepth>\thr@@\@toodeep\else%
      \advance\@itemdepth\@ne%
      \edef\@itemitem{labelitem\romannumeral\the\@itemdepth}%
      \expandafter\list\csname\@itemitem\endcsname{%
	 \def\makelabel##1{\hss\llap{##1}}}%
   \fi%
}{
   \global\advance\@listdepth\m@ne\endtrivlist
}


% Progress bar ----------------------------------------------------------

\newcount\show@slidecnt
\newcount\show@partslidecnt
\newcount\show@partcnt

\newif\ifshowprogressbar
\showprogressbartrue

\gdef\show@partlist@inc{}
\gdef\show@addpart#1#2{%
   \global\advance\show@partcnt by 1%
   \advance\show@partslidecnt by -\show@slidecnt%
   \multiply\show@partslidecnt by -1%
   \immediate\write\@auxout{%
      \string\newlabel{show@part@\the\show@partcnt}{\the\show@partslidecnt}%
   }%
   \global\show@partslidecnt\show@slidecnt%
   \ifx#2\relax\else\ifx\show@partlist@inc\relax%
      \global\edef\show@partlist@inc{#1}%
   \else
      \global\edef\show@partlist@inc{\show@partlist@inc,#1}%
   \fi\fi%
}

\def\show@setslidecnt{
   \@bsphack
   \immediate\write\@auxout{\string\newlabel{show@slides}{\the\show@slidecnt}}
   \immediate\write\@auxout{\string\newlabel{show@parts}{\show@partlist@inc}}
   \@esphack
}

\def\show@getslidecnt{
   \ifx\r@show@slides\undefined
      \def\show@totalslides{1}
   \else
      \let\show@totalslides\r@show@slides
   \fi
}
\def\show@getpartlist{
   \ifx\r@show@parts\undefined
      \let\show@partlist\relax
   \else
      \let\show@partlist\r@show@parts
   \fi
}

\def\show@makeprogressbar#1{\color{color@progress}%
   \vbox{\hrule height 0.2pt\hbox to#1{%
   \def\fboxsep{0pt}%
   \sbox\@tempboxa{\show@partlist}%
   \@tempdimb\ht\@tempboxa\@tempdimc\dp\@tempboxa%
   \@tempdima#1\divide\@tempdima by\show@totalslides%
   \@tempcnta\show@slidecnt\advance\@tempcnta by -1%
   \multiply\@tempdima by\@tempcnta%
   \colorbox{color@progressback}{\hbox to\@tempdima{%
      \vrule width 0pt height\@tempdimb depth\@tempdimc%
   }}%
   \@tempdima#1\divide\@tempdima by\show@totalslides%
   \colorbox{color@progresshback}{\hbox to\@tempdima{%
      \vrule width 0pt height\@tempdimb depth\@tempdimc%
   }}%
   \hss%
   \advance\@tempdimb by 2pt\advance\@tempdimc by 2pt%
   \@tempcnta0%
   \@tempcntb\show@partcnt\advance\@tempcntb by 2%
   \@for\p@rt:=\show@partlist\do{%
      \advance\@tempcnta by 1%
      \expandafter\ifx%
	 \csname r@show@part@\the\@tempcnta\endcsname\undefined\relax\else%
         \@tempdima#1\divide\@tempdima by\show@totalslides%
	 \multiply\@tempdima by \csname r@show@part@\the\@tempcnta\endcsname%
	 \advance\@tempdima by -0.4pt%
	 \vrule width 0.4pt height\@tempdimb depth\@tempdimc%
	 \ifnum\@tempcnta<\@tempcntb%
	    \hbox to\@tempdima{\hss\color{color@progresstback}\bf\p@rt\hss}%
	 \else%
	    \hbox to\@tempdima{\hss\color{color@progressnback}\bf\p@rt\hss}%
	 \fi%
	 \ifnum\@tempcnta=1%
	    \advance\@tempdimb by -2pt\advance\@tempdimc by -2pt%
	 \fi%
      \fi%
   }%
   \advance\@tempdimb by 2pt\advance\@tempdimc by 2pt%
   \vrule width 0.4pt height\@tempdimb depth\@tempdimc%
   \hfill%
}\hrule height 0.4pt}}

\AtBeginDocument{\show@getslidecnt\show@getpartlist}
\AtEndDocument{\show@setslidecnt\show@addpart{}{}}


% Slide definitions -----------------------------------------------------

\let\show@curpart\relax

% A part: #1   #2
\newenvironment{part}[2]{%
   \show@addpart{#1}{x}\gdef\show@curpart{#1}\begin{tslide}{#2}%
}{%
   \end{tslide}%
}

% A part with a different name in the progress bar: #1   #2 (#3)
\newenvironment{partx}[3]{%
   \show@addpart{#3}{x}\gdef\show@curpart{#1}\begin{tslide}{#2}%
}{%
   \end{tslide}%
}

% A part with no slide name: #1    <empty>
\newenvironment{part*}[1]{%
   \show@addpart{#1}{x}\global\let\show@curpart\relax\begin{tslide}{#1}%
}{%
   \end{tslide}%
}

% A part with no slide name and a different name in the progress bar:
% #1    <empty> (#2)
\newenvironment{partx*}[2]{%
   \show@addpart{#2}{x}\global\let\show@curpart\relax\begin{tslide}{#1}%
}{%
   \end{tslide}%
}

% A part with no progress bar and no slide name: #1    <empty>
\newenvironment{nopart*}[1]{%
   \global\let\show@curpart\relax\begin{tslide}{#1}%
}{%
   \end{tslide}%
}

% A slide, with current part name: <part>   #1
\newenvironment{tslide}[1]{%
   \global\advance\show@slidecnt by 1%
   \begin{sslide}{#1}%
}{%
   \end{sslide}%
}

% A sub slide, with current part name (no slide count): <part>   #1
\newenvironment{sslide}[1]{%
   \def\fboxsep{4pt}%
   \begin{slide}\hbox to\hsize{\large\bf\slshape%
      \sbox\@tempboxa{(p)\show@curpart#1\par}%
      \@tempdimb\ht\@tempboxa\@tempdimc\dp\@tempboxa%
      \ifx\show@curpart\relax%
	 \@tempdima0.92\hsize%
      \else%
	 \sbox\@tempboxa{\show@curpart\par}%
	 \ifdim\wd\@tempboxa>0.65\hsize%
	    \wd\@tempboxa0.65\hsize%
	 \fi%
	 \@tempdima0.98\hsize%
	 \advance\@tempdima by -1.05\wd\@tempboxa%
	 \advance\@tempdima by -10pt%
	 \colorbox{color@backtitle}{\parbox[b]{1.05\wd\@tempboxa}{%
	    \vrule width 0pt height\@tempdimb depth\@tempdimc%
	    \color{color@title}\show@curpart\par%
	 }}%
      \fi\hfil\ifx#1\relax\else%
	 \colorbox{color@backtitle}{\hbox to \@tempdima{%
	    \vrule width 0pt height\@tempdimb depth\@tempdimc%
	    \parbox[b]{0.98\@tempdima}{\raggedleft\color{color@title}#1\par}%
	 }}%
      \fi%
   }%
}{%
   \end{slide}%
}

% A slide, with no part name: <empty>   #1
\newenvironment{tslide*}[1]{%
   \def\fboxsep{4pt}%
   \global\advance\show@slidecnt by 1%
   \begin{slide}\hbox to\hsize{\large\bf\slshape%
      \sbox\@tempboxa{Mp#1\par}%
      \@tempdimb\ht\@tempboxa\@tempdimc\dp\@tempboxa%
      \@tempdima0.75\hsize%
      \hfil\ifx#1\relax\else%
	 \colorbox{color@backtitle}{\hbox to \@tempdima{%
	    \vrule width 0pt height\@tempdimb depth\@tempdimc%
	    \parbox[b]{0.98\@tempdima}{\raggedleft\color{color@title}#1\par}%
	 }}%
      \fi%
   }%
}{%
   \end{slide}%
}

\newenvironment{boxeditem}{\colorbox{color@back}\bgroup}{\egroup}
