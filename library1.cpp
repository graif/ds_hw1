//
// Created by omerg on 23/04/2022.
//

#include <memory>
#include "library1.h"
#include "Company.h"
#include "Employee.h"
using std::shared_ptr;

class DataStrcture {
public:
    DataStrcture()=default;
    ~DataStrcture()=default;

    shared_ptr<Company> company_head;
    shared_ptr<Employee> employee_head;
    Employee* highest_earner_employee;
};



void *Init() {

    DataStrcture *DS = new DataStrcture();
    return (void*)DS;

}

StatusType AddCompany(void *DS, int CompanyID, int Value){
    try {
    if(DS==nullptr||CompanyID<=0||Value<=0){
        return INVALID_INPUT;
    }
    if(findCompanyById(((DataStrcture*)DS)->company_head.get(),CompanyID)!=nullptr){
        return FAILURE;
    }

    Company* daddy_ptr=findMyCompanyDaddy(findCompanyById(((DataStrcture*)DS)->company_head.get(),CompanyID), CompanyID);
    if(daddy_ptr==nullptr){
        ((DataStrcture*)DS)->company_head=std::shared_ptr<Company>(new Company(Value,CompanyID,nullptr));
    }
    else if(daddy_ptr->company_id>CompanyID){
        daddy_ptr->left=std::shared_ptr<Company>(new Company(Value,CompanyID,nullptr));
    }
    else{
        daddy_ptr->right=std::shared_ptr<Company>(new Company(Value,CompanyID,nullptr));
    }

        }
    catch(std::bad_alloc&) {
        return ALLOCATION_ERROR;
    }
}

StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if((DS == nullptr) || (EmployeeID <= 0) || (CompanyID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }

    Company* c = findCompanyById(((DataStrcture*)DS)->company_head.get(), CompanyID);
    Employee* e = findEmployeeById(((DataStrcture*)DS)->employee_head.get(), EmployeeID);
    if((c == nullptr) || e != nullptr) return FAILURE; // company doesn't exist or employee already exists

    Employee* daddy_e = findMyEmployeeDaddy(((DataStrcture*)DS)->employee_head.get(), EmployeeID);
    if(daddy_e == nullptr) { // no head, employee is the new head
        try {
            ((DataStrcture*)DS)->employee_head = std::shared_ptr<Employee>(new Employee(EmployeeID, c, Salary, Grade, ((DataStrcture*)DS)->employee_head.get()));
        }
        catch(std::bad_alloc&) {
            return ALLOCATION_ERROR;
        }
    }
    else {
        if(daddy_e->employee_id > EmployeeID) {
            try {
                daddy_e->left = std::shared_ptr<Employee>(new Employee(EmployeeID, c, Salary, Grade, ((DataStrcture*)DS)->employee_head.get()));
            }
            catch(std::bad_alloc&) {
                return ALLOCATION_ERROR;
            }

        }
        else {
            try {
                daddy_e->right = std::shared_ptr<Employee>(new Employee(EmployeeID, c, Salary, Grade, ((DataStrcture*)DS)->employee_head.get()));
            }
            catch(std::bad_alloc&) {
                return ALLOCATION_ERROR;
            }
        }
    }

    /*** need to add the employee's pointer to the pointers_employees tree as well!! */

    return SUCCESS;
}

StatusType RemoveCompany(void *DS, int CompanyID);

StatusType RemoveEmployee(void *DS, int EmployeeID) {

}

StatusType GetCompanyInfo(void *DS, int CompanyID, int *Value, int *NumEmployees);

StatusType GetEmployeeInfo(void *DS, int EmployeeID, int *EmployerID, int *Salary, int *Grade);

StatusType IncreaseCompanyValue(void *DS, int CompanyID, int ValueIncrease);

StatusType PromoteEmployee(void *DS, int EmployeeID, int SalaryIncrease, int BumpGrade);

StatusType HireEmployee(void *DS, int EmployeeID, int NewCompanyID);

StatusType AcquireCompany(void *DS, int AcquirerID, int TargetID, double Factor);

StatusType GetHighestEarner(void *DS, int CompanyID, int *EmployeeID);

StatusType GetAllEmployeesBySalary(void *DS, int CompanyID, int **Employees, int *NumOfEmployees);

StatusType GetHighestEarnerInEachCompany(void *DS, int NumOfCompanies, int **Employees);

StatusType GetNumEmployeesMatching(void *DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId,
                                   int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees);

void Quit(void** DS);