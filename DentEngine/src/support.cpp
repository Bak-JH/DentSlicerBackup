#include "support.h"

SupportBase::SupportBase (float x, float y, float z){
    setStartPosition(x, y, z);
}

vector<IntPoint> overhangDetect(){

}

void SupportBase::setStartPosition(float x, float y, float z){
    start.clear();
    start.push_back(x);
    start.push_back(y);
    start.push_back(z);
    return;
}

void SupportBase::setEndPosition(float x, float y, float z){
    end.clear();
    end.push_back(x);
    end.push_back(y);
    end.push_back(z);
    return;
}
