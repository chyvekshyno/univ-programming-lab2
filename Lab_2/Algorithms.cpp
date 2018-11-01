#include "Algorithms.h"
#include <cmath>
#include <iostream>
#include <string>
#include <ctime>
#include <cmath>

CAlgorithms::CAlgorithms()
{
	matr = new CMatrix;
}


CAlgorithms::CAlgorithms(const CMatrix & _matr)
{
	matr = new CMatrix(_matr);
}

void CAlgorithms::downlMatr(const CMatrix & _matr)
{
	matr = new CMatrix(_matr);
}

void CAlgorithms::downlMatr(const int & rows, const int & cols)
{
	matr = new CMatrix(rows, cols);
	std::cin >> *matr;
}

CAlgorithms::~CAlgorithms()
{
	if (matr != nullptr)
	{
		delete matr;
		matr = nullptr;
	}
}


CMatrix CAlgorithms::GaussianElimination()
{
	directElimination();
	return reverseSubstitution();
}

bool CAlgorithms::nonZeroInCol(const int & col, std::pair<unsigned int, double> & leadEl) const
{

	for (size_t row = col; row < matr->getRows(); row++)
	{
		if ((*matr)[row][col] != 0)
		{
			leadEl = std::make_pair(row, (*matr)[row][col]);
			return true;
		}
	}
	return false;
}

unsigned int CAlgorithms::directElimination()
{
	unsigned int  index = 0
		, next = index
		, emptLines = 0;

	std::pair<unsigned int, double> leadEl;

	for (; index < matr->getRows(); ++index)
	{
		if (!empty(index))
		{
			if (nonZeroInCol(index, leadEl))
			{
				swap(leadEl.first, next);
				normalize(next, leadEl.second);
				subtraction(next);
				++next;
			}
			else
			{
				throw std::logic_error("System of equations has an infinite number of solutions.");
			}
		}
		else
		{
			if ((*matr)[index][matr->getCols()] != 0)
			{
				throw std::logic_error("System of equations has not solution.");
			}
			else if (matr->getRows() + emptLines < matr->getCols())
			{
				throw std::logic_error("System of equations has an infinite number of solutions: SoE has " + std::to_string(emptLines) + " or more linearly dependent equations");
			}
			else
			{
				++emptLines;
			}
		}
	}
}
std::pair<unsigned const int, double> CAlgorithms::findMax(const int & col) const
{
	size_t row = col;
	unsigned int res = row;
	double maximum = (*matr)[row++][col],
		buffer = 0;
		
	

	for (; row < matr->getRows(); ++row)
	{
		buffer = (*matr)[row][col];
		if (abs(maximum) < abs(buffer))	
		{
			maximum = buffer;
			res = row;
		}
	}
	return std::make_pair(res, maximum);
}	

void CAlgorithms::swap(const int & row1, const int & row2)
{
	if (row1 != row2)
	{
		double temp = 0;

		for (size_t col = 0; col < matr->getCols(); ++col)
		{
			temp = (*matr)[row1][col];
			(*matr)[row1][col] = (*matr)[row2][col];
			(*matr)[row2][col] = temp;
		}
	}
}

void CAlgorithms::normalize(unsigned const int & ROW, const double & elem)
{
	for (size_t col = 0; col < matr->getCols(); col++)
	{
		(*matr)[ROW][col] /= elem;
	}
}

void CAlgorithms::subtraction(unsigned int ROW)
{
	if (ROW < matr->getRows() - 1)
	{
		size_t col = ROW;
		for (size_t row = ROW + 1; row < matr->getRows(); ++row)
		{
			col = ROW;
			double koef = (*matr)[row][col];
			for (; col < matr->getCols(); ++col)
			{
				(*matr)[row][col] -= (*matr)[ROW][col] * koef;
			}
		}
	}
}

bool CAlgorithms::empty(const unsigned int & row) const
{
	bool res = true;
	for (int col = 0; col < matr->getCols(); ++col)
	{
		if ((*matr)[row][col] != 0)
		{
			if (col == matr->getCols())
				break;

			res = false;
			break;
		}
	}
	return res;
}

CMatrix & CAlgorithms::reverseSubstitution()
{
	CMatrix *res = new CMatrix(1, matr->getCols() - 1);
	double Ai_lead
		, koef;

	int rowLead = matr->getRows() - 1;

	for (int col = matr->getCols() - 2; col >= 0;--col)
	{ 
		Ai_lead = (*matr)[rowLead][matr->getCols() - 1];
		(*res)[0][matr->getCols() - 2  - col] = (Ai_lead);
		for (int row = rowLead; row > 0; --row)
		{
			koef = (*matr)[row - 1][col];
			(*matr)[row - 1][matr->getCols() - 1] -= koef * Ai_lead;
			(*matr)[row - 1][col] -= koef * (*matr)[rowLead][col];
		}
		--rowLead;
	}
	
	return *res;
}


// Jacobi

std::vector<double>  CAlgorithms::JakobiMethod()
{
	std::cout << *matr << "\n";
	CMatrix * turnMatr = nullptr;
	turnMatr = new CMatrix(*matr);
	std::vector<double> res;
	double precision = 0.005; // than we will changed that and precision will become parameter from user 
	size_t maxRow = matr-> getRows(), maxCol = matr ->getCols();
	double max = 0.0;
	double fault = 0.0; 
	calculateJakobiFault(fault);
	int i = 1;
	while (fault > precision) 
	{
		max = 0.0;
		// Searching max element.
		std::cout << "Jakobi iteration:" << i << "\n";
		JakobiFindMax(max, maxRow, maxCol);
		if (max < precision) break; // need to end while if Jakobi Method finished? but fault still bigger than precision
		std::cout << "max = " << max;
		prepareTurnMatr(maxRow, maxCol, *turnMatr);
		std::cout << "\n" << *turnMatr << "\n";
		JakobiTurnMatrix(*turnMatr);
		i++;
		std::cout << "\n";
		std::cout << *matr;
		std::cout << "\n";
		cleanZeroElements(precision);
		calculateJakobiFault(fault);
		if (i > 40) break; // crutch // unstopable cycle, check fault, check findMax
	}
	JakobiSolution(res);
	return res;
}

// check architecture
void CAlgorithms::JakobiFindMax( double &max, size_t &maxRow, size_t &maxCol)
{

	for (int row = 0; row < matr->getCols(); row++)
	{
		for (int col = 0; col < matr->getCols(); col++)
		{
			if (row != col)
			{
				if ((*matr)[row][col] > 0 && (*matr)[row][col] > max)
				{
					max = (*matr)[row][col];
					maxRow = row;
					maxCol = col;
				}
				else if ((*matr)[row][col] < 0 && -(*matr)[row][col] > max)
				{
					max = -(*matr)[row][col];
					maxRow = row;
					maxCol = col;
				}
			}
		}
	}
}

// change vector to matrix 1*n
void CAlgorithms::JakobiSolution(std::vector<double> &res)
{
	for (int  row = 0; row < matr->getCols(); row++)
	{
		
			res.push_back((*matr)[row][row]);
		
	}
}

void CAlgorithms::makeIdentityMatrix(CMatrix & turnMatr)
{
	for (int row = 0; row < matr->getCols(); row++)
	{
		for (int col = 0; col < matr->getCols(); col++)
		{
			turnMatr[row][col] = 0;
		}
		turnMatr[row][row] = 1;
	}
}

void CAlgorithms::makeZeroMatrix(CMatrix & other)
{
	for (int row = 0; row < other.getCols(); row++)
	{
		for (int col = 0; col < other.getCols(); col++)
		{
			(other)[row][col] = 0.0;
		}
	}
}

void CAlgorithms::prepareTurnMatr(size_t maxRow, size_t maxCol, CMatrix &turnMatr)
{
	double fi;
	makeIdentityMatrix(turnMatr);

	if (((*matr)[maxRow][maxRow] == (*matr)[maxCol][maxCol]) && ( (*matr)[maxCol][maxCol] == (*matr)[maxRow][maxCol]) )
	{
		turnMatr[maxRow][maxRow] = turnMatr[maxCol][maxCol] = 
		turnMatr[maxCol][maxRow] = sqrt(2.0) / 2.0;
			turnMatr[maxRow][maxCol] = -sqrt(2.0) / 2.0;
	}
	else
	{
		fi = 0.5 * atan((2.0 * (*matr)[maxRow][maxCol]) /
			((*matr)[maxRow][maxRow] - (*matr)[maxCol][maxCol]));
		turnMatr[maxRow][maxRow] = turnMatr[maxCol][maxCol] = cos(fi);
		turnMatr[maxRow][maxCol] = -sin(fi);
		turnMatr[maxCol][maxRow] = sin(fi);
	}
}

void CAlgorithms::JakobiTurnMatrix( CMatrix &turnMatr)
{
	CMatrix * temp = nullptr;
	temp = new CMatrix(*matr);

	*temp = (~turnMatr) * (*matr);
	
	*temp = (*temp) * (turnMatr);
	
	double fault = 0.0;
	*matr = *temp;
	if (temp!= nullptr) delete temp;
}


void CAlgorithms::calculateJakobiFault(double & fault)
{
	for (int row = 0; row < matr->getCols(); row++)
	{
		for (int col = row + 1; col < matr->getCols(); col++)
		{
			fault = fault + (*matr)[row][col] * (*matr)[row][col];
		}
	}
	fault = sqrt(fault);
}

void CAlgorithms::cleanZeroElements( double precision)
{
	for (int row = 0; row < matr->getCols(); row++)
	{
		for (int col = 0; col < matr->getCols(); col++)
		{
			if ((*matr)[row][col] < precision) (*matr)[row][col] = 0;
		}
	}

}

// Linear regression
double CAlgorithms::dispRow(const double && row) const
{
	double res = 0;

	for (size_t col = 0; col < matr->getCols(); col++)
	{
		res += (*matr)[row][col];
	}

	res /= matr->getCols();

	return res;
}

double CAlgorithms::dispRows(const double && row1, const double && row2) const
{
	double res = 0;

	for (size_t col = 0; col < matr->getCols(); col++)
	{
		res += (*matr)[row1][col] * (*matr)[row1][col];
	}

	res /= matr->getCols();

	return res;
}


CMatrix CAlgorithms::LinRegression() const
{
	double dispY = dispRow(1)
		, dispX = dispRow(2)
		, dispXY = dispRows(1, 2)
		, dispXX = dispRows(1, 1)

		, argA = (dispX * dispY - dispXY) / (dispX * dispX - dispXX)
		, argB = (dispXY - argA * dispXX) / dispX;

	CMatrix res(1, 2);
	res[0][0] = argA;
	res[0][1] = argB;

	return res;
}

CMatrix & CAlgorithms::getFreeMembMatr()
{
	CMatrix * res = nullptr;
	res = new CMatrix(matr->getCols(), 1, 0);

	//	first elem
	for (size_t row = 0; row < matr->getRows(); ++row)
	{
		(*res)[0][0] += (*matr)[row][0];

	}

	//	 from 1 to last elems
	for (size_t col = 1, row = 1; col < matr->getCols(); ++col, ++row)
	{
		for (size_t rowAdd = 0; rowAdd < matr->getRows(); ++rowAdd)
		{
			(*res)[row][0] += (*matr)[rowAdd][0] * (*matr)[rowAdd][col];
		}
	}

	return *res;
}

CMatrix & CAlgorithms::getSumMatr()
{
	CMatrix * res = nullptr;
	res = new CMatrix(matr->getCols(), matr->getCols(), 0);

	// [0][0] elem
	*res[0][0] = matr->getRows();

	double summ = 0;

	// 0 row and 0 col
	for (int col = 1; col < matr->getCols(); ++col)
	{
		for (int row = 0; row < matr->getRows(); ++row)
		{
			summ += (*matr)[row][col];
		}

		(*res)[col][0] = (*res)[0][col] = summ;
		summ = 0;
	}

	// from 1 to N
	for (int row = 1; row < res->getRows(); ++row)
	{
		for (int col = 1; col < res->getRows(); ++col)
		{
			//	calculate number of element of result matrix [row][col]
			for (int  dot = 0; dot < matr->getRows(); ++dot)
			{
				(*res)[row][col] += (*matr)[dot][row] * (*matr)[dot][col];
				
			}
		}
	}
	return *res;
}


CMatrix & CAlgorithms::randPoints(const CMatrix & koefs)
{
	srand(time(0));
	CMatrix * res = new CMatrix((20 + rand() % 10), koefs.getCols(), 0);
	//double num;
	//for (unsigned int row = 0; row < res->getRows(); ++row)
	//{
	//	for (unsigned int col = 0; col < res->getCols(); ++col)
	//	{
	//		num = (double)(1 + rand() % 5) / 25;
	//		(*res)[row][col] = (koefs[0][col] - ((1 + rand() % 5) * num) + (rand() % 10) * num);
	//	}
	//}

	for (int row = 0; row < res->getRows(); ++row)
	{
		for (int col = 1; col < res->getCols(); ++col)
		{
			(*res)[row][col] = (1 + rand() % 1000)*pow(-1, rand());
			(*res)[row][0] += (*res)[row][col] * koefs[0][col];

		}
		(*res)[row][0] += koefs[0][0];
		(*res)[row][0] += ((rand() % 5))*pow(-1, rand());

	}

	return *res;
}

CMatrix & CAlgorithms::LinRegression()
{
	CMatrix * res = new CMatrix;
	CMatrix * koefs = new CMatrix(1, 3, 0);

	std::cin >> *koefs;
	*matr = randPoints(*koefs);
	std::cout << std::endl << *matr << std::endl;

	CMatrix *A = new CMatrix;
	CMatrix *B = new CMatrix;

	*A = getSumMatr();
	*B = getFreeMembMatr();

	std::cout << *A << std::endl;
	std::cout << *B << std::endl;

	*matr = (*A) | (*B);
	std::cout << *matr;
	*res = GaussianElimination();
	return *res;
}

// kachmage
//double CAlgorithms::KachmageMethod(CMatrix & b)
//{
	//double precision = 0.00000000001;
	//CMatrix x(*this, 0);
	//CMatrix x1(1, N_size, 0);
	//CMatrix sub(1, N_size, 1);
	//int j = 0;
	//while (sub.norma() > precision)
	//{
	//	CMatrix ai(*this, j);
	//	double temp = ((b.M[0][j] - ai.skal_dob(x)) / (ai.norma()*ai.norma()));
	//	ai = ai * temp;
	//	x1 = x + ai;
	//	sub = x1 - x;
	//	//  cout << sub.norma()<<endl;
	//	x = x1;
	//	if (j < N_size - 1)
	//		j++;
	//	else j = 0;
	//}
	//return x;
//}