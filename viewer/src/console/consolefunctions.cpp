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

#include <string>

// #include <iostream>

#include "../gaincludes.h"
#include "../models.h"
#include "../state.h"
// #include "../mvint.h"
#include "console.h"
#include "consolefunction.h"
// #include "consoleoperators.h"
// #include "consoleparse.h"
#include "consolescope.h"
#include "consolestatement.h"
#include "consolevariable.h"

int consoleFreeCvList(consoleVariable *cv, consoleVariable *exempt = NULL);

// the model id of an argument spec (or CVF_ANY_MODEL)
static inline int model(int v)
{
    return v & (MVI_MODEL_MASK | CVF_ANY_MODEL);
}

// static inline int compareModel(int m1, int m2) {
// 	if ((m1 & CVF_ANY_MODEL) || (m2 & CVF_ANY_MODEL)) return 1;
// 	else return (m1 == m2);
// }

// static inline int side(int v) {
// 	return v & (CVF_RHS | CVF_LHS);
// }

consoleVariable *consoleExecUnaryFunc(consoleScope *s, const std::string &name, consoleVariable *cv)
{
    // temporarily cut the list at 'cv'
    consoleVariable *cvNext = cv->next();
    cv->next(NULL);

    // execute function
    cv = consoleExecFunc(s, name, cv);

    // restore the list
    cv->next(cvNext);

    return cv;
}

consoleVariable *consoleExecFunc(consoleScope *s_in, const std::string &name, consoleVariable *cv)
{
    int argType[maxConsoleFuncArg];
    int nbArg = 0, err, i;
    consoleVariable *cvTmp;
    const consoleFunc *f;
    consoleVariable *arg[maxConsoleFuncArg];
    consoleVariable **cvPtr, *cvCast, *cvNext;

    /*cvNext = cv;
    while (cvNext) {
        cvNext = cvNext->next();
    }*/

    // collect point and type of argument into arrays
    cvTmp = cv;
    while (cvTmp)
    {
        arg[nbArg] = cvTmp;
        argType[nbArg] = cvTmp->model() | cvTmp->lhs() | cvTmp->rhs();
        nbArg++;
        cvTmp = cvTmp->next();
    }

    // find best matching function
    if (err = s_in->lookupFunction(name, 0, nbArg, arg, argType, &f))
    { // 0 for not global (todo: not possible to force global func?)
        cprintf("Error: no function '%s' found that matches the arguments\n", name.c_str());
        consoleFreeCvList(cv);
        return (consoleVariable *)consoleVariableScalar("", 0.0);
    }

    // coerce arguments if necessary
    cvPtr = &cv;
    for (i = 0; i < nbArg; i++)
    {
        if (f->arg[i] & CVF_ANY_MODEL)
        {
            cvPtr = &(arg[i]->m_next);
            continue;
        }
        if (model(argType[i]) != model(f->arg[i]))
        {
            // cut list
            cvNext = arg[i]->next();
            arg[i]->next(NULL);

            // coerce
            if (const char *targetModel = modelName(model(f->arg[i])))
            {
                // (note: only the e3ga part of the argument is checked, so this warning is only
                // issued for e3ga arguments that are not a scalar)
                if (!(ga::norm_a(arg[i]->e()) - ga::gradeNorm_a(arg[i]->e(), 0) < 10e-6))
                {
                    if (!s_in->supressWarnings())
                    {
                        if (s_in->globalScope())
                            cprintf("Coerced '%s' to %s\n", arg[i]->name().c_str(), targetModel);
                        else
                            cprintf("Coerced '%s' to %s (in function %s)\n", arg[i]->name().c_str(), targetModel,
                                    s_in->functionName().c_str());
                    }
                }
                cvCast = consoleExecFunc(s_in, std::string("cast_") + targetModel, arg[i]);
            }
            else
            {
                if (!s_in->supressErrors())
                {
                    cprintf("Coerce '%s' failure!\n", arg[i]->name().c_str());
                }
                cvCast = (consoleVariable *)consoleVariableScalar("", 0.0);
            }

            // restore list
            *cvPtr = arg[i] = cvCast;
            cvCast->next(cvNext);
        }

        cvPtr = &(arg[i]->m_next);
    }

    // execute function (call style depends on user or predefined function)
    consoleVariable *result = NULL;
    if (f->funcPtr)
    { // predefined function
        result = f->funcPtr(s_in, arg);
    }
    else if (f->funcStmt)
    { // user function (or batch)
        if (f->funcStmt->function())
        { // function
            // create a new scope
            consoleScope *s = new consoleScope(name, s_in);
            // exec
            result = f->funcStmt->execute(s, arg); // todo: temp s_in
            //  delete
            delete s;
        }
        else
        { // batch
            if (!s_in->globalScope())
            {
                cprintf("Error, in function '%s': batch '%s' can only be called in global scope\n",
                        s_in->functionName().c_str(), f->name);
                result = new consoleVariable("", e3ga::mv(0.0));
            }
            else
            {
                // set batch name
                std::string oldFunctionName = s_in->functionName();
                s_in->functionName(name);

                // exec
                result = f->funcStmt->execute(s_in, arg);

                s_in->functionName(oldFunctionName);
                // free(oldFunctionName);
            }
        }
    }
    else
    {
        cprintf("Warning: function call '%s' failed (dummy function in scope?), returning 0.0\n", f->name);
        result = new consoleVariable("", e3ga::mv(0.0));
    }

    // everything that comes out of a function can be a 'rhs' argument
    result->rhs(1);

    // copy possible operation of first argument
    if (cv)
        result->op(cv->op());

    // free resources (but 'result' is exempt)
    consoleFreeCvList(cv, result);

    return result;
}

int consoleFreeCvList(consoleVariable *cv, consoleVariable *exempt /* = NULL */)
{
    if (cv == NULL)
        return 0;
    consoleFreeCvList(cv->next(), exempt);
    cv->next(NULL);
    if (cv != exempt)
        delete cv;
    return 0;
}

std::string getFuncStringNameWithArguments(const consoleFunc *f, int argNames)
{
    std::string nameWithArguments(f->name);

    nameWithArguments += "(";

    if (f->funcStmt)
    { // user function
        consoleFuncArgSpecListStmt *asl = f->funcStmt->argSpecList();
        consoleFuncArgSpecStmt *s;

        // add all arguments
        int i = 0;
        while (s = asl->argSpec(i))
        {
            nameWithArguments += ((i == 0) ? "" : ", ");

            if (const char *argModel = modelName(s->model()))
                nameWithArguments += std::string(argModel) + " ";
            else
                nameWithArguments += " ";
            if (argNames)
                nameWithArguments += s->name();

            i++;
        }
    }
    else
    { // hard coded function
        // add all arguments
        int i = 0;
        for (i = 0; i < f->nbArg; i++)
        {
            nameWithArguments += ((i == 0) ? "" : ", ");

            if (const char *argModel = modelName(model(f->arg[i])))
                nameWithArguments += std::string(argModel) + " ";
            else
                nameWithArguments += " ";
        }
    }
    nameWithArguments += ")";
    return nameWithArguments;
}
