#include <iostream>
#include "FactoryMethod.h"
#include "WorkItemSpec.h"
#include "WorkData.h"

int main(){

    WorkItemFactory* factory[3];
    factory[0] = new UserTaskFactory();
    factory[1] = new ServiceTaskFactory();
    factory[2] = new ApprovalTaskFactory();

    WorkData config = WorkData();
    WorkItemSpec spec = WorkItemSpec("test", "testSpec", config);
    for (int i = 0; i < 3; i++){
        factory[i]->makeElement(spec);
        std::cout<< "Made element " << ++i <<std::endl;
    }
    
    return 0;
}