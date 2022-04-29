//
// Created by omerg on 25/04/2022.
//

#ifndef DS_HW1_TREE_H
#define DS_HW1_TREE_H

#include <memory>
#include "library1.h"

using std::shared_ptr;


template<class Element>
class tree {
public:
    int id;
    int height;
    shared_ptr<Element> element;
    shared_ptr<tree<Element>> left;
    shared_ptr<tree<Element>> right;

    tree(int id) : id(id) {};
    tree(int id, Element* element) : id(id), element(shared_ptr<Element>(element)) {};
     ~tree() = default;

    tree<Element> * addElement(Element* e, StatusType* status) {
        if(e == nullptr || e->id <= 0) {
            *status = INVALID_INPUT;
            return this;
        }
        try {
            tree<Element> *t = new tree(e->id);
            tree<Element> *T = addElementRecursively(this,t, status);
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
    }

    tree<Element> *  eraseElement(int id,StatusType* status) {
        // need to address deletion of head node at a higher scope (bahootz)
        tree<Element> * element_tree= findById<Element>(this,id);
        if(id<= 0){
            *status=INVALID_INPUT;
            return this;
        }
        if(element_tree== nullptr){
            *status=FAILURE;
            return this;
        }

        return deleteElementRecursively(element_tree,id,status);

    }



    int getBalance() {
        return (getHeight(this->left) - getHeight(this->right));
    }

};
template <class Element>
tree<Element> * deleteElementRecursively( tree<Element> * head ,int id,StatusType* status) {
    if (head == nullptr) {
        return head;
    }
    //case of 1/0 child
    tree<Element> *temp;

    if (head->left == nullptr || head->right == nullptr) {
        if (head->left == nullptr && head->right == nullptr) {
            //make sure that we don't have memory leak at that point, shared ptr suppose to free
            head->element = nullptr;
            head = nullptr;
        } else {
            temp = head->left ? head->left.get() : head->right.get();
            head->element = temp->element;
            head->id = temp->id;
            temp->element = nullptr;
            temp = nullptr;
        }
        *status = SUCCESS;
    }
        //2 child case
    else {
        temp = head;
        while (temp->left != nullptr) {
            temp = temp->left.get();
        }
        head->element = temp->element;
        head->id = temp->id;

        head->right = shared_ptr<tree<Element>>(deleteElementRecursively
                (findById<Element>(head->right.get(),id), temp->id, status));
    }
    if(head== nullptr)
        return nullptr;
    *status = SUCCESS;
    head->height = getMax(getHeight(head->left), getHeight(head->right)) +1;
    int b = head->getBalance();

    // LL
    if (b > 1 && head->left.get()->getBalance()>=0) {
        return right_rot<Element>(head);
    }

    // RR
    if (b < -1 && head->right.get()->getBalance()<=0) {
        return left_rot<Element>(head);
    }

    // LR
    if (b > 1 &&head->left.get()->getBalance()<0)
    {
        head->left = shared_ptr<tree<Element>>(left_rot<Element>(head->left.get()));
        return right_rot<Element>(head);
    }

    // RL
    if (b < -1 && head->right.get()->getBalance()>0)
    {
        head->right = shared_ptr<tree<Element>>(right_rot<Element>(head->right.get()));
        return left_rot<Element>(head);
    }
    // do nothing:
    return head;

}

template<class Element>
int getHeight(shared_ptr<tree<Element>> head){
    if(head== nullptr)
        return 0;
    return head.get()->height;
}

template<class Element>
tree<Element> *findMyDaddy(tree<Element> *head, int id) {
    if (head == nullptr) { // no head, employee is the new head
        return nullptr;
    } else {
        if (head->id > id) {
            if (head->left == nullptr) {
                return head;
            }
            return findMyDaddy(head->left.get(), id);
        } else {
            if (head->right == nullptr) {
                return head;
            }
            return findMyDaddy(head->right.get(), id);
        }
    }
}

template<class Element>
tree<Element> *findById(tree<Element> *head, int id) {
    if (head == nullptr) {
        return nullptr;
    }
    if (head->id == id) {
        return head;
    } else if (head->id > id && head->right != nullptr) {
        return findById(head->right.get(), id);
    } else if (head->id < id && head->left != nullptr) {
        return findById(head->left.get(), id);
    }
    return nullptr;
}
/**
template<class Element>
StatusType addSubElement(tree<Element> *head, Element *elem, int id) {
    try {
        if (id <= 0 || head == nullptr) { // added check if head pointer is null, can't proceed
            return INVALID_INPUT; // need to make sure we never send null head pointer
        }
        if (findById(head, id) != nullptr) { // element already exists in tree
            return FAILURE;
        }
        tree<Element> *daddy_ptr = findMyDaddy(head, id);
        if (daddy_ptr->id > id) {
            daddy_ptr->left = std::shared_ptr<tree<Element>>(new tree<Element>(id, elem));
        } else {
            daddy_ptr->right = std::shared_ptr<tree<Element>>(new tree<Element>(id, elem));
        }
    }
    catch (std::bad_alloc &) {
        return ALLOCATION_ERROR;
    }
    return SUCCESS;
}
**/
template <class Element>
tree<Element>* left_rot(tree<Element>* head){
    shared_ptr<tree<Element>> temp1=head->right;
    shared_ptr<tree<Element>> temp2=temp1->left;
    temp1->left=shared_ptr<tree<Element>>(head);
    head->right=temp2;

    head->height=getMax(getHeight(head->left),getHeight(head->right))+1;
    temp1->height=getMax(getHeight(temp1->left),getHeight(temp1->right))+1;

    return temp1.get();
}

template <class Element>
tree<Element>* right_rot(tree<Element>* head){
    shared_ptr<tree<Element>> temp1=head->left;
    shared_ptr<tree<Element>> temp2=temp1->right;
    temp1->right=shared_ptr<tree<Element>>(head);
    head->left=temp2;

    head->height=getMax(getHeight(head->left),getHeight(head->right))+1;
    temp1->height=getMax(getHeight(temp1->left),getHeight(temp1->right))+1;

    return temp1.get();
}

template<class Element>
//not sure if it cover case of inserting element that will be head
tree<Element>* addElementRecursively(tree<Element>* head,tree<Element>* element_tree, StatusType* status)
{
    if(head== nullptr){
        return element_tree;
    }

    if (head->id > element_tree->id) {
        head->left = shared_ptr<tree<Element>>(addElementRecursively(head->left.get(),element_tree, status));
    }

    else if (head->id < element_tree->id) {
        head->right = shared_ptr<tree<Element>>(addElementRecursively(head->right.get(),element_tree, status));
 }
    else { // cant be equal
        *status = FAILURE; // already exists in tree
        return element_tree;
    }

    *status = SUCCESS;
    head->height = getMax(getHeight(head->left), getHeight(head->right)) +1;
    int b = head->getBalance();

    // LL
    if (b > 1 && element_tree->id < head->left.get()->id) {
        return right_rot<Element>(head);
    }

    // RR
    if (b < -1 && element_tree->id < head->right.get()->id) {
        return left_rot<Element>(head);
    }

    // LR
    if (b > 1 && element_tree->id > head->left.get()->id)
    {
        head->left = shared_ptr<tree<Element>>(left_rot<Element>(head->left.get()));
        return right_rot<Element>(head);
    }

    // RL
    if (b < -1 && element_tree->id <  head->right.get()->id)
    {
        head->right = shared_ptr<tree<Element>>(right_rot<Element>(head->right.get()));
        return left_rot<Element>(head);
    }

    // do nothing:
    return head;
}

int getMax(int a, int b){
    if(a>b)
        return a;
    return b;
}


#endif //DS_HW1_TREE_H
