# Objective : 
              first time, load EU4(Paradox game) savedgame Data and change savedgame Data.
              now, load any data(satisfy following grammar) and change data and save data.
              
# precautions :
                1. this source code has maybe many bugs!. and maybe need many memory.
                
                2. put wiz folder to your std header file location(include).
                
                3. need more speed up! ( ex) linear search -> binary search? )
                
                4. some notation is korean!.
                
# ToDo : 
                1. File Open Fail Processing, fix bug, and speed up, and make new header file and etc.. 
                
# Using C++11, VisualStudio2015, Windows10

# LoadData
 LoadData From File or std::string!

# Grammar(byacc style)
LEFT { RIGHT }  EQ =

program:
    | program _list

_list: _var1 
    | _var2 EQ _val1
    | _var2 EQ _left _val2 _right 
    | _var2 EQ LEFT RIGHT 
    | LEFT RIGHT
    
_val1: STR
    
_val2: _val2 _var1  
    | _var1 
    | _var2 EQ _left _val2 _right 
    | _val2 _var2 EQ _left _val2 _right  
    | _var2 EQ LEFT RIGHT 
    | _val2 _var2 EQ LEFT RIGHT  
    | _var2 EQ _val1 
    | _val2 _var2 EQ _val1  
    | LEFT RIGHT 
    | _val2 LEFT RIGHT  
    | LEFT _val2 RIGHT
    | _val2 LEFT _val2 RIGHT

_var1: STR

_var2: STR 

_left: LEFT 

_right: RIGHT
