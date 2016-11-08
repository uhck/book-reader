#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <sstream>

using namespace std;

enum WHICH_CHILD {RIGHT,LEFT};

template<typename T>
class node
{
    public:
        node(const T &d = T(), unsigned int c=1, unsigned int line=0, unsigned int paragraph=0); //constructor
        ~node(); //deconstructor
        node(const node<T> &other); //copy constructor
        node<T>& operator=(const node<T> &other); //assignment operator (node)
        node<T>& operator+=(unsigned int v); //increment count by v
        node<T>& operator-=(unsigned int v); //decrement count by v

        unsigned int& setCount(); //sets count (left)
        const T& getData() const; //gets data (right)
        const unsigned int& getCount() const; //gets count (right)
        void addLine(unsigned int line);
        void addParagraph(unsigned int paragraph);
        unsigned int getLines();
        unsigned int getParagraphs();
        int operator++(); //increments variable by 1
        int operator++(int); //increments int by 1
        int operator--(); //decrements variable by 1
        int operator--(int); //decrements int by 1

    //Node vs Node bool functions

        template<typename U>
        friend
        bool operator<(const node<U>& x, const node<U> &y);

        template<typename U>
        friend
        bool operator<=(const node<U>& x, const node<U> &y);

        template<typename U>
        friend
        bool operator==(const node<U>& x, const node<U> &y);

        template<typename U>
        friend
        bool operator!=(const node<U>& x, const node<U> &y);

        template<typename U>
        friend
        bool operator>=(const node<U>& x, const node<U> &y);

        template<typename U>
        friend
        bool operator>(const node<U>& x, const node<U> &y);

    //Node vs Data

        template<typename U>
        friend
        bool operator<(const node<U>& x, const U &y);

        template<typename U>
        friend
        bool operator<=(const node<U>& x, const U &y);

        template<typename U>
        friend
        bool operator==(const node<U>& x, const U &y);

        template<typename U>
        friend
        bool operator!=(const node<U>& x, const U &y);

        template<typename U>
        friend
        bool operator>=(const node<U>& x, const U &y);

        template<typename U>
        friend
        bool operator>(const node<U>& x, const U &y);

    //Data vs Node

        template<typename U>
        friend
        bool operator<(const U& x, const node<U> &y);

        template<typename U>
        friend
        bool operator<=(const U& x, const node<U> &y);

        template<typename U>
        friend
        bool operator==(const U& x, const node<U> &y);

        template<typename U>
        friend
        bool operator!=(const U& x, const node<U> &y);

        template<typename U>
        friend
        bool operator>=(const U& x, const node<U> &y);

        template<typename U>
        friend
        bool operator>(const U& x, const node<U> &y);

    //Input/Output of Nodes

        template<typename U>
        friend
        ostream& operator<<(ostream &out, const node<U> &other);

        template<typename U>
        friend
        istream& operator>>(istream &in, node<U> &other);

    private:
        void* data;
        unsigned int count;
        vector<unsigned int> lines;
        vector<unsigned int> paragraphs;

        void copy(const node<T> &other); //helper copy function
        void nukem(); //helper deallocation
};

template<typename T>
node<T>::node(const T &d, unsigned int c, unsigned int line, unsigned int paragraph)
{
    data = new T(d);
    count=c;
    lines.push_back(line);
    paragraphs.push_back(paragraph);
}

template<typename T>
node<T>::~node()
{
    nukem();
}

template<typename T>
node<T>::node(const node<T> &other)
{
    copy(other);
}

template<typename T>
node<T>& node<T>::operator=(const node<T> &other)
{
    if(this != &other)
        copy(other);
    return *this;
}

template<typename T>
node<T>& node<T>::operator+=(unsigned int v)
{
    count += v;
    return *this;
}

template<typename T>
node<T>& node<T>::operator-=(unsigned int v)
{
    count -= v;
    return *this;
}

template<typename T>
const T& node<T>::getData() const
{
    return *(T*)data;
}

template<typename T>
const unsigned int& node<T>::getCount() const
{
    return count;
}

template<typename T>
unsigned int& node<T>::setCount()
{
    return count;
}

template<typename T>
int node<T>::operator++(int)
{
    count++;
    return count;
}

template<typename T>
int node<T>::operator++()
{
    unsigned int temp = count;
    count++;
    return temp;
}

template<typename T>
int node<T>::operator--(int)
{
    return --count;
}

template<typename T>
int node<T>::operator--()
{
    unsigned int temp = count;
    --count;
    return temp;
}

template<typename T>
void node<T>::addLine(unsigned int line)
{
    lines.push_back(line);
}

template<typename T>
void node<T>::addParagraph(unsigned int paragraph)
{
    paragraphs.push_back(paragraph);
}

template<typename T>
unsigned int node<T>::getLines()
{
    return lines[0];
}

template<typename T>
unsigned int node<T>::getParagraphs()
{
    return paragraphs[0];
}

template<typename T>
void node<T>::copy(const node<T> &other)
{
    *(T*)data = *(T*)other.data;
    count = other.count;
    lines = other.lines;
    paragraphs = other.paragraphs;
}

template<typename T>
void node<T>::nukem()
{
    *(T*)data = T();
    count = 0;
    lines.clear();
    paragraphs.clear();
}

//Node vs Node

template<typename U>
bool operator<(const node<U>& x, const node<U> &y)
{
    return *(U*)x.data < *(U*)y.data;
}

template<typename U>
bool operator<=(const node<U>& x, const node<U> &y)
{
    return *(U*)x.data <= *(U*)y.data;
}

template<typename U>
bool operator==(const node<U>& x, const node<U> &y)
{
    return *(U*)x.data == *(U*)y.data;
}

template<typename U>
bool operator!=(const node<U>& x, const node<U> &y)
{
    return x.data != y.data;
}

template<typename U>
bool operator>=(const node<U>& x, const node<U> &y)
{
    return *(U*)x.data >= *(U*)y.data;
}

template<typename U>
bool operator>(const node<U>& x, const node<U> &y)
{
    return *(U*)x.data > *(U*)y.data;
}

//Node vs Data

template<typename U>
bool operator<(const node<U>& x, const U &y)
{
    return *(U*)x.data < y;
}

template<typename U>
bool operator<=(const node<U>& x, const U &y)
{
    return *(U*)x.data <= y;
}

template<typename U>
bool operator==(const node<U>& x, const U &y)
{
    return *(U*)x.data == y;
}

template<typename U>
bool operator!=(const node<U>& x, const U &y)
{
    return *(U*)x.data != y;
}

template<typename U>
bool operator>=(const node<U>& x, const U &y)
{
    return *(U*)x.data >= y;
}

template<typename U>
bool operator>(const node<U>& x, const U &y)
{
    return *(U*)x.data > y;
}

//Data vs Node

template<typename U>
bool operator<(const U& x, const node<U> &y)
{
    return x < *(U*)y.data;
}

template<typename U>
bool operator<=(const U& x, const node<U> &y)
{
    return x <= *(U*)y.data;
}

template<typename U>
bool operator==(const U& x, const node<U> &y)
{
    return x == *(U*)y.data;
}

template<typename U>
bool operator!=(const U& x, const node<U> &y)
{
    return x != *(U*)y.data;
}


template<typename U>
bool operator>=(const U& x, const node<U> &y)
{
    return x >= *(U*)y.data;
}

template<typename U>
bool operator>(const U& x, const node<U> &y)
{
    return x > *(U*)y.data;
}

template<typename U>
ostream& operator<<(ostream &out, const node<U> &other)
{
    out<<*(U*)other.data<<"["<<other.count<<"]\n";
    out<<"*Lines:\n";
    for(unsigned int i=0; i<other.lines.size(); i++)
    {
        out<<setw(7)<<other.lines[i];
        if(!(i%10))
            out<<endl;
    }
    out<<"\n*Paragraphs:\n";
    for(unsigned int i=0; i<other.paragraphs.size(); i++)
    {
        out<<setw(7)<<other.paragraphs[i];
        if(!(i%10))
            out<<endl;
    }
    return out;
}

template<typename U>
istream& operator>>(istream &in, node<U> &other)
{
    return in;
}
#endif // NODE_H
