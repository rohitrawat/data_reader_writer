# data_reader_writer
A C++ library to read and write to data files in various formats with an interface similar to Java collections.

Simple File IO library for pattern by pattern access of data files. It wraps low level IO functions and gives an interface similar to Java collections. Eg. `fileObj.beginReading()`, `fileObj.getNextPattern()`, `fileObj.endReading()`, and similar functions for writing. Random access is not allowed as it is not needed in most applications. It abstracts the internal storage representation as the `File` interface, of which three implementations are provided:

1. `TextFile` (ASCII human readable CSV/TSV files)
2. `BinaryFile` (Binary storage for better speed, accuracy, and compactness than ASCII files)
3. `MemoryFile` (Data is dumped into an STL vector, for super fast reads/writes)

Also included is a FileStats class and preprocessing functions useful for normalizing data.

See main.cpp for usage and a demo.

**Example: A file splitting function can be written in a few lines that will work for any physical format of the file:**


```
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
```
