#include "filter.h"

Filter::Filter(int f){
    frame = f;
    values.clear();
}

Filter::~Filter(){
    
}

unit Filter::newValue(unit n){
    if (values.size() >= frame){
        values.pop_back();
    }
    values.push_front(n);
    unit sum = 0;
    for (int i = 0; i<values.size(); i++)
        sum += values[i];
    return sum/values.size();
}