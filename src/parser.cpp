#include "parser.hpp"
#include <sstream>

Parser::Date Parser::parse_date(const std::string &date) {
    uint8_t month = std::stoul(date.substr(1, 2)) - 1;
    uint8_t day   = std::stoul(date.substr(4, 2));
    uint16_t year = std::stoul(date.substr(7, 4));
    //Format is hardcoded to be "MM/DD/YYYY" as that is the data I'm working with
    //It is possible to make a simple parser that would accept multiple formats,
    //but allowing any format will likely cause more problems unless it made very thorough

    return {day, month, year};
}

Parser::AllTime Parser::parse_file(std::ifstream &file) {
    Parser::AllTime res;
    std::string line;

    std::getline(file, line);
    //Remove first line (header)

    while(std::getline(file, line)) {
        std::stringstream stream(line);
        std::string temp;

        std::getline(stream, temp, ',');
        std::tuple<uint8_t, uint8_t, uint16_t> date(Parser::parse_date(temp));

        std::getline(stream, temp, '"');
        temp.clear();
        char c;
        while((c = static_cast<char>(stream.get())) != '"') {
            if(std::isdigit(c) || c == '.') temp.push_back(c);
        }
        long double close(std::stod(temp));

        res[std::get<2>(date)][std::get<1>(date)][std::get<0>(date)] = {close};
    }

    return res;
}