#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void
usage()
{
    printf("ll <path>\n");
    exit(1);
}

char*
time_to_str(time_t* timep)
{
    char* t = ctime(timep);

    int i;
    for (i = 0; t[i] != 0; i++)
    {
        if (t[i] == '\n')
        {
            t[i] = 0;
            break;
        }
    }

    return t;
}

void
print_file_prop(char* path)
{
    struct stat s;
    if (stat(path, &s) == -1)
    {
        printf("stat error\n");
        exit(1);
    }

    switch (s.st_mode & S_IFMT)
    {
    case S_IFDIR:  printf("d"); break;
    case S_IFCHR:  printf("c"); break;
    case S_IFBLK:  printf("b"); break;
    case S_IFSOCK: printf("s"); break;
    case S_IFLNK:  printf("l"); break;
    case S_IFIFO:  printf("f"); break;
    case S_IFREG:  printf("-"); break;
    default: printf("?");
    }

    printf((s.st_mode & S_IRUSR) ? "r" : "-");
    printf((s.st_mode & S_IWUSR) ? "w" : "-");
    printf((s.st_mode & S_IXUSR) ? "x" : "-");

    printf((s.st_mode & S_IRGRP) ? "r" : "-");
    printf((s.st_mode & S_IWGRP) ? "w" : "-");
    printf((s.st_mode & S_IXGRP) ? "x" : "-");

    printf((s.st_mode & S_IROTH) ? "r" : "-");
    printf((s.st_mode & S_IWOTH) ? "w" : "-");
    printf((s.st_mode & S_IXOTH) ? "x" : "-");

    struct passwd* p_psw = getpwuid(s.st_uid);
    struct group*  p_grp = getgrgid(s.st_gid);

    printf(" %d %s %s %ld %s %s\n",
           s.st_nlink,
           p_psw->pw_name,
           p_grp->gr_name,
           s.st_size,
           time_to_str(&s.st_atim.tv_sec),
           path);
}

void
show_dir(char* path)
{
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        printf("fdopendir error\n");
        exit(1);
    }

    struct dirent* dent = NULL;
    while ((dent = readdir(dir)) != NULL)
    {
        int len = strlen(path) + 1 + strlen(dent->d_name) + 1;
        char* new_path = malloc(len);
        memset(new_path, 0, len);

        strcat(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, dent->d_name);

        print_file_prop(new_path);

        free(new_path);
    }

    closedir(dir);
}

void
show_file(char* path)
{
    print_file_prop(path);
}

int
main(int argc, char** argv)
{
    if (argc != 2)
        usage();

    struct stat s;
    if (stat(argv[1], &s) == -1)
    {
        printf("stat error\n");
        exit(1);
    }

    if (s.st_mode & S_IFDIR)
        show_dir(argv[1]);
    else
        show_file(argv[1]);

    return 0;
}
