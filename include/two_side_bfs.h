#pragma once

#include <LAGraph.h>

int two_side_bfs(GrB_Vector *parent, const LAGraph_Graph G, GrB_Index src, GrB_Index dest, uint32_t path_length,
                 char *msg);
