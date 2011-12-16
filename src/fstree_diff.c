#include "fsmonitor_private.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

fsdiff_t *fstree_diff(fstree_t *previous, fstree_t *current) {
  item_t *previtems = previous->items;
  item_t *curitems = current->items;
  int prevcount = previous->count;
  int curcount = current->count;

  fsdiff_t *diff = (fsdiff_t *) malloc(sizeof(fsdiff_t));
  diff->paths = (char **) malloc((prevcount + curcount) * sizeof(const char *));
  int diffcount = 0;

  int *corresponding = (int *)malloc(curcount * sizeof(int));
  int *rcorresponding = (int *)malloc(prevcount * sizeof(int));
  assert(corresponding);
  assert(rcorresponding);

  memset(corresponding, -1, curcount * sizeof(int));
  memset(rcorresponding, -1, prevcount * sizeof(int));

  corresponding[0] = 0;
  rcorresponding[0] = 0;

  int i = 1, j = 1;
  while (i < curcount && j < prevcount) {
    int cp = corresponding[curitems[i].parent];
    if (cp < 0) {
      //NSLog(@"%@ is a subitem of a new item", curitems[i].name);
      corresponding[i] = -1;
      ++i;
    } else if (previtems[j].parent < cp) {
      //NSLog(@"%@ is a deleted item", previtems[j].name);
      rcorresponding[j] = -1;
      ++j;
    } else if (previtems[j].parent > cp) {
      //NSLog(@"%@ is a new item", curitems[i].name);
      corresponding[i] = -1;
      ++i;
    } else {
      int r = strcmp(curitems[i].name, previtems[j].name);
      if (r == 0) {
        // same item! compare mod times
#ifdef _WIN32
        bool same = curitems[i].attr == previtems[j].attr && curitems[i].write_time.dwHighDateTime == previtems[j].write_time.dwHighDateTime && curitems[i].write_time.dwLowDateTime == previtems[j].write_time.dwLowDateTime;
#else
        bool same = curitems[i].st_mode == previtems[j].st_mode && curitems[i].st_dev == previtems[j].st_dev && curitems[i].st_ino == previtems[j].st_ino && curitems[i].st_mtimespec.tv_sec == previtems[j].st_mtimespec.tv_sec && curitems[i].st_mtimespec.tv_nsec == previtems[j].st_mtimespec.tv_nsec && curitems[i].st_ctimespec.tv_sec == previtems[j].st_ctimespec.tv_sec && curitems[i].st_ctimespec.tv_nsec == previtems[j].st_ctimespec.tv_nsec && curitems[i].st_size == previtems[j].st_size;
#endif
        if (same)
        {
          // unchanged
          //                    NSLog(@"%@ is unchanged item", curitems[i].name);
        } else {
          // changed
          //NSLog(@"%@ is changed item", curitems[i].name);
          if (FSTREE_ITEM_IS_REG(&curitems[i]) || FSTREE_ITEM_IS_REG(&previtems[j])) {
            diff->paths[diffcount++] = strdup(curitems[i].name);
          }
        }
        corresponding[i] = j;
        rcorresponding[j] = i;
        ++i;
        ++j;
      } else if (r > 0) {
        // i is after j => we need to advance j => j is deleted
        //NSLog(@"%@ is a deleted item", previtems[j].name);
        rcorresponding[j] = -1;
        ++j;
      } else /* if (r < 0) */ {
        // i is before j => we need to advance i => i is new
        //NSLog(@"%@ is a new item", curitems[i].name);
        corresponding[i] = -1;
        ++i;
      }
    }
  }
  // for any tail left, we've already filled it in with -1's

  for (i = 0; i < curcount; i++) {
    if (corresponding[i] < 0) {
      if (FSTREE_ITEM_IS_REG(&curitems[i])) {
        diff->paths[diffcount++] = strdup(curitems[i].name);
      }
    }
  }
  for (j = 0; j < prevcount; j++) {
    if (rcorresponding[j] < 0) {
      if (FSTREE_ITEM_IS_REG(&previtems[j])) {
        diff->paths[diffcount++] = strdup(previtems[i].name);
      }
    }
  }

  free(corresponding);
  free(rcorresponding);

  diff->count = diffcount;
  diff->paths = (char **)realloc(diff->paths, diffcount * sizeof(const char *));

  return diff;
}