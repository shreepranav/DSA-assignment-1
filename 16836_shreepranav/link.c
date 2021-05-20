#include <stdio.h>
#include <stdlib.h>

#define BETA 0.1
#define IX(n, i, j, k) (i * n * n + j * n + k)

typedef struct Node
{
    int dest;
    struct Node *next;
} Node;

typedef struct AdjList
{
    struct Node *head;
} AdjList;

typedef struct Graph
{
    int n;
    AdjList *array;
} Graph;

typedef struct Edge_score
{
    int v, w;
    float score;
} Edge_score;

void update(Edge_score *array, int v, int w, float score, int len, int k)
{
    if (len == 0)
    {
        array[0].v = v, array[0].w = w, array[0].score = score;
        len = 1;
        return;
    }
    if (len == k && score <= array[len - 1].score)
        return;
    int i = 0;
    for (i = 0; i < len && score <= array[i].score; i++)
        ;
    if (len < k)
    {
        for (int j = len; j > i; j--)
            array[j] = array[j - 1];
        len += 1;
    }
    else
    {
        for (int j = k - 1; j > i; j--)
            array[j] = array[j - 1];
    }
    array[i].v = v, array[i].w = w, array[i].score = score;
}

Graph *create_graph(int n)
{
    Graph *g = (Graph *)malloc(sizeof(Graph));
    g->array = (struct AdjList *)malloc(n * sizeof(struct AdjList));
    for (int i = 0; i < n; i++)
        g->array[i].head = NULL;
    g->n = n;
    return g;
}

void add_edge(Graph *g, int v, int w)
{
    Node *new_1 = (Node *)malloc(sizeof(Node));
    new_1->dest = w;
    new_1->next = g->array[v - 1].head;
    g->array[v - 1].head = new_1;

    Node *new_2 = (Node *)malloc(sizeof(Node));
    new_2->dest = v;
    new_2->next = g->array[w - 1].head;
    g->array[w - 1].head = new_2;
}

void graph_print(Graph *g)
{
    for (int v = 1; v < g->n + 1; v++)
    {
        printf("%d: ", v);
        Node *p = g->array[v - 1].head;
        while (p)
        {
            printf("%d", p->dest);
            if (p->next)
                printf(", ");
            p = p->next;
        }
        printf("\n");
    }
}

int is_edge(Graph *g, int v, int w)
{
    Node *p = g->array[v - 1].head;
    while (p)
    {
        if (p->dest == w)
            return 1;
        p = p->next;
    }
    return 0;
}

int degree(Graph *g, int v)
{
    int res = 0;
    Node *p = g->array[v - 1].head;
    while (p)
    {
        res++;
        p = p->next;
    }
    return res;
}

float jaccard(Graph *g, int v, int w)
{
    if (is_edge(g, v, w))
        return -1.0;

    int a = 0, b = 0, c = 0;
    int flag = 1;
    Node *p = g->array[v - 1].head;

    while (p)
    {
        Node *q = g->array[w - 1].head;
        while (q)
        {
            if (flag)
                b++;
            if (p->dest == q->dest)
                c++;
            q = q->next;
        }
        p = p->next;
        flag = 0;
        a++;
    }
    return 1.0 * c / (a + b - c);
}

int *walks_matrix(Graph *g)
{
    int n = g->n;
    int l = 6 * n * n;
    int *A = (int *)malloc(l * sizeof(int));
    for (int i = 0; i < l; i++)
        A[i] = 0;

    for (int v = 0; v < n; v++)
    {
        Node *p = g->array[v].head;
        while (p)
        {
            A[IX(n, 0, v, p->dest - 1)] = 1;
            p = p->next;
        }
    }

    for (int i = 1; i < 6; i++)
    {
        for (int v = 0; v < n; v++)
        {
            for (int w = 0; w < n; w++)
            {
                Node *p = g->array[v].head;
                while (p)
                {
                    A[i * n * n + v * n + w] += A[(i - 1) * n * n + (p->dest - 1) * n + w];
                    p = p->next;
                }
            }
        }
    }
    return A;
}

void matrix_print(float *matrix, int n)
{
    int l = n * n;
    for (int i = 0; i < l; i++)
    {
        printf("%f ", matrix[i]);
        if (i % n == n - 1)
            printf("\n");
    }
    printf("\n");
}

float *prob_ht_matrix(Graph *g, int **X)
{
    int n = g->n;
    int l = n * n;
    float *A = (float *)malloc(l * sizeof(float));
    float *B = (float *)malloc(l * sizeof(float));
    float *C = (float *)malloc(2 * l * sizeof(float));

    for (int i = 0; i < l; i++)
    {
        A[i] = 0;
        C[i] = 0;
        C[l + i] = 0;
    }

    for (int v = 0; v < n; v++)
    {
        Node *p = g->array[v].head;
        while (p)
        {
            A[n * v + p->dest - 1] = 1.0 / degree(g, v + 1);
            C[n * v + p->dest - 1] = A[n * v + p->dest - 1];
            p = p->next;
        }
    }

    int count = l;
    for (int i = 2; i < 7 || count < l; i++)
    {
        for (int v = 0; v < n; v++)
        {
            for (int w = 0; w < n; w++)
            {
                B[v * n + w] = 0;
                if (v == w)
                    continue;
                Node *p = g->array[v].head;
                while (p)
                {
                    if (p->dest != (w + 1))
                        B[v * n + w] += A[(p->dest - 1) * n + w] / degree(g, v + 1);
                    p = p->next;
                }
            }
        }

        for (int j = 0; j < l; j++)
        {
            A[j] = B[j];
        }

        
        for (int v = 0; v < n; v++)
        {
            for (int w = 0; w < n; w++)
            {
                int j = v*n + w;
                A[j] = B[j];
                C[j] += i * B[j];
                if (i == 2)
                    C[l + j] += i * B[j];
                else if ((*X)[j] == 0)
                {                    
                    if (i * (B[j] + B[w*n + v]) < 0.01)
                    {
                        (*X)[j] = i - 1;
                        count++;
                    }
                    else 
                    {
                        C[l + j] += i * B[j];
                    }
                }                
            }
        }
    }
    return C;
}

float katz_2(int *A, int n, int v, int w)
{
    float res = 0.0;
    for (int i = 5; i > 0; i--)
    {
        res = BETA * (res + A[i * n * n + (v - 1) * n + (w - 1)]);
    }
    return BETA * res;
}

float prob_ht(Graph *g, int v, int w, int len)
{
    if (v == w)
        return 0.0;
    else if (len == 1)
    {
        if (is_edge(g, v, w))
            return 1.0 / degree(g, v);
        return 0.0;
    }
    else
    {
        float res = 0.0;
        Node *p = g->array[v - 1].head;
        while (p)
        {
            res += prob_ht(g, p->dest, w, len - 1) / degree(g, v);
            p = p->next;
        }
        return res;
    }
}

float hitting_time(Graph *g, int v, int w, int n)
{
    float time = 0.0;
    for (int i = 2; i < n + 1; i++)
        time += i * prob_ht(g, v, w, i);
    return time;
}

int main()
{
    FILE *fptr;
    fptr = fopen("../contact-high-school-proj-graph.txt", "r");

    int i, j, k;
    int n = 0;
    while (EOF != fscanf(fptr, "%d %d %d", &i, &j, &k))
    {
        n = (i > n) ? i : n;
        n = (j > n) ? j : n;
    }

    Graph *g = create_graph(n);
    fseek(fptr, 0, SEEK_SET);

    while (EOF != fscanf(fptr, "%d %d %d", &i, &j, &k))
        add_edge(g, i, j);

    fclose(fptr);

    int l = n*n;
    int *X = (int *)malloc(n * n * sizeof(int));

    for (int i = 0; i < n * n; i++)
        X[i] = 0;

    float *HT = prob_ht_matrix(g, &X);
    int *Katz = walks_matrix(g);

    printf("Please enter k: ");
    scanf("%d", &k);

    Edge_score *res1 = (Edge_score *)malloc(k * sizeof(Edge_score));
    Edge_score *res2 = (Edge_score *)malloc(k * sizeof(Edge_score));
    Edge_score *res3 = (Edge_score *)malloc(k * sizeof(Edge_score));
    Edge_score *res4 = (Edge_score *)malloc(k * sizeof(Edge_score));

    int res_len = 0;
    float score;

    for (i = 1; i < n + 1; i++)
    {
        for (j = i + 1; j < n + 1; j++)
        {
            if (!is_edge(g, i, j))
            {
                update(res1, i, j, jaccard(g, i, j), res_len, k);
                update(res2, i, j, katz_2(Katz, n, i, j), res_len, k);
                update(res3, i, j, -1.0 * (HT[(i - 1) * n + j - 1] + HT[(j - 1) * n + i - 1]), res_len, k);
                update(res4, i, j, -1.0 * (HT[l + (i - 1) * n + j - 1] + HT[l + (j - 1) * n + i - 1]), res_len, k);
                if (res_len < k)
                    res_len++;
            }
        }
    }

    FILE *fp;
    fp = fopen(".\\Jaccard.txt", "w");
    for (int a = 0; a < res_len; a++)
        fprintf(fp, "%d %d %f\n", res1[a].v, res1[a].w, res1[a].score);
    fp = fopen(".\\Katz.txt", "w");
    for (int a = 0; a < res_len; a++)
        fprintf(fp, "%d %d %f\n", res2[a].v, res2[a].w, res2[a].score);
    fp = fopen(".\\HittingTime.txt", "w");
    for (int a = 0; a < res_len; a++)
        fprintf(fp, "%d %d %f\n", res3[a].v, res3[a].w, res3[a].score);
    fp = fopen(".\\HittingTimeAccurate.txt", "w");
    for (int a = 0; a < res_len; a++)
        fprintf(fp, "%d %d %f %d\n", res4[a].v, res4[a].w, res4[a].score, X[n * (res4[a].v - 1) + res4[a].w - 1]);
    fclose(fp);

    free(res1);
    free(res2);
    free(res3);
    free(Katz);
    free(HT);
    free(X);
    return 0;
}
