namespace arg_parser
{
  typedef unsigned int err;
  static const err unmatched_equals = 0x01;
  static const err unmatched_ampersand = 0x02;
  static const err var_too_long = 0x04;
  static const err value_too_long = 0x08;
  static const err too_many_processed = 0x10;

  class arguments
  {
  public:
    err process();
    arguments(char *);
    arguments(char *, int);
    arguments();
    ~arguments();
    struct item
    {
      char * var;
      char * value;
    };
    //item *const processed;
    item * processed;
    unsigned int actually_processed;
  private:
    //char *const raw_query_string;
    char * raw_query_string;
    bool items_are_allocated;

    static const int max_processed = 20;
    static const int max_var_sz = 10;
    static const int max_value_sz = 10;

  };

}