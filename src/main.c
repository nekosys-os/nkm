#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "options.h"
#include "version.h"
#include "parser.h"

void print_version() {
    printf("%s (%s) v%s by %s \n", NKM_NAME_LONG, NKM_NAME_SHORT, NKM_VERSION, NKM_AUTHOR);
}

void print_help() {
    printf("%s\n", NKM_NAME_LONG);
    printf("  nkm -version: Show version information\n");
    printf("  nkm -help: Show this help page\n");
    printf("  nkm <target>: Build a target in the current directory\n");
    printf("  nkm: Runs the default target in the current directory\n");
}

void run_builder(nkm_options *options) {
    printf("Building target %s in script %s...\n", options->target, options->build_file);

    if (access(options->build_file, F_OK) == -1) {
        printf("error: No build file found in current directory\n");
        return;
    }

    printf("%s\n", options->build_file);

    parser_ctx *ctx;
    int status = parser_init(options, &ctx);

    if (status != 0) {
        printf("error: Cannot open build file");
        return;
    }

    parser_parse(ctx);

    parser_destroy(&ctx);
}

int main(int argc, char **argv) {
    nkm_options options;

    char workingDir[PATH_MAX];
    getcwd(workingDir, sizeof(workingDir));
    options.working_dir = workingDir;

    char buildFile[strlen(workingDir) + strlen("Nekofile") + 2];
    strcpy(buildFile, workingDir);
    strcat(buildFile, "/Nekofile");
    options.build_file = buildFile;

    if (argc > 1) {
        const char *arg = argv[1];

        if (strcmp(arg, "-version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(arg, "-help") == 0) {
            print_help();
            return 0;
        } else {
            options.target = arg;
        }

    } else {
        options.target = NULL;
    }

    run_builder(&options);

    return 0;
}
