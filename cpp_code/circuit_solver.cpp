#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <random>
#include <Eigen/Dense>
#include <fftw3.h>

using namespace Eigen;
using namespace std;

// 0   1   2   3
// V   W   R   W
// W   0   0   W
// W   W   W   G

// 0   1   2 
// 1   100 -1
// 0   0   0
// 0   0   0



vector<vector<string>> initialize_visual(vector<vector<string>> build_me){
    double R1 = 100; 
    double V1 = 1; 

    build_me[0][0] = "V1";
    build_me[0][1] = "W1";
    build_me[0][2] = "R1";
    build_me[0][3] = "W2";
    build_me[1][3] = "W2";
    build_me[2][3] = "W2";
    build_me[2][3] = "G0";
    build_me[2][2] = "W3";
    build_me[2][1] = "W3";
    build_me[2][0] = "W3";
    build_me[1][0] = "W3";
    
    return(build_me);
}

vector<vector<double>> initialize_voltage(vector<vector<string>> visual_reference, vector<vector<double>> build_me){
    double V1 = 1; 

    for(int i = 0; i < visual_reference.size(); i++){
        for(int j = 0; j < visual_reference[0].size(); j++){
            if(visual_reference[i][j] == "V1"){
                build_me[i][j] = V1; 
            }
        }
        cout << endl;
    }
    return(build_me);
}

vector<vector<double>> initialize_resistance(vector<vector<string>> visual_reference, vector<vector<double>> build_me){
    double R1 = 100; 

    for(int i = 0; i < visual_reference.size(); i++){
        for(int j = 0; j < visual_reference[0].size(); j++){
            if(visual_reference[i][j] == "R1"){
                build_me[i][j] = R1; 
            }
        }
        cout << endl;
    }
    return(build_me);
}


vector<vector<string>> resolve_grounds(vector<vector<string>> resolve_me){
    int total_points = resolve_me.size()*resolve_me[0].size();

    for(int i = 0; i < resolve_me.size(); i++){
        for(int j = 0; j < resolve_me[0].size(); j++){
            cout << resolve_me[i][j] << " ";
        }
        cout << endl;
    }

    int resolved = 0; 
    int counter = 0; 
    bool grounds_resolved = false; 
    while(!grounds_resolved){
        for(int i = 0; i < resolve_me.size(); i++){
            for(int j = 0; j < resolve_me[0].size(); j++){
                if((resolve_me[i][j] == "G0")){
                    if((j + 1 < resolve_me[0].size()) && (resolve_me[i][j + 1].at(0) == 'W')){
                        resolve_me[i][j + 1] = "G0";
                    }
                    else if((j - 1 >= 0) && (resolve_me[i][j - 1].at(0) == 'W')){
                        resolve_me[i][j - 1] = "G0";
                    }
                    else if((i + 1 < resolve_me.size()) && (resolve_me[i + 1][j].at(0) == 'W')){
                        resolve_me[i + 1][j] = "G0";
                    }
                    else if((i - 1 >= 0) && (resolve_me[i - 1][j].at(0) == 'W')){
                        resolve_me[i - 1][j] = "G0";
                    }
                    else{
                        resolved++;
                    }
                    if(resolved == total_points){
                        grounds_resolved = true;
                    }
                }
            }
        }
        counter++;
        if(counter > 10000){
            grounds_resolved = true; 
            cout << "Broken by variable:counter." << endl;
            cout << "Either your circuit is too big or there is an issue in the code." << endl;
        }
    }

    cout << "Grounds resolved." << endl;
    cout << endl;

    return(resolve_me);
}

vector<string> resolve_vars(vector<vector<string>> search_me){
    vector<string> vars;

    for(int i = 0; i < search_me.size(); i++){
        for(int j = 0; j < search_me[0].size(); j++){
            if(search_me[i][j].at(0) == 'W'){
                string temp = "W"; 
                temp += search_me[i][j].at(1); 
                if(find(vars.begin(), vars.end(),temp) == vars.end()){
                    vars.push_back(temp);
                }   
            }
            else if(search_me[i][j].at(0) == 'V'){
                string temp = "I"; 
                temp += search_me[i][j].at(1);
                if(find(vars.begin(), vars.end(),temp) == vars.end()){
                    vars.push_back(temp);
                }    
            }
        }
    }
    return(vars);
}

vector<vector<string>> find_locations(vector<vector<string>> find_my_locations){
    vector<string> individual_location;
    vector<vector<string>> finding_locations;

    for(int i = 0; i < find_my_locations.size(); i++){
        for(int j = 0; j < find_my_locations[0].size(); j++){
            if(find_my_locations[i][j].at(0) == 'W'){
                string W_label = "W"; 
                W_label += find_my_locations[i][j].at(1); 
                individual_location.push_back(W_label);
                individual_location.push_back(to_string(i));
                individual_location.push_back(to_string(j));
                finding_locations.push_back(individual_location);
                individual_location.clear();
            }
            else if(find_my_locations[i][j].at(0) == 'R'){
                string R_label = "R"; 
                R_label += find_my_locations[i][j].at(1); 
                individual_location.push_back(R_label);
                individual_location.push_back(to_string(i));
                individual_location.push_back(to_string(j));
                finding_locations.push_back(individual_location);
                individual_location.clear();
            }
        }
    }

    for(int i = 0; i < finding_locations.size(); i++){
        for(int j = 0; j < finding_locations[0].size(); j++){
            cout << finding_locations[i][j] << " "; 
        }
        cout << endl;
    }
    return(finding_locations);
}

vector<vector<double>> build_G(vector<vector<string>> make_my_G, vector<string> vars, vector<vector<string>> vars_locations, vector<vector<double>> resistor_values){
    int n = 0; 
    for(int i = 0; i < vars.size(); i++){
        if(vars[i].at(0) == 'W'){
            n++; 
        }
    }
    
    vector<vector<double>> building_G (n, vector<double>(n));
    for(int i = 0; i < building_G.size(); i++){
        for(int j = 0; j < building_G[0].size(); j++){
            building_G[i][j] = 0;
        }
    }

    for(int i = 0; i < vars_locations.size(); i++){ // Checking for all positions of nodes
        if(vars_locations[i][0].at(0) == 'W'){ // Node found
            int temp_x = stoi(vars_locations[i][1]); // Stores position of nodes
            int temp_y = stoi(vars_locations[i][2]);
            for(int k = 0; k < vars_locations.size(); k++){ // Checking for all positions of resistors
                if(vars_locations[k][0].at(0) == 'R'){ // Resistor found
                    int temp2_x = stoi(vars_locations[k][1]); // Stores position of resistor
                    int temp2_y = stoi(vars_locations[k][2]);
                    if((temp_x + 1 == temp2_x) || (temp_x - 1 == temp2_x) || (temp_y + 1 == temp2_y) || (temp_y - 1 == temp2_y)){
                        // Finds if node and resistor are touching
                        int temp = int(vars_locations[k][0].at(1) - '0'); // Finds location along G diagonal
                        building_G[temp - 1][temp - 1] += pow(resistor_values[temp2_x][temp2_y], -1); // Adds conductance values to correct location in G
                    }
                }
            }
        }
    }

    return(building_G);
}

double main2(int nothing){
    int n = 3; 
    int m = 4; 
    vector<vector<string>> visual (n, vector<string>(m));
    vector<vector<double>> current (n, vector<double>(m));
    vector<vector<double>> voltage (n, vector<double>(m));
    vector<vector<double>> resistance (n, vector<double>(m));
    vector<string> solve_for;

    for(int i = 0; i < visual.size(); i++){
        for(int j = 0; j < visual[0].size(); j++){
            visual[i][j] = "00";
            voltage[i][j] = 0;
            current[i][j] = 0;
            resistance[i][j] = 0;
        }
    }

    visual = initialize_visual(visual);
    visual = resolve_grounds(visual);
    voltage = initialize_voltage(visual, voltage);
    resistance = initialize_resistance(visual, resistance);

    cout << "Visualizer: " << endl; 
    for(int i = 0; i < visual.size(); i++){
        for(int j = 0; j < visual[0].size(); j++){
            cout << visual[i][j] << " ";
        }
        cout << endl;
    } 
    cout << endl;

    cout << "Voltage Values: " << endl; 
    for(int i = 0; i < voltage.size(); i++){
        for(int j = 0; j < voltage[0].size(); j++){
            cout << voltage[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "Resistor Values: " << endl; 
    for(int i = 0; i < resistance.size(); i++){
        for(int j = 0; j < resistance[0].size(); j++){
            cout << resistance[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    vector<vector<string>> locations (n, vector<string>(m));
    locations = find_locations(visual);

    vector<vector<double>> A (solve_for.size(), vector<double>(solve_for.size()));
    
    // The layout of matrix A is as follows: 
    // A = | G   B |
    //     | C   D |

    solve_for = resolve_vars(visual);
    cout << "Solve For: " << endl; 
    for(int i = 0; i < solve_for.size(); i++){
        cout << solve_for[i] << " ";
    }
    cout << endl;

    vector<vector<double>> G;
    G = build_G(visual, solve_for, locations, resistance); 

    for(int i = 0; i < G.size(); i++){
        for(int j = 0; j < G[0].size(); j++){
            cout << G[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    return(0);
}

int main(void) {
    cout << "Begin" << endl;
    main2(0);
    cout << "End" << endl;
}
