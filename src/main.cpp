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
#include "levcalc.hpp"

static std::string float_to_string(const long double &num) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << num;
    return stream.str();
}

static void plot_data(const LevCalc::PlotCache &plot_data) {
    Gnuplot gp;
    gp << "set xlabel 'Year'\n";
    gp << "set ylabel 'Price'\n";
    gp << "set title 'Leveraged SP500'\n";
    //gp << "set output 'plot.png'\n";
    //gp << "set term png size 1920,1080\n";
    
    std::string command("plot ");
    for(auto & plot : plot_data) {
        command += "'-' with lines title '" + float_to_string(plot.first.first) + "X, " + float_to_string(plot.first.second) + "%', ";
    }

    command.pop_back();
    command.pop_back();
    command += "\n";
    gp << command;

    for(auto const & [key, plot] : plot_data) {
        gp.send1d(plot);
    }

#ifdef _WIN32
	// For Windows, prompt for a keystroke before the Gnuplot object goes out of scope so that
	// the gnuplot window doesn't get closed.
	std::cout << "Press enter to exit." << std::endl;
	std::cin.get();
    // http://stahlke.org/dan/gnuplot-iostream/
#endif
}

int main(const int argc, const char *const *const argv) {
    if (argc < 5 || (argc - 3) % 2 != 0) {
        std::cerr << "Usage: " << argv[0] << " <risk_free_rate_file> <stock_return_file> <leverage1> <fee1> [<leverage2> <fee2> ...]" << std::endl;
        std::cerr << "  <risk_free_rate_file>  Path to the risk-free rate input file (CSV format)" << std::endl;
        std::cerr << "  <stock_return_file>    Path to the stock return input file (CSV format)" << std::endl;
        std::cerr << "  <leverage>             Leverage for each scenario (decimal number with 2 decimal places)" << std::endl;
        std::cerr << "  <fee>                  Fee for each scenario (decimal number with 2 decimal places)" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string risk_free_rate_file (argv[1]);
    const std::string stock_return_file   (argv[2]);

    std::vector<std::pair<long double, long double>> leverage_fees;
    leverage_fees.reserve((argc - 3) / 2);

    for(int i = 3; i < argc; i += 2) {
        leverage_fees.emplace_back(std::stold(argv[i]), std::stold(argv[i + 1]));
    }

    LevCalc levCalc;

    levCalc.set_risk_free_rate(risk_free_rate_file);
    levCalc.set_stock_return(stock_return_file);

    for(auto const& [leverage, fee] : leverage_fees) {
        levCalc.compute_leverage(leverage, fee);
    }

    const auto &plots = levCalc.get_plot_cache();

    if(plots.empty()) {
        std::cerr << "No data to plot" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    plot_data(plots);

    std::exit(EXIT_SUCCESS);
}