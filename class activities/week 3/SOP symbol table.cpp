#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <variant>


enum class dataType {
    DTINT,
    DTFLOAT,
    DTCHAR
};

// For holding the value of a variable
using primValue = std::variant<int, float, char>;

struct Symbol
{
    dataType type;
    primValue value;
    int owner; // which process
    std::string scope; // main, local, etc
};



class SymbolTable {

private:
    std::unordered_map<std::string, Symbol> table;

public:
    // STORE
    void storeInt(const std::string& name, int val) {
        if (table.count(name))
            throw std::runtime_error("Variable" + name + " already exists");
        table[name] = { dataType::DTINT, val, 0, "global" };
    }

    void storeFloat(const std::string& name, float val) {
        if (table.count(name))
            throw std::runtime_error("Variable already declared: " + name);
        table[name] = { dataType::DTFLOAT, val };
    }

    void storeChar(const std::string& name, char val) {
        if (table.count(name))
            throw std::runtime_error("Variable already declared: " + name);
        table[name] = { dataType::DTCHAR, val };
    }

    // UPDATE
    void updateInt(const std::string& name, int val) {
        auto i = table.find(name);

        // if it doesnt exist
        if (i == table.end()) 
            throw std::runtime_error("Variable not found");
        // if its not int
        if (i->second.type != dataType::DTINT) //symbol.datatype != int
            throw std::runtime_error("Type mismatch");

        i->second.value = val;
    }

    void updateFloat(const std::string& name, float val) {
        auto i = table.find(name);
        if (i == table.end())
            throw std::runtime_error("Variable not found: " + name);
        if (i->second.type != dataType::DTFLOAT)
            throw std::runtime_error("Type mismatch for: " + name);
        i->second.value = val;
    }

    void updateChar(const std::string& name, char val) {
        auto i = table.find(name);
        if (i == table.end())
            throw std::runtime_error("Variable not found: " + name);
        if (i->second.type != dataType::DTCHAR)
            throw std::runtime_error("Type mismatch for: " + name);
        i->second.value = val;
    }

    // parse the input to type, name, value then uses store functions
    void parseNstore(const std::string& input) {

        //int myCSOPESYGrade = 100
        size_t firstSpace = input.find(' ');
        size_t equalSign = input.find('=');

        std::string typeStr = input.substr(0, firstSpace);
        std::string name = input.substr(firstSpace + 1, equalSign - firstSpace - 1);
        std::string valueStr = input.substr(equalSign + 1);

        if (typeStr == "int") { //int myCSOPESYGrade = 100
            int value = std::stoi(valueStr);
            storeInt(name, value);
        } else if (typeStr == "float") { // float num = 1.67
            float value = std::stof(valueStr);
            storeFloat(name, value);
        } else if (typeStr == "char") { // char letter = 'c'
            char value = valueStr[1];
            storeChar(name, value);
        } else {
            throw std::runtime_error("Unknown data type");
        }
    }

    int main() {
        std::string input = "int myCSOPESYGrade = 100";
        // Declare as temp var then use the symbol table to store "int myCSOPESYGrade = 100"
        SymbolTable st;
        st.parseNstore(input);

        // Declare as unique pointer then store again
        std::unique_ptr<SymbolTable> ptrTable = std::make_unique<SymbolTable>();
        ptrTable->parseNstore(input);
    }

};