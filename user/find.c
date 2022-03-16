#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return "\0"-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), '\0', DIRSIZ - strlen(p));
  return buf;
}

void find(char *path, char *filename)
{
  // for debugging
  // printf("path: %s\n", path);
  // printf("filename: %s\n", filename);

  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // printf("%d\n", st.type);

  switch (st.type) {
  case T_FILE:
    if (!strcmp(fmtname(path), filename))
      // printf("%d\n", strcmp( filename, fmtname(path)));
      printf("%s\n", path);
      // printf("%s\n", fmtname(path));
      // printf("%s\n", filename); 
      // printf("\n");
    break;
  case T_DIR:
    // printf("path: %s\n", path);
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    // printf("buf: %s\n", buf);
    int cnt = 0;
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      cnt++;
      if (cnt == 40)
        break;
      // printf("%s\n", de.name);

      if (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      // printf("buf: %s\n", buf);
      if (strcmp(p, ".") && strcmp(p, ".."))
        find(buf, filename);
    }
    break;
  default:
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("error: parameter is not enough!\n");
  }
  else if (argc < 3)
  {
    find(".", argv[1]);
  }
  else if (argc < 4)
  {
    find(argv[1], argv[2]);
  }
  else
  {
    printf("too many parameters!\n");
  }
  exit(0);
}