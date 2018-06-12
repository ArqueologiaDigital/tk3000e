; Firmware do Mouse
; Versão 1.0
; 23/01/2004
;
; Emulador do computador TK3000 //e (Microdigital)
; por Fábio Belavenuto - Copyright (C) 2004
;
; Este arquivo é distribuido pela Licença Pública Geral GNU.
; Veja o arquivo Licenca.txt distribuido com este software.
; ESTE SOFTWARE NÃO OFERECE NENHUMA GARANTIA

; Dispositivo do Mouse
; Endereco e descrição: (R=Leitura, W=Escrita, RW=Leitura e escrita)
; $C0x0 - W  - Modo do Mouse   ( 00 à 0F )
; $C0x1 - W  - Eixo do Clamp   ( 00 à 01 )
; $C0x2 - W  - Outros Comandos ( 00 à 06 )

STDOUT		.EQU	$36
STDIN		.EQU	$38
TEXTLENGTH	.EQU	$0638
STATUSINT	.EQU	$06B8
MSLOT		.EQU	$07F8
ENDMODO		.EQU	$C080
ENDCLAMP	.EQU	$C081
ENDCMD		.EQU	$C082
ENDE1		.EQU	$C083
ENDE2		.EQU	$C084

CMDSERVEMOUSE	.EQU	$01
CMDREADMOUSE	.EQU	$02
CMDCLEARMOUSE	.EQU	$03
CMDPOSMOUSE	.EQU	$04
CMDHOMEMOUSE	.EQU	$05
CMDINITMOUSE	.EQU	$06

	.ORG	0

;NOTA: Para identificação do cartão do mouse, os seguintes endereços dever tem os
;      seguintes valores:
;$Cx05 = $38
;$Cx07 = $18
;$Cx0B = $01
;$Cx0C = $20
;$CxFB = $D6

	BIT	$FF58
	BVS	INICIO
CIN
	SEC
	.DB	$90
COUT
	CLC
	CLV
	BVC	INICIO
	.DB	$01,$20

;NOTA: a seguinte tabela deve estar presente, ela aponta para as funções:
;
; Não Documentado:
;$Cx0D ?
;$Cx0E ?
;$Cx0F ?
;$Cx10 ?
;$Cx11 ?
; Documentados:
;$Cx12 SETMOUSE 	Determina modo do mouse;
;$Cx13 SERVEMOUSE 	Verifica se ocorreu alguma interrupção
;$Cx14 READMOUSE 	Ler a posição do mouse
;$Cx15 CLEARMOUSE 	Zerar a posição do mouse
;$Cx16 POSMOUSE 	Mudar a posição do mouse
;$Cx17 CLAMPMOUSE 	Determinar limites do mouse
;$Cx18 HOMEMOUSE 	Cursor do mouse no canto superior-esquerdo
;$Cx19 INITMOUSE 	Inicializar mouse
; Não Documentado:
;$Cx1A GETCLAMP         Adquire limites do mouse
;$Cx1B ?
;$Cx1C TIMEDATA		Mudar frequência de interrupção VBL (50 ou 60 Hertz)
;$Cx1D ?
;$Cx1E ?
;$Cx1F ?

; Quando o software chama estas funções, o reg. X contém Cs, e o Y contém s0

	.DB	SEMFUNCAO		; 0x0D
	.DB	SEMFUNCAO		; 0x0E
	.DB	SEMFUNCAO		; 0x0F
	.DB	SEMFUNCAO		; 0x10
	.DB	SEMFUNCAO		; 0x11
	.DB	SETMOUSE		; 0x12
	.DB	SERVEMOUSE		; 0x13
	.DB	READMOUSE		; 0x14
	.DB	CLEARMOUSE		; 0x15
	.DB	POSMOUSE		; 0x16
	.DB	CLAMPMOUSE		; 0x17
	.DB	HOMEMOUSE		; 0x18
	.DB	INITMOUSE		; 0x19
	.DB	SEMFUNCAO		; 0x1A
	.DB	SEMFUNCAO		; 0x1B
	.DB	SEMFUNCAO		; 0x1C
	.DB	SEMFUNCAO		; 0x1D
	.DB	SEMFUNCAO		; 0x1E
	.DB	SEMFUNCAO		; 0x1F

	.ORG	$20
INICIO
	PHP
	SEI
	STA	MSLOT
	PHA
	TYA
	PHA
	TXA
	PHA
	JSR	$FF58
	TSX
	LDA	$0100,X
	TAX
	PHP
	ASL	A
	ASL	A
	ASL	A
	ASL	A
	PLP
	TAY
	LDA	MSLOT
	STX	MSLOT
	PHA
	BVS	VERIFICAR
	BCS	RECEBE_CHAR
	BCC	ENVIA_CHAR
VERIFICAR
	CPX	STDOUT+1
	BNE	P1
	LDA	#COUT
	CMP	STDOUT
	BEQ	P1
	STA	STDOUT
ENVIA_CHAR
	PLA
	STA	ENDE1,Y
	PLA
	TAX
	PLA
	TAY
	PLA
	PLP
	RTS
P1
	CPX	STDIN+1
	BNE	ENVIA_CHAR
	LDA	#CIN
	STA	STDIN
RECEBE_CHAR
	STA	ENDE2,Y
	PLA
	LDA	TEXTLENGTH,X
	TAX
	PLA
	PLA
	TAY
	PLA
	PLP
	LDA	$0200,X
	RTS

SETMOUSE
	CMP	#$10
	BCS	SETC
	STA	ENDMODO,Y
	RTS

SERVEMOUSE
	PHA
	LDA	#CMDSERVEMOUSE
	STA	ENDCMD,Y
	CLC
	LDA	STATUSINT,X
	AND	#$0E
	BNE	SEMINT
	SEC
SEMINT
	PLA
	RTS

CLAMPMOUSE
	CMP	#$02
	BCS	SETC
	STA	ENDCLAMP,Y
	RTS

READMOUSE
	LDA	#CMDREADMOUSE
	STA	ENDCMD,Y
	LDA	STATUSINT,X
	CLC
	RTS

INITMOUSE
	PHA
	LDA	#CMDINITMOUSE
ENVIACMD
	STA	ENDCMD,Y
	PLA
	RTS

CLEARMOUSE
	PHA
	LDA	#CMDCLEARMOUSE
	BNE	ENVIACMD

POSMOUSE
	PHA
	LDA	#CMDPOSMOUSE
	BNE	ENVIACMD

HOMEMOUSE
	PHA
	LDA	#CMDHOMEMOUSE
	BNE	ENVIACMD
SETC
	SEC
	RTS

SEMFUNCAO
	LDX	#$03
	SEC
	RTS

	.DB     "Firmware Mouse - "
	.DB	"By Fábio Belavenuto",$A0

	.ORG	$FB
	.DB	$D6

	.ORG	$FF
	.DB	$01

	.END
