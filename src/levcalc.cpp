#include "levcalc.hpp"
#include "parser.hpp"

#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

static std::ifstream check_and_open_file(const fs::path &file_path) {
    if (!fs::exists(file_path))
        throw std::runtime_error(file_path.string() + " does not exist.");

    if (!fs::is_regular_file(file_path))
        throw std::runtime_error(file_path.string() + " is not a file.");

    std::ifstream file(file_path);

    if (!file.is_open())
        throw std::runtime_error(file_path.string() + " could not be opened.");

    return file;
}

static auto generic_set(const fs::path &file_path) {
    std::ifstream file{ check_and_open_file(file_path) };

    auto result = Parser::parse_file(file);

    file.close();
    return result;
}

LevCalc &LevCalc::set_risk_free_rate(const fs::path &file_path) {
    risk_free_rate = generic_set(file_path);
    return *this;
}

LevCalc &LevCalc::set_stock_return(const fs::path &file_path) {
    stock_return = generic_set(file_path);
    return *this;
}

LevCalc &LevCalc::compute_leverage(const long double &leverage, const long double &fee) {
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

                    day_leverage_return = day_stock_return * leverage                                                             // Return with free leverage
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

