#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<cassert>
#include<unordered_map>
#include<functional>
using namespace std;

vector<string>R_instr={"add","sub","sll","sltu","xor","srl","sra","or","and","addw","subw","sllw","slrw","mul","divu","rem"};
vector<string>I_instr={"addi","andi","andw","slti","slri","addiw","jalr","sltiu","ori"};
vector<string>S_instr={"sw","sb","sh","sd"};
vector<string>SB_instr={"beq","bne","blt","bge","bltu","bgeu"};
vector<string>U_instr={"auipac", "lui"};
vector<string>UJ_instr={"jal"};
vector<string>pseudo_instr={"beqz","bnez","li","mv","neg","nop","bgt","ble"};
vector<string>L_instr={"lb","lw","ld","lwu","lbu"};

vector<pair<string,int>>general_purpose_register(32,{"0",0});
vector<string>Instruction_Memory;
unordered_map<string,string>data_memory;
int PC=0;
int PC_valid=1;

string Rp(vector<string>&tokens);
string Ip(vector<string>&tokens);
string Sp(vector<string>&tokens);
string SBp(vector<string>&tokens);
string Up(vector<string>&tokens);
string UJp(vector<string>&tokens);
string Psp(vector<string>&tokens);
string Lp(vector<string>&tokens);

class Machine_Code{
    public:
        static vector<string>tokenize(const string&line){
            stringstream ss(line);
            string token;
            vector<string>tokens;
            
            while(ss >> token){
                tokens.push_back(token);
            }
            return tokens;
        }
        static string interpret(vector<string>&tokens){
            auto parser_func = determine_type(tokens[0]);
            return parser_func(tokens);
        }
    private:
        static bool valid_line(const string& line){
            return !line.empty()&& line[0] != '#';
        }
        static function<string(vector<string>&)>determine_type(const string &instr){
            vector<vector<string>>instr_sets={R_instr,I_instr,S_instr,SB_instr,U_instr,UJ_instr,pseudo_instr,L_instr};
            vector<function<string(vector<string>&)>>parsers={Rp , Ip , Sp , SBp , Up , UJp , Psp,Lp};
            
            for(size_t i=0;i < instr_sets.size() ; i++){
                if(find(instr_sets[i].begin(), instr_sets[i].end(), instr)!=instr_sets[i].end()){
                    return parsers[i];
                }
            }
            throw invalid_argument("Bad Instruction Provided: "+instr +"!");
        }
};

string convert_to_bit(string a){
    char reg=a[1];
    
    int imm_value=reg-'0';
    if(imm_value <-2048 || imm_value>2047){
        throw out_of_range("Immediate value out of 12_bit range");
    }
    string reg_bit = bitset<5>(imm_value).to_string();
    return reg_bit;
}

string safe_substr(const string& str,size_t pos,size_t len){
    if(pos>=str.size())return string(len, '0');
    if(pos + len > str.size())return str.substr(pos) + string(pos + len- str.size() , '0');
    return str.substr(pos,len);
    
}

string imm_bits(string imm){
    int imm_value =stoi(imm);
    if(imm_value <-2048 || imm_value>2047){
        throw out_of_range("Immediate value out of 12_bit");
    }
    string imm_binary = bitset<12>(imm_value).to_string();
    return imm_binary;
}

string imm_bits_20(const string &imm){
    int imm_value = stoi(imm);
    if(imm_value <-524288 || imm_value >524287){
        throw out_of_range("Immediate value is out of 20-bit range");
    }
    string imm_binary = bitset<20>(imm_value).to_string();
    return imm_binary;
}

string Rp(vector<string>&tokens){
    string func7, func3;
    if(tokens[0]=="add"){
        func7 ="0000000";
        func3="000";
    }
    else if(tokens[0]=="sub"){
        func7 ="0100000";
        func3="000";
    }
    else if(tokens[0]=="sll"){
        func7 ="0000000";
        func3="001";
    }
    else if(tokens[0]=="sltu"){
        func7 ="0000000";
        func3="011";
    }
    else if(tokens[0]=="xor"){
        func7 ="0000000";
        func3="100";
    }
    else if(tokens[0]=="srl"){
        func7 ="0000000";
        func3="101";
    }
    else if(tokens[0]=="sra"){
        func7 ="0100000";
        func3="101";
    }
    else if(tokens[0]=="or"){
        func7 ="0000000";
        func3="110";
    }
    else if(tokens[0]=="and"){
        func7 ="0000000";
        func3="111";
    }
    string rd = convert_to_bit(tokens[1]);
    string rs1=convert_to_bit(tokens[2]);
    string rs2 =convert_to_bit(tokens[3]);
    
    return func7 + rs2 + rs1 + func3 + rd + "0110011";
}

string Ip(vector<string>&tokens){
    
    string func3;
    if(tokens[0]=="addi"){
        
        func3="000";
    }
    else if(tokens[0]=="slti"){
        
        func3="010";
    }
    else if(tokens[0]=="sltiu"){
        
        func3="011";
    }
    else if(tokens[0]=="xori"){
        
        func3="100";
    }

    else if(tokens[0]=="ori"){
        
        func3="110";
    }
    else if(tokens[0]=="andi"){
        
        func3="111";
    }
    string rd = convert_to_bit(tokens[1]);
    string rs1=convert_to_bit(tokens[2]);
    string imm =imm_bits(tokens[3]);
    // if(tokens[0]=="and"){
    // cout<<"imm_ "<<imm<<" rs1 "<<rs1<<" rd "<<rd<<" imm "<<imm<<"\n";}
    return imm + rs1 + func3 + rd + "0010011";
}

string Sp(vector<string>&tokens){
    string func3;
    if(tokens[0]=="sw"){
        
        func3="010";
    }
    else if(tokens[0]=="sb"){
        
        func3="000";
    }
    else if(tokens[0]=="sh"){
        
        func3="001";
    }
    else if(tokens[0]=="sd"){
        
        func3="011";
    }
    
    string t3="";
    int i=0;
    while(tokens[2][i]!='('){
        i++;
    }
    i++;
    while(tokens[2][i]!=')'){
        t3+=tokens[2][i];
        i++;
    }
    
    string rs2 = convert_to_bit(tokens[1]);
    string imm=imm_bits(tokens[2]);

    string rs1 =convert_to_bit(t3);
    
    return safe_substr(imm,0,7) + rs2 + rs1 + func3 + safe_substr(imm,7,5) + "0100011";
}

string SBp(vector<string>&tokens){
    
    string func3;
    if(tokens[0]=="beq"){
        
        func3="000";
    }
    else if(tokens[0]=="bne"){
        
        func3="001";
    }
    else if(tokens[0]=="blt"){
        
        func3="100";
    }
    else if(tokens[0]=="bge"){
        
        func3="101";
    }

    else if(tokens[0]=="bltu"){
        
        func3="110";
    }
    else if(tokens[0]=="bgeu"){
        
        func3="111";
    }
    string rs1 = convert_to_bit(tokens[1]);
    string rs2=convert_to_bit(tokens[2]);
    string imm =imm_bits(tokens[3]);
    //cout<<"rs2 "<<safe_substr(imm , 0 , 7)<<" rs1 "<<safe_substr(imm,8,4)<<" imm "<<imm<<"\n";
    return safe_substr(imm , 0 , 7) + rs2 + rs1 + func3 + safe_substr(imm,7,5) + "1100011";
}

string Up(vector<string>&tokens){
    string rd = convert_to_bit(tokens[1]);
    string imm= imm_bits_20(tokens[2]);
    
    if(tokens[0]=="lui"){
        return imm+rd+"0110111";
    }
    else if(tokens[0]=="auipc"){
        return imm+rd+"0010111";
    }
    else{
        return 0;
    }
}  

string UJp(vector<string>&tokens){
    string rd = convert_to_bit(tokens[1]);
    string imm= imm_bits(tokens[2]);
    imm+="00000000";
    //cout<<"jump "<<imm<<"\n";
        return imm + rd + "1101111";
    //return safe_substr(imm,0,1) + safe_substr(imm, 10 , 10)+safe_substr(imm , 9 , 1)+safe_substr(imm , 1 , 8) + rd + "1101111";
}

string Psp(vector<string>&tokens){
    return "000000000000000000000000000010011";
}

string Lp(vector<string>&tokens){
    string rd = convert_to_bit(tokens[1]);
    string imm =imm_bits(tokens[2]);
    
    string t3="";
    int i=0;
    while(tokens[2][i]!='('){
        i++;
    }
    i++;
    while(tokens[2][i]!=')'){
        t3+=tokens[2][i];
        i++;
    }
    string rs1 =convert_to_bit(t3);
    string func3;
    if(tokens[0]=="lb"){
        func3="000";
    }
    else if(tokens[0]=="lbu"){
        func3="100";
    }
    else if(tokens[0]=="ld"){
        func3="011";
    }
    else if(tokens[0]=="lw"){
        func3="010";
    }
    else if(tokens[0]=="lwu"){
        func3="110";
    }
    
    return imm + rs1 +func3+ rd+ "0000011";
    
}


class AssemblyConverter{
    public:
        AssemblyConverter(const string& output_mode="a",bool nibble_mode=false,bool hex_mode=false)
            : output_mode_(output_mode), nibble_mode_(nibble_mode), hex_mode_(hex_mode){}
        
        void convert(const vector<string>&assembly_code){
            vector<string>binary_code;
            for(const string &line : assembly_code){
                if(!line.empty()){
                    auto tokens=Machine_Code::tokenize(line);
                    if(!tokens.empty()){
                        binary_code.push_back(Machine_Code::interpret(tokens));
                    }
                }
            }
            output(binary_code);
        }
        void output(const vector<string>&binary_code)const{
            if(output_mode_=="p"){
                for(const string& code: binary_code){
                    //cout<< code <<endl;
                    Instruction_Memory.push_back(code);
                }
            }
            else if(output_mode_ == "f"){
                ofstream out_file("output.bin");
                for(const string& code :binary_code){
                    out_file<< code <<endl;
                }
                out_file.close();
            }
        }
        
    private:
        string output_mode_;
        bool nibble_mode_;
        bool hex_mode_;
};


//Pipeline::::::::::::starts:::::::::::


int string_to_decimal(string s){
    string num = s;
    int dec_val = 0;
    int base = 1;
    int len = num.length();
    for(int i= len-1;i >=0;i--){
        if(num[i] == '1'){
            dec_val += base;
        }
        base = base * 2;
    }
    return dec_val;
}

//Fetch:::::::
class Fetch{
    public:
    struct fetch_data{
        int IFID_stall;
        int IFID_valid;
        string DPC;
        string NPC;
        string IR;
    };
    fetch_data FD;
    void fetch_func(){
        if(FD.IFID_stall || !PC_valid)return;
        FD.NPC = to_string(PC+1);
        FD.DPC = to_string(PC);
        if(PC<Instruction_Memory.size()){
            FD.IR = Instruction_Memory[PC];
        }
        FD.IFID_valid = true;
        PC+=1;
        return;
        
    }
    void print_fetch(){
        cout<<"Fetched data: \n";
        cout<<"DPC: "<< FD.DPC<<" | NPC: "<< FD.NPC<<" |IR: "<< FD.IR;
    }
};
//decode:::::::::
class Instruction_decode{
    public:
    struct decode_data{
        string JPC;
        string DPC;
        string NPC;
        string imm;
        string rdl;
        string func3;
        string func7;
        string rs1;
        string rs2;
        int IDEX_valid;
        int IDEX_stall;
    };
    struct IDEX_CW{
        bool RegRead;
        bool ALUSrc;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        string ALUOp;
        bool Branch;
        bool Jump;
        bool zeroflag;
        bool Mem2Reg;
    };
    decode_data IDEX;
    IDEX_CW CW;
    //Controller:::::::::
    IDEX_CW controller(string opcode){
        
        CW.RegRead=true;
        CW.RegWrite=false;
        CW.ALUSrc =false;
        CW.ALUOp =-1;
        CW.Branch=false;
        CW.Jump=false;
        CW.MemWrite=false;
        CW.MemRead=false;
        CW.Mem2Reg=false;
        CW.zeroflag=false;
        //r-type
        if(opcode=="0110011"){
             CW.RegRead=true;
            CW.RegWrite=true;
            CW.ALUSrc=false;
            CW.MemRead=false;
            CW.MemWrite=false;
            CW.Branch=false;
            CW.ALUOp="10";
        }
        //imm
        else if(opcode=="0010011" || opcode=="1110011"){
            CW.RegWrite=true;
            CW.ALUSrc=true;
            CW.MemRead=false;
            CW.MemWrite=false;
            CW.Branch=false;
            CW.ALUOp="11";
        }
        //load
        else if(opcode=="0000011"){
            CW.Mem2Reg=true;
            CW.RegWrite=true;
            CW.ALUSrc=true;
            CW.MemRead=true;
            CW.MemWrite=false;
            CW.Branch=false;
            CW.ALUOp="00";
        }
        //store
        else if(opcode=="0100011"){
            CW.RegWrite=false;
            CW.ALUSrc=true;
            CW.MemRead=false;
            CW.MemWrite=true;
            CW.Branch=false;
            CW.ALUOp="00";
        }
        //branch
        else if(opcode=="1100011"){
            CW.RegWrite=false;
            CW.ALUSrc=false;
            CW.MemRead=false;
            CW.RegRead=true;
            CW.MemWrite=false;
            CW.Branch=true;
            CW.ALUOp="01";
        }
        //jal
        else if(opcode=="1101111"){
            CW.RegWrite=false;
            CW.ALUSrc=false;
            CW.MemRead=false;
            CW.Jump=true;
            CW.RegRead=false;
            CW.MemWrite=false;
            CW.Branch=false;
            CW.ALUOp="00";
        }
        
        
        return CW;
    }
    // IDEX_CW CW;
    void decode_func(Fetch::fetch_data &IFID){
        
        if(IDEX.IDEX_stall || !IFID.IFID_valid)return;
        
        IDEX.JPC =to_string(stoi(IFID.NPC)-string_to_decimal(IFID.IR.substr(0,12)));
        IDEX.DPC=IFID.DPC;
        IDEX.imm =IFID.IR.substr(0,12);
        IDEX.rdl =IFID.IR.substr(20,5);
        IDEX.func3 =IFID.IR.substr(17,3);
        IDEX.func7 =IFID.IR.substr(0,7);
        
        CW =controller(IFID.IR.substr(25,7));
        if(CW.Branch==1){
            IDEX.imm=IFID.IR.substr(0,7)+IFID.IR.substr(20,5);
        }
        if(CW.RegRead==1){
            if(general_purpose_register[string_to_decimal(IFID.IR.substr(12,5))].second==0){
                IDEX.rs1 =general_purpose_register[string_to_decimal(IFID.IR.substr(12,5))].first;
            }
            else{
                IFID.IFID_stall=true;
                return;
            }
            
        }
        if(CW.ALUSrc==1){
            if(CW.RegRead==1){
                IDEX.rs2 =IFID.IR.substr(0,12);
                IDEX.rs2=to_string(string_to_decimal(IDEX.rs2));
            }
        }
        else{
            if(CW.RegRead==1){
                if(general_purpose_register[string_to_decimal(IFID.IR.substr(7,5))].second==0){
                    IDEX.rs2=general_purpose_register[string_to_decimal(IFID.IR.substr(7,5))].first;
                }
                else{
                    IFID.IFID_stall=true;
                    return;
                }
                
            }
        }
        //added
        if(CW.MemWrite==1){
            IDEX.rs1=IFID.IR.substr(12,5);
            IDEX.rdl =IFID.IR.substr(7,5);
            IDEX.rs2=IFID.IR.substr(0,7)+IFID.IR.substr(20,5);
            IDEX.rs2 =to_string(string_to_decimal(IDEX.rs2));
            IDEX.rs1 =to_string(string_to_decimal(IDEX.rs1));
        }
        
        if(CW.MemRead==1){
            IDEX.rs1=IFID.IR.substr(12,5);
            IDEX.rdl =IFID.IR.substr(20,5);
            IDEX.rs2=IFID.IR.substr(0,12);
            IDEX.rs2 =to_string(string_to_decimal(IDEX.rs2));
            IDEX.rs1 =to_string(string_to_decimal(IDEX.rs1));
        }
        
        general_purpose_register[string_to_decimal(IDEX.rdl)].second=-1;
        IFID.IFID_stall=false;
        IDEX.IDEX_valid=true;
        return;
        
    }
    void print_data(){
        if(IDEX.DPC=="")return;
        cout<<"Decoded Data: \n";
        cout<<"DPC: "<< IDEX.DPC<< " |func3: "<< IDEX.func3<< " |func7: "<<IDEX.func7<<endl;
    }
};

class Execution{
    public:
    struct Execution_data{
        string ALUOut;
        int EXMO_valid;
        int EXMO_stall;
        string TPC;
        string rdl;
    };
    struct EXMO_CW{
        bool RegRead;
        bool ALUSrc;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        string ALUOp;
        bool Branch;
        bool Jump;
        bool Mem2Reg;
        bool zeroflag;
};
    string ALU(string ALUControl,string operand1, string operand2, bool &zeroflag){
        int op1 =stoi(operand1);
        int op2 =stoi(operand2);
        int result;
        if(ALUControl=="0000"){
            result =op1 & op2;//AND
        }
        else if(ALUControl=="0001"){
            result= op1 | op2;//or
        }
        else if(ALUControl=="0110"){
            result= op1 - op2;//sub
        }
        
        else if(ALUControl=="0010"){
            result= op1 + op2;//add
        }
        else if(ALUControl=="0011"){
            result= op1 | op2;//xor
        }
        else if(ALUControl=="0101"){
            result= op1 << op2;//sll
        }
        else if(ALUControl=="0111"){
            result= op1 >> op2;//srl
        }
        else if(ALUControl=="1000"){
            result= op1 >> op2;//sra
        }
        else if(ALUControl=="1001"){
            result= (op1 < op2) ? 1 : 0;//slt
        }
        else if(ALUControl=="1011"){
            result= op1 * op2;//mult
        }
        else{
            result =0;
        }
        //remaining
    
    zeroflag=(result==0);
    return to_string(result);
        
    }
    
    string ALUControl(Instruction_decode::IDEX_CW &IDEX_CW,Instruction_decode::decode_data &IDEX){
        if(IDEX_CW.ALUOp == "00"){
            return "0010";
        }
        else if(IDEX_CW.ALUOp == "01"){
            return "0110";
        }
        else if(IDEX_CW.ALUOp == "10"){
            if(IDEX.func3 == "000" && IDEX.func7 == "0000000"){
                return "0010";//add
            }
            else if(IDEX.func3 == "000" && IDEX.func7 == "0100000"){
                return "0110";//sub
            }
            else if(IDEX.func3 == "111" && IDEX.func7 == "0000000"){
                return "0000";//and
            }
            else if(IDEX.func3 == "110" && IDEX.func7 == "0000000"){
                return "0001";//or
            }
            else if(IDEX.func3 == "100" && IDEX.func7 == "0000000"){
                return "0011";//xor
            }
            else if(IDEX.func3 == "001" && IDEX.func7 == "0000000"){
                return "0101";//sll
            }
            else if(IDEX.func3 == "101" && IDEX.func7 == "0000000"){
                return "0111";//srl
            }
            else if(IDEX.func3 == "101" && IDEX.func7 == "0100000"){
                return "1000";//sra
            }
            else if(IDEX.func3 == "010" && IDEX.func7 == "0000000"){
                return "1001";//slt
            }
            else if(IDEX.func3 == "011" && IDEX.func7 == "0000000"){
                return "1010";//sltu
                
            }
            else if(IDEX.func3 == "000" && IDEX.func7 == "0000001"){
                return "1011";//mul
            }
        }
        else if(IDEX_CW.ALUOp == "11"){
            if(IDEX.func3=="000"){
                return "0010";
            }
            else if(IDEX.func3 == "111"){
                return "0000";
            }
            else if(IDEX.func3 == "110"){
                return "0001";
            }
            else if(IDEX.func3 == "100"){
                return "0011";
            }
            else if(IDEX.func3 == "001"){
                return "0101";
            }
            else if(IDEX.func3 == "101" && IDEX.func7 == "0000000"){
                return "0111";
            }
            else if(IDEX.func3 == "101" && IDEX.func7 == "0100000"){
                return "1000";
            }
        }
        return "xxxx";
        
    }
    Execution_data EXMO;
    EXMO_CW CW;
    void controlWordEqual(Instruction_decode::IDEX_CW &idexCW){
        CW.RegRead=idexCW.RegRead;
        CW.ALUSrc=idexCW.ALUSrc;
        CW.MemRead=idexCW.MemRead;
        CW.RegWrite=idexCW.RegWrite;
        CW.MemWrite=idexCW.MemWrite;
        CW.ALUOp=idexCW.ALUOp;
        CW.Branch=idexCW.Branch;
        CW.Jump=idexCW.Jump;
        CW.Mem2Reg=idexCW.Mem2Reg;
    }
    int execute_func(Instruction_decode::decode_data& IDEX,Instruction_decode::IDEX_CW& IDEX_CW,Fetch::fetch_data& IFID){
        if(EXMO.EXMO_stall || !IDEX.IDEX_valid)return 0;
        
        controlWordEqual(IDEX_CW);
        EXMO.rdl=IDEX.rdl;
        string ALUSelect =ALUControl(IDEX_CW,IDEX);
        EXMO.ALUOut =ALU(ALUSelect,IDEX.rs1,IDEX.rs2,IDEX_CW.zeroflag);
        //bool ALUZeroFlag =(IDEX.rs1==IDEX.rs2);
        CW.MemWrite=IDEX_CW.MemWrite;
        
        int ALUZeroFlag =((IDEX.rs1)==(IDEX.rs2));
        CW.zeroflag=ALUZeroFlag;
        if(IDEX_CW.Branch && ALUZeroFlag){
            // int len=IDEX.imm.length();
            // string imm1 =IDEX.imm.substr(1,len-1);
            // imm1+="0";
            EXMO.TPC=to_string(string_to_decimal(IDEX.imm)+stoi(IDEX.DPC));
            IFID.IFID_valid=false;
            IDEX.IDEX_valid=false;
            PC_valid=false;
        }
        else if(IDEX_CW.Jump){
            EXMO.TPC=IDEX.JPC;
            IFID.IFID_valid=false;
            IDEX.IDEX_valid=false;
            PC_valid=false;
        }
        IDEX.IDEX_stall=false;
        EXMO.EXMO_valid=true;
        return 0;
    }
    void print_execute(){
        if(CW.ALUOp=="")return;
        cout<<"Execute Data :\n";
        cout<<"CW.Mem2Reg"<<CW.Mem2Reg<<"CW.ALUOp"<<CW.ALUOp<<"CW.MemRead"<<CW.MemRead<<"CW.ALUSrc"<<CW.ALUSrc<<endl;
    }

};

class Memory_Access{
    public:
    struct Memory_data{
        string LDout;
        string ALUout;
        string rdl;
        int MOWB_valid;
        int MOWB_stall;
        string TPC;
    };
    
    struct MOWB_CW{
        bool RegRead;
        bool ALUSrc;
        bool MemRead;
        bool MemWrite;
        bool RegWrite;
        string ALUOp;
        bool Branch;
        bool Jump;
        bool Mem2Reg;
    };
    MOWB_CW CW;
    void controlWordEqual(Execution::EXMO_CW &idexCW){
        CW.RegRead=idexCW.RegRead;
        CW.ALUSrc=idexCW.ALUSrc;
        CW.MemRead=idexCW.MemRead;
        CW.RegWrite=idexCW.RegWrite;
        CW.MemWrite=idexCW.MemWrite;
        CW.ALUOp=idexCW.ALUOp;
        CW.Branch=idexCW.Branch;
        CW.Jump=idexCW.Jump;
        CW.Mem2Reg=idexCW.Mem2Reg;
    }
    Memory_data MOWB;
    void Memory_func(Execution::Execution_data &EXMO,Execution::EXMO_CW &EXMO_CW,Instruction_decode::decode_data &IDEX){
        //Memory_data MOWB;
        if(MOWB.MOWB_stall || !EXMO.EXMO_valid)return;
        
        MOWB.rdl=EXMO.rdl;
        if(EXMO_CW.MemWrite){
            //data_memory[EXMO.ALUOut]=IDEX.rs2;
            data_memory[to_string(string_to_decimal(MOWB.rdl))]=general_purpose_register[stoi(EXMO.ALUOut)].first;
        }
        if(EXMO_CW.MemRead){
            MOWB.LDout =data_memory[EXMO.ALUOut];
        }
        MOWB.ALUout=EXMO.ALUOut;
        controlWordEqual(EXMO_CW);
        
        if((CW.Branch && EXMO_CW.zeroflag) || CW.Jump){
            PC=stoi(EXMO.TPC);
            PC_valid=true;
        }
        
        EXMO.EXMO_stall=false;
        MOWB.MOWB_valid=true;
        return;
    }
    
    void print_memory(){
        if(CW.ALUOp=="")return;
        cout<<"CW.Mem2Reg"<<CW.Mem2Reg<<"CW.ALUOp"<<CW.ALUOp<<"CW.MemRead"<<CW.MemRead<<"CW.ALUSrc"<<CW.ALUSrc<<endl;

    }

};


class Register_Write{
    public:
    void Reg_write_func(Memory_Access::MOWB_CW &MOWB_CW,Memory_Access::Memory_data &MOWB){
        if(!MOWB.MOWB_valid)return;
        if(!MOWB_CW.RegWrite)return;
        
            else if(MOWB_CW.Mem2Reg && general_purpose_register[string_to_decimal(MOWB.rdl)].second==-1){
                general_purpose_register[string_to_decimal(MOWB.rdl)].first=MOWB.LDout;
            }
            else{
                general_purpose_register[string_to_decimal(MOWB.rdl)].first=MOWB.ALUout;
            }
            general_purpose_register[string_to_decimal(MOWB.rdl)].second=0;
            MOWB.MOWB_stall=false;
            return;
        
    }
};



int main(){
    // vector<string> assemblycode={
    //     "ld x4, 4(x3)",
    //     "addi x2, x2, 6",
    //     "add x3, x1, x2",
    //     "beq x1, x2, 7",
    //     "addi x1, x1, 1",
    //     "addi x0, x0, 1",
    //     "add x5, x5, x2",
    //     "addi x6, x6, 2",
    //     "jal x7, 6"
    // };
    
    vector<string> assemblycode={
      "sw x6, 1(x3)",
      "beq x0, x4, 5",
      "addi x0, x0, 1",
      "add x2, x2, x0",
      "jal x5, 4",
    };
    
    // "addi x7, x8, 10",
    AssemblyConverter converter("p",false,false);
    
    converter.convert(assemblycode);
    
    for(int i=0;i<31;i++){
        data_memory[to_string(i+1)]=to_string(i+2);
    }
    
    for(int i=0;i<Instruction_Memory.size();i++){
        cout<<Instruction_Memory[i]<<"\n";
    }
    
    
    Fetch fetchUnit;
    Instruction_decode decode;
    Execution execute;
    Memory_Access memory_access;
    Register_Write reg_write;
    
    fetchUnit.FD.IFID_stall=0;
    fetchUnit.FD.IFID_valid=0;
    
    decode.IDEX.IDEX_stall=0;
    decode.IDEX.IDEX_valid=0;
    
    execute.EXMO.EXMO_stall=0;
    execute.EXMO.EXMO_valid=0;
    
    memory_access.MOWB.MOWB_stall=0;
    memory_access.MOWB.MOWB_valid=0;
    
    int i=0;
    general_purpose_register[4].first="5";
    
    while(PC<=Instruction_Memory.size()+3){
        cout<<"\ncycle"<<i+1<<":\n";
        
        reg_write.Reg_write_func(memory_access.CW,memory_access.MOWB);
        
        memory_access.Memory_func(execute.EXMO,execute.CW,decode.IDEX);
        memory_access.print_memory();
        
        execute.execute_func(decode.IDEX,decode.CW,fetchUnit.FD);
        execute.print_execute();
        
        decode.decode_func(fetchUnit.FD);
        decode.print_data();
        
         fetchUnit.fetch_func();
        fetchUnit.print_fetch();
        i++;
    }
    
    cout<<"\n\nThe general Purpose Registers are: \n";
    for(int i=0;i<32;i++){
        cout<<general_purpose_register[i].first<<" ";
    }
    cout<<"\n\nThe data memory is :\n";
    for(auto it:data_memory){
        cout<<it.first<<"_"<<it.second<<"  ";
    }
    
    return 0;
}    
    
 