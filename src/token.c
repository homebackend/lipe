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

#include "token.h"
#include <stdlib.h>

token get_token(FILE *file)
{
	int ch;
	token tok;
	char temp[MAX_TOKEN_SIZE];
	int i;
	ch=fgetc(file);
	switch(ch)
	{
		case EOF:
			tok.type=eof;
			tok.tokenval=0;
			break;
		case '#':
			tok.type=hash;
			tok.tokenval=0;
			break;
		case ';':
			tok.type=semicolon;
			tok.tokenval=0;
			break;
		case '\n':
			tok.type=newline;
			tok.tokenval=0;
			break;
		case ' ':
		case '\t':
			while(1)
			{
				ch=fgetc(file);
				if(ch!=' '&&ch!='\t')
					break;
			}
			ungetc(ch,file);
			tok.type=spaceandtab;
			tok.tokenval=0;
			break;
		default:
			if(isalnum(ch))
			{
				temp[0]=ch;
				for(i=1;isalnum(ch)&&i<MAX_TOKEN_SIZE;i++)
				{
					ch=fgetc(file);
					temp[i]=ch;
				}
				if(i>=MAX_TOKEN_SIZE)
				{
					tok.type=error;
					tok.tokenval=max_token_size;
				}
				else
				{
					tok.type=string;
					ungetc(ch,file);
					temp[i-1]='\0';
					tok.tokenval=(int)malloc(strlen(temp)+1);
					strcpy((char *)tok.tokenval,temp);
				}
			}
	}
	return tok;
}

