		PUBLIC	MXWAITRETRACE
MX_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:MX_TEXT, DS:DGROUP, SS:DGROUP
MXWAITRETRACE:
	mov		dx,3daH
L$1:
	in		al,dx
	test		al,8
	je		L$1
	retf
MX_TEXT		ENDS
		END
