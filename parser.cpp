//
//  parser.cpp
//  lab3
//
//  Modified by Tarek Abdelrahman on 2020-10-04.
//  Created by Tarek Abdelrahman on 2018-08-25.
//  Copyright © 2018-2020 Tarek Abdelrahman.
//
//  Permission is hereby granted to use this code in ECE244 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.


#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#include "globals.h"
#include "Shape.h"
#include <algorithm>
#include <iostream>

#define ERR_INVALIDCMD      "invalid command"
#define ERR_INVALIDARG      "invalid argument"
#define ERR_INVALIDSHPNM    "invalid shape name"
#define ERR_SHPEXISTS       "shape name exists"
#define ERR_SHPNOTFND       "shape name not found"
#define ERR_INVALIDSHPTP    "invalid shape type"
#define ERR_INVALIDVAL      "invalid value"
#define ERR_TOOMANYARGS     "too many arguments"
#define ERR_TOOFEWARGS      "too few arguments"
#define ERR_ARYFULL         "shape array full"
#define ERR_OK              "OK"

#define err(errmsg)    {cout << "Error: " << errmsg << endl;}

bool arrayContains(string*, int, string);
bool isReserved(string);
bool checkStreamError(stringstream&, bool);
string addToDatabase(Shape*);
string removeFromDatabase(string name);
int indexOfShape(string name);
string buildErrorMessageWithName(string, string);


// This is the shape array, to be dynamically allocated
Shape** shapesArray;

// The number of shapes in the database, to be incremented 
int shapeCount = 0;

// The value of the argument to the maxShapes command
int max_shapes;

// ECE244 Student: you may want to add the prototype of
// helper functions you write here

int main() {

    string line;
    string command;
    
    cout << "> ";         // Prompt for input
    getline(cin, line);    // Get a line from standard input

    while ( !cin.eof () ) {
        // Put the line in a linestream for parsing
        // Making a new sstream for each line so flags etc. are cleared
        stringstream lineStream (line);
        
        // Read from string stream into the command
        // The only way this can fail is if the eof is encountered
        lineStream >> command;

        // Check for the command and act accordingly
        // ECE244 Student: Insert your code here
        if (lineStream.eof()) {
            err(ERR_INVALIDCMD);
            lineStream.clear();
            lineStream.ignore();
            goto nextloop;;
        }
        if (command.compare("maxShapes")==0) {
            lineStream >> max_shapes;
            if (checkStreamError(lineStream, true)) {
                goto nextloop;;
            }
            
            for (int i=0;i<shapeCount;i++) {
                delete shapesArray[i];
            }
            delete shapesArray;
            shapesArray = new Shape*[max_shapes];
            
            for (int i=0;i<max_shapes;i++) {
                shapesArray[i] = NULL;
            }
            
            cout << "New database: max shapes is " << max_shapes << endl;
            
        } else if (command.compare("create")==0) {
            string name;
            lineStream >> name;
            if (checkStreamError(lineStream,false)) {
                goto nextloop;;
            }
            
            if (isReserved(name)) {
                err(ERR_INVALIDSHPNM)
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            string type;
            lineStream >> type;
            if (checkStreamError(lineStream, false)) {
                goto nextloop;;
            }
            
            if (!arrayContains(shapeTypesList, 4, type)) {
                err(ERR_INVALIDSHPTP)
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            int x;
            lineStream >> x;
            if (checkStreamError(lineStream,false)) {
                goto nextloop;;
            }
            if (x<0) {
                err(ERR_INVALIDVAL);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            int y;
            lineStream >> y;
            if (checkStreamError(lineStream, false)) {
                goto nextloop;;
            }
            if (y<0) {
                err(ERR_INVALIDVAL);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            int szx;
            lineStream >> szx;
            if (checkStreamError(lineStream, false)) {
                goto nextloop;;
            }
            if (szx<0) {
                err(ERR_INVALIDVAL);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }

            int szy;
            lineStream >> szy;
            if (checkStreamError(lineStream, true)) {
                goto nextloop;;
            }
            if (szy<0) {
                err(ERR_INVALIDVAL);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            if (type.compare("circle")==0 && szx != szy) {
                err(ERR_INVALIDVAL);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            Shape* shape = new Shape(name, type, x,szx,y,szy);
            string errMsg;
            errMsg = addToDatabase(shape);
            if (errMsg!= ERR_OK) {
                err(errMsg);
                goto nextloop;;
            }
            cout << "Created ";
            shape->draw();
            cout << endl;
        } else if (command.compare("move")==0) {
            string name;
            lineStream >> name;
            if (checkStreamError(lineStream, false)) {
                goto nextloop;;
            }
            
            if (isReserved(name)) {
                err(ERR_INVALIDSHPNM)
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            int idx = indexOfShape(name);
            if (idx<0) {
                err(buildErrorMessageWithName(ERR_SHPNOTFND,name));
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            int x;
            lineStream >> x;
            if (checkStreamError(lineStream, false)) {
                goto nextloop;;
            }
            if (x<0) {
                err(ERR_INVALIDARG);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            int y;
            lineStream >> y;
            if (checkStreamError(lineStream, true)) {
                goto nextloop;;
            }
            if (y<0) {
                err(ERR_INVALIDARG);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            shapesArray[idx]->setXlocation(x);
            shapesArray[idx]->setYlocation(y);
            
            cout << "Moved " << name << " to " << x << " " << y << endl;
            
        } else if (command.compare("rotate")==0) {
            string name;
            lineStream >> name;
            if (checkStreamError(lineStream, false)) {
                goto nextloop;;
            }
            
            if (isReserved(name)) {
                err(ERR_INVALIDSHPNM)
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            int idx = indexOfShape(name);
            if (idx<0) {
                err(ERR_SHPNOTFND);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            
            int x;
            lineStream>>x;
            if (checkStreamError(lineStream, true)) {
                goto nextloop;;
            }
            
            if (x<0 || x>=360) {
                err(ERR_INVALIDVAL);
                lineStream.clear();
                lineStream.ignore();
                goto nextloop;;
            }
            shapesArray[idx]->setRotate(x);
            //Rotated my_rectangle by 100 degrees
            cout << "Rotated " << name << " by " << x << " degrees" << endl;       
            
        } else if (command.compare("draw")==0) {
            string name;
            lineStream >> name;
            if (checkStreamError(lineStream, true)) {
                goto nextloop;;
            }
            
            if (name.compare("all")==0) {
                cout << "Drew all shapes" << endl;
                for (int i=0; i<shapeCount; i++) {
                    shapesArray[i]->draw();
                    cout << endl;
                }
            } else {
                if (isReserved(name)) {
                    err(ERR_INVALIDSHPNM)
                    lineStream.clear();
                    lineStream.ignore();
                    goto nextloop;;
                }
                
                int idx = indexOfShape(name);
                if (idx<0) {
                    err(buildErrorMessageWithName(ERR_SHPNOTFND,name));
                    lineStream.clear();
                    lineStream.ignore();
                    goto nextloop;;
                }
                
                cout << "Drew ";
                shapesArray[idx]->draw();
                cout << endl;
                
            }
        } else if (command.compare("delete")==0) {
            string name;
            lineStream >> name;
            if (checkStreamError(lineStream, true)) {
                goto nextloop;;
            }
            
            if (name.compare("all")==0) {
                cout << "Deleted: all shapes" << endl;
                for (int i=shapeCount-1; i>=0; i--) {
                    shapesArray[i] = NULL;
                }
                shapeCount = 0;
            } else {
                if (isReserved(name)) {
                    err(ERR_INVALIDSHPNM)
                    lineStream.clear();
                    lineStream.ignore();
                    goto nextloop;;
                }

                string errMsg;
                errMsg = removeFromDatabase(name);
                if (errMsg.compare(ERR_OK)==0) {
                    cout << "Deleted shape " << name << endl;
                } else {
                    err(errMsg);
                }
            }
        } else {
            err(ERR_INVALIDCMD);
            lineStream.clear();
            lineStream.ignore();

        }
        
        
nextloop:        
        // Once the command has been processed, prompt for the
        // next command
        cout << "> ";          // Prompt for input
        getline(cin, line);
        
    }  // End input loop until EOF.
    
    return 0;
}

bool isReserved(string s) {
    return arrayContains(keyWordsList, 7, s) || arrayContains(shapeTypesList, 4, s) ;
}

bool arrayContains(string* arr, int len, string s) {
    for (int i=0;i<len;i++) {
        if (arr[i]==s)
            return true;
    }
    return false;
}

bool checkStreamError(stringstream& st, bool ignoreEof) {
    if (ignoreEof) {
        if (!st.eof()){
            string s;
            st >> s;
            if (!st.eof() && !st.fail()) {
                err(ERR_TOOMANYARGS);
                return true;
            }
        }
    }
    else if (st.eof()) {
        err(ERR_TOOFEWARGS);
        return true;
    } 
    if (st.fail()) {
        err(ERR_INVALIDARG);
        return true;
    }
    return false;
}

string addToDatabase(Shape* shape) {
    if (shapeCount >= max_shapes) {
        return ERR_ARYFULL;
    }
    
    int idx = indexOfShape(shape->getName());
    if (idx>=0) {
        return buildErrorMessageWithName(ERR_SHPEXISTS, shape->getName());
    }
    
    shapesArray[shapeCount] = shape;
    shapeCount++;
    return ERR_OK;
}

int indexOfShape(string name) {
    for (int i=0;i<shapeCount;i++) {
        if (shapesArray[i]->getName() == name) {
            return i;
        }
    }
    
    return -1;
}

string removeFromDatabase(string name) {
    int idx = indexOfShape(name);
    if (idx<0) {
        return buildErrorMessageWithName(ERR_SHPNOTFND,name);
    }
    for (int i=idx; i<shapeCount-1; i++) {
        shapesArray[i] = shapesArray[i+1];
    }
    shapeCount--;
    return ERR_OK;
}

string buildErrorMessageWithName(string err, string name) {
    stringstream tmpst;
    if (err.compare(ERR_SHPEXISTS)==0) {
        tmpst << "shape " << name << " exists";
        return tmpst.str();
    }
    if (err.compare(ERR_SHPNOTFND)==0) {
        tmpst << "shape " << name << " not found";
        return tmpst.str();
    }
    return NULL;
    
}