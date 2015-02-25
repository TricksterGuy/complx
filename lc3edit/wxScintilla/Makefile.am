INCLUDES = $(WX_CXXFLAGS) \
			-I$(srcdir)/include \
			-I$(srcdir)/src/scintilla/include \
			-I$(srcdir)/src/scintilla/lexlib \
			-I$(srcdir)/src/scintilla/src

AM_CPPFLAGS = -DSCI_LEXER -DLINK_LEXERS -DGTK -D__WX__

noinst_LTLIBRARIES = libwxscintilla.la

libwxscintilla_la_SOURCES = src/PlatWX.cpp \
			src/ScintillaWX.cpp \
			src/wxscintilla.cpp \
			src/scintilla/lexers/LexA68k.cxx \
			src/scintilla/lexers/LexAPDL.cxx \
			src/scintilla/lexers/LexASY.cxx \
			src/scintilla/lexers/LexAU3.cxx \
			src/scintilla/lexers/LexAVE.cxx \
			src/scintilla/lexers/LexAVS.cxx \
			src/scintilla/lexers/LexAbaqus.cxx \
			src/scintilla/lexers/LexAda.cxx \
			src/scintilla/lexers/LexAsm.cxx \
			src/scintilla/lexers/LexAsn1.cxx \
			src/scintilla/lexers/LexBaan.cxx \
			src/scintilla/lexers/LexBash.cxx \
			src/scintilla/lexers/LexBasic.cxx \
			src/scintilla/lexers/LexBullant.cxx \
			src/scintilla/lexers/LexCLW.cxx \
			src/scintilla/lexers/LexCOBOL.cxx \
			src/scintilla/lexers/LexCPP.cxx \
			src/scintilla/lexers/LexCSS.cxx \
			src/scintilla/lexers/LexCaml.cxx \
			src/scintilla/lexers/LexCmake.cxx \
			src/scintilla/lexers/LexCoffeeScript.cxx \
			src/scintilla/lexers/LexConf.cxx \
			src/scintilla/lexers/LexCrontab.cxx \
			src/scintilla/lexers/LexCsound.cxx \
			src/scintilla/lexers/LexD.cxx \
			src/scintilla/lexers/LexECL.cxx \
			src/scintilla/lexers/LexEScript.cxx \
			src/scintilla/lexers/LexEiffel.cxx \
			src/scintilla/lexers/LexErlang.cxx \
			src/scintilla/lexers/LexFlagship.cxx \
			src/scintilla/lexers/LexForth.cxx \
			src/scintilla/lexers/LexFortran.cxx \
			src/scintilla/lexers/LexGAP.cxx \
			src/scintilla/lexers/LexGui4Cli.cxx \
			src/scintilla/lexers/LexHTML.cxx \
			src/scintilla/lexers/LexHaskell.cxx \
			src/scintilla/lexers/LexInno.cxx \
			src/scintilla/lexers/LexKix.cxx \
			src/scintilla/lexers/LexLisp.cxx \
			src/scintilla/lexers/LexLout.cxx \
			src/scintilla/lexers/LexLua.cxx \
			src/scintilla/lexers/LexMMIXAL.cxx \
			src/scintilla/lexers/LexMPT.cxx \
			src/scintilla/lexers/LexMSSQL.cxx \
			src/scintilla/lexers/LexMagik.cxx \
			src/scintilla/lexers/LexMarkdown.cxx \
			src/scintilla/lexers/LexMatlab.cxx \
			src/scintilla/lexers/LexMetapost.cxx \
			src/scintilla/lexers/LexModula.cxx \
			src/scintilla/lexers/LexMySQL.cxx \
			src/scintilla/lexers/LexNimrod.cxx \
			src/scintilla/lexers/LexNsis.cxx \
			src/scintilla/lexers/LexOScript.cxx \
			src/scintilla/lexers/LexOpal.cxx \
			src/scintilla/lexers/LexOthers.cxx \
			src/scintilla/lexers/LexPB.cxx \
			src/scintilla/lexers/LexPLM.cxx \
			src/scintilla/lexers/LexPO.cxx \
			src/scintilla/lexers/LexPOV.cxx \
			src/scintilla/lexers/LexPS.cxx \
			src/scintilla/lexers/LexPascal.cxx \
			src/scintilla/lexers/LexPerl.cxx \
			src/scintilla/lexers/LexPowerPro.cxx \
			src/scintilla/lexers/LexPowerShell.cxx \
			src/scintilla/lexers/LexProgress.cxx \
			src/scintilla/lexers/LexPython.cxx \
			src/scintilla/lexers/LexR.cxx \
			src/scintilla/lexers/LexRebol.cxx \
			src/scintilla/lexers/LexRuby.cxx \
			src/scintilla/lexers/LexSML.cxx \
			src/scintilla/lexers/LexSQL.cxx \
			src/scintilla/lexers/LexScriptol.cxx \
			src/scintilla/lexers/LexSmalltalk.cxx \
			src/scintilla/lexers/LexSorcus.cxx \
			src/scintilla/lexers/LexSpecman.cxx \
			src/scintilla/lexers/LexSpice.cxx \
			src/scintilla/lexers/LexTACL.cxx \
			src/scintilla/lexers/LexTADS3.cxx \
			src/scintilla/lexers/LexTAL.cxx \
			src/scintilla/lexers/LexTCL.cxx \
			src/scintilla/lexers/LexTCMD.cxx \
			src/scintilla/lexers/LexTeX.cxx \
			src/scintilla/lexers/LexTxt2tags.cxx \
			src/scintilla/lexers/LexVB.cxx \
			src/scintilla/lexers/LexVHDL.cxx \
			src/scintilla/lexers/LexVerilog.cxx \
			src/scintilla/lexers/LexVisualProlog.cxx \
			src/scintilla/lexers/LexYAML.cxx \
			src/scintilla/lexlib/Accessor.cxx \
			src/scintilla/lexlib/CharacterSet.cxx \
			src/scintilla/lexlib/LexerBase.cxx \
			src/scintilla/lexlib/LexerModule.cxx \
			src/scintilla/lexlib/LexerNoExceptions.cxx \
			src/scintilla/lexlib/LexerSimple.cxx \
			src/scintilla/lexlib/PropSetSimple.cxx \
			src/scintilla/lexlib/StyleContext.cxx \
			src/scintilla/lexlib/WordList.cxx \
			src/scintilla/src/AutoComplete.cxx \
			src/scintilla/src/CallTip.cxx \
			src/scintilla/src/Catalogue.cxx \
			src/scintilla/src/CellBuffer.cxx \
			src/scintilla/src/CharClassify.cxx \
			src/scintilla/src/ContractionState.cxx \
			src/scintilla/src/Decoration.cxx \
			src/scintilla/src/Document.cxx \
			src/scintilla/src/Editor.cxx \
			src/scintilla/src/ExternalLexer.cxx \
			src/scintilla/src/Indicator.cxx \
			src/scintilla/src/KeyMap.cxx \
			src/scintilla/src/LineMarker.cxx \
			src/scintilla/src/PerLine.cxx \
			src/scintilla/src/PositionCache.cxx \
			src/scintilla/src/RESearch.cxx \
			src/scintilla/src/RunStyles.cxx \
			src/scintilla/src/ScintillaBase.cxx \
			src/scintilla/src/Selection.cxx \
			src/scintilla/src/Style.cxx \
			src/scintilla/src/UniConversion.cxx \
			src/scintilla/src/ViewStyle.cxx \
			src/scintilla/src/XPM.cxx

noinst_HEADERS = src/PlatWX.h \
			src/ScintillaWX.h \
			src/scintilla/include/ILexer.h \
			src/scintilla/include/Platform.h \
			src/scintilla/include/SciLexer.h \
			src/scintilla/include/Scintilla.h \
			src/scintilla/include/ScintillaWidget.h \
			src/scintilla/lexlib/Accessor.h \
			src/scintilla/lexlib/CharacterSet.h \
			src/scintilla/lexlib/LexAccessor.h \
			src/scintilla/lexlib/LexerBase.h \
			src/scintilla/lexlib/LexerModule.h \
			src/scintilla/lexlib/LexerNoExceptions.h \
			src/scintilla/lexlib/LexerSimple.h \
			src/scintilla/lexlib/OptionSet.h \
			src/scintilla/lexlib/PropSetSimple.h \
			src/scintilla/lexlib/SparseState.h \
			src/scintilla/lexlib/StyleContext.h \
			src/scintilla/lexlib/WordList.h \
			src/scintilla/src/AutoComplete.h \
			src/scintilla/src/CallTip.h \
			src/scintilla/src/Catalogue.h \
			src/scintilla/src/CellBuffer.h \
			src/scintilla/src/CharClassify.h \
			src/scintilla/src/ContractionState.h \
			src/scintilla/src/Decoration.h \
			src/scintilla/src/Document.h \
			src/scintilla/src/Editor.h \
			src/scintilla/src/ExternalLexer.h \
			src/scintilla/src/FontQuality.h \
			src/scintilla/src/Indicator.h \
			src/scintilla/src/KeyMap.h \
			src/scintilla/src/LineMarker.h \
			src/scintilla/src/Partitioning.h \
			src/scintilla/src/PerLine.h \
			src/scintilla/src/PositionCache.h \
			src/scintilla/src/RESearch.h \
			src/scintilla/src/RunStyles.h \
			src/scintilla/src/ScintillaBase.h \
			src/scintilla/src/Selection.h \
			src/scintilla/src/SplitVector.h \
			src/scintilla/src/Style.h \
			src/scintilla/src/SVector.h \
			src/scintilla/src/UniConversion.h \
			src/scintilla/src/ViewStyle.h \
			src/scintilla/src/XPM.h

sdkdir=$(includedir)/codeblocks/wxscintilla/include/wx

sdk_HEADERS = include/wx/wxscintilla.h

EXTRA_DIST = $(srcdir)/Readme \
	$(srcdir)/build/*.txt \
	$(srcdir)/build/Makefile \
	$(srcdir)/build/wxscintilla.* \
	$(srcdir)/src/scintilla/*.txt \
	$(srcdir)/src/scintilla/README \
	$(srcdir)/src/scintilla/include/*.py \
	$(srcdir)/src/scintilla/include/*.iface \
	$(srcdir)/src/scintilla/src/*.py \
	$(srcdir)/src/scintilla/src/*.properties

# add upper-case copy
CLEANFILES = README
README: $(srcdir)/Readme
	cp -p $< $@
