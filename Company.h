//
// Created by omerg on 23/04/2022.
//
#ifndef DS_HW1_COMPANY_H
#define DS_HW1_COMPANY_H

#include <memory>
#include "Employee.h"
#include "tree.h"

using std::shared_ptr;

class Employee;

class Company {
public:
    int employee_count;
    Employee *highest_earner_employee;
    int value;
    shared_ptr<tree<Employee>> employees_pointers;
    Company(int value, int company_id);
    ~Company() = default;

    int id;
};

#endif //DS_HW1_COMPANY_H
