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
    int id; //iterator
    int height;
    shared_ptr<Element> element;
    shared_ptr<tree<Element>> left;
    shared_ptr<tree<Element>> right;

    tree(int id) : id(id) {};
    tree(int id, Element* element) : id(id), element(shared_ptr<Element>(element)) {};
     ~tree() {
             clear<Element>(this->left.get());
             clear<Element>(this->right.get());
             this->element= nullptr;
             free(this);
     };

     /**
      * the func clear the tree (deallocate element and tree node)
      * @tparam Element
      * @param tree
      */
    template<class Element>
     void clear(tree<Element>* tree){
         if(tree== nullptr){
             return;
         }
        if(tree->left!= nullptr){
            tree->element= nullptr;
            tree = nullptr;
            return;
        }
        if(tree->right!= nullptr){
            tree->element= nullptr;
            tree= nullptr;
            return;
        }
        tree->element= nullptr;
        tree= nullptr;
        return;

     }

     /**
      * the func add element to a tree by iterator (can be id or salary in our cases)
      * @param e
      * @param iterator
      * @param status
      * @return
      */
    tree<Element> * addElement(Element* e,int iterator, StatusType* status) {
        if(e == nullptr || iterator <= 0) {
            *status = INVALID_INPUT;
            return this;
        }
        try {
            tree<Element> *t = new tree(iterator);
            tree<Element> *T = addElementRecursively(this,t,iterator, status);
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
    /**
       * the func erase element from a tree by iterator (can be id or salary in our cases)
       * @param e
       * @param iterator
       * @param status
       * @return
       */
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

    /**
     * the func return the balance height of the curr node
     * @return
     */
    int getBalance() {
        return (getHeight(this->left) - getHeight(this->right));
    }

};

/**
 * the func help the delete func to erase element recursively
 * @tparam Element
 * @param head
 * @param id
 * @param status
 * @return
 */
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

/**
 * the func return the height of a node
 * @tparam Element
 * @param head
 * @return
 */
template<class Element>
int getHeight(shared_ptr<tree<Element>> head){
    if(head== nullptr)
        return 0;
    return head.get()->height;
}

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
            return findMyDaddy(head->left.get(), id);
        } else {
            if (head->right == nullptr) {
                return head;
            }
            return findMyDaddy(head->right.get(), id);
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
    } else if (head->id > id && head->right != nullptr) {
        return findById(head->right.get(), id);
    } else if (head->id < id && head->left != nullptr) {
        return findById(head->left.get(), id);
    }
    return nullptr;
}

/*******************************************   rotate functions   ***********************************************************/
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
tree<Element>* addElementRecursively(tree<Element>* head,tree<Element>* element_tree,int iterator, StatusType* status)
{
    if(head== nullptr){
        return element_tree;
    }

    if (head->id > iterator) {
        head->left = shared_ptr<tree<Element>>(addElementRecursively(head->left.get(),element_tree,iterator, status));
    }

    else if (head->id < iterator) {
        head->right = shared_ptr<tree<Element>>(addElementRecursively(head->right.get(),element_tree,iterator, status));
 }
    else { // cant be equal
        *status = FAILURE; // already exists in tree
        return element_tree;
    }

    *status = SUCCESS;
    head->height = getMax(getHeight(head->left), getHeight(head->right)) +1;
    int b = head->getBalance();

    // LL
    if (b > 1 && iterator < head->left.get()->id) {
        return right_rot<Element>(head);
    }

    // RR
    if (b < -1 && iterator < head->right.get()->id) {
        return left_rot<Element>(head);
    }

    // LR
    if (b > 1 && iterator > head->left.get()->id)
    {
        head->left = shared_ptr<tree<Element>>(left_rot<Element>(head->left.get()));
        return right_rot<Element>(head);
    }

    // RL
    if (b < -1 && iterator <  head->right.get()->id)
    {
        head->right = shared_ptr<tree<Element>>(right_rot<Element>(head->right.get()));
        return left_rot<Element>(head);
    }

    // do nothing:
    return head;
}

static int getMax(int a, int b){
    if(a>b)
        return a;
    return b;
}



#endif //DS_HW1_TREE_H
