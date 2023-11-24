#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "akinator.h"
#include "akinator_replics.h"
#include "tree.h"
#include "strings.h"
#include "my_assert.h"
#include "file_processing.h"

const size_t MAX_STRING_SIZE = 64;
const Tree_t AKINATOR_TRASH_VALUE = "nill";
const char * AKINATOR_FUNCTIONS = "QGM";
const char * AKINATOR_SUPPORTED_ANSWERS = "YN";
const char * AKINATOR_DUMP_FILE_NAME = "./graphviz/akinator_dump";

static AError_t create_akinator_nodes_recursive(Tree * tree, TreeNode * node, const char * * buffer_ptr);
static bool is_open_braket(const char * buffer_ptr);
static bool is_close_braket(const char * buffer_ptr);
static void output_message(const char * message);
static char get_answer(const char * posible_answers, size_t size);
static AkinatorFunctions akinator_chose_function(void);
static AError_t akinator_guess(Tree * tree);
static AError_t recursive_guessing(Tree * tree, TreeNode * node);
static AkinatorAnswers akinator_get_answer(void);
static AError_t akinator_add_object(Tree * tree, TreeNode * node, char * new_object_name,
                                    char * last_object_name, char * difference);
static void akinator_print_tree_nodes(const TreeNode * node, FILE * fp);
static void akinator_print_tree_edges(const TreeNode * node, FILE * fp);


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


static bool is_open_braket(const char * buffer_ptr)
{
    return *buffer_ptr == '{';
}


static bool is_close_braket(const char * buffer_ptr)
{
    return *buffer_ptr == '}';
}


AError_t akinator_start_game(Tree * tree)
{
    MY_ASSERT(tree);

    AError_t aktor_errors = 0;

    output_message(ENG_AKINATOR_REPLICS.say_hello);
    output_message(ENG_AKINATOR_REPLICS.show_menu);

    AkinatorFunctions choosen_function = AKINATOR_FUNCTIONS_NOTHING;

    while ((choosen_function = akinator_chose_function()) != AKINATOR_FUNCTIONS_QUIT)
    {

        switch (choosen_function)
        {
            case AKINATOR_FUNCTIONS_GUESS:
                output_message(ENG_AKINATOR_REPLICS.start_guessing);

                if (aktor_errors = akinator_guess(tree))
                {
                    return aktor_errors;
                }
                break;

            case AKINATOR_FUNCTIONS_MAKE_DUMP:
                akinator_dump(tree);
                output_message(ENG_AKINATOR_REPLICS.dump_maked);

                break;

            case AKINATOR_FUNCTIONS_NOTHING:
                break;

            case AKINATOR_FUNCTIONS_QUIT:
                MY_ASSERT(0 && "UNREACHABLE AKINATOR_FUNCTIONS_QUIT");
                break;

            default:
                MY_ASSERT(0 && "UNREACHABLE");
                break;
        }

        output_message(ENG_AKINATOR_REPLICS.show_menu);
    }

    output_message(ENG_AKINATOR_REPLICS.say_goodbye);

    return aktor_errors;
}


static void output_message(const char * message)
{
    MY_ASSERT(message);

    printf("%s", message);

    return;
}


static char get_answer(const char * posible_answers, size_t size)
{
    MY_ASSERT(posible_answers);

    char ans = 0;
    char extra_char = 0;

    while ((ans = (char) getchar()) != EOF && isspace(ans))
        continue;

    ans = (char) toupper(ans);

    while ((extra_char = (char) getchar()) != '\n' && extra_char != EOF)
    {
        if (!isspace(extra_char))
            return (char) NULL;
    }

    for (size_t i = 0; i < size; i++)
    {
        if (ans == posible_answers[i])
            return ans;
    }

    return (char) NULL;
}


static AkinatorFunctions akinator_chose_function(void)
{
    char character = get_answer(AKINATOR_FUNCTIONS, strlen(AKINATOR_FUNCTIONS));

    switch (character)
    {
        case 'Q':
            return AKINATOR_FUNCTIONS_QUIT;
            break;

        case 'G':
            return AKINATOR_FUNCTIONS_GUESS;
            break;

        case 'M':
            return AKINATOR_FUNCTIONS_MAKE_DUMP;
            break;

        case (char) NULL:
            return AKINATOR_FUNCTIONS_NOTHING;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return AKINATOR_FUNCTIONS_NOTHING;
}


static AError_t akinator_guess(Tree * tree)
{
    MY_ASSERT(tree);

    AError_t aktor_errors = 0;

    if (aktor_errors = recursive_guessing(tree, tree->root))
    {
        return aktor_errors;
    }

    return aktor_errors;
}


static AError_t recursive_guessing(Tree * tree, TreeNode * node)
{
    MY_ASSERT(tree);
    MY_ASSERT(node);

    AError_t aktor_errors = 0;
    char message[MAX_STRING_SIZE] = "";

    sprintf(message, "%s " TREE_SPEC "? ",
            ENG_AKINATOR_REPLICS.ask, node->value);

    output_message(message);
    output_message(ENG_AKINATOR_REPLICS.answer_help);

    AkinatorAnswers ans = AKINATOR_ANSWERS_INVALID;

    while ((ans = akinator_get_answer()) == AKINATOR_ANSWERS_INVALID)
        output_message(ENG_AKINATOR_REPLICS.answer_help);

    switch (ans)
    {
        case AKINATOR_ANSWERS_YES:
            if (!node->left)
            {
                output_message(ENG_AKINATOR_REPLICS.success);
                return aktor_errors;
            }

            if (aktor_errors = recursive_guessing(tree, node->left))
            {
                return aktor_errors;
            }

            break;

        case AKINATOR_ANSWERS_NO:
            if (!node->right)
            {
                output_message(ENG_AKINATOR_REPLICS.failure);

                char new_object_name[MAX_STR_SIZE] = "";
                scanf("%s", new_object_name);
                char last_object_name[MAX_STR_SIZE] = "";
                strncpy(last_object_name, node->value, MAX_STR_SIZE);

                char difference_message[MAX_STRING_SIZE] = "";
                sprintf(difference_message, "%s %s %s %s?\n",
                        ENG_AKINATOR_REPLICS.get_difference, new_object_name,
                        ENG_AKINATOR_REPLICS.and_word, last_object_name);
                output_message(difference_message);

                char difference[MAX_STR_SIZE] = "";
                scanf("%s", difference);

                if (aktor_errors = akinator_add_object(tree, node, new_object_name, last_object_name, difference))
                {
                    return aktor_errors;
                }

                output_message(ENG_AKINATOR_REPLICS.next_time);

                return aktor_errors;
            }

            if (aktor_errors = recursive_guessing(tree, node->right))
            {
                return aktor_errors;
            }

            break;

        case AKINATOR_ANSWERS_INVALID:
            MY_ASSERT(0 && "UNREACHABLE AKINATOR_ANSWER_INVALID");
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return aktor_errors;
}


static AkinatorAnswers akinator_get_answer(void)
{
    char character = get_answer(AKINATOR_SUPPORTED_ANSWERS, strlen(AKINATOR_SUPPORTED_ANSWERS));

    switch (character)
    {
        case 'Y':
            return AKINATOR_ANSWERS_YES;
            break;

        case 'N':
            return AKINATOR_ANSWERS_NO;
            break;

        case (char) NULL:
            return AKINATOR_ANSWERS_INVALID;
            break;

        default:
            MY_ASSERT(0 && "UNREACHABLE");
            break;
    }

    return AKINATOR_ANSWERS_INVALID;
}


static AError_t akinator_add_object(Tree * tree, TreeNode * node, char * new_object_name,
                                    char * last_object_name, char * difference)
{
    MY_ASSERT(tree);
    MY_ASSERT(node);
    MY_ASSERT(new_object_name);
    MY_ASSERT(last_object_name);
    MY_ASSERT(difference);

    TError_t tree_errors = 0;
    AError_t aktor_errors = 0;

    if ((tree_errors = tree_insert(tree, node, TREE_NODE_BRANCH_LEFT, new_object_name)) ||
        (tree_errors = tree_insert(tree, node, TREE_NODE_BRANCH_RIGHT, last_object_name)) ||
        (tree_errors = tree_set_node_value(node, difference)))
    {

        aktor_errors |= AKINATOR_ERRORS_TREE_ERROR;
        return aktor_errors;
    }

    return aktor_errors;
}


void akinator_dump(Tree * tree)
{
    MY_ASSERT(tree);

    FILE * fp = NULL;

    char dot_file_name[64] = "";
    make_file_extension(dot_file_name, AKINATOR_DUMP_FILE_NAME, ".dot");

    if (!(fp = file_open(dot_file_name, "wb")))
    {
        return;
    }

    fprintf(fp, "digraph G\n"
                "{\n"
                "    graph [dpi = 150]\n"
                "    ranksep = 0.6;\n"
                "    bgcolor = \"#f0faf0\"\n"
                "    splines = curved;\n"
                "    edge[minlen = 3];\n"
                "    node[shape = record, style = \"rounded\", color = \"#f58eb4\",\n"
                "        fixedsize = true, height = 1, width = 6, fontsize = 15];\n"
                "    {rank = min;\n"
                "        inv_min [style = invis];\n"
                "    }\n");

    if (tree->root)
    {
        akinator_print_tree_nodes(tree->root, fp);
        akinator_print_tree_edges(tree->root, fp);
    }

    fprintf(fp, "}");

    fclose(fp);

    static size_t akinator_dumps_count = 0;
    char png_dump_file_name[MAX_STRING_SIZE] = "";
    char command_string[MAX_STRING_SIZE] = "";
    char extension_string[MAX_STRING_SIZE] = "";

    sprintf(extension_string, "%zd.png", akinator_dumps_count);
    make_file_extension(png_dump_file_name, AKINATOR_DUMP_FILE_NAME, extension_string);
    sprintf(command_string, "dot %s -T png -o %s", dot_file_name, png_dump_file_name);
    system(command_string);

    akinator_dumps_count++;
}


static void akinator_print_tree_nodes(const TreeNode * node, FILE * fp)
{
    MY_ASSERT(node);

    if (node->left && node->right)
    {
        fprintf(fp, "    node%p [ label = \"{" TREE_SPEC "? | { <l> YES | NO  }}\" ]\n",
                node, node->value);
    }
    else
    {
        fprintf(fp, "    node%p [ label = \"{" TREE_SPEC "}\", color = green ]\n",
                node, node->value);
    }

    if (node->right)
    {
        akinator_print_tree_nodes(node->right, fp);
    }

    if (node->left)
    {
        akinator_print_tree_nodes(node->left, fp);
    }
}

static void akinator_print_tree_edges(const TreeNode * node, FILE * fp)
{
    MY_ASSERT(node);

    if (node->left)
    {
        fprintf(fp, "    node%p:<l> -> node%p;\n", node, node->left);
        akinator_print_tree_edges(node->left, fp);
    }

    if (node->right)
    {
        fprintf(fp, "    node%p:<r> -> node%p;\n", node, node->right);
        akinator_print_tree_edges(node->right, fp);
    }
}
