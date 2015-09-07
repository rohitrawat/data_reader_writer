#include "types.h"
#include <algorithm>

#define EPSILON 1e-12

void AddArray(Array &a, Array &b, Array &c) {
    //assert(a.size() == b.size());
    int N = min(a.size(), b.size());
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

void AddArray(Array &a, Array &b) {
    //assert(a.size() == b.size());
    int N = min(a.size(), b.size());
    for (int i = 0; i < N; i++) {
        a[i] = a[i] + b[i];
    }
}

void SquareArray(Array &a) {
    for (size_t i = 0; i < a.size(); i++) {
        a[i] = a[i] * a[i];
    }
}

void SubtractArray(const Array &a, const Array &b, Array &c) {
    //assert(a.size() == b.size());
    int N = min(a.size(), b.size());
    for (int i = 0; i < N; i++) {
        c[i] = a[i] - b[i];
    }
}

void SubtractArray(Array &a, const Array &b) {
    //assert(a.size() == b.size());
    int N = min(a.size(), b.size());
    for (int i = 0; i < N; i++) {
        a[i] = a[i] - b[i];
    }
}

void ElementDivide(const Array &a, const Array &b, Array &c) {
    //assert(a.size() == b.size());
    int N = min(a.size(), b.size());
    for (int i = 0; i < N; i++) {
        if(b[i]>EPSILON) {
            c[i] = a[i] / b[i];
        } else {
            c[i] = 1.0;
        }
    }
}

Array& zeroArray(int length) {
    static Array zeros;
    zeros.resize(length);
    return zeros;
}

Array& unitArray(int length) {
    static Array ones;
    ones.resize(length, 1.0);
    return ones;
}

void zero_array(Array &arr) {
    for (unsigned int i = 0; i < arr.size(); i++) {
        arr[i] = 0;
    }
}

void zero_matrix(Matrix &mat) {
    for (unsigned int i = 0; i < mat.size(); i++) {
        for (unsigned int j = 0; j < mat[i].size(); j++) {
            mat[i][j] = 0;
        }
    }
}

int findMinimaIndex(const Array &arr) {
    double min = arr[0];
    //cout<<"find_minima: "<<clusterPruningError[0]<<", ";
    int min_index = 0;
    for (size_t i = 1; i < arr.size(); i++) {
        //cout<<clusterPruningError[i]<<", ";
        if (arr[i] < min) {
            min = arr[i];
            min_index = i;
        }
    }
    return min_index;
}

int median(vector<int> v) {
    int m = -1;
    int mid = v.size()/2;
    //cout<<mid<<endl;
    
    std::sort(v.begin(), v.end());
    
    return v[mid];
}

