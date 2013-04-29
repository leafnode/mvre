/*
 * mvre.c
 *
 * $Id: mvre.c,v 1.2 2004/08/07 20:33:08 leafnode Exp $
 */

#include <pcre.h>
#include <stdio.h>
#include <errno.h>
#include "replace.h"

void usage(char * name) {
   printf("Usage: %s <regexp> <rename-to> <file1> <file2> ...\n",name);
}

int main(int argc, char **argv)
{
   pcre * re;
   int erroffset;
   const char *error;
   int i, rc;
   char * out;
   const char * replacer = argv[2];

   if (argc < 4) {
      printf("Bad number of arguments\n");
      usage(argv[0]);
      return 1;
   }

   re = pcre_compile(
         argv[1],
         0,
         &error,
         &erroffset,
         NULL);

   if (re == NULL) {
      printf("Error during PCRE compilation: %s\n", error);
      return 2;
   }

   for (i = 3; i < argc; i++) {
      out = pcre_replace_re(re, replacer, argv[i], &rc);
      if (rc == PCRE_ERROR_NOMATCH) {
         printf("Can't match %s\n", argv[i]);
      } else if (rc == 0) {
         printf("Too many matches - can only match %d matches\n", (OVECCOUNT/3));
      } else {
         printf("Renaming '%s' -> '%s'... ", argv[i], out);
         if (rename(argv[i], out) == -1) {
            printf("error!");
            switch(errno) {
               case EACCES:
                  printf("Permission denied.\n");
                  break;
               case EBUSY:
                  printf("File is in use.\n");
                  break;
               case ENOTEMPTY:
               case EEXIST:
                  printf("%s is used.\n", out);
                  break;
               case EINVAL:
                  printf("Can't move directory to itself.\n");
                  break;
               case EISDIR:
                  printf("Target name is a directory.\n");
                  break;
               case EMLINK:
                  printf("Too many links.\n");
                  break;
               case ENOENT:
                  printf("Can't open file.\n");
                  break;
               case ENOSPC:
                  printf("No room for file.\n");
                  break;
               case EROFS:
                  printf("Read-only filesystem.\n");
                  break;
               case EXDEV:
                  printf("Can't span over filesystems.\n");
                  break;
            }
         } else {
            printf("done\n");
         }
      }
   }
   return 0;
}
