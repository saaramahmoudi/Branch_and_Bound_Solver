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

unsigned long long int CHECK_BIT(unsigned long long int var,unsigned long long int pos){
    unsigned long long int mover = 1;
    return ((var) & (mover<<(pos)));
}

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
    long long graphic_index;
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

void drawscreen (void);
void act_on_button_press (float x, float y);
void delay();
void draw_nodes();


int num_of_variables = 0;
int num_of_clauses = 0;
int best_sol = INT_MAX;
int best_shuffle_sol = 0;
int num_of_traversed_node = 1;
queue<Node*> graphical_node;
int* repeat_count;
int* not_repeat_count;
bool* comp_visitd;
bool* locked;
vector<pair<int,int>> literals;
Node* best_leaf;
Node* best_shuffle_leaf;
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

int count_var_visited(Clause clause, int curr_depth){
    int counter = 0;
    for(int i = 0; i < clause.vars_len; i++){
        if(variable_visited(abs(clause.vars[i]),curr_depth))
            counter++;
    }
    return counter;
}

bool find_index(Clause clause, int var, int curr_depth){
    for(int i = 0 ; i < clause.vars_len; i++){
        int var_val = clause.vars[i];
        if(variable_visited(abs(var_val),curr_depth))
            continue;
        if(var_val == var){
            // cout << "search function " << endl;
            // cout << var_val << " at index " << clause.vars[i] << endl;
            return true;
        }
    }
    return false;
}

int count_compliment_clause(vector<int> remained_clause, int curr_depth){
    int num_of_comp = 0;
    bool* comp_visitd = new bool [num_of_clauses];
    for(int i = 0; i < num_of_clauses; i++)
        comp_visitd[i] = false;
    for(int i = 0; i < remained_clause.size(); i++){
        for(int j = 0; j < remained_clause.size(); j++){
            if(i == j){
                continue;
            }
            if(i > j)
                continue;
            int c_index_1 = remained_clause[i];
            int c_index_2 = remained_clause[j];
            if(comp_visitd[c_index_1] || comp_visitd[c_index_2])
                continue;
            // cout << "****" << endl;
            // cout << c_index_1 << " " << c_index_2 << endl;
            // cout << "****" << endl;
            int c1 = count_var_visited(c[c_index_1],curr_depth+1);
            int c2 = count_var_visited(c[c_index_2],curr_depth+1);
            if(c[c_index_1].vars_len - c1 != c[c_index_2].vars_len - c2){//different size, can not be compliment
                continue;
            }
            if(c[c_index_1].vars_len - c1 > 1)
                continue;
            //same size might be compliment
            bool comp = true;
            // cout << "================INJA=====================" << endl;
            // cout << "depth " << curr_depth << endl;
            for(int k = 0; k < c[c_index_1].vars_len; k++){
                int var = c[c_index_1].vars[k];
                if(variable_visited(abs(var),curr_depth+1)){
                    continue;
                }
                else{
                    // cout << "search variable" << -1*var << endl;
                    comp = find_index(c[c_index_2],-1*var,curr_depth+1);
                    if(!comp){
                        break;
                    }
                }
            }
            //debug purpose
            if(comp){
                // cout << c_index_1+2 << " " << c_index_2+2 << endl;
                // for(int k = 0;  k < c[c_index_1].vars_len; k++)
                //     cout << c[c_index_1].vars[k] << " ";
                // cout << endl;
                // for(int k = 0;  k < c[c_index_2].vars_len; k++)
                //     cout << c[c_index_2].vars[k] << " ";
                // cout << endl;
                // cout << "end of compliment clauses!!" << endl;
                comp_visitd[c_index_1] = true;
                comp_visitd[c_index_2] = true;
                num_of_comp++;
                // cout << "comp!!" << endl;
            }
        }
    }
    // cout << "num_of_comp : " << num_of_comp << endl;
    return num_of_comp;
}

bool branch_and_bound(Node* node){
    int clause_status = -1;
    int num_of_zero = node->parent->number_of_zero;
    // cout << "=============================================" << endl;
    // cout << node->depth << " " << node->path << endl;
    // cout << node->parent->depth << " " << node->parent->path << endl;

    /***********************************************PURE LITERAL******************************************************/
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
        return true;
    }
    /***********************************************COUNT FALSE CLAUSE*************************************************/
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
    /***********************************************SEARCH COMPLIMENT**************************************************/
    int num_of_compliment = count_compliment_clause(node->remained_claused,node->parent->depth-1);
    

    /***********************************************PRUNE CHECK ******************************************************/
    node->number_of_zero = num_of_zero;
    if(num_of_zero + num_of_compliment >= best_sol){//should be pruned
        // cout << "************************************" << endl;
        // cout << "pruned" << endl;
        // cout << node->depth << " " << node->path << endl;
        // cout << num_of_compliment << endl;
        // cout << num_of_zero - num_of_compliment << endl;
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
    if(root->depth == num_of_variables+1){
        return;
    }
    
    unsigned long long int mover = 1;
    unsigned long long int path = root->path | mover << (literals[root->depth-1].second-1);
    
    
    root->right = new Node(root->depth+1,path,root);
    root->right->graphic_index = 2*root->graphic_index+1; 
    //check whether we should build right subtree
    root->right->pruned = branch_and_bound(root->right);
    
    num_of_traversed_node++;
    
    root->left = new Node(root->depth+1,root->path,root);
    root->left->graphic_index = 2*root->graphic_index;
    
    //check whether we should build left subtree
    root->left->pruned = branch_and_bound(root->left);
    num_of_traversed_node++;
    
    if(repeat_count[literals[root->depth-1].second-1] > not_repeat_count[literals[root->depth-1].second-1]){
        graphical_node.push(root->right);
        graphical_node.push(root->left);
        if(!root->right->pruned){ //if not pruned continue building
            build_tree(root->right);
        }
        if(!root->left->pruned){//if not pruned continue building
            build_tree(root->left);
        }
    }
    else{
        graphical_node.push(root->left);
        graphical_node.push(root->right);
        if(!root->left->pruned){
            build_tree(root->left);
        }
        if(!root->right->pruned){
            build_tree(root->right);
        }
    }
    return;
}
int find_variable_in_literal(int variable){
    for(int i = 0; i < num_of_variables; i++){
        if(literals[i].second == variable)
            return i;
    }
    return -1;
}

void sort_clause_all(int index){
    for(int i = 0; i < c[index].vars_len - 1; i++){
        for(int j = i+1; j < c[index].vars_len; j++){
            // int ind1 = c[index].vars[i] > 0 ? repeat_count[c[index].vars[i]] : not_repeat_count[-1*c[index].vars[i]];
            // int ind2 = c[index].vars[j] > 0 ? repeat_count[c[index].vars[j]] : not_repeat_count[-1*c[index].vars[j]];
            int ind1 = find_variable_in_literal(abs(c[index].vars[i]));
            int ind2 = find_variable_in_literal(abs(c[index].vars[j]));
            if(ind1 >= ind2 ){
                int temp = c[index].vars[i];
                c[index].vars[i] = c[index].vars[j];
                c[index].vars[j] = temp;
            }
        }
    }
}

void best_sol_init(){
    unsigned long long int best_init = 0; //initilazed all to false
    for(int i = 0; i < num_of_variables; i++)
        locked[i] = false;
    int zero_clauses_num = 0;
    for(int i = 0; i < num_of_clauses; i++){
        sort_clause_all(i);
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
    best_shuffle_sol = zero_clauses_num;
    best_shuffle_leaf = new Node(num_of_variables+1,best_init,NULL);
}

void clause_shuffle(){
    unsigned seed = 0;
    for(int i = 0; i < 50; i++){
        shuffle(c,c+num_of_clauses,default_random_engine(seed));
        best_sol_init();
        if(best_sol > best_shuffle_sol){
            best_sol = best_shuffle_sol;
            best_leaf = best_shuffle_leaf;
        }
    }
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
    root->graphic_index = 0;
    graphical_node.push(root);
    for(int i = 0; i < num_of_clauses; i++){
        root->remained_claused.push_back(i);
    }
    comp_visitd = new bool [num_of_clauses];
    locked = new bool[num_of_variables];
    
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
    // cout << "Preproccessed done" << endl;
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
    clause_shuffle();
    
    
    
    //draw root
    // fillarc(root->x_pos,root_pos_y,30,0,360);


    // cout << "Initial Number of false Clauses " << best_sol << endl;
    // cout << "Initial Max Statisfied clauses " << num_of_clauses - best_sol << endl;
    build_tree(root);
    // print_binary_tree(root ,"");
    // // cout << endl;
    cout << "Nodes: " << num_of_traversed_node << endl;
    // cout << "Number of false Clauses " << best_sol << endl;
    // cout << "Max Statisfied Clauses " << num_of_clauses - best_sol << endl;
  
    // for(int i = 0; i < num_of_variables; i++){
    //     if(!CHECK_BIT(best_leaf->path,i)){
    //         cout << "x" << i+1 << " " << "false" << endl;
    //     }
    //     else{
    //         cout << "x" << i+1 << " " << "true" << endl;
    //     }
    // } 
    init_graphics("Assignment 3 - Part1", WHITE);
    init_world (0.,0.,5000.,5000.);
    update_message("Fatemehsadat(Sara) Mahmoudi - Branch and Bound");
    event_loop(act_on_button_press, NULL, NULL, drawscreen); 
    draw_nodes();
    event_loop(act_on_button_press, NULL, NULL, drawscreen); 
    return 0;
}


void drawscreen (void) {
    set_draw_mode (DRAW_NORMAL);
    clearscreen(); 
    setfontsize (10);
    setlinestyle (SOLID);
    setlinewidth (2);
    setcolor (BLACK);

    
}

void act_on_button_press (float x, float y) {
    printf("User nets_clicked a button at coordinates (%f, %f)\n", x, y);
}


void delay(){
    for (int i = 0; i < 10000; i++){
        for(int j = 0; j < 5000; j++);
    }
}




void draw_nodes(){
    double v_dist = (double) 5000./ (pow(2,num_of_variables)-1);
    double root_y_pos = (double) 5000 / (num_of_variables+1);
    
    while(!graphical_node.empty()){
        
        Node* n = graphical_node.front();
        graphical_node.pop();
        

        if(!n->pruned){
            setcolor(BLUE);
        }
        else{
            setcolor(RED);
        }

        if(n->depth <= num_of_variables){
            long long x_pos = pow(2,num_of_variables+1-(n->depth));
            // cout << "===========================================" << endl;
            // cout << n->depth << " " << n->graphic_index << endl;
            // cout << x_pos << endl;
            // cout << (n->graphic_index*x_pos+x_pos/2-0.5)*v_dist << " " << endl;
            fillarc((n->graphic_index*x_pos+x_pos/2-0.5)*v_dist,root_y_pos * (n->depth-0.5),20,0,360);
            if (n->depth == 1){
                // flushinput();
                // delay();
                continue;
            }
            //draw the line to its parent
            setcolor(BLACK);
            long long par_x_pos = pow(2,num_of_variables+1-(n->parent->depth));
            double par_pos = (n->parent->graphic_index*par_x_pos+par_x_pos/2-0.5)*v_dist;
            drawline(par_pos,root_y_pos * (n->parent->depth-0.5),(n->graphic_index*x_pos+x_pos/2-0.5)*v_dist,root_y_pos * (n->depth-0.5));
        }
        else{
            //leaf nodes
            fillarc(n->graphic_index*v_dist,root_y_pos * (n->depth-0.5),20,0,360);
            setcolor(BLACK);
            long long par_x_pos = pow(2,num_of_variables+1-(n->parent->depth));
            double par_pos = (n->parent->graphic_index*par_x_pos+par_x_pos/2-0.5)*v_dist;
            drawline(par_pos,root_y_pos * (n->parent->depth-0.5),n->graphic_index*v_dist,root_y_pos * (n->depth-0.5));
            
        }
        // flushinput();
        // delay();
    }
}