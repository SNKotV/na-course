#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

std::pair<double, int> parabolic_integration(double (*f)(double x), double a, double b, double eps);

struct intergated_function {
	double(*f)(double x);
	double a, b;
	std::string f_view;
};

int main(int argc, char** argv) {
#ifdef _DEBUG
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#endif
	std::ios::sync_with_stdio(0);
	std::cout.tie(0); std::cin.tie(0);
	std::cout.setf(std::ios_base::fixed); std::cout.precision(3);
	   
	double eps;

#ifdef _DEBUG
	std::cin >> eps;
#else
	std::cout << "Enter eps = ";
	std::cin >> eps;
#endif

	std::vector<intergated_function> integrals{
		intergated_function{ [](double x) {return std::sin(1.0 / (x * x)); }, 3, 5, "sin(1 / x^2)" },
		intergated_function{ [](double x) {return std::log(x) * std::sin(1.0 / (x * x * x)); }, 3, 4, "ln(x) * sin(1 / x^3)" },
		intergated_function{ [](double x) {return std::exp(2 * x) * std::cos(1.0 / x); }, 1, 2, "e^2x cos (1 / x)" },
		intergated_function{ [](double x) {return std::exp(x) / x * std::sin(1.0 / x * x * x); }, 2, 3, "e^x / x * sin(1 / x^3)" },
		intergated_function{ [](double x) {return std::pow(3.0, x) / (x * x) * std::log(x * x); }, 1, 2, "3^x / x^2 * ln(x^2)" }
	};
	
	for (auto &integral : integrals) {
		std::pair<double, int> integration_info = parabolic_integration(integral.f, integral.a, integral.b, eps);
		std::cout << "Integral of [" << integral.f_view << "] from " << integral.a << " to " << integral.b << " = " << integration_info.first << '\n';
		std::cout << "Number of divisions = " << integration_info.second << '\n';
		std::cout << '\n';
	}

#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif

	return 0;
}

std::pair<double, int> parabolic_integration(double(*f)(double x), double a, double b, double eps) {
	double theta = 1.0 / 15.0;
	int n = 2;
	
	double prev_integral_value;
	double integral_value = (b - a) / 6.0 * (f(a) + 4 * f((a + b) / 2.0) + f(b));

	double delta = 1e18;
	while (delta - eps > 1e-12) {
		n <<= 1;

		prev_integral_value = integral_value;

		double odd_sum = 0;
		double even_sum = 0;

		double h = (b - a) / n;
		double odd_x = a + h;
		double even_x = odd_x + h;

		for (int k = 0; k < n / 2; k++) {
			odd_sum += f(odd_x);
			if (k == n / 2 - 1) break;
			even_sum += f(even_x);
			
			odd_x = even_x + h;
			even_x = odd_x + h;
		}

		integral_value = (b - a) / (3 * n) * (f(a) + 4 * odd_sum + 2 * even_sum + f(b));

		delta = theta * std::fabs(integral_value - prev_integral_value);
	}


	return { integral_value, n };
}