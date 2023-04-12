#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {

    // compare two file contents

    // open file1

    ifstream ifs("input.txt");
    string inputText( istreambuf_iterator<char>(ifs), (istreambuf_iterator<char>()) );
    ifs.close();



    // open file2

    ifstream ifs2("decompressed.txt");
    string decompressedString( istreambuf_iterator<char>(ifs2), (istreambuf_iterator<char>()) );
    ifs2.close();
    


    if (inputText == decompressedString) {
        cout << "The files are identical" << endl;
    } else {
        cout << "The files are not identical" << endl;
    }

    return 0;

}