#include "../include/Iterator.h"
#include "../include/composite.h"

#include <vector>

DepthFirstIterator::DepthFirstIterator(WorkElement* root){
    start = root;
    cur = nullptr;
    first();
}

void DepthFirstIterator::first(){
    stack.clear();
    stack.push_back(start);
    next();
}

void DepthFirstIterator::next(){
    if(stack.empty()){
        cur = nullptr;
        return;
    }

    cur = stack.back();
    stack.pop_back();

    // a null root or a task has no children to visit
    if(cur == nullptr){
        return;
    }
    WorkGroup* group = cur->asGroup();
    if(group == nullptr){
        return;
    }

    // pushed in reverse so that the first child comes off the stack next
    const std::vector<WorkElement*>& children = group->getItems();
    int size = children.size();
    for(int i = size - 1; i >= 0; i--){
        stack.push_back(children[i]);
    }
}

bool DepthFirstIterator::isDone(){
    return cur == nullptr;
}

WorkElement* DepthFirstIterator::current(){
    return cur;
}
