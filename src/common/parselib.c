/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** configuration scripts parsing library
*************************************************************************/
#include "common.h"

// ------------------------- * globals * -------------------------

// ------------------------- * aux * -------------------------
char p_pos[256], p_err[256]="";

/*
** P_BeginParse
*/
void P_BeginParse(parse_t *p, char *data)
{
	memset(p, 0, sizeof(parse_t));
	p->src=p->lineStart=p->tokenStart=data;
}

/*
** P_StreamPosition
*/
char *P_StreamPosition(parse_t *p)
{
	sprintf(p_pos, "Ln %d, Col %d", p->lineNumber+1, p->src-p->lineStart+1);
	return p_pos;
}

/*
** P_LastError
*/
char *P_LastError(void)
{
	return p_err;
}

/*
** P_TokenType
*/
char *P_TokenType(ttype_t n)
{
	switch(n)
	{
	case TOKEN_NULL:
		return "no token";
	case TOKEN_SYMBOL:
		return "symbol";
	case TOKEN_NUM:
		return "number";
	case TOKEN_QUOTE:
		return "quoted string";
	case TOKEN_ID:
		return "identifier";
	}
	return "unknown";
}

// ------------------------- * parsing * -------------------------

/*
** UnGetToken
**
** returns last parsed token to the stream
*/
void P_UnGetToken(parse_t *p)
{
	p->tokenReturned=true;
}

/*
** P_GetToken
**
** gets next token from the stream
*/
ttype_t P_GetToken(parse_t *p)
{
	char *dest;

	if(p->tokenReturned)
	{
		p->tokenReturned=false;
		return p->tokenType;
	}

	p->tokenType=TOKEN_NULL;
	*p->token=0;
	dest=p->token;

	if(!p->src) return TOKEN_NULL; // just in case

	while(1)
	{
		// skip whitespace
		while(*p->src<=' ')
		{
			if(!*p->src) return TOKEN_NULL; // end of stream
			if(*p->src++=='\n')
			{
				p->lineNumber++;
				p->lineStart=p->src;
			}
		}
		// single line comments ";", "#", "//"
		if(*p->src==';' || *p->src=='#' || (p->src[0]=='/' && p->src[1]=='/'))
			while(*p->src && *p->src!='\n') p->src++;
		// multiline comments "/* .. */"
		else if(p->src[0]=='/' && p->src[1]=='*')
		{
			while(*p->src && !(p->src[0]=='*' && p->src[1]=='/'))
				if(*p->src++=='\n')
				{
					p->lineNumber++;
					p->lineStart=p->src;
				}
			if(*p->src) p->src+=2; // skip "*/"
		}
		else
			break;
	}

	p->tokenStart=p->src; // save token start

	// quoted string
	if(*p->src=='"')
	{
		p->src++;
		while(*p->src && *p->src!='"' && *p->src!='\n')
		{
			if(*p->src=='\\') // escape sequence
				switch(*++p->src)
			{
			case '\\':
			case '"':
				*dest++=*p->src++;
				break;
			default:
				*dest++='\\';
				break;
			}
			else
				*dest++=*p->src++;
		}
		if(*p->src=='"') *p->src++;
		*dest=0;
		return p->tokenType=TOKEN_QUOTE;
	}
	// symbol
	if(strchr(";,+-<>/*%&\\^=()[]{}':?", *p->src))
	{
		*dest++=*p->src++;
		*dest=0;
		return p->tokenType=TOKEN_SYMBOL;
	}
	// number
	if(isdigit(*p->src) || *p->src=='.')
	{
		strtod(dest=p->src, &p->src);
		strncpy(p->token, dest, p->src-dest);
		p->token[p->src-dest]=0;
		return p->tokenType=TOKEN_NUM;
	}
	// identifier
	while(*p->src && !strchr(" ;,+-<>/*%&\\^=()[]{}\"':?\t\n\r", *p->src))
		*dest++=*p->src++;
	*dest=0;
	return p->tokenType=TOKEN_ID;
}

// ------------------------- * expect functions * -------------------------

/*
** P_Expect
*/
void P_Expect(parse_t *p, ttype_t type, char *val)
{
	if(P_GetToken(p)!=type || strcmp(p->token, val))
	{
		sprintf(p_err, "\"%s\" expected", val);
		longjmp(p->error, 1);
	}
}

/*
** P_ExpectString
*/
void P_ExpectString(parse_t *p, char *dest, unsigned maxlen)
{
	if(P_GetToken(p)!=TOKEN_QUOTE)
	{
		strcpy(p_err, "string expected");
		longjmp(p->error, 1);
	}
	if(maxlen && strlen(p->token)>=maxlen)
	{
		sprintf(p_err, "string exceeds maximum length of %d", maxlen);
		longjmp(p->error, 1);
	}
	strcpy(dest, p->token);
}

/*
** P_ExpectString
**
** allocates space automatically
*/
char *P_ExpectStringEx(parse_t *p)
{
	if(P_GetToken(p)!=TOKEN_QUOTE)
	{
		strcpy(p_err, "string expected");
		longjmp(p->error, 1);
	}
	
	return copystring(p->token);
}

/*
** P_ExpectId
*/
void P_ExpectId(parse_t *p, char *dest, unsigned maxlen)
{
	if(P_GetToken(p)!=TOKEN_ID)
	{
		strcpy(p_err, "identifier expected");
		longjmp(p->error, 1);
	}
	if(maxlen && strlen(p->token)>=maxlen)
	{
		sprintf(p_err, "identifier exceeds maximum length of %d", maxlen);
		longjmp(p->error, 1);
	}
	strcpy(dest, p->token);
}

/*
** P_ExpectString
**
** allocates space automatically
*/
char *P_ExpectIdEx(parse_t *p)
{
	if(P_GetToken(p)!=TOKEN_ID)
	{
		strcpy(p_err, "identifier expected");
		longjmp(p->error, 1);
	}
	
	return copystring(p->token);
}

/*
** P_ExpectSymbol
*/
void P_ExpectSymbol(parse_t *p, int c)
{
	if(!P_CheckSymbol(p, c))
	{
		sprintf(p_err, "%c expected", c);
		longjmp(p->error, 1);
	}
}

/*
** P_ExpectInt
*/
void P_ExpectInt(parse_t *p, int *dest)
{
	int sign=1;

	if(!P_CheckSymbol(p, '+') && P_CheckSymbol(p, '-'))
		sign=-1;

	if(P_GetToken(p)!=TOKEN_NUM)
	{
		strcpy(p_err, "number expected");
		longjmp(p->error, 1);
	}

	*dest=atol(p->token)*sign;
}

/*
** P_ExpectInt
*/
void P_ExpectFloat(parse_t *p, float *dest)
{
	float sign=1;

	if(!P_CheckSymbol(p, '+') && P_CheckSymbol(p, '-'))
		sign=-1;

	if(P_GetToken(p)!=TOKEN_NUM)
	{
		strcpy(p_err, "number expected");
		longjmp(p->error, 1);
	}

	*dest=(float)atof(p->token)*sign;
}

/*
** P_CheckSymbol
*/
bool P_CheckSymbol(parse_t *p, int c)
{
	if(P_GetToken(p)!=TOKEN_SYMBOL || *p->token!=c)
	{
		P_UnGetToken(p);
		return false;
	}
	return true;
}
