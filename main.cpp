/*
 * File:   main.cpp
 * Author: rohit
 *
 * Created on September 5, 2015, 7:40 PM
 */

#include <cstdlib>
#include "file_io.h"
#include "numeric.h"

using namespace std;

FileStats computeStats(File &sourceFile) {
    BinaryFile tempFile(sourceFile.getN(), sourceFile.getM(), sourceFile.getIsClassification());
    FileStats stats(tempFile);
    Array xp;
    xp.resize(sourceFile.getPatternSize());

    sourceFile.beginReading();
    tempFile.beginWriting();
    stats.beginWriting();
    while (sourceFile.getNextPattern(xp)) {
        tempFile.putPattern(xp);
        stats.putPattern(xp);
    }
    sourceFile.endReading();
    tempFile.endWriting();
    stats.endWriting();

    //print(stats.getInputMeans());
    //print(stats.getInputStd());
    //print(stats.getOutputEnergies());

    return stats;
}

FileStats preProcess(File &sourceFile, File &destFile) {
    BinaryFile tempFile(sourceFile.getN(), sourceFile.getM(), sourceFile.getIsClassification());
    FileStats stats(tempFile);
    Array xp;
    xp.resize(sourceFile.getPatternSize());

    sourceFile.beginReading();
    tempFile.beginWriting();
    stats.beginWriting();
    while (sourceFile.getNextPattern(xp)) {
        tempFile.putPattern(xp);
        stats.putPattern(xp);
    }
    sourceFile.endReading();
    tempFile.endWriting();
    stats.endWriting();
    //print(stats.getInputMeans());
    //print(stats.getInputStd());
    //print(stats.getOutputEnergies());

    FileStats stats2(destFile);
    tempFile.beginReading();
    destFile.beginWriting();
    stats2.beginWriting();
    while (tempFile.getNextPattern(xp)) {
        SubtractArray(xp, stats.getInputMeans());
        ElementDivide(xp, stats.getInputStd(), xp);
        destFile.putPattern(xp);
        stats2.putPattern(xp);
    }
    tempFile.endReading();
    destFile.endWriting();
    stats2.endWriting();

    stats2.setDistanceMeasure(stats.getDistanceMeasure());

    //print(stats2.getInputMeans());
    //print(stats2.getInputStd());
    //print(stats2.getOutputEnergies());
    //print(stats2.getDistanceMeasure());

    return stats;
}

FileStats preProcess(File &sourceFile, File &destFile, FileStats &sourceFileStats) {
    Array xp;
    xp.resize(sourceFile.getPatternSize());

    FileStats stats2(destFile);
    sourceFile.beginReading();
    destFile.beginWriting();
    stats2.beginWriting();
    while (sourceFile.getNextPattern(xp)) {
        SubtractArray(xp, sourceFileStats.getInputMeans());
        ElementDivide(xp, sourceFileStats.getInputStd(), xp);
        destFile.putPattern(xp);
        stats2.putPattern(xp);
    }
    sourceFile.endReading();
    destFile.endWriting();
    stats2.endWriting();

    stats2.setDistanceMeasure(sourceFileStats.getDistanceMeasure());

    //print(stats2.getInputMeans());
    //print(stats2.getInputStd());
    //print(stats2.getOutputEnergies());
    //print(stats2.getDistanceMeasure());

    return stats2;
}


FileStats preProcessFiles(string trainingFileName, string validationFileName, int N, int M, int isClassification, BinaryFile &preprocessedTrainingFile, BinaryFile &preprocessedValidationFile, int labelFirst) {
    cout << "Training file: " << trainingFileName << endl;
    cout << "Validation file: " << validationFileName << endl;
    cout << "Type: " << (isClassification ? "Classification" : "Regression") << " type" << endl;
    cout << "N = " << N << ", M = " << M << endl;

    TextFile trainingFile(trainingFileName, N, M, isClassification);
    TextFile validationFile(validationFileName, N, M, isClassification);

    if(labelFirst) {
      trainingFile.setLabelFirst();
      validationFile.setLabelFirst();
    }

    cout << "Processing training file..\r\n";
    FileStats trgStats = preProcess(trainingFile, preprocessedTrainingFile);
    cout << "Stats of original training file: " << endl;
    trgStats.print();

    cout << "Processing validation file using training file stats..\r\n";
    FileStats valStats = preProcess(validationFile, preprocessedValidationFile, trgStats);
    cout << "Stats of validation file after pre-processing:\n";
    valStats.print();

    trgStats.setZeroMeanUnitStd();

    return trgStats;
}

void splitTrainingValidation(File &sourceFile, File &trainingFile, File &validationFile, float ratio = 0.7) {
    Array xp;
    xp.resize(sourceFile.getPatternSize());
    float p;

    sourceFile.beginReading();
    trainingFile.beginWriting();
    validationFile.beginWriting();
    srand(3141);
    int Nv_t = 0, Nv_v = 0;
    while (sourceFile.getNextPattern(xp)) {
        p = (float)rand()/(float)(RAND_MAX);
        if(p<ratio) {
            trainingFile.putPattern(xp);
            Nv_t++;
        } else {
            validationFile.putPattern(xp);
            Nv_v++;
        }
    }
    sourceFile.endReading();
    trainingFile.endWriting();
    validationFile.endWriting();

    printf("Training Nv = %d\nValidation Nv = %d\n", Nv_t, Nv_v);
}

void splitTrainingValidation(string sourceFileName, string trainingFileName, string validationFileName, int N, int M, int isClassification, float ratio = 0.7, int labelFirst = 0) {
    cout << "Source file: " << sourceFileName << endl;
    cout << "Training file: " << trainingFileName << endl;
    cout << "Validation file: " << validationFileName << endl;
    cout << "Type: " << (isClassification ? "Classification" : "Regression") << " type" << endl;
    cout << "N = " << N << ", M = " << M << endl;

    enum {existing_file, new_file};
    TextFile sourceFile(sourceFileName, N, M, isClassification, existing_file);
    TextFile trainingFile(trainingFileName, N, M, isClassification, new_file);
    TextFile validationFile(validationFileName, N, M, isClassification, new_file);

    splitTrainingValidation(sourceFile, trainingFile, validationFile, ratio);
}


/*
 *
 */
int main(int argc, char** argv) {
    char fnameTrg[] = "data/mnist100.tra";
    char fnameTst[] = "data/mnist100.tst";
    int N=784;
    int M=10;
    int isClassification = 1;
    int labelFirst = 1;
    float ratio = 0.7;

    splitTrainingValidation(fnameTrg, "train0.tsv", "val0.tsv", N, M, isClassification, ratio, labelFirst);
    cout<<"done"<<endl;
    exit(0);

    BinaryFile preprocessedTrainingFile(N, M, isClassification), preprocessedValidationFile(N, M, isClassification);
    FileStats trgStats = preProcessFiles("train0.tsv", "val0.tsv", N, M, isClassification, preprocessedTrainingFile, preprocessedValidationFile, labelFirst);

    return 0;
}
