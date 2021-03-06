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

using namespace std;

//#define DEBUG_PRE_PROCESS
#ifdef DEBUG_PRE_PROCESS 
#define D(x) x
#else 
#define D(x)
#endif

#define CONSTRUCTOR_MSG(className) //cout<<"Constructing "<<className<<" object."<<endl;
#define DESTRUCTOR_MSG(className) //cout<<"Destructing "<<className<<" object."<<endl;

enum file_type {
    REGRESSION_FILE, CLASSIFICATION_FILE
};

class File {
protected:
    int N, M, isClassification;
    int patternSize;
    int Nv;
    int hasOutputs;
    string label;

    enum {
        CLOSED, READING, WRITING
    } state;
public:

    File(int N1, int M1, int isClassification1, int hasOutputs1 = 1, string label1 = "File") : N(N1), M(M1), isClassification(isClassification1), hasOutputs(hasOutputs1), label(label1) {
        if(N<1) {
            std::cout<<"File("<<N<<","<<M<<","<<isClassification1<<","<<hasOutputs<<") failed."<<std::endl;
            exit(0);
        }
        state = CLOSED;
        if (isClassification)
            patternSize =  N + 1;
        else
            patternSize =  N + M;
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

    int getPatternSize() const {
        return patternSize;
    }

    int getIsClassification() const {
        return isClassification;
    }
    
    string getLabel() {
        return label;
    }
    
    void setLabel(string l) {
        label = l;
    }
};

class TextFile : public File {
    string fileName;
    FILE *fp;

    char delim;
    int n;
    unsigned long int sizeBytes;
    long tenPercentBytes;
    int tenPercentCount;
    char writeFormat[10];
    int labelFirst;
    vector<string> colnames;

public:
    TextFile(string fname, int N, int M, int isClassification, int fresh = 0, char* writeFormat = NULL, int hasOutputs = 1, int labelFirst = 0);
    ~TextFile();

    void beginWriting();
    void putPattern(const Array &arr);
    void endWriting();

    void beginReading();
    int getNextPattern(Array &arr);
    void endReading();

    string getColName(int i);
    const vector<string>& getAllColNames();

    void setLabelFirst();
};

class BinaryFile : public File {
    string fileName;
    FILE *fp;
    vector<REAL> data;
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
    
    void truncate();
    void truncate(int N, int M, int isClassification);
    
    void dump(string dumpName);
};

class MemoryFile : public File {
    vector<REAL> data;
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
    Array outputMeans;
    Array outputStd;
    Array originalInputMeans;
    Array originalInputStd;
    Array originalOutputMeans;
    Array originalOutputStd;
    Array distanceMeasure;
    int N, M, Nv, isClassification;
    int Nvv;
    vector<int> classCounts;
public:
    FileStats(const File &file);
    void beginWriting();
    void putPattern(const Array &arr);
    void endWriting();

    const Array& getInputMeans() const;
    const Array& getInputStd() const;
    const Array& getOutputMeans() const;
    const Array& getOutputStd() const;
    const Array& getOriginalInputMeans() const;
    const Array& getOriginalInputStd() const;
    const Array& getOriginalOutputMeans() const;
    const Array& getOriginalOutputStd() const;
    const Array& getDistanceMeasure() const;

    void setInputMeans(const Array &arr);
    void setInputStd(const Array &arr);
    void setInputVar(const Array &arr);
    void setDistanceMeasure(const Array &arr);
    void setOriginalInputMeans(const Array &arr);
    void setOriginalInputStd(const Array &arr);

    int getNv();
    void print();
    
    void addRandomProbes(int numProbes);
    
    void reOrder(vector<int> order_fn);

    void setNvv(int Nvv);
    int getNvv();
};

// -------------------------------------------------------------------------- //
// file_utils.cpp
// -------------------------------------------------------------------------- //

// compute file stats
FileStats computeStats(File &sourceFile);
FileStats preProcess(File &sourceFile, File &destFile, int disableNormalization);
FileStats preProcess(File &sourceFile, File &destFile, FileStats &sourceFileStats);
FileStats preProcessFiles(string trainingFileName, string validationFileName, int N, int M, int isClassification, BinaryFile &preprocessedTrainingFile, BinaryFile &preprocessedValidationFile, int disableNormalization);
void check_file_health(File & file);
void dump(File &file, string dumpName);
int saveSelectedFeatures(File &infile, File &outfile, vector<int> input_columns_list);




#endif	/* FILE_IO_H */
