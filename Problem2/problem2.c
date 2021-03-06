#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node
{
    int i, j;
    float a;
    struct Node *next, *prev;
} Node;

// The first term is considered to be x^i1*y^j1 and the second term is x^i2*y^j2
// Returns 0 if two terms have same degree, 1 if term1 is greater and -1 if
// term2 is greater
int compare(int i1, int j1, int i2, int j2)
{
    if (i1 + j1 == i2 + j2)
    {
        if (i1 == i2)
            return 0;
        return (i1 < i2) ? -1 : 1;
    }
    return (i1 + j1 < i2 + j2) ? -1 : 1;
}

// Deletes term (frees it) and returns the address of the previous term (NULL if
// first term was deleted)
Node *delete (Node **headp, Node **tailp, Node *term)
{
    if (*headp == NULL || term == NULL)
        return NULL;

    Node *result = NULL;
    if (*headp == term)
    {
        if (term->next == NULL)
        {
            *headp = NULL;
            *tailp = NULL;
            return result;
        }
        *headp = term->next;
        (term->next)->prev = NULL;
    }
    else if (*tailp == term)
    {
        result = *tailp = term->prev;
        (term->prev)->next = NULL;
    }
    else
    {
        result = term->prev;
        (term->next)->prev = term->prev;
        (term->prev)->next = term->next;
    }
    free(term);
    return result;
}

// Inserts (i,j,a) after "place"
// If "place" is NULL, (i,j,a) is inserted at the beginning
void insert(Node **headp, Node **tailp, Node *place, int i, int j, float a)
{
    Node *term = (Node *)malloc(sizeof(Node));
    term->i = i;
    term->j = j;
    term->a = a;
    term->prev = place;

    if (place == NULL)
    {
        term->next = *headp;
        if (*headp != NULL)
            (*headp)->prev = term;
        *headp = term;
        if (*tailp == NULL)
            *tailp = term;
        return;
    }
    term->next = place->next;
    if (place == *tailp)
        *tailp = term;
    else
        (place->next)->prev = term;
    place->next = term;
}

void list_print(Node *head)
{
    Node *tail = NULL;
    if (head == NULL)
        printf("(0,0,0)");
    for (Node *p = head; p != NULL; p = p->next)
    {
        printf("(%d,%d,%g)", p->i, p->j, p->a);
        if (p->next != NULL)
            printf(",");
        else
            tail = p;
    }
    printf("\n");
    for (Node *p = tail; p != NULL; p = p->prev)
    {
        if (p->prev == NULL)
            assert(p == head);
    }
}

Node *scan_p()
{
    Node *head = NULL, *tail = NULL;
    while (1)
    {
        int i, j;
        float a;
        int rc = scanf("(%d,%d,%f)", &i, &j, &a);
        if (rc == EOF || rc == 0)
            break;
        if (a != 0)
        {
            if (head == NULL || compare(i, j, head->i, head->j) > 0)
                insert(&head, &tail, NULL, i, j, a);
            else
            {
                Node *p = head;
                while (p->next && compare(i, j, p->next->i, p->next->j) < 0)
                    p = p->next;
                insert(&head, &tail, p, i, j, a);
            }
        }
        scanf(",");
    }
    while (1)
    {
        char c;
        scanf("%c", &c);
        if (c == '\n')
            break;
    }
    return head;
}

Node *add(Node *p1, Node *p2)
{
    Node *head = NULL, *tail = NULL;
    while (p1 && p2)
    {
        int c = compare(p1->i, p1->j, p2->i, p2->j);
        if (c == 0)
        {
            if (p1->a + p2->a != 0)
                insert(&head, &tail, tail, p1->i, p1->j, p1->a + p2->a);
            p1 = p1->next;
            p2 = p2->next;
        }
        else if (c > 0)
        {
            insert(&head, &tail, tail, p1->i, p1->j, p1->a);
            p1 = p1->next;
        }
        else
        {
            insert(&head, &tail, tail, p2->i, p2->j, p2->a);
            p2 = p2->next;
        }
    }
    if (p1 == NULL)
    {
        while (p2)
        {
            insert(&head, &tail, tail, p2->i, p2->j, p2->a);
            p2 = p2->next;
        }
    }
    while (p1)
    {
        insert(&head, &tail, tail, p1->i, p1->j, p1->a);
        p1 = p1->next;
    }
    return head;
}

Node *mul(Node *p1, Node *p2)
{
    Node *head = NULL, *tail = NULL;
    for (; p1 != NULL; p1 = p1->next)
    {
        Node *t = head;
        for (Node *q = p2; q != NULL; q = q->next)
        {
            while (t && t->next && compare((t->next)->i, (t->next)->j, p1->i + q->i, p1->j + q->j) >= 0)
            {
                t = t->next;
            }
            if (t == NULL)
            {
                insert(&head, &tail, head, p1->i + q->i, p1->j + q->j, p1->a * q->a);
                t = head;
            }
            else if (compare(t->i, t->j, p1->i + q->i, p1->j + q->j) == 0)
            {
                if (p1->a * q->a + t->a == 0)
                    t = delete (&head, &tail, t);
                else
                    t->a += p1->a * q->a;
            }
            else
            {
                insert(&head, &tail, t, p1->i + q->i, p1->j + q->j, p1->a * q->a);
            }
        }
    }
    return head;
}

void list_free(Node *cur)
{
    Node *temp;
    while (cur != NULL)
    {
        temp = cur->next;
        free(cur);
        cur = temp;
    }
}

int main()
{
    char cont = 'y';
    char op;
    Node *result;

    while (cont == 'y')
    {
        printf("Please enter the operator(+/*): ");
        scanf("%c", &op);
        printf("%c\n", op);
        while (1)
        {
            char c;
            scanf("%c", &c);
            if (c == '\n')
                break;
        }
        printf("Please enter polynomial A: ");
        Node *p1 = scan_p();
        list_print(p1);
        printf("Please enter polynomial B: ");
        Node *p2 = scan_p();
        list_print(p2);
        if (op == '+')
            result = add(p1, p2);
        else if (op == '*')
            result = mul(p1, p2);
        else
        {
            printf("'%c' is an invalid operation!", op);
            exit(1);
        }
        printf("A%cB: ", op);
        list_print(result);
        list_free(p1);
        list_free(p2);
        list_free(result);
        printf("Do you want to continue? (y/n): ");
        cont = getchar();
        printf("%c\n\n", cont);
        while (1)
        {
            char c;
            scanf("%c", &c);
            if (c == '\n')
                break;
        }
    }
    
    return 0;
}