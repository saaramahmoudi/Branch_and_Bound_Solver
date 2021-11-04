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

using namespace std;

struct Clause{
    int* vars;
    int  vars_len;
    double weight;
};


struct Node{
    int node_id;  
    Node* right; //True
    Node* left;  //False
    bool pruned;

    Node(int id){
        node_id = id;
        right = NULL;
        left = NULL;
        pruned = false;
    } 
};

int num_of_variables = 0;
int num_of_clauses = 0;
int best_sol_zero_clause = 0;
Clause* c;
Node root = Node(1);


void insert_node (Node root_node, int id , int side){ //side = 0 -> left, side = 1 -> right
    if(side == 0){
        root_node.left = new Node(id);
    }
    else{
        root_node.right = new Node(id);
    }
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
    best_sol_zero_clause = zero_clauses_num;
}

void init(){
    c = new Clause[num_of_clauses];
    for(int i = 0; i < num_of_clauses; i++){
        c[i].vars = new int[max_num_var_in_clause];
        c[i].vars_len = 0;
        c[i].weight = 0;
    }
    // root = Node(1); //create the first node of the binary tree
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
        int input;
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

    
    return 0;
}