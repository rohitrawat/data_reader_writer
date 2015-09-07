/* 
 * File:   file_io.h
 * Author: rohit
 *
 * Created on September 5, 2015, 6:04 PM
 */

#ifndef FILE_IO_H
#define	FILE_IO_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

// To suppress MS CRT secure warnings. Should be placed before all include statements.
#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stdio.h>

#include <cstdlib>
#include <cmath>

#include "types.h"

#define CONSTRUCTOR_MSG(className) //cout<<"Constructing "<<className<<" object."<<endl;
#define DESTRUCTOR_MSG(className) //cout<<"Destructing "<<className<<" object."<<endl;

enum file_type {
    REGRESSION_FILE, CLASSIFICATION_FILE
};

class File {
protected:
    int N, M, isClassification;
    int Nv;
    int hasOutputs;

    enum {
        CLOSED, READING, WRITING
    } state;
public:

    File(int N1, int M1, int isClassification1, int hasOutputs1 = 1) : N(N1), M(M1), isClassification(isClassification1), hasOutputs(hasOutputs1) {
        state = CLOSED;
    }

    ~File() {
        state = CLOSED;
    }

    virtual void beginWriting() = 0;
    virtual void putPattern(const Array &arr) = 0;
    virtual void endWriting() = 0;

    virtual void beginReading() = 0;
    virtual int getNextPattern(Array &arr) = 0;
    virtual void endReading() = 0;

    int getN() const {
        return N;
    }

    int getM() const {
        return M;
    }

    int getPatternSize() {
        if (isClassification)
            return N + 1;
        else
            return N + M;
    }

    int getIsClassification() const {
        return isClassification;
    }
};

class TextFile : public File {
    string fileName;
    FILE *fp;
    
    char delim;
    int n;
    int sizeBytes;
    long tenPercentBytes;
    int tenPercentCount;
    char writeFormat[10];
    int labelFirst;

public:
    TextFile(string fname, int N, int M, int isClassification, int fresh = 0, char* writeFormat = NULL, int hasOutputs = 1, int labelFirst = 0);
    ~TextFile();

    void beginWriting();
    void putPattern(const Array &arr);
    void endWriting();

    void beginReading();
    int getNextPattern(Array &arr);
    void endReading();
};

class BinaryFile : public File {
    string fileName;
    FILE *fp;
    vector<double> data;
    unsigned long int counter;
    int pattern_size;

public:
    BinaryFile(const File &file);
    BinaryFile(int N, int M, int isClassification);
    void init();
    ~BinaryFile();

    void beginWriting();
    void putPattern(const Array &arr);
    void endWriting();
    void beginReading();
    int getNextPattern(Array &arr);
    void endReading();
};

class MemoryFile : public File {
    vector<double> data;
    unsigned long int counter;
    int pattern_size;

public:
    MemoryFile(const File &file);
    MemoryFile(int N, int M, int isClassification);
    void init();
    ~MemoryFile();

    void beginWriting();
    void putPattern(const Array &arr);
    void endWriting();
    void beginReading();
    int getNextPattern(Array &arr);
    void endReading();
};


class FileStats {
    Array inputMeans;
    Array inputStd;
    Array originalInputMeans;
    Array originalInputStd;
    Array distanceMeasure;
    int N, M, Nv, isClassification;
public:
    FileStats(const File &file);
    void beginWriting();
    void putPattern(const Array &arr);
    void endWriting();

    const Array& getInputMeans() const;
    const Array& getInputStd() const;
    const Array& getOriginalInputMeans() const;
    const Array& getOriginalInputStd() const;
    const Array& getDistanceMeasure() const;

    void setInputMeans(const Array &arr);
    void setInputStd(const Array &arr);
    void setZeroMeanUnitStd();
    void setInputVar(const Array &arr);
    void setDistanceMeasure(const Array &arr);

    int getNv();
    void print();
    
    void addRandomProbes(int numProbes);
    
    void reOrder(vector<int> order_fn);
};

#endif	/* FILE_IO_H */
