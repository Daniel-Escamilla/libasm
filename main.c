#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define RESET     "\x1b[0m"
#define BOLD      "\x1b[1m"
#define DIM       "\x1b[2m"
#define CYAN      "\x1b[36m"
#define GREEN     "\x1b[32m"
#define RED       "\x1b[31m"

#define SQ_OK     "\x1b[32m✓\x1b[0m"
#define SQ_FAIL   "\x1b[31m✗\x1b[0m"

size_t  ft_strlen(const char *s);
char    *ft_strcpy(char *dest, const char *src);
int     ft_strcmp(const char *s1, const char *s2);
ssize_t ft_write(int fd, const void *buf, size_t count);
ssize_t ft_read(int fd, void *buf, size_t count);
char    *ft_strdup(const char *s);

static int g_pass;
static int g_total;

static int sign(int n) {
    if (n < 0)
        return (-1);
    if (n > 0)
        return (1);
    return (0);
}

static void escape_str(char *dst, const char *src, size_t size) {
    size_t i = 0;
    size_t j = 0;

    dst[j++] = '"';
    while (src[i] && j + 3 < size) {
        if (src[i] == '\n') {
            dst[j++] = '\\';
            dst[j++] = 'n';
        } else {
            dst[j++] = src[i];
        }
        i++;
    }
    dst[j++] = '"';
    dst[j] = '\0';
}

static void title(const char *name) {
    printf("\n" BOLD CYAN "─── [ %s ] ─────────────" RESET "\n", name);
}

static void report(int ok, const char *fmt, ...) {
    va_list ap;
    char    buf[256];

    g_total++;
    g_pass += (ok != 0);

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    printf("  %-68s %s\n", buf, ok ? SQ_OK : SQ_FAIL);
}

void test_strlen(void) {
    char    *tests[] = {"", "a", "Hola mundo!"};
    char    input[64];
    size_t  mine;
    size_t  real;
    int     n;
    int     i;

    title("ft_strlen");
    n = sizeof(tests) / sizeof(tests[0]);
    i = 0;
    while (i < n) {
        mine = ft_strlen(tests[i]);
        real = strlen(tests[i]);
        snprintf(input, sizeof(input), "\"%s\"", tests[i]);
        report(mine == real, "%-18s " DIM "->" RESET " mine=%zu real=%zu",
            input, mine, real);
        i++;
    }
}

void test_strcpy(void) {
    char    *tests[] = {"", "a", "Hola mundo!"};
    char    buf_mine[64];
    char    buf_real[64];
    char    input[64];
    int     n;
    int     i;

    title("ft_strcpy");
    n = sizeof(tests) / sizeof(tests[0]);
    i = 0;
    while (i < n) {
        ft_strcpy(buf_mine, tests[i]);
        strcpy(buf_real, tests[i]);
        snprintf(input, sizeof(input), "\"%s\"", tests[i]);
        report(strcmp(buf_mine, buf_real) == 0,
            "%-18s " DIM "->" RESET " mine=\"%s\" real=\"%s\"",
            input, buf_mine, buf_real);
        i++;
    }
}

void test_strcmp(void) {
    char    *s1[] = {"abc", "abc", "abc", "a", "abc", "", "", "Hola"};
    char    *s2[] = {"abc", "abd", "ab", "abc", "a", "", "x", "Hola"};
    char    cmp_str[64];
    int     mine;
    int     real;
    int     n;
    int     i;

    title("ft_strcmp");
    n = sizeof(s1) / sizeof(s1[0]);
    i = 0;
    while (i < n) {
        mine = ft_strcmp(s1[i], s2[i]);
        real = strcmp(s1[i], s2[i]);
        snprintf(cmp_str, sizeof(cmp_str), "\"%s\" vs \"%s\"", s1[i], s2[i]);
        report(sign(mine) == sign(real),
            "%-18s " DIM "->" RESET " mine=%d real=%d",
            cmp_str, mine, real);
        i++;
    }
}

void test_write(void) {
    char    *msg = "Hola mundo!\n";
    char    escaped[64];
    int     fds[2];
    char    buf[64];
    ssize_t mine;
    ssize_t real;
    int     mine_errno;
    int     real_errno;

    escape_str(escaped, msg, sizeof(escaped));

    title("ft_write");
    pipe(fds);
    mine = ft_write(fds[1], msg, strlen(msg));
    read(fds[0], buf, sizeof(buf));
    report((mine == (ssize_t)strlen(msg)) && (memcmp(buf, msg, mine) == 0),
        "%-18s " DIM "->" RESET " mine=%zd real=%zu",
        escaped, mine, strlen(msg));
    close(fds[0]);
    close(fds[1]);

    errno = 0;
    mine = ft_write(-1, msg, strlen(msg));
    mine_errno = errno;
    errno = 0;
    real = write(-1, msg, strlen(msg));
    real_errno = errno;
    report((mine == -1) && (real == -1) && (mine_errno == real_errno),
        "%-18s " DIM "->" RESET " mine=%zd (errno=%d) real=%zd (errno=%d)",
        "fd=-1", mine, mine_errno, real, real_errno);
}

static void test_read(void) {
    char    *msg = "Hola mundo!\n";
    char    escaped[64];
    int     fds[2];
    char    buf_mine[64];
    char    buf_real[64];
    ssize_t mine;
    ssize_t real;
    int     mine_errno;
    int     real_errno;

    escape_str(escaped, msg, sizeof(escaped));

    title("ft_read");
    pipe(fds);
    write(fds[1], msg, strlen(msg));
    mine = ft_read(fds[0], buf_mine, sizeof(buf_mine));
    report((mine == (ssize_t)strlen(msg)) && (memcmp(buf_mine, msg, mine) == 0),
        "%-18s " DIM "->" RESET " mine=%zd real=%zu",
        escaped, mine, strlen(msg));
    close(fds[0]);
    close(fds[1]);

    errno = 0;
    mine = ft_read(-1, buf_mine, sizeof(buf_mine));
    mine_errno = errno;
    errno = 0;
    real = read(-1, buf_real, sizeof(buf_real));
    real_errno = errno;
    report((mine == -1) && (real == -1) && (mine_errno == real_errno),
        "%-18s " DIM "->" RESET " mine=%zd (errno=%d) real=%zd (errno=%d)",
        "fd=-1", mine, mine_errno, real, real_errno);
}

void test_strdup(void) {
    char    *tests[] = {"", "a", "Hola mundo!"};
    char    input[64];
    char    *mine;
    char    *real;
    int     n;
    int     i;

    title("ft_strdup");
    n = sizeof(tests) / sizeof(tests[0]);
    i = 0;
    while (i < n) {
        mine = ft_strdup(tests[i]);
        real = strdup(tests[i]);
        snprintf(input, sizeof(input), "\"%s\"", tests[i]);
        report((mine != NULL && real != NULL) && (strcmp(mine, real) == 0),
            "%-18s " DIM "->" RESET " mine=\"%s\" real=\"%s\"",
            input, mine ? mine : "NULL", real ? real : "NULL");
        free(mine);
        free(real);
        i++;
    }
}

static void summary(void) {
    printf("\n" BOLD "────────────────────────────────────────────────────────────" RESET "\n");
    if (g_pass == g_total) {
        printf("  " BOLD GREEN "RESULTADO: %d/%d PRUEBAS SUPERADAS (100%%) ✔" RESET "\n", g_pass, g_total);
    } else {
        printf("  " BOLD RED "RESULTADO: %d/%d SUPERADAS, %d FALLIDAS ✘" RESET "\n", g_pass, g_total, g_total - g_pass);
    }
    printf(BOLD "────────────────────────────────────────────────────────────" RESET "\n\n");
}

int main(void) {
    test_strlen();
    test_strcpy();
    test_strcmp();
    test_write();
    test_read();
    test_strdup();
    summary();
    return (g_pass != g_total);
}