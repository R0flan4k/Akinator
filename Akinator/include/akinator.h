#ifndef AKINATOR_H
    #define AKINATOR_H

    #include "tree.h"

    typedef int AError_t;

    enum AkinatorErrorsMasks {
        AKINATOR_ERRORS_CANT_CONVERT_TEXT_FILE = 1 << 0,
        AKINATOR_ERRORS_INVALID_SYNTAXIS       = 1 << 1,
        AKINATOR_ERRORS_TREE_ERROR             = 1 << 2,
    };

    AError_t create_akinator_tree(Tree * tree, char * buffer);
    // AError_t akinator_start_game(Tree * tree);

#endif
