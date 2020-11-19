/** @file psh/variables.h - @brief psh variables, functions and aliases */
/*
    Copyright 2020 Zhang Maiyun

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

#ifndef _PSH_VARIABLE_H
#define _PSH_VARIABLE_H

/** @brief Variable scopes. */
typedef enum _psh_variable_scope
{
    /** Local variable scope. */
    LOCAL = 0,
    /** Global variable scope. */
    GLOBAL,
    /** Environmental variable scope. */
    ENVIRONMENT
} psh_variable_scope;

/** @brief Variable container. */
struct psh_variable_container
{
    /** @brief Variable attributes. */
    enum _psh_variable_attributes
    {
        /** Indexed arrays. */
        INDEX_ARRAY = 0x01,
        /** Associative arrays. */
        ASSOC_ARRAY = 0x02,
        /** Integer. */
        INTEGER = 0x04,
        /** Reference to the variable by its value. */
        REFERENCE = 0x08,
        /** Readonly. */
        READONLY = 0x10,
        /** Trace. */
        TRACE = 0x20,
        /** Exported. */
        EXPORT = 0x40
    } attributes; /**< The attributes for this variable. */
    char *payload;
};
#endif
