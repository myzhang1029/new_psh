/*
   token.h - parser tokens

   Copyright 2018 Zhang Maiyun.

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

enum psh_tokens
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
    ASSIGNMENT,          /* = */
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

typedef struct psh_token_stream
{
    enum psh_tokens the_token;
    char *arg;
} psh_tokenstream;
