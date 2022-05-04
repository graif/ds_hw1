//
// Created by omerg on 23/04/2022.
//

#include <memory>
#include <iostream>
#include "library1.h"
#include "Company.h"
#include "Employee.h"
#include "tree.h"

class DataStrcture {
public:
    DataStrcture() = default;

    ~DataStrcture() = default;

    int employee_count;
    tree<Company>* company_head;
    tree<Employee>* employee_head;
    Employee *highest_earner_employee;
    tree<Employee>* employees_pointers_by_salary;
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
    Company* c;
    try {
        DataStrcture* DSS = ((DataStrcture *) DS);
        if (DS == nullptr || CompanyID <= 0 || Value <= 0) {
            return INVALID_INPUT;
        }

        c = new Company(Value,CompanyID);
        if(((DataStrcture*)DS)->company_head == nullptr) {
            ((DataStrcture*)DS)->company_head = new tree<Company>(CompanyID,c);
        }
        else {
            StatusType status = FAILURE;
            tree<Company> * temp = addElement(DSS->company_head,c,c->id,false, &status); //valgrind segfault cause
            if(status != SUCCESS) {
                return status;
            }
            if(((DataStrcture *) DS)->company_head->id != temp->id) {
                ((DataStrcture *) DS)->company_head = temp; //valgrind segfault cause
                //((DataStrcture *) DS)->company_head.get() = temp;
            }
        }
        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        delete (c);
        return ALLOCATION_ERROR;
    }
}

/*void UpdateHighestEarner(DataStrcture *DS, Employee *e) {
    if(DS == nullptr || e == nullptr) return;
    // if no highest earner currently exists
    // or if current highest earner, earns less than our employee
    // or if he earns exactly the same as our employee
    // but also has a higher employee ID
    if (DS->highest_earner_employee == nullptr || DS->highest_earner_employee->salary < e->salary ||
        (DS->highest_earner_employee->salary == e->salary &&
         DS->highest_earner_employee->id > e->id)) {
        DS->highest_earner_employee = e;
    }
}*/
/**
void UpdateCompanyHighestEarner(Company *c, Employee *e) {
    if(c == nullptr || e == nullptr) return;
    // same as above but for the highest earner for the specific company
    if (c->highest_earner_employee == nullptr || c->highest_earner_employee->salary < e->salary ||
        (c->highest_earner_employee->salary == e->salary && c->highest_earner_employee->id > e->id)) {
        c->highest_earner_employee = e;
    } // valgrind invalid read of size 8 (pointer)
}
**/
Employee * UpdateHighestEarner(tree<Employee> *c) {
    if(c==nullptr)
        return nullptr;

    while(c->right){
        c = c->right;
    }
    return c->element;
}

StatusType AddEmployee_C(void *DS, int EmployeeID, Company* c, int Salary, int Grade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }

    DataStrcture* DSS = (DataStrcture*)DS;
    if(((DataStrcture*)DS)->company_head == nullptr) return FAILURE;
    if (c == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee

    try {
        Employee* e = new Employee(EmployeeID, c, Salary, Grade);
        if (((DataStrcture *) DS)->employee_head == nullptr) {
            ((DataStrcture *) DS)->employee_head = new tree<Employee>(EmployeeID, e);
        }
        else {
            StatusType status = FAILURE;
            tree<Employee>* temp = addElement(DSS->employee_head, e,EmployeeID,false,&status);
            if(status != SUCCESS) return status;
            ((DataStrcture *) DS)->employee_head = temp;
        }

        /*** add the employee's pointer to the pointers_employees tree: **/
        /* if employees pointers is empty, initialize one */
        tree<Employee>* t = findById(((DataStrcture*)DS)->employee_head,EmployeeID);

        if(t == nullptr) {
            std::cout << "t is null for some stupid reason (EmployeeID: " << EmployeeID << ")" << std::endl;
            return FAILURE;
        }

        if (c->employees_pointers == nullptr) {
            tree<Employee>* ba = new tree<Employee>(EmployeeID, t->element);
            c->employees_pointers = ba;
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = addElement(c->employees_pointers,t->element, EmployeeID, false, &status);
            if(status != SUCCESS) return status;
            c->employees_pointers = temp;
        }

        /*** add the employee's pointer by SALARY: **/
        if (c->employees_pointers_by_salary == nullptr) {
            tree<Employee>* ba2 = new tree<Employee>(Salary, t->element);
            c->employees_pointers_by_salary = ba2;
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = addElement(c->employees_pointers_by_salary,t->element, Salary,true, &status);
            if(status != SUCCESS) {
                return status;
            }
            c->employees_pointers_by_salary = temp;
        }

        /*** add the employee's pointer to the general employee by SALARY tree ***/
        if(((DataStrcture*)DS)->employees_pointers_by_salary == nullptr) {
            tree<Employee>* ba3 = new tree<Employee>(Salary, t->element);
            ((DataStrcture*)DS)->employees_pointers_by_salary = ba3;
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = addElement(((DataStrcture*)DS)->employees_pointers_by_salary,t->element, Salary,true, &status);
            if(status != SUCCESS) return status;
            ((DataStrcture*)DS)->employees_pointers_by_salary = temp;
        }
        ((DataStrcture *)DS)->highest_earner_employee = UpdateHighestEarner(((DataStrcture *)DS)->employees_pointers_by_salary);
        c->highest_earner_employee = UpdateHighestEarner(c->employees_pointers_by_salary);

        (((DataStrcture*)DS)->employee_count)++;
        (c->employee_count)++; // raise the company's employee count by 1
        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
        /// remember to delete previous steps if failed mid-way
    }
}

StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (CompanyID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }

    DataStrcture* DSS = (DataStrcture*)DS;
    if(((DataStrcture*)DS)->company_head == nullptr) return FAILURE;
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head, CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee

    try {
        Employee* e = new Employee(EmployeeID, c->element, Salary, Grade);
        if (((DataStrcture *) DS)->employee_head == nullptr) {
            ((DataStrcture *) DS)->employee_head = new tree<Employee>(EmployeeID, e);
        }
        else {
            StatusType status = FAILURE;
            tree<Employee>* temp = addElement(DSS->employee_head, e,EmployeeID,false,&status);
            if(status != SUCCESS) return status;
            ((DataStrcture *) DS)->employee_head = temp;
        }

        /*** add the employee's pointer to the pointers_employees tree: **/
        /* if employees pointers is empty, initialize one */
        tree<Employee>* t = findById(((DataStrcture*)DS)->employee_head,EmployeeID);

        if(t == nullptr) {
            std::cout << "t is null for some stupid reason (EmployeeID: " << EmployeeID << ")" << std::endl;
            return FAILURE;
        }

        if (c->element->employees_pointers == nullptr) {
            tree<Employee>* ba = new tree<Employee>(EmployeeID, t->element);
            c->element->employees_pointers = ba;
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = addElement(c->element->employees_pointers,t->element, EmployeeID, false, &status);
            if(status != SUCCESS) return status;
            c->element->employees_pointers = temp;
        }

        /*** add the employee's pointer by SALARY: **/
        if (c->element->employees_pointers_by_salary == nullptr) {
            tree<Employee>* ba2 = new tree<Employee>(Salary, t->element);
            c->element->employees_pointers_by_salary = ba2;
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = addElement(c->element->employees_pointers_by_salary,t->element, Salary,true, &status);
            if(status != SUCCESS) {
                return status;
            }
            c->element->employees_pointers_by_salary = temp;
        }

        /*** add the employee's pointer to the general employee by SALARY tree ***/
        if(((DataStrcture*)DS)->employees_pointers_by_salary == nullptr) {
            tree<Employee>* ba3 = new tree<Employee>(Salary, t->element);
            ((DataStrcture*)DS)->employees_pointers_by_salary = ba3;
        }
        else {
            StatusType status = FAILURE;
            tree<Employee> * temp = addElement(((DataStrcture*)DS)->employees_pointers_by_salary,t->element, Salary,true, &status);
            if(status != SUCCESS) return status;
            ((DataStrcture*)DS)->employees_pointers_by_salary = temp;
        }
        ((DataStrcture *)DS)->highest_earner_employee = UpdateHighestEarner(((DataStrcture *)DS)->employees_pointers_by_salary);
        c->element->highest_earner_employee = UpdateHighestEarner(c->element->employees_pointers_by_salary);

        (((DataStrcture*)DS)->employee_count)++;
        (c->element->employee_count)++; // raise the company's employee count by 1
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
        tree<Company> *c = findById(((DataStrcture *) DS)->company_head, CompanyID);
        if (c == nullptr )
            return FAILURE; // company doesn't exist, nowhere to add employee
        StatusType status = FAILURE;
        tree<Company> *temp = ((DataStrcture *) DS)->company_head->eraseElement(c->element,false, &status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->company_head = temp;

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

        tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
        if (e == nullptr||e->element== nullptr ) return FAILURE; // company doesn't exist, nowhere to add employee
        Company *c = e->element->company;
        StatusType status = FAILURE;
        //delete emp pntr by salary DS
        tree<Employee> *temp = ((DataStrcture *) DS)->employees_pointers_by_salary->eraseElement(e->element, true,&status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->employees_pointers_by_salary = temp;

        //delete emp pntr Company
        temp = c->employees_pointers->eraseElement(e->element, false,&status);

        if (status != SUCCESS) {
            return status;
        }
        c->employees_pointers = temp;

       //delete emp pntr by salary Company
        temp = c->employees_pointers_by_salary->eraseElement(e->element, true,&status);

        if (status != SUCCESS) {
            return status;
        }
        c->employees_pointers_by_salary = temp;

        //delete emp pntr DS
        temp = ((DataStrcture *) DS)->employee_head->eraseElement( e->element,false,&status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->employee_head = temp;

        //update counters
        c->employee_count--;
        ((DataStrcture *) DS)->employee_count--;

        //update highest earner
        /*temp=((DataStrcture *) DS)->employees_pointers_by_salary;
        while(temp->right) {
            if(!temp->right) break;
            temp=temp->right;
        }
        ((DataStrcture *) DS)->highest_earner_employee=temp->element;

        temp=c->employees_pointers_by_salary;
        while(temp->right) {
            if(!temp->right) break;
            temp=temp->right;
        }
        c->highest_earner_employee = temp->element;*/
        ((DataStrcture *)DS)->highest_earner_employee = UpdateHighestEarner(((DataStrcture *)DS)->employees_pointers_by_salary);
        c->highest_earner_employee = UpdateHighestEarner(c->employees_pointers_by_salary);

        delete e->element;
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
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head, CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist

    *Value = c->element->value;
    *NumEmployees = c->element->employee_count;

    return SUCCESS;
}

StatusType GetEmployeeInfo(void *DS, int EmployeeID, int *EmployerID, int *Salary, int *Grade) {
    if((DS == nullptr) || (EmployeeID <= 0) || (Salary == nullptr) || (Grade == nullptr)) {
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
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
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head, CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist

    c->element->value += ValueIncrease;
    return SUCCESS;
}

StatusType PromoteEmployee(void *DS, int EmployeeID, int SalaryIncrease, int BumpGrade) {
    if((DS == nullptr) || (EmployeeID <= 0) || (SalaryIncrease <= 0)) {
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    if (e == nullptr) return FAILURE; // employee doesn't exist

    int temp_salary = e->element->salary + SalaryIncrease;
    int temp_grade = e->element->grade;
    if(BumpGrade > 0) {
        temp_grade++;
    }

    Company* c = e->element->company;
    RemoveEmployee(DS, EmployeeID);
    AddEmployee_C(DS,EmployeeID,c,temp_salary,temp_grade);

    return SUCCESS;
}

StatusType HireEmployee(void *DS, int EmployeeID, int NewCompanyID) {
    if((DS == nullptr) || (EmployeeID <= 0) || (NewCompanyID <= 0)) {
        return INVALID_INPUT;
    }
    tree<Employee>* e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    tree<Company>* c = findById(((DataStrcture *) DS)->company_head, NewCompanyID);
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

        tree<Company>* Acquirer = findById(((DataStrcture *) DS)->company_head, AcquirerID);
        tree<Company>* Target = findById(((DataStrcture *) DS)->company_head, TargetID);
        if(Acquirer == nullptr || Target == nullptr || Acquirer->element->value < (10*Target->element->value)) {
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
                (Acquirer->element->employees_pointers,Target->element->employees_pointers,
                 Acquirer->element->employee_count,Target->element->employee_count,&status);
        Acquirer->element->employees_pointers = merged_emp_ptr;

        tree<Employee>* merged_emp_salary= CombineTree<Employee>
                (Acquirer->element->employees_pointers_by_salary,Target->element->employees_pointers_by_salary,
                 Acquirer->element->employee_count,Target->element->employee_count,&status);

        Acquirer->element->employees_pointers_by_salary = merged_emp_salary;

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

    DataStrcture* DSS = (DataStrcture*)DS;

    tree<Company>* c; // used to avoid calling it when not necessary:
    if(CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head, CompanyID);

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
    if(e== nullptr)
        return SUCCESS;

    StatusType result = GetAllEmployeesBySalary_helper(e->right, Employees, NumOfEmployees);
    if(result != SUCCESS) return result;

    (*(Employees))[*NumOfEmployees] = e->element->id;
    (*NumOfEmployees)++;

    result = GetAllEmployeesBySalary_helper(e->left, Employees, NumOfEmployees);
        if(result != SUCCESS) return result;

    return SUCCESS;
}

StatusType GetAllEmployeesBySalary(void *DS, int CompanyID, int **Employees, int *NumOfEmployees) {
    try {
        if ((DS == nullptr) || (Employees == nullptr) || (NumOfEmployees == nullptr) || (CompanyID == 0)) {
            return INVALID_INPUT;
        }

        tree<Company>* c; // used to avoid calling it when not necessary:
        if(CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head, CompanyID);
        if(((CompanyID < 0) && (((DataStrcture*)DS)->employee_head == nullptr)) || ((CompanyID > 0) && (c == nullptr)) || ((CompanyID > 0) && (c->element->employees_pointers == nullptr))) {
            return FAILURE;
        }

        if(CompanyID > 0) {
            *NumOfEmployees = c->element->employee_count;
        }
        else { // we check if 0 earlier
            *NumOfEmployees = ((DataStrcture*)DS)->employee_count;
        }

        *Employees = (int*)malloc(sizeof(int)*(*NumOfEmployees)); // this is supposed to be deleted by 'them'

        int index = 0;
        if(CompanyID < 0) {
            // get an ordered list of employees by salary from all employees
            StatusType result = GetAllEmployeesBySalary_helper(((DataStrcture*)DS)->employees_pointers_by_salary, Employees, &index);
            if(result != SUCCESS) return result;
        }
        else {
            // get an ordered list of employees by salary from all employees in the company
            StatusType result = GetAllEmployeesBySalary_helper(c->element->employees_pointers_by_salary, Employees, &index);
            if(result != SUCCESS) return result;
        }
        if(index + 1 != *NumOfEmployees) {
            //std::cout << "index: " << index + 1 << " NumOfEmployees: " << *NumOfEmployees << std::endl;
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
        companies[*counter] = c->element;
        (*counter)++;
        return SUCCESS; // smallest node
    }

    StatusType result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, Employees, c->left, companies, counter);
    if(result != SUCCESS) return result;

    // add c
    companies[*counter] = c->element;
    (*counter)++;

    if(c->right != nullptr) {
        result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, Employees, c->right, companies, counter);
        if(result != SUCCESS) return result;
    }
    return SUCCESS; //check which status is required here
}

StatusType GetHighestEarnerInEachCompany(void *DS, int NumOfCompanies, int **Employees) {
    try {
        if((DS == nullptr) || (Employees == nullptr) || NumOfCompanies < 1) return INVALID_INPUT;
        *Employees = new int[NumOfCompanies]; // this is supposed to be deleted by 'them'

        Company** companies = new Company*[NumOfCompanies];
        int counter = 0;

        tree<Company>* c = ((DataStrcture*)DS)->company_head;
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

    StatusType result = GetNumEmployeesMatching_helper(e->left, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
    if(result != SUCCESS) return result;

    // add e
    if(e->id >= MinEmployeeID && e->id <= MaxEmployeeId)
        (*TotalNumOfEmployees)++;

    if(e->right != nullptr && e->right->id <= MaxEmployeeId) {
        result = GetNumEmployeesMatching_helper(e->right, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if(result != SUCCESS) return result;
    }
    return SUCCESS; //check which status is required here
}

StatusType GetNumEmployeesMatching(void *DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId,
                                   int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees) {
    if(DS == nullptr || TotalNumOfEmployees == nullptr || NumOfEmployees == nullptr || CompanyID == 0 ||
    MinEmployeeID < 0 || MaxEmployeeId < 0 || MinSalary < 0 || MinGrade < 0 || MinEmployeeID > MaxEmployeeId) {
        return INVALID_INPUT;
    }

    tree<Company>* c; // used to avoid calling it when not necessary:
    if(CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head, CompanyID);

    if((CompanyID < 0 && ((DataStrcture*)DS)->employee_head == nullptr) ||
       (CompanyID > 0 && c == nullptr) ||
       (CompanyID > 0 && c->element->employees_pointers == nullptr)) {
        return FAILURE;
    }

    if(CompanyID < 0) {
        StatusType result = GetNumEmployeesMatching_helper(((DataStrcture*)DS)->employee_head, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if(result != SUCCESS) return result;
    }
    else {
        StatusType result = GetNumEmployeesMatching_helper(c->element->employees_pointers, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if(result != SUCCESS) return result;
    }
    return SUCCESS;
}

void Quit(void **DS){
}
