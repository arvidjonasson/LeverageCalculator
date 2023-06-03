# LeverageCalculator

This is a C++ program that calculates the leveraged return of stocks given the risk-free rate and stock return data, and then generates a plot of the leveraged return using the Gnuplot library.

The code uses CSV files to get data on stock return and risk-free rate, and calculates the leveraged return for different leverage and fee scenarios. This calculated data is then plotted using Gnuplot.

## Requirements

* C++17
* [Gnuplot-iostream](https://github.com/dstahlke/gnuplot-iostream) library
* Boost library

## Compilation

To compile this code, you would need a C++ compiler that supports the C++17 standard or later. 

For example, you can use the `g++` compiler as follows:

```
g++ -std=c++17 -I/path/to/boost/include -I/path/to/gnuplot-iostream -L/path/to/boost/lib -lboost_iostreams -lboost_system -lboost_filesystem main.cpp -o levCalc
```

Replace `/path/to/boost/include`, `/path/to/gnuplot-iostream` and `/path/to/boost/lib` with the paths where the Boost and Gnuplot-iostream libraries are located on your system.

## Running the Program

To run the program, you need to have two CSV files: one for the risk-free rate and another for the stock return. These files should have dates formatted as `MM/DD/YYYY` and the corresponding data value for each date.

The program expects the CSV files to be named `USD.csv` for the risk-free rate and `SP500.csv` for the stock return. These files should be located in the same directory as the executable.

Once you have these files, you can run the program as follows:

```
./levCalc
```

After running the program, you will receive a plot of the leveraged returns for different leverage and fee scenarios.

![plot](https://github.com/arvidjonasson/LeverageCalculator/assets/111796600/a667216c-48c2-4b0f-ada8-69303d346009)
- An example comparing `3X`, `2X`, `unlevered` and `0.75X` with `1%`, `0.6%`, `0.2%` and `0.1%` in annual fees respectively.

## Code Overview

* `float_to_string(const long double &num)`: Helper function to convert float numbers to string with fixed precision.

* `LevCalc`: Main class of the program that contains functions to parse CSV data files, compute leveraged returns, and generate plot data.

  - `set_risk_free_rate(const std::string &file_path)`: Set risk-free rate data from a CSV file.
  - `set_stock_return(const std::string &file_path)`: Set stock return data from a CSV file.
  - `compute_leverage(const long double &leverage, const long double &fee)`: Compute leveraged return for a given leverage and fee.
  - `get_plot_data(const long double &leverage, const long double &fee)`: Get plot data for a given leverage and fee.
  - `get_plot_cache()`: Get all plot data.

* `main()`: Main function of the program. It sets risk-free rate and stock return data, computes leveraged return for different scenarios, and generates a plot using Gnuplot.

## Note

The paths to the `USD.csv` and `SP500.csv` files are hardcoded in the `main()` function. If your files are located in a different directory or have different names, you need to modify the file paths in the `main()` function.
