/* Luis de Santos */

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <utility>
#include "encodingTree.h"
#define NOT_A_CHAR 0x80
#define EOFC 0x81;

using namespace std;

void _buildCode(HCNode* node, unordered_map<unsigned char, string>& m, const string& code){
  if(node == nullptr)
    return;
  if(node->letter != NOT_A_CHAR){
    m[node->letter] = code;
    return;
  }
  _buildCode(node->zero, m, code + "0");
  _buildCode(node->one, m, code + "1");
}

HCNode* buildEncodingInfo(string input){
	ifstream infile(input);
	if(!infile.is_open()){
		cout << "Failed to open file!" << endl;
		return nullptr;
	}
	unordered_map<unsigned char, unsigned int> myMap;	
	char c;
	while(infile.get(c)){
		myMap[(unsigned char) c]++;
	}

	priorityqueue pq;
  vector<unsigned char> keys;
	for(auto it = myMap.begin(); it != myMap.end(); it++){
		pq.insert(new HCNode(it->first, it->second));
    keys.push_back(it->first);
	}

  while(pq.size() > 1){
    HCNode* first = new HCNode(pq.pop());
    HCNode* second = new HCNode(pq.pop());
    HCNode* parent = new HCNode(NOT_A_CHAR, first->frequency + second->frequency);
    parent->one = first;
    parent->zero = second;
    pq.insert(parent);
  }

  HCNode* root = new HCNode(pq.pop());

  ofstream myfile;
  myfile.open(input += ".hi");
  unsigned char start = 0x0;
  unordered_map<unsigned char, string> codeMap;
  while(start != 0x80){
      unsigned int dec = start;
    if(myMap.count(start) > 0){
      string code = "";
      _buildCode(root, codeMap, code);
      myfile << dec << "    " << code + codeMap[start] << endl;
    }else{
      myfile << dec << "    " << 0 << endl;
    }
    start++;
  }

  myfile.close();
  return root;
}

HCNode* importHuffmanCode(string filename){
  ifstream infile(filename);
  if(!infile.is_open()){
    cout << "Failed to open file!" << endl;
    return nullptr;
  }
  string s;
  vector<pair<unsigned int, string> > v;
  while(getline(infile, s)){
    stringstream ss(s);
    string left, right;
    ss >> left >> right;
    if(right != "0"){
      v.push_back(make_pair(stoi(left), right));
    }
  }
  infile.close();
  encodingTree t(nullptr);
  t.buildEncodingTree(v);

  return t.getRoot();
}


void compressFile(const std::string& inputFilename, HCNode* root) {
    ifstream check(inputFilename + ".hi");
    if(!check.is_open() && root == nullptr){
      cout << "Huffman encoding has not been done!" << endl;
      check.close();
      return;
    }
    check.close();

    // Determine the output filename
    std::string outputFilename = inputFilename + ".hc";

    // Open the input file
    std::ifstream inputFile(inputFilename, std::ios::binary);

    // Check if the input file is open
    if (!inputFile.is_open()) {
        std::cout << "Error: Failed to open input file." << std::endl;
        return;
    }

    // Open the output file
    std::ofstream outputFile(outputFilename, std::ios::binary);

    // Check if the output file is open
    if (!outputFile.is_open()) {
        std::cout << "Error: Failed to create output file." << std::endl;
        inputFile.close();
        return;
    }

    //Read input file character by character and write Huffman codes to output file
    char c;
    unsigned char bitBuffer = 0;
    int bitsWritten = 0;

    // Create an instance of HuffmanTree
    encodingTree t(root);

    int numChars = 0;
    int totalWritten = 0;
    while (inputFile.get(c)) { // Read character by character
        numChars++;
        // Write the code to count the number of bits read in from the inputfile
    
        std::string huffmanCode = "";
        t.getHuffmanCode(root, c, huffmanCode, huffmanCode); // Get encoding

        for (char bit : huffmanCode) {
            // Set the corresponding bit in the bit buffer
            bitBuffer |= ((bit - '0') << (7 - bitsWritten));
            bitsWritten++;

            // If the bit buffer is full, write it to the output file
            if (bitsWritten == 8) {
                outputFile.write(reinterpret_cast<char*>(&bitBuffer), sizeof(bitBuffer));
                totalWritten++;

                // Reset the bit buffer and bitsWritten
                bitBuffer = 0;
                bitsWritten = 0;
            }
        }
    }

    // Write any remaining bits in the bit buffer to the output file
    if (bitsWritten > 0) {
        outputFile.write(reinterpret_cast<char*>(&bitBuffer), sizeof(bitBuffer));
        totalWritten++;
    }

    // Close input and output files
    inputFile.close();
    outputFile.close();

    std::cout << "File compressed successfully. Output file: " << outputFilename << std::endl;
    cout << "Number of characters in the original file: " << numChars << endl;
    cout << "Number of character in the compressed file: " << totalWritten << endl;
    cout << "Compression ratio: " << (float)numChars / (float)totalWritten << endl; 
    cout << "Space saved: " << (1.0 - ((float)totalWritten / (float)numChars)) * 100.0 << "%" << endl;
}

void decompressFile(const std::string& compressedFile, HCNode* root) {
    std::ifstream inFile(compressedFile, std::ios::binary);
    if (!inFile.is_open() || root == nullptr) {
        std::cerr << "Error opening compressed file: " << compressedFile << std::endl;
        return;
    }
    string output = compressedFile + ".decompressed";
    std::ofstream outFile(output, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error creating output file: " << output << std::endl;
        inFile.close();
        return;
    }

    unsigned char byte;
    HCNode* currentNode = root;
    bool lastBit = false;
    while (inFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        for (int i = 7; i >= 0; i--) {
            bool bit = (byte >> i) & 1;
            if (bit == 0 && currentNode->zero != nullptr) {
                currentNode = currentNode->zero;
            } else if (bit == 1 && currentNode->one != nullptr) {
                currentNode = currentNode->one;
            }
            if (currentNode && currentNode->letter != NOT_A_CHAR) {
                outFile << currentNode->letter;
                currentNode = root;
            }
        }
    }

    // Handle end-of-file
    if (!lastBit && currentNode->letter != NOT_A_CHAR) {
        outFile << currentNode->letter;
    }

    inFile.close();
    outFile.close();
}

void displayCommands(){
  cout << "\nOperation are given by digits 1 through 5\n\n";
  cout << "  1 <filename> - create a new Huffman Information file from an original file\n";
  cout << "  2 <filename> - load a Huffman Information file \n";
  cout << "  3 <filename> - compress a file using the current Huffman Information file\n";
  cout << "  4 <filename> - decompress a file using the current Huffman Information file\n";
  cout << "  5            - quit the program\n\n";

}

int main(int argc, char** argv){

  cout << "Welcome to File Compression program\n";

  displayCommands();
  encodingTree loaded;
  
  bool done = false;
  string line;
  string input;

  do {
    cout << "cmd> ";

    // read a complete line
    std::getline(std::cin, line);

    // now create a "stringstream" on the line just read
    std::stringstream ss(line);

    // clear input from any previous value
    input = "";

    // get command character
    char command;
    ss >> command;
    //cout << "Debug command:" << command << "***\n";
    
    if(command == '1'){
        ss >> input;
        // build a new .hi file using the information in the file: input
		buildEncodingInfo(input);
    }

    if(command == '2'){
        ss >> input;
        // load a .hi file to later perform compression and decompression
        loaded = importHuffmanCode(input);
    }
        
    if(command == '3'){
        ss >> input;
        // use the current .hi file to compress the file in input
        compressFile(input, loaded.getRoot());
    }

    if(command == '4'){
        ss >> input;
        // use the current .hi file to decompress the file in input
        // note if the file was not compressed using the same .hi file, the output will be meaningless
        decompressFile(input, loaded.getRoot());
    }

    if(command == '5' || command == 'q'){
        done = true;
    }


  } while(!done && !cin.eof());

  return 0;
}
