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

    ~DataStrcture(){};

    int employee_count;
    tree<Company> *company_head;
    tree<Employee> *employee_head;
    Employee *highest_earner_employee;
    tree<Employee> *employees_pointers_by_salary;
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
    Company *c;
    try {
        DataStrcture *DSS = ((DataStrcture *) DS);
        if (DS == nullptr || CompanyID <= 0 || Value <= 0) {
            return INVALID_INPUT;
        }

        c = new Company(Value, CompanyID);
        if (((DataStrcture *) DS)->company_head == nullptr) {
            ((DataStrcture *) DS)->company_head = new tree<Company>(CompanyID, c);
        } else {
            StatusType status = FAILURE;
            tree<Company> *temp = addElement(DSS->company_head, c, c->id, false, &status); //valgrind segfault cause
            if (status != SUCCESS) {
                return status;
            }
            if (((DataStrcture *) DS)->company_head->id != temp->id) {
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
Employee *UpdateHighestEarner(tree<Employee> *c) {
    if (c == nullptr)
        return nullptr;

    while (c->right) {
        c = c->right;
    }
    return c->element;
}

StatusType AddEmployee_C(void *DS, int EmployeeID, Company *c, int Salary, int Grade) {

      //  Employee *e = new Employee(EmployeeID, c, Salary, Grade);
    if ((DS == nullptr) || (EmployeeID <= 0) || (c->id <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }
    int counter = 0;
    DataStrcture *DSS = (DataStrcture *) DS;
    if (((DataStrcture *) DS)->company_head == nullptr) return FAILURE;
    if (c == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee
    tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    if (e != nullptr) return FAILURE; // employee already in tree

    try {
        Employee *e = new Employee(EmployeeID, c, Salary, Grade);
        if (((DataStrcture *) DS)->employee_head == nullptr) {
            ((DataStrcture *) DS)->employee_head = new tree<Employee>(EmployeeID, e);
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(DSS->employee_head, e, EmployeeID, false, &status);
            if (status != SUCCESS) {
                delete e;
                return status;
            }
            ((DataStrcture *) DS)->employee_head = temp;
        }

        /*** add the employee's pointer to the pointers_employees tree: **/
        /* if employees pointers is empty, initialize one */
        tree<Employee> *t = findById(((DataStrcture *) DS)->employee_head, EmployeeID);

        if (t == nullptr) {
            std::cout << "t is null for some stupid reason (EmployeeID: " << EmployeeID << ")" << std::endl;
            delete e;
            return FAILURE;
        }

        if (c->employees_pointers == nullptr) {
            tree<Employee> *ba = new tree<Employee>(EmployeeID, t->element);
            c->employees_pointers = ba;
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(c->employees_pointers, t->element, EmployeeID, false, &status);
            if (status != SUCCESS) return status;
            c->employees_pointers = temp;
        }

        /*** add the employee's pointer by SALARY: **/
        if (c->employees_pointers_by_salary == nullptr) {
            tree<Employee> *ba2 = new tree<Employee>(Salary, t->element);
            c->employees_pointers_by_salary = ba2;
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(c->employees_pointers_by_salary, t->element, Salary, true,
                                              &status);
            if (status != SUCCESS) {
                return status;
            }
            c->employees_pointers_by_salary = temp;
        }

        /*** add the employee's pointer to the general employee by SALARY tree ***/
        if (((DataStrcture *) DS)->employees_pointers_by_salary == nullptr) {
            tree<Employee> *ba3 = new tree<Employee>(Salary, t->element);
            ((DataStrcture *) DS)->employees_pointers_by_salary = ba3;
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(((DataStrcture *) DS)->employees_pointers_by_salary, t->element, Salary,
                                              true, &status);
            if (status != SUCCESS) return status;
            ((DataStrcture *) DS)->employees_pointers_by_salary = temp;
        }
        ((DataStrcture *) DS)->highest_earner_employee = UpdateHighestEarner(
                ((DataStrcture *) DS)->employees_pointers_by_salary);
        c->highest_earner_employee = UpdateHighestEarner(c->employees_pointers_by_salary);

        (((DataStrcture *) DS)->employee_count)++;
        (c->employee_count)++; // raise the company's employee count by 1

        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
        /// remember to delete previous steps if failed mid-way
    }
}

template<class Element>
void countery(tree<Element> *e, int *counter) {
    if (e == nullptr)
        return;
    countery(e->left, counter);
    (*counter)++;
    countery(e->right, counter);
}

StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (CompanyID <= 0) || (Salary <= 0) || (Grade < 0)) {
        return INVALID_INPUT;
    }
    int counter = 0;
    DataStrcture *DSS = (DataStrcture *) DS;
    if (((DataStrcture *) DS)->company_head == nullptr) return FAILURE;
    tree<Company> *c = findById(((DataStrcture *) DS)->company_head, CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee
    tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    if (e != nullptr) return FAILURE; // employee already in tree

    try {
        Employee *e = new Employee(EmployeeID, c->element, Salary, Grade);
        if (((DataStrcture *) DS)->employee_head == nullptr) {
            ((DataStrcture *) DS)->employee_head = new tree<Employee>(EmployeeID, e);
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(DSS->employee_head, e, EmployeeID, false, &status);
            if (status != SUCCESS) {
                delete e;
                return status;
            }
            ((DataStrcture *) DS)->employee_head = temp;
        }

        /*** add the employee's pointer to the pointers_employees tree: **/
        /* if employees pointers is empty, initialize one */
        tree<Employee> *t = findById(((DataStrcture *) DS)->employee_head, EmployeeID);

        if (t == nullptr) {
            std::cout << "t is null for some stupid reason (EmployeeID: " << EmployeeID << ")" << std::endl;
            delete e;
            return FAILURE;
        }

        if (c->element->employees_pointers == nullptr) {
            tree<Employee> *ba = new tree<Employee>(EmployeeID, t->element);
            c->element->employees_pointers = ba;
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(c->element->employees_pointers, t->element, EmployeeID, false, &status);
            if (status != SUCCESS) return status;
            c->element->employees_pointers = temp;
            countery(c->element->employees_pointers, &counter);
        }

        /*** add the employee's pointer by SALARY: **/
        if (c->element->employees_pointers_by_salary == nullptr) {
            tree<Employee> *ba2 = new tree<Employee>(Salary, t->element);
            c->element->employees_pointers_by_salary = ba2;
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(c->element->employees_pointers_by_salary, t->element, Salary, true,
                                              &status);
            if (status != SUCCESS) {
                return status;
            }
            c->element->employees_pointers_by_salary = temp;
        }

        /*** add the employee's pointer to the general employee by SALARY tree ***/
        if (((DataStrcture *) DS)->employees_pointers_by_salary == nullptr) {
            tree<Employee> *ba3 = new tree<Employee>(Salary, t->element);
            ((DataStrcture *) DS)->employees_pointers_by_salary = ba3;
        } else {
            StatusType status = FAILURE;
            tree<Employee> *temp = addElement(((DataStrcture *) DS)->employees_pointers_by_salary, t->element, Salary,
                                              true, &status);
            if (status != SUCCESS) return status;
            ((DataStrcture *) DS)->employees_pointers_by_salary = temp;
        }
        ((DataStrcture *) DS)->highest_earner_employee = UpdateHighestEarner(
                ((DataStrcture *) DS)->employees_pointers_by_salary);
        c->element->highest_earner_employee = UpdateHighestEarner(c->element->employees_pointers_by_salary);

        (((DataStrcture *) DS)->employee_count)++;
        (c->element->employee_count)++; // raise the company's employee count by 1

        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
        /// remember to delete previous steps if failed mid-way
    }
}

StatusType RemoveCompany(void *DS, int CompanyID) {
    try {
        if (CompanyID <= 0 || DS == nullptr) {
            return INVALID_INPUT;
        }
        tree<Company> *c = findById(((DataStrcture *) DS)->company_head, CompanyID);
        if (c == nullptr || c->element->employees_pointers != nullptr)
            return FAILURE; // company doesn't exist, nowhere to add employee
        StatusType status = FAILURE;
        tree<Company> *temp = ((DataStrcture *) DS)->company_head->eraseElement(c->element, false,true, &status);
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


StatusType RemoveEmployee(void *DS, int EmployeeID) {
    try {
        if (EmployeeID <= 0 || DS == nullptr) {
            return INVALID_INPUT;
        }
        //DataStrcture *DSS = (DataStrcture *) DS;

        tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
        if (e == nullptr || e->element == nullptr) return FAILURE; // company doesn't exist, nowhere to add employee
        Employee *ele = e->element;
        Company *c = e->element->company;
        StatusType status = FAILURE;
        //delete emp pntr by salary DS
        tree<Employee> *temp = ((DataStrcture *) DS)->employees_pointers_by_salary->eraseElement(ele, true,false, &status);
        if (status != SUCCESS) {
            return status;
        }
        ((DataStrcture *) DS)->employees_pointers_by_salary = temp;

        //delete emp pntr Company
        temp = c->employees_pointers->eraseElement(ele, false,false, &status);

        if (status != SUCCESS) {
            return status;
        }
        c->employees_pointers = temp;

        //delete emp pntr by salary Company
        temp = c->employees_pointers_by_salary->eraseElement(ele, true,false, &status);

        if (status != SUCCESS) {
            return status;
        }
        c->employees_pointers_by_salary = temp;

        //delete emp pntr DS
        temp = ((DataStrcture *) DS)->employee_head->eraseElement(ele, false,false, &status);
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
        ((DataStrcture *) DS)->highest_earner_employee = UpdateHighestEarner(
                ((DataStrcture *) DS)->employees_pointers_by_salary);
        c->highest_earner_employee = UpdateHighestEarner(c->employees_pointers_by_salary);
        delete ele;
        return SUCCESS;
    }

    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
        /// remember to delete previous steps if failed mid-way
    }

}

StatusType GetCompanyInfo(void *DS, int CompanyID, int *Value, int *NumEmployees) {
    if ((DS == nullptr) || (CompanyID <= 0) || (Value == nullptr) || (NumEmployees == nullptr)) {
        return INVALID_INPUT;
    }
    tree<Company> *c = findById(((DataStrcture *) DS)->company_head, CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist

    *Value = c->element->value;
    *NumEmployees = c->element->employee_count;

    return SUCCESS;
}

StatusType GetEmployeeInfo(void *DS, int EmployeeID, int *EmployerID, int *Salary, int *Grade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (Salary == nullptr) || (Grade == nullptr)) {
        return INVALID_INPUT;
    }
    tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    if (e == nullptr) return FAILURE; // employee doesn't exist

    *EmployerID = e->element->company->id;
    *Salary = e->element->salary;
    *Grade = e->element->grade;

    return SUCCESS;
}

StatusType IncreaseCompanyValue(void *DS, int CompanyID, int ValueIncrease) {
    if ((DS == nullptr) || (CompanyID <= 0) || (ValueIncrease <= 0)) {
        return INVALID_INPUT;
    }
    tree<Company> *c = findById(((DataStrcture *) DS)->company_head, CompanyID);
    if (c == nullptr) return FAILURE; // company doesn't exist

    c->element->value += ValueIncrease;
    return SUCCESS;
}

StatusType PromoteEmployee(void *DS, int EmployeeID, int SalaryIncrease, int BumpGrade) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (SalaryIncrease <= 0)) {
        return INVALID_INPUT;
    }
    tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    if (e == nullptr) return FAILURE; // employee doesn't exist

    int temp_salary = e->element->salary + SalaryIncrease;
    int temp_grade = e->element->grade;
    if (BumpGrade > 0) {
        temp_grade++;
    }

    Company *c = e->element->company;
    RemoveEmployee(DS, EmployeeID);
    AddEmployee_C(DS, EmployeeID, c, temp_salary, temp_grade);

    return SUCCESS;
}

StatusType HireEmployee(void *DS, int EmployeeID, int NewCompanyID) {
    if ((DS == nullptr) || (EmployeeID <= 0) || (NewCompanyID <= 0)) {
        return INVALID_INPUT;
    }

    tree<Employee> *e = findById(((DataStrcture *) DS)->employee_head, EmployeeID);
    tree<Company> *c = findById(((DataStrcture *) DS)->company_head, NewCompanyID);
    if (c == nullptr || e == nullptr || e->element->company->id == NewCompanyID) return FAILURE;

    // delete employee from previous company's pointers tree:
    StatusType status;

    try {
        Employee *new_emp = new Employee(e->element->id, e->element->company, e->element->salary, e->element->grade);
        status = RemoveEmployee(DS, e->id);
        if (status != SUCCESS) {
            delete new_emp;
            return status;
        }
        status = AddEmployee_C(DS, new_emp->id, c->element, new_emp->salary, new_emp->grade);
        delete new_emp;

        return status;

    }

    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
}

void printEmployees(Employee** employees, int size) {
    std::cout << "[";
    for(int i = 0; i < size; i++) {
        std::cout << (employees[i])->id << ",";
    }
    std::cout << "]" << std::endl;
}

void printEmployeesSalary(Employee** employees, int size) {
    std::cout << "printingEmployeesSalary:" << std::endl;
    std::cout << "[";
    for(int i = 0; i < size; i++) {
        std::cout << (employees[i])->salary << ",";
    }
    std::cout << "]" << std::endl;
}


tree<Employee> *arrayToTree(Employee **array, int begin, int end) {
    if (begin > end) {
        return nullptr;
    }
    int mid = (begin + end) / 2;

    tree<Employee> *head = new tree<Employee>(array[mid]->id, array[mid]);
    head->left = arrayToTree(array, begin, mid - 1);
    head->right = arrayToTree(array, mid + 1, end);

    int a = getHeight(head->left);
    int b = getHeight(head->right);
    head->height = (a > b ? a : b) + 1;

    //head->height = getMax(getHeight(head->left), getHeight(head->right)) +1; // valgrind error (getMax defined multiple times)
    return head;
}

tree<Employee> *
CombineTree(Company * comp, tree<Employee> *head1, tree<Employee> *head2, int size1, int size2, StatusType *status, bool isSalary) {


    Employee **tree1 = new Employee *[size1];
    Employee **tree2 = new Employee *[size2];
    int index = 0;
    treeToArray<Employee>(head1, tree1, &index);
    int index2 = 0;
    treeToArray<Employee>(head2, tree2, &index2);

    clear(head1);
    clear(head2);
/*
    if(!isSalary) {
        printEmployees(tree1, index);
        printEmployees(tree2, index2);
    }
    else {
        printEmployeesSalary(tree1, index);
        printEmployeesSalary(tree2, index2);
    }

     std::cout << "size1: " << size1 << " index1: " << index << " size2: " << size2 << " index2: " << index2 << std::endl;
     */
    index = 0;
    index2 = 0;

    Employee **merged = new Employee *[size1 + size2];
    int i = 0, j = 0, k = 0;
    while ((i < size1) && (j < size2)) {
        if (!isSalary) {
            if (tree1[i]->id > tree2[j]->id) {
                merged[k] = tree2[j];
                merged[k]->company = comp;
                j++;
            } else {
                merged[k] = tree1[i];
                merged[k]->company = comp;
                i++;
            }
        } else {
            //   std::cout << "k: " << k << std::endl;

            // std::cout << "tree1[0]->salary: " << ((Employee*)tree1[0])->salary << std::endl;
            //    std::cout << "tree2[0]->salary: " << ((Employee*)tree2[0])->salary  << std::endl;
            //   std::cout << "tree1[0]->id: " << ((Employee*)tree1[0])->id  << std::endl;
            //     std::cout << "tree2[0]->id: " << ((Employee*)tree2[0])->id << std::endl;
            if (tree1[i]->salary > tree2[j]->salary) { // problem
                merged[k] = tree2[j];
                merged[k]->company = comp;
                j++;
            } else if (tree1[i]->salary < tree2[j]->salary) {
                merged[k] = tree1[i];
                merged[k]->company = comp;
                i++;
            } else { // equal
                if (tree1[i]->id < tree2[j]->id) {
                    merged[k] = tree2[j];
                    merged[k]->company = comp;
                    j++;
                } else {
                    merged[k] = tree1[i];
                    merged[k]->company = comp;
                    i++;
                }
            }
        }
        k++;
    }

    std::cout << "size1: " << size1 << " index1: " << i << " size2: " << size2 << " index2: " << j << std::endl;
    while (j < size2) {
        merged[k] = tree2[j]; // tree1[j] invalid read
        merged[k]->company = comp;  // tree1[j]->company invalid write
        k++;
        j++;
    }
    while (i < size1) {
        merged[k] = tree1[i];
        merged[k]->company = comp;
        k++;
        i++;
    }
/*
    if(!isSalary) printEmployees(merged, size1+size2);
    else printEmployeesSalary(merged, size1+size2);
*/
    tree<Employee> *new_head = arrayToTree(merged, 0, size2 + size1 - 1);

    /*for (int i = 0; i < size1; i++) {
        delete tree1[i];
    }*/
    delete[] tree1;
    /*for (int i = 0; i < size2; i++) {
        delete tree2[i];
    }*/
    delete [] tree2;
    /*for (int i = 0; i < size1 + size2; i++) {
        delete merged[i];
    }*/
    delete [] merged;

    return new_head;


}


StatusType AcquireCompany(void *DS, int AcquirerID, int TargetID, double Factor) {
    try {
        if (DS == nullptr || AcquirerID <= 0 || TargetID <= 0 || AcquirerID == TargetID || Factor < 1.00) {
            return INVALID_INPUT;
        }
        DataStrcture *DSS = (DataStrcture *) DS;
        tree<Company> *Acquirer = findById(((DataStrcture *) DS)->company_head, AcquirerID);
        tree<Company> *Target = findById(((DataStrcture *) DS)->company_head, TargetID);
        if (Acquirer == nullptr || Target == nullptr || Acquirer->element->value < (10 * Target->element->value)) {
            return FAILURE;
        }
        Company* Acquirer_c=Acquirer->element;
        Company* Target_c=Target->element;

        if(AcquirerID == 321 && TargetID == 330) {
            int hi = 5;
        }

        // add acquisition here
        int temp_val = (int) ((Acquirer->element->value + Target->element->value) * Factor);
        Employee *temp_highest_earner = Acquirer->element->highest_earner_employee;

        if (Target->element->employees_pointers != nullptr) {
             if(Acquirer->element->highest_earner_employee!= nullptr){
            if (Target->element->highest_earner_employee->salary >=
                Acquirer->element->highest_earner_employee->salary) {
                if (Target->element->highest_earner_employee->salary ==
                    Acquirer->element->highest_earner_employee->salary) {
                    if (Target->element->highest_earner_employee->id < Acquirer->element->highest_earner_employee->id) {
                        temp_highest_earner = Target->element->highest_earner_employee;
                    }
                } else {
                    temp_highest_earner = Target->element->highest_earner_employee;
                }
            }
        } else
                 temp_highest_earner = Target->element->highest_earner_employee;

            int temp_emp_count = Acquirer->element->employee_count + Target->element->employee_count;

            StatusType status = FAILURE; // not really used
            tree<Employee> *merged_emp_ptr = CombineTree(Acquirer->element,Acquirer->element->employees_pointers,
                                                         Target->element->employees_pointers,
                                                         Acquirer->element->employee_count,
                                                         Target->element->employee_count, &status, false);

            Target_c->employees_pointers= nullptr;
            Acquirer_c->employees_pointers = merged_emp_ptr;

            tree<Employee> *merged_emp_salary = CombineTree
                    (Acquirer_c,Acquirer_c->employees_pointers_by_salary, Target_c->employees_pointers_by_salary,
                     Acquirer_c->employee_count, Target_c->employee_count, &status, true);

            Target_c->employees_pointers_by_salary= nullptr;
            Acquirer_c->employees_pointers_by_salary = merged_emp_salary;
            Acquirer_c->highest_earner_employee = temp_highest_earner;
            Acquirer_c->employee_count = temp_emp_count;
        }
        else {
            StatusType status = RemoveCompany(DS,TargetID);
            if(status != SUCCESS) return status;

        }
        Acquirer_c->value = temp_val;

        return SUCCESS;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

StatusType GetHighestEarner(void *DS, int CompanyID, int *EmployeeID) {
    if ((DS == nullptr) || (EmployeeID == nullptr) || (CompanyID == 0)) {
        return INVALID_INPUT;
    }

    tree<Company> *c; // used to avoid calling it when not necessary:
    if (CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head, CompanyID);

    if ((CompanyID < 0 && ((DataStrcture *) DS)->employee_head == nullptr) ||
        (CompanyID > 0 && c == nullptr) ||
        (CompanyID > 0 && c->element->employees_pointers == nullptr)) {
        return FAILURE;
    }

    if (CompanyID < 0) {
        *EmployeeID = ((DataStrcture *) DS)->highest_earner_employee->id;
    } else {
        *EmployeeID = c->element->highest_earner_employee->id;
    }

    return SUCCESS;
}

StatusType GetAllEmployeesBySalary_helper(tree<Employee> *e, int **Employees, int *NumOfEmployees) {
    //if(*counter >= NumOfCompanies) return SUCCESS;
    if (e == nullptr)
        return SUCCESS;

    StatusType result = GetAllEmployeesBySalary_helper(e->right, Employees, NumOfEmployees);
    if (result != SUCCESS) return result;

    (*(Employees))[*NumOfEmployees] = e->element->id;
    (*NumOfEmployees)++;

    result = GetAllEmployeesBySalary_helper(e->left, Employees, NumOfEmployees);
    if (result != SUCCESS) return result;

    return SUCCESS;
}

StatusType GetAllEmployeesBySalary(void *DS, int CompanyID, int **Employees, int *NumOfEmployees) {
    try {
        if ((DS == nullptr) || (Employees == nullptr) || (NumOfEmployees == nullptr) || (CompanyID == 0)) {
            return INVALID_INPUT;
        }
        *NumOfEmployees = 0;

        tree<Company> *c; // used to avoid calling it when not necessary:
        if (CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head, CompanyID);
        if (((CompanyID < 0) && (((DataStrcture *) DS)->employee_head == nullptr)) ||
            ((CompanyID > 0) && (c == nullptr)) || ((CompanyID > 0) && (c->element->employees_pointers == nullptr))) {
            return FAILURE;
        }

        if (CompanyID > 0) {
            *NumOfEmployees = c->element->employee_count;
        } else { // we check if 0 earlier
            *NumOfEmployees = ((DataStrcture *) DS)->employee_count;
        }

        *Employees = (int *) malloc(sizeof(int) * (*NumOfEmployees)); // this is supposed to be deleted by 'them'

        int index = 0;
        if (CompanyID < 0) {
            // get an ordered list of employees by salary from all employees
            StatusType result = GetAllEmployeesBySalary_helper(((DataStrcture *) DS)->employees_pointers_by_salary,
                                                               Employees, &index);
            if (result != SUCCESS) return result;
        } else {
            // get an ordered list of employees by salary from all employees in the company
            StatusType result = GetAllEmployeesBySalary_helper(c->element->employees_pointers_by_salary, Employees,
                                                               &index);
            if (result != SUCCESS) return result;
        }
        *NumOfEmployees = index;
        if (index + 1 != *NumOfEmployees) {
            //std::cout << "index: " << index + 1 << " NumOfEmployees: " << *NumOfEmployees << std::endl;
        }
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

/*StatusType GetHighestEarnerInEachCompany_helper(void *DS, int NumOfCompanies, int **Employees, tree<Company>* c, Company** companies, int* counter) {
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
    if(c != nullptr) {
        companies[*counter] = c->element;
        (*counter)++;
    }

    if(c->right != nullptr) {
        result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, Employees, c->right, companies, counter);
        if(result != SUCCESS) return result;
    }
    return SUCCESS; //check which status is required here
}*/

StatusType GetHighestEarnerInEachCompany_helper(void *DS, int NumOfCompanies, tree<Company> *c, Company **companies,
                                                int *counter) {
    if (c == nullptr) return SUCCESS;

    StatusType result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, c->left, companies, counter);

    if (result != SUCCESS) return result;

    if (*counter < NumOfCompanies) {
        // add c
        companies[*counter] = c->element; // make sure each company has at least one employee
        (*counter)++;
    }
    result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, c->right, companies, counter);

    if (result != SUCCESS) return result;

    return SUCCESS; //check which status is required here
}

StatusType GetHighestEarnerInEachCompany(void *DS, int NumOfCompanies, int **Employees) {
    try {
        if ((DS == nullptr) || (Employees == nullptr) || NumOfCompanies < 1) return INVALID_INPUT;
        *Employees = (int *) malloc(sizeof(int) * (NumOfCompanies)); // this is supposed to be deleted by 'them'

        Company **companies = new Company *[NumOfCompanies];
        int counter = 0;

        tree<Company> *c = ((DataStrcture *) DS)->company_head;
        StatusType result = GetHighestEarnerInEachCompany_helper(DS, NumOfCompanies, c, companies, &counter);
        if (result != SUCCESS) {
            delete[] companies;
            /*for(int i = 0; i < counter; i++) {
                delete companies[i];
            }
            delete[] companies;*/
            return result;
        }
        if (counter != NumOfCompanies) {
            delete[] companies;
            /*for(int i = 0; i < counter; i++) {
                delete companies[i];
            }
            delete[] companies;*/
            return FAILURE;
        }

        for (int i = 0; i < NumOfCompanies; i++) {
            (*(Employees))[i] = companies[i]->highest_earner_employee->id;
        }

        /*for(int i = 0; i < NumOfCompanies; i++) {
            delete companies[i];
        }
        delete[] companies;*/
        delete[] companies;
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}

/*StatusType GetNumEmployeesMatching_helper(tree<Employee> *e, int MinEmployeeID, int MaxEmployeeId, int *TotalNumOfEmployees) {
    if (e->left == nullptr || e->left->id < MinEmployeeID) {
        // add e
        if (e->id >= MinEmployeeID && e->id <= MaxEmployeeId)
            (*TotalNumOfEmployees)++;
        return SUCCESS; // smallest node
    }

    StatusType result = GetNumEmployeesMatching_helper(e->left, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
    if (result != SUCCESS) return result;

    // add e
    if (e->id >= MinEmployeeID && e->id <= MaxEmployeeId)
        (*TotalNumOfEmployees)++;

    if (e->right != nullptr && e->right->id <= MaxEmployeeId) {
        result = GetNumEmployeesMatching_helper(e->right, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees);
        if (result != SUCCESS) return result;
    }
    return SUCCESS; //check which status is required here
}*/

StatusType
GetNumEmployeesMatching_helper(tree<Employee> *e, int MinEmployeeID, int MaxEmployeeId, int *TotalNumOfEmployees,
                               int *NumOfEmployees, int MinSalary, int MinGrade) {
    if (e == nullptr) {
        return SUCCESS;
    }

    StatusType result = GetNumEmployeesMatching_helper(e->left, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees,
                                                       NumOfEmployees, MinSalary, MinGrade);
    if (result != SUCCESS) return result;

    // add e
    if (e->id >= MinEmployeeID && e->id <= MaxEmployeeId) {
        (*TotalNumOfEmployees)++;
        if (e->element->salary >= MinSalary && e->element->grade >= MinGrade) {
            (*NumOfEmployees)++;
        }
    }

    result = GetNumEmployeesMatching_helper(e->right, MinEmployeeID, MaxEmployeeId, TotalNumOfEmployees, NumOfEmployees,
                                            MinSalary, MinGrade);
    if (result != SUCCESS) return result;

    return SUCCESS; //check which status is required here
}

StatusType GetNumEmployeesMatching(void *DS, int CompanyID, int MinEmployeeID, int MaxEmployeeId,
                                   int MinSalary, int MinGrade, int *TotalNumOfEmployees, int *NumOfEmployees) {
    if (DS == nullptr || TotalNumOfEmployees == nullptr || NumOfEmployees == nullptr || CompanyID == 0 ||
        MinEmployeeID < 0 || MaxEmployeeId < 0 || MinSalary < 0 || MinGrade < 0 || MinEmployeeID > MaxEmployeeId) {
        return INVALID_INPUT;
    }

    *NumOfEmployees = 0;
    *TotalNumOfEmployees = 0;

    tree<Company> *c; // used to avoid calling it when not necessary:
    if (CompanyID > 0) c = findById(((DataStrcture *) DS)->company_head, CompanyID);

    if ((CompanyID < 0 && ((DataStrcture *) DS)->employee_head == nullptr) ||
        (CompanyID > 0 && c == nullptr) ||
        (CompanyID > 0 && c->element->employees_pointers == nullptr)) {
        return FAILURE;
    }

    if (CompanyID < 0) {
        StatusType result = GetNumEmployeesMatching_helper(((DataStrcture *) DS)->employee_head, MinEmployeeID,
                                                           MaxEmployeeId, TotalNumOfEmployees, NumOfEmployees,
                                                           MinSalary, MinGrade);
        if (result != SUCCESS) return result;
    } else {
        StatusType result = GetNumEmployeesMatching_helper(c->element->employees_pointers, MinEmployeeID, MaxEmployeeId,
                                                           TotalNumOfEmployees, NumOfEmployees, MinSalary, MinGrade);
        if (result != SUCCESS) return result;
    }
    return SUCCESS;
}

/*
void Quit_Helper(void **DS, tree<Company>* &curr) {
    if(curr == nullptr) return;

    Quit_Helper(DS, curr->left);
    Quit_Helper(DS, curr->right);

    clear(curr->element->employees_pointers_by_salary);
    curr->element->employees_pointers = nullptr;
    clear(curr->element->employees_pointers);
    curr->element->employees_pointers_by_salary = nullptr;
    delete curr->element;
    curr->element = nullptr;
    delete curr;
}

void Quit(void **DS) {
    tree<Company>* &c = ((DataStrcture*)DS)->company_head;
    Quit_Helper(DS,c);
    ((DataStrcture*)DS)->company_head = nullptr;
    clearAll(((DataStrcture*)DS)->employee_head);
    ((DataStrcture*)DS)->employee_head = nullptr;
    clear(((DataStrcture*)DS)->employees_pointers_by_salary);
    ((DataStrcture*)DS)->employees_pointers_by_salary = nullptr;
    ((DataStrcture*)DS)->highest_earner_employee = nullptr;
    delete ((DataStrcture*)DS);
}
*/


void Quit_Helper(void *DS, tree<Company>* &curr) {
    if(curr == nullptr) return;

    Quit_Helper(DS, curr->left);
    curr->left= nullptr;
    Quit_Helper(DS, curr->right);

    clear(curr->element->employees_pointers_by_salary);
    curr->element->employees_pointers_by_salary = nullptr;
    clear(curr->element->employees_pointers);
    curr->element->employees_pointers = nullptr;
    delete curr->element;
    curr->element = nullptr;
    delete curr;
}

void Quit(void **DS) {
    DataStrcture* DSS=((DataStrcture*)*DS);
    tree<Company>* &c = DSS->company_head;
    Quit_Helper(DSS,c);
    DSS->company_head = nullptr;
    clear(DSS->employees_pointers_by_salary);
    DSS->employees_pointers_by_salary = nullptr;
    clearAll(DSS->employee_head);
    DSS->employee_head = nullptr;
    DSS->highest_earner_employee = nullptr;
    delete DSS;
    *DS = nullptr;
}