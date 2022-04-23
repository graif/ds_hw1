//
// Created by omerg on 23/04/2022.
//

#ifndef DS_HW1_POINTER_EMPLOYEES_H
#define DS_HW1_POINTER_EMPLOYEES_H

#include "Employee.h"
using std::shared_ptr;

class Pointer_employees {
public:
Employee* ptr_employee;
    std::shared_ptr<Pointer_employees> head;
    std::shared_ptr<Pointer_employees> left;
    std::shared_ptr<Pointer_employees> right;
    Pointer_employees(Employee* ptr_employee,Pointer_employees* head);
    ~Pointer_employees()=default;
};


#endif //DS_HW1_POINTER_EMPLOYEES_H
