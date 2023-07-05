#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <random>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

vector<vector<string>> initialize_visual(int x){
    const char *path1="../input_files/input_circuit.csv";
    fstream file (path1, ios::in);

	vector<string> row;
	string line, cell;
 
    vector<vector<string>> build_me;
	if(file.is_open()){
        cout << " File opened. " << endl;
		while(getline(file, line)){
			row.clear();
			stringstream strstream(line);
 
			while(getline(strstream, cell, ',')){
                row.push_back(cell);
            }
			build_me.push_back(row);
		}
	}
    else{
        cout << " File was not opened, looking for: " << path1 << endl;
    }


    cout << "This is the circuit I read in: " << endl;
    for(int i = 0; i < build_me.size(); i++){
        for(int j = 0; j < build_me[0].size(); j++){
            cout << build_me[i][j] << " "; 
        }
        cout << endl;
    }
    cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
    cout << endl;

    return(build_me);
}

vector<vector<string>> initialize_values(int x){
    const char *path1="../input_files/input_values.csv";
    fstream file (path1, ios::in);

	vector<string> row;
	string line, cell;
 
    vector<vector<string>> build_me;
	if(file.is_open()){
        cout << " File opened. " << endl;
		while(getline(file, line)){
			row.clear();
			stringstream strstream(line);
 
			while(getline(strstream, cell, ',')){
                row.push_back(cell);
            }
			build_me.push_back(row);
		}
	}
    else{
        cout << " File was not opened, looking for: " << path1 << endl;
    }
    
    cout << "Values: " << endl; 
    for(int i = 0; i < build_me.size(); i++){
        for(int j = 0; j < build_me[0].size(); j++){
            cout << build_me[i][j] << " ";
        }
        cout << endl;
    } 
    cout << endl;

    return(build_me);
}

vector<vector<double>> initialize_voltage(vector<vector<string>> visual_reference, vector<vector<string>> values, vector<vector<double>> build_me){
    for(int i = 0; i < visual_reference.size(); i++){
        for(int j = 0; j < visual_reference[0].size(); j++){
            if(visual_reference[i][j].at(0) == 'V'){
                for(int z = 0; z < values.size(); z++){
                    if(visual_reference[i][j] == values[z][0]){
                        build_me[i][j] = stoi(values[z][1]);
                    } 
                }
            }
        }
        cout << endl;
    }
    return(build_me);
}

vector<vector<double>> initialize_resistance(vector<vector<string>> visual_reference, vector<vector<string>> values, vector<vector<double>> build_me){
    for(int i = 0; i < visual_reference.size(); i++){
        for(int j = 0; j < visual_reference[0].size(); j++){
            if(visual_reference[i][j].at(0) == 'R'){
                for(int z = 0; z < values.size(); z++){
                    if(visual_reference[i][j] == values[z][0]){
                        build_me[i][j] = stoi(values[z][1]);
                    } 
                }
            }
        }
    }
    return(build_me);
}

vector<vector<string>> resolve_grounds(vector<vector<string>> resolve_me){
    int total_points = resolve_me.size()*resolve_me[0].size();

    int resolved = 0; 
    int counter = 0; 
    bool grounds_resolved = false; 
    while(!grounds_resolved){
        for(int i = 0; i < resolve_me.size(); i++){
            resolved = 0;
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

    return(resolve_me);
}

vector<string> resolve_vars(vector<vector<string>> search_me){
    vector<string> vars;

    for(int i = 0; i < search_me.size(); i++){
        for(int j = 0; j < search_me[0].size(); j++){
            if(search_me[i][j].at(0) == 'V'){
                string temp = "I"; 
                temp += search_me[i][j].at(1);
                if(find(vars.begin(), vars.end(),temp) == vars.end()){
                    vars.push_back(temp);
                }    
            }
            else if(search_me[i][j].at(0) == 'W'){
                string temp = "W"; 
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
            else if(find_my_locations[i][j].at(0) == 'V'){
                string V_label = "V"; 
                V_label += find_my_locations[i][j].at(1); 
                individual_location.push_back(V_label);
                individual_location.push_back(to_string(i));
                individual_location.push_back(to_string(j));
                finding_locations.push_back(individual_location);
                individual_location.clear();
            }
        }
    }

    cout << "\n Locations: " << endl;
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

    // Adding comments becuase this nesting is particularly ugly
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
                        int temp = int(vars_locations[i][0].at(1) - '0'); // Finds location along G diagonal
                        building_G[temp - 1][temp - 1] += pow(resistor_values[temp2_x][temp2_y], -1); // Adds conductance values to correct location in G
                        for(int t = 0; t < vars_locations.size(); t++){ // Checking for nodes bordering resistor on a different side
                            if((vars_locations[t][0].at(0)) == 'W' && (vars_locations[t][0] != vars_locations[i][0])){ // Node found 
                                int temp3_x = stoi(vars_locations[t][1]); // Stores position of nodes
                                int temp3_y = stoi(vars_locations[t][2]);
                                if((temp2_x + 1 == temp3_x) || (temp2_x - 1 == temp3_x) || (temp2_y + 1 == temp3_y) || (temp2_y - 1 == temp3_y)){
                                    int temp2 = int(vars_locations[i][0].at(1) - '0'); // Coordinates in G
                                    int temp3 = int(vars_locations[t][0].at(1) - '0'); 
                                    building_G[temp2 - 1][temp3 - 1] = -1*pow(resistor_values[temp2_x][temp2_y], -1); // Adds to G
                                    building_G[temp3 - 1][temp2 - 1] = -1*pow(resistor_values[temp2_x][temp2_y], -1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return(building_G);
}

vector<vector<double>> build_V(vector<vector<string>> make_my_V, vector<vector<string>> vars_locations){
    int n = 0; 
    int m = 0; 
    for(int i = 0; i < make_my_V.size(); i++){
        for(int j = 0; j < make_my_V[0].size(); j++){
            if(make_my_V[i][j].at(0) == 'W'){
               n++; 
            }
            if(make_my_V[i][j].at(0) == 'V'){
               m++; 
            }
        }
    } 

    vector<vector<double>> building_V (n, vector<double>(m));
    for(int i = 0; i < building_V.size(); i++){
        for(int j = 0; j < building_V[0].size(); j++){
            building_V[i][j] = 0;
        }
    }
    
    for(int i = 0; i < vars_locations.size(); i++){ // Checking for all positions of nodes
        if(vars_locations[i][0].at(0) == 'V'){ // Node found
            int temp_x = stoi(vars_locations[i][1]); // Stores position of nodes
            int temp_y = stoi(vars_locations[i][2]);
            for(int k = 0; k < vars_locations.size(); k++){ // Checking for all positions of resistors
                if(vars_locations[k][0].at(0) == 'W'){ // Resistor found
                    int temp2_x = stoi(vars_locations[k][1]); // Stores position of resistor
                    int temp2_y = stoi(vars_locations[k][2]);
                    if((temp_x + 1 == temp2_x) || (temp_x - 1 == temp2_x) || (temp_y + 1 == temp2_y) || (temp_y - 1 == temp2_y)){
                        int temp1 = int(vars_locations[i][0].at(1) - '0');
                        int temp2 = int(vars_locations[k][0].at(1) - '0'); // Finds location along G diagonal
                        building_V[temp1 - 1][temp2 - 1] = 1; // Adds conductance values to correct location in G
                    }
                }
            }
        }
    }

    return(building_V);
}

vector<vector<double>> build_VT(vector<vector<double>> transpose_me){
    int n = transpose_me.size(); 
    int m = transpose_me[0].size(); 
    
    vector<vector<double>> building_VT (m, vector<double>(n));
    for(int i = 0; i < building_VT.size(); i++){
        for(int j = 0; j < building_VT[0].size(); j++){
            building_VT[i][j] = transpose_me[j][i];
        }
    }

    return(building_VT);
}

vector<vector<double>> build_Z(vector<vector<double>> find_my_cols){
    int n = find_my_cols[0].size(); 
    
    vector<vector<double>> building_Z (n, vector<double>(n));
    for(int i = 0; i < building_Z.size(); i++){
        for(int j = 0; j < building_Z[0].size(); j++){
            building_Z[i][j] = 0;
        }
    }

    return(building_Z);
}

vector<double> build_knowns(vector<vector<string>> vars_locations, vector<vector<double>> voltage_values, vector<vector<double>> G_temp){
    vector<double> building_knowns;
    for(int i = 0; i < G_temp.size(); i++){
        building_knowns.push_back(0);
    }

    for(int i = 0; i < vars_locations.size(); i++){
        if(vars_locations[i][0].at(0) == 'V'){
            int temp_x = stoi(vars_locations[i][1]);
            int temp_y = stoi(vars_locations[i][2]);
            building_knowns.push_back(voltage_values[temp_x][temp_y]);
        }
    }

    return(building_knowns);
}

VectorXd vec_std_to_Eigen(vector<double> convert_me){
    int size = convert_me.size(); 
    VectorXd A(size);
    for(int i = 0; i < size; i++){
        A[i] = convert_me[i];
    }
    return(A);
}

MatrixXd mat_std_to_Eigen(vector<vector<double>> convert_me){
    int rows = convert_me.size(); 
    int cols = convert_me[0].size(); 
    MatrixXd A(rows,cols);
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            A(i,j) = convert_me[i][j];
        }
    }
    return(A);
}

vector<double> vec_Eigen_to_std(VectorXd convert_me){ 
    int size = convert_me.size();
    vector<double> A(size);
    for(int i = 0; i < size; i++){
        A[i] = convert_me[i];
    }
    return(A);
}

vector<vector<double>> mat_Eigen_to_std(MatrixXd convert_me){
    int rows = convert_me.rows(); 
    int cols = convert_me.cols(); 
    vector<double> B;
    vector<vector<double>> A;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            B.push_back(convert_me(i,j));
        }
        A.push_back(B);
        B.clear();
    }
    return(A);
}

double main2(int nothing){
    vector<vector<string>> visual;
    visual = initialize_visual(0);
    vector<vector<string>> values_key;
    values_key = initialize_values(0);

    int n = visual.size(); 
    int m = visual[0].size(); 

    vector<vector<double>> current (n, vector<double>(m));
    vector<vector<double>> voltage (n, vector<double>(m));
    vector<vector<double>> resistance (n, vector<double>(m));
    vector<string> solve_for;

    for(int i = 0; i < visual.size(); i++){
        for(int j = 0; j < visual[0].size(); j++){
            voltage[i][j] = 0;
            current[i][j] = 0;
            resistance[i][j] = 0;
        }
    }

    visual = resolve_grounds(visual);
    voltage = initialize_voltage(visual, values_key, voltage);
    resistance = initialize_resistance(visual, values_key, resistance);

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
    
    // The layout of matrix A is as follows: 
    // A = | G   B |
    //     | C   D |

    solve_for = resolve_vars(visual);
    cout << "\n Solve For: " << endl; 
    for(int i = 0; i < solve_for.size(); i++){
        cout << solve_for[i] << " ";
    }
    cout << endl;

    vector<vector<double>> A (solve_for.size(), vector<double>(solve_for.size()));

    vector<vector<double>> G;
    G = build_G(visual, solve_for, locations, resistance); 

    vector<vector<double>> V;
    V = build_V(visual, locations); 

    vector<vector<double>> VT;
    VT = build_VT(V); 

    vector<vector<double>> Z;
    Z = build_Z(V); 

    for(int i = 0; i < A.size(); i++){
        for(int j = 0; j < A[0].size(); j++){
            if((i < G.size()) && (j < G.size())){
                A[i][j] = G[i][j];
            }
            else if((i < G.size()) && (j >= G.size())){
                A[i][j] = V[i][j - G.size()];
            }
            else if((i >= G.size()) && (j < G.size())){
                A[i][j] = VT[i - G.size()][j];
            }
            else{
                A[i][j] = 0;
            }
        }
    }

    vector<double> knowns; 
    knowns = build_knowns(locations, voltage, G);

    MatrixXd eigen_A;
    eigen_A = mat_std_to_Eigen(A);

    VectorXd eigen_knowns;
    eigen_knowns = vec_std_to_Eigen(knowns);

    cout << " A = \n" << eigen_A << endl; 
    cout << endl;
    cout << " Knowns = \n" << eigen_knowns << endl; 
    cout << endl;

    VectorXd eigen_solved;
    eigen_solved = eigen_A.inverse()*eigen_knowns; 

    cout << " Solved = \n " << eigen_solved << endl;

    vector<double> solved; 
    solved = vec_Eigen_to_std(eigen_solved);

    return(0);
}

int main(void) {
    cout << "Begin" << endl;
    main2(0);
    cout << "End" << endl;
}