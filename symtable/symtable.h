//
// Created by Dalibor Detko on 17/04/2026.
//

#ifndef COMPILER_SYMTABLE_H
#define COMPILER_SYMTABLE_H
#include <string>


struct Symbol {
    std::string name;
    std::string type;
};

/**
 * Implemented as Hash table with implicit chaining.
 * Implicit chaining means, that when there is a collision,
 * instead of putting the collisions into array, you just find
 * another position for the element in the table
 */
class symtable {
    void insert(Symbol sym);
    void remove(Symbol sym);
    void print();
    bool isSymbolInTable(Symbol sym);
};



#endif //COMPILER_SYMTABLE_H
