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

// Copyright 2002-2009, Daniel Fontijne, University of Amsterdam -- fontijne@science.uva.nl

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <sstream>
#include <string>

#include "../osdep.h"
#include "../state.h"
#include "consolelangparser.h"
#include "consolescope.h"
#include "consolestatement.h"

/*
Read a source file, parses it, and adds possible functions to the global scope.
*/

int state::consoleSourceFileRead(const char *filename)
{
    consoleStmtListStmt *sl = NULL;

    cprintf("Parsing '%s'", filename);

    setWorkingDirectory(filename, true);

    std::ifstream input(filename);
    if (!input)
    {
        cprintf("\nstate::consoleSourceFileRead(): could not open '%s'\n", filename);
        return -1;
    }
    std::ostringstream source;
    source << input.rdbuf();

    // CRLF -> LF on every platform (text mode does this on Windows only; the
    // book figures have CRLF line endings, and a '\r' inside an operator token
    // would be rejected by consoleStatement::setOp())
    std::string text = source.str();
    std::size_t length = 0;
    for (std::size_t i = 0; i < text.size(); i++)
        if ((text[i] != '\r') || (i + 1 == text.size()) || (text[i + 1] != '\n'))
            text[length++] = text[i];
    text.resize(length);

    sl = consoleParseSource(text, filename);

    if (sl)
    {
        // consoleVariable *cv = sl->execute(g_state->m_globalScope);
        //  if (cv) delete cv;
        g_state->m_globalScope->execStmtList(sl);
        sl = NULL;
        return 0;
    }

    return -1;
}

int state::consoleSourceDirRead(const char *dirname)
{
    u_dirEnt *dir = u_dirOpen(dirname, NULL);
    if (dir == NULL)
    {
        cprintf("state::consoleSourceDirRead(): Could not open directory '%s'\n", dirname);
        return -1;
    }

    cprintf("Reading all .g files in '%s'\n", dirname);

    char filename[1024], fullname[1024];
    int t, l;

    while ((t = u_dirNext(dir, filename, 1023)) != U_DIR_END)
    {
        if (filename[0] == '.')
            continue;
        u_dirFullName(dir, filename, fullname, 1023);
        if (t == U_DIR_DIR)
        {
            consoleSourceDirRead(fullname);
        }
        else if (t == U_DIR_FILE)
        {
            l = strlen(fullname);
            if ((l > 1) && (!strcmpnc(fullname + l - 2, ".g")))
                consoleSourceFileRead(fullname);
        }
    }

    u_dirClose(dir);

    return 0;
}
