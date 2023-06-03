#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <unordered_map>
#include <sstream>
#include <array>
#include <tuple>
#include <stdexcept>
#include "boost/functional/hash.hpp"
#include "gnuplot-iostream/gnuplot-iostream.h"

static std::string float_to_string(const long double &num) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << num;
    return stream.str();
}

class LevCalc {
public:
    using Day       = struct { long double close; }; // Might add more data later, thus struct
    using Month     = std::map<uint8_t, Day>;
    using Year      = std::array<Month, 12>;
    using AllTime   = std::map<uint16_t, Year>;
    using PlotData  = std::vector<std::tuple<long double, long double>>;
    //                                          ^^^^^        ^^^^^
    //                                            x            y
    //                                          (time)       (price)
    // x = current time in years. i.e. 0.5 = 6 months
    // y = current price

    using PlotCache = std::unordered_map<std::pair<long double, long double>, PlotData,
                             boost::hash<std::pair<long double, long double>>>;

private:
    AllTime risk_free_rate;
    AllTime stock_return;
    PlotCache plot_cache;



    static std::tuple<uint8_t, uint8_t, uint16_t> parse_date(const std::string &date) {
        uint8_t month = std::stoul(date.substr(1, 2)) - 1;
        uint8_t day = std::stoul(date.substr(4, 2));
        uint16_t year = std::stoul(date.substr(7, 4));
        //Format is hardcoded to be "MM/DD/YYYY" as that is the data I'm working with
        //It is possible to make a simple parser that would accept multiple formats,
        //but allowing any format will likely cause more problems unless it made very thorough

        return {day, month, year};
    }

    static AllTime parse_file(std::ifstream &file) {
        AllTime res;
        std::string line;

        std::getline(file, line);
        //Remove first line (header)

        while(std::getline(file, line)) {
            std::stringstream stream(line);
            std::string temp;

            std::getline(stream, temp, ',');
            std::tuple<uint8_t, uint8_t, uint16_t> date(parse_date(temp));

            std::getline(stream, temp, '"');
            temp.clear();
            char c;
            while((c = static_cast<char>(stream.get())) != '"') {
                if(std::isdigit(c) || c == '.') temp.push_back(c);
            }
            long double close(std::stod(temp));

            res[get<2>(date)][get<1>(date)][get<0>(date)] = {close};
        }

        return res;
    }

public:
    LevCalc& set_risk_free_rate(const std::string &file_path) {
        std::ifstream file(file_path);

        if(!file.is_open()) {
            throw std::runtime_error(file_path + " could not be opened.");
        }

        risk_free_rate = parse_file(file);

        file.close();
        return *this;
    }

    LevCalc& set_stock_return(const std::string &file_path) {
        std::ifstream file(file_path);

        if(!file.is_open()) {
            throw std::runtime_error(file_path + " could not be opened.");
        }

        stock_return = parse_file(file);

        file.close();
        return *this;
    }

    LevCalc& compute_leverage(const long double &leverage, const long double &fee) {
        long double last_risk_free_rate = 0.0;
        bool have_last_risk_free_rate = false;
        long double last_day_stock_close = 0.0;
        long double last_leveraged_day_stock_close = 0.0;

        PlotData res;

        for(auto &[year, months] : stock_return) {

            // Skip years that don't have risk-free rate data
            if(!have_last_risk_free_rate && !risk_free_rate.contains(year)) continue;

            size_t days_in_year = 0, days_passed = 0;

            for(auto &days : months) {
                days_in_year += days.size();
            }

            for(size_t month = 0; auto &days : months) {

                // Skip months that don't have risk-free rate data
                if(have_last_risk_free_rate || !risk_free_rate[year][month].empty())
                for(auto &[day, data] : days) {
                    ++days_passed;
                    if(!have_last_risk_free_rate && !risk_free_rate[year][month].contains(day)) continue;
                    // Skip days that don't have risk-free rate data

                    long double day_stock_close;
                    long double leveraged_day_stock_close;
                    long double day_stock_return;
                    long double day_leverage_return;
                    long double timestamp;

                    timestamp = year + static_cast<long double>(days_passed) / static_cast<long double>(days_in_year);

                    if(!have_last_risk_free_rate) {
                        have_last_risk_free_rate = true;
                        last_risk_free_rate = risk_free_rate[year][month][day].close;

                        last_day_stock_close = last_leveraged_day_stock_close = data.close;
                        res.emplace_back(timestamp, last_leveraged_day_stock_close);
                        continue;
                    }

                    day_stock_close = data.close;

                    day_stock_return = day_stock_close / last_day_stock_close - 1.0;

                    day_leverage_return = day_stock_return * leverage                                               // Return with free leverage
                            - (leverage - 1.0) * last_risk_free_rate / 100 / static_cast<long double>(days_in_year) // Cost of borrowing
                            - fee / 100 / static_cast<long double>(days_in_year);                                   // Fund management fee


                    leveraged_day_stock_close = last_leveraged_day_stock_close * (1.0 + day_leverage_return);



                    res.emplace_back(timestamp, leveraged_day_stock_close);

                    if(risk_free_rate.contains(year) && risk_free_rate[year][month].contains(day)) {
                        last_risk_free_rate = risk_free_rate[year][month][day].close;
                    }
                    last_day_stock_close = day_stock_close;
                    last_leveraged_day_stock_close = leveraged_day_stock_close;
                } else {
                    days_passed += days.size();
                }
                ++month;
            }
        }

        plot_cache[std::make_pair(leverage, fee)] = res;
        return *this;
    }

    PlotData &get_plot_data(const long double &leverage, const long double &fee) {
        return plot_cache[std::make_pair(leverage, fee)];
    }

    PlotCache &get_plot_cache() {
        return plot_cache;
    }
};

int main() {
    LevCalc levCalc;

    levCalc.set_risk_free_rate("USD.csv");
    levCalc.set_stock_return("SP500.csv");

    levCalc.compute_leverage(3, 1.0);
    levCalc.compute_leverage(2, 0.6);
    levCalc.compute_leverage(1, 0.2);
    levCalc.compute_leverage(0.75, 0.1);

    Gnuplot gp;
    gp << "set xlabel 'Year'\n";
    gp << "set ylabel 'Price'\n";
    gp << "set title 'Leveraged SP500'\n";

    auto plots = levCalc.get_plot_cache();
    std::string command("plot ");
    for(auto & plot : plots) {
        command += "'-' with lines title '" + float_to_string(plot.first.first) + "X, " + float_to_string(plot.first.second) + "%', ";
    }

    command.pop_back();
    command.pop_back();
    command += "\n";
    gp << command;

    for(auto & [key, plot] : plots) {
        gp.send1d(plot);
    }

    return 0;
}