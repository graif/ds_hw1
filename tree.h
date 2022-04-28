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
    Element *element;
    shared_ptr<tree> left;
    shared_ptr<tree> right;

    tree(int id) : id(id) {};
     ~tree() = default;

    tree<Element>* addElementRecursively(tree<Element>* node, StatusType* status)
    {
        if(node == nullptr)
            return nullptr;

        if (node->id < this->id) {
            this->left = this->addElementRecursively(this->left.get());
        }
        else if (node->id > this->id) {
            this->right = this->addElementRecursively(this->right.get());
        }
        else { // cant be equal
            return this;
        }

        *status = SUCCESS;
        this->height = getMax(getHeight(this->left.get()), getHeight(this->right.get()) + 1);
        int b = this->getBalance();

        // LL
        if (b > 1 && node->id < this->left.get()->id) {
            return right_rot<Element>(this);
        }

        // RR
        if (b < -1 && node->id > this->right.get()->id) {
            return  left_rot<Element>(this);
        }

        // LR
        if (b > 1 && node->id > this->left.get()->id)
        {
            this->left =shared_ptr<tree<Element>>(left_rot<Element>(this->left.get()));
            return right_rot<Element>(this);
        }

        // RL
        if (b < -1 && node->id < this->right->id)
        {
            this->right = shared_ptr<tree<Element>>(right_rot<Element>(this->right.get()));
            return left_rot<Element>(this);
        }

        // do nothing:
        return this;
    }

    tree<Element>* addElement(StatusType* status) {
        try {
            tree<Element> *t = new tree();
            tree<Element> *T2 = addElementRecursively(t, &status);
            if(*status != SUCCESS) {
                delete t;
                return nullptr;
            }
            return T2;
        }
        catch (std::bad_alloc &) {
            *status = ALLOCATION_ERROR;
            return nullptr;
        }
    }

    StatusType eraseElement(int id) {
        // need to address deletion of head node at a higher scope (bahootz)
    }

    int getBalance() {
        return (getHeight(this->left) - getHeight(this->right));
    }

};

int getMax(int a, int b){
    if(a>b)
        return a;
    return b;
}
template<class Element>
int getHeight(tree<Element> *head){
    if(head== nullptr)
        return 0;
    return head->height;
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

template <class Element>
tree<Element>* left_rot(tree<Element>* head){
    shared_ptr<tree<Element>> temp1=head->right;
    shared_ptr<tree<Element>> temp2=temp1->left;
    temp1->left=shared_ptr<tree<Element>>(head);
    head->right=temp2;

    head->height=getMax(getHeight(head->left.get()),getHeight(head->right.get()))+1;
    temp1->height=getMax(getHeight(temp1->left),getHeight(temp1->right))+1;

    return temp1;
}

template <class Element>
tree<Element>* right_rot(tree<Element>* head){
    shared_ptr<tree<Element>> temp1=head->left;
    shared_ptr<tree<Element>> temp2=temp1->right;
    temp1->right=shared_ptr<tree<Element>>(head);
    head->left=temp2;

    head->height=getMax(getHeight(head->left.get()),getHeight(head->right.get()))+1;
    temp1->height=getMax(getHeight(temp1->left),getHeight(temp1->right))+1;

    return temp1;
}


#endif //DS_HW1_TREE_H
