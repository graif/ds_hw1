//
// Created by omerg on 23/04/2022.
//

#include "Employee.h"
#include "library1.h"

using std::shared_ptr;

Employee::Employee(int id, Company* company, int salary, int grade) :
        id(id), company(company), salary(salary), grade(grade) {};