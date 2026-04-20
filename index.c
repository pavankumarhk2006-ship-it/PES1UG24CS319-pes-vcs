#include "index.h"
#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int index_load(Index *index) {
    FILE *fp = fopen(".pes/index", "r");

    if (!fp) {
        index->count = 0;
        return 0;
    }

    index->count = 0;

    while (index->count < MAX_INDEX_ENTRIES) {
        IndexEntry *e = &index->entries[index->count];

        char hash_hex[65];

        if (fscanf(fp, "%o %64s %lu %u %[^\n]\n",
                   &e->mode,
                   hash_hex,
                   &e->mtime_sec,
                   &e->size,
                   e->path) != 5) {
            break;
        }

        for (int i = 0; i < HASH_SIZE; i++) {
            sscanf(hash_hex + (i * 2), "%2hhx", &e->hash.hash[i]);
        }

        index->count++;
    }

    fclose(fp);
    return 0;
}

int index_save(const Index *index) {
    mkdir(".pes", 0755);

    FILE *fp = fopen(".pes/index", "w");
    if (!fp) return -1;

    for (int i = 0; i < index->count; i++) {
        const IndexEntry *e = &index->entries[i];

        char hash_hex[65];
        hash_to_hex(&e->hash, hash_hex);

        fprintf(fp, "%o %s %lu %u %s\n",
                e->mode,
                hash_hex,
                e->mtime_sec,
                e->size,
                e->path);
    }

    fclose(fp);
    return 0;
}

IndexEntry* index_find(Index *index, const char *path) {
    for (int i = 0; i < index->count; i++) {
        if (strcmp(index->entries[i].path, path) == 0) {
            return &index->entries[i];
        }
    }
    return NULL;
}

int index_add(Index *index, const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    void *data = malloc(size);
    if (!data) {
        fclose(fp);
        return -1;
    }

    fread(data, 1, size, fp);
    fclose(fp);

    ObjectID id;
    if (object_write(OBJ_BLOB, data, size, &id) != 0) {
        free(data);
        return -1;
    }

    free(data);

    struct stat st;
    stat(path, &st);

    IndexEntry *e = index_find(index, path);

    if (!e) {
        e = &index->entries[index->count++];
    }

    e->mode = get_file_mode(path);
    e->hash = id;
    e->mtime_sec = st.st_mtime;
    e->size = size;

    strncpy(e->path, path, sizeof(e->path) - 1);
    e->path[sizeof(e->path) - 1] = '\0';

    return 0;
}

int index_remove(Index *index, const char *path) {
    for (int i = 0; i < index->count; i++) {
        if (strcmp(index->entries[i].path, path) == 0) {
            for (int j = i; j < index->count - 1; j++) {
                index->entries[j] = index->entries[j + 1];
            }
            index->count--;
            return 0;
        }
    }
    return -1;
}

int index_status(const Index *index) {
    printf("Staged files:\n");
    for (int i = 0; i < index->count; i++) {
        printf("  %s\n", index->entries[i].path);
    }
    return 0;
}
