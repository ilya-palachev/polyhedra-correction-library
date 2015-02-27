%
%   This file provides LaTeX document style 'lomonosov'
%
%   File version: 1.0
%   Last revision: 08/12/2014
%

%
%   General info
%

\NeedsTeXFormat{LaTeX2e}[1994/12/01]
\ProvidesClass{lomonosov}
\LoadClass[openany, twoside, a5paper, 10pt]{extbook}
\RequirePackage[T2A]{fontenc}
\usepackage[utf8]{inputenc}
\usepackage[english, russian]{babel}

%
%	Pictures: vector vs. raster
%

% \DeclareOption{UP}{\RequirePackage[pdftex]{graphicx}}

\newif\ifmypackage@usePics
\DeclareOption{usePics}{\mypackage@usePicstrue}
\DeclareOption{noUsePics}{\mypackage@usePicsfalse}

\ExecuteOptions{noUsePics} % default is noUsePics

\ProcessOptions\relax

\ifmypackage@usePics
  \RequirePackage[pdftex]{graphicx}
\fi

%
%	Text formatting
%
\usepackage{indentfirst}
\usepackage{setspace}
\usepackage{mdwlist}
\usepackage{float}
\usepackage{url}

\frenchspacing
\sloppy

%
%	Math
%
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{amsthm}
\usepackage{amscd}
\usepackage{bm}
% ---------------------------------------------------------------------------
%	a5paper: 148 x 210
%	print area: 110 x 180
%	top = bottom = (210 - 180) / 2 = 15
%	left = right = (148 - 110) / 2 = 19
\special{papersize=148mm,210mm}
\usepackage[%
   left=1.9cm,% 1.9
    top=1.5cm,% 1.5 
    right=1.9cm,% 1.9
    bottom=1.5cm,% 1.5
    headsep=0.2cm,%
    footskip=0.5cm,%
    includehead,%
    includefoot]{geometry}
\textwidth 110mm
\textheight 170mm
\linespread{1.00}
%
%	Macros
%
\newcommand{\Booktitle}{Тезисы конференции <<Ломоносов --- 2015>>}

\newcommand{\const}{\mathop{\mathrm{const}}}
\newcommand{\bbr}{\mathbb{R}}
\newcommand{\scalar}[2]{\left<#1,#2\right>}
\newcommand{\norm}[1]{\left\lVert #1 \right\rVert}
\renewcommand{\phi}{\varphi}
\DeclareMathOperator{\conv}{conv}
\DeclareMathOperator{\sgn}{sgn}
\DeclareMathOperator{\tr}{tr}
\DeclareMathOperator{\stick}{\biggl|}
\DeclareMathOperator{\Argmin}{Argmin}

%
%	Fancyheader settings
%
\usepackage{fancyhdr}
\lhead{}
\rhead{}
\lfoot{}
\cfoot{\thepage}
\rfoot{}
\pagestyle{fancy}

%
%	Misc new commands
%

\newcommand{\ENGLISH}[1]{#1}%\selectlanguage{english}{#1}\selectlanguage{russian}}
\newcommand*{\HM}[1]{#1\nobreak\discretionary{}{\hbox{\(\mathsurround=0pt #1\)}}{}}

\newcommand{\processlist}[3][\relax]{% 
  \def\listfinish{#1}% 
  \long\def\listact{#2}% 
  \processnext#3\listfinish} 

\newcommand{\processnext}[1]{% 
  \ifx\listfinish#1\empty\else\listact{#1}\expandafter\processnext\fi} 

%
%	Theorems
% 

\theoremstyle{plain}
\newtheorem{theorem}{Теорема}
\newtheorem{lemma}{Лемма}
\newtheorem{corollary}{Следствие}

\theoremstyle{plain}
\newtheorem{definition}{Определение}

%
%	Thesis view commands
%

\newcommand{\CurTitle}{TITLE}

\newcommand\Title[2]{%		{Title}{Ivanov I.I.}
    \par
    \begin{centering}
        \smallskip
        {
            \large
            \textbf{\textsc{#1}}\nopagebreak \\
        }
    \end{centering}
    \nopagebreak
   % \addcontentsline{toc}{section}{ {#1} (\textit{#2})}
    \processlist{\index}{#2}
    \renewcommand{\CurTitle}{#1}
    \@afterheading 
    \smallskip
    \nopagebreak 
}
\makeatother

%
%
%


\makeatletter
\newcommand\Author[6]{% 1{Ivanov Ivan Ivanovich}2{Student}3{CMC MSU}4{Moscow}5{Russia}6{ivan@mail.ru}
	\begin{center}
	\par {\textit{\textbf{#1}}\\
	{\footnotesize \textit{#2}}}\\
	{\footnotesize \textit {#3, #4, #5}} \\
	{\footnotesize \textit{E-mail:} \texttt{#6}}\\
    \end{center}
    \addtocontents{toc}{\noindent\small\hspace{-0.4cm}\textit{#1}}
    \addcontentsline{toc}{OurOwnSection}{
					\small
					\CurTitle
				}
    \nopagebreak
}
\newcounter{local}


\newcommand\PrintAuthor[1]{%
	\textit{\textbf{#1}}$^{\text{\arabic{local}}}$\\\
	\addtocounter{local}{1}
}
\newcommand\PrintInfo[1]{%
	{\footnotesize\arabic{local}: \textit{#1}}\\\
	\addtocounter{local}{1}
}

\newcommand\AuthorM[3]{% 1{Ivanov Ivan Ivanovich}2{Student}3{CMC MSU}4{Moscow}5{Russia}6{ivan@mail.ru}
	
	\setcounter{local}{1}
	\begin{center}
	\par 
	\setcounter{local}{1} \processlist{\PrintAuthor}{#1}\\
	\setcounter{local}{1} \processlist{\PrintInfo}{#2}
	{\footnotesize \textit{E-mail:} \texttt{#3}}\\
	\end{center}
	\nopagebreak
}


%
%		Literature and so
%
\makeatletter
\newcommand{\References}{
    \smallskip
    \nopagebreak
    \centerline{\textbf{Литература}}
    \nopagebreak
    \smallskip
    \@afterheading
}
\makeatother

\newenvironment{references}{\References\begin{enumerate*}}{\end{enumerate*}}
\newcommand{\Source}{\item}


\makeatother
%
%		Pictures
%
\newcommand{\PicPath}{}
\makeatletter
\newcommand{\Pictures}{
    \smallskip
    \centerline{\textbf{Иллюстрации}}
    \nopagebreak
    \smallskip
    \@afterheading
}
\makeatother

\makeatletter
\newcommand{\Picture}[3]{%
\begin{figure}[H]
  \begin{center}
  \includegraphics[keepaspectratio, width=#3\textwidth]{\PicPath#1}
  
    {\small #2}
  
  \end{center}
\end{figure}
}
\makeatother

%
%
%
\newenvironment{thesis}{\setcounter{theorem}{0}\setcounter{definition}{0}\setcounter{lemma}{0}\setcounter{equation}{0}\chead[Текущая секция]{\Booktitle}\begin{document}}{\stop}
