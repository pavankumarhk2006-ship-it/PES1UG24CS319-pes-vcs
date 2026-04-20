#include <stdio.h>
#include <string.h>
#include "index.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: pes <command> [args]\n");
        return 1;
    }

    // -------- ADD --------
    if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: pes add <file>\n");
            return 1;
        }

        Index index;

        if (index_load(&index) != 0) {
            printf("Error loading index\n");
            return 1;
        }

        for (int i = 2; i < argc; i++) {
            if (index_add(&index, argv[i]) != 0) {
                printf("Failed to add %s\n", argv[i]);
                return 1;
            }
        }

        if (index_save(&index) != 0) {
            printf("Error saving index\n");
            return 1;
        }

        return 0;
    }

    // -------- STATUS --------
    if (strcmp(argv[1], "status") == 0) {
        Index index;

        if (index_load(&index) != 0) {
            printf("Error loading index\n");
            return 1;
        }

        index_status(&index);
        return 0;
    }

    // -------- UNKNOWN --------
    printf("Unknown command: %s\n", argv[1]);
    return 1;
}
