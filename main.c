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

static int g_pass, g_total;

static void title(const char *name) {
    static int first = 1;
    printf("%s" BOLD CYAN "─── [ %s ] ─────────────" RESET "\n", first ? "" : "\n", name);
    first = 0;
}

static void report(int ok, const char *fmt, ...) {
    va_list ap;
    char    buf[256];

    g_total++; g_pass += (ok != 0);
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    printf("  %-68s %s\n", buf, ok ? SQ_OK : SQ_FAIL);
}

static void format_str(char *dst, const char *src, size_t sz) {
    if (!src) { snprintf(dst, sz, "NULL"); return; }
    if (strlen(src) > 15) { snprintf(dst, sz, "\"%.10s...\"", src); return; }
    size_t j = 0;
    dst[j++] = '"';
    for (size_t i = 0; src[i] && j + 5 < sz; i++) {
        unsigned char c = (unsigned char)src[i];
        if (c == '\n') { dst[j++] = '\\'; dst[j++] = 'n'; }
        else if (c == '\t') { dst[j++] = '\\'; dst[j++] = 't'; }
        else if (c < 32 || c >= 127) { j += snprintf(dst + j, sz - j, "\\x%02x", c); }
        else dst[j++] = c;
    }
    dst[j++] = '"'; dst[j] = '\0';
}

void test_strlen(void) {
    char long_str[10001]; memset(long_str, 'A', 10000); long_str[10000] = '\0';
    char *tests[] = {"", "a", "Hola mundo!\n", long_str, NULL};
    char input[64];

    title("ft_strlen");
    for (int i = 0; tests[i]; i++) {
        size_t mine = ft_strlen(tests[i]), real = strlen(tests[i]);
        format_str(input, tests[i], sizeof(input));
        report(mine == real, "%-18s " DIM "->" RESET " mine=%zu real=%zu", input, mine, real);
    }
}

void test_strcpy(void) {
    char *tests[] = {"", "a", "Hola mundo!\n", NULL};
    char b1[64], b2[64], input[64], out[64];

    title("ft_strcpy");
    for (int i = 0; tests[i]; i++) {
        char *r1 = ft_strcpy(b1, tests[i]), *r2 = strcpy(b2, tests[i]);
        format_str(input, tests[i], sizeof(input));
        format_str(out, b1, sizeof(out));
        report(!strcmp(b1, b2) && r1 == b1, "%-18s " DIM "->" RESET " mine=%s ret_ok=%s",
               input, out, (r1 == b1) ? "yes" : "no");
    }
}

void test_strcmp(void) {
    char *s1[] = {"abc", "abc", "abc", "a", "", "\x80", NULL};
    char *s2[] = {"abc", "abd", "ab",  "abc", "", "a",    NULL};
    char str1[32], str2[32], cmp[128]; /* Aumentado a 128 para evitar truncamiento */

    title("ft_strcmp");
    for (int i = 0; s1[i]; i++) {
        int m = ft_strcmp(s1[i], s2[i]), r = strcmp(s1[i], s2[i]);
        format_str(str1, s1[i], sizeof(str1));
        format_str(str2, s2[i], sizeof(str2));
        snprintf(cmp, sizeof(cmp), "%s vs %s", str1, str2);
        int ok = (m < 0 && r < 0) || (m > 0 && r > 0) || (m == 0 && r == 0);
        report(ok, "%-18s " DIM "->" RESET " mine=%d real=%d", cmp, m, r);
    }
}

static void test_write_case(const void *buf, size_t count, int custom_fd, const char *label) {
    int fds[2]; pipe(fds);
    int fd = custom_fd ? custom_fd : fds[1];

    errno = 0; ssize_t m = ft_write(fd, buf, count); int m_err = errno;
    errno = 0; ssize_t r = write(fd, buf, count);    int r_err = errno;
    close(fds[0]); close(fds[1]);

    if (m_err || r_err)
        report(m == r && m_err == r_err, "%-18s " DIM "->" RESET " mine=%zd (errno=%d) real=%zd (errno=%d)", label, m, m_err, r, r_err);
    else
        report(m == r, "%-18s " DIM "->" RESET " mine=%zd real=%zd", label, m, r);
}

void test_write(void) {
    title("ft_write");
    test_write_case("Hola mundo!\n", 12, 0, "\"Hola mundo!\\n\"");
    test_write_case("Hola", 0, 0, "count=0");
    test_write_case("Hola", 4, -1, "fd=-1");
    test_write_case(NULL, 5, 0, "buf=NULL");
}

static void test_read_case(const char *feed, size_t count, void *buf, int custom_fd, const char *label) {
    int fds[2]; pipe(fds);
    int fd = custom_fd ? custom_fd : fds[0];

    if (feed) (void)write(fds[1], feed, strlen(feed));
    errno = 0; ssize_t m = ft_read(fd, buf, count); int m_err = errno;
    close(fds[0]); close(fds[1]);

    pipe(fds);
    fd = custom_fd ? custom_fd : fds[0];
    if (feed) (void)write(fds[1], feed, strlen(feed));
    errno = 0; ssize_t r = read(fd, buf, count); int r_err = errno;
    close(fds[0]); close(fds[1]);

    if (m_err || r_err)
        report(m == r && m_err == r_err, "%-18s " DIM "->" RESET " mine=%zd (errno=%d) real=%zd (errno=%d)", label, m, m_err, r, r_err);
    else
        report(m == r, "%-18s " DIM "->" RESET " mine=%zd real=%zd", label, m, r);
}

void test_read(void) {
    char buf[64];
    title("ft_read");
    test_read_case("Hola!\n", 6, buf, 0, "\"Hola!\\n\"");
    test_read_case("Hola", 0, buf, 0, "count=0");
    test_read_case("Hola", 6, buf, -1, "fd=-1");
    test_read_case("Hola", 5, NULL, 0, "buf=NULL");
}

void test_strdup(void) {
    char *tests[] = {"", "a", "Hola mundo!\n", NULL};
    char input[64], out[64];

    title("ft_strdup");
    for (int i = 0; tests[i]; i++) {
        char *m = ft_strdup(tests[i]), *r = strdup(tests[i]);
        format_str(input, tests[i], sizeof(input));
        format_str(out, m, sizeof(out));
        report(m && r && !strcmp(m, r) && m != tests[i],
               "%-18s " DIM "->" RESET " mine=%s ptr_diff=%s",
               input, out, (m != tests[i]) ? "yes" : "no");
        free(m); free(r);
    }
}

static void summary(void) {
    printf("\n" BOLD "────────────────────────────────────────────────────────────" RESET "\n");
    if (g_pass == g_total)
        printf("  " BOLD GREEN "RESULTADO: %d/%d PRUEBAS SUPERADAS (100%%) ✔" RESET "\n", g_pass, g_total);
    else
        printf("  " BOLD RED "RESULTADO: %d/%d SUPERADAS, %d FALLIDAS ✘" RESET "\n", g_pass, g_total, g_total - g_pass);
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