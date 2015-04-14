#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

class Utils
{
	public:
		static const char* SkipBlank(const char* ptr){
			if(ptr)
				while (isspace(*ptr)) ++ptr;
			return ptr;
		}
		static const char* SkipALps(const char* ptr){
			if(ptr)
				while (isalpha(*ptr)) ++ptr;
			return ptr;
		}
		static const char* SkipValidNames(const char* ptr){
			if(ptr)
				while (isalnum(*ptr) || *ptr == '-'|| *ptr == '_') ++ptr;
			return ptr;
		}	
			
		static const char* SkipNotALpnum(const char* ptr){
			while (*ptr && !isalnum(*ptr) ) ++ptr;
			return ptr;
		}
		static const char* SkipNotBlank(const char* ptr,const char* limit=0){
			if(limit)			
				while (*ptr && !isspace(*ptr)&&ptr<limit) ++ptr;
			else while (*ptr && !isspace(*ptr) ) ++ptr;
			return ptr;
		}
		static const char *find_next_char(const char *c, const char *end, char ch)
		{
			if(c && end && c<end)
			{			
				for(;c<end;++c)
				 if(*c == ch)
				 	return c;
			}
			return end;
		}
		static bool IsGraph(const char* ptr)
		{
			if(!ptr)return true;
			for(int i=0;i<strlen(ptr);++i)
				if(isgraph(*ptr))
					return false;
			return true;
		}
	 
};
#endif
