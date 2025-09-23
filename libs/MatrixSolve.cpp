/* 
 * File:   MatrixSolve.cpp
 * Author: QUBYX Software Technologies LTD HK
 * 
 * Created on 27 липня 2011, 11:25
 */

#include "MatrixSolve.h"

#include <cmath>
#include <vector>

//#include "svd.h"
#include "Eigen/SVD"
#include "Eigen/Dense"

#include <iostream>

MatrixSolve::MatrixSolve()
{
}

MatrixSolve::~MatrixSolve()
{
}

Matrix MatrixSolve::LU_Decomposition(Matrix A, Matrix B)
{
    if (!A.rows() || A.columns()!=A.rows() || A.rows() != B.rows())
        return Matrix();
    
    if (B.rows()!=A.rows() || B.columns()!=1)
        return Matrix();
    
    int n = B.rows();

    Eigen::VectorXd x = A.m_.lu().solve(B.m_);
    
    Matrix X(n, 1);
    for (int i=0;i<n;i++)
        X.at(i, 0) = x[i];
    
    return X;
}

Matrix MatrixSolve::Symetric_Holetsky(Matrix A, Matrix B)
{
    if (!A.rows() || A.columns()!=A.rows() || A.rows() != B.rows())
        return Matrix();
    
    if (B.rows()!=A.rows() || B.columns()!=1)
        return Matrix();
    
    if (fabsl(A.D())<1e-20)
        return Matrix();
    
    
   
    Matrix t(A.rows(), A.columns());
    long double s;
    int n = A.columns();
    for (int j=0;j<n;++j)
    {
        s = 0;
        for (int k=0;k<j;++k)
            s += t.at(j,k)*t.at(j,k);
        t.at(j,j) = sqrtl(A.at(j,j)-s);
        for (int i=j+1;i<n;++i)
        {
            s = 0;
            for (int k=0;k<j;++k)
                s += t.at(i,k)*t.at(j,k);
            t.at(i,j) = (A.at(i,j)-s)/t.at(j,j);
        }
    }
    
    std::vector<long double> y(B.rows());
    for (int i=0;i<n;++i)
    {
        s = 0;
        for (int k=0;k<i;++k)
            s += t.at(i,k)*y[k];
        y[i] = (B.at(i,0)-s)/t.at(i,i);
    }
            

    Matrix X(n, 1);
    
    for (int k=n-1;k>=0;--k)
    {
        s = 0;
        for (int j=k+1;j<n;++j)
            s+= t.at(j,k)*X.at(j,0);
        X.at(k,0)=(y[k]-s)/t.at(k,k);
    }
    
    return X;
}

bool MatrixSolve::SingularValueDecomposition(Matrix &A, Matrix &U, Matrix &S, Matrix &V)
{
    if (A.rows()<A.columns())
        return false;
    
    
    auto svd = A.m_.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
    
    U = Matrix(svd.matrixU());
    V = Matrix(svd.matrixV());
    Matrix S1 = Matrix(svd.singularValues());
    
    S = Matrix(A.rows(), A.columns());
    
    S.fillWithZero();
    for (int i=0;i<S1.rows();++i)
        S.at(i,i) = S1.at(i,0);
    
    
    
    return true;
}

Matrix MatrixSolve::HouseHolderQRSolve(Matrix A, Matrix y)
{
    Eigen::HouseholderQR<Eigen::MatrixXd> qr(A.m_);
    return Matrix(qr.solve(y.m_));
}
