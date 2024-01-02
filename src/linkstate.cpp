#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <climits>
using namespace std;

typedef pair<int, int> Pair;

struct vertex{
    int source;
    vector<int> distance;
    vector<int> previous;
};

vector<int> resultOfPath(int source, int destination, vertex* vertices)  {
    vector<int> result;

    if (vertices[source].previous[destination] == source) {
        result.push_back(destination);
    }

    int next = destination;
    while (vertices[source].previous[next] != source){
        if (vertices[source].previous[next] == -1){
            next = source;
            result.push_back(next);
            break;
        }
        next = vertices[source].previous[next];
        result.push_back(next);
    }
    return result;
}

void printFromMessageText(FILE* fpOut, string line, vertex* vertices) {
    int source = stoi(line.substr(0, line.find(" ")).c_str());

    int space_index_second = line.find(" ") + 1;
    int destination = stoi(line.substr(line.find(" ") + 1, line.find(" ", space_index_second)).c_str());
    
    string message = line.substr(line.find(" ", space_index_second) + 1);

    if (vertices[source].distance[destination] == INT_MAX) {
        fprintf(fpOut, "from %d to %d cost infinite hops unreachable message %s\n", source, destination, message.c_str());
        return;
    } else {
        if (vertices[source].distance[destination] != INT_MAX) {
            vector<int> path = resultOfPath(source, destination, vertices);
            path.push_back(source);
            fprintf(fpOut, "from %d to %d cost %d hops ", source, destination, vertices[source].distance[destination]);
            for (int i = path.size() - 1; i >= 0; i--) {
                if (path[i] == destination) {
                    continue;
                } else {
                    fprintf(fpOut, "%d ", path[i]);
                }
            }
            fprintf(fpOut, "message %s\n", message.c_str());
        }
    }
}

void dijkstras(vertex *new_vertex, int number_of_vertices, vector<Pair> graph[]) {
    if (new_vertex != NULL) {
        vector<bool> visited(number_of_vertices, false);
        priority_queue<Pair, vector<Pair>, greater<Pair>> priority_queue;
        new_vertex->previous.clear();
        new_vertex->distance.clear();
        new_vertex->distance.resize(number_of_vertices + 1, INT_MAX);
        new_vertex->previous.resize(number_of_vertices + 1, -1);

        new_vertex->distance[new_vertex->source] = 0;
        priority_queue.push(make_pair(0, new_vertex->source));

        while (!priority_queue.empty()) {
            int u = priority_queue.top().second;
            priority_queue.pop();
            if (visited[u] == true) {
                continue;
            }
            visited[u] = true;

            for (int i = 0; i < graph[u].size(); i++) {

                int v = graph[u][i].first;
                int w = graph[u][i].second;
                if (new_vertex->distance[v] == new_vertex->distance[u] + w) {
                    if (new_vertex->previous[v] > u) {
                        new_vertex->previous[v] = u;
                        priority_queue.push(make_pair(new_vertex->distance[v], v));
                    }
                } else if (new_vertex->distance[v] > new_vertex->distance[u] + w) {
                    new_vertex->distance[v] = new_vertex->distance[u] + w;
                    new_vertex->previous[v] = u;
                    priority_queue.push(make_pair(new_vertex->distance[v], v));
                }
            }
        }
    } else {
        return;
    }
}

void adjustToChanges(int weight_int, int destination_int, int source_int, vector<Pair> graph[]) {
    if (weight_int == -999){
        for (int i = 0; i < graph[source_int].size(); i++) {
            if (graph[source_int][i].first == destination_int) {
                graph[source_int].erase(graph[source_int].begin() + i);
                break;
            }
        }
        for (int i = 0; i < graph[destination_int].size(); i++) {
            if (graph[destination_int][i].first == source_int) {
                graph[destination_int].erase(graph[destination_int].begin() + i);
                break;
            }
        }
    }else{
        bool found_vertex = 0;
        for (int i = 0; i < graph[source_int].size(); i++) {
            if (graph[source_int][i].first == destination_int) {
                graph[source_int][i].second = weight_int;
                found_vertex = 1;
                break;
            }
        }
        for (int i = 0; i < graph[destination_int].size(); i++) {
            if (graph[destination_int][i].first == source_int) {
                graph[destination_int][i].second = weight_int;
                found_vertex = 1;
                break;
            }
        }
        if (found_vertex == 0){
            graph[source_int].push_back(make_pair(destination_int, weight_int));
            graph[destination_int].push_back(make_pair(source_int, weight_int));
        }
    }
}

void initializeGraph(string weight, string destination, string source, vector<Pair> graph[]) {
    int source_int = stoi(source.c_str());
    int destination_int = stoi(destination.c_str());
    int weight_int = stoi(weight.c_str());
    if (weight_int != -999) {
        graph[source_int].push_back(make_pair(destination_int, weight_int));
        graph[destination_int].push_back(make_pair(source_int, weight_int));
    }
}


int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    ifstream inFile(argv[1]);
    string source; 
    string destination; 
    string weight;
    
    int number_of_vertices = 0;
    while(inFile >> source >> destination >> weight){
        int source_int = stoi(source.c_str());
        int destination_int = stoi(destination.c_str());
        number_of_vertices = max(destination_int, number_of_vertices);
        number_of_vertices = max(source_int, number_of_vertices);
    }
    inFile.close();

    inFile.open(argv[1]);
    vector<Pair> graph[number_of_vertices + 1]; 
    while(inFile >> source >> destination >> weight){
        initializeGraph(weight, destination, source, graph);
    }
    inFile.close();

    vertex* vertices = new vertex[number_of_vertices + 1];
    for (int i = 1; i <= number_of_vertices; i++) {
        vertices[i].source = i;
        vertices[i].distance.resize(number_of_vertices + 1, INT_MAX);
        vertices[i].previous.resize(number_of_vertices + 1, -1);
    }

    for (int i = 1; i <= number_of_vertices; ++i) {
        dijkstras(&vertices[i], number_of_vertices, graph);
    }

    FILE *fpOut;
    fpOut = fopen("output.txt", "w");

    for (int source = 1; source <= number_of_vertices; source++) {
        for (int destination = 1; destination <= number_of_vertices; ++destination) {
            if (vertices[source].distance[destination] != INT_MAX) {
                vector<int> path = resultOfPath(source, destination, vertices);
                fprintf(fpOut, "%d %d %d\n", destination, path.back(), vertices[source].distance[destination]);
            }
        }
    }
    
    inFile.open(argv[2]);
    string line;
    while(getline(inFile, line)){
        printFromMessageText(fpOut, line, vertices);
    }
    inFile.close();

    ifstream changesFile(argv[3]);
    while(changesFile >> source >> destination >> weight){
        int source_int = stoi(source.c_str());
        int destination_int = stoi(destination.c_str());
        int weight_int = stoi(weight.c_str());

        adjustToChanges(weight_int, destination_int, source_int, graph);

        for (int i = 1; i <= number_of_vertices; ++i) {
            dijkstras(&vertices[i], number_of_vertices, graph);
        }

        for (int source = 1; source <= number_of_vertices; source++) {
            for (int destination = 1; destination <= number_of_vertices; destination++) {
                if (vertices[source].distance[destination] != INT_MAX) {
                    vector<int> path = resultOfPath(source, destination, vertices);
                    fprintf(fpOut, "%d %d %d\n", destination, path.back(), vertices[source].distance[destination]);
                }
            }
        }

        inFile.open(argv[2]);
        while(getline(inFile, line)){
            printFromMessageText(fpOut, line, vertices);
        }
        inFile.close();
    }
    changesFile.close();
    fclose(fpOut);

    return 0;
}

