#include <fcntl.h> // open
#include <iso646.h>
#include <stdbool.h>  // bool
#include <stdio.h>    // perror
#include <stdlib.h>   // malloc
#include <sys/mman.h> // mmap
#include <sys/stat.h> // fstat
#include <unistd.h>   // close

struct location {
    int x, y;
};

struct tail_visited {
    struct location loc;
    struct tail_visited * next;
};

static void add_to_tail(struct tail_visited ** list, struct location loc) {
    if (*list == NULL) {
        *list = malloc(sizeof(struct tail_visited));
        (*list)->next = NULL;
    } else {
        struct tail_visited * new_node = malloc(sizeof(struct tail_visited));
        new_node->next = *list;
        *list = new_node;
    }
    (*list)->loc = loc;
}

static struct tail_visited * tail_list = NULL;
static struct location head_location = {.x = 0, .y = 0};

static int count_visited_locations() {
    struct tail_visited * visited_list = NULL;

    for (struct tail_visited * iter = tail_list; iter != NULL; iter = iter->next) {
        bool should_add = true;
        for (struct tail_visited * find_iter = visited_list; find_iter != NULL and should_add;
             find_iter = find_iter->next) {
            if (iter->loc.x == find_iter->loc.x and iter->loc.y == find_iter->loc.y) {
                should_add = false;
            }
        }

        if (should_add) {
            printf("x: %2d, y: %2d\n", iter->loc.x, iter->loc.y);
            add_to_tail(&visited_list, iter->loc);
        }
    }

    int count = 0;
    for (struct tail_visited * iter = visited_list; iter != NULL;) {
        ++count;
        struct tail_visited * node = iter;
        iter = iter->next;
        free(node);
    }
    return count;
}

static void move_head_to(struct location loc) {
    if (tail_list == NULL) { add_to_tail(&tail_list, head_location); }

    if (abs(tail_list->loc.x - loc.x) > 1 or abs(tail_list->loc.y - loc.y) > 1) {
        bool is_diagonal
            = head_location.x != tail_list->loc.x and head_location.y != tail_list->loc.y;
        bool reversing = (loc.x == tail_list->loc.x and loc.y == tail_list->loc.y)
                      or (loc.x == head_location.x and loc.y == head_location.y);

        if (is_diagonal or not reversing) { add_to_tail(&tail_list, head_location); }
    }

    head_location = loc;
}

static void parse_line(const char * start_of_line) {

    unsigned distance;
    if (sscanf(start_of_line + 2, "%u", &distance) != 1) {
        char temp[16] = {0};
        for (size_t i = 0; i < sizeof(temp) and start_of_line[i] != '\n'; ++i) {
            temp[i] = start_of_line[i];
        }
        printf("Failed to parse line %s\n", temp);
        return;
    }

    switch (*start_of_line) {
    case 'R':
        // go right
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location.x + 1, .y = head_location.y}));
        }
        break;
    case 'U':
        // go up
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location.x, .y = head_location.y + 1}));
        }
        break;
    case 'L':
        // go left
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location.x - 1, .y = head_location.y}));
        }
        break;
    case 'D':
        // go down
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location.x, .y = head_location.y - 1}));
        }
        break;
    }
}

int main(const int arg_count, const char * const * const args) {

    if (arg_count != 2) {
        printf("Usage: %s <input filename>\n", *args);
        return 0;
    }

    // Read in the file

    int fd = open(args[1], 0);
    if (fd == -1) {
        perror("open(input.txt)");
        return 1;
    }

    struct stat stat_res;
    if (fstat(fd, &stat_res) != 0) {
        perror("fstat(input.txt)");
        return 2;
    }

    off_t size = stat_res.st_size;

    char * data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap(input.txt)");
        return 3;
    }

    if (close(fd) == -1) {
        perror("close(input.txt)");
        return 4;
    }

    // Parse input
    for (const char * iter = data; iter < data + size;) {
        parse_line(iter);
        while (*iter != '\n') ++iter;
        ++iter;
    }

    // Print result
    printf("%d\n", count_visited_locations());

    munmap(data, size);
}
