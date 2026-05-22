#include "Assembler.h"
#include <stdexcept>

Assembler::Assembler()
{
    regMap.clear();
    regIndex = 5; // R5–R7 are temp registers
    labelCounter = 0;  
}

// register allocator (t1 → R5 etc.)

std::string Assembler::getReg(const std::string& temp)
{
    // Guard against empty strings
    if (temp.empty()) {
        return "R0";
    }

    if (regMap.find(temp) != regMap.end())
        return regMap[temp];

    std::string r = "R" + std::to_string(regIndex);
    regMap[temp] = r;

    regIndex++;
    if (regIndex > 7)
        regIndex = 5;

    return r;
}


// IR → Assembly

std::vector<AssemblyInstruction>
Assembler::generate(const std::vector<IRInstruction>& ir)
{
    std::vector<AssemblyInstruction> out;

    for (auto& i : ir)
    {
        // ================= ADD =================
        if (i.op == "ADD")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);

            out.push_back({ "ADD", rd, r1, r2, "" });
        }

        // ================= SUB =================
        else if (i.op == "SUB")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);

            out.push_back({ "SUB", rd, r1, r2, "" });
        }

        // ================= MUL =================
        else if (i.op == "MUL")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);

            out.push_back({ "MUL", rd, r1, r2, "" });
        }

        // ================= DIV =================
        else if (i.op == "DIV")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);

            out.push_back({ "DIV", rd, r1, r2, "" });
        }

        // ================= NUMBER =================
        else if (i.op == "NUMBER")
        {
            std::string rd = getReg(i.result);
            out.push_back({ "LI", rd, i.arg1, "", "" });
        }

        // ================= ASSIGN =================
        else if (i.op == "ASSIGN")
        {
            std::string r = getReg(i.arg1);
            out.push_back({ "STORE", i.result, r, "", "" });  // STORE, not MOV
        }

        // ================= PRINT =================
        else if (i.op == "PRINT")
        {
            std::string r = getReg(i.arg1);
            out.push_back({ "OUT", r, "", "", "" });
        }

        // ================= JUMP =================
        else if (i.op == "JUMP" || i.op == "JMP")
        {
            out.push_back({ "J", i.result, "", "", "" });
        }

        // ================= JUMP_IF_FALSE =================
        else if (i.op == "JUMP_IF_FALSE")
        {
            std::string r = getReg(i.arg1);
            out.push_back({ "BEQZ", r, i.result, "", "" });
        }

        // ================= FUNC_BEGIN =================
        else if (i.op == "FUNC_BEGIN")
        {
            currentFunction = i.result;
            out.push_back({ "LABEL", i.result, "", "", "" });
            out.push_back({ "PUSH", "BP", "", "", "" });
            out.push_back({ "MOV", "BP", "SP", "", "" });
        }

        // ================= FUNC_END =================
        else if (i.op == "FUNC_END")
        {
            if (i.result != "main") {
                out.push_back({ "MOV", "SP", "BP", "", "" });
                out.push_back({ "POP", "BP", "", "", "" });
                out.push_back({ "RET", "", "", "", "" });
            }
            currentFunction = "";
        }

        // ================= LABEL =================
        else if (i.op == "LABEL")
        {
            std::string labelName;
            if (!i.result.empty()) {
                labelName = i.result;
            }
            else if (!i.arg1.empty()) {
                labelName = i.arg1;
            }
            else {
                // Skip empty labels
                continue;
            }
            out.push_back({ "LABEL", labelName, "", "", "" });
        }

        else if (i.op == "LOAD")
        {
            std::string rd = getReg(i.result);
            out.push_back({ "LOAD", rd, getReg(i.arg1), "", "" });
        }

        // ================= GT =================
        else if (i.op == "GT")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);
            std::string skip = "skip_gt" + std::to_string(labelCounter++);

            out.push_back({ "LI", rd, "0", "", "" });
            out.push_back({ "CMP", r1, r2, "", "" });
            out.push_back({ "JLE", skip, "", "", "" });
            out.push_back({ "LI", rd, "1", "", "" });
            out.push_back({ "LABEL", skip, "", "", "" });
        }

        // ================= LT =================
        else if (i.op == "LT")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);
            std::string skip = "skip_lt" + std::to_string(labelCounter++);

            out.push_back({ "LI", rd, "0", "", "" });
            out.push_back({ "CMP", r1, r2, "", "" });
            out.push_back({ "JGE", skip, "", "", "" });
            out.push_back({ "LI", rd, "1", "", "" });
            out.push_back({ "LABEL", skip, "", "", "" });
        }

        // ================= EQ =================
        else if (i.op == "EQ")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);
            std::string skip = "skip_eq" + std::to_string(labelCounter++);

            out.push_back({ "LI", rd, "0", "", "" });
            out.push_back({ "CMP", r1, r2, "", "" });
            out.push_back({ "JNE", skip, "", "", "" });
            out.push_back({ "LI", rd, "1", "", "" });
            out.push_back({ "LABEL", skip, "", "", "" });
        }

        // ================= NEQ =================
        else if (i.op == "NEQ")
        {
            std::string r1 = getReg(i.arg1);
            std::string r2 = getReg(i.arg2);
            std::string rd = getReg(i.result);
            std::string skip = "skip_neq" + std::to_string(labelCounter++);

            out.push_back({ "LI", rd, "0", "", "" });
            out.push_back({ "CMP", r1, r2, "", "" });
            out.push_back({ "JEQ", skip, "", "", "" });
            out.push_back({ "LI", rd, "1", "", "" });
            out.push_back({ "LABEL", skip, "", "", "" });
        }

        // ================= CMP =================
        else if (i.op == "CMP")
        {
            out.push_back({ "CMP", getReg(i.arg1), getReg(i.arg2), "", "" });
        }

        // ================= AND =================
        else if (i.op == "AND")
        {
            out.push_back({ "AND", getReg(i.result), getReg(i.arg1), getReg(i.arg2), "" });
        }

        // ================= OR =================
        else if (i.op == "OR")
        {
            out.push_back({ "OR", getReg(i.result), getReg(i.arg1), getReg(i.arg2), "" });
        }

        // ================= NOT =================
        else if (i.op == "NOT")
        {
            out.push_back({ "NOT", getReg(i.result), getReg(i.arg1), "", "" });
        }

        // ================= STORE =================
        else if (i.op == "STORE")
        {
            if (i.result.empty() || i.arg1.empty()) {
                continue;  
            }
            out.push_back({ "STORE", getReg(i.result), getReg(i.arg1), "", "" });
        }

        // ================= CALL =================
        else if (i.op == "CALL")
        {
            out.push_back({ "CALL", i.arg1, "", "", "" });

            if (!i.result.empty() && i.result != "R0") {
                out.push_back({ "MOV", i.result, "R0", "", "" });
            }
        }

        // ================= RET =================
        else if (i.op == "RET" || i.op == "RETURN")
        {
            // Handle return value
            if (!i.arg1.empty() && i.arg1 != "0")
            {
                std::string r = getReg(i.arg1);
                if (r != "R0") {
                    out.push_back({ "MOV", "R0", r, "", "" });
                }
            }
            else if (i.arg1 == "0")
            {
                out.push_back({ "LI", "R0", "0", "", "" });
            }

            if (currentFunction == "main")
            {
                out.push_back({ "HALT", "", "", "", "" });
            }
            else
            {
                out.push_back({ "RET", "", "", "", "" });
            }
            }

        // ================= PARAM =================
        else if (i.op == "PARAM")
        {
            // Debug output
            //std::cout << "PARAM: arg1=" << i.arg1 << " arg2=" << i.arg2 << " result=" << i.result << std::endl;

            std::string r = getReg(i.arg1);

            if (i.arg2.empty()) {
                out.push_back({ "PUSH", r, "", "", "" });
            }
            else {
                int offset = std::stoi(i.arg2) * 4;
                out.push_back({ "STORE", "SP+" + std::to_string(offset), r, "", "" });
            }
        }

        // ================= Jumps =================
        else if (i.op == "JLE")
        {
            out.push_back({ "JLE", i.result, "", "", "" });
        }
        else if (i.op == "JGE")
        {
            out.push_back({ "JGE", i.result, "", "", "" });
        }
        else if (i.op == "JLT")
        {
            out.push_back({ "JLT", i.result, "", "", "" });
        }
        else if (i.op == "JGT")
        {
            out.push_back({ "JGT", i.result, "", "", "" });
        }
        else if (i.op == "JEQ")
        {
            out.push_back({ "JEQ", i.result, "", "", "" });
        }
        else if (i.op == "JNE")
        {
            out.push_back({ "JNE", i.result, "", "", "" });
        }
        else if (i.op == "JZ")
        {
            out.push_back({ "BEQZ", getReg(i.arg1), i.result, "", "" });
        }
        else if (i.op == "JNZ")
        {
            out.push_back({ "BNEZ", getReg(i.arg1), i.result, "", "" });
        }

        // ================= MOV =================
        else if (i.op == "MOV")
        {
            std::string rd = getReg(i.result);
            std::string rs = getReg(i.arg1);
            out.push_back({ "MOV", rd, rs, "", "" });
        }

        // ================= PUSH / POP =================
        else if (i.op == "PUSH")
        {
            out.push_back({ "PUSH", i.arg1, "", "", "" });
        }
        else if (i.op == "POP")
        {
            out.push_back({ "POP", i.arg1, "", "", "" });
        }

        // ================= DECLARE (ignore) =================
        else if (i.op == "DECLARE")
        {
            // nothing needed
        }

        else
        {
            throw std::runtime_error("Unknown IR opcode: " + i.op);
        }
    }
    std::unordered_map<std::string, int> labelMap;
    int pc = 0;
    for (auto& instr : out) {
        if (instr.op == "LABEL") {
            labelMap[instr.rd] = pc;
        }
        pc += 4; // each instruction is 4 bytes
    }
    for (auto& instr : out) {
        if (instr.op == "J" || instr.op == "JLE" || instr.op == "JGE" ||
            instr.op == "JLT" || instr.op == "JGT" || instr.op == "JEQ" || instr.op == "JNE" ||
            instr.op == "BEQZ" || instr.op == "BNEZ" || instr.op == "CALL") {
            if (labelMap.count(instr.rd)) {
                instr.rd = std::to_string(labelMap[instr.rd]);
            }
        }
    }

    if (currentFunction.empty()) {
        out.push_back({ "HALT", "", "", "", "" });
    }

    return out;
}
