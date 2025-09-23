/* 
 * File:   Matrix.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 29 листопада 2010, 15:29
 */

#include "Matrix.h"
#include "MatrixSolve.h"

#include <vector>
#include <sstream>
#include <iostream>

#include <algorithm>
#include <limits>
#include <cmath>

#include "Eigen/Eigen"

Matrix::Matrix() : rows_(0), columns_(0)
{
}

Matrix::Matrix(Eigen::MatrixXd m) : rows_(m.rows()), columns_(m.cols()),
                                    m_(m)
{
}

Matrix::Matrix(int rowsize, int columnsize) : rows_(rowsize), columns_(columnsize),
                                              m_(rows_, columns_)
{
}

Matrix::Matrix(std::vector<std::vector<double>> M)
{
    rows_ = M.size();
    if (rows_ )
    {
        columns_ = M[0].size();
        for (int i=1;i<rows_;i++)
            columns_ = std::min(columns_, (int)M[i].size());
     
        m_ = Eigen::MatrixXd(rows_, columns_);
        for (int i=0;i<rows_;i++)
            for (int j=0;j<columns_;j++)
                at(i, j) = M[i][j];
                
    }
    else
        columns_ = 0;
}

Matrix::~Matrix()
{
}

 bool Matrix::isNull()
 {
    return !rows_ || !columns_;
 }
 
 void Matrix::fillWithZero()
 {
    m_ = Eigen::MatrixXd::Zero(rows_, columns_); 
 }

double& Matrix::at(int row, int column)
{
    return m_(row, column);
    //return m_[row][column];
}

int Matrix::rows()
{
    return rows_;
}

int Matrix::columns()
{
    return columns_;
}

Matrix Matrix::operator+(const Matrix& other) const
{
    if (rows_!=other.rows_ || columns_!=other.columns_)
        return Matrix(0,0);
    
    return Matrix(this->m_ + other.m_);
}

Matrix Matrix::operator-(const Matrix& other) const
{
    if (rows_!=other.rows_ || columns_!=other.columns_)
        return Matrix(0,0);
    
    return Matrix(this->m_ - other.m_);
}

Matrix Matrix::operator*(const double& Const) const
{
    return Matrix(this->m_ * Const);
}

Matrix Matrix::operator*(const Matrix& other) const
{
    if (columns_!=other.rows_)
        return Matrix(0,0);
    
    return Matrix(this->m_ * other.m_);
}

Matrix Matrix::operator-() const
{
    return Matrix(-this->m_);
}

bool Matrix::operator==(const Matrix& other) const
{
    return !(operator!=(other));
}

bool Matrix::operator!=(const Matrix& other) const
{
    return this->m_ != other.m_;
}

Matrix Matrix::Transpone(Matrix srcmatrix)
{
    return Matrix(srcmatrix.m_.transpose());
}

Matrix Matrix::Inverse(Matrix srcmatrix)
{
    return Matrix(srcmatrix.m_.inverse());

}

long double Matrix::D()
{
    return m_.determinant();
}

Matrix Matrix::transpone()
{
    return Transpone(*this);
}

Matrix Matrix::inverse()
{
    return Inverse(*this);
}

bool Matrix::has_nan()
{
    for (int i=0;i<rows_;i++)
        for (int j=0;j<columns_;j++)
            if (std::isnan(at(i, j)))
                    return true;
    return false;
}

Matrix Matrix::elementMul(const Matrix& other) const
{
    if (columns_!=other.columns_ || rows_ != other.rows_)
        return Matrix(0,0);
    
    return Matrix(m_.cwiseProduct(other.m_) );
}

Matrix Matrix::elementDiv(const Matrix& other) const
{
    if (columns_!=other.columns_ || rows_ != other.rows_)
        return Matrix(0,0);
    
    return Matrix(m_.cwiseQuotient(other.m_) );
}
