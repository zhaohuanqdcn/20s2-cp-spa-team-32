#ifndef __PARSER__HELPER__H__
#define __PARSER__HELPER__H__

#include <vector>
#include <unordered_map>
#include <map>
#include <queue>

using namespace std;

/*
*   Check if edges contain call to a non-exist procedure.  
*/
bool checkCallOfNonProcedure(unordered_map<string, vector<string> >& edges, set<string>& allProcedures) {
    for (auto& listEdge: edges) {
        for (auto& v: listEdge.second) {
            if (allProcedures.find(v) == allProcedures.end()) {
                return true;
            }
        }
    }
    return false;
}

/*
*  Check if SIMPLE code contain any cyclic call of procedure, given a map of procedure call for each procedure name
*/
bool checkCyclicCalls(unordered_map<string, vector<string> >& edges) {
    map<string, int> degree;

    for (auto& x: edges) {
        degree[x.first] += 0;
        for (auto& y: x.second) {
            degree[y]++;
        }
    }

    int needed = degree.size();
    queue<string> qu;
    for (auto& x: degree) {
        if (x.second == 0) {
            qu.push(x.first);
        }
    }

    int collected = 0;
    while (qu.size() > 0) {
        auto u = qu.front();
        qu.pop();
        ++collected;
        for (auto& v: edges[u]) {
            degree[v]--;
            if (degree[v] == 0) {
                qu.push(v);
            }
        }
    }

    return collected != needed;
}

#endif  ///__PARSER__HELPER__H__