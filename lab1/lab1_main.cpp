#include <fstream>
#include <iostream>

#include "graph.h"


int main() {
    std::list<rule_t> rules = {{"1", {"2", "3"}},   {"1", {"14"}},
                               {"3", {"10", "11"}}, {"2", {"4", "7"}},
                               {"2", {"7", "8"}},   {"4", {"5", "6"}},
                               {"8", {"12"}},       {"8", {"13"}}};
    Graph             graph{rules};

    bool res = graph.dfs({"12", "7", "10", "11"}, "1");
    std::cout << "dfs res = " << (res ? "true" : "false") << std::endl;

    res = graph.bfs({"12", "7", "10", "11"}, "1");
    std::cout << "bfs res = " << (res ? "true" : "false") << std::endl;

    res = graph.from_target({"12", "7", "10", "11"}, "1");
    std::cout << "from_target res = " << (res ? "true" : "false") << std::endl;

    // bool res = graph.dfs({"4", "7", "10"}, "1");
    // std::cout << "dfs res = " << (res ? "true" : "false") << std::endl;

    // res = graph.bfs({"4", "7", "10"}, "1");
    // std::cout << "bfs res = " << (res ? "true" : "false") << std::endl;
    return 0;
}