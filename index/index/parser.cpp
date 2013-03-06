#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale>
#include "parser.h"
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>

#define PTAG_B	1
#define PTAG_I	2
#define	PTAG_H	3
#define PTAG_TITLE	4
#define PTAG_SCRIPT	5

#define _TITLE_TAG	0x0001
#define _B_TAG		0x0004
#define _H_TAG		0x0008
#define _I_TAG		0x0010

#define xl_isdigit(c) (((c) >= '0') && ((c) <= '9'))
#define xl_islower(c) (((c) >= 'a') && ((c) <= 'z'))
#define xl_isupper(c) (((c) >= 'A') && ((c) <= 'Z'))
#define xl_isindexable(c) (xl_isdigit(c) || xl_islower(c) || xl_isupper(c))
#define xl_tolower(c) ((c) += 'a' - 'A')

char* parser_init(char* doc)
{
	char *p;
    
	if (strncasecmp(doc, "HTTP/", 5))
		return NULL;
	
	for (p = doc; (*p != ' ')&&(*p); p++);
	if (*p == '\0')
		return NULL;
    
	if (atoi(p) != 200)
		return NULL;
    
	p = strstr(p,  "\r\n\r\n");
	if (p == NULL)
		return NULL;
    
	return p+4;
}

int tag_parser(char* tag, int len, char* back_tag)
{
	int i = 0;
    
	if (tag[0] == '/')
	{
		*back_tag = 1;
		i++;
        
	} else
		*back_tag = 0;
    
	switch (tag[i])
	{
        case 'b':
        case 'B':
        case 'i':
        case 'I':
            if (!isspace(tag[i+1]))
                return 0;
            if ((tag[i] == 'b') || (tag[i] == 'B'))
                return PTAG_B;
            return PTAG_I;
            
        case 'e':
        case 'E':
            i++;
            if (((tag[i]=='m')||(tag[i]=='M')) && (isspace(tag[i+1])))
                return PTAG_I;
            return 0;
            
        case 'h':
        case 'H':
            i++;
            if (((tag[i]>='1')&&(tag[i]<='6')) && (isspace(tag[i+1])))
                return PTAG_H;
            return 0;
            
        case 't':
        case 'T':
            i++;
            if ((0==strncasecmp(tag+i, "itle", 4)) && (isspace(tag[i+4])))
                return PTAG_TITLE;
            return 0;
            
        case 's':
        case 'S':
            i++;
            if ((0==strncasecmp(tag+i, "trong", 5)) && (isspace(tag[i+5])))
                return PTAG_B;
            if ((0==strncasecmp(tag+i, "cript", 5)) && (isspace(tag[i+5])))
                return PTAG_SCRIPT;
            return 0;
            
        default:
            break;
	}
    
	return 0;
}

#define xlbit_set(__b1, __b2)	((__b1) |= (__b2))
#define xlbit_unset(__b1, __b2)	((__b1) &= ~(__b2))
#define xlbit_check(__b1, __b2) ((__b1)&(__b2))

int parser(char* url, char* doc, char* buf, int blen)
{
	char *p, *purl, *word, *ptag, *pbuf;
	char ch, back_tag, intag, inscript;
	unsigned tag_flag;
	int ret;
    
	p = parser_init(doc);
	if (p == NULL)
		return 0;
	pbuf = buf;
    
    /* parsing URL */
	purl = url;
	while (*purl != '\0')
	{
		if (!xl_isindexable(*purl))
		{
			purl++;
			continue;
		}
        
		word = purl;
		while (xl_isindexable(*purl))
		{
			if (xl_isupper(*purl))
				xl_tolower(*purl);
			purl++;
		}
        
		ch = *purl;
		*purl = '\0';
        
		if (pbuf-buf+purl-word+3 > blen-1)
			return -1;
		sprintf(pbuf, "%s U\n", word);
		pbuf += (purl-word)+3;
        
		*purl = ch;
	}
    
    /* parsing page */
	tag_flag = 0;
	intag = 0;
	inscript = 0;
    
	while (*p != '\0')
	{
		if (!xl_isindexable(*p))
		{
			if (*p != '>')
			{
				if (*p == '<')
				{
					ptag = p;
					intag = 1;
				}
				p++;
				continue;
			}
            
            if(intag == 0)
            {
                p++;
                continue;
            }
            
			*p = ' ';
			ret = tag_parser(ptag+1, p-ptag, &back_tag);
			switch (ret)
			{
				case PTAG_B:
                    
					if (back_tag == 0)
						xlbit_set(tag_flag, _B_TAG);
					else
						xlbit_unset(tag_flag, _B_TAG);
					break;
                    
				case PTAG_I:
                    
					if (back_tag == 0)
						xlbit_set(tag_flag, _I_TAG);
					else
						xlbit_unset(tag_flag, _I_TAG);
					break;
                    
				case PTAG_H:
                    
                    if (back_tag == 0)
                        xlbit_set(tag_flag, _H_TAG);
                    else
                        xlbit_unset(tag_flag, _H_TAG);
                    break;
                    
				case PTAG_TITLE:
                    
					if (back_tag == 0)
						xlbit_set(tag_flag, _TITLE_TAG);
					else
						xlbit_unset(tag_flag, _TITLE_TAG);
					break;
                    
				case PTAG_SCRIPT:
                    
					if (back_tag == 0)
						inscript = 1;
					else
						inscript = 0;
                    
				default:
					break;
			}
            
			intag = 0;
			p++;
			continue;
		}
        
		if (inscript || intag)
		{
			p++;
			continue;
		}
        
		word = p;
		while (xl_isindexable(*p))
		{
			if (xl_isupper(*p))
				xl_tolower(*p);
			p++;
		}
        
		ch = *p;
		*p = '\0';
        
		if (pbuf-buf+p-word+1 > blen-1)
			return -1;
		sprintf(pbuf, "%s ", word);
		pbuf += (p-word)+1;
        
		if (xlbit_check(tag_flag, _B_TAG))
		{
			if (pbuf-buf+1> blen-1)
				return -1;
			*pbuf = 'B';
			pbuf++;
		}
        
		if (xlbit_check(tag_flag, _H_TAG))
		{
			if (pbuf-buf+1> blen-1)
				return -1;
			*pbuf = 'H';
			pbuf++;
		}
        
		if (xlbit_check(tag_flag, _I_TAG))
		{
			if (pbuf-buf+1> blen-1)
				return -1;
			*pbuf = 'I';
			pbuf++;
		}
        
		if (xlbit_check(tag_flag, _TITLE_TAG))
		{
			if (pbuf-buf+1> blen-1)
				return -1;
			*pbuf = 'T';
			pbuf++;
		}
        
		if (tag_flag == 0)
		{
			if (pbuf-buf+1> blen-1)
				return -1;
			*pbuf = 'P';
			pbuf++;
		}
        
		if (pbuf-buf+1> blen-1)
			return -1;
		*pbuf = '\n';
		pbuf++;
		*p = ch;
	}
    
	*pbuf = '\0';
	return pbuf-buf;
}

int GetPostingFromPage(RawPostingVector *vector, char* page, char* url, int length, uint32_t docID)
{
    //std::cout<<page<<std::endl;
    //RawPostingVector *postingVector = new RawPostingVector;
    char *parsedBuf = new char[length];
    parser(url, page, parsedBuf, length);
    //std::cout<<parsedBuf<<std::endl;
    if(parsedBuf == NULL || strlen(parsedBuf) == 0) {
        return 1;
    }
    
    size_t parsedBufLength = strlen(parsedBuf);
    char* pagePointer = parsedBuf;
    int counter = 1;
    while(1) {
        RawPosting *posting = new RawPosting;
        int nRead;
        char context;
        char* word = new char[parsedBufLength];
        int ret = sscanf(pagePointer, "%s %c%n", word, &context, &nRead);
        if(ret == 2) {
            posting->docID = docID;
            posting->word=word;
            std::transform(posting->word.begin(), posting->word.end(), posting->word.begin(), ::tolower);
            switch(context) {
                case 'P':
                    posting->context = 0;
                    break;
                case 'B':
                    posting->context = 1;
                    break;
                case 'H':
                    posting->context = 2;
                    break;
                case 'I':
                    posting->context = 3;
                    break;
                case 'T':
                    posting->context = 4;
                    break;
                case 'U':
                    posting->context = 5;
                    break;
                default:
                    posting->context = 10;
                    break;
            }
            posting->post = counter;
            pagePointer +=nRead;
            counter ++;
            vector->push_back(posting);
            delete word;
        }
        else {
            delete word;
            delete posting;
            break;
        }
    }

    return 1;
}