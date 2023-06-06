#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <tuple>
#include <string>

#include "levcalc.hpp"

class Parser {
public:
    using AllTime   = LevCalc::AllTime;
    using Date      = std::tuple<uint8_t, uint8_t, uint16_t>;

    static Date parse_date(const std::string &date);

    static AllTime parse_file(std::ifstream &file);
};

#endif //PARSER_HPP