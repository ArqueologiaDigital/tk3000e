; Firmware da placa TKCLOCK
; Versão 1.0
; 15/03/2004
;
; Emulador do computador TK3000 //e (Microdigital)
; por Fábio Belavenuto - Copyright (C) 2004
;
; Este arquivo é distribuido pela Licença Pública Geral GNU.
; Veja o arquivo Licenca.txt distribuido com este software.
; ESTE SOFTWARE NÃO OFERECE NENHUMA GARANTIA

; Modos de Trabalho:
;
; $A3 = '#' : Formato ProDOS		"S,dd,mm,HH,mi,ss"		-> 16 carac
; $BE = '>' : Formato Interger BASIC	"mm/dd HH;mi;ss.Saa"		-> 18 carac
; $A0 = ' ' : Formato AppleClock	"mm/dd HH;mi;ss.Saa"		-> 18 carac
; $A5 = '%' : Formato Applesoft BASIC	"SSS mmm dd hh:mi:ss XM"	-> 22 carac
; $A6 = '&' : Formato Applesoft BASIC	"SSS mmm dd HH:mi:ss"		-> 19 carac
; $BA = ':' : Formato TKCLOCK		"S mm/dd/aa HH:mi:ss"		-> 19 carac

REGA		.EQU	$C080
CTRLREGA	.EQU	$C081
REGB		.EQU	$C082
CTRLREGB	.EQU	$C083
CONVSLT		.EQU	$C93E
XX1EXT		.EQU	$C9BE
XX2EXT		.EQU	$CAAD
SAIDA		.EQU	$C864
CONFIRQ		.EQU	$C96B	; CONFIGURA INT
XX5EXT		.EQU	$C948	; PROCESSA  INT
CLRROM		.EQU	$CFFF

STDOUT		.EQU	$36
STDIN		.EQU	$38
ACC		.EQU	$45
PILHA		.EQU	$0100
IRQLOC		.EQU	$03FE
L04B8		.EQU	$04B8
MODO		.EQU	$0538
ULTC		.EQU	$05B8
SLOT16		.EQU	$0778
MSLOT		.EQU	$07F8

	.ORG	0

;NOTA: Para identificação do TKCLOCK os seguintes endereços devem ter os valores:
;
; Cx00 = $08
; Cx01 = $78
; Cx02 = $28
; *Cx04 = $58
; *Cx05 = $FF
; *Cx06 = $70
; *Cx07 = $05
; *CxFC = $D4
; *CxFD = $CD
; *CxFE = $B2
; *CxFF = $03

	.DB	$08,$78,$28,$2C,$58,$FF,$70,$05

INICIO:
	SEC
	BCS	PULA1
INICIO2:
	CLC
PULA1:
	CLV
	PHP
	SEI
	PHA
	TXA
	PHA
	TYA
	PHA
	LDA	CLRROM
	JSR	CONVSLT
	STA	SLOT16
	STX	MSLOT
	BVC	LC45A
	LDA	STDOUT
	BNE	LC42A
	CPX	STDOUT+1
	BEQ	LC430
LC42A:
	LDA	#INICIO
	STA	STDIN
	BNE	LC461
LC430:
	LDA	#INICIO2
	STA	STDOUT
	LDX	MSLOT
	LDA	#$04
	STA	MODO,X
	STA	L04B8,X
	TSX
LC440:
	LDA	PILHA+3,X
	AND	#$7F
	EOR	#$08
	CMP	#$28
	BCC	LC46B
	CMP	#$37
	BCS	LC46B
	AND	#$0F
	LSR	A
	LDY	MSLOT
	STA	MODO,Y
	BNE	SAIR
LC45A:	
	TSX
	LDA	PILHA+4,X
	LSR	A
	BCC	LC440
LC461:
	JSR	XX1EXT
SAIR:
	PLA
	TAY
	PLA
	TAX
	PLA
	PLP
	RTS
LC46B:
	EOR	#$08
	LDX	MSLOT
	CMP	#$2E
	BEQ	LC4A6
	CMP	#$2A
	BEQ	LC495
	CMP	#$41
	BCC	SAIR
	CMP	#$5B
	BCS	SAIR
	AND	#$1F
	LDY	MODO,X
	BPL	LC49C
	ASL	A
	SBC	#$00
	STA	L04B8,X
	TYA
	AND	#$7F
LC490:
	STA	MODO,X
	BCS	SAIR
LC495:
	LDA	MODO,X
	ORA	#$80
	BNE	LC490
LC49C:
	CMP	#$17
	BCS	SAIR
	JSR	XX2EXT
	JMP	SAIDA
LC4A6:
	CPX	IRQLOC+1
	BEQ	LC4B1
	JSR	CONFIRQ
	STX	IRQLOC+1
LC4B1:
	LDA	#ENTINT
	STA	IRQLOC
	LDA	MODO,X
	ORA	#$40
	STA	MODO,X
	LDY	SLOT16
	LDA	#$04
	STA	CTRLREGB,Y
	LDA	#$05
	STA	CTRLREGA,Y
	BNE	SAIR
ENTINT:
	TYA
	PHA
	TXA
	PHA
	LDA	CLRROM
	JSR	CONVSLT
	JSR	XX5EXT
	PLA
	TAX
	PLA
	TAY
	LDA	ACC
	RTI

	.ORG	$FC
	.DB	$D4,$CD,$B2,$03
	.END
