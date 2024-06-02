/*
 * 
 * Copyright (C) 2004 Neeraj Jakhar <neerajcd@iitk.ac.in>
 *
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either 
 * version 2 of the License, or (at your option) any later 
 * version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_SIZE	128

enum token_errors
{
	max_token_size
};


enum token_types
{
	error,
	eof,
	newline,
	hash,
	semicolon,
	spaceandtab,
	string
};

typedef struct token			/* return-type for function
					 * get_token().
					 */
{
	enum token_types type;		/* enumerated data-type */
	long tokenval;			/* stored token value is
					 * interpreted as char* if type
					 * is string, int otherwise.
					 */
}token;


extern token get_token(FILE *);

#endif	/* TOKEN_H */

