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
    Element *element;
    shared_ptr<tree> left;
    shared_ptr<tree> right;

    tree(int id, Element *element) : id(id), element(element) {};

    virtual ~tree() = default;
};

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


#endif //DS_HW1_TREE_H
