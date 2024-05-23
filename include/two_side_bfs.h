#pragma once

#include <LAGraph.h>

int two_side_bfs(const LAGraph_Graph G, const LAGraph_Graph G_tr, GrB_Index src, GrB_Index dest, uint32_t path_length,
                 char *msg);
