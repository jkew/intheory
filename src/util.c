#include <stdlib.h>

void discard(void *thing) {
  free(thing);
}
