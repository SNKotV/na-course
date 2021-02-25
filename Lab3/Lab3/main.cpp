#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>


double spline_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x, std::vector<std::vector<double>> &coefficients);
std::vector<std::vector<double>> compute_coefficients(std::vector<std::pair<double, double>> &key_value_table);
std::vector<double> solve_system_of_equations(std::vector<std::vector<double>> &system_matrix, std::vector<double> &vector);

void plot(std::vector<std::pair<double, double>> &key_value_table, std::vector<std::vector<double>> &coefficients);


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

	std::vector<std::vector<double>> coefficients = compute_coefficients(key_value_table);
	std::cout << "s(" << x << ") = " << spline_interpolation(key_value_table, x, coefficients) << '\n';
	plot(key_value_table, coefficients);


#ifndef _DEBUG
	std::cin.get();
	std::cin.get();
#endif

	return 0;
}

double spline_interpolation(std::vector<std::pair<double, double>> &key_value_table, double x, std::vector<std::vector<double>> &coefficients) {
	for (int i = 1; i < key_value_table.size(); i++) {
		if (x - key_value_table[i].first < 1e-9 && key_value_table[i - 1].first - x < 1e-9) {
			double a = coefficients[i][0];
			double b = coefficients[i][1];
			double c = coefficients[i][2];
			double d = coefficients[i][3];

			double diff = x - key_value_table[i].first;

			return a + b * diff + c / 2 * diff * diff + d / 6 * diff * diff * diff;
		}
	}
	return -1e9;
}

std::vector<std::vector<double>> compute_coefficients(std::vector<std::pair<double, double>> &key_value_table) {
	int n = key_value_table.size();
	
	std::vector<std::vector<double>> system_matrix(n, std::vector<double>(n));
	std::vector<double> vector(n);	

	system_matrix[0][0] = system_matrix[n - 1][n - 1] = 1;
	for (int i = 1; i < n - 1; i++) {
		double h_cnt = key_value_table[i].first - key_value_table[i - 1].first;
		double h_next = key_value_table[i + 1].first - key_value_table[i].first;
		
		system_matrix[i][i - 1] = h_cnt;
		system_matrix[i][i] = 2 * (h_cnt + h_next);
		system_matrix[i][i + 1] = h_next;

		double f_cnt = key_value_table[i].second - key_value_table[i - 1].second;
		double f_next = key_value_table[i + 1].second - key_value_table[i].second;

		vector[i] = 6 * (f_next / h_next - f_cnt / h_cnt);
	}

	std::vector<double> solution = solve_system_of_equations(system_matrix, vector);
	
	std::vector<std::vector<double>> result(n, std::vector<double>(4));
	
	for (int i = 1; i < n - 1; i++) {
		double h = key_value_table[i].first - key_value_table[i - 1].first;

		double c = solution[i];
		double a = key_value_table[i].second;
		double d = (c - solution[i - 1]) / 3 / h;
		double b = (a - key_value_table[i - 1].second) / h + (2 * c + solution[i - 1]) / 3 * h;

		result[i][0] = a;
		result[i][1] = b;
		result[i][2] = c;
		result[i][3] = d;
	}

	double h = key_value_table[n - 1].first - key_value_table[n - 2].first;
	result[n - 1][0] = key_value_table[n - 1].second;
	result[n - 1][1] = (key_value_table[n - 1].second - key_value_table[n - 2].second) / h + solution[n - 2] / 3 * h;
	result[n - 1][2] = 0;
	result[n - 1][3] = -solution[n - 2] / 3 / h;

	return result;
}

std::vector<double> solve_system_of_equations(std::vector<std::vector<double>> &system_matrix, std::vector<double> &vector) {
	int n = vector.size();

	std::vector<double> alpha(n);
	std::vector<double> beta(n);

	alpha[0] = system_matrix[0][1] / system_matrix[0][0];
	beta[0] = vector[0] / system_matrix[0][0];
	for (int i = 1; i < n - 1; i++)	{
		alpha[i] = system_matrix[i][i + 1] / (system_matrix[i][i] - system_matrix[i][i - 1] * alpha[i - 1]);
		beta[i] = (vector[i] - system_matrix[i][i - 1] * beta[i - 1]) / (system_matrix[i][i] - system_matrix[i][i - 1] * alpha[i - 1]);
	}
	beta[n - 1] = (vector[n - 1] - system_matrix[n - 1][n - 1 - 1] * beta[n - 1 - 1]) / (system_matrix[n - 1][n - 1] - system_matrix[n - 1][n - 1 - 1] * alpha[n - 1 - 1]);

	std::vector<double> solution(n);

	solution[n - 1] = beta[n - 1];
	for (int i = n - 2; i >= 0; i--) {
		solution[i] = beta[i] - alpha[i] * solution[i + 1];
	}

	return solution;
}


void plot(std::vector<std::pair<double, double>> &key_value_table, std::vector<std::vector<double>> &coefficients) {
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
	for (double x = 0; std::fabs(1.01 - x) > 1e-9; x += 0.01) {
		fout << x << ' ';
	}
	fout << '\n';
	for (double x = 0; std::fabs(1.01 - x) > 1e-9; x += 0.01) {
		fout << spline_interpolation(key_value_table, x, coefficients) << ' ';
	}
	
	fout.close();

	system("python plotter.py");
	std::string delete_file = "del " + tmp_filename;
	system(delete_file.c_str());
}