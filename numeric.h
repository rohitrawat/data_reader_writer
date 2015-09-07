/* 
 * File:   numeric.h
 * Author: rohit
 *
 * Created on September 6, 2015, 8:56 AM
 */

#ifndef NUMERIC_H
#define	NUMERIC_H

void AddArray(const Array &a, const Array &b, Array &c);
void AddArray(Array &a, const Array &b);
void SquareArray(Array &a);
void SubtractArray(const Array &a, const Array &b, Array &c);
void SubtractArray(Array &a, const Array &b);
void ElementDivide(const Array &a, const Array &b, Array &c);
void print(const Array &arr);
void print(const Matrix &arr);
Array& zeroArray(int length);
Array& unitArray(int length);
void zero_array(Array &arr);
void zero_matrix(Matrix &mat);
int findMinimaIndex(const Array &arr);
int median(vector<int> v);


#endif	/* NUMERIC_H */

