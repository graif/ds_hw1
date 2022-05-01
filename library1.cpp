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
    shared_ptr<tree<Employee>> employees_pointers_by_salary;
};


/*** Eden's notes 30/APR/2022
 * Make sure we are updating the highest earners both in company and in general
 * */

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
            tree<Company> * temp = ((DataStrcture *) DS)->company_head->addElement(c.get(),c->id, &status);
            if(status != SUCCESS) {
                ((DataStrcture *) DS)->company_head=std::shared_ptr<tree<Company>>(temp);
                return status;
            }
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
            tree<Employee>* temp = ((DataStrcture *) DS)->employee_head->addElement(e.get(),e->id,&status);
            if(status != SUCCESS) return status;
            ((DataStrcture *) DS)->employee_head = shared_ptr<tree<Employee>>(temp);;
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

StatusType RemoveCompany(void *DS, int CompanyID){
    if(CompanyID<=0||DS== nullptr){
        return INVALID_INPUT;
    }
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
    if (c == nullptr||c->element->employees_pointers== nullptr) return FAILURE; // company doesn't exist, nowhere to add employee
    StatusType status=FAILURE;
    tree<Company>* temp = ((DataStrcture*)DS)->company_head->eraseElement(c->id,&status);
    if(status != SUCCESS){
        return status;
    }
    ((DataStrcture*)DS)->company_head = std::shared_ptr<tree<Company>>(temp);

    return SUCCESS;

}

//employee count, employe pointer - company and DS, higest - company and DS
StatusType RemoveEmployee(void *DS, int EmployeeID){
    if(EmployeeID<=0||DS== nullptr){
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head.get(), EmployeeID);
    Company* c = e->element->company;
    if (e == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee
    StatusType status=FAILURE;



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
    tree<Employee>* temp = e->element->company->employees_pointers->addElement(e->element.get(),e->id, &status);
    if(status != SUCCESS) return status;
    e->element->company->employees_pointers = shared_ptr<tree<Employee>>(temp);

    return SUCCESS;
}

StatusType AcquireCompany(void *DS, int AcquirerID, int TargetID, double Factor);

StatusType GetHighestEarner(void *DS, int CompanyID, int *EmployeeID) {
    if((DS == nullptr) || (EmployeeID == nullptr) || (CompanyID == 0)) {
        return INVALID_INPUT;
    }

    tree<Company>* c; // used to avoid calling it when not necessary:
    if(CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);

    if((CompanyID < 0 && ((DataStrcture*)DS)->employee_head == nullptr) ||
            (CompanyID > 0 && c == nullptr) ||
            (CompanyID > 0 && c->element->employees_pointers == nullptr)) {
        return FAILURE;
    }

    if(CompanyID < 0) {
        *EmployeeID = ((DataStrcture*)DS)->highest_earner_employee->id;
    }
    else {
        *EmployeeID = c->element->highest_earner_employee->id;
    }

    return SUCCESS;
}

tree<Employee>* findHighestEarner_Helper(tree<Employee>* t, tree<Employee>* curr_highest, int maxLimit = -1) {
    if(t == nullptr) return nullptr;
    if(t->left == nullptr && t->right == nullptr) {
        return curr_highest;
    }
    if((t->left != nullptr) && ((t->left->element->salary > curr_highest->element->salary) ||
            ((t->left->element->salary == curr_highest->element->salary) && (t->left->element->id < curr_highest->element->id)))) {
        curr_highest = t->left.get();
    }
    if((t->right != nullptr) && ((t->right->element->salary > curr_highest->element->salary) ||
       ((t->right->element->salary == curr_highest->element->salary) && (t->right->element->id < curr_highest->element->id)))) {
        curr_highest = t->right.get();
    }

    tree<Employee>* left_highest = findHighestEarner_Helper(t->left.get(), curr_highest, maxLimit);
    tree<Employee>* right_highest = findHighestEarner_Helper(t->right.get(), curr_highest, maxLimit);

    if(left_highest != nullptr && right_highest != nullptr) {
        if((left_highest->element->salary > right_highest->element->salary) ||
                (((left_highest->element->salary == right_highest->element->salary)) && ((left_highest->element->id < right_highest->element->id)))) {
            return left_highest;
        }
        else {
            return right_highest;
        }
    }
    else {
        if(left_highest != nullptr) {
            return left_highest;
        }
        else {
            return right_highest;
        }
    }
}

tree<Employee>* findHighestEarner(tree<Employee>* t, int maxLimit = -1) {
    return findHighestEarner_Helper(t, t, maxLimit);
    /// need to add support for maxLimit in helper function
}

StatusType GetAllEmployeesBySalary(void *DS, int CompanyID, int **Employees, int *NumOfEmployees) {
    try {
        if ((DS == nullptr) || (Employees == nullptr) || (NumOfEmployees == nullptr) || (CompanyID == 0)) {
            return INVALID_INPUT;
        }
        tree<Company>* c; // used to avoid calling it when not necessary:
        if(CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);

        if(CompanyID < 0 && ((DataStrcture*)DS)->employee_head == nullptr || CompanyID > 0 && c == nullptr) {
            return FAILURE;
        }

        /// need to add employees by salary order to the Employees array using 'findHighestEarner' help function
    }
    catch (std::bad_alloc&) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

StatusType GetHighestEarnerInEachCompany(void *DS, int NumOfCompanies, int **Employees);

StatusType GetNumEmployeesMatching(void *DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId,
                                   int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees);

void Quit(void **DS);