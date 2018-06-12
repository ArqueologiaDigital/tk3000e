; Firmware do Disco Rígido
; Versão 1.0
; 23/01/2004
;
; Emulador do computador TK3000 //e (Microdigital)
; por Fábio Belavenuto - Copyright (C) 2004
;
; Este arquivo é distribuido pela Licença Pública Geral GNU.
; Veja o arquivo Licenca.txt distribuido com este software.
; ESTE SOFTWARE NÃO OFERECE NENHUMA GARANTIA

; Dispositivo de HD
; Endereco e descrição: (R=Leitura, W=Escrita, RW=Leitura e escrita)
; $C0x0 - W  - Comando ProDOS (0=Status, 1=Leitura, 2=Escrita, 3=Formatação)
; $C0x1 - RW - Número do Dispositivo (0 ou 1) 
; $C0x2 - RW - Byte menos significativo do número do bloco
; $C0x3 - RW - Byte mais significativo do número do bloco
; $C0x4 - RW - Byte Lido ou Byte para gravar
; $C0x5 - R  - Status ($00=OK, $27=Erro I/O, $28=Sem Dispositivo, $2B=Protegido contra escrita)

SLOTSAVE	.EQU $2B
CMD		.EQU $42
UNIT		.EQU $43
BUF		.EQU $44
BLK		.EQU $46
TMP		.EQU $FF
BOOTBUF		.EQU $0800
ENDBASE		.EQU $C080
AUTOSCAN	.EQU $FABA
IORTS		.EQU $FF58

		.ORG 0

;NOTA: Para identificação do sistema ProDOS os seguintes offsets devem ter os
;      seguintes valores:
;$Cx01 = $20
;$Cx03 = $00
;$Cx05 = $03
;$Cx07 = $3C

	LDA #$20
	LDX #$00
	LDA #$03
	LDA #$3C

;Qual Slot?

	PHP
	SEI				;desabilita interrupções
	JSR	IORTS			;obtém na pilha o PC atual
	TSX
	LDA	$0100,X			;A contém $Cx
	PLP
	TAX
	ASL	A			;passar para $x0
	ASL	A
	ASL	A
	ASL	A
	STA	SLOTSAVE
	
;Carrega bloco 0 na memória apontada por BOOTBUF

	STA	UNIT			;configura UNIT para $x0
	LDA	#$01			;configura Comando de Leitura
	STA	CMD
	LDA	#BOOTBUF & $FF		;configura buffer para BOOTBUF
	STA	BUF
	LDA	#BOOTBUF >> 8
	STA	BUF+1
	LDA	#$00			;configura bloco para 0000
	STA	BLK
	STA	BLK+1
	
;Simula 'JSR DRIVER'

	TXA				;guarda endereço de retorno
	PHA
	LDA	#RETORNO-1
	PHA
	TXA				;guara endereço do driver
	PHA
	LDA	#DRIVER-1
	PHA
	RTS				;'JSR DRIVER'

RETORNO
	BCC	BOOT			;Se não houve erro, inicia sequência de BOOT
	JMP	AUTOSCAN		;Se houve erro, continuar com a sequência de scan
BOOT
	LDX	SLOTSAVE
	JMP	BOOTBUF+1

;Ponto de entrada para o Driver

DRIVER
	LDA	TMP
	PHA
	LDA	#$60
	STA	TMP
	PHP
	SEI				;desabilita interrupções
	JSR	TMP			;obtém na pilha o PC atual
	TSX
	LDA	$0100,X			;A contém $Cx
	PLP
	ASL	A			;passar para $x0
	ASL	A
	ASL	A
	ASL	A
	TAX
	PLA
	STA	TMP

;ProDOS Unit #
; bit 7 = 0 é primeira unidade, 1 é segunda unidade.

	LDA	UNIT			;converter ProDOS unit para # da unidade
	AND	#$80
	CLC
	ROL	A
	ROL	A
	STA	ENDBASE+1,X
	LDA	BLK			;configura # do bloco
	STA	ENDBASE+2,X
	LDA	BLK+1
	STA	ENDBASE+3,X

;Manipula comando

	LDA	CMD
	CMP	#$02
	BEQ	ESCRITA
	CMP	#$01
	BEQ	LEITURA
	STA	ENDBASE+0,X
	BNE	FORMAT_STATUS		;comandos STATUS e FORMAT não precisam de mais nada
LEITURA
ESCRITA
	LDY	#$00			;inicia ponteiro Y
	CLC				;C=0 = primeiros 256 bytes, C=1 = segundos 256 bytes
PROXIMO_BYTE
	LDA	CMD			;pega comando (read/write)
	EOR	#$01			;Leitura:01 EOR 01 = 00, Escrita:10 EOR 01 = 11
	BNE	ESCREVE_BYTE
LE_BYTE
	LDA	CMD
	STA	ENDBASE+0,X		;manda comando para o dispositivo
	LDA	ENDBASE+4,X		;le dados
	STA	(BUF),Y			;coloca-o no buffer
	INY				;incrementa ponteiro
	BNE	PROXIMO_BYTE
	BEQ	PROXIMO_SETOR
ESCREVE_BYTE
	LDA	(BUF),Y			;le byte do buffer
	STA	ENDBASE+4,X		;coloca-o no dispositivo
	LDA	CMD
	STA	ENDBASE+0,X		;manda comando para o dispositivo
	INY
	BNE	PROXIMO_BYTE
PROXIMO_SETOR
	BCS	TERMINADO		;se C estiver configurado, então já terminou os seg. 256 bytes
	INC	BUF+1
	SEC				;configura para ler segundos 256 bytes
	BNE	PROXIMO_BYTE
TERMINADO
	DEC	BUF+1
FORMAT_STATUS
	CLC				;pega status do dispositivo
	LDA	ENDBASE+5,X
	BNE	ERRO
	LDA	CMD			;sem erros
	BNE	SAIR			;sai se o comando não for STATUS
	LDA	ENDBASE+3,X		;pega # blocos, byte mais significativo
	TAY
	LDA	ENDBASE+2,X		;pega # blocos, byte menos significativo
	TAX
SAIR
	LDA	#$00
	RTS
ERRO
	SEC
	RTS
	
	.DB	$A0,$A0
	.DB	"Firmware HD - "
	.DB	"By Fábio Belavenuto",$A0,$A0
	
;Informações do dispositivo

; $CsFE = status bits (BAP p7-14)
;  7 = medium is removable
;  6 = device is interruptable
;  5-4 = number of volumes (0..3 means 1..4)
;  3 = device supports Format call
;  2 = device can be written to
;  1 = device can be read from (must be 1)
;  0 = device status can be read (must be 1)


	.ORG	$FC
	.DB	$00,$00			;# blocos desconhecidos
	.DB	$9F			;bits de Status
	.DB	$(DRIVER)		;ponteiro para o driver
	
	.END
	