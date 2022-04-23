//
// Created by omerg on 23/04/2022.
//

#include "Company.h"

Company::Company( int value, int company_id,Company *head) : employee_count(0)
                 ,highest_earner_employee(NULL), value(value),company_id(company_id){
    this->employees_pointers=
    this->head=std::shared_ptr<Company>(head);
    this->left=NULL;
    this->right=NULL;
}
