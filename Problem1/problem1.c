#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 8

typedef struct Chunk
{
    int data[CHUNK_SIZE];
    int filled; //indicates how many of the indices in "data" are filled
    struct Chunk *next;
} Chunk;

// Stores position in a sequence using the chunk address and index in the "data"
// array of that chunk
// An empty position (denoting end of sequence) is always stored as {NULL, 0}
typedef struct Position
{
    Chunk *chunk;
    int index;
} Position;

// The position at p is shifted "n" positions to the right
Position *increment_pos(Position *p, int n)
{
    // No change is made in case of a {NULL, 0} position
    if (p->chunk == NULL)
        return p;

    for (int i = 0; i < n; i++)
    {
        // When *p reaches the end of current chunk, we move to the next chunk
        if (++p->index == p->chunk->filled)
        {
            p->chunk = p->chunk->next;
            p->index = 0;
        }
    }
    return p;
}

// Returns the length of the sequence *seq
int seqlen(Chunk *seq)
{
    int res = 0;
    for (; seq; seq = seq->next)
        res += seq->filled;
    return res;
}

void sequence_print(Chunk *head)
{
    // Elements of the sequence are printed one after the other
    for (Position p = {head, 0}; p.chunk; increment_pos(&p, 1))
        printf("%d,", p.chunk->data[p.index]);
    printf("$\n");
}

Chunk *scan_sequence()
{
    Chunk *head = NULL;
    Chunk *p = head;
    int i;

    while (scanf("%d,", &i))
    {
        // Either the sequence is empty, or all chunks are filled
        if (head == NULL || p->filled == CHUNK_SIZE)
        {
            // When starting off with an empty sequence, head is set the
            // new node created
            if (head == NULL)
                head = p = (Chunk *)malloc(sizeof(Chunk));

            // A new chunk is created and links are made appropriately
            else
            {
                p->next = (Chunk *)malloc(sizeof(Chunk));
                p = p->next;
            }

            p->filled = 0;
            p->next = NULL;
        }

        // The scanned value is pushed into the sequence
        p->data[p->filled++] = i;
    }
    while (getchar() != '$')
        ;
    while (getchar() != '\n')
        ;
    return head;
}

// Frees the sequence from the heap
void free_sequence(Chunk *head)
{
    while (head != NULL)
    {
        Chunk *temp = head->next;
        free(head);
        head = temp;
    }
}

//Calculates gap from write to read. If len_t > gap, sufficient chunks are added,
// and the last added chunk is returned. If no chunk is added, write.chunk
// is returned.
void make_room(Position *read, Position *write, int len_t)
{
    // Calculating gap between read and write positions
    int gap = read->index - write->index;
    for (Chunk *p = write->chunk; p != read->chunk; p = p->next)
    {
        p->filled = CHUNK_SIZE;
        gap += CHUNK_SIZE;
    }

    // Sufficient space is present, nothing to be done
    if (len_t < gap)
        return;

    // Number of chunks to be added is ceiling of (len_t - gap) / CHUNK_SIZE
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

    // If read->chunk is the same as write->chunk, read->chunk is updated to
    // the last inserted chunk
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

// Removes a multiple of CHUNK_SIZE number of entries to bring "read" and
// "write" within CHUNK_SIZE of each other
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

    // Check if write and read can fit in a single chunk
    if (read->chunk && write->index <= read->index)
    {
        // Copy data from read chunk to write chunk
        for (int i = read->index; i < read->chunk->filled; i++)
            write->chunk->data[i] = read->chunk->data[i];
        write->chunk->filled = read->chunk->filled;
        write->chunk->next = read->chunk->next;

        // Remove the read chunk from the sequence
        free(read->chunk);
        read->chunk = write->chunk;
    }
    return;
}

// Compares pattern with target starting from position and returns 1
// if it's a match, and 0 if not
// Returns 0 if pattern is empty
int compare(Chunk *pattern, Chunk *target, int position)
{
    if (pattern == NULL)
        return 0;

    Position p = {pattern, 0};
    Position q = {target, position};
    while (p.chunk != NULL)
    {
        // If we reach the end of the target sequence before pattern finishes,
        // or if there is a mismatch 0 is immediately returned
        if (q.chunk == NULL || p.chunk->data[p.index] != q.chunk->data[q.index])
            return 0;
        increment_pos(&p, 1);
        increment_pos(&q, 1);
    }

    // Since there was no mismatch, 1 is returned
    return 1;
}

// Two positions, name "read" and "write" are utilized. Comparison with pattern
// is always done at "read". If there is no match, the entry at "read" is written
// into "write" (provided both positions are not the same) and both positions are
// incremented. If there is a match, "read" is made to go to the end of the pattern,
// and "text" is copied in at "write"
void replace(Chunk **seqp, Chunk *pattern, Chunk *text)
{
    int len_p = seqlen(pattern);
    int len_t = seqlen(text);
    int deleted_just_now = 0;
    Position read = {*seqp, 0}, write = {*seqp, 0};

    // Do nothing if pattern and text are the same
    if (len_p == len_t && (compare(pattern, text, 0) == 1 || len_p == 0))
        return;

    if (*seqp == NULL)
    {
        if (len_p != 0)
            return;

        *seqp = (Chunk *)malloc(sizeof(Chunk));
        (*seqp)->filled = CHUNK_SIZE;
        (*seqp)->next = read.chunk;
        write.chunk = *seqp;
    }

    // If pattern is empty, making room and copying "text" is done once
    if (len_p == 0)
    {

        make_room(&read, &write, len_t);

        // Copy in "text" at position "write" (which is the beginning of the sequence)
        Position temp = {text, 0};
        while (temp.chunk != NULL)
        {
            write.chunk->data[write.index] = temp.chunk->data[temp.index];
            increment_pos(&temp, 1);
            if (read.chunk || temp.chunk)
                increment_pos(&write, 1);
        }

        // Remove any unnecessary chunks
        remove_excess(&read, &write);
    }

    while (read.chunk != NULL)
    {
        // Check if pattern appears starting from position "read"
        if (compare(pattern, read.chunk, read.index) == 1)
        {
            increment_pos(&read, len_p);
            make_room(&read, &write, len_t);

            // Copy in "text" at position "write"
            Position temp = {text, 0};
            while (temp.chunk != NULL)
            {
                write.chunk->data[write.index] = temp.chunk->data[temp.index];
                increment_pos(&temp, 1);
                if (read.chunk || temp.chunk)
                    increment_pos(&write, 1);
            }

            if (len_t == 0)
                deleted_just_now = 1;

            // Remove any unnecessary chunks
            remove_excess(&read, &write);
        }
        else
        {
            deleted_just_now = 0;

            // In case of empty pattern, "read" and "write" indices are same
            // iff read is the same position as write
            // In this case, length of text is a multiple of CHUNK_SIZE,
            // and thus no shifting is required
            if (len_p == 0 && read.index == write.index)
                return;

            // If there is no match with pattern at "read", the entry at "read"
            // is written into "write"
            write.chunk->data[write.index] = read.chunk->data[read.index];
            increment_pos(&read, 1);

            // "write" is incremented provided this isn't the last entry that
            // is going to be written
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

    // Filled field of the last chunk is set appropriately
    write.chunk->filled = write.index + 1 - deleted_just_now;

    // If the whole sequence is deleted, *seqp is made NULL (empty sequence)
    if (write.chunk == *seqp && write.chunk->filled == 0)
    {
        free(write.chunk);
        *seqp = NULL;
    }
}

int main()
{
    char c = 'y';
    printf("Please enter the sequence S: ");
    Chunk *seq = scan_sequence();
    sequence_print(seq);
    while (c == 'y' || c == 'Y')
    {
        printf("Please enter the pattern to be replaced: ");
        Chunk *pattern = scan_sequence();
        sequence_print(pattern);
        printf("Please enter the replacement text: ");
        Chunk *text = scan_sequence();
        sequence_print(text);
        printf("After replacement, S: ");
        replace(&seq, pattern, text);
        sequence_print(seq);
        printf("Do you want to continue? (y/n): ");
        c = getchar();
        printf("%c\n\n", c);
        free_sequence(pattern);
        free_sequence(text);
    }

    return 0;
}
