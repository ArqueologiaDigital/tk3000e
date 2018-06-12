; Firmware clone do Super Parallel Card da Microdigital
; Vers�o 1.0
; 23/01/2004
;
; Emulador do computador TK3000 //e (Microdigital)
; por F�bio Belavenuto - Copyright (C) 2004
;
; Este arquivo � distribuido pela Licen�a P�blica Geral GNU.
; Veja o arquivo Licenca.txt distribuido com este software.
; ESTE SOFTWARE N�O OFERECE NENHUMA GARANTIA

; Dispositivo de Impress�o
; Endere�o e descri��o: (R=Leitura, W=Escrita, RW=Leitura e escrita)
; $C0x0 - W  - Byte para ser impresso

IORTS		.EQU	$FF58
ENDIMPTELA	.EQU	$FDF0
ENDBASE		.EQU	$C080
CH		.EQU	$24
STDOUT		.EQU	$36
TEMP		.EQU	$538
CTRL		.EQU	$638
FLAGS		.EQU	$6B8
MAXH		.EQU	$4B8
POSH		.EQU	$738

	.ORG	0

;NOTA: Para identifica��o do cart�o paralelo, os seguintes endere�os dever tem os
;      seguintes valores:
;$Cx05 = $48
;$Cx07 = $48
	
	CLC
	.DB	$B0		; BCS
INICIO	
	SEC
	PHA
	TXA
	PHA
	TYA
	PHA
	PHP
	SEI
	JSR	IORTS
	TSX
	PLA
	PLA
	PLA
	PLA
	TAY
	DEX
	TXS
	PLA
	TAX
	PLP
	TYA
	PHA
	BCS	MANDA_CARAC
	LDA	#$89
	STA	CTRL,X
	LDA	#$80
	STA	FLAGS,X
	LDA	#40
	STA	MAXH,X
	LDA	#INICIO
	STA	STDOUT
	STX	STDOUT+1	
MANDA_CARAC
	TXA
	ASL	A
	ASL	A
	ASL	A
	ASL	A
	TAY
VERIFICA_CH	
	LDA	POSH,X
	CMP	CH
	PLA
	BCS	PULA1
	PHA
	AND	#$80
	ORA	#$20
PULA1
	BIT	IORTS
	BEQ	MANDA_IMPRESSORA
	INC	POSH,X
	BVS	MANDA_IMPRESSORA
INTERPRETA_CTRL
	TYA
	AND	#$7F
	CMP	#13
	BEQ	FLAG_NAOCTRL
	CMP	#32
	BCC	MUDA_CTRL
	EOR	#$30
	CMP	#10
	BCC	E_NUMERO
	EOR	#$7E
	BEQ	COMP_LINHA
	CMP	#$07
	BEQ	FLAG_NAOCTRL
	CMP	#$0E
	BNE	PULA_RESET
	LDA	#$89
	STA	CTRL,X
FLAG_NAOCTRL
	LDA	#$80
GRAVA_FLAGS
	STA	FLAGS,X
	BCS	SAI_SEMTELA
PULA_RESET
	CMP	#$14
	BNE	PULA_DESABILITA_CTRL
MUDA_CTRL
	TYA
	STA	CTRL,X
PULA_DESABILITA_CTRL
	LDA	#$80
	AND	FLAGS,X
COMP_LINHA
	SEC
	BCS	GRAVA_FLAGS
MANDA_IMPRESSORA
	STA	ENDBASE,Y
	BCC	VERIFICA_CH
	TAY
	LDA	FLAGS,X
	LSR	A
	BCS	INTERPRETA_CTRL
	TYA
	EOR	#$8D
	ASL	A
	BEQ	E_RETURN
	LDA	CTRL,X
	ASL	A
	BMI	VERIFICA_IMP_TELA
	TYA
	EOR	CTRL,X
	ASL	A
	BNE	VERIFICA_IMP_TELA
	INC	FLAGS,X
	CLV
	BVC	SALVA_TEMP
E_NUMERO	
	LDY	#10
LOOP_NUM	
	ADC	TEMP,X
	DEY
	BNE	LOOP_NUM
	STA	MAXH,X
SALVA_TEMP
	STA	TEMP,X
	BVC	SAIR
COMPARA_CH
	CMP	CH
	BCC	ZERA_CH
	BCS	SAIR
E_RETURN	
	STA	CH
	STA	POSH,X
VERIFICA_IMP_TELA	
	LDA	FLAGS,X
	ASL	A
	LDA	#39
	BCS	COMPARA_CH
	LDA	POSH,X
	SBC	MAXH
	CMP	#$F8
	BCC	ZERA_CH
	ADC	#39
	.DB	$AC	;LDY XXXX
ZERA_CH
	LDA	#$00
	STA	CH
SAI_SEMTELA	
	CLV
SAIR
	PLA
	TAY
	PLA
	TAX
	PLA
	BVC	SEM_TELA
	JMP	ENDIMPTELA
SEM_TELA
	RTS
	
	.DB	$A0,$A0
	.DB	"IMPRESSORA"
	.DB	$A0,$A0
	
	.ORG	$FF
	.DB	$60
	
	.END	
