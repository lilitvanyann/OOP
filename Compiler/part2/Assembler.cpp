#include "Assembler.h"
#include <stdexcept>

Assembler::Assembler()
{
    regMap.clear();
    regIndex = 5; // R5–R7 are temp registers; R0=ret, R1/R2=args, R3=unused, R4=addr scratch
    labelCounter = 0;
    nextVarAddr = 0x8000; // data segment at 32KB, safely past any reachable code
    inFuncPrologue = false;
}

// register allocator (t1 → R5 etc.)

std::string Assembler::getReg(const std::string& temp)
{
    if (temp.empty())
        return "R0";

    // Pass real register names through unchanged
    if (temp.size() == 2 && temp[0] == 'R' && isdigit(temp[1]))
        return temp;

    if (regMap.find(temp) != regMap.end())
        return regMap[temp];

    std::string r = "R" + std::to_string(regIndex);
    regMap[temp] = r;

    regIndex++;
    if (regIndex > 7)
        regIndex = 5;

    return r;
}

// Returns the fixed memory address (as a string) for a variable name.
// Bug fix: variable names are NOT registers; they live in a data segment.
std::string Assembler::getVarAddress(const std::string& varName)
{
    if (varMem.find(varName) == varMem.end()) {
        varMem[varName] = nextVarAddr;
        nextVarAddr += 4;
    }
    return std::to_string(varMem[varName]);
}


// IR → Assembly

std::vector<AssemblyInstruction>
Assembler::generate(const std::vector<IRInstruction>& ir)
{
    std::vector<AssemblyInstruction> out;

    // Jump to main so execution skips over multiply/countdown bodies
    out.push_back({ "J", "main", "", "", "" });

    for (auto& i : ir)
    {
        // Reset prologue flag on any instruction that isn't part of FUNC_BEGIN/PARAM sequence
        if (i.op != "FUNC_BEGIN" && i.op != "PARAM") {
            inFuncPrologue = false;
        }
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
        // Bug fix: i.result is a variable name, not a register.
        // Load its address into R4, then STORE the value register into that address.
        else if (i.op == "ASSIGN")
        {
            std::string srcReg = getReg(i.arg1);
            std::string addrStr = getVarAddress(i.result);
            out.push_back({ "LI",    "R4", addrStr, "", "" });
            out.push_back({ "STORE", srcReg, "R4",  "", "" });
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
            inFuncPrologue = true;
            out.push_back({ "LABEL", i.result, "", "", "" });
            // No BP/SP frame: R[29]/R[30] are out-of-bounds for the 8-register CPU.
            // Arguments are passed in R1/R2 and stored to the data segment by PARAM below.
        }

        // ================= FUNC_END =================
        else if (i.op == "FUNC_END")
        {
            // Safety RET in case the function body had no explicit return.
            // For main, RET already emitted HALT; no extra instruction needed.
            if (i.result != "main") {
                out.push_back({ "RET", "", "", "", "" });
            }
            currentFunction = "";
            inFuncPrologue = false;
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

        // Bug fix: i.arg1 is a variable name, not a register.
        // Load its address into R4, then LOAD from that address.
        else if (i.op == "LOAD")
        {
            std::string dstReg = getReg(i.result);
            std::string addrStr = getVarAddress(i.arg1);
            out.push_back({ "LI",   "R4", addrStr, "", "" });
            out.push_back({ "LOAD", dstReg, "R4",  "", "" });
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
            std::string r1 = getReg(i.arg1);
            std::string r2;
            // If arg2 is a numeric literal (e.g. "0"), load it into R4 first.
            if (!i.arg2.empty() && isdigit((unsigned char)i.arg2[0])) {
                out.push_back({ "LI", "R4", i.arg2, "", "" });
                r2 = "R4";
            }
            else {
                r2 = getReg(i.arg2);
            }
            out.push_back({ "CMP", r1, r2, "", "" });
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
            int idx = i.arg2.empty() ? 0 : std::stoi(i.arg2);
            std::string argReg = "R" + std::to_string(idx + 1); // R1=arg0, R2=arg1

            if (inFuncPrologue) {
                // Callee side: copy incoming register arg into the parameter's data segment slot.
                std::string addrStr = getVarAddress(i.arg1);
                out.push_back({ "LI",    "R4", addrStr, "", "" });
                out.push_back({ "STORE", argReg, "R4",  "", "" });
            }
            else {
                // Caller side: move the argument value into the arg-passing register.
                std::string srcReg = getReg(i.arg1);
                out.push_back({ "MOV", argReg, srcReg, "", "" });
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
        // IR convention: arg1 = destination, arg2 = source, result = unused
        else if (i.op == "MOV")
        {
            std::string rd = getReg(i.arg1);
            std::string rs = getReg(i.arg2);
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

    // Bug fix: RET in main already emits HALT; only add a safety HALT when needed.
    if (out.empty() || out.back().op != "HALT") {
        out.push_back({ "HALT", "", "", "", "" });
    }

    return out;
}
