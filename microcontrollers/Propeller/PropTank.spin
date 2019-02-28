{{
Object file:    PropTank.spin
Version:        1.0.0
Date:           6/16/2018
Author:         Jonathan Chin
Licensing:      MIT License - see end of file for terms of use.

Description:
Use to control toy robot tank                 
}}
CON
  'Set up the clock mode
  Delay = 9_000_000
  
  'SparkFun Motor Driver - Dual TB6612FNG 
  PWMA = 22
  AIN2 = 21
  AIN1 = 20
  STBY = 19
  BIN1 = 18
  BIN2 = 17
  PWMB = 16

OBJ
  'HC-05
  pc_serial        : "FullDuplexSerial"
  bt_serial        : "FullDuplexSerial"
  arduino_serial   : "FullDuplexSerial"  
  pwm_control      : "Pwm"
  motor            : "Tb6612fng"
  num              : "Numbers"

VAR
  byte pc_input[64]
  byte bt_input[64]
  byte arduino_input[64] 
  byte pc_word[64]
  byte bt_word[64]
  byte arduino_word[64]

  long SqStack[32]                'Stack space for Square cog
  long ArdStack[32]
  byte  CogIDPWMA                 'Stores the ID of new cog
  byte  CogIDPWMB                 'Stores the ID of new cog

  long tempSpeed


PUB Main | i
  pc_serial.Start(31, 30, %0000, 9_600)      ' rx, tx
  bt_serial.Start(13, 15, %0000, 9_600)

  pc_word[0] := 0     

  cognew(Read_Bt_Input, @SqStack)   'Launch square cog
      
  repeat
    pc_input[0] := pc_serial.Rx
    if pc_input[0] == "!"
      bt_serial.Str(@pc_word)
      pc_word[0] := 0  
    else
      pc_input[1] := 0
      bytemove(@pc_word + strsize(@pc_word),@pc_input,strsize(@pc_input)+1)


PUB Read_Bt_Input
  {{
       ! : direction
       @ : speed
  }}

  dira[AIN2]~~                'Set I/O pin to output direction 
  dira[AIN1]~~
  dira[STBY]~~
  dira[BIN1]~~
  dira[BIN2]~~

  dira[PWMA]~~
  dira[PWMB]~~

  outa[AIN2] := 0
  outa[AIN1] := 0
  outa[STBY] := 0
  outa[BIN1] := 0
  outa[AIN2] := 0
  outa[BIN2] := 0
 
  bt_word[0] := 0
  tempSpeed := 0
   
  motor.init(AIN1, AIN2, PWMA, BIN1, BIN2, PWMB, STBY)

  repeat
    bt_input[0] := bt_serial.Rx  
    if bt_input[0] == "!"
      if strcomp(@bt_word, string("n"))
        motor.operateSync(motor#CMD_CCW)
        motor.setSpeedSync(tempSpeed)  
      elseif strcomp(@bt_word, string("e"))
        motor.operateAsync(motor#CMD_CW, motor#CMD_CCW)
        motor.setSpeedSync(tempSpeed)  
      elseif strcomp(@bt_word, string("s"))
        motor.operateSync(motor#CMD_CW)
        motor.setSpeedSync(tempSpeed)  
      elseif strcomp(@bt_word, string("w"))
        motor.operateAsync(motor#CMD_CCW, motor#CMD_CW)
        motor.setSpeedSync(tempSpeed) 
      elseif strcomp(@bt_word, string("h"))
        motor.operateSync(motor#CMD_STOP)
        motor.setSpeedSync(tempSpeed) 
      elseif strcomp(@bt_word, string("ne"))
        motor.operateSync(motor#CMD_CCW)
        motor.setSpeedAsync(0, tempSpeed)
      elseif strcomp(@bt_word, string("nw"))
        motor.operateSync(motor#CMD_CCW)
        motor.setSpeedAsync(tempSpeed, 0) 
      elseif strcomp(@bt_word, string("se"))
        motor.operateSync(motor#CMD_CW)
        motor.setSpeedAsync(0, tempSpeed)
      elseif strcomp(@bt_word, string("sw"))
        motor.operateSync(motor#CMD_CW)
        motor.setSpeedAsync(tempSpeed, 0)
       
      bt_word[0] := 0
    elseif bt_input[0] == "@"
      tempSpeed := num.FromStr(@bt_word, num#DEC)    
      bt_word[0] := 0
    else
      bt_input[1] := 0
      bytemove(@bt_word + strsize(@bt_word),@bt_input,strsize(@bt_input)+1)                  
    
PUB Wait_Seconds(seconds)
{{
  Pauses the calling cog's execution for approximately one second.
    
  parameters:    none
  return:        none
  
  example usage: Wait_1_Second
}}

  waitcnt(cnt + (seconds*Delay))

PUB Stop(id)
  if id > -1
    cogstop(id)                      'Stop previously launched cog 

PUB PrintNewLine
{{
  Prints a newline character to the terminal.
    
  parameters:    none
  return:        none
  
  example usage: PrintNewLine
}}

  pc_serial.Tx($0D)                                        'ASCII 13 = carriage return