/* 
 * File:   types.h
 * Author: rohit
 *
 * Created on September 5, 2015, 6:04 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#include <vector>

using namespace std;

typedef int INT;
typedef double REAL;
typedef vector<REAL> Array;
typedef vector<Array> Matrix;
typedef vector<int> ArrayInt;
typedef vector<ArrayInt> MatrixInt;

#define EPSILON 1e-12

#endif	/* TYPES_H */
