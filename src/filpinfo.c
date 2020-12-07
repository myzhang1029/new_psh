/*
    psh/filpinfo.c - function to fill parse info(merges original preprocesser,
    splitbuf, parser) and some other functions for command managing.
    Copyright 2017-2020 Zhang Maiyun.

    This file is part of Psh, P shell.

    Psh is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Psh is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "command.h"
#include "filpinfo.h"
#include "libpsh/util.h"
#include "libpsh/xmalloc.h"
#include "psh.h"
#include "util.h"

/* Get the index of the last char in the buffer */
static int ignore_IFSs(char *buffer, int count)
{
    do
    {
        if (!buffer[count]) /* EOL */
            return -5;
        if (buffer[count] != ' ' && buffer[count] != '\t')
            return --count;
    } while (++count);
    return -6; /* Reaching here impossible */
}

/* Fill a command with a buffer, free() the buffer, and return the number of
 * characters processed */
int filpinfo(psh_state *state, char *buffer, struct _psh_command *info)
{
/* Report a syntax error */
#define synerr(token)                                                          \
    OUT2E("%s: syntax error near unexpected token `%s'\n", state->argv0,       \
          (token))

/* Increase cnt_buffer to the last space from buffer[cnt_buffer] */
#define ignIFS()                                                               \
    do                                                                         \
    {                                                                          \
        int tmp;                                                               \
        if ((tmp = ignore_IFSs(buffer, cnt_buffer)) == -5)                     \
            goto done;                                                         \
        cnt_buffer = tmp;                                                      \
    } while (0)

/* Increase cnt_buffer to the last space from buffer[cnt_buffer+1] */
#define ignIFS_from_next_char()                                                \
    do                                                                         \
    {                                                                          \
        int tmp;                                                               \
        if ((tmp = ignore_IFSs(buffer, ++cnt_buffer)) == -5)                   \
            goto done;                                                         \
        cnt_buffer = tmp;                                                      \
    } while (0)

/* malloc() and zero-initialize an element in argv[][] */
#define malloc_one(n) (cmd_lastnode->argv[n]) = xcalloc(MAXEACHARG, P_CS)

/* Write the current char in buffer to current command, increase cnt_return
 * only if current is neither blank nor 0 */
#define write_current()                                                        \
    do                                                                         \
    {                                                                          \
        if (stat_parsing_redirect == 0)                                        \
            cmd_lastnode->argv[cnt_argument_element][cnt_argument_char++] =    \
                buffer[cnt_buffer];                                            \
        if (stat_parsing_redirect == 1) /* for an fd */                        \
        {                                                                      \
            if (redir_lastnode == NULL)                                        \
                redir_lastnode = xcalloc(1, sizeof(struct _psh_redirect));     \
            if (!isdigit(buffer[cnt_buffer]))                                  \
            {                                                                  \
                OUT2E("%s: %c: Digit input required\n", state->argv0,          \
                      buffer[cnt_buffer]);                                     \
                cnt_return = -2;                                               \
                goto done;                                                     \
            }                                                                  \
        }                                                                      \
        if (strchr(" \t", buffer[cnt_buffer]) == NULL &&                       \
            buffer[cnt_buffer] != 0) /* current char not blank */              \
            cnt_return++;                                                      \
    } while (0) /* Make the semicolon happy */

/* Write any char to current command, increase cnt_return only if c != 0 */
#define write_char(c)                                                          \
    do                                                                         \
    {                                                                          \
        cmd_lastnode->argv[cnt_argument_element][cnt_argument_char++] = c;     \
        if (strchr(" \t", c) == NULL && c != 0)                                \
            cnt_return++;                                                      \
    } while (0)

#define escape (cnt_buffer != 0 && buffer[cnt_buffer - 1] == '\\')
#define ignore (stat_in_dquote == 1 || stat_in_squote == 1 || escape)
    /*
        escape: determine whether last character is '\\'
        ignore: determine whether a meta character should be ignored(not
       for a dollar sign)
    */
    struct _psh_command *cmd_lastnode =
        info /* The last node of the command list */;
    struct _psh_redirect *redir_lastnode = info ? info->rlist : NULL;
    int stat_in_squote = 0, stat_in_dquote = 0, stat_parsing_redirect = 0;
    int cnt_buffer = 0, cnt_argument_char = 0, cnt_argument_element = 0,
        cnt_return = 0, cnt_old_parameter = 0, cnt_first_nonIFS = 0;
    /*
    -- Variable prefixes:
        - cnt: count;
        - stat: status.
    -- Variable description:
        - stat_in_squote: whether in a '' quote;
        - stat_in_dquote: whether in a "" quote;
        - stat_parsing_redirect:
        x = 0: Not parsing for redirect;
        x = 1: Parsing for a fd (like 3>&1, 2<&7);
        x = 2: Parsing for a filename (like 1>output, 2>/dev/null);
        - cnt_buffer: count for buffer;
        - cnt_argument_char: count for current parameter element;
        - cnt_argument_element: count representing how many elements are there
    in parameter;
        - cnt_return: characters actually wrote to the command, returned;
        - cnt_old_parameter: saved cnt_argument_char for undo IFS delim;
        - cnt_first_nonIFS: the first non-IFS char in buffer.
    */
    /* The input command should be initialized in main.c, otherwise report a
     * programming error */
    if (info == NULL)
        psh_code_fault(state, __FILE__, __LINE__);
    if (state->verbose)
        OUT2E("%s\n", buffer);
    ignIFS(); /* Ignore starting spaces */
    cnt_first_nonIFS = ++cnt_buffer;
    do
    {
        switch (buffer[cnt_buffer])
        {
            case '\'':
                if (stat_in_dquote ==
                    1) /* Already in a "" quote, just write a '
                        */
                    write_current();

                else if (stat_in_squote == 1) /* Get out of the quote */
                    stat_in_squote = 0;
                else
                {
                    if (cnt_buffer == cnt_first_nonIFS)
                        stat_in_squote = 1;
                    else if (!escape)
                        stat_in_squote = 1;
                    else /* not the first char and a '\\' is there */
                        /* Write a ' */
                        write_current();
                }
                break;
            case '"':
                if (stat_in_squote == 1)
                    write_current();
                else
                {
                    if (stat_in_dquote == 1)
                        if (escape)
                            write_current();
                        else
                            stat_in_dquote = 0;
                    else if (escape)
                        write_current();
                    else
                        stat_in_dquote = 1;
                }
                break;
            case '\t':
            case ' ':
                if (ignore)
                    write_current();
                else
                {
                    ignIFS();
                    write_char(0);
                    cnt_argument_element++;
                    cnt_old_parameter = cnt_argument_char;
                    cnt_argument_char = 0;
                    malloc_one(cnt_argument_element);
                }
                break;
            case '&':
                if (ignore)
                    write_current();
                else
                {
                    if (cnt_argument_char == 0) /* Previously a blank reached */
                    {
                        cnt_argument_char = cnt_old_parameter;
                        xfree(cmd_lastnode->argv[cnt_argument_element]);
                        cmd_lastnode->argv[cnt_argument_element] = NULL;
                        cnt_argument_element--;
                    }
                    if (ignore_IFSs(buffer,
                                    cnt_buffer + 1 /* the char after & */) ==
                        -5) /* EOL */
                    {
                        /* done */
                        if (cmd_lastnode->type == 0)
                            cmd_lastnode->type = PSH_CMD_BACKGROUND; /* cmd & \0
                                                                      */
                        else
                        {
                            synerr("&");
                            cnt_return = -2;
                        }
                        goto done;
                    }
                    else if (buffer[cnt_buffer + 1] == '&')
                    {
                        cmd_lastnode->next = new_command();
                        if (cmd_lastnode->type == 0)
                            cmd_lastnode->type = PSH_CMD_RUN_AND;
                        else
                        {
                            synerr("&&");
                            cnt_return = -2;
                            goto done;
                        }
                        if (ignore_IFSs(buffer,
                                        cnt_buffer +
                                            2 /* the char after && */) ==
                            -5) /* EOL */
                        {
                            char *cmdand_buf;
                            cmdand_buf = psh_gets("> ");
                            buffer =
                                xrealloc(buffer, P_CS * (strlen(buffer) +
                                                         strlen(cmdand_buf) +
                                                         1 /* \0 */));
                            strncat(buffer, cmdand_buf, strlen(cmdand_buf));
                            xfree(cmdand_buf);
                        }
                        ++cnt_buffer;
                    }
                    else
                    {
                        cmd_lastnode->next = new_command();
                        if (cmd_lastnode->type == 0)
                            cmd_lastnode->type = PSH_CMD_BACKGROUND;
                        else
                        {
                            synerr("&");
                            cnt_return = -2;
                            goto done;
                        }
                    }
                    cmd_lastnode = cmd_lastnode->next;
                    cnt_argument_element = 0;
                    cnt_argument_char = 0;
                    ignIFS_from_next_char();
                }
                break;
            case '|':
                if (ignore)
                    write_current();
                else
                {
                    if (cnt_argument_char == 0) /* Previously a blank reached */
                    {
                        cnt_argument_char = cnt_old_parameter;
                        xfree(cmd_lastnode->argv[cnt_argument_element]);
                        cmd_lastnode->argv[cnt_argument_element] = NULL;
                    }
                    cmd_lastnode->next = new_command();
                    if (buffer[cnt_buffer + 1] == '|')
                    {
                        if (cmd_lastnode->type == 0)
                            cmd_lastnode->type = PSH_CMD_RUN_OR;
                        else
                        {
                            synerr("||");
                            cnt_return = -2;
                            goto done;
                        }
                        if (ignore_IFSs(buffer,
                                        cnt_buffer +
                                            2 /* the char after || */) ==
                            -5) /* EOL */
                        {
                            char *cmdor_buf;
                            cmdor_buf = psh_gets("> ");
                            buffer =
                                xrealloc(buffer, P_CS * (strlen(buffer) +
                                                         strlen(cmdor_buf) +
                                                         1 /* \0 */));
                            strncat(buffer, cmdor_buf, strlen(cmdor_buf));
                            xfree(cmdor_buf);
                        }
                        ++cnt_buffer;
                    }
                    else
                    {
                        if (cmd_lastnode->type == 0)
                            cmd_lastnode->type = PSH_CMD_PIPED;
                        else
                        {
                            synerr("|");
                            cnt_return = -2;
                            goto done;
                        }
                        if (ignore_IFSs(buffer, cnt_buffer +
                                                    2 /* the char after | */) ==
                            -5) /* EOL */
                        {
                            char *pipe_buf;
                            pipe_buf = psh_gets("> ");
                            buffer = xrealloc(buffer, P_CS * (strlen(buffer) +
                                                              strlen(pipe_buf) +
                                                              1 /* \0 */));
                            strncat(buffer, pipe_buf, strlen(pipe_buf));
                            xfree(pipe_buf);
                        }
                    }
                    cmd_lastnode = cmd_lastnode->next;
                    cnt_argument_element = 0;
                    cnt_argument_char = 0;
                    ignIFS_from_next_char();
                }
                break;
            case '~': /* $HOME spanding stable */
                      /* TODO: more tlide */
                if (ignore)
                {
                    write_current();
                    break;
                }
                if (buffer[cnt_buffer + 1] != 0 &&
                    buffer[cnt_buffer + 1] != '\n' &&
                    buffer[cnt_buffer + 1] != '\t' &&
                    buffer[cnt_buffer + 1] != ' ' &&
                    buffer[cnt_buffer + 1] != '/') /* ~username */
                {
                    char *username = xmalloc(P_CS * 256);
                    char *posit;
                    psh_strncpy(username, &(buffer[cnt_buffer + 1]), 256);
                    posit = strchr(username, '/');
                    if (posit != NULL)
                    {
                        while (--posit != username) /* Remove blank */
                            if (*posit != ' ' && *posit != '\t')
                            {
                                *(++posit) = 0; /* Terminate
                                           the
                                           string
                                         */
                                break;
                            }
                    }
                    else
                    {
                        int usernamelen = strlen(username);
                        for (--usernamelen; usernamelen != 0; --usernamelen)
                            if (username[usernamelen] != ' ' &&
                                username[usernamelen] != '\t')
                            {
                                username[++usernamelen] = 0; /* Terminate
                                                the
                                                string*/
                                break;
                            }
                    }
                    char *hdir = psh_backend_get_homedir_username(username);
                    if (hdir == NULL)
                    {
                        /* No such user, treat as a
                         * normal ~ as in bash */
                        write_current();
                        break;
                    }
                    psh_strncpy(cmd_lastnode->argv[cnt_argument_element], hdir,
                                4094 - cnt_argument_char);
                    cnt_buffer += strlen(username);
                    cnt_argument_char += strlen(hdir);
                    xfree(username);
                }
                else /* ~/ and ~ */
                {
                    char *hdir = psh_backend_get_homedir();
                    psh_strncpy(cmd_lastnode->argv[cnt_argument_element], hdir,
                                4094 - cnt_argument_char);
                    cnt_argument_char += strlen(hdir);
                }
                break;
            case '\\':
            {
                int case_count;
                for (case_count = 1; buffer[cnt_buffer];
                     ++case_count, ++cnt_buffer)
                {
                    if (buffer[cnt_buffer] != '\\')
                    {
                        --cnt_buffer;
                        break;
                    }
                    else /* Print the '\' at a even
                        location, and ignore the odd
                        ones, the same behavior as in
                        bash */
                    {
                        if (case_count & 1) /* Odd number */
                            continue;
                        else /* Even number */
                            write_current();
                    }
                }
                break;
            }
            case 0: /* final EOL reached*/
                if (cnt_buffer == cnt_first_nonIFS || !ignore)
                    goto done;
                /* Line: command args... \
                 */
                char *newline_buf;
                newline_buf = psh_gets("> ");
                buffer =
                    xrealloc(buffer, P_CS * (strlen(buffer) +
                                             strlen(newline_buf) + 1 /* \0 */));
                strncat(buffer, newline_buf, strlen(newline_buf));
                xfree(newline_buf);
                break;
            case '>':
                if (ignore)
                {
                    write_current();
                    break;
                }
                if (buffer[cnt_buffer + 1] != '>')
                {
                    if (redir_lastnode->type != 0)
                        psh_code_fault(state, __FILE__, __LINE__);
                    else if (buffer[cnt_buffer + 1] == '&')
                        redir_lastnode->type = PSH_REDIR_FD2FD;
                    else if (buffer[cnt_buffer + 1] == 0)
                    {
                        synerr("newline");
                        cnt_return = -2;
                        goto done;
                    }
                    else
                        redir_lastnode->type = PSH_REDIR_OUT_REDIR;
                }
                if (cnt_buffer == cnt_first_nonIFS)
                    redir_lastnode->lhs.fd = 1 /* stdout */;
                else
                {
                    if (cnt_argument_char == 0) /* Previously a blank reached */
                    {
                        cnt_argument_char = cnt_old_parameter;
                        xfree(cmd_lastnode->argv[cnt_argument_element]);
                        cmd_lastnode->argv[cnt_argument_element] = NULL;
                        cnt_argument_element--;
                    }
                    int case_count = cnt_buffer, case_buf_cnt = 0;
                    char buf[MAXLINE] = {0};
                    while (--case_count, 1 /* infinity loop */)
                    {
                        if (case_count == cnt_first_nonIFS ||
                            !isdigit(buffer[case_count]))
                        {
                            if (buf[0] == 0)
                                buf[0] = 1 /* stdout */;
                            break;
                        }
                        else /* digit */
                            buf[case_buf_cnt++] = buffer[case_count];
                    }
                    int len = strlen(buf);
                    char temp;
                    int i;
                    for (i = 0; i < len / 2; i++) /* reverse buf */
                    {
                        temp = buf[i];
                        buf[i] = buf[len - i - 1];
                        buf[len - i - 1] = temp;
                    }
                    redir_lastnode->lhs.fd = atoi(buf);
                }
                switch (buffer[cnt_buffer + 1])
                {
                    case '&':
                        ++cnt_buffer; /* Increase cnt_buffer to the '&' */
                        if (ignore_IFSs(buffer, ++cnt_buffer) ==
                            -5) /* and remove all following blanks */
                        {
                            synerr("newline"); /* if EOL met, report error */
                            cnt_return = -2;
                            goto done;
                        }
                        stat_parsing_redirect = 1; /* Parsing for fd */
                        break;
                    case ' ':
                    case '\t':
                    case '|':
                        ++cnt_buffer; /* Increase cnt_buffer to the [' ''\t''|']
                                       */
                        if (ignore_IFSs(buffer, ++cnt_buffer) ==
                            -5) /* and remove all following blanks */
                        {
                            synerr("newline"); /* if EOL met, report error */
                            cnt_return = -2;
                            goto done;
                        }
                        stat_parsing_redirect = 2; /* Parsing for filename */
                        break;
                    case '>': /* Out append */
                        if (redir_lastnode->type != 0)
                            psh_code_fault(state, __FILE__, __LINE__);
                        redir_lastnode->type = PSH_REDIR_OUT_APPN;
                        ++cnt_buffer;
                        switch (buffer[cnt_buffer + 1])
                        {
                            case '&':
                                ++cnt_buffer; /* Increase cnt_buffer to the '&'
                                               */
                                if (ignore_IFSs(buffer, ++cnt_buffer) ==
                                    -5) /* and remove all following blanks */
                                {
                                    synerr("newline"); /* if EOL met, report
                                                          error */
                                    cnt_return = -2;
                                    goto done;
                                }
                                stat_parsing_redirect = 1; /* Parsing for fd */
                                break;
                            case ' ':
                            case '\t':
                            case '|':
                                ++cnt_buffer; /* Increase cnt_buffer to the ['
                                               * ''\t''|']
                                               */
                                if (ignore_IFSs(buffer, ++cnt_buffer) ==
                                    -5) /* and remove all following blanks */
                                {
                                    synerr("newline"); /* if EOL met, report
                                                          error */
                                    cnt_return = -2;
                                    goto done;
                                }
                                stat_parsing_redirect =
                                    2; /* Parsing for filename */
                                break;
                            case '>': /* >>> */
                                synerr(">");
                                cnt_return = -2;
                                goto done;
                                break;
                            case 0: /* EOL */
                                synerr("newline");
                                cnt_return = -2;
                                goto done;
                            default:
                                stat_parsing_redirect = 2;
                        }
                        break;
                    case 0:
                        synerr("newline");
                        cnt_return = -2;
                        goto done;
                    default: /* I don't need to handle the rest */
                        stat_parsing_redirect = 2; /* Parsing for filename */
                        break;
                }
                break;
            case '`':
                /* TODO: Write command substitute code here */
                if (stat_in_squote || escape)
                    write_current();
                /* fall through */
            case '$':
                /* TODO: Write variable, variable cut,
                 * ANSI-C style escape, command substitute,
                 * arithmetic expansion code here */
                if (stat_in_squote || escape)
                    write_current();
                switch (buffer[cnt_buffer + 1])
                {
                }
                /* fall through */
            case '(':
            case ')':
            /* TODO: Write command sequence code here */
            case '<':
                /* TODO: Write input redirect, here string and here document
                 * code here */
                write_current();
                break;
            case '#':
                if (cnt_buffer == cnt_first_nonIFS ||
                    (!strchr(" \t", buffer[cnt_buffer - 1])) /* Is IFS */)
                {
                    write_current();
                    break;
                }
                if (cnt_argument_char == 0) /* Previously a blank reached */
                {
                    cnt_argument_char = cnt_old_parameter;
                    xfree(cmd_lastnode->argv[cnt_argument_element]);
                    cmd_lastnode->argv[cnt_argument_element] = NULL;
                    cnt_argument_element--;
                }
                goto done;
            case ';':
                /* TODO: Write multiple command process code here */
                if (ignore)
                    write_current();
                else
                {
                    if (cnt_argument_char == 0) /* Previously a blank reached */
                    {
                        cnt_argument_char = cnt_old_parameter;
                        xfree(cmd_lastnode->argv[cnt_argument_element]);
                        cmd_lastnode->argv[cnt_argument_element] = NULL;
                    }
                    cmd_lastnode->next = new_command();
                    if (cmd_lastnode->type == 0)
                        cmd_lastnode->type = PSH_CMD_FOREGROUND;
                    else
                    {
                        synerr(";");
                        cnt_return = -2;
                        goto done;
                    }
                    if (ignore_IFSs(buffer,
                                    cnt_buffer + 1 /* the char after ; */) ==
                        -5) /* EOL */
                    {
                        goto done; /* Ending `;', end parsing */
                    }
                    ++cnt_buffer;
                }
                cmd_lastnode = cmd_lastnode->next;
                cnt_argument_element = 0;
                cnt_argument_char = 0;
                ignIFS_from_next_char();
                do /* Remove all following `;'s. I'll mess up with case later.
                      TODO */
                {
                    if (!buffer[cnt_buffer])
                    {
                        ;
                    } /* EOL, No new command required */
                    if (buffer[cnt_buffer] != ';')
                    {
                        --cnt_buffer;
                        break;
                    }
                } while (++cnt_buffer);
                break;
            default:
                write_current();
        }
    } while (++cnt_buffer);
done:
    if (cnt_return > 0)
        write_char(0);
    xfree(buffer);
    return cnt_return;
}
