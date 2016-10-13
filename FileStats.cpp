#include "file_io.h"

FileStats::FileStats(const File &file) {
    CONSTRUCTOR_MSG("FileStats");
    N = file.getN();
    M = file.getM();
    isClassification = file.getIsClassification();
    inputMeans.resize(N, 0.0); // note: resize automatically sets data to zeros
    inputStd.resize(N, 0.0);    // must be set to 0  as it is accumulated
    originalInputMeans.resize(N); // this will store the files input mean and std which was used to normalize it
    originalInputStd.resize(N, 1.0);
    distanceMeasure.resize(N, 1.0);
    Nv = 0;
}

void FileStats::putPattern(const Array &arr) {
    for (int i = 0; i < N; i++) {
        inputMeans[i] += arr[i];
        inputStd[i] += arr[i] * arr[i];
    }
    Nv++;
}

const Array& FileStats::getInputMeans() const {
    return inputMeans;
}

const Array& FileStats::getInputStd() const {
    return inputStd;
}

const Array& FileStats::getOriginalInputMeans() const {
    return originalInputMeans;
}

const Array& FileStats::getOriginalInputStd() const {
    return originalInputStd;
}

const Array& FileStats::getDistanceMeasure() const {
    return distanceMeasure;
}

int FileStats::getNv() {
    return Nv;
}

void FileStats::endWriting() {
    assert(Nv > 0); // Remove this assertion to allow empty validation files
    double norm = 0.0;
//    for (int i = 0; i < N; i++) {
//        cout<<inputMeans[i]<<" ";
//    }
//    getchar();
//    for (int i = 0; i < N; i++) {
//        cout<<inputStd[i]<<" ";
//    }
//    getchar();
    for (int i = 0; i < N; i++) {
        if (Nv > 0) {
            inputMeans[i] /= Nv;
            originalInputMeans[i] = inputMeans[i];
            inputStd[i] /= Nv;
            inputStd[i] -= inputMeans[i] * inputMeans[i];
            inputStd[i] = sqrt(inputStd[i]);
            originalInputStd[i] = inputStd[i];
            if(inputStd[i]>EPSILON) {
                distanceMeasure[i] = 1 / inputStd[i];
            } else {
                distanceMeasure[i] = EPSILON;
            }
        } else {
            inputMeans[i] = 0;
            inputStd[i] = 1.0;
            distanceMeasure[i] = 1;
        }
//        cout<<distanceMeasure[i]<<" ";
        norm += distanceMeasure[i];
    }
    if (norm < 1e-6) {
        norm = 1e-6;
//        assert(0); // bad distance measure
    }
    for (int i = 0; i < N; i++) {
        distanceMeasure[i] /= norm;
    }
}

void FileStats::beginWriting() {
    for (int i = 0; i < N; i++) {
        inputMeans[i] = 0;
        inputStd[i] = 0;
    }
    Nv = 0;
}

void FileStats::setInputMeans(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++)
        inputMeans[i] = arr[i];
}

void FileStats::setInputStd(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++)
        inputStd[i] = arr[i];
}

void FileStats::setZeroMeanUnitStd() {
    for (size_t i = 0; i < N; i++) {
        inputMeans[i] = 0.0;
        inputStd[i] = 1.0;
    }
}

void FileStats::setInputVar(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++)
        inputStd[i] = sqrt(arr[i]);
}

void FileStats::setDistanceMeasure(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++)
        distanceMeasure[i] = arr[i];
}

void print1(const Array &arr) {
    for (size_t i = 0; i < arr.size(); i++)
        cout << arr[i] << " ";
    cout << endl;
}

void print1(const Matrix &arr) {
    for (size_t i = 0; i < arr.size(); i++) {
        print1(arr[i]);
    }
}

void FileStats::print() {
    cout << "Nv = " << Nv << endl;
    cout << "mean: ";
    print1(inputMeans);
    cout << "std: ";
    print1(inputStd);
    cout << "dm: ";
    print1(distanceMeasure);
}

void FileStats::addRandomProbes(int numProbes) {

    for(int n=0; n<numProbes; n++) {
    inputMeans.push_back(0.0);
    inputStd.push_back(1.0);
    originalInputMeans.push_back(0.0);
    originalInputStd.push_back(1.0);
    distanceMeasure.push_back(0.0); // random probes will not affect clustering
    
    }
    N+=numProbes;
}

static void reorder(vector<double> &temp, vector<double> &inputMeans, vector<int> &order_fn, int N) {
    for(int n=0; n<N; n++) {
        temp[n] = inputMeans[order_fn[n]-1];
    }
    inputMeans.resize(N); // note: resize automatically sets data to zeros
    for(int n=0; n<N; n++) {
        inputMeans[n] = temp[n];
    }
}

void FileStats::reOrder(vector<int> order_fn) {
    N = order_fn.size();
    
    vector<double> temp;
    temp.resize(N);
    
    reorder(temp, inputMeans, order_fn, N);
    reorder(temp, inputStd, order_fn, N);
    reorder(temp, originalInputMeans, order_fn, N);
    reorder(temp, originalInputStd, order_fn, N);
    reorder(temp, distanceMeasure, order_fn, N);
}
