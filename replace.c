/*
 * replace.c
 *
 * $Id: replace.c,v 1.1.1.1 2004/08/07 19:17:06 leafnode Exp $
 */

#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <ctype.h>
#include "replace.h"

char * pcre_replace( const char * regexp, const char * replacer, const char * subject, int * rc )
{
   pcre * re;
   int erroffset;
   const char *error;
   re = pcre_compile(
         regexp,
         0,
         &error,
         &erroffset,
         NULL);

   if (re == NULL) {
      printf("Error during PCRE compilation: %s\n", error);
      *rc = -2;
      return (char *)NULL;
   }

   return pcre_replace_re(re, replacer, subject, rc);
}

char * pcre_replace_re (pcre * re, const char * replacer, const char * subject, int * rc )
{
   int ovector[OVECCOUNT];
   char * out = (char *)malloc(512);
   int subj_len = strlen(subject);
   int replacer_len = strlen(replacer);
   int i, curr_out, substrNr, substrLen;
   const char * substrPtr;

   *rc = pcre_exec(
         re,
         NULL,
         subject,
         subj_len,
         0,
         0,
         ovector,
         OVECCOUNT);
   if (*rc == PCRE_ERROR_NOMATCH) {
      return NULL;
   }

   if (*rc == 0) {
      *rc = OVECCOUNT/3;
   }

   curr_out = 0;
   for (i = 0; i < replacer_len; i++) {
      if (replacer[i] != '\\') {
         out[curr_out++] = replacer[i];
      } else {
         if (isdigit(replacer[i+1])) { // parsing placeholder
            substrNr = (int)(replacer[i+1]-'0');
            substrLen = pcre_get_substring(
                  subject,
                  ovector,
                  *rc,
                  substrNr,
                  &substrPtr);
            strncat(out, substrPtr, substrLen);
            curr_out += substrLen;
            i++;
         } else {
            out[curr_out++] = replacer[i];
         }
      }
   }
   return out;
}
