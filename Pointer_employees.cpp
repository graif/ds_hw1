//
// Created by omerg on 23/04/2022.
//

#include "Pointer_employees.h"

using std::shared_ptr;

Pointer_employees::Pointer_employees(Pointer_employees *head):ptr_employee(ptr_employee) {
    this->ptr_employee=ptr_employee;
    this->head=std::shared_ptr<Pointer_employees>(head);
    this->left=NULL;
    this->right=NULL;
}