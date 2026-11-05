// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef _CONSOLE_LANG_PARSER_H_
#define _CONSOLE_LANG_PARSER_H_

#include <string>
#include <string_view>

class consoleStmtListStmt;

/*
Hand-written lexer and recursive descent parser for the GAViewer console
language (replaces the ANTLR 2 generated ConsoleLexer/ConsoleParser and
builds exactly the same statement trees).

Parses 'source' (console input or the contents of a .g file) and returns the
statement list, or NULL if there is nothing (valid) to execute. 'filename' is
only used in error messages (empty for console input).

Syntax errors are reported through cprintf(); the offending top-level
statement is skipped and parsing resumes with the next one.

Identifiers are classified as functions or variables using the global scope
(and the scopes of the functions being parsed), exactly like the ANTLR lexer
did: function/batch declarations and 'variable' statements update that
classification while parsing.
*/
consoleStmtListStmt *consoleParseSource(std::string_view source, const std::string &filename = std::string());

#endif /* _CONSOLE_LANG_PARSER_H_ */
