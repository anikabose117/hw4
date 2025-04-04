#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// checks if path are the same height/depth
bool getPathHeight(Node * root, int& univHeight, int leafHeight);


bool equalPaths(Node * root)
{
    // Add your code below
    // initialize startHeight as -1 because no paths have been checked
    // startHeight represents the universal common path height (i.e. if it's 2 for all paths, that means startHeight will equal 2)
    int startHeight = -1;

    // pass argument of 0 because starting at beginning where no paths have been looked at
    return getPathHeight(root, startHeight, 0);
}

bool getPathHeight(Node * root, int& univHeight, int leafHeight){
    // base case: node is null, return true (empty path)
    if (root == NULL){
        return true;
    }

    // node is a leaf node (no children)
    if (root -> left == NULL && root -> right == NULL){

        // if univHeight hasn't been set (still -1), set it to the current leaf's height
        if (univHeight == -1){
            univHeight = leafHeight;
        }
        // cout << root -> key << leafHeight << endl;
        // return true if the current leaf's height matches the stored universal height
        return univHeight == leafHeight;
    }

    // recursively check right and left subtrees
    return getPathHeight(root -> left, univHeight, leafHeight + 1) && getPathHeight(root -> right, univHeight, leafHeight + 1);
}
