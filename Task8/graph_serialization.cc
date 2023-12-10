#include <iostream>
#include <fstream>
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

    std::map<std::uint32_t, std::set<Edge>> edges; // vertex -> set of adjacent edges
    std::set<std::uint32_t> vertices; // set of vertices

    std::uint32_t from, to, weight;
    while (input >> from >> to >> weight) {     // fill in graph info
        edges[from].insert({to, (std::uint8_t)weight});
        edges[to].insert({from, (std::uint8_t)weight});
        vertices.insert(from);
        vertices.insert(to);
    }
    input.close();

    std::ofstream output(output_path, std::ios_base::binary);
    if (!output.is_open()) {
        std::cout << "Wrong output file!" << std::endl;
        exit(WRONG_FILE);
    }

    // on each step take vertexA with highest degree
    // and output its info like this: vertexA degree neigh1 weight1 neigh2 weight2 ...
    while (!vertices.empty()) {
        auto predicate = [&edges](std::uint32_t u, std::uint32_t v) {
            return edges[u].size() < edges[v].size();
        };
        std::uint32_t from = *std::max_element(vertices.begin(), vertices.end(), predicate);
        vertices.erase(from);
        output.write(reinterpret_cast<char*>(&from), sizeof(from));
        std::uint32_t sz = edges[from].size();
        output.write(reinterpret_cast<char*>(&sz), sizeof(sz));
        for (auto [to, weight] : edges[from]) {
            output.write(reinterpret_cast<char*>(&to), sizeof(to));
            output.write(reinterpret_cast<char*>(&weight), sizeof(weight));
            if (from != to) {
                edges[to].erase({from, (std::uint8_t)weight});
                if (edges[to].size() == 0) {
                    vertices.erase(to);
                }
            }
        }
        edges[from].clear();
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

    std::uint32_t from, to, sz;
    std::uint8_t weight;
    while (input.read(reinterpret_cast<char*>(&from), sizeof(from))) {      // read from binary file
        input.read(reinterpret_cast<char*>(&sz), sizeof(sz));
        for (std::uint32_t i = 0; i < sz; ++i) {
            input.read(reinterpret_cast<char*>(&to), sizeof(to));
            input.read(reinterpret_cast<char*>(&weight), sizeof(weight));
            output << from << "\t" << to << "\t" << (std::uint32_t)weight << "\n"; // output into tsv file
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