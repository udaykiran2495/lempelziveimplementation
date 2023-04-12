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

const int MAX_BITS = 6; // max bits for encoding

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
    for(int i = 0; i < 65; i++) {
        alphabetMap[c] = i;
        c++;
    }
    alphabetMap['.'] = 26;
    alphabetMap[' '] = 27;
    alphabetMap['?'] = 28;
    alphabetMap[','] = 29;
    alphabetMap[':'] = 30;
    alphabetMap[';'] = 62;

}

void createReverseAlphabetMap() {
    char c = 'A';
    for(int i = 0; i < 65; i++) {
        reverseAlphabetMap[i].append(1, c);
        c++;
    }
    reverseAlphabetMap[26] = '.';
    reverseAlphabetMap[27] = ' ';
    reverseAlphabetMap[28] = '?';
    reverseAlphabetMap[29] = ',';
    reverseAlphabetMap[30] = ':';
    reverseAlphabetMap[62] = ';';

}

void constructEncodingTable() {
    cout<<"constructEncodingTable: "<<endl;
    for(int i = 0; i < encodingTable.size()   ; i++) {
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
    return;
}

void getEncodingTable(string s) {
    cout<<"getEncodingTable: "<<endl;
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
    cout<<"getBinaryString: "<<endl;
    vector<int> binaryStringVector;
    for(int i = 0; i < encodingTable.size(); i++) {
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
    while (curr < s.length())
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

        if(alphabetPhrase == "") {
            decompressedString = decompressedString + reversePhraseTable[currPhraseInt];
            break;
        }

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
        curr = curr + MAX_BITS;
        i++;
    }
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
    ifstream ifs("input.txt");
    string s((istreambuf_iterator<char>(ifs)),
                 (istreambuf_iterator<char>()));


    cout<<"Enter the choice: "<<endl;
    cout<<"1. Compression"<<endl;
    cout<<"2. Decompression"<<endl;
    int choice;
    cin>>choice;
    string finalCompressedString = "";
    if(choice == 1) {
        // compress
        cout<<"compress"<<endl;
        getEncodingTable(s);
        // iterate over encoding table 
        for(int i = 0; i < encodingTable.size(); i++) {
            phraseItem item = encodingTable[i];
            reversePhraseTable[item.phraseIndex] = item.phrase;
        }

        string finalCompressedString = "";
        vector<int> binaryStringVector = getBinaryString();
        // convert binaryStringVector to string
        for(int i = 0; i < binaryStringVector.size(); i++) {
            finalCompressedString += to_string(binaryStringVector[i]);
        }

        ofstream compressedFile("compressed.txt");
        compressedFile<<finalCompressedString;
    }
    else if(choice == 2) {
        // read file

        ifstream ifs("compressed.txt");
        string compressedString((istreambuf_iterator<char>(ifs)),
                 (istreambuf_iterator<char>()));
        string decompressedString = decompress(compressedString);

        // write decompressedString to file
        ofstream decompressedFile("decompressed.txt");
        decompressedFile<<decompressedString;
    } else {
        cout<<"Invalid choice"<<endl;
    }
    return 0;
}
