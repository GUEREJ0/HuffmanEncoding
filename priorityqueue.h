#include <vector>
#include <algorithm>
#include <iostream>

struct HCNode{
	unsigned char letter;
	unsigned int frequency;
	HCNode* one;
	HCNode* zero;

	HCNode(unsigned char l, unsigned int f){
		letter = l;
		frequency = f;
		one = nullptr;
		zero = nullptr;
	}
};


struct Compare{
	bool operator()(const HCNode& a, const HCNode& b){return a.frequency > b.frequency;}
};

class priorityqueue{
	private:
		HCNode* front;
		std::vector<HCNode> q; // Thinking of storing our queue in an array, and front
							   // just points to the front of the array.

	public:
		priorityqueue(){
			front = nullptr;
		}

		int size(){return q.size();}

		HCNode pop(){ // Remove
			if(q.size() == 0){
				throw std::runtime_error("Priority queue is empty.");
			}

			pop_heap(q.begin(), q.end(), Compare());
			HCNode top = q.back();
			q.pop_back();

			return top;
		}

		void insert(HCNode* node){
			q.push_back(*node);
			push_heap(q.begin(), q.end(), Compare());
		}

		void print(){
			priorityqueue temp = *this;
			while(temp.size() > 0){
				HCNode p = temp.pop();
				int asciiValue = static_cast<int>(p.letter);
				std::cout << asciiValue << " | " << p.letter << " | " << p.frequency << std::endl;
			}	
		}
};
