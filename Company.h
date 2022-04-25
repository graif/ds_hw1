//
// Created by omerg on 23/04/2022.
//
#ifndef DS_HW1_COMPANY_H
#define DS_HW1_COMPANY_H

#include <memory>
#include "Employee.h"

using std::shared_ptr;

class Employee;

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

Company* findMyCompanyDaddy(Company* head_company ,int CompanyID){
    if(head_company == nullptr) { // no head, employee is the new head
        return nullptr;
    }
    else {
        if(head_company->company_id > CompanyID) {
            if(head_company->left == nullptr) {
                return head_company;
            }
            return findMyCompanyDaddy(head_company->left.get(), CompanyID);
        }
        else {
            if(head_company->right == nullptr) {
                return head_company;
            }
            return findMyCompanyDaddy(head_company->right.get(), CompanyID);
        }
    }
}

Company* findCompanyById(Company* head_company ,int CompanyID){
    if(head_company==nullptr){
        return nullptr;
    }
    if(head_company->company_id==CompanyID){
        return head_company;
    }
    else if(head_company->company_id>CompanyID && head_company->right!=nullptr) {
        return findCompanyById(head_company->right.get(), CompanyID);
    }
    else if(head_company->company_id<CompanyID && head_company->left!=nullptr) {
        return findCompanyById(head_company->left.get(), CompanyID);
    }
    return nullptr;
}

#endif //DS_HW1_COMPANY_H
