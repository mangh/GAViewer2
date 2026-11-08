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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>

#include "../state.h"
#include "console.h"
#include "consolelangparser.h"
#include "consolescope.h"
#include "consolestatement.h"

void g_consoleCommandCallback(const char *cmd, void *cbArg)
{
    if ((cbArg == NULL) || (cmd == NULL))
        return;

    // trim right
    size_t l = strlen(cmd);
    while ((l > 0) && isspace(static_cast<unsigned char>(cmd[l - 1])))
        l--;
    std::string buf(cmd, l);

    // if the last character is not a ',' or ';', then append ','
    // (an empty command is passed on as is: it resumes a suspended statement list)
    if ((l > 0) && (buf.back() != ',') && (buf.back() != ';'))
        buf += "\n,";

    ((state *)cbArg)->consoleCommandCallback(buf.c_str());
}

consoleStmtListStmt *consoleParse(const char *cmd, const char *wd /* = NULL */)
{
    if (wd)
        g_state->setWorkingDirectory(wd);
    else
        g_state->setWorkingDirectory(g_state->m_initialWorkingDirectory);

    return consoleParseSource(cmd);
}

int state::consoleCommandCallback(const char *cmd)
{
    // (a global scope replaced during the command is deleted when the outermost command has finished)
    struct depthGuard
    {
        state *s;
        explicit depthGuard(state *st) : s(st)
        {
            s->m_consoleCommandDepth++;
        }
        ~depthGuard()
        {
            if (--s->m_consoleCommandDepth == 0)
            {
                for (consoleGlobalScope *gs : s->m_retiredGlobalScopes)
                    delete gs;
                s->m_retiredGlobalScopes.clear();
            }
        }
    };

    bool reset;
    {
        depthGuard guard(this);

        consoleStmtListStmt *stmtList = consoleParse(cmd);

        if ((stmtList == NULL) || (stmtList->dummy()))
        {
            // if we have suspended stmtLists resume the top one on the stack
            //			cprintf("Dummy!\n");
            if (stmtList)
                delete stmtList;
            g_state->m_globalScope->resumeStmtList();
        }
        else
            g_state->m_globalScope->execStmtList(stmtList);

        reset = m_globalScope->resetRequested();
        if (!reset)
            g_state->updateDynStmts();
    }

    // (reset() requested by the command: after it, the command has finished)
    if (reset)
        g_state->reset();

    return 0;
}
