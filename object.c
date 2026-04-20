#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include "pes.h"

void hash_to_hex(const ObjectID *id, char *hex_out) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex_out + (i * 2), "%02x", id->hash[i]);
    }
    hex_out[64] = '\0';
}

int hex_to_hash(const char *hex, ObjectID *id_out) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        if (sscanf(hex + (i * 2), "%2hhx", &id_out->hash[i]) != 1) {
            return -1;
        }
    }
    return 0;
}

void object_path(const ObjectID *id, char *path) {
    char hex[65];
    hash_to_hex(id, hex);

    char dir[256];
    snprintf(dir, sizeof(dir), ".pes/objects/%.2s", hex);

    mkdir(".pes", 0755);
    mkdir(".pes/objects", 0755);
    mkdir(dir, 0755);

    snprintf(path, 256, "%s/%s", dir, hex + 2);
}

int object_write(ObjectType type, const void *data, size_t len, ObjectID *id_out) {
    const char *type_str = (type == OBJ_BLOB) ? "blob" :
                           (type == OBJ_TREE) ? "tree" : "commit";

    char header[64];
    int header_len = snprintf(header, sizeof(header), "%s %zu", type_str, len);
    header[header_len] = '\0';
    header_len += 1;

    size_t total_size = header_len + len;

    unsigned char *buffer = malloc(total_size);
    if (!buffer) return -1;

    memcpy(buffer, header, header_len);
    memcpy(buffer + header_len, data, len);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(buffer, total_size, hash);

    memcpy(id_out->hash, hash, SHA256_DIGEST_LENGTH);

    char path[256];
    object_path(id_out, path);

    FILE *fp = fopen(path, "wb");
    if (!fp) {
        free(buffer);
        return -1;
    }

    fwrite(buffer, 1, total_size, fp);
    fclose(fp);
    free(buffer);

    return 0;
}

int object_read(const ObjectID *id, ObjectType *type_out, void **data_out, size_t *len_out) {
    char path[256];
    object_path(id, path);

    FILE *fp = fopen(path, "rb");
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    unsigned char *buffer = malloc(file_size);
    if (!buffer) {
        fclose(fp);
        return -1;
    }

    fread(buffer, 1, file_size, fp);
    fclose(fp);

    unsigned char computed_hash[SHA256_DIGEST_LENGTH];
    SHA256(buffer, file_size, computed_hash);

    if (memcmp(computed_hash, id->hash, SHA256_DIGEST_LENGTH) != 0) {
        free(buffer);
        return -1;
    }

    unsigned char *null_pos = memchr(buffer, '\0', file_size);
    if (!null_pos) {
        free(buffer);
        return -1;
    }

    size_t header_len = null_pos - buffer + 1;

    char type_str[16];
    sscanf((char *)buffer, "%15s %zu", type_str, len_out);

    if (strcmp(type_str, "blob") == 0) *type_out = OBJ_BLOB;
    else if (strcmp(type_str, "tree") == 0) *type_out = OBJ_TREE;
    else *type_out = OBJ_COMMIT;

    *data_out = malloc(*len_out);
    if (!(*data_out)) {
        free(buffer);
        return -1;
    }

    memcpy(*data_out, buffer + header_len, *len_out);

    free(buffer);
    return 0;
}
