/*
   token.h - parser tokens

   Copyright 2018 Zhang Maiyun.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
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
        WORD, ASSIGNMENT,    /* = */
        NUMBER, ARITH_CMD,   /*  */
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
