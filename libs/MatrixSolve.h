/* 
 * File:   MatrixSolve.h
 * Author: QUBYX Software Technologies LTD HK
 *
 * Created on 27 липня 2011, 11:25
 */

#ifndef MATRIXSOLVE_H
#define	MATRIXSOLVE_H

#include "Matrix.h"

class MatrixSolve 
{
    MatrixSolve();
public:
    virtual ~MatrixSolve();
    
    /**
     * Solve matrix equaltion AX = B
     * @param A - symetric matrix NxN
     * @param B - matrix Nx1
     * @return solution of AX = B equation
     */
    static Matrix LU_Decomposition(Matrix A, Matrix B);
    

    /**
     * Solve matrix equaltion AX = B
     * @param A - symetric matrix NxN
     * @param B - matrix Nx1
     * @return solution of AX = B equation
     */
    static Matrix Symetric_Holetsky(Matrix A, Matrix B);
    
    /**
     * Make Singular Value Decomposition of Matrix. make A=UxSx(V^T)
     * @param A input matrix m*n, where m>=n
     * @param U output matrix
     * @param S output matrix
     * @param V output matrix
     * @return is successful
     */
    static bool SingularValueDecomposition(Matrix &A, Matrix &U, Matrix &S, Matrix &V);

    /**
     * @brief HouseHolderQRSolve Solve matrix equaltion AX = y
     * @param A
     * @param y
     * @return solution of AX = y equation
     */
    static Matrix HouseHolderQRSolve(Matrix A, Matrix y);
};

#endif	/* MATRIXSOLVE_H */

