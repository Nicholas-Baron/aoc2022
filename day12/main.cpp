#include <climits>    // INT_MAX
#include <fstream>    // ifstream
#include <functional> // hash
#include <iostream>   // cout
#include <optional>
#include <queue> // priority_queue
#include <string>
#include <unordered_set>
#include <vector>

using heightmap_t = std::vector<std::string>;

struct location {
    size_t x, y;

  private:
    [[nodiscard]] friend bool operator==(const location & lhs, const location & rhs) {
        return lhs.x == rhs.x and lhs.y == rhs.y;
    }
};

template<>
struct std::hash<location> {
    std::size_t operator()(const location & loc) const noexcept {
        return (std::hash<size_t>{}(loc.x) << 1) xor std::hash<size_t>{}(loc.y);
    }
};

static std::optional<location> find_character(const heightmap_t & heightmap, char c) {

    for (size_t i = 0; i < heightmap.size(); ++i) {
        if (auto pos = heightmap[i].find(c); pos != std::string::npos) { return location{i, pos}; }
    }
    return std::nullopt;
}

static std::optional<int> find_path(const heightmap_t & heightmap, location start, location end) {
    struct queue_entry {
        location loc;
        size_t distance;

        [[nodiscard]] bool operator<(const queue_entry & rhs) const noexcept {
            return distance > rhs.distance;
        }
    };

    std::unordered_set<location> visited;
    std::priority_queue<queue_entry> pq;
    pq.push({start, 0});

    auto add_neighbor
        = [&pq, &heightmap](char src_height, size_t dest_x, size_t dest_y, size_t src_distance) {
              if (dest_x >= heightmap.size()) return;
              if (dest_y >= heightmap[dest_x].size()) return;

              auto dest_height = heightmap[dest_x][dest_y];

              if (src_height == 'S') { src_height = 'a'; }
              if (dest_height == 'E') { dest_height = 'z'; }

              if (src_height + 1 < dest_height) return;

              pq.push({location{dest_x, dest_y}, src_distance + 1});
          };

    while (not pq.empty()) {
        auto visiting = pq.top();
        pq.pop();
        if (visiting.loc == end) return visiting.distance;
        if (visited.find(visiting.loc) != visited.end()) continue;
        else
            visited.emplace(visiting.loc);

        auto height = heightmap[visiting.loc.x][visiting.loc.y];
        add_neighbor(height, visiting.loc.x + 1, visiting.loc.y, visiting.distance);
        add_neighbor(height, visiting.loc.x - 1, visiting.loc.y, visiting.distance);
        add_neighbor(height, visiting.loc.x, visiting.loc.y + 1, visiting.distance);
        add_neighbor(height, visiting.loc.x, visiting.loc.y - 1, visiting.distance);
    }

    return std::nullopt;
}

int main(const int arg_count, const char * const * const args) {
    if (arg_count != 2) {
        std::cout << "Usage: " << args[0] << " <input file>" << std::endl;
        return 1;
    }
    std::ifstream file{args[1]};

    heightmap_t heightmap{""};
    while (std::getline(file, heightmap.back())) { heightmap.push_back(""); }
    heightmap.pop_back();

    file.close();

    auto starting_position = find_character(heightmap, 'S');
    if (not starting_position.has_value()) {
        std::cout << "Could not find 'S' in the input" << std::endl;
        return 2;
    }

    auto ending_position = find_character(heightmap, 'E');
    if (not ending_position.has_value()) {
        std::cout << "Could not find 'E' in the input" << std::endl;
        return 3;
    }

    auto shortest_path
        = find_path(heightmap, starting_position.value(), ending_position.value()).value_or(-1);
    std::cout << shortest_path << std::endl;

    for (size_t i = 0; i < heightmap.size(); ++i) {
        for (size_t j = 0; j < heightmap[i].size(); ++j) {
            if (heightmap[i][j] != 'a') continue;

            shortest_path
                = std::min(shortest_path,
                           find_path(heightmap, {i, j}, ending_position.value()).value_or(INT_MAX));
        }
    }

    std::cout << shortest_path << std::endl;
}
