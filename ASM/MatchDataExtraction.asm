#**************************************************************************
#                                       Inject at address 800D32FC 
# Unsure what the inject target function does exactly but I do know it ends up calling the
# stock subtraction instructions. It is called once per frame per character. It is also
# called during the score screen
#***************************************************************************

#replaced code line is executed at the end

#***************************************************************************
#                  		subroutine: writeStats
#  description: writes stats to EXI port on each frame
#***************************************************************************
#create stack frame and store link register
mflr r0
stw r0, 0x4(r1)
stwu r1,-0x20(r1)
stw r31,0x1C(r1)
stw r30,0x18(r1)
stw r29,0x14(r1)

#check if there are 3 or more players
lis	r3,0x8016
ori	r3,r3,0xB558 # load CountPlayers function
mtlr r3
blrl
cmpwi r3,3 # 3 or more players in match?
bge- CLEANUP # skip all this if so

#an input to this function is r5, r5 is the pointer of the player currently being considered + 0x60
#skip everything if pointer is not equal to the last players pointer
#load character pointer for this player
li r30, 3 #load last player number first
LAST_PLAYER_CHECK:
lis r3, 0x8045
ori r3, r3, 0x3130
mulli r4, r30, 0xE90 #computer address of this player's pointer
add r3, r3, r4
lwz r3, 0x0(r3)
cmpwi r3,0
bne LAST_PLAYER_FOUND #loop through until we find last player in game

subi r30,r30,1 #decrement player id
cmpwi r30,1
bge LAST_PLAYER_CHECK #iterate until potential last player candidates have been checked

LAST_PLAYER_FOUND:
addi r3, r3, 0x60
cmpw r3,r5
bne CLEANUP #if last valid player found does not equal player being considered, skip

#check if in single player mode, and ignore code if so
lis	r3,0x801A # load SinglePlayer_Check function
ori	r3,r3,0x4340	
mtlr	r3	
lis	r3,0x8048	
lbz	r3,-0x62D0(r3) #load menu controller major
blrl	
cmpwi r3,1 # is this single player mode?	
beq- CLEANUP # if in single player, ignore everything

#check frame count
lis	r3,0x8047
lwz	r3,-0x493C(r3)	# load match frame count
lis	r4,0x8048 #check scene controller frame count to make sure it is zero as well (only want to trigger OnStart on very first frame of match)
lwz	r4,-0x62A8(r4)	# load scene controller frame count

#check scene controller first, if zero it's either start or end of match
cmpwi r4,0
bne UPDATE_OR_RESULTS

#Here the scene controller is equal to zero, either trigger OnStart or OnEnd
cmpwi r3,0
bne ON_END_EVENT #if match frame count is greater than zero, this is the results screen

#------------- ON_START_EVENT -------------
#TEMPORARY: Send dummy event in order to get communication functioning properly. Unsure why this happens, i'll look into it later
#this transaction will get ignored anyway
bl startExiTransfer
li r3, 0xFE
bl sendByteExi
bl endExiTransfer

bl startExiTransfer #indicate transfer start

li r3, 0x37
bl sendByteExi #send OnMatchStart event code

lis r31, 0x8045
ori r31, r31, 0xAC4C

lhz r3, 0x1A(r31) #stage ID half word
bl sendHalfExi

li r30, 0 #load player count

MP_WRITE_PLAYER:
#load character pointer for this player
lis r3, 0x8045
ori r3, r3, 0x3130
mulli r4, r30, 0xE90
add r3, r3, r4
lwz r3, 0x0(r3)

#skip this player if not in game
cmpwi r3, 0
beq MP_INCREMENT

#start writing data
mr r3, r30 #send character port ID
bl sendByteExi

#get start address for this player
lis r31, 0x8045
ori r31, r31, 0xAC4C
mulli r4, r30, 0x24
add r31, r31, r4

lbz r3, 0x6C(r31) #character ID
bl sendByteExi
lbz r3, 0x6D(r31) #player type
bl sendByteExi
lbz r3, 0x6F(r31) #costume ID
bl sendByteExi

MP_INCREMENT:
addi r30, r30, 1
cmpwi r30, 4
blt MP_WRITE_PLAYER

bl endExiTransfer #stop transfer
b CLEANUP

ON_END_EVENT:
#------------- ON_END_EVENT -------------
bl startExiTransfer #indicate transfer start

li r3, 0x39
bl sendByteExi #send OnMatchEnd event code

#check byte that will tell us whether the game was won by stock loss or by ragequit
lis r3, 0x8047
lbz r3, -0x4960(r3)
bl sendByteExi #send win condition byte. this byte will be 0 on ragequit, 1 or 3 on win by stock loss

bl endExiTransfer #stop transfer
b CLEANUP

UPDATE_OR_RESULTS:
#check if we are in results screen, if so, skip update
lis r3, 0x8045
lbz r3, 0x30C9(r3) #this global address exists for all players and appears to be = 1 when in game and = 0 when in results screen
cmpwi r3, 0
beq CLEANUP

#------------- FRAME_UPDATE -------------
bl startExiTransfer #indicate transfer start

li r3, 0x38
bl sendByteExi #send OnFrameUpdate event code

lis	r3,0x8047
lwz	r3,-0x493C(r3) #load match frame count
bl sendWordExi #0

lis r3,0x804D
lwz	r3,0x5F90(r3) #load random seed
bl sendWordExi #4

li r30, 0 #load player count

FU_WRITE_PLAYER:
#load character pointer for this player
lis r3, 0x8045
ori r3, r3, 0x3080
mulli r4, r30, 0xE90
add r4, r3, r4
lwz r3, 0xB0(r4)

#skip this player if not in game
cmpwi r3, 0
beq FU_INCREMENT

mr r29, r4 #load static player memory address into r29
mr r31, r3 #load player address into r31

lwz	r3,0x64(r31) #load internal char ID
bl sendByteExi #8
lwz	r3,0x70(r31) #load action state ID
bl sendHalfExi #9
lwz	r3,0x110(r31) #load Top-N X coord
bl sendWordExi #11
lwz	r3,0x114(r31) #load Top-N Y coord
bl sendWordExi #15
lwz	r3,0x680(r31) #load Joystick X axis
bl sendWordExi #19
lwz	r3,0x684(r31) #load Joystick Y axis
bl sendWordExi #23
lwz	r3,0x698(r31) #load c-stick X axis
bl sendWordExi #27
lwz	r3,0x69c(r31) #load c-stick Y axis
bl sendWordExi #31
lwz	r3,0x6b0(r31) #load analog trigger input
bl sendWordExi #35
lwz	r3,0x6bc(r31) #load buttons pressed this frame
bl sendWordExi #39
lwz	r3,0x1890(r31) #load current damage
bl sendWordExi #43
lwz	r3,0x19f8(r31) #load shield size
bl sendWordExi #47
lwz	r3,0x20ec(r31) #load last attack landed
bl sendByteExi #51
lhz	r3,0x20f0(r31) #load combo count
bl sendByteExi #52
lwz	r3,0x1924(r31) #load player who last hit this player
bl sendByteExi #53

lbz	r3,0x8E(r29) # load stocks remaining
bl sendByteExi #54

FU_INCREMENT:
addi r30, r30, 1
cmpwi r30, 4
blt FU_WRITE_PLAYER

bl endExiTransfer #stop transfer

CLEANUP:
#restore registers and sp
lwz r0, 0x24(r1)
lwz r31, 0x1C(r1)
lwz r30, 0x18(r1)
lwz r29, 0x14(r1)
addi r1, r1, 0x20
mtlr r0

b GECKO_END

#***************************************************************************
#                  subroutine: startExiTransfer
#  description: prepares port B exi to be written to
#***************************************************************************
startExiTransfer:
lis r11, 0xCC00 #top bytes of address of EXI registers

#disable read/write protection on memory pages
lhz r10, 0x4010(r11)
ori r10, r10, 0xFF
sth r10, 0x4010(r11) # disable MP3 memory protection

#set up EXI
li r10, 0xD0 #bit pattern to set clock to 32 MHz and enable CS for device 0
stw	r10, 0x6814(r11) #start transfer, write to parameter register

blr

#***************************************************************************
#                   	subroutine: sendByteExi
#  description: sends one byte over port B exi
#  inputs: r3: byte to send
#***************************************************************************
sendByteExi:
lis r11, 0xCC00 #top bytes of address of EXI registers
li r10, 0x5 #bit pattern to write to control register to write one byte

#write value in r3 to EXI
slwi r3, r3, 24 #the byte to send has to be left shifted
stw	r3, 0x6824(r11) #store current byte into transfer register
stw	r10, 0x6820(r11) #write to control register to begin transfer

#wait until byte has been transferred
EXI_CHECK_RECEIVE_WAIT:                
lwz	r10, 0x6820(r11)
andi. r10, r10, 1
bne	EXI_CHECK_RECEIVE_WAIT #while((exi_chan1cr)&1);

blr

#***************************************************************************
#                   	subroutine: sendHalfExi
#  description: sends two bytes over port B exi
#  inputs: r3: bytes to send
#***************************************************************************
sendHalfExi:
lis r11, 0xCC00 #top bytes of address of EXI registers
li r10, 0x15 #bit pattern to write to control register to write one byte

#write value in r3 to EXI
slwi r3, r3, 16 #the bytes to send have to be left shifted
stw	r3, 0x6824(r11) #store bytes into transfer register
stw	r10, 0x6820(r11) #write to control register to begin transfer

#wait until byte has been transferred
EXI_CHECK_RECEIVE_WAIT_HALF:                
lwz	r10, 0x6820(r11)
andi. r10, r10, 1
bne	EXI_CHECK_RECEIVE_WAIT_HALF #while((exi_chan1cr)&1);

blr

#***************************************************************************
#                   	subroutine: sendWordExi
#  description: sends one word over port B exi
#  inputs: r3: word to send
#***************************************************************************
sendWordExi:
lis r11, 0xCC00 #top bytes of address of EXI registers
li r10, 0x35 #bit pattern to write to control register to write four bytes

#write value in r3 to EXI
stw	r3, 0x6824(r11) #store current bytes into transfer register
stw	r10, 0x6820(r11) #write to control register to begin transfer

#wait until byte has been transferred
EXI_CHECK_RECEIVE_WAIT_WORD:                
lwz	r10, 0x6820(r11)
andi. r10, r10, 1
bne	EXI_CHECK_RECEIVE_WAIT_WORD #while((exi_chan1cr)&1);

blr

#***************************************************************************
#                  subroutine: endExiTransfer
#  description: stops port B writes
#***************************************************************************
endExiTransfer:
mflr r0
stw r0, 0x4(r1)
stwu r1,-0x20(r1)
#TEMPORARY: Send dummy byte at the end of message so fpga can write out last good byte
li r3, 0xFF
bl sendByteExi

lis r11, 0xCC00 #top bytes of address of EXI registers

li r10, 0
stw	r10, 0x6814(r11) #write 0 to the parameter register

lwz r0, 0x24(r1)
addi r1, r1, 0x20
mtlr r0
blr

GECKO_END:
lwz r0, 0x24(r1) #execute replaced code line