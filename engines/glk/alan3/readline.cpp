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

#include "glk/alan3/readline.h"

#include "glk/alan3/sysdep.h"
#include "glk/alan3/output.h"
#include "glk/alan3/term.h"
#include "glk/alan3/exe.h"
#include "glk/alan3/save.h"
#include "glk/alan3/location.h"

#include "glk/alan3/options.h"
#include "glk/alan3/alan3.h"
#include "glk/alan3/glkio.h"
#include "glk/alan3/resources.h"

namespace Glk {
namespace Alan3 {

#define LINELENGTH 1000

/*======================================================================

  readline()

  Read a line from the user, with history and editing

*/

/* TODO - length of user buffer should be used */
bool readline(char buffer[])
{
    event_t event;
    static bool readingCommands = FALSE;
    static frefid_t commandFileRef;
    static strid_t commandFile;
#ifdef HAVE_WINGLK
    static frefid_t logFileRef;
    INT_PTR e;
#endif

    if (readingCommands) {
        if (g_vm->glk_get_line_stream(commandFile, buffer, 255) == 0) {
			g_vm->glk_stream_close(commandFile, NULL);
            readingCommands = FALSE;
        } else {
			g_vm->glk_set_style(style_Input);
            printf(buffer);
			g_vm->glk_set_style(style_Normal);
        }
    } else {
		g_vm->glk_request_line_event(glkMainWin, buffer, 255, 0);
        /* FIXME: buffer size should be infallible: all existing calls use 256 or
           80 character buffers, except parse which uses LISTLEN (currently 100)
        */
        do
            {
                g_vm->glk_select(&event);
                switch (event.type) {
                case evtype_Arrange:
                    statusline();
                    break;
#ifdef HAVE_WINGLK
                case winglk_evtype_GuiInput:
                    switch (event.val1) {
                    case ID_MENU_RESTART:
                        restartGame();
                        break;
                    case ID_MENU_SAVE:
                        glk_set_style(style_Input);
                        printf("save\n");
                        glk_set_style(style_Normal);
                        save();
                        para();
                        printf("> ");
                        break;
                    case ID_MENU_RESTORE:
                        glk_set_style(style_Input);
                        printf("restore\n");
                        glk_set_style(style_Normal);
                        restore();
                        look();
                        para();
                        printf("> ");
                        break;
                    case ID_MENU_RECORD:
                        if (transcriptOption || logOption) {
                            glk_stream_close(logFile, NULL);
                            transcriptOption = FALSE;
                            logOption = FALSE;
                        }
                        logFileRef = glk_fileref_create_by_prompt(fileusage_InputRecord+fileusage_TextMode, filemode_Write, 0);
                        if (logFileRef == NULL) break;
                        logFile = glk_stream_open_file(logFileRef, filemode_Write, 0);
                        if (logFile != NULL)
                            logOption = TRUE;
                        break;
                    case ID_MENU_PLAYBACK:
                        commandFileRef = glk_fileref_create_by_prompt(fileusage_InputRecord+fileusage_TextMode, filemode_Read, 0);
                        if (commandFileRef == NULL) break;
                        commandFile = glk_stream_open_file(commandFileRef, filemode_Read, 0);
                        if (commandFile != NULL)
                            if (glk_get_line_stream(commandFile, buffer, 255) != 0) {
                                readingCommands = TRUE;
                                printf(buffer);
                                return TRUE;
                            }
                        break;
                    case ID_MENU_TRANSCRIPT:
                        if (transcriptOption || logOption) {
                            glk_stream_close(logFile, NULL);
                            transcriptOption = FALSE;
                            logOption = FALSE;
                        }
                        logFileRef = glk_fileref_create_by_prompt(fileusage_Transcript+fileusage_TextMode, filemode_Write, 0);
                        if (logFileRef == NULL) break;
                        logFile = glk_stream_open_file(logFileRef, filemode_Write, 0);
                        if (logFile != NULL) {
                            transcriptOption = TRUE;
                            glk_put_string_stream(logFile, "> ");
                        }
                        break;
                    case ID_MENU_ABOUT:
                        e = DialogBox(myInstance, MAKEINTRESOURCE(IDD_ABOUT), NULL, &AboutDialogProc);
                        (void)e;
                        break;
                    }
                    break;
#endif
                }
            } while (event.type != evtype_LineInput);
        if (buffer[0] == '@') {
            buffer[event.val1] = 0;
            commandFileRef = g_vm->glk_fileref_create_by_name(fileusage_InputRecord+fileusage_TextMode, &buffer[1], 0);
            commandFile = g_vm->glk_stream_open_file(commandFileRef, filemode_Read, 0);
            if (commandFile != NULL)
                if (g_vm->glk_get_line_stream(commandFile, buffer, 255) != 0) {
                    readingCommands = TRUE;
					g_vm->glk_set_style(style_Input);
                    printf(buffer);
					g_vm->glk_set_style(style_Normal);
                }
        } else
            buffer[event.val1] = 0;
    }
    return TRUE;
}

} // End of namespace Alan3
} // End of namespace Glk
