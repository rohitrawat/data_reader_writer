/* 
 * File:   types.h
 * Author: rohit
 *
 * Created on September 5, 2015, 6:04 PM
 */

#ifndef _TYPES_H
#define	_TYPES_H

//#define USEVALARR
#define USEFLOAT

#include <vector>
using std::vector;
#include <valarray>
using std::valarray;

typedef int INT;

#ifdef USEFLOAT
typedef float REAL;
#else
typedef double REAL;
#endif

#ifdef USEVALARR
typedef valarray<REAL> Array;
typedef valarray<Array> Matrix;
typedef valarray<int> ArrayInt;
typedef valarray<ArrayInt> MatrixInt;
#else
typedef vector<REAL> Array;
typedef vector<Array> Matrix;
typedef vector<int> ArrayInt;
typedef vector<ArrayInt> MatrixInt;
#endif


#define EPSILON 1e-12

#endif	/* TYPES_H */
