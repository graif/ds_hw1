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
         DS->highest_earner_employee->employee_id > e->employee_id)) {
        DS->highest_earner_employee = e;
    }
}

void UpdateCompanyHighestEarner(Company *c, Employee *e) {
    // same as above but for the highest earner for the specific company
    if (c->highest_earner_employee == nullptr || c->highest_earner_employee->salary <= e->salary ||
        (c->highest_earner_employee->salary == e->salary && c->highest_earner_employee->employee_id > e->employee_id)) {
        c->highest_earner_employee = e;
    }
}

StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (CompanyID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }

    tree<Company> *c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee


    try {
        /* Allocating a new Employee and trying to add it to tree as subElement
         * If the tree is empty, will receive INVALID_INPUT and manually set the new Employee as the new head
         * This is IMPORTANT because we need the function addSubElement to always happen
         * as it contains all the necessary checks*/

        Employee *e = new Employee(EmployeeID, c, Salary, Grade);
        StatusType status = addSubElement(((DataStrcture *) DS)->employee_head.get(), e, EmployeeID);
        if (status == INVALID_INPUT && EmployeeID > 0) { // employee = new head:
            ((DataStrcture *) DS)->employee_head = std::shared_ptr<tree<Employee>>(new tree<Employee>(EmployeeID, e));
        }
        else if (status != SUCCESS) return status;

        UpdateHighestEarner((DataStrcture *)DS, e);
        UpdateCompanyHighestEarner(c->element, e);
        (c->element->employee_count)++; // raise the company's employee count by 1

        /*** need to add the employee's pointer to the pointers_employees tree as well!! */
        /* if employees pointers is empty, initialize one */

        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
}

/* // backup command
 * StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if((DS == nullptr) || (EmployeeID <= 0) || (CompanyID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }

    Company* c = findCompanyById(((DataStrcture*)DS)->company_head.get(), CompanyID);
    Employee* e = findEmployeeById(((DataStrcture*)DS)->employee_head.get(), EmployeeID);
    if((c == nullptr) || e != nullptr) return FAILURE; // company doesn't exist or employee already exists

    Employee* daddy_e = findMyEmployeeDaddy(((DataStrcture*)DS)->employee_head.get(), EmployeeID);
    try {
        if (daddy_e == nullptr) { // no head, employee is the new head
            ((DataStrcture *) DS)->employee_head = std::shared_ptr<Employee>(new Employee(EmployeeID, c, Salary, Grade, ((DataStrcture *) DS)->employee_head.get()));
        } else {
            if (daddy_e->employee_id > EmployeeID) {
                daddy_e->left = std::shared_ptr<Employee>(new Employee(EmployeeID, c, Salary, Grade, ((DataStrcture *) DS)->employee_head.get()));
            } else {
                daddy_e->right = std::shared_ptr<Employee>(new Employee(EmployeeID, c, Salary, Grade, ((DataStrcture *) DS)->employee_head.get()));
            }
        }
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }

    if(((DataStrcture *) DS)->highest_earner_employee == nullptr || // if no highest earner currently exists
            ((DataStrcture *) DS)->highest_earner_employee->salary <= Salary || // or if current highest earner, earns less than our employee
            (((DataStrcture *) DS)->highest_earner_employee->salary == Salary && // or if he earns exactly the same as our employee
             ((DataStrcture *) DS)->highest_earner_employee->employee_id > EmployeeID)) { // but also has a higher employee ID
        ((DataStrcture *) DS)->highest_earner_employee = daddy_e; // or left or right, need to change
    }

    if(c->highest_earner_employee == nullptr || // same as above but for the highest earner for the specific company
            c->highest_earner_employee->salary <= Salary ||
       (c->highest_earner_employee->salary == Salary &&
               c->highest_earner_employee->employee_id > EmployeeID)) {
        c->highest_earner_employee = daddy_e; // or left or right, need to change
    }

    (c->employee_count)++; // raise the company's employee count by 1

return SUCCESS;
}
 */

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

void Quit(void **DS);