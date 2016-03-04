# data_reader_writer
A C++ library to read and write to data files in various formats with an interface similar to Java collections.

Simple File IO library for pattern by pattern access of data files. It wraps low level IO functions and gives an interface similar to Java collections. Eg. `fileObj.beginReading()`, `fileObj.getNextPattern()`, `fileObj.endReading()`, and similar functions for writing. Random access is not allowed as it is not needed in most applications. It abstracts the internal storage representation as the `File` interface, of which three implementations are provided:

1. `TextFile` (ASCII human readable CSV/TSV files)
2. `BinaryFile` (Binary storage for better speed, accuracy, and compactness than ASCII files)
3. `MemoryFile` (Data is dumped into an STL vector, for super fast reads/writes)

Also included is a FileStats class and preprocessing functions useful for normalizing data.

See main.cpp for usage and a demo.
