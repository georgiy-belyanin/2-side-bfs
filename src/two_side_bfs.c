#define LG_FREE_WORK                                                                                                   \
    { GrB_free(&frontier); }

#define LG_FREE_ALL                                                                                                    \
    {                                                                                                                  \
        LG_FREE_WORK;                                                                                                  \
        GrB_free(&l_parent);                                                                                           \
        GrB_free(&l_level);                                                                                            \
    }
#define GRB_CATCH(info)                                                                                                \
    {                                                                                                                  \
        printf("GraphBLAS failure (file %s, line %d): info: %d", __FILE__, __LINE__, info);                            \
        LG_FREE_ALL;                                                                                                   \
        return (info);                                                                                                 \
    }

#include <two_side_bfs.h>

#include <stdio.h>

int two_side_bfs(GrB_Vector *parent, const LAGraph_Graph G, GrB_Index src, GrB_Index dest, uint32_t path_length,
                 char *msg) {
    GrB_Vector frontier = NULL;
    GrB_Vector l_parent = NULL;
    GrB_Vector l_level = NULL;

    uint32_t front_length = path_length - 1;
    uint32_t back_length = path_length - front_length;

    GrB_Vector back_masks[10];

    *parent = NULL;

    LAGraph_CheckGraph(G, msg);

    GrB_Matrix A = G->A;

    GrB_Index n;
    GRB_TRY(GrB_Matrix_nrows(&n, A));

    GrB_Type int_type = (n > INT32_MAX) ? GrB_INT64 : GrB_INT32;
    GrB_Type bool_type = GrB_BOOL;
    GrB_BinaryOp second_op = (n > INT32_MAX) ? GrB_SECOND_INT64 : GrB_SECOND_INT32;
    GrB_Semiring semiring = NULL;
    GrB_Semiring bool_semiring = LAGraph_any_one_bool;
    GrB_IndexUnaryOp ramp = NULL;

    GRB_TRY(GrB_Vector_new(&l_parent, int_type, n));

    semiring = (n > INT32_MAX) ? GrB_MIN_FIRST_SEMIRING_INT64 : GrB_MIN_FIRST_SEMIRING_INT32;

    GRB_TRY(GrB_Vector_new(&frontier, int_type, n));
    GRB_TRY(GrB_Vector_setElement(frontier, src, src));

    ramp = (n > INT32_MAX) ? GrB_ROWINDEX_INT64 : GrB_ROWINDEX_INT32;

    GrB_Index nq = 1;
    GrB_Index last_nq = 0;
    GrB_Index current_level = 0;
    GrB_Index nvals = 1;

    for (int i = 0; i < back_length; i++) {
        GRB_TRY(GrB_Vector_new(&back_masks[i], GrB_BOOL, n));
    }
    GRB_TRY(GrB_Vector_setElement(back_masks[0], true, dest));

    GrB_Vector mask = l_parent;

    GRB_TRY(GrB_assign(l_parent, frontier, GrB_NULL, frontier, GrB_ALL, n, GrB_DESC_S));
    GRB_TRY(GrB_apply(frontier, GrB_NULL, GrB_NULL, ramp, frontier, 0, GrB_NULL));

    for (int i = 0; i < front_length; i++) {
        GRB_TRY(GrB_vxm(frontier, mask, GrB_NULL, semiring, frontier, A, GrB_DESC_RSC));
        GRB_TRY(GrB_assign(l_parent, frontier, GrB_NULL, frontier, GrB_ALL, n, GrB_DESC_S));
        GRB_TRY(GrB_apply(frontier, GrB_NULL, GrB_NULL, ramp, frontier, 0, GrB_NULL));
    }

    for (int i = 1; i < back_length; i++) {
        GRB_TRY(GrB_assign(back_masks[i], mask, GrB_PLUS_BOOL, back_masks[i - 1], GrB_ALL, n, GrB_DESC_SC));
        GRB_TRY(
            GrB_vxm(back_masks[i], back_masks[i - 1], GrB_PLUS_BOOL, semiring, back_masks[i - 1], A, GrB_DESC_SCT1));
    }

    for (int i = back_length - 1; i >= 0; i--) {
        GRB_TRY(GrB_vxm(frontier, back_masks[i], GrB_NULL, semiring, frontier, A, GrB_DESC_RS));
        GRB_TRY(GrB_assign(l_parent, frontier, GrB_NULL, frontier, GrB_ALL, n, GrB_DESC_S));
        GRB_TRY(GrB_apply(frontier, GrB_NULL, GrB_NULL, ramp, frontier, 0, GrB_NULL));
    }

    (*parent) = l_parent;
    LG_FREE_WORK;
    return GrB_SUCCESS;
}
