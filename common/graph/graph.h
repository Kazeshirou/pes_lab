#pragma once

#include <algorithm>
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

        bool   rv   = false;
        size_t iter = 0;
        print_iteration(iter);
        print_current_state(rules_stack.begin(), rules_stack.end());
        add_rules_to_stack(rules, rules_stack);
        print_current_state(rules_stack.begin(), rules_stack.end());

        while (!rules_stack.empty()) {
            print_iteration(iter);
            auto rule = rules_stack.back();
            rules_stack.pop_back();
            print_current_state(rules_stack.begin(), rules_stack.end(), rule);
            working_memory_.insert(rule->result);
            print_current_state(rules_stack.begin(), rules_stack.end(), rule);

            if (rule->result == target) {
                rv = true;
                break;
            }

            add_rules_to_stack(rules, rules_stack);
            print_current_state(rules_stack.begin(), rules_stack.end());
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
        size_t                   iter = 0;
        print_iteration(iter);
        print_current_state(rules_queue.begin(), rules_queue.end());

        bool rv = false;
        add_rules_to_queue(rules, rules_queue);
        print_current_state(rules_queue.begin(), rules_queue.end());

        while (!rules_queue.empty()) {
            print_iteration(iter);
            auto rule = rules_queue.front();
            rules_queue.pop_front();
            print_current_state(rules_queue.begin(), rules_queue.end(), rule);
            working_memory_.insert(rule->result);
            print_current_state(rules_queue.begin(), rules_queue.end(), rule);

            if (rule->result == target) {
                rv = true;
                break;
            }

            add_rules_to_queue(rules, rules_queue);
            print_current_state(rules_queue.begin(), rules_queue.end());
        }

        return rv;
    }

    bool from_target(const std::list<std::string>& input,
                     const std::string&            target) {
        std::cout << "\n\nПоиск от цели\n";

        opened_facts_.clear();
        closed_facts_.clear();
        opened_rules_.clear();
        closed_rules_.clear();

        opened_facts_.push_back(target);

        size_t iter = 0;
        print_iteration(iter);
        print_current_state();

        std::list<const rule_t*> allowed_rules;
        for (auto& rule : rules_) {
            allowed_rules.push_back(&rule);
        }

        bool rv = false;

        do {
            print_iteration(iter);

            auto current_fact = opened_facts_.back();

            bool is_rule_added = false;

            for (auto it = allowed_rules.begin(); (it != allowed_rules.end());
                 it++) {
                if ((*it)->result == current_fact) {
                    opened_rules_.push_back(*it);
                    allowed_rules.erase(it--);

                    is_rule_added = true;
                    break;
                }
            }

            print_current_state(current_fact);

            if (is_rule_added) {
                auto current_rule       = opened_rules_.back();
                auto rule_preconditions = current_rule->preconditions;

                for (auto it = rule_preconditions.begin();
                     it != rule_preconditions.end(); it++) {
                    opened_facts_.push_back(*it);
                }

                print_current_state(current_fact, current_rule);
                while (will_move_to_closed(input)) {
                    closed_facts_.push_back(opened_facts_.back());
                    opened_facts_.pop_back();

                    print_current_state();

                    if (opened_rules_.size()) {
                        if (is_rule_can_be_closed(opened_rules_.back())) {
                            closed_rules_.push_back(opened_rules_.back());
                            opened_rules_.pop_back();

                            print_current_state();
                        }
                    }
                }

                if (std::any_of(
                        closed_facts_.begin(), closed_facts_.end(),
                        [target](std::string& s) { return s == target; })) {
                    rv = true;
                    break;
                }
            } else {
                opened_facts_.pop_back();

                print_current_state();

                if (opened_rules_.size()) {
                    auto rule_preconditions =
                        opened_rules_.back()->preconditions;

                    if (!is_any_of_precondition_opened(rule_preconditions)) {
                        opened_rules_.pop_back();

                        print_current_state();
                    }
                }
            }
        } while (opened_facts_.size());

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

    bool is_rule_can_be_closed(const rule_t* rule) {
        std::set<std::string_view> closed_set(closed_facts_.begin(),
                                              closed_facts_.end());
        for (auto& precondition : rule->preconditions) {
            if (!closed_set.count(precondition)) {
                return false;
            }
        }
        return true;
    }


    bool will_move_to_closed(const std::list<std::string>& input) {
        return opened_facts_.size() &&
               (std::any_of(input.begin(), input.end(),
                            [this](const std::string& s) -> bool {
                                return s == opened_facts_.back();
                            }) ||
                (closed_rules_.size() &&
                 (closed_rules_.back()->result == opened_facts_.back())));
    }

    bool is_any_of_precondition_opened(
        const std::set<std::string> preconditions) {
        for (auto& opened_fact : opened_facts_) {
            if (preconditions.count(opened_fact)) {
                return true;
            }
        }
        return false;
    }

    void print_iteration(size_t& iter) {
        std::cout << "------ Итерация " << iter++ << " -------\n";
    }

    void print_current_state(const std::string_view current_fact = "",
                             const rule_t*          current_rule = nullptr) {
        std::string firs_without_enter = "";
        if (current_fact.length()) {
            std::cout << firs_without_enter << "Текущий факт: \n\t"
                      << current_fact;
            firs_without_enter = "\n";
        }

        if (current_rule) {
            std::cout << firs_without_enter << "Текущee правило: \n";
            std::cout << "\t" << current_rule->name << ": "
                      << current_rule->result << " <- ";
            for (auto it1 = current_rule->preconditions.begin();
                 it1 != current_rule->preconditions.end(); it1++) {
                std::cout << *it1 << " ";
            }
            std::cout << "\n";
            firs_without_enter = "\n";
        }

        std::cout << firs_without_enter << "Список открытых фактов: \n\t";
        for (auto it = opened_facts_.begin(); it != opened_facts_.end(); it++) {
            std::cout << *it << " ";
        }


        std::cout << "\nСписок закрытых фактов: \n\t";
        for (auto it = closed_facts_.begin(); it != closed_facts_.end(); it++) {
            std::cout << *it << " ";
        }


        std::cout << "\nСписок открытых правил: \n";
        for (auto it = opened_rules_.begin(); it != opened_rules_.end(); it++) {
            std::cout << "\t" << (*it)->name << ": " << (*it)->result << " <- ";
            for (auto it1 = (*it)->preconditions.begin();
                 it1 != (*it)->preconditions.end(); it1++) {
                std::cout << *it1 << " ";
            }
            std::cout << "\n";
        }

        std::cout << "Список закрытых правил: \n";
        for (auto it = closed_rules_.begin(); it != closed_rules_.end(); it++) {
            std::cout << "\t" << (*it)->name << ": " << (*it)->result << " <- ";
            for (auto it1 = (*it)->preconditions.begin();
                 it1 != (*it)->preconditions.end(); it1++) {
                std::cout << *it1 << " ";
            }
            std::cout << "\n";
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
    std::list<const rule_t*>   opened_rules_;
    std::list<const rule_t*>   closed_rules_;
    std::list<std::string>     opened_facts_;
    std::list<std::string>     closed_facts_;
};