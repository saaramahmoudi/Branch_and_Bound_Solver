#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <queue>
#include <limits.h>
#include <math.h>
#include <bits/stdc++.h>
#include "graphics.h"

#define max_num_var_in_clause 10
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

using namespace std;

struct Clause{
    int* vars;
    int  vars_len;
    double weight;
};


struct Node{
    int depth; 
    long path;
    Node* right; //True
    Node* left;  //False
    bool pruned;

    Node(int dep, long p){
        depth = dep;
        right = NULL;
        left = NULL;
        pruned = false;
        path = p;
    } 
};

int num_of_variables = 0;
int num_of_clauses = 0;
int best_sol = 0;
int num_of_traversed_node = 1;
Node* best_leaf;
Clause* c;
Node* root;


//print binary tree
void print_binary_tree(Node* root, string previous_log){
    if (root != NULL){
        cout << previous_log << root->depth << " " << root->path <<endl;
        print_binary_tree(root->left, previous_log+"\t");
        print_binary_tree(root->right,previous_log+"\t");
    }
}

bool branch_and_bound_optimization(Node* node){
    
    return false;
}

void build_tree(Node* root){
    if(root->depth == num_of_variables+1)
        return;
    
    num_of_traversed_node++;
    int path = root->path | 1UL << (root->depth-1);
    root->right = new Node(root->depth+1,path);
    //check whether we should build right subtree
    root->right->pruned = branch_and_bound_optimization(root->right);
    if(!root->right->pruned){ //if not pruned continue building
        build_tree(root->right);
    }
    root->left = new Node(root->depth+1,root->path);
    //check whether we should build left subtree
    root->left->pruned = branch_and_bound_optimization(root->left);
    if(!root->left->pruned){//if not pruned continue building
        build_tree(root->left);
    }
    return;
}

void best_sol_init(){
    //assume all variables are set to false!
    int zero_clauses_num = 0;
    for(int i = 0; i < num_of_clauses; i++){
        bool is_zero = true;
        for(int j = 0; j < c[i].vars_len; j++){
            // if ~x[i] exists, the clause is not false!
            if(c[i].vars[j] < 0){
                is_zero = false;
                break;
            }
        }
        if(is_zero){
            zero_clauses_num++;
        }
    }
    best_sol = zero_clauses_num;
    best_leaf = new Node(num_of_variables+1,0);

}

void init(){
    c = new Clause[num_of_clauses];
    for(int i = 0; i < num_of_clauses; i++){
        c[i].vars = new int[max_num_var_in_clause];
        c[i].vars_len = 0;
        c[i].weight = 0;
    }
    root = new Node(1,0);
}

int main(){
    string fileName;
    cout << "Enter the name of test file (1.cnf)" << endl;
    cin >> fileName; 
    ifstream testFile(fileName);
    
    testFile >> num_of_variables;
    testFile >> num_of_clauses;

    init();

    int read_line = 0;
    while(read_line < num_of_clauses){
        double input;
        testFile >> input;
        if(input != 0){
            c[read_line].vars[c[read_line].vars_len] = input;
            c[read_line].vars_len++;
        }
        else{
            testFile >> input;
            c[read_line].weight = input;
            read_line++;
        }
    }

    // for(int i = 0; i < num_of_clauses ; i++){
    //     cout << c[i].vars_len << endl;
    //     for(int j = 0; j < c[i].vars_len; j++){
    //         cout << c[i].vars[j] << " ";
    //     }
    //     cout << "==============================" << endl;
    // }
    
    best_sol_init();
    cout << best_sol << endl;
    build_tree(root);
    print_binary_tree(root ,"");
    // cout << endl;
    cout << num_of_traversed_node << endl;
    cout << best_sol << endl;
    cout << best_leaf->path << endl;
    for(int i = 0; i < num_of_variables; i++){
        if(!CHECK_BIT(best_leaf->path,i)){
            cout << "x" << i+1 << " " << "false" << endl;
        }
        else{
            cout << "x" << i+1 << " " << "true" << endl;
        }
    }

    return 0;
}