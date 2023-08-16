#include "encodingTree.h"
#define NOT_A_CHAR 0x80

encodingTree::encodingTree(){
		root = nullptr;
}

encodingTree::encodingTree(HCNode* node){
		root = node;
}

HCNode* encodingTree::getRoot(){return root;}
std::vector<unsigned char> encodingTree::getKeys(){return keys;}

HCNode* encodingTree::buildEncodingTree(std::vector<std::pair<unsigned int, std::string> > v){
	root = new HCNode(NOT_A_CHAR, 0);
	for(const auto& huffmanCode : v){
		unsigned char character = huffmanCode.first;
		const std::string& code = huffmanCode.second;
		HCNode* trav = root;	
		for(int i = 0; i < code.length(); i++){
			char bit = code[i];
			if(bit == '0'){
				if(!trav->zero)
					trav->zero = new HCNode(NOT_A_CHAR, 0);
				trav = trav->zero;
			}else{
				if(!trav->one)
					trav->one = new HCNode(NOT_A_CHAR, 0);
				trav = trav->one;
			}
		}
		trav->letter = character;
		trav->frequency++;
	}
	root->frequency = root->zero->frequency + root->one->frequency;
	return root;
}

/* Builds the appropriate huffman encodingTree
 * Left means zero, right means 1
 */
void encodingTree::getHuffmanCode(HCNode* root, unsigned int key, const std::string& path, std::string& result) {
    if (root == nullptr) {
        return; // Key not found, return -1
    }

    if (root->letter == key) {
		result = path;
        return; // Key found, return the path
    }
	getHuffmanCode(root->zero, key, path + "0", result); // Traverse left, append '0' to the path
	getHuffmanCode(root->one, key, path + "1", result); // Traverse right, append '1' to the path
}
