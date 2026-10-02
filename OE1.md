# Estado del Arte: Fundamentos de SF2 y Audio Digital (OE1)

Este documento expone los fundamentos teóricos necesarios para la implementación de un motor de síntesis basado en muestras, abarcando la especificación del formato SoundFont 2 y los principios de procesamiento digital de señales involucrados en la reproducción.

## 1. Formato SoundFont 2 (.sf2)

El formato SoundFont 2 es un estándar desarrollado por E-mu Systems y Creative Labs para la reproducción de audio basada en tablas de ondas (wavetable synthesis). Utiliza la estructura de contenedores RIFF (Resource Interchange File Format) y organiza la información en un modelo jerárquico de tres niveles:

1. **Samples (Muestras):** Datos de audio crudo en formato PCM lineal de 16 bits, generalmente monofónicos o estéreo, grabados a una frecuencia de muestreo específica. Contienen metadatos de bucle (loop points) para mantener la reproducción sostenida de una nota.
2. **Instruments (Instrumentos):** Mapean una o más muestras a lo largo del rango del teclado MIDI (key zones) y de la dinámica (velocity zones). Definen parámetros de articulación a nivel de muestra, como envolventes de volumen (AHDSR), LFOs y filtros paso bajo resonantes.
3. **Presets (Ajustes preestablecidos):** Representan el parche final que se asigna a un canal MIDI. Agrupan uno o más instrumentos, permitiendo divisiones del teclado (splits) o capas (layers).

## 2. Fundamentos de Audio Digital y Muestreo

El audio digital en los sintetizadores wavetable opera bajo el principio de Modulación por Impulsos Codificados (PCM). La viabilidad de este modelo está regida por el **Teorema de Muestreo de Nyquist-Shannon**, el cual establece que para reconstruir exactamente una señal de banda limitada, la frecuencia de muestreo $f_s$ debe ser al menos el doble de la frecuencia máxima contenida en la señal $f_{max}$:

$$f_s \ge 2 f_{max}$$

En la práctica, operar a una frecuencia de muestreo fija (como $44100\text{ Hz}$ o $22050\text{ Hz}$) requiere que cualquier alteración en el tono de una muestra implique modificar la velocidad a la que se leen sus datos desde la memoria.

## 3. Pitch-Shifting (Desplazamiento de Tono)

Para reproducir una muestra a una nota MIDI distinta de su nota original (Root Key), se ajusta su tasa de reproducción (Playback Rate). La relación de frecuencias en el sistema musical de temperamento igual se calcula en base a la distancia en semitonos $\Delta k$ entre la nota objetivo $k_{target}$ y la nota original $k_{root}$:

$$\Delta k = k_{target} - k_{root}$$

El factor de cambio de tono o incremento de fase (Phase Increment), denotado como $\rho$, se obtiene mediante la ecuación:

$$\rho = 2^{\frac{\Delta k}{12}}$$

* Si $\rho = 1$, la muestra se lee a su velocidad original.
* Si $\rho = 2$, la muestra se lee al doble de velocidad (una octava superior).
* Si $\rho = 0.5$, la muestra se lee a la mitad de velocidad (una octava inferior).

El puntero de lectura de la memoria, comúnmente de punto flotante, avanza sumando $\rho$ en cada ciclo de reloj del sistema de audio: $pos_{n+1} = pos_n + \rho$.

## 4. Interpolación Numérica

Como el incremento de fase $\rho$ rara vez es un número entero, el puntero de lectura caerá sistemáticamente entre dos muestras discretas de la tabla de ondas. Es matemáticamente necesario estimar el valor de amplitud en esa posición fraccionaria para evitar ruido de cuantización o *aliasing*. 

Sea un índice de lectura continuo $x = n + \alpha$, donde $n$ es la parte entera (índice en el arreglo) y $\alpha \in [0, 1)$ es la parte fraccionaria. Los métodos principales son:

* **Vecino más cercano (Zero-Order Hold):** Selecciona la muestra más cercana ignorando $\alpha$. 
  $y(x) = y[n]$. Requiere nulo costo computacional, pero genera distorsión inarmónica severa (aliasing).
* **Interpolación Lineal (First-Order):** Traza una recta entre $y[n]$ y $y[n+1]$. 
  $y(x) = (1 - \alpha) y[n] + \alpha y[n+1]$. Atenúa las altas frecuencias (efecto de filtro paso bajo) pero reduce significativamente el ruido.
* **Interpolación Cúbica (Hermite o Catmull-Rom):** Utiliza cuatro puntos adyacentes ($y[n-1], y[n], y[n+1], y[n+2]$) para trazar una curva suave. Ofrece un equilibrio óptimo entre costo de procesamiento en microcontroladores (como el ESP32) y calidad de reconstrucción espectral de alta fidelidad.
