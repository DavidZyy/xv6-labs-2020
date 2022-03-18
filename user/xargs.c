/**
 * @file xargs.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-03-16
 *
 * @copyright Copyright (c) 2022
 * @discription
 * the output of the left side of pipe is input(feed) into the
 * the right side of pipe as commmand in lines.
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

/**
 * @brief argv[0] = "xargs"
 * argv[1] = command to exec, argv[2~] = parameters
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char *argv[])
{
  int n, m;
  char *p, *q;
  char buf[1024];
  char *new_argv[MAXARG];
  if (argc <= 0)
  {
    printf("too few arguments!\n");
    exit(1);
  }
  if (argc > MAXARG + 1)
  {
    printf("too many arguments!\n");
    exit(1);
  }

  for (int i = 1; i < argc; i++)
  {
    new_argv[i-1] = argv[i];
  }
  m = 0;
  // read from standard output
  while ((n = read(0, buf + m, sizeof(buf) - m - 1)) > 0)
  {
    m += n;
    buf[m] = '\0';
    p = buf;
    while ((q = strchr(p, '\n')) != 0)
    {
      // write(1, p, q+1 - p);
      *q = '\0';
      new_argv[argc-1] = p;
      if (fork() == 0)
      {
        // printf("%s\n", argv[1]);
        exec(new_argv[0], new_argv); // failed here!!
      }
      wait(0);
      // printf("%s\n", p);
      *q = '\n';
      p = q + 1;
    }
    if (m > 0)
    {
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
  exit(0);

  // for(int i = 0; i < argc; i++){
  //   printf("%s\n", argv[i]);
  // }
  // exit(0);
}