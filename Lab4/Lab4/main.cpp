#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

struct quadratic_function {
	std::vector<double> a;
	quadratic_function() { a.resize(3);  }
	void fit(std::vector<std::pair<double, double>> &key_value_table);
	double evaluate(double x);
};

std::vector<double> solve_system_of_equations(std::vector<std::vector<double>> system_matrix, std::vector<double> vector);

void plot(std::vector<std::pair<double, double>> &key_value_table);


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

	plot(key_value_table);

#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif

	return 0;
}

void quadratic_function::fit(std::vector<std::pair<double, double>> &key_value_table) {
	int n = key_value_table.size();
	
	double sum_x , sum_x2 , sum_x3 , sum_x4 , sum_y , sum_xy , sum_x2y;
	sum_x = sum_x2 = sum_x3 = sum_x4 = sum_y = sum_xy = sum_x2y = 0;
	
	for (int i = 0; i < n; i++) {
		double x = key_value_table[i].first;
		double y = key_value_table[i].second;

		sum_x += x;
		sum_x2 += x * x;
		sum_x3 += x * x * x;
		sum_x4 += x * x * x * x;
		sum_y += y;
		sum_xy += x * y;
		sum_x2y += x * x * y;
	}

	std::vector<std::vector<double>> system_matrix{ {sum_x4, sum_x3, sum_x2},
													{sum_x3, sum_x2, sum_x},
													{sum_x2, sum_x, 1.0 * n} };
	std::vector<double> vector{ sum_x2y, sum_xy, sum_y };

	a = solve_system_of_equations(system_matrix, vector);

	std::vector<double> tmp(3);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tmp[i] += system_matrix[i][j] * a[j];
		}
	}

	for (auto &i : vector) std::cout << i << ' ';
	std::cout << '\n';
	for (auto &i : tmp) std::cout << i << ' ';
}

double quadratic_function::evaluate(double x) {
	return a[0] * x * x + a[1] * x + a[2];
}

std::vector<double> solve_system_of_equations(std::vector<std::vector<double>> system_matrix, std::vector<double> vector) {
	int n = vector.size();

	const double eps = 1e-12;
	for (int i = 0; i < n; i++) {
		if (fabs(system_matrix[i][i]) < eps) {
			for (int j = i + 1; j < n; j++)	{
				if (fabs(system_matrix[j][i]) > eps) {
					std::vector<double> tmp = system_matrix[i];
					system_matrix[i] = system_matrix[j];
					system_matrix[j] = tmp;

					double temp = vector[i];
					vector[i] = vector[j];
					vector[j] = temp;
					break;
				}
			}
		}

		if (fabs(system_matrix[i][i] - 1) > eps) {
			for (int j = i + 1; j < n; j++) {
				system_matrix[i][j] /= system_matrix[i][i];
			}
			vector[i] /= system_matrix[i][i];
			system_matrix[i][i] = 1;
		}

		for (int j = 0; j < i; j++)	{
			for (int k = i + 1; k < n; k++) {
				system_matrix[j][k] -= system_matrix[i][k] * system_matrix[j][i];
			}
			vector[j] -= vector[i] * system_matrix[j][i];
			system_matrix[j][i] = 0;
		}

		for (int j = i + 1; j < n; j++)		{
			for (int k = i + 1; k < n; k++) {
				system_matrix[j][k] -= system_matrix[i][k] * system_matrix[j][i];
			}
			vector[j] -= vector[i] * system_matrix[j][i];
			system_matrix[j][i] = 0;
		}
	}

	std::vector<double> solution = vector;
	return solution;
}


void plot(std::vector<std::pair<double, double>> &key_value_table) {
	std::string tmp_filename = "key_value_pairs.txt";
	std::ofstream fout(tmp_filename);
	fout.setf(std::ios_base::fixed); fout.precision(3);
	
	for (auto& p : key_value_table) {
		fout << p.first << ' ';
	}
	fout << '\n';
	for (auto& p : key_value_table) {
		fout << p.second << ' ';
	}
	fout << '\n';

	quadratic_function f;
	f.fit(key_value_table);

	double h = 0.01;
	for (double x = 0; std::fabs(key_value_table.back().first + h - x) > 1e-9; x += h) {
		fout << x << ' ';
	}
	fout << '\n';
	for (double x = 0; std::fabs(key_value_table.back().first + h - x) > 1e-9; x += h) {
		fout << f.evaluate(x) << ' ';
	}
	
	fout.close();

	system("python plotter.py");
	std::string delete_file = "del " + tmp_filename;
	system(delete_file.c_str());
}