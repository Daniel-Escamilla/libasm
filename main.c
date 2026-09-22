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

size_t  ft_strlen(const char *str);
char    *ft_strcpy(char *dest, const char *src);
int     ft_strcmp(const char *str1, const char *str2);
ssize_t ft_write(int fdd, const void *buf, size_t count);
ssize_t ft_read(int fdd, void *buf, size_t count);
char    *ft_strdup(const char *str);

static int g_pass, g_total;

static void title(const char *name) {
    static int first = 1;
    printf("%s" BOLD CYAN "─── [ %s ] ─────────────" RESET "\n", first ? "" : "\n", name);
    first = 0;
}

static void report(int is_ok, const char *fmt, ...) {
    va_list args;
    char    buf[256];

    g_total++; g_pass += (is_ok != 0);
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    printf("  %-68s %s\n", buf, is_ok ? SQ_OK : SQ_FAIL);
}

static void format_str(char *dst, const char *src, size_t size) {
    if (!src) { snprintf(dst, size, "NULL"); return; }
    if (strlen(src) > 15) { snprintf(dst, size, "\"%.10s...\"", src); return; }
    size_t idx_j = 0;
    dst[idx_j++] = '"';
    for (size_t idx_i = 0; src[idx_i] && idx_j + 5 < size; idx_i++) {
        unsigned char chr = (unsigned char)src[idx_i];
        if (chr == '\n') { dst[idx_j++] = '\\'; dst[idx_j++] = 'n'; }
        else if (chr == '\t') { dst[idx_j++] = '\\'; dst[idx_j++] = 't'; }
        else if (chr < 32 || chr >= 127) { idx_j += snprintf(dst + idx_j, size - idx_j, "\\x%02x", chr); }
        else dst[idx_j++] = chr;
    }
    dst[idx_j++] = '"'; dst[idx_j] = '\0';
}

void test_strlen(void) {
    char long_str[10001]; memset(long_str, 'A', 10000); long_str[10000] = '\0';
    char *tests[] = {"", "a", "Hola mundo!\n", long_str, NULL};
    char input[64];

    title("ft_strlen");
    for (int idx = 0; tests[idx]; idx++) {
        size_t mine = ft_strlen(tests[idx]), real = strlen(tests[idx]);
        format_str(input, tests[idx], sizeof(input));
        report(mine == real, "%-18s " DIM "->" RESET " mine=%zu real=%zu", input, mine, real);
    }
}

void test_strcpy(void) {
    char *tests[] = {"", "a", "Hola mundo!\n", NULL};
    char buf1[64], buf2[64], input[64], out[64];

    title("ft_strcpy");
    for (int idx = 0; tests[idx]; idx++) {
        char *res1 = ft_strcpy(buf1, tests[idx]);
        strcpy(buf2, tests[idx]);
        format_str(input, tests[idx], sizeof(input));
        format_str(out, buf1, sizeof(out));
        report(!strcmp(buf1, buf2) && res1 == buf1, "%-18s " DIM "->" RESET " mine=%s ret_ok=%s",
               input, out, (res1 == buf1) ? "yes" : "no");
    }
}

void test_strcmp(void) {
    char *arr1[] = {"abc", "abc", "abc", "a", "", "\x80", NULL};
    char *arr2[] = {"abc", "abd", "ab",  "abc", "", "a",    NULL};
    char str1[32], str2[32], cmp[128];

    title("ft_strcmp");
    for (int idx = 0; arr1[idx]; idx++) {
        int mine_res = ft_strcmp(arr1[idx], arr2[idx]), real_res = strcmp(arr1[idx], arr2[idx]);
        format_str(str1, arr1[idx], sizeof(str1));
        format_str(str2, arr2[idx], sizeof(str2));
        snprintf(cmp, sizeof(cmp), "%s vs %s", str1, str2);
        int is_ok = (mine_res < 0 && real_res < 0) || (mine_res > 0 && real_res > 0) || (mine_res == 0 && real_res == 0);
        report(is_ok, "%-18s " DIM "->" RESET " mine=%d real=%d", cmp, mine_res, real_res);
    }
}

static void test_write_case(const void *buf, size_t count, int custom_fd, const char *label) {
    int fds[2]; pipe(fds);
    int curr_fd = custom_fd ? custom_fd : fds[1];

    errno = 0; ssize_t mine_res = ft_write(curr_fd, buf, count); int m_err = errno;
    errno = 0; ssize_t real_res = write(curr_fd, buf, count);    int r_err = errno;
    close(fds[0]); close(fds[1]);

    if (m_err || r_err)
        report(mine_res == real_res && m_err == r_err, "%-18s " DIM "->" RESET " mine=%zd (errno=%d) real=%zd (errno=%d)", label, mine_res, m_err, real_res, r_err);
    else
        report(mine_res == real_res, "%-18s " DIM "->" RESET " mine=%zd real=%zd", label, mine_res, real_res);
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
    int curr_fd = custom_fd ? custom_fd : fds[0];

    if (feed) (void)write(fds[1], feed, strlen(feed));
    errno = 0; ssize_t mine_res = ft_read(curr_fd, buf, count); int m_err = errno;
    close(fds[0]); close(fds[1]);

    pipe(fds);
    curr_fd = custom_fd ? custom_fd : fds[0];
    if (feed) (void)write(fds[1], feed, strlen(feed));
    errno = 0; ssize_t real_res = read(curr_fd, buf, count); int r_err = errno;
    close(fds[0]); close(fds[1]);

    if (m_err || r_err)
        report(mine_res == real_res && m_err == r_err, "%-18s " DIM "->" RESET " mine=%zd (errno=%d) real=%zd (errno=%d)", label, mine_res, m_err, real_res, r_err);
    else
        report(mine_res == real_res, "%-18s " DIM "->" RESET " mine=%zd real=%zd", label, mine_res, real_res);
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
    for (int idx = 0; tests[idx]; idx++) {
        char *mine_str = ft_strdup(tests[idx]), *real_str = strdup(tests[idx]);
        format_str(input, tests[idx], sizeof(input));
        format_str(out, mine_str, sizeof(out));
        report(mine_str && real_str && !strcmp(mine_str, real_str) && mine_str != tests[idx],
               "%-18s " DIM "->" RESET " mine=%s ptr_diff=%s",
               input, out, (mine_str != tests[idx]) ? "yes" : "no");
        free(mine_str); free(real_str);
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