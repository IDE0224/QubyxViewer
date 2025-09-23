/* 
 * File:   Matrix.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 29 листопада 2010, 15:29
 */

#ifndef MATRIX_H
#define	MATRIX_H

#include <vector>
#include <map>
#include <string>

#include "Eigen/Dense"

class Matrix
{
    int rows_, columns_;
    Eigen::MatrixXd m_;

    Matrix(Eigen::MatrixXd m);
    
    std::map<std::string, long double> minorsmap_;
public:
    Matrix();
    Matrix(int rowsize, int columnsize);
    Matrix(std::vector<std::vector<double>> M);
    virtual ~Matrix();
    
    bool isNull();
    void fillWithZero();

    double& at(int row, int column);
    int rows();
    int columns();
    long double D();

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const double& Const) const;
    
    Matrix operator-() const;
    
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;
    
    /**
     * element-wise multiplication
     * @param other
     * @return 
     */
    Matrix elementMul(const Matrix& other) const;
    
    /**
     * element-wise division
     * @param other
     * @return 
     */
    Matrix elementDiv(const Matrix& other) const;
    
    Matrix transpone();
    Matrix inverse();

    /**
     * @brief has_nan check if any element of matrix is nan
     * @return
     */
    bool has_nan();
    
    static Matrix Transpone(Matrix srcmatrix);
    static Matrix Inverse(Matrix srcmatrix);
    
    template<typename Type>
    static Matrix RowMatrix(std::vector<Type> vect)
    {
        Matrix res(1, vect.size());
        for (int i=0;i<vect.size();i++)
            res.at(0, i) = static_cast<double>(vect[i]);
        return res;
    }
    template<typename Type>
    static Matrix ColumnMatrix(std::vector<Type> vect)
    {
        Matrix res(vect.size(), 1);
        for (int i=0;i<vect.size();i++)
            res.at(i, 0) = static_cast<double>(vect[i]);
        return res;
    }

    template<typename Type>
    static Matrix RowMatrix(std::initializer_list<Type> vect)
    {
        return RowMatrix(std::vector<Type>(vect));
    }

    template<typename Type>    
    static Matrix ColumnMatrix(std::initializer_list<Type> vect)
    {
        return ColumnMatrix(std::vector<Type>(vect));
    }
    
    friend class MatrixSolve;
};

#endif	/* MATRIX_H */

