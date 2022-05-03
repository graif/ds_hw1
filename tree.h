//
// Created by omerg on 25/04/2022.
//

#ifndef DS_HW1_TREE_H
#define DS_HW1_TREE_H

#include <memory>
#include "library1.h"
#include <algorithm>

class Company;
template<class Element>
class tree {
public:
    int id; //iterator
    int height;
    Element* element;
    tree<Element> * left;
    tree<Element> * right;
    tree(int id) : id(id), height(1), element(nullptr), left(nullptr), right(nullptr) {};
    tree(int id, Element* element) : id(id), height(1), element(element), left(nullptr), right(nullptr) {};
    ~tree() {
        if(this->element) {
            delete element;
            this->element = nullptr;
        }
        this->left = nullptr;
        this->right = nullptr;
     };

     /**
      * the func clear the tree (deallocate element and tree node)
      * @tparam Element
      * @param tree
      */
     void clear(tree<Element> * tree){
         if(tree == nullptr){
             return;
         }
         clear(tree->left);
         clear(tree->right);

         delete tree;
         tree = nullptr;
     }

     /**
      * the func add element to a tree by iterator (can be id or salary in our cases)
      * @param e
      * @param iterator
      * @param status
      * @return
      */
    /*tree<Element> * addElement(Element* e,int iterator,bool is_salary, StatusType* status) {
        if(e == nullptr || iterator <= 0) {
            *status = INVALID_INPUT;
            return this;
        }
        try {
            tree<Element> *t = new tree(iterator,e);

            tree<Element>* T = addElementRecursively(this,t,iterator,is_salary, status);
            if(*status != SUCCESS) {
                delete t;
                return nullptr;
            }
            return T;
        }
        catch (std::bad_alloc &) {
            *status = ALLOCATION_ERROR;
            return this;
        }
    }*/
    /**
       * the func erase element from a tree by iterator (can be id or salary in our cases)
       * @param e
       * @param iterator
       * @param status
       * @return
       */
    tree<Element> *  eraseElement(Element* e,bool is_salary,StatusType* status) {
        // need to address deletion of head node at a higher scope (bahootz)
        if(id<= 0){
            *status=INVALID_INPUT;
            return this;
        }
        if(e == nullptr){
            *status=FAILURE;
            return this;
        }

        return deleteElementRecursively(this,e,is_salary,status);

    }

    /**
     * the func return the balance height of the curr node
     * @return
     */
    int getBalance() {
        if(this == nullptr) return 0;
        return (getHeight(this->left) - getHeight(this->right));
    }

};


/**
 * the func add element to a tree by iterator (can be id or salary in our cases)
 * @param e
 * @param iterator
 * @param status
 * @return
 */
 template <class Element>
tree<Element> * addElement(tree<Element> * head,Element* e,int iterator,bool is_salary, StatusType* status) {
    if(head == nullptr || e == nullptr || iterator <= 0) {
        *status = INVALID_INPUT;
        return head;
    }
    try {
        tree<Element> *t = new tree<Element>(iterator,e);
        tree<Element>* T = addElementRecursively(head,t,iterator,is_salary, status);
        if(*status != SUCCESS) {
            delete t;
            return nullptr;
        }
        return T;
    }
    catch (std::bad_alloc &) {
        *status = ALLOCATION_ERROR;
        return  head;
    }
}


/**
 * the func return the height of a node
 * @tparam Element
 * @param head
 * @return
 */
template<class Element>
int getHeight(tree<Element> * head){
    if(head == nullptr)
        return 0;
    return head->height;
}

/*int getMax(int a, int b) {
    return a > b ? a : b;
}*/

/**
 * the func get a tree and a element and return pointer for the daddy node of the element. if not found
 * the func return nullptr
 * @tparam Element
 * @param head
 * @param id
 * @return
 */
template<class Element>
tree<Element> *findMyDaddy(tree<Element> *head, int id) {
    if (head == nullptr) { // no head, employee is the new head
        return nullptr;
    } else {
        if (head->id > id) {
            if (head->left == nullptr) {
                return head;
            }
            return findMyDaddy(head->left, id);
        } else {
            if (head->right == nullptr) {
                return head;
            }
            return findMyDaddy(head->right, id);
        }
    }
}

/**
 *  the func get a tree and id and return the node  of the element. if not found
 * the func return nullptr
 * @tparam Element
 * @param head
 * @param id
 * @return
 */
template<class Element>
tree<Element> *findById(tree<Element> *head, int id) {
    if (head == nullptr) {
        return nullptr;
    }
    if (head->id == id) {
        return head;
    } else if (head->id < id && head->right != nullptr) {
        return findById(head->right, id);
    } else if (head->id > id && head->left != nullptr) {
        return findById(head->left, id);
    }
    return nullptr;
}

/*******************************************   rotate functions   ***********************************************************/
template <class Element>
tree<Element>* left_rot(tree<Element> * head) {
    tree<Element> * temp1=head->right;
    tree<Element> * temp2=temp1->left;
    temp1->left=head;
    head->right=temp2;

    //head->height = getMax(getHeight(head->left),getHeight(head->right))+1;
    //temp1->height = getMax(getHeight(temp1->left),getHeight(temp1->right))+1;
    int a = getHeight(head->left);
    int b = getHeight(head->right);
    head->height = (a > b ? a : b) + 1;

    a = getHeight(temp1->left);
    b = getHeight(temp1->right);
    temp1->height = (a > b ? a : b) + 1;

    return temp1;
}

template <class Element>
tree<Element>* right_rot(tree<Element> * head){
    tree<Element>* temp1=head->left;
    tree<Element>* temp2=temp1->right;
    temp1->right=head;
    head->left=temp2;

    //head->height=getMax(getHeight(head->left),getHeight(head->right))+1;
    //temp1->height=getMax(getHeight(temp1->left),getHeight(temp1->right))+1;
    int a = getHeight(head->left);
    int b = getHeight(head->right);
    head->height = (a > b ? a : b) + 1;
    a = getHeight(temp1->left);
    b = getHeight(temp1->right);
    temp1->height = (a > b ? a : b) + 1;

    return temp1;
}


template<class Element>
//not sure if it cover case of inserting element that will be head
tree<Element>* addElementRecursively(tree<Element>* head,tree<Element>* element_tree,int iterator,bool is_salary, StatusType* status)
{
    if(head == nullptr){
        return element_tree;
    }


    if (head->id > iterator) {
        head->left = addElementRecursively(head->left,element_tree,iterator,is_salary, status);
    }

    else if (head->id < iterator) {
        head->right = addElementRecursively(head->right,element_tree,iterator,is_salary, status);

 }
    else { // cant be equal

        if((!is_salary)){
            *status = FAILURE; // already exists in tree
            return head; // possibly element_tree
        }
        else if(head->element->id > element_tree->element->id){
            head->right = addElementRecursively(head->right,element_tree,iterator,is_salary, status);
            }

        else if(head->element->id < element_tree->element->id){
            head->left = addElementRecursively(head->left,element_tree,iterator,is_salary, status);
        }
    }

    *status = SUCCESS;
    int a = getHeight(head->left);
    int c = getHeight(head->right);
    head->height = (a > c ? a : c) + 1;
    //head->height = getMax(getHeight(head->left), getHeight(head->right)) +1;
    int b = head->getBalance();

    // LL
    if (head->left != nullptr && (b > 1 && iterator < head->left->id)) {
        return right_rot<Element>(head);
    }

    // RR
    if (head->right != nullptr && (b < -1 && iterator > head->right->id)) {
        return left_rot<Element>(head);
    }

    // LR
    if (head->left != nullptr && (b > 1 && iterator > head->left->id))
    {
        head->left = left_rot<Element>(head->left);
        return right_rot<Element>(head);
    }

    // RL
    if (head->right != nullptr && (b < -1 && iterator <  head->right->id))
    {
        head->right = right_rot<Element>(head->right);
        return left_rot<Element>(head);
    }

    // do nothing:
    return head;
}


/**
 * the func help the delete func to erase element recursively
 * @tparam Element
 * @param head
 * @param id
 * @param status
 * @return
 */
template <class Element>
tree<Element> * deleteElementRecursively( tree<Element> * head,Element* e,bool is_salary,StatusType* status) {

    if (head == nullptr || head->element == nullptr || e == nullptr) {
        *status = FAILURE; // already exists in tree
        return head;
    }
    //choose which id is the condition
    int id = e->id;
    if (is_salary)
        id = e->salary;

    if (id < head->id) {
        head->left = deleteElementRecursively(head->left,e, is_salary, status);
    } else if (id > head->id) {
        head->right = deleteElementRecursively(head->right,e, is_salary, status);
    } else if (is_salary && head->element->id != id) {
        id = e->id;
        if (id < head->id) {
            head->right = deleteElementRecursively(head->right, e, is_salary, status);
        } else if (id > head->id) {
            head->left = deleteElementRecursively(head->left, e, is_salary, status);
        }
    }
        //found the element to be deleted
    else {

        //case of 1/0 child
        tree<Element> *temp;
        if (head->left == nullptr || head->right == nullptr) {
            if (head->left == nullptr && head->right == nullptr) {
                //make sure that we don't have memory leak at that point, shared ptr suppose to free
                temp= head;
                head = nullptr;
            } else {
                temp = head->left ? head->left : head->right;
                head->element = temp->element;
                if (is_salary) {
                    head->id = temp->element->salary;
                } else
                    head->id = temp->id;
            }
            delete temp;

            *status = SUCCESS;
        }
            //2 child case
        else {
            temp = head->right;
            if(temp != nullptr) {
                while (temp->left != nullptr) {
                    temp = temp->left;
                }

                head->element = temp->element;
                if (is_salary) {
                    head->id = temp->element->salary;
                } else
                    head->id = temp->id;

                head->right = deleteElementRecursively(head->right,
                                                       temp->element, is_salary, status);
            }
        }
    }

    *status = SUCCESS;
    if (head == nullptr)
        return nullptr;


    int a = getHeight(head->left);
    int c = getHeight(head->right);
    head->height = (a > c ? a : c) + 1;
    //head->height = getMax(getHeight(head->left), getHeight(head->right)) +1;// valgrind error (getMax defined multiple times)
    int b = head->getBalance();

    // LL
    if (head->left!= nullptr && b > 1 && head->left->getBalance()>=0) {
        return right_rot<Element>(head);
    }

    // RR
    if (head->right!= nullptr && b < -1 && head->right->getBalance()<=0) {
        return left_rot<Element>(head);
    }

    // LR
    if (head->left!= nullptr && b > 1 &&head->left->getBalance()<0)
    {
        head->left = left_rot<Element>(head->left);
        return right_rot<Element>(head);
    }

    // RL
    if (head->right!= nullptr && b < -1 && head->right->getBalance()>0)
    {
        head->right = right_rot<Element>(head->right);
        return left_rot<Element>(head);
    }
    // do nothing:
    return head;

}



template <class Element>
tree<Element> * arrayToTree(Element ** array,int begin,int end){
    if(begin>end){
        return nullptr;
    }
    int mid=(begin+end)/2;
    tree<Element> * head = new tree<Element>((array[mid])->id,array[mid]);
    head->left = arrayToTree(array,begin,mid-1);
    head->right = arrayToTree(array,mid+1,end);

    int a = getHeight(head->left);
    int b = getHeight(head->right);
    head->height = (a > b ? a : b) + 1;
    //head->height = getMax(getHeight(head->left), getHeight(head->right)) +1; // valgrind error (getMax defined multiple times)
    return head;
}

//the func also delete the old values
template <class Element>
void treeToArray(tree<Element>* head, Element ** dest, int* index){
    if(head== nullptr){
        return;
    }
    treeToArray(head->left,dest,index);
    dest[*index]=head->element;
    //head->element = nullptr;
    (*index)++;
    treeToArray(head->right,dest,index);
    //head = nullptr;
    delete head;
}




template <class Element>
tree<Element>* CombineTree(tree<Element>* head1,tree<Element>* head2, int size1 , int size2,StatusType* status){
    try {
        Element** tree1 = new Element*[size1];
        Element** tree2 = new Element*[size2];
        int index = 0;
        treeToArray<Element>(head1, tree1, &index);
        index = 0;
        treeToArray<Element>(head2, tree2, &index);
        index = 0;

        Element** merged = new Element*[size1+size2];

        int i=0,j=0,k=0;
        while(i<size1&&j<size2){
            if(tree1[i]>tree2[j]){
                merged[k]=tree1[j];
                k++;
                i++;
            }
            else {
                merged[k]=tree1[i];
                k++;
                j++;
            }
        }
        while(j<size2){
            merged[k]=tree1[j];
            k++;
            i++;
        }
        while(i<size1){
            merged[k]=tree1[i];
            k++;
            i++;
        }

        tree<Element>* new_head= arrayToTree<Element>(merged,0,size2+size1-1);

        delete [] tree1;
        delete [] tree2;
        delete[] merged;

        return new_head;

    }
    catch (std::bad_alloc &) {
        *status = ALLOCATION_ERROR;
        return nullptr;
    }

}




#endif //DS_HW1_TREE_H
