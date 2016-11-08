#ifndef BST_H
#define BST_H
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include "node.h"
#include <vector>
#include <cmath>

using namespace std;
using std::cin;
using std::endl;
using std::pow;
using std::vector;
using std::ostream;
using std::istream;

enum BST_ERRORS{EMPTY};

template<typename T>
class bst
{
    public:
        typedef void (bst<T>::*fptr)(ostream& out, unsigned int where) const;
        bst();
        ~bst();
        bst(const bst<T> &other);
        bst<T>& operator=(const bst<T> &other);
        bool empty() const;
        unsigned int count(unsigned int where);
        bool search(const T &data);
        void insert(const T &data, unsigned int count, unsigned int line, unsigned int paragraph);
        node<T>* deleteInOrder();
        const int getDepth();
        const vector<node<T>*>* getTree(int n);
        void clear();

        template<typename U>
        friend
        ostream& operator<<(ostream& out, bst<U> &tree);

        template<typename U>
        friend
        istream& operator>>(istream& in, bst<U> &tree);

    private:
        vector<node<T>*> vec;
        unsigned int depth;

        void nukem();
        void insert(node<T>* &newNode, unsigned int where=1);
        void copy(unsigned int where, const vector<T> other);
        unsigned int find(const T& data, unsigned int where);
        unsigned int findRightMostChild(unsigned int where) const;
        unsigned int findLeftMostChild(unsigned int where) const;
        unsigned int findParent(unsigned int where, const T &data, WHICH_CHILD &which);
        int getDepth(unsigned int where);
        bool balanced(unsigned int subRoot);
        void rebalance(unsigned int parent);
        bool removeDepths();
        void rotateLeftChild(int n);
        void rotateRightChild(int n);
        void shiftSubTreeDownRight(int from, int to);
        void shiftSubTreeDownLeft(int from, int to);
        void shiftSubTreeUpRight(int from, int to);
        void shiftSubTreeUpLeft(int from, int to);
        void shiftSubTree(unsigned int from, unsigned int to);
};

template<typename T>
bst<T>::bst()
{
    depth = 0;
    vec.resize(0);
}

template<typename T>
bst<T>::~bst()
{
    nukem();
}

template<typename T>
bst<T>::bst(const bst<T> &other)
{
    depth = 0;
    for(int i = 1; i < other.vec.size(); ++i)
        if(other.vec[i])
            insert(other.vec[i]->getData(),other.vec[i]->getCount(),0,0);
}

template<typename T>
bst<T>& bst<T>::operator=(const bst<T> &other)
{
    if(this != &other)
    {
        nukem();
        for(int i = 1; i < other.vec.size(); ++i)
            if(other.vec[i])
            {
                node<T>* newNode = new node<T>(other.vec[i]->getData(),other.vec[i]->getCount(),0,0);
                insert(newNode, 1);
            }
    }
    return *this;
}

template<typename T>
void bst<T>::insert(const T &data, unsigned int count, unsigned int line, unsigned int paragraph)
{
    node<T>* newNode = new node<T>(data,count,line,paragraph);
    insert(newNode, 1);
}

template<typename T>
void bst<T>::insert(node<T>* &newNode, unsigned int where)
{
    if(vec.empty())
    {
        vec.resize(2);
        vec[1] = newNode;
    }
    else
    {
        if(vec[where]->getData() == newNode->getData())
        {
            vec[where]->setCount()=vec[where]->getCount()+newNode->getCount();
            vec[where]->addLine(newNode->getLines());
            vec[where]->addParagraph(newNode->getParagraphs());
        }
        else if(newNode->getData() < vec[where]->getData())
        {
            if((2*where)<vec.size() && !vec[2*where])
                vec[2*where] = newNode;
            else if(2*where >= vec.size())
            {
                ++depth;
                int size = vec.size();
                vec.resize(vec.size()+pow(2,depth));
                for(int i=size+1; i<size+pow(2,depth); ++i)
                    vec[i]=NULL;
                vec[2*where] = newNode;
            }
            else
                insert(newNode,2*where);
        }
        else
        {
            if((2*where+1)<vec.size() && !vec[2*where+1])
                vec[2*where+1] = newNode;
            else if (2*where >= vec.size())
            {
                ++depth;
                int size = vec.size();
                vec.resize(size+pow(2,depth));
                for(int i=size+1; i<size+pow(2,depth); ++i)
                    vec[i] = NULL;
                vec[2*where+1] = newNode;
            }
            else
                insert(newNode,2*where+1);
        }
    }
    rebalance(where);
}

template<typename T>
void copy(unsigned int where, const vector<T> other)
{
    insert(other[where]->getData(), other[where]->getCount());
}

template<typename T>
void bst<T>::nukem()
{
    vec.clear();
    depth = 0;
}

template<typename T>
unsigned int bst<T>::find(const T &data, unsigned int where)
{
    if(where < vec.size() && data == vec[where]->getData())
        return where;
    else if(data < vec[where]->getData() && 2*where < vec.size && vec[2*where])
        return find(data, 2*where);
    else if (data > vec[where]->getData() && (2*where+1) < vec.size && vec[2*where+1])
        return find(data,2*where+1);
    return 0;
}

template<typename T>
bool bst<T>::empty() const
{
    return !vec[1];
}

template<typename T>
bool bst<T>::search(const T &data)
{
    return find(data,1);
}

template<typename T>
node<T>* bst<T>::deleteInOrder()
{
    if(empty())
        return NULL;
    int least = findLeftMostChild(1);
    node<T>* removed = vec[least];
    if(2*least+1 < vec.size() && vec[2*least+1])
        shiftSubTreeUpLeft(2*least+1, least);
    else
        vec[least] = NULL;
    rebalance(1);
    while(removeDepths());
    return removed;
}

template<typename T>
void bst<T>::clear()
{
    vec.clear();
    depth = 0;
}

template<typename T>
const int bst<T>::getDepth()
{
    return depth;
}

template<typename T>
const vector<node<T>*>* bst<T>::getTree(int n)
{
    vector<node<T>*>* tree = &vec;
    return tree;
}

template<typename T>
int bst<T>::getDepth(unsigned int where)
{
    return (where<vec.size() && vec[where]) ? 1 + std::max(getDepth(2*where), getDepth(2*where+1)) : 0;
}

template<typename T>
bool bst<T>::balanced(unsigned int subRoot)
{
    return (subRoot >= vec.size()) ? true : (abs(getDepth(2*subRoot)-getDepth(2*subRoot+1))) <= 1;
}

template<typename T>
void bst<T>::rebalance(unsigned int parent)
{
    if(balanced(parent))
        return;
    if(getDepth(2*parent) > getDepth(2*parent+1))
    {
        if (getDepth(2*(2*parent)+1) > getDepth(2*2*parent))
        {
            rotateRightChild(parent*2);
            rotateLeftChild(parent);
        }
        else
            rotateLeftChild(parent);
    }
    else
    {
        if (getDepth(2*(2*parent+1)) > getDepth(2*(2*parent+1)+1))
        {
            rotateLeftChild(2*parent+1);
            rotateRightChild(parent);
        }
        else
            rotateRightChild(parent);
    }
}

template<typename T>
void bst<T>::rotateLeftChild(int n)
{
    shiftSubTreeDownRight(n,2*n+1);
    vec[n] = vec[2*n];
    if(2*(2*n)+1 < vec.size())
        shiftSubTree(2*(2*n)+1, 2*(2*n+1));
    if(2*2*n < vec.size() && vec[2*2*n])
        shiftSubTreeUpLeft(2*2*n,2*n);
    else
        vec[2*n] = NULL;
}

template<typename T>
void bst<T>::rotateRightChild(int n)
{
    shiftSubTreeDownLeft(n,2*n);
    vec[n] = vec[2*n+1];
    if(2*(2*n+1) < vec.size())
        shiftSubTree(2*(2*n+1),2*(2*n)+1);
    if(2*(2*n+1)+1 < vec.size() && vec[2*(2*n+1)+1])
        shiftSubTreeUpRight(2*(2*n+1)+1,2*n+1);
    else
        vec[2*n+1] = NULL;
}

template<typename T>
unsigned int bst<T>::findParent(unsigned int where, const T &data, WHICH_CHILD &which)
{
    if(vec[where]->getData() == data)
        return where;

    bool cont = true;
    while(where < vec.size() && cont)
    {
        if(2*where < vec.size() && vec[2*where]->getData() == data)
        {
            which = LEFT;
            cont = false;
        }
        else
        {
            if((2*where+1) < vec.size() && vec[2*where+1]->getData() == data)
            {
                which = RIGHT;
                cont = false;
            }
            else
                if(data < vec[where]->getData())
                    where = 2*where;
                else
                    where = 2*where+1;
        }
    }
    if(where >= vec.size())
        return 0;
    return where;
}

template<typename T>
unsigned int bst<T>::count(unsigned int where)
{
    unsigned int qty = 0;
    if(where<vec.size() && vec[where])
    {
        qty += vec[where]->getCount();
        qty += count(2*where);
        qty += count(2*where+1);
    }
    return qty;
}

template<typename T>
unsigned int bst<T>::findRightMostChild(unsigned int where) const
{
    unsigned int top = where;
    while((2*top+1)<vec.size() && vec[2*top+1])
        top = 2*top+1;
    return top;
}

template<typename T>
unsigned int bst<T>::findLeftMostChild(unsigned int where) const
{
    unsigned int top = where;
    while((2*top)<vec.size() && vec[2*top])
        top = 2*top;
    return top;
}

template<typename U>
ostream& operator<<(ostream& out, bst<U> &tree)
{
    if(tree.empty())
    {
        out << "Empty Tree!" << endl;
        return out;
    }
    while(!tree.empty())
        out<<*(node<U>*)tree.deleteInOrder()<<endl;
    return out;
}

template<typename U>
istream& operator>>(istream& in, bst<U> &tree)
{
    U input;
    in>>input;
    node<U>* newNode = new node<U>(input);
    tree.insert(newNode,1);
    return in;
}

template<typename T>
void bst<T>::shiftSubTreeUpLeft(int from, int to)
{
    int depths = getDepth(from);
    int subLevel = 0;
    for(; subLevel<depths; ++subLevel);

    int currentLevel = 0;
    while(currentLevel < subLevel)
    {
        int numNodes = pow(2,currentLevel);
        for(int i=0; i<numNodes; ++i)
            vec[to+i] = vec[from+i];
        from = 2*from;
        to = 2*to;
        ++currentLevel;
    }
    int numNodesLast = pow(2,currentLevel);
    for(int i = 0; i<numNodesLast; ++i)
        vec[to+i] = NULL;
}

template<typename T>
void bst<T>::shiftSubTreeUpRight(int from, int to)
{
    int depths = getDepth(from);
    int subLevel = 0;
    for(; subLevel < depths; ++subLevel);

    int currentLevel = 0;
    while(currentLevel < subLevel)
    {
        int numNodes = pow(2,currentLevel);
        for(int i = 0; i < numNodes; ++i)
            vec[to-i] = vec[from-i];
        from = 2*from+1;
        to = 2*to+1;
        ++currentLevel;
    }
    int numNodesLast = pow(2,currentLevel);
    for(int i = 0; i < numNodesLast; ++i)
        vec[to-i] = NULL;
}

template<typename T>
void bst<T>::shiftSubTreeDownLeft(int from, int to)
{
    int depths = getDepth(to);
    int where = to;
    int subLevel = 0;
    for(; subLevel<depths; ++subLevel, where=2*where);
    if(subLevel == 0)
        vec[to] = vec[from];
    else
    {
        while(where >= from && subLevel >= 0)
        {
            int numNodes = pow(2,subLevel);
            for(int i = 0; i < numNodes; ++i)
            {   if(i <= (numNodes-1)/2)
                    vec[where+i] = vec[(where/2)+i];
                else
                    vec[where+i] = NULL;
            }
            where = where/2;
            --subLevel;
        }
    }
}

template<typename T>
void bst<T>::shiftSubTreeDownRight(int from, int to)
{
    int depths = getDepth(to);
    int where = to;
    int subLevel = 0;
    for(; subLevel < depths; ++subLevel, where = 2*where+1);
    if(subLevel == 0)
        vec[to] = vec[from];
    else
    {
        while(where >= from && subLevel >= 0)
        {
            int numNodes = pow(2,subLevel);
            for(int i = 0; i < numNodes; ++i)
            {
                if(i <= (numNodes-1)/2)
                    vec[where-i] = vec[(where/2)-i];
                else
                    vec[where-i] = NULL;
            }
            where = where/2;
            --subLevel;
        }
    }
}

template<typename T>
void bst<T>::shiftSubTree(unsigned int from, unsigned int to)
{
    if(2*from < vec.size() && vec[2*from])
        shiftSubTree(2*from,2*to);
    if((2*from+1 < vec.size()) && (vec[2*from+1]))
        shiftSubTree(2*from+1,2*to+1);

    vec[to] = vec[from];
    vec[from] = NULL;
}

template<typename T>
bool bst<T>::removeDepths()
{
    if(!depth)
        return false;
    int numNodesLast = pow(2,depth);
    int lastLevel = 1+numNodesLast;
    for(int i = 0; i < numNodesLast-1; ++i)
    {
        if(vec[lastLevel+i])
            return false;
    }
    for(int i = 0; i < lastLevel; ++i)
        vec[lastLevel+i]=NULL;
    vec.resize(lastLevel);
    depth--;
    return true;
}

#endif //BST_H
