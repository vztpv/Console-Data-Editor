# Objective : 
              first time, load EU4(Paradox game) savedgame Data and change savedgame Data.
              now, load any data(satisfy following grammar) and change data and save data.
              ( For EU4, DHR, Stellaris(gamestate), HOI4(if save_as_binary is no in settings.txt and save) )
            
# precautions :
                1. this source code has maybe many bugs!. and maybe need many memory.
                
                2. put wiz folder to your std header file location(include).
                
                3. need more speed up! ( ex) linear search -> binary search? )
                
                4. some notation is korean!.
                
# ToDo : 
                1. File Open Fail Processing, fix bug, and speed up, and make new header file and etc.. 
                2. Some HOI4(and etc..) db file is lua file! - so need process for new notation --  
                3. add information of text mode.
                
# Using C++11, VisualStudio2015, Windows10

# LoadData
 LoadData From File or std::string!
 
# # is comment delimiter!

# Usage
  1. Up : w or W
  2. Down : s or S
  
  3. Enter : Enter key
  
  4. Back : b
  5. Quit : q
  
  6. Editor mode : e 
      ( add - a, change - c, remove - r, save - s )
  7. Text mode : t ( only at main screen, first screen )
  
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
    | _left _val2 _right
    | _val2 _left _val2 _right

_var1: STR

_var2: STR 

_left: LEFT 

_right: RIGHT
