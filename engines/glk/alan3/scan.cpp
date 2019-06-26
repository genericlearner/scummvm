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

#include "glk/alan3/scan.h"
#include "glk/alan3/lists.h"
#include "glk/alan3/dictionary.h"
#include "glk/alan3/debug.h"
#include "glk/alan3/exe.h"
#include "glk/alan3/glkio.h"
#include "glk/alan3/inter.h"
#include "glk/alan3/literal.h"
#include "glk/alan3/memory.h"
#include "glk/alan3/msg.h"
#include "glk/alan3/options.h"
#include "glk/alan3/output.h"
#include "glk/alan3/params.h"
#include "glk/alan3/readline.h"
#include "glk/alan3/term.h"
#include "glk/alan3/word.h"
#include "common/textconsole.h"

namespace Glk {
namespace Alan3 {

/* PUBLIC DATA */
bool continued = FALSE;


/* PRIVATE DATA */
static char buf[1000]; /* The input buffer */
static char isobuf[1000]; /* The input buffer in ISO */
static bool eol = TRUE; /* Looking at End of line? Yes, initially */
static char *token = NULL;


/*======================================================================*/
void forceNewPlayerInput() {
    setEndOfArray(&playerWords[currentWordIndex]);
}


/*----------------------------------------------------------------------*/
static void unknown(char tok[]) {
    char *str = strdup(tok);
    Parameter *messageParameters = newParameterArray();
	
#if ISO == 0
    fromIso(str, str);
#endif
    addParameterForString(messageParameters, str);
    printMessageWithParameters(M_UNKNOWN_WORD, messageParameters);
	deallocate(messageParameters);
    free(str);
    abortPlayerCommand();
}


/*----------------------------------------------------------------------*/
static int number(char tok[]) {
    int i;
	
    sscanf(tok, "%d", &i);
    return i;
}


/*----------------------------------------------------------------------*/
static int lookup(char wrd[]) {
    int i;
	
    for (i = 0; !isEndOfArray(&dictionary[i]); i++) {
        if (compareStrings(wrd, (char *) pointerTo(dictionary[i].string)) == 0) {
            return (i);
        }
    }
    unknown(wrd);
    return (EOF);
}


/*----------------------------------------------------------------------*/
static bool isWordCharacter(int ch) {
    return isISOLetter(ch) || isdigit(ch) || ch == '\'' || ch == '-' || ch == '_';
}

/*----------------------------------------------------------------------*/
static char *gettoken(char *txtBuf) {
    static char *marker;
    static char oldch;
	
    if (txtBuf == NULL)
        *marker = oldch;
    else
        marker = txtBuf;
    while (*marker != '\0' && isSpace(*marker) && *marker != '\n')
        marker++;
    txtBuf = marker;
    if (isISOLetter(*marker))
        while (*marker && isWordCharacter(*marker))
            marker++;
    else if (isdigit((int)*marker))
        while (isdigit((int)*marker))
            marker++;
    else if (*marker == '\"') {
        marker++;
        while (*marker != '\"')
            marker++;
        marker++;
    } else if (*marker == '\0' || *marker == '\n' || *marker == ';')
        return NULL;
    else
        marker++;
    oldch = *marker;
    *marker = '\0';
    return txtBuf;
}


/*----------------------------------------------------------------------*/
// TODO replace dependency to exe.c with injection of quitGame() and undo()
static void getLine(void) {
    para();
    do {
        statusline();
        if (header->prompt) {
            anyOutput = FALSE;
            interpret(header->prompt);
            if (anyOutput)
                printAndLog(" ");
            needSpace = FALSE;
        } else
            printAndLog("> ");

#ifdef USE_READLINE
        if (!readline(buf)) {
#else
        fflush(stdout);
        if (fgets(buf, LISTLEN, stdin) == NULL) {
#endif
            newline();
            quitGame();
        }

        getPageSize();
        anyOutput = FALSE;
        if (transcriptOption || logOption) {
             // TODO: Refactor out the logging to log.c?
			g_vm->glk_put_string_stream(logFile, buf);
			g_vm->glk_put_char_stream(logFile, '\n');
        }
        /* If the player input an empty command he forfeited his command */
#ifdef TODO
		if (strlen(buf) == 0) {
            clearWordList(playerWords);
            longjmp(forfeitLabel, 0);
        }
#else
		::error("TODO: empty command");
#endif
		
#if ISO == 0
        toIso(isobuf, buf, NATIVECHARSET);
#else
        strcpy(isobuf, buf);
#endif
        token = gettoken(isobuf);
        if (token != NULL) {
            if (strcmp("debug", token) == 0 && header->debug) {
                debugOption = TRUE;
                debug(FALSE, 0, 0);
                token = NULL;
            } else if (strcmp("undo", token) == 0) {
                token = gettoken(NULL);
                if (token != NULL) /* More tokens? */
                    error(M_WHAT);
                undo();
            }
        }
    } while (token == NULL);
    eol = FALSE;
}



/*======================================================================*/
void scan(void) {
    int i;
    int w;
	
    if (continued) {
        /* Player used '.' to separate commands. Read next */
        para();
        token = gettoken(NULL); /* Or did he just finish the command with a full stop? */
        if (token == NULL)
            getLine();
        continued = FALSE;
    } else
        getLine();
	
    freeLiterals();
    playerWords[0].code = 0; // TODO This means what?
    i = 0;
    do {
        ensureSpaceForPlayerWords(i+1);
        playerWords[i].start = token;
        playerWords[i].end = strchr(token, '\0');
        if (isISOLetter(token[0])) {
            w = lookup(token);
            if (!isNoise(w))
                playerWords[i++].code = w;
        } else if (isdigit((int)token[0]) || token[0] == '\"') {
            if (isdigit((int)token[0])) {
                createIntegerLiteral(number(token));
            } else {
                char *unquotedString = strdup(token);
                unquotedString[strlen(token) - 1] = '\0';
                createStringLiteral(&unquotedString[1]);
                free(unquotedString);
            }
            playerWords[i++].code = dictionarySize + litCount; /* Word outside dictionary = literal */
        } else if (token[0] == ',') {
            playerWords[i++].code = conjWord;
        } else if (token[0] == '.') {
            continued = TRUE;
            setEndOfArray(&playerWords[i]);
            eol = TRUE;
            break;
        } else
            unknown(token);
        setEndOfArray(&playerWords[i]);
        eol = (token = gettoken(NULL)) == NULL;
    } while (!eol);
}

} // End of namespace Alan3
} // End of namespace Glk
