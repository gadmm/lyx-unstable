# Author : Guillaume Munch-Maccagnoni <gm@lyx.org>

Format 65

NoStyle                 Address
NoStyle                 Right_Address
NoStyle                 LyX-Code
NoStyle                 Verse

Columns                 1
Sides                   2
PageStyle               fancy
DefaultStyle            Standard
DefaultModule           theorems-ams

Provides amsthm         1
Provides xcolor         1
Provides geometry       1
Provides footmisc       1
Provides hyperref       1
Provides graphicx       1
Provides listings       1
Provides latexsym       1
Provides amsthm         1
Provides empheq         1
Provides keyval         1
Provides lastpage       1
Provides fancyhdr       1

ClassOptions
    FontSize            8|9|10|11|12
    PageStyle           fancy
    Other               a4paper
End

InsetLayout Flex:email
    LyxType custom
    ContentAsLabel 1
    Decoration Classic
    LabelString "Email"
    LatexName email
    LatexType command
    Font
        Family  Typewriter
    EndFont
    LabelFont
        Color Blue
        Shape Italic
        Family Roman
    EndFont
    Spellcheck 0
End

InsetLayout Flex:inst_(for_Author)
    LyxType custom
    ContentAsLabel 1
    Decoration Classic
    LabelString "inst"
    LatexName inst
    LatexType command
    LabelFont
        Color           latex
    EndFont
End

Counter institute
End
Counter author
End

# for \Collect@Body
AddToPreamble
  \RequirePackage{environ}
EndPreamble

Style Author
    Category            FrontMatter
    LatexType           Item_Environment
    LatexName           lyxauthor
    ItemCommand         and
    Preamble
        \def\@lyxauthor#1{\author{\@gobble#1}}
        \newenvironment{lyxauthor}{\Collect@Body\@lyxauthor}{}
    EndPreamble
    NextNoIndent        1
    ToggleIndent        Never
    Align               Left
    LabelType           Static
    LabelCounter        author
    ResumeCounter       0
    LabelString         "Author \theauthor "
    LabelFont
        Color           blue
    EndFont
    Margin              Dynamic
End

Style Institute
    CopyStyle           Author
    LatexName           lyxinstitute
    Preamble
        \def\@lyxinstitute#1{\institute{\@gobble#1}}
        \newenvironment{lyxinstitute}{\Collect@Body\@lyxinstitute}{}
    EndPreamble
    LabelCounter        institute
    LabelString         "Institute \theinstitute "
End

# From llncs.layout by Matthias Ettrich, Pascal André, and others.
Style Running_LaTeX_Title
    Category              FrontMatter
    Margin                Static
    InTitle               1
    LatexType             Command
    LatexName             titlerunning
    NextNoIndent          1
    LeftMargin            "Running title:xxx"
    LabelSep              xxx
    ParSkip               0.4
    TopSep                1.3
    BottomSep             0.7
    ParSep                0.7
    Align                 Left
    LabelType             Static
    LabelString           "Running Title:"
    LabelFont
        Series            Bold
        Shape             Italic
        Color             Red
    EndFont
End

Style Author_Running
    CopyStyle             Running_LaTeX_Title
    LatexName             authorrunning
    LeftMargin            "Author Running:xxx"
    LabelString           "Author Running:"
End
