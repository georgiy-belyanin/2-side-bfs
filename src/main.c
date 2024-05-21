#include <LAGraph.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <two_side_bfs.h>
#define AMOUNT 100

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
    printf("two-side-bfs <graph.mtx> [results-dir/]\n");
    exit(-2);
}

int main(int argc, char **argv) {
    char msg[512];

    LAGraph_Init(msg);
    GrB_Matrix A = NULL;

    srand(time(NULL));

    if (argc < 2)
        usage();

    const char *aname = argv[1];
    LAGraph_Kind kind = LAGraph_ADJACENCY_DIRECTED;
    if (strlen(aname) == 0)
        return -1;

    printf("\nMatrix: %s\n", aname);
    FILE *f = fopen(aname, "r");
    CHECK_TRUE(f != NULL);
    CHECK_ZERO(LAGraph_MMRead(&A, f, msg));
    printf("\n%s\n", msg);
    CHECK_ZERO(fclose(f));

    CHECK_ZERO(LAGraph_New(&G, &A, kind, msg));
    CHECK_TRUE(A == NULL);

    GrB_Index n = 0;
    CHECK_ZERO(GrB_Matrix_nrows(&n, G->A));

    for (int path_length = 5; path_length >= 2; path_length--) {
        int count = 0;
        double elapsedV = 0.0;
        double timesV[AMOUNT + 1];
        for (int64_t i = 0; i < AMOUNT; i++, count++) {
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

        for (int i = 0; i < AMOUNT; i++)
            meanV += timesV[i];
        meanV /= AMOUNT;

        double stdenvV = 0.0;
        for (int i = 0; i < AMOUNT; i++)
            stdenvV += (timesV[i] - meanV) * (timesV[i] - meanV);
        stdenvV = stdenvV / AMOUNT;

        printf("Path length: %d\n", path_length);
        printf("Modified      : mean %fs, std-env%fs\n", meanV, stdenvV);

        // TBD: file output
        // FILE *f = fopen(argc < 2
        //                : "results/"
        //                  "") CHECK_TRUE(f != NULL);

        // for (int i = 0; i < AMOUNT; i++)
        //     fprintf(f, "%f, ", timesV[i]);

        // CHECK_ZERO(fclose(f));
    }

    CHECK_ZERO(LAGraph_Delete(&G, msg));

    LAGraph_Finalize(msg);
}
