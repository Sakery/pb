P0

5 G=0
10 PRINT "SLOT GAME"
20 INPUT "AMOUNT OF BET",A
30 B=1
40 FOR C=1 TO 12
50 X=INT(RAN#*10)
60 Y=INT(RAN#*10)
70 Z=INT(RAN#*10)
80 PRINT CSR 1;X;":";Y;":";Z;
90 FOR D=1 TO C^2
100 NEXT D
110 NEXT C
120 IF X=Y;B=B*4
130 IF Y=Z;B=B*4
140 IF X=Z;B=B*4
150 IF B=1;B=B-1
160 G=G+B*A:STOP
170 PRINT CSR 1;B*A;"(";G;")"
180 GOTO 20
