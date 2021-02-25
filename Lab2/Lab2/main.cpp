#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

double newton_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x);
double gauss_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x);

std::vector<std::vector<double>> compute_divided_differences_table(std::vector<std::pair<double, double>> &key_value_table);
double f(double x);

std::vector<double> factorials;


int main(int argc, char** argv) {
#ifdef _DEBUG
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
#endif
	std::ios::sync_with_stdio(0);
	std::cout.tie(0); std::cin.tie(0);
	std::cout.setf(std::ios_base::fixed); std::cout.precision(3);

	int n;	std::string filename;

#ifdef _DEBUG
	std::cin >> n;
#else
	std::cout << "Enter name of table file: ";
	std::cin >> filename;
#endif

	std::vector<std::pair<double, double>> key_value_table;

#ifdef _DEBUG
	key_value_table.resize(n);
	for (auto& p : key_value_table) {
		std::cin >> p.first >> p.second;
	}
#else
	std::ifstream fin(filename);
	if (!fin.is_open()) {
		std::cout << "File not found\nEvaluation aborted\n";
		return 0;
	}
	fin >> n;
	key_value_table.resize(n);
	for (auto& p : key_value_table) {
		fin >> p.first >> p.second;
	}
#endif

	double x;

#ifdef _DEBUG
	std::cin >> x;
#else
	std::cout << "Enter x = ";
	std::cin >> x;
#endif 

	if (x - key_value_table[0].first < 1e-12 || x - key_value_table[n - 1].first > 1e-12) {
#ifndef _DEBUG
		std::cout << "x = " << x << " is out of bounds [" << key_value_table[0].first << " , " << key_value_table[n - 1].first << "]\n"
			<< "Evaluation aborted\n";
		std::cin.get();
		std::cin.get();
#endif
		return 0;
	}

	factorials.resize(n + 1);
	factorials[0] = 1;
	for (int i = 1; i <= n; i++) {
		factorials[i] = i * factorials[i - 1];
	}

	std::cout << "Newton interpolation: f(" << x << ") = " << newton_interpolation(key_value_table, x) << '\n';
	std::cout << "Gauss interpolation: f(" << x << ") = " << gauss_interpolation(key_value_table, x) << '\n';

	std::cout << "\nComputing divided differences for 3x^6 - 2x^4 + 7x^3 - 4x + 1\n\n";

	n = 11;
	key_value_table.resize(n);
	x = 0;
	for (int i = 0; i < n; i++) {
		key_value_table[i] = { x, f(x) };
		x += 0.1;
	}
	std::vector<std::vector<double>> divided_diff_table = compute_divided_differences_table(key_value_table);
	for (int i = 0; i < n; i++) {
		std::cout << std::setw(6) << i << ' ';
	}
	std::cout << '\n';
	for (auto& row : divided_diff_table) {
		for (auto& y : row) {
			std::cout << std::setw(6) << y << ' ';
		}
		std::cout << '\n';
	}

#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif

	return 0;
}

double newton_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x) {
	int n = key_value_table.size();
	std::vector<std::vector<double>> divided_diff_table = compute_divided_differences_table(key_value_table);

	int m = n / 2;
	double res;
	double h = key_value_table[1].first - key_value_table[0].first;
	
	if (std::fabs(x - key_value_table[m].first) < 1e-12) {
		return key_value_table[m].second;
	}
	else if (x > key_value_table[m].first) {
		res = divided_diff_table[n - 1][0];
		double t = (x - key_value_table[n - 1].first) / h;

		for (int i = 1; i < n; i++) {
			double multiplier = t;
			for (int j = 1; j < i; j++) {
				multiplier *= t + j;
			}
			res += multiplier * divided_diff_table[n - i - 1][i] / factorials[i];
		}
	}
	else {
		res = divided_diff_table[0][0];
		double t = (x - key_value_table[0].first) / h;

		for (int i = 1; i < n; i++) {
			double multiplier = t;
			for (int j = 1; j < i; j++) {
				multiplier *= t - j;
			}
			res += multiplier * divided_diff_table[0][i] / factorials[i];
		}
	}

	return res;
}

double gauss_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x) {
	int n = key_value_table.size();
	std::vector<std::vector<double>> divided_diff_table = compute_divided_differences_table(key_value_table);

	int m = n / 2;
	double res = divided_diff_table[m][0];
	double h = key_value_table[1].first - key_value_table[0].first;
	double t = (x - key_value_table[m].first) / h;

	if (std::fabs(x - key_value_table[m].first) < 1e-12) {
		return key_value_table[m].second;
	}
	else if (x > key_value_table[m].first) {
		for (int i = 1; i < n - 1; i++) {
			double multiplier = t;
			int k = 1;
			for (int j = 1; j < i; j++) {
				if (j % 2 != 0) {
					multiplier *= t - k;
				}
				else {
					multiplier *= t + k;
					k++;
				}
			}
			res += multiplier * divided_diff_table[m][i] / factorials[i];
			if (i % 2 != 0) {
				m--;
			}
		}
	}
	else {
		m--;

		for (int i = 1; i < n - 1; i++) {
			double multiplier = t;
			int k = 1;
			for (int j = 1; j < i; j++) {
				if (j % 2 == 0) {
					multiplier *= t - k;
					k++;
				}
				else {
					multiplier *= t + k;
				}
			}
			res += multiplier * divided_diff_table[m][i] / factorials[i];
			if (i % 2 == 0) {
				m--;
			}
		}
	}

	return res;
}

std::vector<std::vector<double>> compute_divided_differences_table(std::vector<std::pair<double, double>> &key_value_table) {
	int n = key_value_table.size();

	std::vector<std::vector<double>> divided_diff_table(n, std::vector<double>(n));
	for (int i = 0; i < n; i++) {
		divided_diff_table[i][0] = key_value_table[i].second;
	}
	for (int j = 1; j < n; j++) {
		for (int i = 0; i < n - j; i++) {
			divided_diff_table[i][j] = divided_diff_table[i + 1][j - 1] - divided_diff_table[i][j - 1];
		}
	}

	return divided_diff_table;
}

double f(double x) {
	return 3 * x * x * x * x * x * x - 2 * x * x * x * x + 7 * x * x * x - 4 * x + 1;
}