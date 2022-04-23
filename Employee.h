//
// Created by omerg on 23/04/2022.
//

#ifndef DS_HW1_EMPLOYEE_H
#define DS_HW1_EMPLOYEE_H


#include "Company.h"
using std::shared_ptr;
class Employee {
    int employee_id;
    Company* company;
    int salary;
    int grade;
    shared_ptr<Employee> head;
    shared_ptr<Employee> left;
    shared_ptr<Employee> right;
public:
    Employee(int employee_id, Company* company,int salary,int grade,Employee* head);
    ~Employee()=default;
};

#endif //DS_HW1_EMPLOYEE_H
