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


unsigned long long int CHECK_BIT(unsigned long long int var,unsigned long long int pos){
    unsigned long long int mover = 1;
    return ((var) & (mover<<(pos)));
}

using namespace std;

struct Clause{
    int* vars;
    int  vars_len;
    double weight;
};


struct Node{
    int depth; 
    unsigned long long int path;
    Node* right; //True
    Node* left;  //False
    bool pruned;
    vector<int> remained_claused;
    int number_of_zero; 
    Node* parent;

    Node(int dep, unsigned long long int p, Node* par){
        depth = dep;
        right = NULL;
        left = NULL;
        pruned = false;
        path = p;
        parent = par;
        number_of_zero = 0;
    } 
};

int num_of_variables = 0;
int num_of_clauses = 0;
int best_sol = 0;
int num_of_traversed_node = 1;
int* repeat_count;
int* not_repeat_count;
vector<pair<int,int>> literals;
Node* best_leaf;
Clause* c;
Node* root;


//print binary tree
void print_binary_tree(Node* root, string previous_log){
    if (root != NULL){
        cout << previous_log << root->depth << " " <<  root->path << " " << literals[root->depth-1].second <<endl;
        print_binary_tree(root->left, previous_log+"\t");
        print_binary_tree(root->right,previous_log+"\t");
    }
}

bool variable_visited(int var, int size){
    for(int i = 0; i < size; i++){
        if(literals[i].second == var)
            return true;
    }
    return false;

}

bool pure_literal(vector<int> remained_claused, int var){
    for(int i = 0; i < remained_claused.size(); i++){
        int c_index = remained_claused[i];
        for(int j = 0; j < c[c_index].vars_len; j++){
            if(c[c_index].vars[j] == var)
                return false;
        }
    }
    return true;
}

bool branch_and_bound(Node* node){
    int clause_status = -1;
    int num_of_zero = node->parent->number_of_zero;
    // cout << "============================" << endl;
    // cout << node->depth << " " << node->path << endl;\
    // cout << node->parent->depth << " " << node->parent->path << endl;
    bool should_skip = false;

    if(CHECK_BIT(node->path,literals[node->parent->depth-1].second-1)){
        should_skip = pure_literal(node->parent->remained_claused,literals[node->parent->depth-1].second);
    }
    else{
        should_skip = pure_literal(node->parent->remained_claused,-1*literals[node->parent->depth-1].second);
    }
    if(should_skip){
        node->remained_claused = node->parent->remained_claused;
        node->number_of_zero = node->parent->number_of_zero;
        // cout << "=======================" << endl;
        // cout << "skipped " << node->depth << " " << node->path << endl;
        // cout << CHECK_BIT(node->path,literals[node->parent->depth-1].second-1) << endl;
        // cout << literals[node->parent->depth-1].second << endl;
        // for(int i = 0; i < node->remained_claused.size(); i++){
        //     for(int j = 0; j < c[node->remained_claused[i]].vars_len; j++)
        //         cout << c[node->remained_claused[i]].vars[j] << " ";
        //     cout << endl;
        // }
        return true;
    }

    for(int k = 0; k < node->parent->remained_claused.size(); k++){
        int i = node->parent->remained_claused[k];
        clause_status = -1;
        for(int j =0 ; j < c[i].vars_len; j++){      
            int var_index = abs(c[i].vars[j]) -1 ; 
            if(!variable_visited(abs(c[i].vars[j]),node->depth-1)){//still not visited
                clause_status = 2;
                continue;
            }
            if(c[i].vars[j] > 0 && CHECK_BIT(node->path,var_index)){
                clause_status = 1;
                break;
            }
            if(c[i].vars[j] < 0 && !CHECK_BIT(node->path,var_index)){
                clause_status = 1;
                break;
            }
        }

        if(clause_status == -1){
            clause_status = 0;
        }
        if(clause_status == 2){
            node->remained_claused.push_back(i);
        }
        if(clause_status == 0){
            num_of_zero++;
        }

        // if(clause_status == 0){
        //     cout << "Clause " << i+2 << " is false" << endl;
        // }
        // else if(clause_status == 1){
        //     cout << "Clause " << i+2 << " is true" << endl;
        // }
        // else if(clause_status == 2){
        //     cout << "Clause " << i+2 << " is DONT know" << endl;
        // }

    }
    // cout << num_of_zero << endl;
    node->number_of_zero = num_of_zero;
    if(num_of_zero >= best_sol){//should be pruned
        // cout << "************************************" << endl;
        // cout << "pruned" << endl;
        // cout << node->depth << " " << node->path << endl;
        // cout << num_of_zero << endl;
        // cout << "************************************" << endl;
        node->pruned = true;
        return true;
    }
    if(num_of_zero <= best_sol && node->depth > num_of_variables){//only leaf node should change the best solution
        best_sol = num_of_zero;
        best_leaf = new Node(node->depth,node->path,NULL);
    }
    return false;
}

void build_tree(Node* root){
    if(root->depth == num_of_variables+1)
        return;
    
    unsigned long long int mover = 1;
    unsigned long long int path = root->path | mover << (literals[root->depth-1].second-1);
    
    root->right = new Node(root->depth+1,path,root);
    //check whether we should build right subtree
    root->right->pruned = branch_and_bound(root->right);
    num_of_traversed_node++;
    root->left = new Node(root->depth+1,root->path,root);
    //check whether we should build left subtree
    root->left->pruned = branch_and_bound(root->left);
    num_of_traversed_node++;
    
    if(repeat_count[literals[root->depth-1].second-1] > not_repeat_count[literals[root->depth-1].second-1]){
        if(!root->right->pruned){ //if not pruned continue building
            build_tree(root->right);
        }
        if(!root->left->pruned){//if not pruned continue building
            build_tree(root->left);
        }
    }
    else{
        if(!root->left->pruned){
            build_tree(root->left);
        }
        if(!root->right->pruned){
            build_tree(root->right);
        }
    }
    return;
}


void best_sol_init(){
    unsigned long long int best_init = 0; //initilazed all to false
    bool* locked = new bool[num_of_variables];
    for(int i = 0; i < num_of_variables; i++)
        locked[i] = false;
    int zero_clauses_num = 0;
    for(int i = 0; i < num_of_clauses; i++){
        bool is_zero = true;
        for(int j = 0; j < c[i].vars_len; j++){
            int var = abs(c[i].vars[j]);
            if(locked[var-1]){//if variable has been selected already
                if(c[i].vars[j] > 0 && CHECK_BIT(best_init,var-1)){
                    is_zero = false;
                    break;
                }
                if(c[i].vars[j] < 0 && !CHECK_BIT(best_init,var-1)){
                    is_zero = false;
                    break;
                }
            }
        }
        if(is_zero){
            for(int j = 0; j < c[i].vars_len; j++){
                int var = abs(c[i].vars[j]);
                if(locked[var-1])
                    continue;
                
                if(c[i].vars[j] > 0){
                    unsigned long long int mover = 1;       
                    best_init |= mover << (var-1);
                    locked[var-1] = true;
                    is_zero = false;
                    break;
                }
                else{
                    locked[var-1] = true;
                    is_zero = false;
                    break;
                }
            }
        }
        if(is_zero){
            zero_clauses_num++;
        }
    }
    best_sol = zero_clauses_num;
    best_leaf = new Node(num_of_variables+1,best_init,NULL);
    delete[] locked; 
}

void init(){
    c = new Clause[num_of_clauses];
    for(int i = 0; i < num_of_clauses; i++){
        c[i].vars = new int[max_num_var_in_clause];
        c[i].vars_len = 0;
        c[i].weight = 0;
    }
    repeat_count = new int[num_of_variables];
    not_repeat_count = new int [num_of_variables];
    for(int i = 0; i < num_of_variables; i++){
        repeat_count[i] = 0;
        not_repeat_count[i] = 0;
    }
    root = new Node(1,0,NULL);
    for(int i = 0; i < num_of_clauses; i++){
        root->remained_claused.push_back(i);
    }
    
}

void preprocess(){
    for(int i = 0; i < num_of_variables; i++){
        pair <int,int> temp;
        temp.first = repeat_count[i] + not_repeat_count[i];
        temp.second = i+1;
        literals.push_back(temp);
    }
    sort(literals.rbegin(),literals.rend());
    // for(int i = 0; i < num_of_variables; i++){
    //     cout << literals[i].second << " " << literals[i].first << endl;
    // }
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
            if(input > 0){
                repeat_count[(int) (input-1)]++;
            }
            else{
                not_repeat_count[(int)(abs(input)-1)]++;
            }
            c[read_line].vars_len++;
        }
        else{
            testFile >> input;
            c[read_line].weight = input;
            read_line++;
        }
    }

    preprocess();
    
    best_sol_init();
    cout << best_sol << endl;
    build_tree(root);
    // print_binary_tree(root ,"");
    cout << endl;
    cout << num_of_traversed_node << endl;
    cout << best_sol << endl;
    // cout << best_leaf->path << endl;
  
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