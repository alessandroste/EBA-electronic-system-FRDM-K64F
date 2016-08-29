#ifndef _FILTER_H_
#define _FILTER_H_

#include <deque>
using std::deque;

typedef int unit;
typedef deque<unit> vec;

class Filter{
private:
    int frame;
    vec values;
public:
    Filter(int f);
    ~Filter();
    unit newValue(unit n);
};



#endif //_FILTER_H_