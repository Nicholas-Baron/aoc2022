#include <fcntl.h> // open
#include <iso646.h>
#include <stdio.h>    // printf
#include <sys/mman.h> // mmap
#include <sys/stat.h> // fstat
#include <unistd.h>   // close

static int signal_strength = 0;
static int x_reg = 1;
static int cycle_count = 0;

static void tick() {
    ++cycle_count;
    if (cycle_count % 40 == 20) { signal_strength += x_reg * cycle_count; }
}

static void parse_line(const char * start_of_line) {
    if (start_of_line[0] == 'n') {
        // noop
        tick();
    } else {
        // addx
        int value;
        if (sscanf(start_of_line, "addx %i", &value) != 1) {
            char temp[16] = {0};
            for (size_t i = 0; i < sizeof(temp) and start_of_line[i] != '\n'; ++i) {
                temp[i] = start_of_line[i];
            }
            printf("Could not parse %s\n", temp);
            return;
        }

        tick();
        tick();
        x_reg += value;
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

    printf("%d\n", signal_strength);

    munmap(data, size);
}
