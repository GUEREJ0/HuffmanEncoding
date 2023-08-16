#ifndef ENCODINGTREE_H
#define ENCODINGTREE_H

#include "priorityqueue.h"
#include <utility>
#include <string>

class encodingTree{
	private:
		HCNode* root;
		std::vector<unsigned char> keys;

	public:
		encodingTree();
		encodingTree(HCNode* node);
		HCNode* getRoot();
		std::vector<unsigned char> getKeys();
		HCNode* buildEncodingTree(std::vector<std::pair<unsigned int, std::string> > v);
		void getHuffmanCode(HCNode* root, unsigned int key, const std::string& path, std::string& result);
};

#endif
