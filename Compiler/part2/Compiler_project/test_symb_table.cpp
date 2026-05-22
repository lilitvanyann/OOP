//#include "Symbol_Table.h"
//#include <iostream>
//
//int main() {
//    try {
//        SymbolTable st;
//
//        std::cout << "Test 1: Declare variable\n";
//        st.declare("x", "int");
//        std::cout << "✓ x declared\n";
//
//        std::cout << "Test 2: Check exists\n";
//        if (st.exists("x"))
//            std::cout << "✓ x exists\n";
//
//        std::cout << "Test 3: Enter new scope\n";
//        st.enterScope();
//
//        std::cout << "Test 4: Declare in inner scope\n";
//        st.declare("y", "int");
//
//        std::cout << "Test 5: Check current scope\n";
//        if (st.existsInCurrentScope("y"))
//            std::cout << "✓ y in current scope\n";
//
//        std::cout << "Test 6: Lookup from outer scope\n";
//        Symbol* sym = st.lookup("x");
//        if (sym)
//            std::cout << "✓ x found (stack offset: " << sym->stackOffset << ")\n";
//
//        std::cout << "Test 7: Exit scope\n";
//        st.exitScope();
//
//        std::cout << "Test 8: Verify y is gone\n";
//        if (!st.exists("y"))
//            std::cout << "✓ y no longer exists\n";
//
//        std::cout << "\nAll tests passed!\n";
//
//    }
//    catch (const std::exception& e) {
//        std::cout << "ERROR: " << e.what() << "\n";
//        return 1;
//    }
//
//    return 0;
//}
