#include "arg_parser.h"
#include <cstdio>
#include <cstring>
#include <malloc.h>

namespace arg_parser
{
  err arguments::process()
  {
    err ret = 0;

    char var_buf[50];
    char * var_buf_iter = &var_buf[0];

    char value_buf[50];
    char * value_buf_iter = &value_buf[0];
    char * qs_iter = raw_query_string;

    item * processed_iter = processed;

    //allocate space for `item` fields ahead of time
    for(int i = 0; i < max_processed; ++i)
    {
      (processed_iter)->var = new char[max_var_sz];
      (processed_iter)->value = new char[max_value_sz];
      ++processed_iter;
    } processed_iter = processed;
    items_are_allocated = true;

    //main loop
    unsigned char incomplete = 1;
    
    while(incomplete)
    {
      for( ;
        *qs_iter != '=';
        ++qs_iter)
      {
        if(*qs_iter == 0)
        {
          ret |= unmatched_equals;
          goto cleanup;
        }

        *(var_buf_iter++) = *qs_iter;
      } *var_buf_iter = 0;
      ++qs_iter; //advance to get rid of equals

      for( ;
        *qs_iter != '&';
        ++qs_iter)
      {
        if(*qs_iter == 0)
        {
          incomplete ^= 1;
          break;
        }
        *(value_buf_iter++) = *qs_iter;
      }
      *value_buf_iter = 0;
      ++qs_iter; //advance to get rid of ampersand
      
      //copy buffers to processed
      
      ////first check sizes
      if(var_buf_iter - var_buf + 1 >
        max_var_sz)
      { ret |= var_too_long; goto cleanup; }
      if(value_buf_iter - value_buf + 1 >
        max_value_sz)
      { ret |= value_too_long; goto cleanup; }
      if(processed_iter - processed >
        max_processed)
      { ret |= too_many_processed; goto cleanup; }

      ////now actually copy
      memcpy(processed_iter->var, var_buf,
        var_buf_iter - var_buf + 1);
      memcpy(processed_iter->value,
        value_buf, value_buf_iter-value_buf+1);
      ++processed_iter;

      //reset value_buf_iter
      value_buf_iter = value_buf;
      //reset var_buf_iter
      var_buf_iter = var_buf;
    }

    actually_processed = processed_iter - processed;

cleanup:
    if(ret) //if any error occurred
    {
      for(item * pprocessed = processed;
        pprocessed != pprocessed + max_processed;
        ++pprocessed)
      {
        delete pprocessed->var;
        delete pprocessed->value;
      }

      items_are_allocated = false;
    }
    return ret;
  }

  arguments::arguments(char * query_string)
    :raw_query_string(query_string),
    processed(new item[max_processed])
  {
  }

  arguments::arguments(char * query_string, int a)
  {
    processed = new item[max_processed];
    raw_query_string = query_string;
  }

  arguments::arguments()
  {
  }

  arguments::~arguments()
  {
    //delete all the items recursively
    if(!items_are_allocated)
      goto no_deallocate_items;

    //actually deallocate
    for(item * pprocessed = processed;
      pprocessed != pprocessed + max_processed;
      ++pprocessed)
    {
      delete pprocessed->var;
      delete pprocessed->value;
    }

no_deallocate_items:

    delete processed;
  }
}