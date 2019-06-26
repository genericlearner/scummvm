/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "glk/alan3/location.h"
#include "glk/alan3/instance.h"
#include "glk/alan3/options.h"
#include "glk/alan3/word.h"
#include "glk/alan3/inter.h"
#include "glk/alan3/glkio.h"
#include "glk/alan3/lists.h"
#include "glk/alan3/checkentry.h"
#include "glk/alan3/debug.h"
#include "glk/alan3/memory.h"
#include "glk/alan3/dictionary.h"
#include "glk/alan3/output.h"
#include "glk/alan3/msg.h"
#include "glk/alan3/current.h"

namespace Glk {
namespace Alan3 {

/*----------------------------------------------------------------------*/
static void traceExit(int location, int dir, char *what) {
    printf("\n<EXIT %s[%d] from ",
           (char *)pointerTo(dictionary[playerWords[currentWordIndex-1].code].string), dir);
    traceSay(location);
    printf("[%d], %s:>\n", location, what);
}



/*======================================================================*/
void go(int location, int dir)
{
    ExitEntry *theExit;
    bool ok;
    Aword oldloc;

    theExit = (ExitEntry *) pointerTo(instances[location].exits);
    if (instances[location].exits != 0)
        while (!isEndOfArray(theExit)) {
            if (theExit->code == dir) {
                ok = TRUE;
                if (theExit->checks != 0) {
                    if (traceSectionOption)
                        traceExit(location, dir, "Checking");
                    ok = !checksFailed(theExit->checks, EXECUTE_CHECK_BODY_ON_FAIL);
                }
                if (ok) {
                    oldloc = location;
                    if (theExit->action != 0) {
                        if (traceSectionOption)
                            traceExit(location, dir, "Executing");
                        interpret(theExit->action);
                    }
                    /* Still at the same place? */
                    if (where(HERO, TRANSITIVE) == oldloc) {
                        if (traceSectionOption)
                            traceExit(location, dir, "Moving");
                        locate(HERO, theExit->target);
                    }
                    return;
                } else
                    error(NO_MSG);
            }
            theExit++;
        }
    error(M_NO_WAY);
}


/*======================================================================*/
bool exitto(int to, int from)
{
    ExitEntry *theExit;

    if (instances[from].exits == 0)
        return FALSE; /* No exits */
    
    for (theExit = (ExitEntry *) pointerTo(instances[from].exits); !isEndOfArray(theExit); theExit++)
        if (theExit->target == to)
            return TRUE;
    
    return FALSE;
}


/*======================================================================*/
void look(void)
{
    int i;

    /* Set describe flag for all objects and actors */
    for (i = 1; i <= header->instanceMax; i++)
        admin[i].alreadyDescribed = FALSE;

    if (anyOutput)
        para();

    setSubHeaderStyle();
    sayInstance(current.location);
    setNormalStyle();

    newline();
    capitalize = TRUE;
    if (describe(current.location))
        describeInstances();
}

} // End of namespace Alan3
} // End of namespace Glk
