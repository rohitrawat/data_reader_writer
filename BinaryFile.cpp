#include "file_io.h"
#include <string.h>

using namespace std;

#if 0

#include <vector>
#include <string.h>
using namespace std;

BinaryFile::BinaryFile(const File& file)
: File(file.getN(), file.getM(), file.getIsClassification()) {
    init();
}

BinaryFile::BinaryFile(int N1, int M1, int isClassification1)
: File(N1, M1, isClassification1) {
    init();
}

void BinaryFile::init() {
    CONSTRUCTOR_MSG("BinaryFile");
    
    counter = 0;
    data.clear();
    if (isClassification) {
        pattern_size = N+1;
    } else {
        pattern_size = N+M;
    }
    //data.capacity(128*pattern_size);
    state = WRITING;
}

void BinaryFile::beginReading() {
    counter = 0;
    state = READING;
}

void BinaryFile::beginWriting() {
    counter = data.size()/pattern_size;
    state = WRITING;
}

void BinaryFile::endReading() {
    assert(state == READING);
    // nothing to do
    state = CLOSED;
}

void BinaryFile::endWriting() {
    assert(state == WRITING);
    // nothing to do
    state = CLOSED;
}

int BinaryFile::getNextPattern(Array &arr) {
    assert(state == READING);
    assert((isClassification && arr.size() >= (size_t) (N + 1)) || (!isClassification && arr.size() >= (size_t) (N + M))); // arr is not of sufficient size

    if ((counter+1)*pattern_size>data.size()) // end of file
        return 0;

    memcpy(&arr[0], &data[counter*pattern_size], sizeof (REAL)*pattern_size);
    counter++;

    // more checks needed.

    return 1;
}

void BinaryFile::putPattern(const Array &arr) {
    assert(state == WRITING);
    assert((isClassification && arr.size() >= (size_t) (N + 1)) || (!isClassification && arr.size() >= (size_t) (N + M))); // arr is not of sufficient size

    for(int i=0; i<pattern_size; i++) {
        data.push_back(arr[i]);
    }
    counter++;
}

BinaryFile::~BinaryFile() {
    DESTRUCTOR_MSG("BinaryFile");
    
    data.clear();
    counter = 0;
}

#else

BinaryFile::BinaryFile(const File& file)
: File(file.getN(), file.getM(), file.getIsClassification()) {
    init();
}

BinaryFile::BinaryFile(int N1, int M1, int isClassification1)
: File(N1, M1, isClassification1) {
    init();
}

void BinaryFile::init() {
    CONSTRUCTOR_MSG("BinaryFile");

    // Obtain path to the "temp" folder	// todo make this static member
    string path_to_temp = ".";
#ifdef unix
    const char *temp_env_var = "TMPDIR";
    const char *default_tempdir = "/tmp";
#else
    char temp_env_var[] = "TEMP";
    char default_tempdir[] = ".";
#endif
    if (getenv(temp_env_var) == 0) {
        path_to_temp = default_tempdir;
    } else {
        path_to_temp = getenv(temp_env_var);
    }

    // Open a transient binary file
#ifdef unix
    string templatestr = path_to_temp + "/PlnTmpXXXXXX";
    
    char *templatechr = new char[templatestr.length()+8];
    strcpy(templatechr, templatestr.c_str());
    int fdescriptor = mkstemp(templatechr);
    fp = fdopen(fdescriptor, "wb+");
    fileName = string(templatechr);
#else
    char *tempnam_result = _tempnam(path_to_temp.c_str(), "PlnTmp");
    fileName = string(tempnam_result);
    if (tempnam_result) // fixed a leak detected by valgrind
        free(tempnam_result);
    fp = fopen(fileName.c_str(), "wb+");
#endif

    if (fp == NULL) {
        perror(fileName.c_str());
        exit(0);
    }
    state = WRITING;
}

void BinaryFile::beginReading() {
    assert(fp != NULL);
    fseek(fp, 0L, ios::beg);
    state = READING;
}

void BinaryFile::beginWriting() {
    assert(fp != NULL);
    fseek(fp, 0L, ios::end);
    state = WRITING;
}

void BinaryFile::endReading() {
    assert(state == READING);
    // nothing to do
    state = CLOSED;
}

void BinaryFile::endWriting() {
    assert(state == WRITING);
    // nothing to do
    state = CLOSED;
}

int BinaryFile::getNextPattern(Array &arr) {
    assert(state == READING);
    assert((isClassification && arr.size() >= (size_t) (N + 1)) || (!isClassification && arr.size() >= (size_t) (N + M))); // arr is not of sufficient size

    if (fp == NULL)
        return 0;

    if (feof(fp)) // end of file
        return 0;

    if (isClassification) {
        if (fread(&arr[0], sizeof (REAL), N + 1, fp) != N + 1) {
            return 0; // bad pattern
        }
    } else {
        if (fread(&arr[0], sizeof (REAL), N + M, fp) != N + M) {
            return 0; // bad pattern
        }
    }

    // more checks needed.

    return 1;
}

void BinaryFile::putPattern(const Array &arr) {
    assert(state == WRITING);
    assert((isClassification && arr.size() >= (size_t) (N + 1)) || (!isClassification && arr.size() >= (size_t) (N + M))); // arr is not of sufficient size

    if (isClassification) {
        fwrite(&arr[0], sizeof (REAL), N + 1, fp);
    } else {
        fwrite(&arr[0], sizeof (REAL), N + M, fp);
    }
}

BinaryFile::~BinaryFile() {
    DESTRUCTOR_MSG("BinaryFile");
    if (fp != NULL)
        fclose(fp);

    if (remove(fileName.c_str()) != 0) {
        perror(fileName.c_str());
    }
}

void BinaryFile::truncate() {
    fp = freopen(NULL, "wb+", fp);
    state = WRITING;
    if (fp == NULL) {
        perror(fileName.c_str());
        exit(0);
    }
}

void BinaryFile::truncate(int N1, int M1, int isClassification1) {
    truncate();
    
    N = N1;
    M = M1;
    isClassification = isClassification1;
    
    if (isClassification)
        patternSize =  N + 1;
    else
        patternSize =  N + M;
}

void BinaryFile::dump(string dumpName) {
    Array pattern;
    pattern.resize(getPatternSize());
    beginReading();
    
    TextFile out(dumpName, N, M, isClassification, 1);
    out.beginWriting();
    while(getNextPattern(pattern)) {
        out.putPattern(pattern);
    }
    out.endWriting();
    endReading();
}

#endif
