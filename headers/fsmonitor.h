#ifndef FSMONITOR_H_INCLUDED
#define FSMONITOR_H_INCLUDED

/**************************************************************/

typedef struct fsfilter_t fsfilter_t;

fsfilter_t *fsfilter_create(const char **included_extensions, const char **excluded_paths);
void fsfilter_free(fsfilter_t *filter);

/**************************************************************/

typedef struct fsdiff_t fsdiff_t;

void fsdiff_free(fsdiff_t *diff);

int fsdiff_count(fsdiff_t *diff);
const char *fsdiff_get(fsdiff_t *diff, int index);
void fsdiff_dump(fsdiff_t *diff);

/**************************************************************/

typedef struct fstree_t fstree_t;

fstree_t *fstree_create(const char *path, fsfilter_t *filter, fstree_t *previous);
void fstree_free(fstree_t *tree);

void fstree_dump(fstree_t *tree);

/**************************************************************/

fsdiff_t *fstree_diff(fstree_t *previous, fstree_t *current);

/**************************************************************/

typedef struct fsmonitor_t fsmonitor_t;

typedef void (*fsmonitor_callback_t)(fsdiff_t *diff, fstree_t *current, void *data);

fsmonitor_t *fsmonitor_create(const char *path, fsfilter_t *filter, fsmonitor_callback_t callback, void *data);
void fsmonitor_free(fsmonitor_t *monitor);

#endif