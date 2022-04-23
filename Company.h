//
// Created by omerg on 23/04/2022.
//
#ifndef DS_HW1_COMPANY_H
#define DS_HW1_COMPANY_H

#include <memory>
#include "Employee.h"

using std::shared_ptr;
class Company {
public:
    int employee_count;
    Employee* highest_earner_employee;
    int value;
    Employee* employees_pointers;
    shared_ptr<Company> head;
    shared_ptr<Company> left;
    shared_ptr<Company> right;
    Company( int value, int company_id,Company *head);
    ~Company()=default;

    int company_id;
};


#endif //DS_HW1_COMPANY_H
