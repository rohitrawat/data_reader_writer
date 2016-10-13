#include "file_io.h"

using namespace std;

FileStats::FileStats(const File &file) {
    CONSTRUCTOR_MSG("FileStats");
    N = file.getN();
    M = file.getM();
    if(N<1) {
        cout<<"Error N<1"<<endl;
        exit(0);
    }
    isClassification = file.getIsClassification();
    inputMeans.resize(N, 0.0); // note: resize automatically sets data to zeros
    inputStd.resize(N, 0.0);    // must be set to 0  as it is accumulated
    outputMeans.resize(M, 0.0);
    outputStd.resize(M, 0.0);
    originalInputMeans.resize(N, 0.0); // this will store the files input mean and std which was used to normalize it
    originalInputStd.resize(N, 1.0);
    originalOutputMeans.resize(M, 0.0);
    originalOutputStd.resize(M, 1.0);
    distanceMeasure.resize(N, 1.0/N);
    Nv = 0;
    
    Nvv = 0;
    
    classCounts.resize(M);
}

void FileStats::putPattern(const Array &arr) {
    for (int i = 0; i < N; i++) {
        inputMeans[i] += arr[i];
        inputStd[i] += arr[i] * arr[i];
    }
    for (int i = 0; !isClassification && i < M; i++) {
        outputMeans[i] += arr[i+N];
        outputStd[i] += arr[i+N] * arr[i+N];
    }
    if(isClassification) {
        if(arr[N]>M || int(arr[N])!=arr[N]) {
            cout<<"Invalid class ID found..";
            cout<<"Nv = "<<Nv<<", class ID = "<<arr[N]<<endl;
            exit(0);
        }
        classCounts[arr[N]-1]++;
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

const Array& FileStats::getOutputMeans() const {
    return outputMeans;
}

const Array& FileStats::getOutputStd() const {
    return outputStd;
}

const Array& FileStats::getOriginalOutputMeans() const {
    return originalOutputMeans;
}

const Array& FileStats::getOriginalOutputStd() const {
    return originalOutputStd;
}

const Array& FileStats::getDistanceMeasure() const {
    return distanceMeasure;
}

int FileStats::getNv() {
    return Nv;
}

int FileStats::getNvv() {
    return Nvv;
}

void FileStats::setNvv(int Nvv1) {
    Nvv = Nvv1;
}

void FileStats::endWriting() {
    assert(Nv > 0); // Remove this assertion to allow empty validation files
    double norm = 0.0;
    int zeroVarInputCount = 0;
    for (int i = 0; i < N; i++) {
        if (Nv > 0) {
            inputMeans[i] /= Nv;
            originalInputMeans[i] = inputMeans[i];

            inputStd[i] /= Nv;
            inputStd[i] -= inputMeans[i] * inputMeans[i];
            inputStd[i] = sqrt(inputStd[i]);
            originalInputStd[i] = inputStd[i];

            if(inputStd[i] > 1e-6) {
                distanceMeasure[i] = 1 / inputStd[i];
            } else {
                distanceMeasure[i] = 0;     // if an input is constant, it has no usefulness in computing the distance measure
                cout<<"Detected zero variance input.."<<i<<"\n";
                zeroVarInputCount++;
                inputStd[i] = 1.0;
            }
        } else {  // sometimes we have empty files (validation?), use dummy values
            inputMeans[i] = 0;
            inputStd[i] = 1.0;
            distanceMeasure[i] = 1;
        }
        norm += distanceMeasure[i];
    }
    if (norm < 1e-6) {
        norm = 1e-6;
        cout<<"Feature variance too high! bad distance measure!"<<endl;
        //assert(0); // bad distance measure
        norm = N;
    }
    for (int i = 0; i < N; i++) {
        distanceMeasure[i] /= norm;
    }
    
    if(zeroVarInputCount>0) {
        cout<<"Found "<<zeroVarInputCount<<" zero variance inputs.\n";
    }

    // output means and std are only calculated for regression files
    for (int i = 0; i < M; i++) {
        if (Nv > 0 && !isClassification) {
            outputMeans[i] /= Nv;
            originalOutputMeans[i] = outputMeans[i];

            outputStd[i] /= Nv;
            outputStd[i] -= outputMeans[i] * outputMeans[i];
            outputStd[i] = sqrt(outputStd[i]);
            originalOutputStd[i] = outputStd[i];

            if(outputStd[i] < 1e-6) {
                cout<<"Detected zero variance output.."<<i<<"\n";
                cout<<"Please remove it from your data!\n";
                //exit(0); WARNING
                outputStd[i] = 1.0;
            }
            
        } else {  // dummy values
            outputMeans[i] = 0;
            outputStd[i] = 1.0;
        }
    }
    
    // check for proper class distribution
    if(isClassification) {
        for(int i=0; i<M; i++) {
            if(double(classCounts[i])/Nv<0.2/M) {
                cout<<"Class "<<i+1<<" has too few patterns = "<<classCounts[i]<<" ("<<double(classCounts[i])*100.0/Nv<<"%). Cannot continue.\n";
                exit(0);
            }
        }
    }
}

void FileStats::beginWriting() {
    for (int i = 0; i < N; i++) {
        inputMeans[i] = 0;
        inputStd[i] = 0;
    }
    for (int i = 0; !isClassification && i < M; i++) {
        outputMeans[i] = 0;
        outputStd[i] = 0;
    }
    Nv = 0;
}

void FileStats::setInputMeans(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++) {
        inputMeans[i] = arr[i];
    }
}

void FileStats::setInputStd(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++) {
        inputStd[i] = arr[i];
    }
}

void FileStats::setInputVar(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++) {
        inputStd[i] = sqrt(arr[i]);
    }
}

void FileStats::setDistanceMeasure(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++) {
        distanceMeasure[i] = arr[i];
    }
}

void FileStats::setOriginalInputMeans(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++) {
        originalInputMeans[i] = arr[i];
    }
}

void FileStats::setOriginalInputStd(const Array &arr) {
    assert(arr.size() == N);
    for (size_t i = 0; i < arr.size(); i++) {
        originalInputStd[i] = arr[i];
    }
}

//static void print1(const Array &arr) {
//    for (size_t i = 0; i < arr.size(); i++) {
//        cout << arr[i] << " ";
//    }
//    cout << endl;
//}

static void print1(const Array &arr, int trunc=0) {
    // trunc = 0 (default))
    int limit;
    if(trunc && arr.size()>10) {
        limit = 5;
    } else {
        limit = arr.size();
    }
    for(int i=0; i<limit; i++) {
        cout<<arr[i]<<",";
    }
    if(trunc && arr.size()>10) {
        cout<<" ... (truncated) ";
        for(int i=arr.size()-5; i<arr.size(); i++) {
            cout<<arr[i]<<",";
        }
    }
    cout<<"\n";
}

static void print1(const Matrix &arr) {
    for (size_t i = 0; i < arr.size(); i++) {
        print1(arr[i]);
    }
}

#define TRUNC 1
void FileStats::print() {
    cout << "Nv = " << Nv << endl;
    cout << "input_mean = [";
    print1(inputMeans,TRUNC);
    cout<<"]\n";
    cout << "input_std = [";
    print1(inputStd,TRUNC);
    cout<<"]\n";
    cout << "output_mean = [";
    print1(outputMeans,TRUNC);
    cout<<"]\n";
    cout << "output_std = [";
    print1(outputStd,TRUNC);
    cout<<"]\n";
    cout << "dm = [";
    print1(distanceMeasure,TRUNC);
    cout<<"]\n";
}

void FileStats::addRandomProbes(int numProbes) {
    N+=numProbes;
    for(int n=0; n<numProbes; n++) {
        inputMeans.push_back(0.0);
        inputStd.push_back(1.0);
        originalInputMeans.push_back(0.0);
        originalInputStd.push_back(1.0);
        // distanceMeasure.push_back(0.5/N); // random probes will not affect clustering (much))
        //Note: if CPLN is used in future, using 0.5/N may not be good. We should basically use zeros here.
        distanceMeasure.push_back(0.0); // random probes will not affect clustering (much))
    }
    // re-normalize the distanceMeasure
    double norm = 0.0;
    for(int n=0; n<N; n++) {
        norm += distanceMeasure[n];
    }
    for(int n=0; n<N; n++) {
        distanceMeasure[n] /= norm;
    }
}

// selected_features starts at 1
static void reorder(Array &temp, Array &array, vector<int> &selected_features, int N) {
    for(int n=0; n<N; n++) {
        temp[n] = array[selected_features[n]-1];
    }
    array.resize(N); // note: resize automatically sets data to zeros
    for(int n=0; n<N; n++) {
        array[n] = temp[n];
    }
}

// selected_features starts at 1
void FileStats::reOrder(vector<int> selected_features) {
    N = selected_features.size();
    
    Array temp;
    temp.resize(N);
    
    reorder(temp, inputMeans, selected_features, N);
    reorder(temp, inputStd, selected_features, N);
    reorder(temp, originalInputMeans, selected_features, N);
    reorder(temp, originalInputStd, selected_features, N);
    reorder(temp, distanceMeasure, selected_features, N);
    double norm = 0.0;
    for(int i=0; i<N; i++) {
        norm += distanceMeasure[i];
    }
    if(norm == 0) {
        for(int i=0; i<N; i++) {
           distanceMeasure[i] = 1.0/N;
        }
    } else {
        for(int i=0; i<N; i++) {
            distanceMeasure[i]/=norm;
        }
    }
}
