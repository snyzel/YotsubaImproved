/*
 *  Yotsuba Improved
 *  Author: snyzel
 *
 *  A reverse engineered implementation of
 *  the proprietary imageboard software
 *  ``Yotsuba'' used by 4chan. Yotsuba
 *  Improved (backend) is written in native
 *  code.
 *  
 *  Yotsuba Improved is guaranteed to
 *  compile and execute on x86 Apache or
 *  IIS servers.
 *
 *  License: MIT
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stddef.h>
#include "arg_parser.h"

#define QWORD unsigned long long int
#define DWORD unsigned int

namespace text
{
  typedef unsigned int status;
  static const status word_size_exceeded = 0x01;
  static const status out_of_dest_bounds = 0x02;

  template<size_t x, size_t y>
  QWORD split_at_colon(
    char dest[x][y],
    char src[]
    )
  {
    static const int max_word_size = 100;
    int internal_x = 0,
      internal_y = 0;

    char buffer[max_word_size];
    QWORD ret = 0;

    for(char * psrc = src, * pbuf = buffer;
      *psrc != 0; ++psrc)
    {
      if(*(psrc + 1) == 0)
      {
        *(pbuf++) = *psrc;
        goto handle_colon;
      }

      switch(*psrc)
      {
      case ' ':
          continue;

      case ':':
        {
handle_colon:
          *pbuf = 0;
          
          //bounds check first
          internal_y = pbuf - buffer + 1;
          if(internal_x > x || internal_y > y)
            return *((DWORD *) &ret) = out_of_dest_bounds;
          
          //write buffer to destination
          memcpy(dest[internal_x++],
            buffer, internal_y); 
          //words read:
          ++*((DWORD *) &ret + 1);

          /*
          reset buffer ptr; we're ready
          to read a new word into buffer
          */
          pbuf = buffer;
          
          continue;
        }
      }

      if(pbuf - buffer < max_word_size)
        *(pbuf++) = *psrc;
      else
        return *((DWORD *) &ret) = word_size_exceeded;
    }

    return ret;
  }
}

int main(void)
{
  printf("Content-Type: text/plain;charset=us-ascii\n\n");
  const char * qs_src = getenv("QUERY_STRING");
  if(!qs_src)
    return 0;
  puts(qs_src);
  const int qs_sz = strlen(qs_src) + 1;
  char * qs = new char[qs_sz];
  memcpy(qs, qs_src, qs_sz);
  arg_parser::arguments argp(qs);
  /*
  arg_parser::err state = argp.process();
  //to do: properly handle these errors
  // {
  assert(state ^ arg_parser::unmatched_ampersand);
  assert(state ^ arg_parser::unmatched_equals);
  assert(state ^ arg_parser::var_too_long);
  assert(state ^ arg_parser::value_too_long);
  assert(state ^ arg_parser::too_many_processed);
  // }
  
  for(unsigned int i = 0; i < argp.actually_processed; ++i)
  {
    printf("%d var: %s\n", i,
      argp.processed[i].var);
    printf("%d value: %s\n", i,
      argp.processed[i].value);
  }
  */

  return 0;
}