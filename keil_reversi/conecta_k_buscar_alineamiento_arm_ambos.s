;************************conecta_k_buscar_alineamiento_arm_ambos************************
;
;		pre: r0 -> *tablero; r1 -> fila; r2-> columna; r3-> color; 
;		r4 y r5 se pasan a través de la pila siendo los parámetros delta_fila y delta_columna respectivamente
;		post: devuelve el numero de K seguidos hacia ambos lados
;
;***********************************************************************
							AREA datos,READONLY 
K_size						EQU		4						; conecta K	
Max_fil						EQU		6						; máx fila
Max_col						EQU		6						; máx col 	
Min_fil						EQU		0						; min fila
Min_col						EQU		0						; min col 	

									
									AREA codigo,code
									EXPORT conecta_K_buscar_alineamiento_arm_ambos
										
									;prólogo
conecta_K_buscar_alineamiento_arm_ambos	MOV 	IP, SP						; guarda la pos de la stack antes de la funcion 
										PUSH	{r4-r10,FP,IP,LR}
										mov		FP, SP
									
								;**************************PARÁMETROS****************************************
									ldr 	r4, [FP,#40]				; r4 = deltas_fila // parámetro
									ldr 	r5, [FP, #44]				; r5 = deltas_col	//parámetro
									mov		r10, #0
									mov		r9, #0
									
							;********************comprobaci?n valores v?lidos***************************
bucle								mov 	r7, #Max_fil				; cambio esto de sitio porq no puedo poner un inmediato ene l cmp primero
									mov 	r6, #Max_col				; cambio esto de sitio porq no puedo poner un inmediato en l cmp primero
									cmp		r1, #Min_fil				; comprueba si es una fila v?lida	
									cmpge 	r2, #Min_col				; comprueba si es una columna v?lida
									cmpge	r7,r1						; comprueba si es una fila v?lida
									cmpge 	r6,r2						; comprueba si es una columna v?lida
									blt		no_exito					
							;***************************************************************************
									mov		r7, #6						; r7 = 6
									mul		r6, r1, r7					; r6 = r1*6
									add		r6, r6, r0					; r6 = tablero en la fila correcta
									mov 	r7, #0						; r7 = i
buscar_columna						ldrsb	r8, [r6], #1				; r8 = @r6 (correct row), postindexa #1(col++)
									cmp		r8, #-1						; valor vacío
									beq		no_exito
									cmp		r8, r2						; columnaActual(r8) == columa(r2)
									beq		fin_buscar
									add		r7, r7, #1					; i++ (columna en la que esta el color buscada)
									b		buscar_columna
									
							;********************** FIN BUSCAR COLUMNA ***********************************
fin_buscar							add		r6, r0, #42					; se encuentran los colores correspondientes a la fila y columna buscada
									mov 	r8, #6						
									mul		r8, r1, r8					; filas a avanzar
									add		r6, r6, r8					; r6 = tablero en la fila correcta
buscar_color						;cmp	r7, #0					;
									add		r6,r6,r7					; r6 += columna en la que se encuntra el dato
encontrado							ldrb	r8, [r6]					; r8 = @colorActual
									AND		r8, #0x03					; ultimos dos bits importan
									cmp		r8, r3						; colorActual == color(r3)
									bne		no_exito
									add 	r1,r1,r4					; si son iguales, r1=r1+r4 --> fila = fila + delta_fila
									add 	r2,r2,r5					; r2=r2+r5 --> columna = columna + delta_columna
									add 	r9,r9,#1					; res++
									b 		bucle
									
													
no_exito							cmp		r9, #K_size					; si res >= K_size, salir_funcion
									cmplt	r10, #1						; sino, si estamos en la segunda "llamada", salir funcion
									bge		salir_funcion
									sub 	r1,r1,r4					; sino actualizo valores ;; r1 = fila+delta_fila
									sub		r2,r2,r5					; r1 = col+delta_col
									rsb		r4, r4,#0					; delta_fila = -delta_fila
									rsb		r5, r5, #0					; delta_col = -delta_col
									add 	r10, r10, #1				; sumo uno al contador de llamadas
									mov 	r9, #0						; res=0;
									b		bucle						; #corresponde con la segunda llamada a la funcion
									
salir_funcion						mov 	r0,r9						; resultado en r0
									POP		{r4-r10,FP,IP,PC}
									END
								
								