# Definition of flex insets for beamer-flex
# Author: Guillaume Munch-Maccagnoni <gm@lyx.org>
# Based on beamer.layout by Till Tantau and others.

# This file is included by beamer-flex.layout and beamer-flex.module

Format 60

InsetLayout Flex:Frame
  LatexType        Environment
  LatexName        frame
  LabelString      "Frame"
  LyxType          custom
  Decoration       classic
  BgColor          white
  LabelFont
    Color          latex
    Family         Roman
    Size           Small
  EndFont
  MultiPar         true
  ResetsFont       true
  Argument 1
    LabelString     "Action"
    MenuString      "Overlay Specifications|v"
    Tooltip         "Overlay specifications for this frame"
    LeftDelim       <
    RightDelim      >
    PassThru        1
    Font
      Color         latex
      Family        typewriter
    EndFont
    LabelFont
      Color         latex
      Size          Small
    EndFont
  EndArgument
  Argument 2
    LabelString     "Default Overlay Specifications"
    Tooltip         "Default overlay specifications within this frame"
    LeftDelim       [<
    RightDelim      >]
    PassThru        1
    Font
      Color         latex
      Family        typewriter
    EndFont
    LabelFont
      Color         latex
      Size          Small
    EndFont
  EndArgument
  Argument 3
    MenuString      "Frame Options"
    LabelString     "Options"
    Tooltip         "Frame options (see beamer manual)"
  EndArgument
  Argument 4
    LabelString    "Frame Title"
    Tooltip        "Enter the frame title here"
    LeftDelim      {
    RightDelim     }
    Decoration     conglomerate
    Font
      Color        blue
      Size         largest
    EndFont
    AutoInsert     1
  EndArgument
End


InsetLayout Flex:Frame_(plain)
  CopyStyle Flex:Frame
  LabelString       "Frame (plain)"
  Argument 3
    MenuString      "Frame Options"
    LabelString     "Options"
    Tooltip         "Frame options (see beamer manual)"
    PresetArg       "plain"
  EndArgument
End


InsetLayout Flex:Frame_(fragile)
  CopyStyle Flex:Frame
  LabelString	"Frame (fragile)"
  Argument 3
    MenuString      "Frame Options"
    LabelString     "Options"
    Tooltip         "Frame options (see beamer manual)"
    PresetArg       "fragile"
  EndArgument
End


InsetLayout Flex:Columns
  LatexType        Environment
  LyxType          custom
  Decoration       classic
  BgColor          white
  LabelFont
    Color          latex
    Family         Roman
    Size           Small
  EndFont
  MultiPar         true
  ResetsFont       true
  LatexName        columns
  LabelString      "Columns"
  Argument 1
    LabelString    "Column Placement Options"
    Tooltip        "Column placement options (t, T, c, b)"
  EndArgument
End


# Remove "increase depth!" recommendation which is now bogus
Style Column
  LabelString      "Start column, width:"
  LeftMargin       "Start column, width:xx"
  PassThru         1
  Font
    Color          latex
    Family         typewriter
    Size           small
  EndFont
  KeepEmpty        1
End


InsetLayout Flex:Block
  LatexType        Environment
  LyxType          custom
  Decoration       classic
  BgColor          white
  LabelFont
    Color          latex
    Family         Roman
    Size           Small
  EndFont
  MultiPar         true
  ResetsFont       true
  LatexName        block
  LabelString      "Block"
  Argument 1
    LabelString    "Action"
    MenuString     "Action Specification|S"
    Tooltip        "Specify the overlay settings (see beamer manual)"
    LeftDelim      <
    RightDelim     >
    PassThru        1
    Font
      Color         latex
      Family        typewriter
    EndFont
    LabelFont
      Color         latex
      Size          Small
    EndFont
  EndArgument
  Argument 2
    LabelString    "Block Title"
    Tooltip        "Enter the block title here"
    Mandatory      1
    Decoration     conglomerate
    Font
      Color        blue
      Size         large
    EndFont
    AutoInsert     1
  EndArgument
End

InsetLayout Flex:Example_Block
  CopyStyle        Flex:Block
  LatexName        exampleblock
  LabelString      "Example Block"
  Font
    Color          green
  EndFont
End

InsetLayout Flex:Alert_Block
  CopyStyle        Flex:Block
  LatexName        alertblock
  LabelString      "Alert Block"
  Font
    Color          red
  EndFont
End


Style Part
  NextNoIndent 0
End

Style Section
  NextNoIndent 0
End

Style Section*
  NextNoIndent 0
End

Style Subsection
  NextNoIndent 0
End

Style Subsection*
  NextNoIndent 0
End

Style Corollary
   KeepEmpty   0
End

Style Definition
   KeepEmpty   0
End

Style Definitions
   KeepEmpty   0
End

Style Example
   KeepEmpty   0
End

Style Examples
   KeepEmpty   0
End

Style Fact
   KeepEmpty   0
End

Style Lemma
   KeepEmpty   0
End

Style Proof
   KeepEmpty   0
End

Style Theorem
   KeepEmpty   0
End