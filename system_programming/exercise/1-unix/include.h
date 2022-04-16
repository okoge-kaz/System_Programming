# define errno_message(msg) do { fprintf(stderr, "%s: %s\n", msg, strerror(errno)); exit(EXIT_FAILURE); } while (0)
# define TEST_ASSERT(expr) do { if (!(expr)) { fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #expr); exit(EXIT_FAILURE); } } while (0)
