//
// Created by omerg on 23/04/2022.
//

#include <memory>
#include "library1.h"
#include "Company.h"
#include "Employee.h"
using std::shared_ptr;

class DS{
shared_ptr<Company> company_head;
shared_ptr<Employee> employee_head;
Employee* highest_earner_employee;
};


void *Init();

StatusType AddCompany(void *DS, int CompanyID, int Value);

StatusType AddEmployee(void *DS, int EmployeeID, int CompanyID, int Salary, int Grade);

StatusType RemoveCompany(void *DS, int CompanyID);

StatusType RemoveEmployee(void *DS, int EmployeeID);

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