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
    // Calculating gap between read and write positions
    int gap = read->index - write->index;
    for (Chunk *p = write->chunk; p != read->chunk; p = p->next)
    {
        p->filled = CHUNK_SIZE;
        gap += CHUNK_SIZE;
    }

    if (len_t < gap)
        return;

    int chunks_to_add = (len_t - gap + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // Add the necessary chunks after write->chunk
    Chunk *cur = write->chunk, *temp = write->chunk->next;
    for (int i = 0; i < chunks_to_add; i++)
    {
        cur->next = (Chunk *)malloc(sizeof(Chunk));
        cur->next->filled = CHUNK_SIZE;
        cur = cur->next;
    }
    cur->next = temp;

    // If read->chunk is the same as write->chunk, read->chunk is updated to the last inserted chunk
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

// Compares pattern with target starting from position and returns 1 if it's a match, and 0 if not
// Returns 0 if pattern is empty
int compare(Chunk *pattern, Chunk *target, int position)
{
    if (pattern == NULL)
        return 0;

    Position p = {pattern, 0};
    Position q = {target, position};
    while (p.chunk != NULL)
    {
        if (q.chunk == NULL || p.chunk->data[p.index] != q.chunk->data[q.index])
            return 0;

        increment_pos(&p, 1);
        increment_pos(&q, 1);
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

    while (read.chunk != NULL)
    {
        // some comment
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
            if (len_p == 0 && read.index == write.index)
                break;
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
    int i;

    while (scanf("%d,", &i))
    {
        if (head == NULL || p->filled == CHUNK_SIZE)
        {
            if (head == NULL)
                head = p = (Chunk *)malloc(sizeof(Chunk));
            else
            {
                p->next = (Chunk *)malloc(sizeof(Chunk));
                p = p->next;
            }
            p->filled = 0;
            p->next = NULL;
        }
        p->data[p->filled++] = i;
    }
    while (getchar() != '$')
        ;
    while (getchar() != '\n')
        ;
    return head;
}

int main()
{
    char c = 'y';
    printf("Enter input sequence: ");
    Chunk *seq = scan_sequence();
    sequence_print(seq);
    while (c == 'y' || c == 'Y')
    {
        printf("Enter pattern: ");
        Chunk *pattern = scan_sequence();
        sequence_print(pattern);
        printf("Enter text: ");
        Chunk *text = scan_sequence();
        sequence_print(text);
        printf("Sequence after replacement: ");
        replace_2(&seq, pattern, text);
        sequence_print(seq);
        printf("Do you wish to continue (y/n): ");
        c = getchar();
        printf("%c\n\n", c);
    }

    return 0;
}