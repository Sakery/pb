P0 (DEFINE 25 MORE VARIABLES WITH COMMAND "DEFM 25")

1 PRINT "SEARCH"
2 GOSUB#9:FOR B=0 TO 2:IF $=E$(B) THEN 5
3 NEXT B
4 PRINT "NO TR.NAME":GOTO 2
5 INPUT "H ",D:FOR C=H(B) TO H(B+1)-1:IF D<=INT L(C);GOSUB 7
6 NEXT C:GOTO 5
7 PRINT E$(B);C-H(B)+1;" :":SET F2:PRINT L(C):SET N:RETURN

P1

1 PRINT "ENTER";A+1:IF A>=3;PRINT "TIME TAB OVF":GOTO 1
2 H(A)=K:GOSUB#9:E4(A)=$:A=A+1
3 IF K>40;PRINT "TIME TAB OVF":GOTO 3
4 PRINT D$(A);K-G(A)+1;":":INPUT "H.M ",L(K):K=K+1:GOTO 3

P2

1 PRINT "TIMETABLE"
2 VAC
3 GOTO#1

P9

1 INPUT "TR. NAME ",$:IF LEN($)>7;$=,MID(1,7)
2 RETURN
