#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*
void
pipelining()
{
  int buf;
  while(read(p[0], &buf, 4))
  {
    fprintf(1, "%d\n", buf);
  }
  return;
}
*/

// to analusis the program flow, take 2~5 for example and draw picture.
void pipelining(int *p)
{
  close(p[1]);
  int p1[2];
  pipe(p1);

  int m;
  read(p[0], &m, 4);
  fprintf(1, "prime %d\n", m);
  int n;

  int fork_flag = 1;
  while (read(p[0], &n, 4))
  {
    // one process only fork once
    if (fork_flag)
    {
      if (fork() == 0)
      {
        close(p[0]);
        pipelining(p1);
      }
      fork_flag = 0;
    }
    if (n % m)
    {
      write(p1[1], &n, 4);
    }
  }
  close(p1[0]); // zyy:no use in parent process
  close(p1[1]);
  close(p[0]);
  wait(0);
  exit(0);
}

int main(int argc, char *argv[])
{
  int p[2];
  pipe(p);
  int begin = 2, end = 35;
  for (int i = begin; i <= end; i++)
  {
    write(p[1], &i, 4);
  }
  /**zyy
   * if not close the write end of file, the "read" in pipelining while will blocked,
   * and the program will not return.
   * 
   * the "close" is moved into pipeling
   * */

  pipelining(p);

  exit(0);
}