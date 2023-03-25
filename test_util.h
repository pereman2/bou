
#define TEST(fn, name)                                                             \
  {                                                                                \
    printf("\033[36mRunning test::%s => %-50s ...\033[36m ", __FILE_NAME__, name); \
    fflush(stdout);                                                                \
    fn();                                                                          \
    printf("\033[32mOK\033\n");                                                    \
  }

