#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <string.h>
#include <stdint.h>
#include <algorithm>

#define WRONG_PARAM_NUM 1
#define WRONG_FILE 2
#define WRONG_PARAMS 3

// edge structure -> (to, weight)
struct Edge {
    std::uint32_t to;
    std::uint8_t weight;

    Edge(const std::uint32_t& to, const std::uint8_t& weight) : to(to), weight(weight) {};

    friend bool operator<(const Edge& lhs, const Edge& rhs) {
        return (lhs.to < rhs.to) || (lhs.to == rhs.to && lhs.weight < rhs.weight);
    }
};

// Serialize graph from tsv file into binary file
void Serialize(char* input_path, char* output_path) {
    std::ifstream input(input_path);
    if (!input.is_open()) {
        std::cout << "Wrong input file!" << std::endl;
        exit(WRONG_FILE);
    }

    std::ofstream output(output_path, std::ios_base::binary);
    if (!output.is_open()) {
        std::cout << "Wrong output file!" << std::endl;
        exit(WRONG_FILE);
    }


    std::map<std::uint32_t, std::set<Edge>> edges; // vertex -> set of adjacent edges
    std::set<std::uint32_t> vertices; // set of vertices
    std::set<std::pair<std::uint32_t, std::uint32_t>> vertex; // (degree, vertex)

    std::uint32_t from, to, weight;
    while (input >> from >> to >> weight) {     // fill in graph info
        edges[from].insert({to, (std::uint8_t)weight});
        edges[to].insert({from, (std::uint8_t)weight});
        vertices.insert(from);
        vertices.insert(to);
    }
    input.close();

    for (int v : vertices) {
        vertex.insert({edges[v].size(), v});
    }

    std::map<std::uint32_t, std::vector<std::uint32_t>> degree_cluster; // degree -> set of vertices with such degree
    while (!vertex.empty()) {
        auto v = *vertex.rbegin();
        std::uint32_t deg = v.first;
        std::uint32_t from = v.second;
        vertex.erase(std::prev(vertex.end()));
        degree_cluster[deg].push_back(from);
        for (auto [to, weight] : edges[from]) {
            if (from != to) {
                vertex.erase({edges[to].size(), to});
                edges[to].erase({from, (std::uint8_t)weight});
                if (edges[to].size() != 0) {
                    vertex.insert({edges[to].size(), to});
                }
            }
        }
    }

    // fill in info
    // degree cluster_size vi_from_cluster vj wij ...
    for (auto it = degree_cluster.begin(); it != degree_cluster.end(); ++it) {
        std::uint32_t degree = it->first;
        output.write(reinterpret_cast<char*>(&degree), sizeof(degree));
        std::uint32_t sz = it->second.size();
        output.write(reinterpret_cast<char*>(&sz), sizeof(sz));
        for (auto from : it->second) {
            output.write(reinterpret_cast<char*>(&from), sizeof(from));
            for (auto [to, weight] : edges[from]) {
                output.write(reinterpret_cast<char*>(&to), sizeof(to));
                output.write(reinterpret_cast<char*>(&weight), sizeof(weight));
            }
        }
    }

    output.close();
}

// Deserialize graph from binary file into tsv file
void Deserialize(char* input_path, char* output_path) {
    std::ifstream input(input_path, std::ios_base::binary);
    if (!input.is_open()) {
        std::cout << "Wrong input file!" << std::endl;
        exit(WRONG_FILE);
    }
    std::ofstream output(output_path);
    if (!output.is_open()) {
        std::cout << "Wrong output file!" << std::endl;
        exit(WRONG_FILE);
    }

    std::uint32_t degree, from, to, sz;
    std::uint8_t weight;
    while (input.read(reinterpret_cast<char*>(&degree), sizeof(degree))) {      // read from binary file
        input.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        while (sz--) {
            input.read(reinterpret_cast<char*>(&from), sizeof(from));
            for (std::uint32_t i = 0; i < degree; ++i) {
                input.read(reinterpret_cast<char*>(&to), sizeof(to));
                input.read(reinterpret_cast<char*>(&weight), sizeof(weight));
                output << from << "\t" << to << "\t" << (std::uint32_t)weight << "\n"; // output into tsv file
            }
        }
    }
    input.close();
    output.close();
}

int main(int argc, char **argv) {
    if (argc != 6) {
        std::cout << "Wrong number of parameters!" << std::endl;
        exit(WRONG_PARAM_NUM);
    }

    if (!strcmp(argv[1], "-s") && !strcmp(argv[2], "-i") && !strcmp(argv[4], "-o")) {
        Serialize(argv[3], argv[5]);
    } else if (!strcmp(argv[1], "-d") && !strcmp(argv[2], "-i") && !strcmp(argv[4], "-o")) {
        Deserialize(argv[3], argv[5]);
    } else {
        std::cout << "Wrong parameters!" << std::endl;
        exit(WRONG_PARAMS);
    }
}