# Conecta-K - Documentación del Proyecto

Este repositorio contiene el código fuente y la documentación del juego "Conecta-K", desarrollado como parte del curso de la asignatura Proyecto Hardware utilizando el procesador LPC2105 y los periféricos asociados.

## Descripción del Hardware

El LPC2105 es un microcontrolador basado en el núcleo ARM7TDMI-S de 32 bits fabricado por NXP Semiconductors. Ofrece una variedad de periféricos integrados como puertos GPIO, temporizadores, ADC (Analog to Digital Converter) y comunicación serie. Destaca por su versatilidad y eficiencia en el consumo de energía.

## Entorno de Desarrollo

Se ha utilizado Keil uVision como entorno de desarrollo integrado (IDE) para programar microcontroladores ARM. Keil uVision proporciona un editor de código, compilador, enlazador, depurador y simulador, facilitando la escritura, compilación y depuración de programas. Es versátil y admite múltiples dispositivos, ofreciendo herramientas para análisis estático y simulación de periféricos.

## Optimización del Código

El código ha sido compilado en modo O3 (nivel de optimización más alto disponible), mejorando el rendimiento pero empeorando la legibilidad del código.

## Estructura del Proyecto

El trabajo se divide en tres prácticas, centrando la documentación en las dos últimas:

1. **Práctica 1:** Optimización del rendimiento de funciones principales mediante código en ensamblador ARM.
2. **Práctica 2:** Desarrollo modular de periféricos y sistema de eventos. Se crearon módulos de bajo nivel (timers, botones, GPIO), gestores de alto nivel y estructuras para la gestión de eventos.
3. **Práctica 3:** Continuación de la segunda parte y unión con la primera. Modificación de módulos y desarrollo de un juego completamente funcional.

## Funcionamiento del Sistema

El sistema inicializa todos los componentes necesarios (timers, GPIO, UART, cola FIFO, alarmas, juego) y trata las interrupciones generadas por los periféricos. El planificador gestiona los eventos generados, llamando a los gestores correspondientes para su procesamiento.

