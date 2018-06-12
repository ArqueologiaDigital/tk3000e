; Firmware do Relógio
; Versão 1.0
; 23/01/2004
;
; Emulador do computador TK3000 //e (Microdigital)
; por Fábio Belavenuto - Copyright (C) 2004
;
; Este arquivo é distribuido pela Licença Pública Geral GNU.
; Veja o arquivo Licenca.txt distribuido com este software.
; ESTE SOFTWARE NÃO OFERECE NENHUMA GARANTIA

; Dispositivo de Relógio
; Endereco e descrição: (R=Leitura, W=Escrita, RW=Leitura e escrita)
; $C0x0 - R  - Ano (Parte Baixa)			(Formato BCD)
; $C0x1 - R  - Ano (Parte Alta)				(Formato BCD)
; $C0x2 - R  - Mes					(Formato BCD)
; $C0x3 - R  - Dia					(Formato BCD)
; $C0x4 - R  - Dia da Semama (0=Dom, 1=Seg, etc)	(Formato BCD)
; $C0x5 - R  - Hora					(Formato BCD)
; $C0x6 - R  - Minuto					(Formato BCD)
; $C0x7 - R  - Segundo					(Formato BCD)
; $C0x8 - RW - Modo de Trabalho (ver abaixo)
; $C0x9 - R  - Comprimento da String
; $C0xA - RW - Posição na String
; $C0xB - R  - Caractere da String

; Os caracteres da String de formato são:
; S   - Dia da semana (número)		(1 caractere)
; SSS - Dia da semana (texto)		(3 caracteres)
; dd  - Dia do mês			(2 caracteres)
; mm  - Mês (número)			(2 caracteres)
; mmm - Mês (texto)			(3 caracteres)
; aa  - Ano				(2 caracteres)
; HH  - Hora (24 horas)			(2 caracteres)
; hh  - Hora (12 horas)			(2 caracteres)
; mi  - Minuto				(2 caracteres)
; ss  - Segundos			(2 caracteres)
; X   - A ou P (AM ou PM)		(1 caractere)
;
; Modos de Trabalho:
;
; $A3 = '#' : Formato ProDOS		"S,dd,mm,HH,mi,ss"		-> 16 carac
; $BE = '>' : Formato Interger BASIC	"mm/dd HH;mi;ss.Saa"		-> 18 carac
; $A0 = ' ' : Formato AppleClock	"mm/dd HH;mi;ss.Saa"		-> 18 carac
; $A5 = '%' : Formato Applesoft BASIC	"SSS mmm dd hh:mi:ss XM"	-> 22 carac
; $A6 = '&' : Formato Applesoft BASIC	"SSS mmm dd HH:mi:ss"		-> 19 carac
; $BA = ':' : Formato TKCLOCK		"S mm/dd/aa HH:mi:ss"		-> 19 carac

ENDBASE	.EQU $C080
STDOUT	.EQU $36
STDIN	.EQU $38
CARAC	.EQU $538
ULTC	.EQU $5B8
TMP	.EQU $06

	.ORG 0

;NOTA: Para identificação da Placa do Relógio os seguintes endereços devem ter os valores:
;
; Cx00 = $08
; Cx01 = $78
; Cx02 = $28
; Cx04 = $58
; Cx05 = $FF
; Cx06 = $70
; Cx07 = $05

; CxFE = $B2
; CxFF = modo


	.DB	$08,$78,$28,$2C,$58,$FF,$70,$05


PD_LERRELOGIO
	SEC					;Ponto de entrada de leitura do ProDOS
	BCS	ENTRADA_INICIAL
PD_ESCREVERRELOGIO
ENTRADA_STDOUT
	CLC					;Ponto de entrada de escrita do ProDOS
ENTRADA_INICIAL
	CLV					;Limpa flag V para informar que é STDOUT
						;Ponto de Entrada normal
	PHP
	SEI
	PHA
	TXA
	PHA
	TYA
	PHA
	LDX	TMP
	LDA	#$60
	STA	TMP
	JSR	TMP
	STX	TMP
	TSX
	LDA	$0100, X
	TAY
	LDA	$0103, X
	PHA
	TYA
	TAX
	PHP
	ASL A
	ASL A
	ASL A
	ASL A
	PLP
	TAY

	BVC	CHAMADA_STDOUT			;se V=0, é STDOUT
	LDA	STDOUT				;senão verifica se é primeira chamada a stdout
	BNE	CHAMADA_STDIN
	CPX	STDOUT+1
	BNE	CHAMADA_STDIN
	LDA	#ENTRADA_STDOUT
	STA	STDOUT
	LDA	#$00
	STA	CARAC,X
	BEQ     CHAMADA_STDOUT_2
CHAMADA_STDOUT
	BCS	PEGA_DATA_HORA
CHAMADA_STDOUT_2
	PLA
	CLC
	BCC	DETERMINA_MODO
CHAMADA_STDIN
	PLA					; ignora caractere
PEGA_DADO
	TXA
	ASL A
	ASL A
	ASL A
	ASL A
	TAY
	LDA	CARAC,X
	STA	ENDBASE+$A,Y
	LDA	ENDBASE+$B,Y
INC_CONTADOR
	PHA
	LDA	CARAC,X				; se chegou no fim, retorna
	CMP	ENDBASE+$9,Y
	BEQ	RETORNA_CR
	INC	CARAC,X				; senão, próximo caractere
	BNE	RETORNA_CARAC			; sempre pula
RETORNA_CR
	LDA	#$00
	STA	CARAC,X
	PLA
	LDA	#$8D
	PHA
RETORNA_CARAC
	PLA
	STA	ULTC,X
	TAY
	PHP
	SEI					; desabilita interrupções
	TSX
	PLA					; P
	PLA					;original Y
	PLA					;original X
	PLA					;original A
	TYA					;restaura A salvado no Y
	PHA					;e coloca na pilha
	TXS
	PLP					;restaura P
	CLC
	BCC	SAIDA

PD_DETERMINA_MODO
	SEC
DETERMINA_MODO
	PHP
	CMP	ULTC,X
	BEQ	NAOMANDA
	STA	ENDBASE+$8,Y
NAOMANDA
	PLP
SAIDA
	PLA
	TAY
	PLA
	TAX
	PLA
	PLP
SAIR
	RTS

;Pega Data e Hora do cartão
;O registrador X contém $Cx

PEGA_DATA_HORA
	PLA
	STA	ENDBASE+$8,Y
	LDA	#$00
	STA	CARAC,X
	TAY

MAIS_DADOS					;simular um JSR PEGA_DADO
	TXA					;coloca endereço de retorno na pilha
	PHA
	LDA	#RETORNO-1			;RTS incrementará PC
	PHA

	PHP
	PHA					;simular ação da ENTRADA
	TXA
	PHA
	TYA
	PHA

	TXA
	PHA
	LDA	#PEGA_DADO-1			;RTS incrementará PC
	PHA

	RTS					;'JSR PEGA_DADO'
RETORNO
	STA	$0200,Y
	CMP	#$8D
	BEQ	FINAL_STRING
	INY
	BNE	MAIS_DADOS
FINAL_STRING
	LDA	#$00
	STA	$0200,Y
	CLC
	BCC	SAIDA

	.DB	$A0,$A0
	.DB     "Firmware Clock - "
	.DB	"By Fábio Belavenuto",$A0,$A0

	.ORG	$FE
	.DB	$B2,$01

	.END
