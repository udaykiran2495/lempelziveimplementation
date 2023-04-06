#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main() {

    // string to compare

    // load string from file
    ifstream finalStringFile;
    finalStringFile.open("finalString.txt");

    // read file
    string finalString;
    finalStringFile>>finalString;

    // load string from file
    ifstream compressedFile;
    compressedFile.open("compressed.txt");

    // read file
    string compressedString;
    compressedFile>>compressedString;

    bool isSameString;

    if(compressedString == finalString) {
        isSameString = true;
        cout<<"Same string"<<endl;
    }
    else {
        isSameString = false;
        cout<<"Different string"<<endl;
    }



    
    return 0;
}