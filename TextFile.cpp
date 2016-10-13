#include "file_io.h"

#include <string.h>

TextFile::TextFile(string fname1, int N1, int M1, int isClassification1, int fresh, char *writeFormat1, int hasOutputs1, int labelFirst1)
: File(N1, M1, isClassification1, hasOutputs1) {
    CONSTRUCTOR_MSG("TextFile");
    fileName = fname1;

    if (fresh == 0) {
        fp = fopen(fileName.c_str(), "r");
        state = READING;
    } else {
        fp = fopen(fileName.c_str(), "w");
        state = WRITING;
    }
    if (fp == NULL) {
        printf("TextFile: fopen() failed.\n");
        perror(fileName.c_str());
        printf("\n");
        exit(0);
    }
    delim = ' ';
    if (writeFormat1 == NULL) {
        strcpy(writeFormat, "%f\t");
    } else {
        strcpy(writeFormat, writeFormat1);
    }
    labelFirst = labelFirst1;
}

void TextFile::beginReading() {
    assert(fp != NULL);
    fseek(fp, 0L, ios::beg);
    state = READING;

    int header = 0;
    int read;
    char c;
    fseek(fp, 0L, SEEK_END);
    sizeBytes = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    while (!feof(fp)) {
        read = fscanf(fp, "%c", &c);
        if (read == 0) {
            break;
        }
        if (c == '\n') {
            break;
        }
        if (isalpha(c) && c!='e' && c!='E') {
            header = 1;
        }
        if (c == ',') {
            delim = ',';
        }
    }
    printf("Header: %d\n", header);
    printf("Delimiter: '%c'\n", delim);
    printf("Size: %ld\n", sizeBytes);
    if (header == 1) {
        printf("Rewinding..\n");
        rewind(fp);
        string col = "";
        while (!feof(fp)) {
            read = fscanf(fp, "%c", &c);
            if (read == 0) {
                break;
            }
            if (c == delim || c == '\t') {
                if(col.length()==0) {
                    continue; // ignore repeated delimiters
                }
                colnames.push_back(col);
                //cout<<col<<",";
                col.clear();
                continue;
            }
            if (c == '\n') {
                colnames.push_back(col);
                break;
            }
            col.push_back(c);
        }
    }
    if (header == 0) {
        printf("Rewinding..\n");
        rewind(fp);
    }
    Nv = 0;
    tenPercentBytes = (long) floor(sizeBytes / 10.0);
    tenPercentCount = 1;
}

void TextFile::beginWriting() {
    //	assert(0);	// not implemented correctly
    //	assert(fp != NULL);
    //	fseek(fp, 0L, ios::end);
    //	state = WRITING;

    assert(fp != NULL);
    fseek(fp, 0L, ios::beg);
    state = WRITING;
}

void TextFile::endReading() {
    // nothing to do
    cout << "Done. " << endl;
    fflush(0);
    state = CLOSED;
}

void TextFile::endWriting() {
    // nothing to do
    state = CLOSED;
    fclose(fp);
    fp = NULL; // fix double free corruption
}

int TextFile::getNextPattern(Array &arr) {
    int read;
    assert(state == READING);
    if (fp == NULL || feof(fp)) {
        return 0;
    }
    
    double temp;

    if (labelFirst && isClassification) {
        if (delim == ',') {
            read = fscanf(fp, "%lf,", &temp);
        } else {
            read = fscanf(fp, "%lf", &temp);
        }
        if (read != 1) {
            return 0;
        }
        arr[N] = (REAL) ((int) temp);
        if (temp < 0 || temp > M) { // should be strictly <1 || >M, but currently relaxed
            cout << "Invalid class ID: " << temp << endl;
            exit(0);
        }
    }

    for (int i = 0; i < N; i++) {
        if (delim == ',') {
            read = fscanf(fp, "%lf,", &temp);
            arr[i] = temp;
        } else {
            read = fscanf(fp, "%lf", &temp);
            arr[i] = temp;
        }
        if (read != 1) {
            return 0;
        }
    }

    if (hasOutputs && labelFirst == 0) {
        if (isClassification) {
            double temp;
            if (fscanf(fp, "%lf", &temp) != 1) {
                return 0;
            }
            arr[N] = (REAL) ((int) temp);
            if (temp < 0 || temp > M) {
                cout << "Invalid class ID: " << temp << endl;
                exit(0);
            }
        } else {
            for (int i = 0; i < M; i++) {
                if (delim == ',') {
                    read = fscanf(fp, "%lf,", &temp);
                    arr[N + i] = temp;
                } else {
                    read = fscanf(fp, "%lf", &temp);
                    arr[N + i] = temp;
                }
                if (read != 1) {
                    return 0;
                }
            }
        }
    }
    Nv++;

    long bytes = ftell(fp);
    if (bytes > tenPercentCount * tenPercentBytes) {
        cout << tenPercentCount * 10 << "%.. ";
        fflush(0);
        tenPercentCount++;
    }
    return 1;
}

void TextFile::putPattern(const Array &arr) {
    //	assert(state == WRITING);
    //        printf("TextFile::putPattern Not implemented.");
    //	exit(0);	// not implemented

    for (int i = 0; i < N; i++) {
        fprintf(fp, writeFormat, arr[i]);
    }
    if (hasOutputs) {
        if (isClassification) {
            fprintf(fp, "%d", (int) arr[N]);
        } else {
            for (int i = 0; i < M; i++) {
                fprintf(fp, writeFormat, arr[N + i]);
            }
        }
    }
    fprintf(fp, "\n");
}

TextFile::~TextFile() {
    DESTRUCTOR_MSG("TextFile");
    if (fp != NULL)
        fclose(fp);
}

string TextFile::getColName(int i) {
    if(colnames.size()==0) {
        return "V"+i;
    }
    return colnames[i];
}

const vector<string>& TextFile::getAllColNames() {
    return colnames;
}

void TextFile::setLabelFirst() {
    labelFirst = 1;
}
