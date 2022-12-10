#include <assert.h>
#include <fcntl.h> // open
#include <iso646.h>
#include <stdbool.h>  // bool
#include <stdio.h>    // perror
#include <stdlib.h>   // malloc
#include <string.h>   // memset
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
static struct location rope[10];

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
    if (tail_list == NULL) { add_to_tail(&tail_list, rope[0]); }

    // printf("rope[0] : (%2d, %2d)\nloc: (%2d, %2d)\n", rope[0].x, rope[0].y, loc.x, loc.y);

    rope[0] = loc;

    for (size_t index = 1; index < sizeof(rope) / sizeof(*rope); ++index) {
        struct location prev_knot = rope[index - 1];
        printf("prev_knot : (%2d, %2d); rope[%2zu]: (%2d, %2d)\n", prev_knot.x, prev_knot.y, index,
               rope[index].x, rope[index].y);
        if (abs(rope[index].x - prev_knot.x) <= 1 and abs(rope[index].y - prev_knot.y) <= 1) {
            break;
        }

        int deltaX = abs(rope[index].x - prev_knot.x);
        int deltaY = abs(rope[index].y - prev_knot.y);

        assert(deltaX <= 2);
        assert(deltaY <= 2);

        if (deltaX == 2 and deltaY == 0) {
            rope[index].x += (rope[index].x > prev_knot.x) ? -1 : 1;
        } else if (deltaY == 2 and deltaX == 0) {
            rope[index].y += (rope[index].y > prev_knot.y) ? -1 : 1;
        } else if (deltaX == 2 and deltaY >= 1) {
            rope[index].x += (rope[index].x > prev_knot.x) ? -1 : 1;
            rope[index].y += (rope[index].y > prev_knot.y) ? -1 : 1;
        } else if (deltaY == 2 and deltaX >= 1) {
            rope[index].x += (rope[index].x > prev_knot.x) ? -1 : 1;
            rope[index].y += (rope[index].y > prev_knot.y) ? -1 : 1;
        } else {
            break;
        }
    }

    struct location current_tail = rope[sizeof(rope) / sizeof(*rope) - 1];
    assert(abs(current_tail.x - tail_list->loc.x) <= 1
           and abs(current_tail.y - tail_list->loc.y) <= 1);

    if (current_tail.x != tail_list->loc.x or current_tail.y != tail_list->loc.y) {
        printf("Moving tail to (%2d,%2d)\n", current_tail.x, current_tail.y);
        add_to_tail(&tail_list, current_tail);
    }
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

    printf("Moving %u units\n", distance);

    struct location * head_location = rope;

    switch (*start_of_line) {
    case 'R':
        // go right
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location->x + 1, .y = head_location->y}));
        }
        break;
    case 'U':
        // go up
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location->x, .y = head_location->y + 1}));
        }
        break;
    case 'L':
        // go left
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location->x - 1, .y = head_location->y}));
        }
        break;
    case 'D':
        // go down
        for (unsigned i = 0; i < distance; ++i) {
            move_head_to(((struct location){.x = head_location->x, .y = head_location->y - 1}));
        }
        break;
    }

    printf("[");
    for (size_t index = 0; index < sizeof(rope) / sizeof(*rope); ++index) {
        printf("(%2d, %2d) ", rope[index].x, rope[index].y);
    }
    puts("]");
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

    memset(rope, 0, sizeof(rope));

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
