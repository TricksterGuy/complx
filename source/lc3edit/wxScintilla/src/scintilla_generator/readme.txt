Generation:
-----------
These files are taken from wxWidgets SVN sources to create the interface.

The folder is...
older wxWidgets ( <=2.8 ) -> {wxWidgets.svn}\contrib\src\stc
newer wxWidgets ( > 2.8 ) -> {wxWidgets.svn}\src\stc
                         and {wxWidgets.svn}\include\wx\stc\stc.h
The files are...
- gen_iface.py (modified for C::B, sync!)
- stc.cpp.in
- stc.h.in
(Re-)Generated files will be...
- stc.h
- stc.cpp
- stc.doc

PLACE A COPY OF A RECENT SCINTILLA IN A SUBFOLDER NAMED "scintilla" TO WORK!

Calling "gen_iface.py" actually creates the interface.

This will (re-)generate ("sync") stc.h with {scintilla}\include\SciLexer.h
(starting in stc.h with the comment "// For SciLexer.h").

Compare:
stc.cpp with wxscintilla.cpp
and
stc.h with wxscintilla.h
afterwards.

Issues with interface generation:
---------------------------------
In Scintilla.iface in "SetSelectionNEnd" (around line 2000) the comma before the
closing bracket of the second parameter must be removed, thus:
set void SetSelectionNEnd=2586(int selection, position pos,)
->
set void SetSelectionNEnd=2586(int selection, position pos)
Having the comma there the line cannot be parsed as the syntax does not match
the allowed syntax of the reg-exe's
