#include <LAGraph.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <two_side_bfs.h>
#include <unistd.h>

#define CHECK_TRUE(cond)                                                                                               \
    do {                                                                                                               \
        if (cond)                                                                                                      \
            break;                                                                                                     \
        printf("Check failed " #cond ", fatal\n");                                                                     \
        exit(-1);                                                                                                      \
    } while (0)
#define CHECK_ZERO(expr)                                                                                               \
    do {                                                                                                               \
        if (expr == 0)                                                                                                 \
            break;                                                                                                     \
        printf("Result of " #expr " is not ok, fatal\n");                                                              \
        exit(-1);                                                                                                      \
    } while (0)

LAGraph_Graph G = NULL;

void usage() {
    printf("\ntwo-side-bfs\n"
           "\t[-r run count (default: 1000)]\n"
           "\t[-l minimum path length (default: 2)]\n"
           "\t[-m maximum path length (default: 5)]\n"
           "\t<graph.mtx>\n"
           "\t[results/]\n\n");
    exit(-2);
}

int main(int argc, char **argv) {
    int min_length = 2;
    int max_length = 5;
    int runs = 1000;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "l:m:r:")) != -1)
        switch (c) {
        case 'l':
            min_length = atoi(optarg);
            break;
        case 'm':
            max_length = atoi(optarg);
            break;
        case 'r':
            runs = atoi(optarg);
            break;
        case '?':
            if (optopt == 'l' || optopt == 'm' || optopt == 'r')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort();
        }

    if (argc - optind < 1)
        usage();

    char msg[512];

    LAGraph_Init(msg);
    GrB_Matrix A = NULL;

    srand(time(NULL));

    if (argc < 2)
        usage();

    const char *aname = argv[optind];
    LAGraph_Kind kind = LAGraph_ADJACENCY_DIRECTED;
    if (strlen(aname) == 0)
        return -1;

    printf("Running on a matrix %s\n", aname);
    FILE *f = fopen(aname, "r");
    CHECK_TRUE(f != NULL);
    CHECK_ZERO(LAGraph_MMRead(&A, f, msg));
    CHECK_ZERO(fclose(f));

    CHECK_ZERO(LAGraph_New(&G, &A, kind, msg));
    CHECK_TRUE(A == NULL);

    GrB_Index n = 0;
    CHECK_ZERO(GrB_Matrix_nrows(&n, G->A));

    for (int path_length = max_length; path_length >= min_length; path_length--) {
        int count = 0;
        double elapsedV = 0.0;
        double timesV[runs + 1];
        for (int64_t i = 0; i < runs; i++, count++) {
            uint32_t src = rand() % n;
            uint32_t dest = rand() % n;

            GrB_Vector parent = NULL;

            int64_t maxlevel;
            GrB_Index nvisited;
            struct timespec start, finish;

            clock_gettime(CLOCK_MONOTONIC, &start);
            two_side_bfs(&parent, G, src, dest, path_length, msg);
            clock_gettime(CLOCK_MONOTONIC, &finish);

            printf("%d %d: checked\n", src + 1, dest + 1);

            elapsedV = (finish.tv_sec - start.tv_sec);
            elapsedV += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
            timesV[count] = elapsedV;

            CHECK_ZERO(GrB_free(&parent));
        }

        double meanV = 0.0;

        for (int i = 0; i < runs; i++)
            meanV += timesV[i];
        meanV /= runs;

        double stdenvV = 0.0;
        for (int i = 0; i < runs; i++)
            stdenvV += (timesV[i] - meanV) * (timesV[i] - meanV);
        stdenvV = stdenvV / runs;

        printf("Path length: %d\n", path_length);
        printf("Modified      : mean %fs, std-env%fs\n", meanV, stdenvV);

        // TBD: file output
        // FILE *f = fopen(argc < 2
        //                : "results/"
        //                  "") CHECK_TRUE(f != NULL);

        // for (int i = 0; i < runs; i++)
        //     fprintf(f, "%f, ", timesV[i]);

        // CHECK_ZERO(fclose(f));
    }

    CHECK_ZERO(LAGraph_Delete(&G, msg));

    LAGraph_Finalize(msg);
}
