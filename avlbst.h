#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n1);
    void rotateLeft(AVLNode<Key,Value>* n1);
    void removeFix(AVLNode<Key,Value>* n, int diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this -> root_);

    // beginning node should be root, so its parent should be null
    AVLNode<Key, Value>* tempParent = NULL;

    // traverse through list until insertion point is found
    while (temp != NULL){
        tempParent = temp;
        
        // key already exists, replace value
        if (new_item.first == temp -> getKey()){
            temp -> setValue(new_item.second); 
            return;                  
        }
        else if (new_item.first < temp -> getKey()){
            temp = temp -> getLeft();
        } 
        else {
            temp = temp -> getRight();     
        }
    }

    // create new node with key/value from the argument
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, tempParent);
    newNode -> setBalance(0);

    // tree is empty, set new node as root
    if (tempParent == NULL){
        this -> root_ = newNode;
        return;

    // add node as child of parent (check which side) 
    } else if (new_item.first < tempParent -> getKey()){
        tempParent -> setLeft(newNode);
        // newNode -> setParent(tempParent);
    } else {
        tempParent -> setRight(newNode);
        // newNode -> setParent(tempParent);

    }

    // updating balances based off of new node
    if (newNode -> getParent() != NULL){
        if ((newNode -> getParent()) -> getBalance() == -1){
            (newNode -> getParent()) -> setBalance(0);
        } else if ((newNode -> getParent()) -> getBalance() == 1){
            (newNode -> getParent()) -> setBalance(0);
        } else if ((newNode -> getParent()) -> getBalance() == 0){
            if (newNode == (newNode -> getParent()) -> getLeft()){
                newNode -> getParent() -> updateBalance(-1);
            } else {
                newNode -> getParent() -> updateBalance(1);
            }
            // calling insert to rebalance the tree
            insertFix(newNode -> getParent(), newNode);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
    // TODO
    if (p == NULL || p -> getParent() == NULL){
        return;
    }

    // g = grandparent
    AVLNode<Key, Value>* g = p -> getParent();
    
    // follows pseudocode on slides for insert fix
    // parent is left child of grandparent
    if (p == g -> getLeft()){
        g -> updateBalance(-1);  
        
        if (g -> getBalance() == 0){
            // no change needed
            return;
        } else if (g -> getBalance() == -1){
            insertFix(g, p);
        } else if (g -> getBalance() == -2){
            // zigzigleft
            if (n == p -> getLeft() /*&& p == g -> getLeft()*/){
                rotateRight(g);
                p -> setBalance(0);
                g -> setBalance(0);
            // zigzagleft
            } else /*if (n -> getBalance() == 1)*/{
                rotateLeft(p);
                rotateRight(g);
                // adjusts balances as needed after rotations
                if (n -> getBalance() == -1){
                    p -> setBalance(0);
                    g -> setBalance(1);
                    n -> setBalance(0);
                } else if (n -> getBalance() == 0){
                    p -> setBalance(0);
                    g -> setBalance(0);
                    n -> setBalance(0);
                } else if (n -> getBalance() == 1){
                    p -> setBalance(-1);
                    g -> setBalance(0);
                    n -> setBalance(0);
                }
            }
        } 
    // parent is right child of grandparent
    } else if (p == g -> getRight()){
        g -> updateBalance(1);
        // nothing to balance, so return
        if (g -> getBalance() == 0){
            return;
        } else if (g -> getBalance() == 1){
            insertFix(g, p);
        } else if (g -> getBalance() == 2){
            // zig zig right
            if (n == p -> getRight() /*&& p == g -> getRight()*/){
                rotateLeft(g);
                p -> setBalance(0);
                g -> setBalance(0);
            // zig zag right
            } else /*if (n -> getBalance() == -1)*/{
                rotateRight(p);
                rotateLeft(g);
                // adjust balances as needed after rotations
                if (n -> getBalance() == 1){
                    p -> setBalance(0);
                    g -> setBalance(-1);
                    n -> setBalance(0);
                } else if (n -> getBalance() == 0){
                    p -> setBalance(0);
                    g -> setBalance(0);
                    n -> setBalance(0);
                } else if (n -> getBalance() == -1){
                    p -> setBalance(1);
                    g -> setBalance(0);
                    n -> setBalance(0);
                }
            }
        } 
    }
}

// perform right rotation on given node
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n1){
    
    // new parent of node n1 will be the current left child of n1 
    AVLNode<Key, Value>* p = n1 -> getLeft();

    if (p != NULL){
        // move parent right child to n1's left child
        n1 -> setLeft(p -> getRight());
        
        // update parent pointers as necessary
        if (p -> getRight() != NULL){
            p -> getRight() -> setParent(n1);
        }

        p -> setParent(n1 -> getParent());

        if (n1 -> getParent() == NULL){
            // update root
            this -> root_ = p;
        // update n1's parent's child pointers
        } else if (n1 == (n1 -> getParent()) -> getLeft()) {
            (n1 -> getParent()) -> setLeft(p);
        } else {
            (n1 -> getParent()) -> setRight(p);
        }
        
        n1 -> setParent(p);
        p -> setRight(n1);

    } else {
        return;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n1){
    
    // new parent of node n1 will be the current right child of n1 
    AVLNode<Key, Value>* p = n1 -> getRight();

    if (p != NULL){
        // move parent left child to n1's right child
        n1 -> setRight(p -> getLeft());
        
        // update parent pointers as necessary
        if (p -> getLeft() != NULL){
            p -> getLeft() -> setParent(n1);
        }

        p -> setParent(n1 -> getParent());

        if (n1 -> getParent() == NULL){
            // update root
            this -> root_ = p;

        // update n1's parent's child pointers
        } else if (n1 == (n1 -> getParent()) -> getRight()) {
            (n1 -> getParent()) -> setRight(p);
        } else {
            (n1 -> getParent()) -> setLeft(p);
        }
        p -> setLeft(n1);
        n1 -> setParent(p);

    } else {
        return;
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    int diff = 0;

    AVLNode<Key, Value>* badNode = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
    if (badNode == NULL) {
        return;
    }
    // delete badNode;
    AVLNode<Key, Value>* p = badNode -> getParent();

    if (p != NULL){
        if (badNode == p -> getLeft()){
            diff = 1;
        } else {
            diff = -1;
        }
    }

    // node has two children, so must swap with its predecessor before removing
    if (badNode -> getLeft() != NULL && badNode -> getRight() != NULL){
        AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this -> predecessor(badNode));
        nodeSwap(badNode, temp);
    }
    
    p = badNode -> getParent();
    if (p != NULL){
        if (badNode == p -> getLeft()){
            // height on left decrease by 1 when I use diff in helper
            diff = 1;
        } else {
            // height on right decrease by 1 when I use diff in helper
            diff = -1;
        }
    }


    // node has no children (leaf node)
    if (badNode -> getRight() == NULL && badNode -> getLeft() == NULL){
        // root case
        if (badNode -> getParent() == NULL){
            this -> root_ = NULL;
        } else {
            // edit parent of node 
            if ((badNode -> getParent()) -> getLeft() == badNode){
                (badNode -> getParent()) -> setLeft(NULL);
            } else {
                (badNode -> getParent()) -> setRight(NULL);
            }
        }
    } else {
        // node has only one child
        AVLNode<Key, Value>* temp = NULL;
        // node has a left child
        if (badNode -> getLeft() != NULL){
            temp = badNode -> getLeft();
        } else {
            // node has a right child
            temp = badNode -> getRight();
        }
        // update child's parent pointer
        if (temp != NULL) {
            temp -> setParent(badNode -> getParent());
        }
        // update parent's child pointer
        if (badNode == this -> root_) {
            this -> root_ = temp;
        } else {
            if (badNode -> getParent() -> getLeft() == badNode) {
                (badNode -> getParent()) -> setLeft(temp);
            } else {
                (badNode -> getParent()) -> setRight(temp);
            }
        }
    }

    delete badNode; // free memory

    // balance tree
    if (p != NULL){
        removeFix(p, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff){
    
    // base case
    if (n == NULL){
        return;
    }

    // for recursive calls
    int ndiff = 0;
    AVLNode<Key, Value>* p = n -> getParent();

    // check if node is left or right child to know what adjustment to diff
    if (p != NULL){
        if (n == (n -> getParent()) -> getLeft()){
            ndiff = 1;
        } else {
            ndiff = -1;
        }
    }

    // diff gotten from remove()
    n -> setBalance(n -> getBalance() + diff);

    // psuedocode taken from slides
    // imbalance on left size
    if (diff == -1){
        // left heavy
        if (n -> getBalance() == -2){
            AVLNode<Key, Value>* c = n -> getLeft();
            if (c -> getBalance() == -1){
                rotateRight(n);
                n -> setBalance(0);
                c -> setBalance(0);
                // recurse up the tree (by calling parent)
                removeFix(p, ndiff);
            } else if (c -> getBalance() == 0){
                rotateRight(n);
                n -> setBalance(-1);
                c -> setBalance(1);
            } else if (c -> getBalance() == 1){
                AVLNode<Key, Value>* g = c -> getRight();
                rotateLeft(c);
                rotateRight(n);
                // adjust balances as needed
                if (g -> getBalance() == 1){
                    n -> setBalance(0);
                    c -> setBalance(-1);
                    g -> setBalance(0);
                } else if (g -> getBalance() == 0){
                    n -> setBalance(0);
                    c -> setBalance(0);
                    g -> setBalance(0);
                } else if (g -> getBalance() == -1){
                    n -> setBalance(1);
                    c -> setBalance(0);
                    g -> setBalance(0);
                }
                // recursive up the tree by passing parent
                removeFix(p, ndiff);
            }
        } else if (n -> getBalance() == -1){
            // node is still balanced because <= abs(1)
            n -> setBalance(-1);
        } else if (n -> getBalance() == 0){
            // recurse
            removeFix(p, ndiff);
        }

    // imbalance on the right side
    } else if (diff == 1){
        // right heavy
        if (n -> getBalance() == 2){
            AVLNode<Key, Value>* c = n -> getRight();
            if (c -> getBalance() == 1){
                rotateLeft(n);
                n -> setBalance(0);
                c -> setBalance(0);
                // recurse up the tree by passing parent
                removeFix(p, ndiff);
            } else if (c -> getBalance() == 0){
                rotateLeft(n);
                n -> setBalance(1);
                c -> setBalance(-1);
            } else if (c -> getBalance() == -1){
                AVLNode<Key, Value>* g = c -> getLeft();
                rotateRight(c);
                rotateLeft(n);
                // adjust balance_ for nodes as needed
                if (g -> getBalance() == -1){
                    n -> setBalance(0);
                    c -> setBalance(1);
                    g -> setBalance(0);
                } else if (g -> getBalance() == 0){
                    n -> setBalance(0);
                    c -> setBalance(0);
                    g -> setBalance(0);
                } else if (g -> getBalance() == 1){
                    n -> setBalance(-1);
                    c -> setBalance(0);
                    g -> setBalance(0);
                }
                // recursive call
                removeFix(p, ndiff);
            }
        } else if (n -> getBalance() == 1){
            // node is still balanced bc <= abs(1)
            n -> setBalance(1);
        } else if (n -> getBalance() == 0){
            // recurse
            removeFix(p, ndiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
