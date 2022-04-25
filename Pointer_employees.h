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
    Pointer_employees(Pointer_employees* head);
    ~Pointer_employees()=default;
    void addEmployeeptr( Employee* ptr_employee);
};
Pointer_employees * findMyEmployeePointerDaddy(Pointer_employees* p,Employee* head_employee) {
    if(head_employee == nullptr) { // no head, employee is the new head
        return p;
    }
    else {
        if(p->ptr_employee->employee_id > head_employee->employee_id) {
            if(p->left == nullptr) {
                return p;
            }
            return findMyEmployeePointerDaddy(p->left.get(), head_employee);
        }
        else {
            if(p->right == nullptr) {
                return p;
            }
            return findMyEmployeePointerDaddy(p->right.get(), head_employee);
        }
    }
}
#endif //DS_HW1_POINTER_EMPLOYEES_H
