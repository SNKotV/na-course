#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

struct intergated_function {
	double(*f)(double x, double y, double z);
	std::vector<std::pair<double, double>> bounds;
	std::string f_view;
};

double monte_carlo_integration(intergated_function& f, int n);


int main(int argc, char** argv) {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);

	std::ios::sync_with_stdio(0);
	std::cout.tie(0); std::cin.tie(0);
	std::cout.setf(std::ios_base::fixed); std::cout.precision(3);

	intergated_function f = { [](double x, double y, double z) {return x * x * x; }, { {1, 5}, {2, 4}, {1, 2} }, "x^3" };

	std::vector<double> integrals;

	int n = 1e4;
	for (int i = 0; i < 3; i++) {
		std::cout << "Integral of [" << f.f_view << "]";
		for (auto& b : f.bounds) {
			std::cout << " from " << b.first << " to " << b.second;
		}
		integrals.push_back(monte_carlo_integration(f, n));
		std::cout << " = " << integrals.back() << '\n';
		n *= 1e2;
	}

	std::cout << '\n';

	for (int i = 0; i < 2; i++) {
		double diff = std::fabs(integrals[i] - integrals[i + 1]);
		std::cout << "|I_{" << i + 1 << "} - I_{" << i + 2 << "}| = " << diff << '\n';
	}

	return 0;
}

double monte_carlo_integration(intergated_function& f, int n) {
	double res = 0;

	for (int i = 0; i < n; i++) {
		double x = f.bounds[0].first + (f.bounds[0].second - f.bounds[0].first) * std::rand() * 1.0 / RAND_MAX;
		double y = f.bounds[1].first + (f.bounds[1].second - f.bounds[1].first) * std::rand() * 1.0 / RAND_MAX;
		double z = f.bounds[2].first + (f.bounds[2].second - f.bounds[2].first) * std::rand() * 1.0 / RAND_MAX;
		res += f.f(x, y, z);
	}

	for (auto&b : f.bounds) {
		res *= b.second - b.first;
	}

	res /= n;

	return res;
}