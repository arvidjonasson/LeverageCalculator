#pragma once
#ifndef LEVCALC_HPP
#define LEVCALC_HPP

#include <unordered_map>
#include <array>
#include <tuple>
#include <utility>
#include <map>
#include <vector>
#include "boost/functional/hash.hpp"

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
    using PlotCacheHash = boost::hash<std::pair<long double, long double>>;
    using PlotCacheKey  = std::pair<long double, long double>;
    using PlotCache     = std::unordered_map<PlotCacheKey, PlotData, PlotCacheHash>;

private:
    AllTime risk_free_rate;
    AllTime stock_return;
    PlotCache plot_cache;

public:
    LevCalc& set_risk_free_rate(const std::string &file_path);

    LevCalc& set_stock_return(const std::string &file_path);

    LevCalc& compute_leverage(const long double &leverage, const long double &fee);

    PlotData &get_plot_data(const long double &leverage, const long double &fee) {
        return plot_cache[std::make_pair(leverage, fee)];
    }

    PlotCache &get_plot_cache() {
        return plot_cache;
    }
};

#endif // LEVCALC_HPP