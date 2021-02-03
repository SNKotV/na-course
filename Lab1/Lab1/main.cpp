#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

double lagrange_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x);
double aitken_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x, double eps);

double point;
bool cmp(const std::pair<double, double> &a, const std::pair<double, double> &b);
void resort(std::vector<std::pair<double, double>> &key_value_table);

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
	for (auto &p : key_value_table) {
		std::cin >> p.first >> p.second;
	}
#else
	std::ifstream fin(filename);
	fin >> n;
	key_value_table.resize(n);
	for (auto &p : key_value_table) {
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

	std::cout << "Lagrange interpolation: f(" << x << ") = " << lagrange_interpolation(key_value_table, x) << '\n';

	double eps;

#ifdef _DEBUG
	std::cin >> eps;
#else
	std::cout << "Enter eps = ";
	std::cin >> eps;
#endif 

	std::cout << "Aitken interpolation: f(" << x << ") = " << aitken_interpolation(key_value_table, x, eps) << '\n';

#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif

	return 0;
}

double lagrange_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x) {
	double res = 0;
	
	int n = key_value_table.size();
	for (int i = 0; i < n; i++) {
		double num = 1, den = 1;
		for (int j = 0; j < n; j++) {
			if (i == j) continue;
			num *= x - key_value_table[j].first;
			den *= key_value_table[i].first - key_value_table[j].first;
		}
		res += key_value_table[i].second * num / den;
	}

	return res;
}

double aitken_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x, double eps) {
	std::vector<std::pair<double, double>> tmp_table = key_value_table;
	point = x;
	resort(tmp_table);
	
	int n = tmp_table.size();
	std::vector<std::vector<double>> table(n, std::vector<double>(n));

	for (int i = 0; i < n; i++) {
		table[i][0] = tmp_table[i].first;
		table[i][1] = tmp_table[i].second;
	}

	std::pair<double, int> mineps{1e9, -1};

	for (int j = 2; j < n; j++) {
		double num = table[j - 1][j - 1] * (x - table[0][0]) - table[0][j - 1] * (x - table[j - 1][0]);
		double den = table[j - 1][0] - table[0][0];
		table[0][j] = num / den;

		double cnteps = std::fabs(table[0][j] - table[0][j - 1]);
		if (cnteps - mineps.first < 1e-12) {
			mineps = std::make_pair(cnteps, j);
		}

		if (eps - mineps.first > 1e-12) {
			break;
		}

		for (int i = 1; i < n - j + 1; i++) {
			double num = table[i + j - 1][j - 1] * (x - table[i][0]) - table[i][j - 1] * (x - table[i + j - 1][0]);
			double den = table[i + j - 1][0] - table[i][0];
			table[i][j] = num / den;
		}
	}

	return table[0][mineps.second];
}

bool cmp(const std::pair<double, double> &a, const std::pair<double, double> &b) {
	return std::abs(a.first - point) < std::abs(b.first - point);
}

void resort(std::vector<std::pair<double, double>> &key_value_table) {
	std::vector<std::pair<double, double>> sorted_table = key_value_table;
	for (int i = 0; i < sorted_table.size(); i++) {
		sorted_table[i].second = i;
	}

	std::sort(sorted_table.begin(), sorted_table.end(), cmp);

	for (int i = 0; i < sorted_table.size(); i++) {
		sorted_table[i].second = key_value_table[sorted_table[i].second].second;
	}

	key_value_table = sorted_table;
}

