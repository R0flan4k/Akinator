#include <string.h>
#include "akinator.h"
#include "tree.h"
// #include "strings.h"
#include "my_assert.h"

const size_t MAX_STRING_SIZE = 64;
const Tree_t AKINATOR_TRASH_VALUE = "nill";

static AError_t create_akinator_nodes_recursive(Tree * tree, TreeNode * node, const char * * buffer_ptr);
bool is_open_braket(const char * buffer_ptr);
bool is_close_braket(const char * buffer_ptr);


AError_t create_akinator_tree(Tree * tree, char * buffer)
{
    MY_ASSERT(tree);
    MY_ASSERT(buffer);

    const char * buffer_ptr = buffer;
    AError_t aktor_errors = 0;

    buffer_ptr = skip_spaces(buffer_ptr);

    if (!is_open_braket(buffer_ptr))
    {
        aktor_errors |= AKINATOR_ERRORS_INVALID_SYNTAXIS;
        return aktor_errors;
    }
    buffer_ptr++;

    buffer_ptr = skip_spaces(buffer_ptr);

    if (aktor_errors = create_akinator_nodes_recursive(tree, tree->root, &buffer_ptr))
    {
        return aktor_errors;
    }

    return aktor_errors;
}


static AError_t create_akinator_nodes_recursive(Tree * tree, TreeNode * node, const char * * buffer_ptr)
{
    MY_ASSERT(buffer_ptr);
    MY_ASSERT(node);

    AError_t aktor_errors = 0;
    TError_t tree_errors  = 0;
    int token_size = 0;
    char value[MAX_STR_SIZE] = "";

    *buffer_ptr = skip_spaces(*buffer_ptr);

    if (is_open_braket(*buffer_ptr))
    {
        (*buffer_ptr)++;

        if (tree_errors = tree_insert(tree, node, TREE_NODE_BRANCH_LEFT, AKINATOR_TRASH_VALUE))
        {
            tree_dump(tree);
            aktor_errors |= AKINATOR_ERRORS_TREE_ERROR;
            return aktor_errors;
        }

        if (aktor_errors = create_akinator_nodes_recursive(tree, node->left, buffer_ptr))
        {
            return aktor_errors;
        }
    }

    *buffer_ptr = skip_spaces(*buffer_ptr);

    if (!sscanf(*buffer_ptr, TREE_SPEC "%n", value, &token_size))
    {
        aktor_errors |= AKINATOR_ERRORS_INVALID_SYNTAXIS;
        return aktor_errors;
    }
    tree_set_node_value(node, value);
    *buffer_ptr += token_size;

    *buffer_ptr = skip_spaces(*buffer_ptr);

    if (is_open_braket(*buffer_ptr))
    {
        (*buffer_ptr)++;

        if (tree_errors = tree_insert(tree, node, TREE_NODE_BRANCH_RIGHT, AKINATOR_TRASH_VALUE))
        {
            tree_dump(tree);
            aktor_errors |= AKINATOR_ERRORS_TREE_ERROR;
            return aktor_errors;
        }

        if (aktor_errors = create_akinator_nodes_recursive(tree, node->right, buffer_ptr))
        {
            return aktor_errors;
        }
    }

    *buffer_ptr = skip_spaces(*buffer_ptr);

    if (!is_close_braket(*buffer_ptr))
    {
        aktor_errors |= AKINATOR_ERRORS_INVALID_SYNTAXIS;
        return aktor_errors;
    }
    (*buffer_ptr)++;

    return aktor_errors;
}


bool is_open_braket(const char * buffer_ptr)
{
    return *buffer_ptr == '{';
}


bool is_close_braket(const char * buffer_ptr)
{
    return *buffer_ptr == '}';
}
