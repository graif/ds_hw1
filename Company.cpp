//
// Created by omerg on 23/04/2022.
//

#include "Company.h"

Company::Company(int value, int company_id) : employee_count(0), highest_earner_employee(nullptr), value(value),
                                              id(company_id), employees_pointers(nullptr)
                                              ,employees_pointers_by_salary(nullptr),salary(0) {};
