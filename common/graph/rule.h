#pragma once

#include <set>
#include <string>

struct rule_t {
    rule_t(const std::string&          init_result,
           const std::set<std::string> init_preconditions)
            : result{init_result}, preconditions{init_preconditions} {}
    const std::string           result;
    const std::set<std::string> preconditions;
    const std::string           name{"rule-" + std::to_string(all_++)};

private:
    static size_t all_;
};
