#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <ctype.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#define INTBUF_SIZE 16
#define STRINGBUF_SIZE 128
#define CONTENT_SIZE 65536

#define SECRET_FILE ".passwd"
#define HOMEDIR "/challenge/"
#define TMPDIR "/tmp/"

#define N_ELEMS(x) (sizeof x / sizeof x[0])
#define die() { fprintf(stderr, "died at line %u\n", __LINE__); exit(__LINE__); }
#define display_shift(fp, buf, len, b) read(fileno(fp), buf, len); for (size_t i = 0; i < len; i++) { printf("%c", (buf)[i] + b); }

static unsigned long
get_integer_from_user(void)
{
        unsigned long v;
        char buf[INTBUF_SIZE] = "";
        char *endptr = NULL;

        (void) fgets(buf, sizeof buf, stdin);
        buf[strcspn(buf, "\n")] = '\0';
        errno = 0;
        v = strtoul(buf, &endptr, 0);
        if (ERANGE == errno || '\0' != *endptr)
                return 0;

        return v;
}


static char *
get_string_from_user(void)
{
        static char buf[STRINGBUF_SIZE];

        memset(buf, 0, sizeof buf);
        (void) fgets(buf, sizeof buf, stdin);
        buf[strcspn(buf, "\n")] = '\0';

        return buf;
}

static size_t
get_file_size(int fd)
{
        struct stat st;
        size_t r = 0;

        if (fstat(fd, &st)) {
                perror("stat");
                goto end;
        }

        r = st.st_size;
end:
        return r;
}

static void
allocate_file_content(int fd, char **bufp, size_t *buf_lenp)
{
        size_t buf_len;
        char *buf;

        buf_len = get_file_size(fd);
        if (NULL == (buf = malloc(buf_len))) {
                perror("malloc");
                die();
        }

        *bufp = buf;
        *buf_lenp = buf_len;
}

static FILE *
safe_fopen(const char * const path)
{
        struct stat st, lst;
        FILE *fp = NULL;

        if (lstat(path, &lst)) {
                perror("stat");
                goto bad;
        }

        if (! S_ISREG(lst.st_mode)) {
                printf("%s is not a regular file, ignore it!\n", path);
                goto bad;
        }

        if (NULL == (fp = fopen(path, "r"))) {
                perror("fopen");
                goto bad;
        }

        if (fstat(fileno(fp), &st) < 0) {
                perror("fstat");
                goto bad;
        }

        if (lst.st_ino != st.st_ino || lst.st_dev != st.st_dev) {
                printf("TOCTOU detected!\n");
                goto bad;
        }

        return fp;
bad:
        if (fp)
                fclose(fp);
        return NULL;
}

static void
display_file_infos(void)
{
        struct stat st;
        char *filename;

        printf("Please enter the file name: ");
        if (NULL == (filename = strdup(get_string_from_user()))) {
                perror("strdup");
                goto end;
        }

        if (lstat(filename, &st)) {
                perror("stat");
                goto end;
        }

        if (S_ISLNK(st.st_mode)) {
                printf("%s is a symlink, ignore it!\n", filename);
                goto end;
        }

        printf("-> device: %lu\n", st.st_dev);
        printf("-> inode: %lu\n", st.st_ino);
        printf("-> mode: %#x\n", st.st_mode);
        printf("-> n_links: %lu\n", st.st_nlink);
        printf("-> uid: %lu\n", (unsigned long) st.st_uid);
        printf("-> gid: %lu\n", (unsigned long) st.st_gid);
        printf("-> size: %zu\n", st.st_size);
end:
        free(filename);
}

static void
display_shifted_file_content(void)
{
        FILE *fp = NULL;
        char *filename = NULL, *filename_alloc = NULL;
        unsigned char b;
        char *buf = NULL;
        size_t buf_len = 0;
        struct stat st;

        printf("Please enter the file name to display in shift mode: ");
        filename = get_string_from_user();

        if (NULL == (fp = safe_fopen(filename))) {
                printf("Failed to open \"%s\"\n", filename);
                goto end;
        }

        allocate_file_content(fileno(fp), &buf, &buf_len);
        if (buf_len > CONTENT_SIZE)
                buf_len = CONTENT_SIZE;

        printf("What value would you add to each byte of the content: ");
        b = get_integer_from_user();

        /* do not display files in temporary dir, to avoid possible garbage
         * since it's often a sticky dir where people tend to put anything
         * in, especially on a challenge box;  other people with the same
         * user id could be tempted to mess around */
        if (NULL == (filename_alloc = realpath(filename, NULL))) {
                perror("realpath");
                goto end;
        }

        if (NULL != strstr(filename_alloc, TMPDIR)) {
                puts("Temporary directories are forbidden.");
                goto end;
        }

        if (fstat(fileno(fp), &st) < 0) {
                perror("fstat");
                goto end;
        }

        if (getuid() != st.st_uid) {
                puts("Not a file we own.");
                goto end;
        }

        if (NULL != strstr(filename_alloc, HOMEDIR))
                display_shift(fp, buf, buf_len, b);

  end:
        free(filename_alloc);
        free(buf);
        if (fp)
                fclose(fp);
}

static void
display_file_content(void)
{
        FILE *fp = NULL;
        char content[CONTENT_SIZE] = "";
        char *filename;

        printf("Please enter the file name: ");
        filename = get_string_from_user();

        if (NULL == (fp = safe_fopen(filename))) {
                printf("Failed to open \"%s\"\n", filename);
                goto end;
        }

        if (NULL == (filename = realpath(filename, NULL))) {
                perror("realpath");
                goto end;
        }

        (void) fgets(content, sizeof content, fp);

        if (NULL != strstr(filename, SECRET_FILE)) {
                printf("FORBIDDEN: \"%s\"\n", filename);
                goto end;
        }

        for (size_t i = 0; content[i] != '\n' && i < sizeof content; i++)
                printf("%c", content[i]);

end:
        if (fp)
                fclose(fp);
}

static void
display_user_infos(void)
{
        char *username;
        struct passwd *pw;

        printf("Enter the user name: ");
        if (NULL == (username = strdup(get_string_from_user()))) {
                perror("strdup");
                die();
        }

        if (strstr(username, "root")) {
                printf("VERBOTEN!\n");
                goto end;
        }

        if (NULL == (pw = getpwnam(username))) {
                perror("getpwnam");
                goto end;
        }

        printf("-> uid: %lu\n", (unsigned long) pw->pw_uid);
        printf("-> gid: %lu\n", (unsigned long) pw->pw_gid);
        printf("-> gecos: %s\n", pw->pw_gecos);
        printf("-> home: %s\n", pw->pw_dir);
        printf("-> shell: %s\n", pw->pw_shell);
end:
        free(username);
}

static void
display_directory_entries(void)
{
        char *dirname;
        DIR *d;
        struct dirent *dirent;

        printf("Enter the directory name: ");
        if (NULL == (dirname = strdup(get_string_from_user()))) {
                perror("strdup");
                die();
        }

        if (NULL == (d = opendir(dirname))) {
                perror("dirname");
                goto end;
        }

        while (NULL != (dirent = readdir(d)))
                printf("-> inode=%lu, name=%s\n", dirent->d_ino, dirent->d_name);

end:
        if (d)
                closedir(d);
        free(dirname);
}

static void
exit_program(void)
{
        exit(EXIT_SUCCESS);
}

static void
make_choice(void)
{
        int choice;
        void (*cb[])(void) = {
                [1] = display_file_infos,
                [2] = display_file_content,
                [3] = display_user_infos,
                [4] = display_directory_entries,
                [5] = display_shifted_file_content,
                [6] = exit_program
        };

again:
        printf("\n=========================================================\n"
               "Choice: \n"
                        "\t[1] Display file infos\n"
                        "\t[2] Display file content (or its first line)\n"
                        "\t[3] Display user infos\n"
                        "\t[4] Display directory infos\n"
                        "\t[5] Display a shifted content of a file\n"
                        "\t[6] Exit program\n"
                " -> ");

        choice = get_integer_from_user();
        if (choice >= (int) N_ELEMS(cb) || 0 == choice) {
                printf("Invalid choice.  Try again...\n");
                goto again;
        }

        cb[choice]();
}

int
main(void)
{
        while (1) {
                make_choice();
                sleep(1);
        }

        return 0;
}
