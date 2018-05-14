#include <time.h>

using namespace std;

static unsigned int g_seed;

inline void sfast_rand(int seed = time(NULL)) {
	g_seed = seed;
}

inline int fast_rand() {
	g_seed = (214013 * g_seed + 2531011);

  return (g_seed >> 16) & 0x7FFF;
}

