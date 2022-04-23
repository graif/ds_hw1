//
// Created by omerg on 23/04/2022.
//

#include "Employee.h"
#include "library1.h"
using std::shared_ptr;

Employee::Employee(int employee_id, Company* company,int salary,int grade,Employee* head) :employee_id(employee_id)
        ,company(company),salary(salary),grade(grade) {
    this->head=std::shared_ptr<Employee>(head);
};