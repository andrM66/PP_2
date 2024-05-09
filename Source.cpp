#include <iostream>
#include <vector>
#include<fstream>
#include <random>
#include <time.h>
#include <omp.h>
using namespace std;
class SquareMatrix
{
	vector<vector<int>> _data;
	size_t _size;
public:
	friend ostream& operator<<(ostream& s, SquareMatrix mat);
	SquareMatrix(size_t size, bool null_flag)
	{
		_size = size;
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dist(-100, 100);
		for (size_t i = 0; i < size; ++i)
		{
			vector<int> tmp;
			for (size_t j = 0; j < size; ++j)
			{
				if (!null_flag)
				{
					tmp.push_back(dist(gen));
				}
				else
				{
					tmp.push_back(0);
				}
			}
			_data.push_back(tmp);
		}
	}
	SquareMatrix(const SquareMatrix& mat)
	{
		_size = mat._size;
		for (size_t i = 0; i < _size; ++i)
		{
			vector<int> tmp;
			for (size_t j = 0; j < _size; ++j)
			{
				tmp.push_back(mat._data[i][j]);
			}
			_data.push_back(tmp);
		}
	}
	SquareMatrix& operator=(const SquareMatrix& mat)
	{
		_data.resize(mat._size);
		_size = mat._size;
		if (_size == 0)
		{
			return *this;
		}
		for (size_t i = 0; i < _size; ++i)
		{
			for (size_t j = 0; j < _size; ++j)
			{
				_data[i][j] = mat._data[i][j];
			}
		}
		return *this;
	}
	SquareMatrix operator*(const SquareMatrix& mat) const
	{
		if (_size != mat._size)
		{
			throw(out_of_range("not equal sizes!"));
		}
		SquareMatrix result(_size, true);
		#pragma omp parallel num_threads(4)
		{
			#pragma omp for
			for (int i = 0; i < _size; ++i)
			{
				for (int j = 0; j < _size; ++j)
				{
					for (int k = 0; k < _size; ++k)
					{
						result._data[i][j] += _data[i][k] * mat._data[k][j];
					}
				}
			}
		}
		return result;
	}
	static vector<SquareMatrix> read_matrixes(size_t size, size_t number_of_mat, string path)
	{
		vector<SquareMatrix> result;
		ifstream fin;
		fin.open(path);
		if (!fin)
		{
			throw(logic_error("file does not exist"));
		}
		for (size_t k = 0; k < number_of_mat; ++k)
		{
			SquareMatrix tmp(size, true);
			for (size_t i = 0; i < tmp._size; ++i)
			{
				for (size_t j = 0; j < tmp._size; ++j)
				{
					fin >> tmp._data[i][j];
				}
			}
			result.push_back(tmp);
		}
		fin.close();
		return result;
	}
	static void write_matrixes(const vector<SquareMatrix>& arr, string path)
	{
		ofstream of;
		of.open(path);
		for (size_t k = 0; k < arr.size(); ++k)
		{
			for (size_t i = 0; i < arr[k]._size; ++i)
			{
				for (size_t j = 0; j < arr[k]._size; ++j)
				{
					of << arr[k]._data[i][j];
					if (j == arr[k]._size - 1)
					{
						of << "\n";
					}
					else
					{
						of << " ";
					}
				}
			}
		}
		of.close();
	}
	static vector<SquareMatrix> make_array(size_t size, size_t number)
	{
		vector<SquareMatrix> result;
		for (size_t i = 0; i < number; ++i)
		{
			SquareMatrix tmp(size, false);
			result.push_back(tmp);
		}
		return result;
	}
};
ostream& operator<<(ostream& s, SquareMatrix mat)
{
	for (size_t i = 0; i < mat._size; ++i)
	{
		for (size_t j = 0; j < mat._size; ++j)
		{
			s << mat._data[i][j]<<" ";
		}
		s << "\n";
	}
	return s;
}
void write_time_stat(vector<double> arr, string path)
{
	ofstream of;
	of.open(path);
	for (size_t i = 0; i < arr.size(); ++i)
	{
		of << arr[i] << " ";
	}
	of.close();
}
vector<size_t> A = { 100, 200, 400, 1000 };
size_t NUMBER = 100;
vector<string> PATH = { "matrix_list_100.txt", "matrix_list_200.txt", "matrix_list_400.txt", "matrix_list_1000.txt" };
vector<string> SAVE_PATH = { "result100.txt", "result200.txt", "result400.txt", "result1000.txt"};
vector<string> TIME_RESULT = { "time_res100.txt", "time_res200.txt", "time_res400.txt", "time_res1000.txt" };
int main()
{
	vector<vector<double>> time;
	for (size_t i = 0; i < A.size(); ++i)
	{
		vector<SquareMatrix> arr;
		arr = SquareMatrix::read_matrixes(A[i], NUMBER, PATH[i]);
		vector<SquareMatrix> result;
		time.push_back(vector<double> {});
		for (size_t j = 0; j < arr.size() - 1; ++j)
		{
			clock_t start = clock();
			result.push_back(arr[j] * arr[j + 1]);
			clock_t end = clock();
			double seconds = (double)(end - start) / CLOCKS_PER_SEC;
			time[i].push_back(seconds);
		}
		SquareMatrix::write_matrixes(result, SAVE_PATH[i]);
		/*if (i == 0)
		{
			cout << result[1];
			cout << endl << endl << endl;
			cout << endl << endl << endl;
			cout << result[10];
		}*/
		write_time_stat(time[i], TIME_RESULT[i]);
	}
	return 0;
}