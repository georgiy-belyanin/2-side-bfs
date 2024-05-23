#define LG_FREE_WORK                                                                                                   \
    { GrB_free(&frontier); }

#define LG_FREE_ALL                                                                                                    \
    {                                                                                                                  \
        LG_FREE_WORK;                                                                                                  \
        GrB_free(&l_parent);                                                                                           \
    }
#define GRB_CATCH(info)                                                                                                \
    {                                                                                                                  \
        printf("GraphBLAS failure (file %s, line %d): info: %d", __FILE__, __LINE__, info);                            \
        LG_FREE_ALL;                                                                                                   \
        return (info);                                                                                                 \
    }

#include <two_side_bfs.h>

#include <stdio.h>

/// Алгоритм приблизительной оценки минимального числа путей между двумя узлами в графе.
/// Если путь есть, ответом гарантируется число большее 0 (минимальное число возможных путей между двумя узлами), если
/// пути нет - гарантируется возврат 0 В качестве аргументов передаётся граф, граф, в котором всё рёбра повернуты в
/// обратную сторону (то есть его матрица смежности транспонирована относительно первого), номер узла с которого
/// начинается путь, номер узла, в котором заканчивается путь, и, дополнительно, выходной аргумент сообщения о сбое
///
/// Алгоритм BFS с помощью линейной алгебры основан на том, что при умножении вектора с некоторыми "изначальными"
/// вершинами на матрицу смежности графа получается новый вектор из всех вершин, в которые приходят рёбра, исходящие из
/// "изначальных" вершин, что соответствует одному шагу обхода в ширину. Восстановление пути работает так, что во время
/// обхода формируется вектор, в котором для каждой вершины, в которую мы попали хранится информация об узле, из
/// которого мы попали в нёё.
///
/// Поскольку мы фиксируем целевую вершину, BFS запускается и от первой, и от последней вершины на половину максимальной
/// длины, а результаты работы обходов склеиваются в один.
///
/// Библиотека SuiteSparse:GraphBLAS предоставляет оптимизированные алгоритмы для работы с матрицами,
/// поэтому получившееся решение работает шустро.
int two_side_bfs(const LAGraph_Graph G, const LAGraph_Graph G_tr, GrB_Index src, GrB_Index dest, uint32_t path_length,
                 char *msg) {

    GrB_Vector frontier = NULL;    // Фронт обхода в прямую сторону
    GrB_Vector backier = NULL;     // Фронт обхода в обратную сторону
    GrB_Vector l_parent = NULL;    // Вектор для восстановления "родителей"
    GrB_Vector l_descedent = NULL; // Вектор для восстановления "потомков" при движении в обратную сторону

    uint32_t front_length = path_length / 2;
    uint32_t back_length = path_length - front_length;

    LAGraph_CheckGraph(G, msg);

    GrB_Matrix A = G->A;
    GrB_Matrix A_tr = G_tr->A;

    GrB_Index n;
    GRB_TRY(GrB_Matrix_nrows(&n, A));

    // Получение различных полуколец, в которых будет происходить умножение
    GrB_Type int_type = (n > INT32_MAX) ? GrB_INT64 : GrB_INT32;
    GrB_Type bool_type = GrB_BOOL;
    GrB_BinaryOp second_op = (n > INT32_MAX) ? GrB_SECOND_INT64 : GrB_SECOND_INT32;
    GrB_Semiring semiring = NULL;
    GrB_Semiring bool_semiring = LAGraph_any_one_bool;
    GrB_IndexUnaryOp ramp = NULL;

    // Инициализация векторов для восстановления путей
    GRB_TRY(GrB_Vector_new(&l_parent, int_type, n));
    GRB_TRY(GrB_Vector_new(&l_descedent, int_type, n));

    semiring = (n > INT32_MAX) ? GrB_MIN_FIRST_SEMIRING_INT64 : GrB_MIN_FIRST_SEMIRING_INT32;

    // Установить начальные элементы в оба фронта обхода, в прямом направлении начать обход с источника, в обратном - с
    // цели
    GRB_TRY(GrB_Vector_new(&frontier, int_type, n));
    GRB_TRY(GrB_Vector_setElement(frontier, src, src));

    GRB_TRY(GrB_Vector_new(&backier, int_type, n));
    GRB_TRY(GrB_Vector_setElement(backier, dest, dest));

    ramp = (n > INT32_MAX) ? GrB_ROWINDEX_INT64 : GrB_ROWINDEX_INT32;

    GrB_Vector mask = l_parent;

    // Добавить начальную вершину с
    GRB_TRY(GrB_assign(l_parent, frontier, GrB_NULL, frontier, GrB_ALL, n, GrB_DESC_S));
    GRB_TRY(GrB_apply(frontier, GrB_NULL, GrB_NULL, ramp, frontier, 0, GrB_NULL));

    for (int i = 0; i < front_length; i++) {
        // Основной шаг BFS в прямом направлении, получаемый при умножении ветора на матрицу смежности
        GRB_TRY(GrB_vxm(frontier, mask, GrB_NULL, semiring, frontier, A, GrB_DESC_RSC));
        // Поместить для каждой новой вершины в обходе номер родителя, из которого мы в нёё попали
        GRB_TRY(GrB_assign(l_parent, frontier, GrB_NULL, frontier, GrB_ALL, n, GrB_DESC_S));
        // Обновить номера
        GRB_TRY(GrB_apply(frontier, GrB_NULL, GrB_NULL, ramp, frontier, 0, GrB_NULL));
    }

    mask = l_descedent;
    // Конечную вершину добавить в набор вершин, из которых она достижима
    GRB_TRY(GrB_assign(l_descedent, backier, GrB_NULL, backier, GrB_ALL, n, GrB_DESC_S));
    GRB_TRY(GrB_apply(backier, GrB_NULL, GrB_NULL, ramp, backier, 0, GrB_NULL));
    for (int i = 0; i < back_length; i++) {
        // Шаг BFS в обратном направлении, получаемый при перемножении вектора обхода на транспонированную матрицу
        // смежности
        GRB_TRY(GrB_vxm(backier, mask, GrB_NULL, semiring, backier, A_tr, GrB_DESC_RSC));
        // Поместить для каждой новой вершины в обходе номер потомка, из которого мы в нёё попали
        GRB_TRY(GrB_assign(l_descedent, backier, GrB_NULL, backier, GrB_ALL, n, GrB_DESC_S));
        GRB_TRY(GrB_apply(backier, GrB_NULL, GrB_NULL, ramp, backier, 0, GrB_NULL));
    }

    // Соединить два фронта обхода
    GRB_TRY(GrB_assign(l_descedent, l_parent, GrB_NULL, l_descedent, GrB_ALL, n, GrB_DESC_RS));

    // Посчитать число вершин в полученном соединении
    uint64_t res;
    GRB_TRY(GrB_Vector_nvals(&res, l_descedent));

    GrB_free(&backier);
    GrB_free(&frontier);
    GrB_free(&l_parent);
    GrB_free(&l_descedent);
    return (res);
}
