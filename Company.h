//
// Created by omerg on 23/04/2022.
//
#ifndef DS_HW1_COMPANY_H
#define DS_HW1_COMPANY_H

#include <memory>
#include "Employee.h"

using std::shared_ptr;
class Company {
    int employee_count;
    Employee* highest_earner_employee;
    int value;
    int company_id;
    Employee* employees_pointers;
    shared_ptr<Company> head;
    shared_ptr<Company> left;
    shared_ptr<Company> right;
};


#endif //DS_HW1_COMPANY_H
