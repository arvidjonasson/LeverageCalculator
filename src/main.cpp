#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "gnuplot-iostream/gnuplot-iostream.h"
#include "levcalc.hpp"

#define PROGRAM_NAME    (argv[0])
#define RISK_FREE_RATE  (argv[1])
#define STOCK_RETURN    (argv[2])
#define LEV(X)          (argv[3 + 2 * X])
#define FEE(X)          (argv[4 + 2 * X])
#define NUMBER_OF_PLOTS (argc < 5 ? 0 : ((argc - 3) / 2))
#define INCORRECT_ARGS  (argc < 5 ? 1 : ((argc - 3) % 2)) //If NUMBER_OF_PLOTS == 0 or LEV.size() != FEE.size()

static void plot_data(const LevCalc::PlotCache &plot_data) {
    auto iterator = plot_data.cbegin();
    std::ostringstream command;

    auto add_plot = [&command, &iterator]() {
        const auto &[key, plot]     = *iterator;
        const auto &[leverage, fee] = key;

        command << "'-' with lines title '" << leverage << "X, " << fee << "%'";
        ++iterator;
    };

    Gnuplot gp;
    gp << "set xlabel 'Year'\n";
    gp << "set ylabel 'Price'\n";
    gp << "set title 'Leveraged SP500'\n";
    //gp << "set output 'plot.png'\n";
    //gp << "set term png size 1920,1080\n";

    command << std::fixed << std::setprecision(2);
    command << "plot ";

    add_plot();

    while(iterator != plot_data.cend()) {
        command << ", ";
        add_plot();
    }

    command << '\n';
    gp << command.str();

    for(const auto &[key, plot] : plot_data) {
        gp.send1d(plot);
    }
}

int main(const int argc, const char *const *const argv) {
    if (INCORRECT_ARGS) {
        // If the program wasn't called with the correct argument

        std::cerr << "Usage: " << PROGRAM_NAME << " <risk_free_rate_file> <stock_return_file> <leverage1> <fee1> [<leverage2> <fee2> ...]" << std::endl;
        std::cerr << "  <risk_free_rate_file>  Path to the risk-free rate input file (CSV format)" << std::endl;
        std::cerr << "  <stock_return_file>    Path to the stock return input file (CSV format)" << std::endl;
        std::cerr << "  <leverage>             Leverage for each scenario (decimal number with 2 decimal places)" << std::endl;
        std::cerr << "  <fee>                  Fee for each scenario (decimal number with 2 decimal places)" << std::endl;

        return EXIT_FAILURE;
    }

    std::vector<std::pair<long double, long double>> leverage_fees;
    leverage_fees.reserve(NUMBER_OF_PLOTS);

    for(int i = 0; i < NUMBER_OF_PLOTS; ++i) {
        leverage_fees.emplace_back(std::stold(LEV(i)), std::stold(FEE(i)));
    }

    LevCalc levCalc;

    levCalc.set_risk_free_rate(RISK_FREE_RATE);
    levCalc.set_stock_return(STOCK_RETURN);

    for(auto const& [leverage, fee] : leverage_fees) {
        levCalc.compute_leverage(leverage, fee);
    }

    const auto &plots = levCalc.get_plot_cache();

    if(plots.empty()) {
        std::cerr << "No data to plot" << std::endl;
        return EXIT_FAILURE;
    }

    plot_data(plots);

#ifdef _WIN32
	// For Windows, prompt for a keystroke before the Gnuplot object goes out of scope so that
	// the gnuplot window doesn't get closed.
	std::cout << "Press enter to exit." << std::endl;
	std::cin.get();
    // http://stahlke.org/dan/gnuplot-iostream/
#endif

    return EXIT_SUCCESS;
}