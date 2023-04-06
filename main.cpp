// Lempel Ziv Welch compression algorithm
// http://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch
// LZ 78 algorithm
// https://www.geeksforgeeks.org/lzw-lempel-ziv-welch-compression-technique/
#include <iostream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

const int MAX_BITS = 5; // max bits for encoding

unordered_map<char, int> alphabetMap;
unordered_map<int, string> reverseAlphabetMap;
unordered_map<string, int> phraseTable;
unordered_map<int, string> reversePhraseTable;

struct phraseItem {
    int phraseIndex;
    string phrase;
    string encodedString;
};

vector<phraseItem> encodingTable;

// create phrase item and add to encoding table
void addPhraseItem(int phraseIndex, string phrase, string encodedString) {
    phraseItem item;
    item.phraseIndex = phraseIndex;
    item.phrase = phrase;
    item.encodedString = encodedString;
    encodingTable.push_back(item);
}

vector<int> intToBits(int num, int bitCount) {
    vector<int> bits(bitCount, 0);
    for (int i = bitCount - 1; i >= 0 && num > 0; i--) {
        bits[i] = num % 2;
        num /= 2;
    }
    return bits;
}

void createAlphabetMap() {
    char c = 'A';
    for(int i = 0; i < 26; i++) {
        alphabetMap[c] = i;
        c++;
    }
}

void createReverseAlphabetMap() {
    char c = 'A';
    for(int i = 0; i < 26; i++) {
        reverseAlphabetMap[i].append(1, c);
        c++;
    }
}

void constructEncodingTable() {
    for(int i = 0; i < encodingTable.size() - 1  ; i++) {
        phraseItem item = encodingTable[i];
        // create encoded string
        string phrase = item.phrase;
        if(phrase.length() > 0) {
            string prefix =  phrase.substr(0, phrase.length() - 1);
            string lastChar = phrase.substr(phrase.length() - 1, 1);
            int prefixIndex = phraseTable[prefix];
            encodingTable[i].encodedString = to_string(prefixIndex) + lastChar;
        }
    }

    phraseItem lastItem = encodingTable[encodingTable.size() - 1];
    string phrase = lastItem.phrase;
    string prefix =  phrase.substr(0, phrase.length() - 1);
    string lastChar = phrase.substr(phrase.length() - 1, 1);
    if(phraseTable.find(phrase) == phraseTable.end() ) {
        // not found
        encodingTable[encodingTable.size() - 1].encodedString = "0" + phrase;
    }
    else {
        // found
        int prefixIndex = phraseTable[phrase];
        encodingTable[encodingTable.size() - 1].encodedString = to_string(prefixIndex);
    }
    return;
}

void getEncodingTable(string s) {
    cout<<"getEncodingTable"<<endl;
    int curr = 0;
    int next = 1;
    int phraseIndex = 1;  // as given in the LZ78 algorithm 
    while(next < s.length()  ) {
        string currString = s.substr(curr, next - curr );
        if((phraseTable.find(currString) == phraseTable.end() ) ) {
            // not found, add to encoding table
            phraseTable[currString] = phraseIndex;
            addPhraseItem(phraseIndex, currString, "");
            curr = next;
            next++;
            phraseIndex++;
        }
        else {
            // found
            next++;
        }
    }
    // add last phrase to phrase table
    string currString = s.substr(curr, next - curr );
    addPhraseItem(phraseIndex, currString, "");
    // create encoding table
    constructEncodingTable();
    return;
}

vector<int> getBinaryString() {
    cout<<"getBinaryString"<<endl;
    vector<int> binaryStringVector;
    for(int i = 0; i < encodingTable.size() - 1; i++) {
        phraseItem item = encodingTable[i];
        string encodedString = item.encodedString;
        string phraseIndex = encodedString.substr(0, encodedString.length() - 1);
        string alphabet = encodedString.substr(encodedString.length() - 1, MAX_BITS);
        int phraseIndexBits;
        if(i == 0 || i == 1) {
            phraseIndexBits = 1;
        }
        else {
            phraseIndexBits = log2(i) + 1;
        }
        int phraseIndexInt = stoi(phraseIndex);
        vector<int> phraseVector = intToBits(phraseIndexInt, phraseIndexBits);
        binaryStringVector.insert(binaryStringVector.end(), phraseVector.begin(), phraseVector.end());
        vector<int> alphabetVector = intToBits(alphabetMap[alphabet[0]], MAX_BITS);
        binaryStringVector.insert(binaryStringVector.end(), alphabetVector.begin(), alphabetVector.end());
    }

    // add last phrase
    phraseItem lastItem = encodingTable[encodingTable.size() - 1];
    string encodedString = lastItem.encodedString;

    if(encodedString.length() == 1) {
        // means only phase index is present without alphabet

        string phraseIndex = encodedString.substr(0, encodedString.length());
        int phraseIndexBits;
        phraseIndexBits = log2(encodingTable.size() - 1) + 1;
        int phraseIndexInt = stoi(phraseIndex);
        vector<int> phraseVector = intToBits(phraseIndexInt, phraseIndexBits);
        binaryStringVector.insert(binaryStringVector.end(), phraseVector.begin(), phraseVector.end());

    } else {
        // means both phase index and alphabet is present
        string phraseIndex = encodedString.substr(0, encodedString.length() - 1);
        string alphabet = encodedString.substr(encodedString.length() - 1, MAX_BITS);
        cout<<"phraseIndex: "<<phraseIndex<< " alphabet: "<<alphabet<< endl;
        int phraseIndexBits;
        phraseIndexBits = log2(encodingTable.size() - 1) + 1;
        int phraseIndexInt = stoi(phraseIndex);
        vector<int> phraseVector = intToBits(phraseIndexInt, phraseIndexBits);
        binaryStringVector.insert(binaryStringVector.end(), phraseVector.begin(), phraseVector.end());
        vector<int> alphabetVector = intToBits(alphabetMap[alphabet[0]], MAX_BITS);
        binaryStringVector.insert(binaryStringVector.end(), alphabetVector.begin(), alphabetVector.end());
    }

    return binaryStringVector;
}

int bit_count(int number) {
    int count = 0;
    while (number != 0) {
        count++;
        number >>= 1;
    }
    return count;
}

int binaryStringToInt(const string& binaryString) {
    int result = 0;
    int power = 1;
    for (int i = binaryString.size() - 1; i >= 0; i--) {
        if (binaryString[i] == '1') {
            result += power;
        }
        power *= 2;
    }
    return result;
}

string decompress(string s) {
    cout<<"decompress"<<endl;
    string decompressedString = "";
    int currBits;
    int curr = 0;
    int i = 1;
    int lastPhraseIndex = 0;
    while (1)
    {
        if(i == 1 || i == 2) {
            currBits = 1;
        }
        else {
            currBits = log2(i-1) + 1;
        }
        string currPhrase = s.substr(curr, currBits);
        int currPhraseInt = binaryStringToInt(currPhrase);
        curr = curr + currBits;
        string alphabetPhrase = s.substr(curr , MAX_BITS);
        int alphabetPhraseInt = binaryStringToInt(alphabetPhrase);

        cout<<currPhrase << " " << alphabetPhrase << endl;

        // construct phrase table
        string lastSymbol = "" + reverseAlphabetMap[alphabetPhraseInt];
        if(currPhraseInt == 0) {
            decompressedString = decompressedString + lastSymbol;
            reversePhraseTable[i] = lastSymbol;
        }
        else {
            string prevPhrase = reversePhraseTable[currPhraseInt];
            decompressedString = decompressedString + prevPhrase + lastSymbol;
            reversePhraseTable[i] = prevPhrase + lastSymbol;
        }

        // check if curr is equal to s.length()
        string nextPhrase = s.substr(curr + 1, s.length() - curr);
        if(nextPhrase.length() <= currBits + MAX_BITS) {
            break;
        }

        curr = curr + MAX_BITS;
        i++;
    }


    // add last phrase
    string binaryLastPhrase = s.substr(curr, s.length() - curr);
    
    cout<<"binaryLastPhrase: "<<binaryLastPhrase<<endl;

    if (binaryLastPhrase.length() > currBits + MAX_BITS) {
        cout<<"both phase index and alphabet is present"<<endl;
        // means both phase index and alphabet is present
        string currPhrase = binaryLastPhrase.substr(0, currBits);
        int currPhraseInt = binaryStringToInt(currPhrase);
        string alphabetPhrase = binaryLastPhrase.substr(currBits, MAX_BITS);
        int alphabetPhraseInt = binaryStringToInt(alphabetPhrase);

        string prevPhrase = reversePhraseTable[currPhraseInt];
        decompressedString = decompressedString + prevPhrase + reverseAlphabetMap[alphabetPhraseInt];
    } else {
        cout<<"only phase index is present without alphabet"<<endl;
        // means only phase index is present without alphabet
        int currPhraseInt = binaryStringToInt(binaryLastPhrase);
        decompressedString = decompressedString + reversePhraseTable[currPhraseInt];
    }

    cout<<"decompressedString: "<<decompressedString<<endl;

    return decompressedString;
}


int main()
{
    createAlphabetMap(); // A -> 0 B -> 1 C -> 2 etc
    createReverseAlphabetMap(); // 0 -> A 1 -> B 2 -> C etc


    //TODO: read file and convert to string

    // AABABBBABAABABBBABBABX
    // string s = "AABABBBABAABABBBABBABB";

    // load string from file
    ifstream file("input.txt");
    string s;
    getline(file, s);
    file.close();


    cout<<"Enter the choice: "<<endl;
    cout<<"1. Compression"<<endl;
    cout<<"2. Decompression"<<endl;
    int choice;
    cin>>choice;
    string finalCompressedString = "";
    if(choice == 1) {
        // compress
        cout<<"Original string: "<<s<<endl;
        getEncodingTable(s);
        cout<<"Encoding table: "<<endl;
        // iterate over encoding table 
        for(int i = 0; i < encodingTable.size(); i++) {
            phraseItem item = encodingTable[i];
            cout<<item.phraseIndex<<" "<<item.phrase<<" "<<item.encodedString<<endl;
            reversePhraseTable[item.phraseIndex] = item.phrase;
        }

        string finalCompressedString = "";

        vector<int> binaryStringVector = getBinaryString();

        // convert binaryStringVector to string
        for(int i = 0; i < binaryStringVector.size(); i++) {
            finalCompressedString += to_string(binaryStringVector[i]);
        }

        cout<<"Final compressed string: "<<finalCompressedString<<endl;

        // write finalCompressedString to file
        ofstream compressedFile;
        compressedFile.open("compressed.txt");
        compressedFile<<finalCompressedString;
        compressedFile.close();

    }
    else if(choice == 2) {
        // read file
        string compressedString;

        ifstream compressedFile;
        compressedFile.open("compressed.txt");
        compressedFile>>compressedString;
        compressedFile.close();

        // decompress

        cout<<"Compressed string: "<<compressedString<<endl;

        string decompressedString = decompress(compressedString);

        cout<<"Compressed string: "<<compressedString<<endl;
        cout<<"Decompressed string: "<<decompressedString<<endl;
        // write decompressedString to file
        ofstream decompressedFile;
        decompressedFile.open("decompressed.txt");
        decompressedFile<<decompressedString;
        decompressedFile.close();


    } else {
        cout<<"Invalid choice"<<endl;
    }
    return 0;
}
