#ifndef H_CCG
#define H_CCG

#include <argp.h>
#include <stdbool.h>

int main(int argc, char* argv[]);
static error_t parse_opt(int key, char *arg, struct argp_state *state);
int decompile(char* arch, char* output, char* path);

#endif