/** @file parser.h - @brief Parser */
/*

   Copyright 2018-2020 Zhang Maiyun.

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

/** Valid psh tokens. */
enum _psh_tokens
{
    IF,                  /* if */
    THEN,                /* then */
    ELSE,                /* else */
    ELIF,                /* elif */
    FI,                  /* fi */
    CASE,                /* case */
    ESAC,                /* esac */
    FOR,                 /* for */
    SELECT,              /* select */
    WHILE,               /* while */
    UNTIL,               /* until */
    DO,                  /* do */
    DONE,                /* done */
    FUNCTION,            /* function */
    COPROC,              /* coproc */
    IN,                  /* in */
    BANG,                /* ! */
    TIME,                /* time */
    WORD,                /* whatever */
    ASSIGNMENT,          /* x=y */
    NUMBER,              /* 1234567890 */
    ARITH_CMD,           /*  */
    ARITH_FOR_EXPRS,     /*  */
    AND_AND,             /* && */
    OR_OR,               /* || */
    GREATER_GREATER,     /* >> */
    LESS_LESS,           /* << */
    LESS_AND,            /* <& */
    LESS_LESS_LESS,      /* <<< */
    GREATER_AND,         /* >& */
    SEMI_SEMI,           /* ;; */
    SEMI_AND,            /* ;& */
    SEMI_SEMI_AND,       /* ;;& */
    LESS_LESS_MINUS,     /* <<- */
    AND_GREATER,         /* &> */
    AND_GREATER_GREATER, /* &>> */
    LESS_GREATER,        /* <& */
    GREATER_BAR,         /* >- */
    BAR_AND,             /* -& */
};

/** State of the parser. */
struct _psh_parser_state
{
    /** Result. */
    struct _psh_job *result;
    /** The job we are operating on. */
    struct _psh_job *current_job;
    /** The command we are operating on. */
    struct _psh_cmd *current_cm;
    /** Whether more input is required. */
    int need_input;
    /** If requiring more input, the token that is asking for more tokens. */
    enum _psh_tokens incomplete_token;
    /** If requiring more input, the faulty token as a string literal. */
    char *faulty_token;
};

int expand_and_parse(psh_state *state, const char *buffer,
                     struct _psh_parser_state **pstate);