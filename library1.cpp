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

    int employee_count;
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
            tree<Company> * temp = ((DataStrcture *) DS)->company_head->addElement(c.get(),c->id,false, &status);
            if(status != SUCCESS) {
                return status;
            }
            ((DataStrcture *) DS)->company_head=std::shared_ptr<tree<Company>>(temp);
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
            tree<Employee>* temp = ((DataStrcture *) DS)->employee_head->addElement(e.get(),e->id,false,&status);
            if(status != SUCCESS) return status;
            ((DataStrcture *) DS)->employee_head = shared_ptr<tree<Employee>>(temp);;
        }
        UpdateHighestEarner((DataStrcture *)DS, e.get());
        UpdateCompanyHighestEarner(c->element.get(), e.get());
        (c->element->employee_count)++; // raise the company's employee count by 1

        /*** add the employee's pointer to the pointers_employees tree: **/
        /* if employees pointers is empty, initialize one */
        shared_ptr<tree<Employee>> t = shared_ptr<tree<Employee>>(findById(((DataStrcture*)DS)->employee_head.get(),EmployeeID));
        if (c->element->employees_pointers == nullptr) {
            tree<Employee>* ba = new tree<Employee>(EmployeeID, t->element.get());
            c->element->employees_pointers = shared_ptr<tree<Employee>>(ba);
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = c->element->employees_pointers->addElement(t->element.get(), EmployeeID, false, &status);
            if(status != SUCCESS) return status;
            c->element->employees_pointers = std::shared_ptr<tree<Employee>>(temp);
        }

        /*** add the employee's pointer by SALARY: **/
        if (c->element->employees_pointers_by_salary == nullptr) {
            tree<Employee>* ba = new tree<Employee>(Salary, t->element.get());
            c->element->employees_pointers_by_salary = shared_ptr<tree<Employee>>(ba);
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = c->element->employees_pointers_by_salary->addElement(t->element.get(), Salary,true, &status);
            if(status != SUCCESS) return status;
            c->element->employees_pointers_by_salary = std::shared_ptr<tree<Employee>>(temp);
        }

        /*** add the employee's pointer to the general employee by SALARY tree ***/
        if(((DataStrcture*)DS)->employees_pointers_by_salary == nullptr) {
            tree<Employee>* ba = new tree<Employee>(Salary, t->element.get());
            ((DataStrcture*)DS)->employees_pointers_by_salary = shared_ptr<tree<Employee>>(ba);
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = ((DataStrcture*)DS)->employees_pointers_by_salary->addElement(t->element.get(), Salary,true, &status);
            if(status != SUCCESS) return status;
            ((DataStrcture*)DS)->employees_pointers_by_salary = std::shared_ptr<tree<Employee>>(temp);
        }
        (((DataStrcture*)DS)->employee_count)++;
        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
        /// remember to delete previous steps if failed mid-way
    }
}

StatusType RemoveCompany(void *DS, int CompanyID){
    try {
        if (CompanyID <= 0 || DS == nullptr) {
            return INVALID_INPUT;
        }
        tree<Company> *c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
        if (c == nullptr || c->element->employees_pointers == nullptr)
            return FAILURE; // company doesn't exist, nowhere to add employee
        StatusType status = FAILURE;
        tree<Company> *temp = ((DataStrcture *) DS)->company_head->eraseElement(c->element.get(),false, &status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->company_head = std::shared_ptr<tree<Company>>(temp);

        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
        /// remember to delete previous steps if failed mid-way
    }

}


StatusType RemoveEmployee(void *DS, int EmployeeID){
    try {
        if (EmployeeID <= 0 || DS == nullptr) {
            return INVALID_INPUT;
        }

        tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head.get(), EmployeeID);
        if (e == nullptr||e->element== nullptr ) return FAILURE; // company doesn't exist, nowhere to add employee
        Company *c = e->element->company;
        StatusType status = FAILURE;

        //delete emp pntr by salary DS
        tree<Employee> *temp = ((DataStrcture *) DS)->employees_pointers_by_salary->eraseElement(e->element.get(), true,&status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->employees_pointers_by_salary = shared_ptr<tree<Employee>>(temp);

        //delete emp pntr Company
        temp = c->employees_pointers->eraseElement(e->element.get(), false,&status);

        if (status != SUCCESS) {
            return status;
        }
        c->employees_pointers = shared_ptr<tree<Employee>>(temp);

       //delete emp pntr by salary Company
        temp = c->employees_pointers_by_salary->eraseElement(e->element.get(), true,&status);

        if (status != SUCCESS) {
            return status;
        }
        c->employees_pointers_by_salary = shared_ptr<tree<Employee>>(temp);

        //delete emp pntr DS
        temp = ((DataStrcture *) DS)->employee_head->eraseElement(e->element.get(),false,&status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->employee_head = shared_ptr<tree<Employee>>(temp);

        //update counters
        c->employee_count--;
        ((DataStrcture *) DS)->employee_count--;

        //update highest earner
        temp=((DataStrcture *) DS)->employees_pointers_by_salary.get();
        while(temp->right) {
            if(!temp->right) break;
            temp=temp->right.get();
        }
        ((DataStrcture *) DS)->highest_earner_employee=temp->element.get();

        temp=c->employees_pointers_by_salary.get();
        while(temp->right) {
            if(!temp->right) break;
            temp=temp->right.get();
        }
        c->highest_earner_employee = temp->element.get();

        return SUCCESS;

    }

    catch (std::bad_alloc &) {
            return ALLOCATION_ERROR;
            /// remember to delete previous steps if failed mid-way
    }

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
    StatusType status;

    try {
        Employee *new_emp = new Employee(e->element->id, e->element->company, e->element->salary, e->element->grade);
       status = RemoveEmployee(DS,e->id);
        if (status != SUCCESS){
            delete new_emp;
            return status;
        }
        status= AddEmployee(DS,new_emp->id,NewCompanyID,new_emp->salary,new_emp->grade);

        if (status != SUCCESS) {
            delete new_emp;
            return status;
        }
        delete new_emp;
        return SUCCESS;
    }

    catch (std::bad_alloc&) {
        return ALLOCATION_ERROR;
    }
}

StatusType AcquireCompany(void *DS, int AcquirerID, int TargetID, double Factor) {
    try {
        if(DS == nullptr || AcquirerID <= 0 || TargetID <= 0 || AcquirerID == TargetID || Factor < 1.00) {
            return INVALID_INPUT;
        }

        tree<Company>* Acquirer = findById(((DataStrcture *) DS)->company_head.get(), AcquirerID);
        tree<Company>* Target = findById(((DataStrcture *) DS)->company_head.get(), TargetID);
        if(Acquirer == nullptr || Target == nullptr || Acquirer->element->value >= (10*Target->element->value)) {
            return FAILURE;
        }

        // add acquisition here
        Acquirer->element->value += Target->element->value;
        Acquirer->element->value *= Factor;
        Acquirer->element->employee_count+=Target->element->employee_count;
        if( Target->element->highest_earner_employee->salary > Acquirer->element->highest_earner_employee->salary){
            Acquirer->element->highest_earner_employee=Target->element->highest_earner_employee;
        }

        else if(Target->element->highest_earner_employee->salary == Acquirer->element->highest_earner_employee->salary){
            if(Target->element->highest_earner_employee->id < Acquirer->element->highest_earner_employee->salary)
                Acquirer->element->highest_earner_employee=Target->element->highest_earner_employee;
        }

        StatusType status=FAILURE;
        tree<Employee>* merged_emp_ptr= CombineTree<Employee>
                (Acquirer->element->employees_pointers.get(),Target->element->employees_pointers.get(),
                 Acquirer->element->employee_count,Target->element->employee_count,&status);
        Acquirer->element->employees_pointers=std::shared_ptr<tree<Employee>>(merged_emp_ptr);

        tree<Employee>* merged_emp_salary= CombineTree<Employee>
                (Acquirer->element->employees_pointers_by_salary.get(),Target->element->employees_pointers_by_salary.get(),
                 Acquirer->element->employee_count,Target->element->employee_count,&status);

        Acquirer->element->employees_pointers_by_salary=std::shared_ptr<tree<Employee>>(merged_emp_salary);

        return status;
    }
    catch (std::bad_alloc&) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

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

StatusType GetAllEmployeesBySalary_helper(tree<Employee>* e, int **Employees, int *NumOfEmployees) {
    //if(*counter >= NumOfCompanies) return SUCCESS;
    if(e->left == nullptr) {
        // add e
        *(Employees[*NumOfEmployees]) = e->element->id;
        (*NumOfEmployees)++;
        return SUCCESS; // smallest node
    }

    StatusType result = GetAllEmployeesBySalary_helper(e->left.get(), Employees, NumOfEmployees);
    if(result != SUCCESS) return result;

    // add e
    *(Employees[*NumOfEmployees]) = e->element->id;
    (*NumOfEmployees)++;

    if(e->right != nullptr) {
        result = GetAllEmployeesBySalary_helper(e->right.get(), Employees, NumOfEmployees);
        if(result != SUCCESS) return result;
    }
    return SUCCESS;
}

StatusType GetAllEmployeesBySalary(void *DS, int CompanyID, int **Employees, int *NumOfEmployees) {
    try {
        if ((DS == nullptr) || (Employees == nullptr) || (NumOfEmployees == nullptr) || (CompanyID == 0)) {
            return INVALID_INPUT;
        }
        *Employees = new int[*NumOfEmployees]; // this is supposed to be deleted by 'them'

        tree<Company>* c; // used to avoid calling it when not necessary:
        if(CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head.get(), CompanyID);
        //make sure that parentheses are right//
        if(((CompanyID < 0 )&&( ((DataStrcture*)DS)->employee_head == nullptr)) || (CompanyID > 0 && c == nullptr)) {
            return FAILURE;
        }

        if(CompanyID < 0) {
            // get an ordered list of employees by salary from all employees
            StatusType result = GetAllEmployeesBySalary_helper(((DataStrcture*)DS)->employees_pointers_by_salary.get(), Employees, NumOfEmployees);
            if(result != SUCCESS) return result;
        }
        else {
            // get an ordered list of employees by salary from all employees in the company
            StatusType result = GetAllEmployeesBySalary_helper(c->element->employees_pointers_by_salary.get(), Employees, NumOfEmployees);
            if(result != SUCCESS) return result;
        }
    }
    catch (std::bad_alloc&) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

StatusType GetHighestEarnerInEachCompany_helper(void *DS, int NumOfCompanies, int **Employees, tree<Company>* c, Company** companies, int* counter) {
    if(*counter >= NumOfCompanies) return SUCCESS;
    if(c->left == nullptr) {
        // add c
        companies[*counter] = c->element.get();
        (*counter)++;
        return SUCCESS; // smallest node
    }

    StatusType result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, Employees, c->left.get(), companies, counter);
    if(result != SUCCESS) return result;

    // add c
    companies[*counter] = c->element.get();
    (*counter)++;

    if(c->right != nullptr) {
        result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, Employees, c->right.get(), companies, counter);
        if(result != SUCCESS) return result;
    }
}

StatusType GetHighestEarnerInEachCompany(void *DS, int NumOfCompanies, int **Employees) {
    try {
        if((DS == nullptr) || (Employees == nullptr) || NumOfCompanies < 1) return INVALID_INPUT;
        *Employees = new int[NumOfCompanies]; // this is supposed to be deleted by 'them'

        Company** companies = new Company*[NumOfCompanies];
        int counter = 0;

        tree<Company>* c = ((DataStrcture*)DS)->company_head.get();
        StatusType result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, Employees, c, companies, &counter);
        if(result != SUCCESS) return result;
        if(counter != NumOfCompanies) return FAILURE;

        for(int i = 0; i < NumOfCompanies; i++) {
            *(Employees[i]) = companies[i]->highest_earner_employee->id;
        }

        delete[] companies;
    }
    catch (std::bad_alloc&) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

StatusType GetNumEmployeesMatching_helper(tree<Employee>* e, int MinEmployeeID, int MaxEmployeeId, int *TotalNumOfEmployees) {
    if(e->left == nullptr || e->left->id < MinEmployeeID) {
        // add e
        if(e->id >= MinEmployeeID && e->id <= MaxEmployeeId)
            (*TotalNumOfEmployees)++;
        return SUCCESS; // smallest node
    }

    StatusType result = GetNumEmployeesMatching_helper(e->left.get(), MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
    if(result != SUCCESS) return result;

    // add e
    if(e->id >= MinEmployeeID && e->id <= MaxEmployeeId)
        (*TotalNumOfEmployees)++;

    if(e->right != nullptr && e->right->id <= MaxEmployeeId) {
        result = GetNumEmployeesMatching_helper(e->right.get(), MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if(result != SUCCESS) return result;
    }
}

StatusType GetNumEmployeesMatching(void *DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId,
                                   int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees) {
    if(DS == nullptr || TotalNumOfEmployees == nullptr || NumOfEmployees == nullptr || CompanyID == 0 ||
    MinEmployeeID < 0 || MaxEmployeeId < 0 || MinSalary < 0 || MinGrade < 0 || MinEmployeeID > MaxEmployeeId) {
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
        StatusType result = GetNumEmployeesMatching_helper(((DataStrcture*)DS)->employee_head.get(), MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if(result != SUCCESS) return result;
    }
    else {
        StatusType result = GetNumEmployeesMatching_helper(c->element->employees_pointers.get(), MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if(result != SUCCESS) return result;
    }
    return SUCCESS;
}

void Quit(void **DS){



}
