#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 8

typedef struct Chunk
{
    int data[CHUNK_SIZE];
    int filled; //indicates how many of the indices in "data" are filled
    struct Chunk *next;
} Chunk;

typedef struct Position
{
    Chunk *chunk;
    int index;
} Position;

Position *increment_pos(Position *p, int n)
{
    if (p->chunk == NULL)
        return p;
    for (int i = 0; i < n; i++)
    {
        if (++p->index == p->chunk->filled)
        {
            p->chunk = p->chunk->next;
            p->index = 0;
        }
    }
    return p;
}

// inserts a chunk AFTER the chunk at position, and inserts the chunk at the beginning if positiion is NULL
// a pointer to the new chunk is returned
Chunk *insert(Chunk **headp, Chunk *position, int filled)
{
    Chunk *new = (Chunk *)malloc(sizeof(Chunk));
    new->filled = filled;
    if (*headp == NULL)
    {
        *headp = new;
        new->next = NULL;
        return new;
    }
    if (position == NULL)
    {
        new->next = *headp;
        *headp = new;
        return new;
    }
    new->next = position->next;
    position->next = new;
    return new;
}

//Deletes the chunk at position and returns the next address of next chunk
Chunk *delete (Chunk **headp, Chunk *position)
{
    if (*headp == NULL)
        return NULL;
    if (*headp == position)
    {
        Chunk *temp = (*headp)->next;
        free(*headp);
        *headp = temp;
        return temp;
    }
    /*
    if ((*headp)->next == position)
    {
        (*headp)->next = position->next;
        chunk *temp = position->next;
        free(position);
        return temp;
    }
    */
    Chunk *p = *headp;
    for (; p != NULL && p->next != position; p = p->next)
        ;
    if (p == NULL)
        return position;
    p->next = position->next;
    Chunk *temp = position->next;
    free(position);
    return temp;
}

//Calculates gap from write to read. If len_t > gap, sufficient chunks are added,
//and the last added chunk is returned. If no chunk is added, write.chunk is returned.
void make_room(Position *read, Position *write, int len_t)
{
    int gap = read->index - write->index;
    for (Chunk *p = write->chunk; p != read->chunk; p = p->next)
        gap += CHUNK_SIZE;
    if (len_t < gap)
        return;

    int chunks_to_add = (len_t - gap + CHUNK_SIZE - 1) / CHUNK_SIZE;

    Chunk *cur = write->chunk, *temp = write->chunk->next;
    for (int i = 0; i < chunks_to_add; i++)
    {
        cur->next = (Chunk *)malloc(sizeof(Chunk));
        cur->next->filled = CHUNK_SIZE;
        cur = cur->next;
    }
    cur->next = temp;

    if (read->chunk == write->chunk)
    {
        cur->filled = read->chunk->filled;
        for (int j = cur->filled - 1; j >= read->index; j--)
            cur->data[j] = read->chunk->data[j];
        read->chunk = cur;
    }

    if (chunks_to_add > 0)
        write->chunk->filled = CHUNK_SIZE;

    return;
}

//Deletes n * CHUNK_SIZE entries starting from index i (included) in the chunk at position.
//Returns the address of the input chunk
void remove_excess(Position *read, Position *write)
{
    if (write->chunk == read->chunk)
        return;
    while (write->chunk->next != read->chunk)
    {
        Chunk *temp = write->chunk->next;
        write->chunk->next = temp->next;
        free(temp);
    }

    // check if write and read can fit in a single chunk
    if (read->chunk && write->index <= read->index)
    {
        // copy data from read chunk to write chunk
        for (int i = read->index; i < read->chunk->filled; i++)
            write->chunk->data[i] = read->chunk->data[i];
        write->chunk->filled = read->chunk->filled;
        write->chunk->next = read->chunk->next;

        // remove the read chunk from the sequence
        free(read->chunk);
        read->chunk = write->chunk;
    }
    return;
}

int compare(Chunk *pattern, Chunk *target, int position)
{
    if (pattern == NULL)
        return 0;
    int i = 0;
    while (pattern->next != NULL || i < pattern->filled)
    {
        if (target == NULL ||
            (target->next == NULL && position >= target->filled) ||
            (pattern->data[i % CHUNK_SIZE] != target->data[position % CHUNK_SIZE]))
            return 0;

        i = (i + 1) % CHUNK_SIZE;
        if (i == 0)
            pattern = pattern->next;

        position = (position + 1) % CHUNK_SIZE;
        if (position == 0)
            target = target->next;
    }
    return 1;
}

int seqlen(Chunk *seq)
{
    int res = 0;
    for (; seq; seq = seq->next)
        res += seq->filled;
    return res;
}

void replace_2(Chunk **seqp, Chunk *pattern, Chunk *text)
{
    int len_p = seqlen(pattern);
    int len_t = seqlen(text);
    Position read = {*seqp, 0}, write = {*seqp, 0};
    if (len_p == 0)
    {
        return;
    }
    else
    {
        while (read.chunk != NULL)
        {
            if (compare(pattern, read.chunk, read.index) == 1)
            {
                increment_pos(&read, len_p);
                make_room(&read, &write, len_t);
                Position temp = {text, 0};
                while (temp.chunk != NULL)
                {
                    write.chunk->data[write.index] = temp.chunk->data[temp.index];
                    increment_pos(&temp, 1);
                    if (read.chunk || temp.chunk)
                        increment_pos(&write, 1);
                }
                remove_excess(&read, &write);
            }
            else
            {
                write.chunk->data[write.index] = read.chunk->data[read.index];
                increment_pos(&read, 1);
                if (read.chunk)
                    increment_pos(&write, 1);
            }
        }

        //CLEANUP! - Remove all chunks after write->chunk

        while (write.chunk->next != NULL)
        {
            Chunk *temp = write.chunk->next;
            write.chunk->next = temp->next;
            free(temp);
        }
        write.chunk->filled = write.index + 1;
    }
}

void sequence_print(Chunk *head)
{

    for (Position p = {head, 0}; p.chunk; increment_pos(&p, 1))
    {
        printf("%d", p.chunk->data[p.index]);
        if (p.chunk->next || p.index < p.chunk->filled - 1)
            printf(",");
    }
    printf("\n");
}

Chunk *scan_sequence()
{
    Chunk *head = NULL;
    Chunk *p = head;
    while (1)
    {
        int i;
        int rc = scanf("%d", &i);
        if (rc == EOF || rc == 0)
            break;
        if (p == NULL || p->filled == CHUNK_SIZE)
            p = insert(&head, p, 0);
        p->data[p->filled++] = i;

        char c = getchar();
        if (c == '\n')
            break;

        if (c != ',')
        {
            printf("Bad Input! Invalid character '%c' encountered instead of comma!\n", c);
            exit(1);
        }
    }
    return head;
}

int main()
{
    Chunk *head = scan_sequence();
    //    Position write = {head, 1};
    //    Position read = {head, 3};

    Chunk *pattern = scan_sequence();
    Chunk *text = scan_sequence();
    replace_2(&head, pattern, text);
    sequence_print(head);
    //    replace_2(&head, pattern, text);

    /*
    printf("Sequence: ");
    sequence_print(head);
    printf("Pattern: ");
    sequence_print(pattern);
    printf("Text: ");
    sequence_print(text);
    replace(&head, pattern, text);
    printf("New Sequence: ");
    sequence_print(head);
*/
    return 0;
}