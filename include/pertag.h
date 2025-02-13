#ifndef PERTAG_H
#define PERTAG_H

#include "types.h"


/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (7)


struct Pertag {
  unsigned int curtag, prevtag;      /* current and previous tag */
  int nmasters[TAGCOUNT + 1];        /* number of windows in master area */
  float mfacts[TAGCOUNT + 1];        /* mfacts per tag */
  unsigned int sellts[TAGCOUNT + 1]; /* selected layouts */
  const Layout
      *ltidxs[TAGCOUNT + 1][2]; /* matrix of tags and layouts indexes  */
};

#endif // !PERTAG_H

