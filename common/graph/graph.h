#pragma once

#include <iostream>
#include <iterator>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string_view>

#include "rule.h"

class Graph {
public:
    Graph(const std::list<rule_t>& rules) : rules_{rules} {}

    bool dfs(const std::list<std::string>& input, const std::string& target) {
        std::cout << "\n\nПоиск в глубину\n";

        working_memory_.clear();
        working_memory_.insert(input.begin(), input.end());

        std::list<const rule_t*> rules;
        for (auto& rule : rules_) {
            rules.push_back(&rule);
        }
        std::list<const rule_t*> rules_stack;

        bool rv = false;
        add_rules_to_stack(rules, rules_stack);
        print_current_state(rules_stack.begin(), rules_stack.end());

        while (!rules_stack.empty()) {
            auto rule = rules_stack.back();
            working_memory_.insert(rule->result);
            rules_stack.pop_back();


            print_current_state(rules_stack.begin(), rules_stack.end(), rule);

            if (rule->result == target) {
                rv = true;
                break;
            }

            add_rules_to_stack(rules, rules_stack);
        }

        return rv;
    }


    bool bfs(const std::list<std::string>& input, const std::string& target) {
        std::cout << "\n\nПоиск в ширину\n";

        working_memory_.clear();
        working_memory_.insert(input.begin(), input.end());

        std::list<const rule_t*> rules;
        for (auto& rule : rules_) {
            rules.push_back(&rule);
        }

        std::list<const rule_t*> rules_queue;

        bool rv = false;
        add_rules_to_queue(rules, rules_queue);
        print_current_state(rules_queue.begin(), rules_queue.end());

        while (!rules_queue.empty()) {
            auto rule = rules_queue.front();
            rules_queue.pop_front();

            working_memory_.insert(rule->result);
            print_current_state(rules_queue.begin(), rules_queue.end(), rule);

            if (rule->result == target) {
                rv = true;
                break;
            }

            add_rules_to_queue(rules, rules_queue);
        }

        return rv;
    }

private:
    void add_rules_to_stack(std::list<const rule_t*>& rules,
                            std::list<const rule_t*>& rules_stack) {
        for (auto it = rules.begin(); it != rules.end(); it++) {
            if (is_rule_true(*it)) {
                rules_stack.push_back(*it);
                rules.erase(it--);
            }
        }
    }

    void add_rules_to_queue(std::list<const rule_t*>& rules,
                            std::list<const rule_t*>& rules_queue) {
        for (auto it = rules.begin(); it != rules.end(); it++) {
            if (is_rule_true(*it)) {
                rules_queue.push_back(*it);
                rules.erase(it--);
            }
        }
    }

    bool is_rule_true(const rule_t* rule) {
        for (auto& precondition : rule->preconditions) {
            if (!working_memory_.count(precondition)) {
                return false;
            }
        }
        return true;
    }

    void print_current_state() {
        std::cout << "Рабочая память: \n\t";
        for (auto it = working_memory_.begin(); it != working_memory_.end();
             it++) {
            std::cout << *it << " ";
        }

        std::cout << "\n\n";
    }

    template <class It>
    void print_current_state(It first, It last,
                             const rule_t* current_rule = nullptr) {
        std::cout << "Рабочая память: \n\t";
        for (auto it = working_memory_.begin(); it != working_memory_.end();
             it++) {
            std::cout << *it << " ";
        }

        if (current_rule) {
            std::cout << "\nТекущее правило: \n";
            std::cout << "\t" << current_rule->name << ": "
                      << current_rule->result << " <- ";
            for (auto it1 = current_rule->preconditions.begin();
                 it1 != current_rule->preconditions.end(); it1++) {
                std::cout << *it1 << " ";
            }
        }

        std::cout << "\nПравила в памяти: \n";
        for (It it = first; it != last; it++) {
            std::cout << "\t" << (*it)->name << ": " << (*it)->result << " <- ";
            for (auto it1 = (*it)->preconditions.begin();
                 it1 != (*it)->preconditions.end(); it1++) {
                std::cout << *it1 << " ";
            }
            std::cout << "\n";
        }


        std::cout << "\n\n";
    }


    const std::list<rule_t>    rules_;
    std::set<std::string_view> working_memory_;
};