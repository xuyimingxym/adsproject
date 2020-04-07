/*
	COP 5536 Advanced Data Structures Project
	Author: Yiming Xu
	UFID#: 9946-2650
	Email: yiming.xu@ufl.edu
*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <string>
#include <io.h>
#include <map>
using namespace std;

/******************************/
/* Fibonacci Heap Node class */
/******************************/
template <class T>
class FibNode {
	public :
		T key;				// key
		int degree;			// degree
		FibNode<T>* left;	// left sibling
		FibNode<T>* right;	// right sibling
		FibNode<T>* child;	// child
		FibNode<T>* parent;	// parent
		bool ChildCut;		// ChildCut
		string hashtag;		// hashtag

		// constructor
		FibNode(T value, string tag) : key(value), hashtag(tag), degree(0), left(NULL), right(NULL), child(NULL),
			parent(NULL), ChildCut(false) {
			key = value;
			degree = 0;
			left = this;
			right = this;
			parent = NULL;
			child = NULL;
			ChildCut = false;
			hashtag = tag;
		}	
};

/******************************/
/*    Fibonacci Heap class    */
/******************************/
template <class T>
class FibHeap {
	public:
		int keynum;			// number of keys in FibHeap
		int maxdegree;		// max degree
		FibNode<T>* max;	// max node
		FibNode<T>** cons;	// memory area

		void insertNode(FibNode<T>* node);				  // insert new node to FibHeap
		void addNode(FibNode<T>* node, FibNode<T>* root); // add a node to list (before root node: -node-root-) 
		void meldlist(FibNode<T>* r1, FibNode<T>* r2);	  // meld list 
		void removeNode(FibNode<T>* node);				  // remove node from doubly linked list 
		void makeCon();									  // request space for consolidate
		void consolidate();								  // pairwise combine
		FibNode<T>* Maxtree();							  // extract max tree
		void link(FibNode<T>* node, FibNode<T>* root);    // link node to root
		void renewDegree(FibNode<T>* parent, int degree); // renew degrees of nodes in the tree
		void cut(FibNode<T>* node, FibNode<T>* parent);	  // cut node from parent node, put it in root list
		void cascadingCut(FibNode<T>* node);			  // cascading cut
		string getMaxTag();								  // get hashtage of max

	public:			// The following functions are discussed in class
		FibHeap();			// constructor		
		~FibHeap();			// destructor
		void insert(T key, string tag);					// insert new key with hashtag		
		void meld(FibHeap<T>* otherheap);				// meld with other FibHeap		
		void remove(FibNode<T>* node);					// remove theNode from FibHeap
		void removeMax();								// remove Maximum node
		void increasekey(FibNode<T>* node, T key);		// increase key of the node
		void decreasekey(FibNode<T>* node, T key);		// increase key of the node

};

/* constructor */
template <class T>
FibHeap<T>::FibHeap()
{
	keynum = 0;
	maxdegree = 0;
	max = NULL;
	cons = NULL;
}

/* destructor */
template <class T>
FibHeap<T>::~FibHeap() {}

/* remove node from doubly linked list */
template <class T>
void FibHeap<T>::removeNode(FibNode<T>* node)
{
	node->left->right = node->right;
	node->right->left = node->left;
}

/* add a node to list (before max node) */
template <class T>
void FibHeap<T>::addNode(FibNode<T>* node, FibNode<T>* root)
{
	node->left = root->left;
	root->left->right = node;
	node->right = root;
	root->left = node;
}

/* insert new node to FibHeap */
template <class T>
void FibHeap<T>::insertNode(FibNode<T>* node) 
{
	if (keynum == 0)
		max = node;
	else
	{
		addNode(node, max);
		if (node->key > max->key)
			max = node;
	}
	keynum++;
}

/* insert new key with hashtag */
template <class T>
void FibHeap<T>::insert(T key, string tag) 
{
	FibNode<T>* node;
	node = new FibNode<T>(key, tag);
	if (node == NULL)
		return;
	insertNode(node);
}

/* meld list */
template<class T>
void FibHeap<T>::meldlist(FibNode<T>* r1, FibNode<T>* r2)
{
	FibNode<T>* temp;
	temp = r1->right;
	r1->right = r2->right;
	r2->right->left = r1;
	r2->right = temp;
	temp->left = r2;
}

/* meld with other FibHeap */
template<class T>
void FibHeap<T>::meld(FibHeap<T>* otherheap) 
{
	if (otherheap == NULL)
		return;
	if (otherheap->maxdegree > this->maxdegree)
		swap(*this, *otherheap);
	if ((this->max) == NULL)
	{
		this->max = otherheap->max;
		this->keynum = otherheap->keynum;
		free(otherheap->cons);
		delete otherheap;
	}
	else if ((otherheap->max) == NULL)
	{
		free(otherheap->cons);
		delete otherheap;
	}
	else
	{
		meldlist(this->max, otherheap->max);
		if (this->max->key < otherheap->max)
			this->max = otherheap->max;
		this->keynum += otherheap->keynum;
		free(otherheap->cons);
		delete otherheap;
	}
}

/* extract max tree */
template <class T>
FibNode<T>* FibHeap<T>::Maxtree()
{
	FibNode<T>* p = max;
	if (p == p->right)
		max = NULL;
	else
	{
		removeNode(p);
		max = p->right;
	}
	p->left = p;
	p->right = p;
	return p;
}

/* link node to root */
template <class T>
void FibHeap<T>::link(FibNode<T>* node, FibNode<T>* root)
{
	removeNode(node);
	if (root->child == NULL)
		root->child = node;
	else
		addNode(node, root->child);
	node->parent = root;
	
	root->degree++;
	node->ChildCut = false;
}

/* make space for consolidate */
template <class T>
void FibHeap<T>::makeCon()
{
	int old = maxdegree;
	maxdegree = (log(keynum) / log(2.0)) + 1;
	if (old >= maxdegree)
		return;
	FibNode<T>** tmp;
	cons = (FibNode<T> * *)malloc(sizeof(FibHeap<T>*) * (maxdegree + 1) * 8);
	if (cons != NULL)
	{
		tmp= (FibNode<T> * *)realloc(cons, sizeof(FibHeap<T>*) * (maxdegree + 1) * 8);
		if (tmp != NULL)
		{
			cons = tmp;
		}
	}
}



/* pairwise combine */
template<class T>
void FibHeap<T>::consolidate()
{
	int i, d, D;
	FibNode<T> *x, *y, *tmp;
	makeCon();
	D = maxdegree + 1;
	for (i = 0; i < D; i++)
		cons[i] = NULL;
	while (max != NULL)		//combine trees with same degree
	{
		x = Maxtree();
		//if (x->degree < 0) // bug 
		//	x->degree = 0;
		d = x->degree;
		while (cons[d] != NULL)
		{
			y = cons[d];
			if (x -> key < y->key)
				swap(x, y);
			link(y, x);
			cons[d] = NULL;
			d++;
		}
		cons[d] = x;
	}
	max = NULL;
	
	for (i = 0; i < D; i++)		//add nodes in cons to list 
	{
		if (cons[i] != NULL)
		{
			if (max == NULL)
				max = cons[i];
			else
			{
				addNode(cons[i], max);
				if ((cons[i])->key > max->key)
					max = cons[i];
			}
		}
	}
}

/* remove maximum */
template <class T>
void FibHeap<T>::removeMax()
{
	if (max == NULL)
		return;
	FibNode<T>* child = NULL;
	FibNode<T>* m = max;
	while (m->child != NULL)  // add children of max to list
	{
		child = m->child;
		removeNode(child);
		if (child->right == child)
			m->child = NULL;
		else
			m->child = child->right;
		addNode(child, max);
		child->parent = NULL;
	}
	removeNode(m); //remove max from list
	if (m->right == m)
		max = NULL;
	else
	{
		max = m->right;
	}
	keynum--;
	delete m;

	FibNode<int>* temp = max;	
	while (max!=NULL && temp->right != max)
	{
		temp = temp->right;
		if (temp->key > max->key)
		{
			max = temp;
		}
	}
}

/* renew degrees of nodes in the tree */
template <class T>
void FibHeap<T>::renewDegree(FibNode<T>* parent, int degree)
{
	parent->degree -= degree;
	if (parent->parent != NULL)
		renewDegree(parent->parent, degree);
}

/* cut node from parent node, put it in root list */
template <class T>
void FibHeap<T>::cut(FibNode<T>* node, FibNode<T>* parent)
{
	removeNode(node);
	renewDegree(parent, node->degree);
	if (node == node->right)
		parent->child = NULL;
	else 
		parent->child = node->right;
	node->parent = NULL;
	node->left = node->right = node;
	node->ChildCut = false;
	addNode(node, max);
}

/* cascading cut */
template <class T>
void FibHeap<T>::cascadingCut(FibNode<T>* node)
{
	FibNode<T>* parent = node->parent;
	if (parent != NULL)
	{
		if (node->ChildCut == false)
			node->ChildCut = true;
		else
		{
			cut(node, parent);
			cascadingCut(parent);
		}
	}
}

/* increase key */
template <class T>
void FibHeap<T>::increasekey(FibNode<T>* node, T key)
{
	FibNode<T>* parent;
	if (max == NULL || node == NULL)
		return;
	if (key <= node->key)
		return;
	node->key = key;
	parent = node->parent;
	if (parent != NULL && node->key > parent->key)
	{
		cut(node, parent);
		cascadingCut(parent);
	}
	if (node->key > max->key)
		max = node;
}

/* decrease key */
template <class T>
void FibHeap<T>::decreasekey(FibNode<T>* node, T key)
{
	FibNode<T>* child, * parent, * right;
	if (max == NULL || node == NULL)
		return;
	if (key >= node->key)
		return;
	while (node->child != NULL)
	{
		child = node->child;
		removeNode(child);              
		if (child->right == child)
			node->child = NULL;
		else
			node->child = child->right;
		addNode(child, max);       
		child->parent = NULL;
	}
	node->degree = 0;
	node->key = key;
	parent = node->parent;
	if (parent != NULL)
	{
		cut(node, parent);
		cascadingCut(parent);
	}
	else if (max == node)
	{
		right = node->right;
		while (right != node)
		{
			if (node->key < right->key)
				max = right;
			right = right->right;
		}
	}
}

/* remove theNode */
template <class T>
void FibHeap<T>::remove(FibNode<T>* node)
{
	T tmp = max->key + 1;
	increasekey(node, tmp);
	removeMax();
}

template <class T>
string FibHeap<T>::getMaxTag()
{
	if (max == NULL)
		return "";
	return max->hashtag;

}


/******************************/
/*    Hashtable    */

/* Add data to hashtable */
void addData(string tag,string count, FibHeap<int>* heap,  map<string, FibNode<int>*>* pointer)
{
	string hashtag = tag;
	int key = stoi(count);
	map<string, FibNode<int>*> hashtable;
	hashtable = *pointer;
	if (pointer->find(hashtag) != pointer->end())
	{
		// hashtag found in hashtable, corresponding node increase key
		//cout << "found" << endl;

		heap->increasekey(hashtable[tag], hashtable[tag]->key + key);
	}
	else
	{
		// hashtag not in hashtable, insert hashtage, insert new node to FibHeap
		//cout << "not found" << endl;
		FibNode<int>* node = new FibNode<int>(key, hashtag);
		pointer->insert(make_pair(hashtag, node));
		heap->insertNode(node);
	}
	//cout <<"add data"<< heap->max->degree << endl;
}

/* write output file */
void writefile(string filename, string info)
{
	ofstream tofile;
	if (!tofile) cout << "error" << endl;
	tofile.open(filename, ios_base::app);
	tofile << info << endl;
	tofile.close();
}

/* extract top n hashtag */
string n_max(int n, FibHeap<int>* heap, map<string, FibNode<int>*> hashtable, map<string,FibNode<int>*>* pointer)
{
	string output = heap->max->hashtag;
	int keys[20];
	string tags[20];
	keys[0] = heap->max->key;
	tags[0] = heap->max->hashtag;
	for (int i = 1; i < n; i++)
	{
		pointer->erase(heap->max->hashtag);
		heap->removeMax();
		tags[i] = heap->max->hashtag;
		keys[i] = heap->max->key;
		output = output + "," + heap->max->hashtag;
	}
	for (int i=0; i < n-1; i++)
	{
		addData(tags[i], to_string(keys[i]), heap, pointer);
	}
	return output;
}


int main()
{	
	FibHeap<int> fh;									// FibHeap initialize
	FibHeap<int>* heap = new FibHeap<int>();			// FibHeap pointer
	heap = &fh;
	map<string, FibNode<int>*> hashtable;				// Hashtable initialize
	map<string, FibNode<int>*>* pointer = &hashtable;	// Hashtable pointer
	string inputfile, outputfile, command;
	getline(cin, command);				// get command
	istringstream iss1(command);
	iss1 >> inputfile >> outputfile;	// split command

	/* read file line by line */
	ifstream infile;
	infile.open(inputfile);
	if (!infile) cout << "error" << endl;
	string line, tag, count, MaxOutput;

	while (getline(infile,line))
	{
		istringstream iss(line);
		string a, b;
		iss >> a >> b;
		if (a == string("stop"))		// input line with the word “stop”, close infile, terminate program 
		{
			infile.close();
			return 0;
		}
		else if (a[0] == '#')			// input line with hashtag
		{
			tag = a.substr(1);
			count = b;
			addData(tag, count, heap, pointer);
		}
		else if (a[0]>='0' && a[0]<='9')	// input line with query
		{
			int n = stoi(a);
			MaxOutput = n_max(n,heap,hashtable,pointer);
			if (outputfile.length()!=0)
				writefile(outputfile, MaxOutput);	// write to file
			else
				cout << MaxOutput << endl;			// out put in console
		}
		
	}
	infile.close();
}






