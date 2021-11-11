# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/types.h>
# include <dirent.h>
# include <sys/stat.h>

int main(int argc, char const *argv[])
{
    // odpiranje trenutnega direktorija
    DIR* dp = opendir(".");
    if (dp == NULL){
        perror("Napaka pri odpiranju direktorija:");
        exit(1);
    }

    // read files
    struct dirent* dirp;
    while ((dirp = readdir(dp)) != NULL) {
        struct stat stat_buf;
        if (stat(dirp->d_name, &stat_buf) == -1) {
            perror("Napaka pri branju statistike datoteke:");
            exit(1);
        }

        // get permissions
        char perm[9] = "---------";
        if (stat_buf.st_mode & S_IRUSR) {
            perm[0] = 'r';
        }
        if (stat_buf.st_mode & S_IWUSR) {
            perm[1] = 'w';
        }
        if (stat_buf.st_mode & S_IRGRP) {
            perm[3] = 'r';
        }
        if (stat_buf.st_mode & S_IWGRP) {
            perm[4] = 'w';
        }
        if (stat_buf.st_mode & S_IROTH) {
            perm[6] = 'r';
        }
        if (stat_buf.st_mode & S_IWOTH) {
            perm[7] = 'w';
        }
        if (stat_buf.st_mode & S_ISUID) {
            if (stat_buf.st_mode & S_IXUSR) {
                perm[2] = 's';
            } else {
                perm[2] = 'S';
            }
        } else if (stat_buf.st_mode & S_IXUSR) {
            perm[2] = 'x';
        }
        if (stat_buf.st_mode & S_ISGID) {
            if (stat_buf.st_mode & S_IXGRP) {
                perm[5] = 's';
            } else {
                perm[5] = 'S';
            }
        } else if (stat_buf.st_mode & S_IXGRP) {
            perm[5] = 'x';
        }
        if (stat_buf.st_mode & S_ISVTX) {
            if (stat_buf.st_mode & S_IXUSR) {
                perm[8] = 't';
            } else {
                perm[8] = 'T';
            }
        } else if (stat_buf.st_mode & S_IXUSR) {
            perm[8] = 'x';
        }

        // get file type
        char ftype = '-';
        if (S_ISDIR(stat_buf.st_mode)) {
            ftype = 'd';
        } else if (S_ISCHR(stat_buf.st_mode)) {
            ftype = 'c';
        } else if (S_ISBLK(stat_buf.st_mode)) {
            ftype = 'b';
        } else if (S_ISFIFO(stat_buf.st_mode)) {
            ftype = 'p';
        } else if (S_ISLNK(stat_buf.st_mode)) {
            ftype = 'l';
        } else if (S_ISSOCK(stat_buf.st_mode)) {
            ftype = 'c';
        }

        printf("%c%s %ld %s\n", ftype, perm, stat_buf.st_nlink, dirp->d_name);
    }

    closedir(dp);
    return 0;
}
