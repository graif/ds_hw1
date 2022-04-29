//
// Created by omerg on 23/04/2022.
//

#include <memory>
#include "library1.h"
#include "Company.h"
#include "Employee.h"
#include "tree.h"

using std::shared_ptr;

class DataStrcture {
public:
    DataStrcture() = default;

    ~DataStrcture() = default;

    shared_ptr<tree<Company>> company_head;
    shared_ptr<tree<Employee>> employee_head;
    Employee *highest_earner_employee;
};


void *Init() {
    try {
        DataStrcture *DS = new DataStrcture();
        DS->highest_earner_employee = nullptr;
        DS->company_head = nullptr;
        DS->employee_head = nullptr;
        return (void *) DS;

    }
    catch (std::bad_alloc &) {
        return nullptr;
    }
}

StatusType AddCompany(void *DS, int CompanyID, int Value) {
    try {
        if (DS == nullptr || CompanyID <= 0 || Value <= 0) {
            return INVALID_INPUT;
        }
        shared_ptr<Company> c = shared_ptr<Company>(new Company(Value,CompanyID));
        if(((DataStrcture*)DS)->company_head == nullptr) {
            ((DataStrcture*)DS)->company_head = shared_ptr<tree<Company>>(new tree<Company>(CompanyID,c.get()));
        }
        else {
            StatusType status = FAILURE;
            ((DataStrcture *) DS)->company_head->addElement(c.get(), &status);
            if(status != SUCCESS) return status;
        }
        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
}

void UpdateHighestEarner(DataStrcture *DS, Employee *e) {
    // if no highest earner currently exists
    // or if current highest earner, earns less than our employee
    // or if he earns exactly the same as our employee
    // but also has a higher employee ID
    if (DS->highest_earner_employee == nullptr || DS->highest_earner_employee->salary <= e->salary ||
        (DS->highest_earner_employee->salary == e->salary &&
         DS->highest_earner_employee->id > e->id)) {
        DS->highest_earner_employee = e;
    }
}

void UpdateCompanyHighestEarner(Company *c, Employee *e) {
    // same as above but for the highest earner for the specific company
    if (c->highest_earner_employee == nullptr || c->highest_earner_employee->salary <= e->salary ||
        (c->highest_earner_employee->salary == e->salary && c->highest_earner_employee->id > e->id)) {
        c->highest_earner_employee = e;
    }
}

StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (CompanyID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }

    if(((DataStrcture*)DS)->company_head == nullptr) return FAILURE;
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee

    try {
        shared_ptr<Employee> e = shared_ptr<Employee>(new Employee(EmployeeID, c->element.get(), Salary, Grade));
        if (((DataStrcture *) DS)->employee_head == nullptr) {
            ((DataStrcture *) DS)->employee_head = shared_ptr<tree<Employee>>(new tree<Employee>(EmployeeID, e.get()));
        }
        else {
            StatusType status = FAILURE;
            ((DataStrcture *) DS)->employee_head->addElement(e.get(), &status);
            if(status != SUCCESS) return status;
        }
        UpdateHighestEarner((DataStrcture *)DS, e.get());
        UpdateCompanyHighestEarner(c->element.get(), e.get());
        (c->element->employee_count)++; // raise the company's employee count by 1

        /*** add the employee's pointer to the pointers_employees tree: */
        /* if employees pointers is empty, initialize one */
        shared_ptr<tree<Employee>> t = shared_ptr<tree<Employee>>(findById(((DataStrcture*)DS)->employee_head.get(),EmployeeID));
        if (c->element->employees_pointers == nullptr) {
            c->element->employees_pointers = t;
        }
        else {
            shared_ptr<tree<Employee>> daddy_ptr = shared_ptr<tree<Employee>>(findMyDaddy(c->element->employees_pointers.get(), EmployeeID));
            if(daddy_ptr->id > EmployeeID) {
                daddy_ptr->left = t;
            }
            else {
                daddy_ptr->right = t;
            }
        }

        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
}

StatusType RemoveCompany(void *DS, int CompanyID);

StatusType RemoveEmployee(void *DS, int EmployeeID) {

}

StatusType GetCompanyInfo(void *DS, int CompanyID, int *Value, int *NumEmployees) {
    if((DS == nullptr) || (CompanyID <= 0) || (Value == nullptr) || (NumEmployees == nullptr)) {
        return INVALID_INPUT;
    }
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist

    *Value = c->element->value;
    *NumEmployees = c->element->employee_count;

    return SUCCESS;
}

StatusType GetEmployeeInfo(void *DS, int EmployeeID, int *EmployerID, int *Salary, int *Grade) {
    if((DS == nullptr) || (EmployeeID <= 0) || (Salary == nullptr) || (Grade == nullptr)) {
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head.get(), EmployeeID);
    if (e == nullptr) return FAILURE; // employee doesn't exist

    *EmployerID = e->element->company->id;
    *Salary = e->element->salary;
    *Grade = e->element->grade;

    return SUCCESS;
}

StatusType IncreaseCompanyValue(void *DS, int CompanyID, int ValueIncrease) {
    if((DS == nullptr) || (CompanyID <= 0) || (ValueIncrease <= 0)) {
        return INVALID_INPUT;
    }
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist

    c->element->value += ValueIncrease;
    return SUCCESS;
}

StatusType PromoteEmployee(void *DS, int EmployeeID, int SalaryIncrease, int BumpGrade) {
    if((DS == nullptr) || (EmployeeID <= 0) || (SalaryIncrease <= 0)) {
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head.get(), EmployeeID);
    if (e == nullptr) return FAILURE; // employee doesn't exist

    e->element->salary += SalaryIncrease;
    if(BumpGrade > 0) {
        e->element->grade++;
    }
    return SUCCESS;
}

StatusType HireEmployee(void *DS, int EmployeeID, int NewCompanyID) {
    if((DS == nullptr) || (EmployeeID <= 0) || (NewCompanyID <= 0)) {
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head.get(), EmployeeID);
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head.get(), NewCompanyID);
    if (c == nullptr || e == nullptr || e->element->company->id == NewCompanyID) return FAILURE;

    // delete employee from previous company's pointers tree:
    StatusType status = FAILURE;
    /*** update highestEarners & employee count!!!!***/
    e->element->company->employees_pointers->eraseElement(EmployeeID, &status);
    if(status != SUCCESS) return status;

    e->element->company = c->element.get(); // update employee's company

    // add employee to new company's pointers tree:
    e->element->company->employees_pointers->addElement(e->element.get(), &status);
    if(status != SUCCESS) return status;

    return SUCCESS;
}

StatusType AcquireCompany(void *DS, int AcquirerID, int TargetID, double Factor);

StatusType GetHighestEarner(void *DS, int CompanyID, int *EmployeeID);

StatusType GetAllEmployeesBySalary(void *DS, int CompanyID, int **Employees, int *NumOfEmployees);

StatusType GetHighestEarnerInEachCompany(void *DS, int NumOfCompanies, int **Employees);

StatusType GetNumEmployeesMatching(void *DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId,
                                   int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees);

void Quit(void **DS);