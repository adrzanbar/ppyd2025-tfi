# Informe Final - Multiplicación de Matrices Paralela y Distribuida

---

## 1. Introducción

La multiplicación de matrices representa una operación fundamental en el ámbito de la matemática computacional, con aplicaciones críticas que abarcan desde simulaciones físicas y modelado científico hasta procesamiento de imágenes, visión computacional y entrenamiento de modelos de aprendizaje automático. Dada la naturaleza altamente paralelizable de esta operación, constituye un paradigma idóneo para investigar estrategias de computación paralela y distribuida, con el fin de optimizar rendimiento y escalabilidad.

El presente trabajo se propone diseñar, implementar y evaluar una solución eficiente para la multiplicación de matrices de gran dimensión, utilizando C++ en conjunción con la biblioteca MPI (Message Passing Interface). Se pretende analizar de forma exhaustiva el rendimiento, la escalabilidad, la distribución de carga computacional entre procesos y los costos asociados a la comunicación en arquitecturas distribuidas.

---

## 2. Definición del Problema

Se considera la multiplicación de dos matrices:

-   $A \in \mathbb{R}^{m \times n}$, matriz de dimensiones $m \times n$
-   $B \in \mathbb{R}^{n \times p}$, matriz de dimensiones $n \times p$

El producto $C = A \times B$ resulta en una matriz $C \in \mathbb{R}^{m \times p}$, cuyo elemento $c_{ij}$ se determina mediante la suma ponderada:

$$
c_{ij} = \sum_{k=1}^{n} a_{ik} \cdot b_{kj}
$$ (1)

El complejidad temporal de esta operación, bajo un esquema secuencial clásico, es de orden $O(m \cdot n \cdot p)$, implicando un alto consumo de recursos para valores grandes de $m$, $n$ y $p$. Este alto costo computacional justifica la adopción de técnicas paralelas para mitigar tiempos de ejecución y mejorar la eficiencia.

---

## 3. Análisis de Paralelismo y Estrategia de Descomposición

La multiplicación matricial exhibe un alto grado de paralelismo, dado que el cálculo de cada elemento $c_{ij}$ es independiente de los demás elementos, permitiendo una granularidad gruesa favorable para la distribución eficiente del trabajo entre procesos.

Se adoptó una estrategia de partición por filas (descomposición por dominio), dividiendo la matriz $A$ en bloques horizontales contiguos asignados a distintos procesos. Cada proceso es responsable de computar un subconjunto de filas de la matriz resultado $C$, mientras que la matriz $B$ se distribuye completa a todos los procesos, dado que es requerida íntegramente para el cálculo.

La granularidad de la solución es gruesa, ya que cada unidad de trabajo corresponde a un bloque relativamente grande (una o más filas completas).

Esta estrategia promete un balance de carga óptimo siempre que la distribución de filas sea proporcional al número de procesos, una comunicación mínima entre procesos.

---

## 4. Implementación

La implementación se desarrolló en C++ haciendo uso de MPI para la comunicación interprocesos. Se diseñaron dos versiones del algoritmo:

1. Versión secuencial, utilizada como referencia para medir el desempeño.
2. Versión paralela distribuida, capaz de ejecutarse concurrentemente en múltiples procesos MPI.

### Estructura de Datos — `Matrix`

Se diseñó una clase `Matrix` que encapsula tanto los datos como las operaciones fundamentales sobre matrices, con las siguientes características:

- Almacenamiento contiguo en memoria en orden fila-major para optimizar la localidad de datos.
- Sobrecarga de operadores para facilitar la manipulación y acceso a elementos.
- Métodos para inicialización con datos aleatorios controlables, permitiendo reproducibilidad.
- Conversión a formatos legibles para depuración y validación.
- Acceso directo a la estructura de datos subyacente para interoperabilidad con funciones MPI.

### Multiplicación Secuencial

Se implementó el algoritmo clásico con tres bucles anidados que recorren las filas de $A$, columnas de $B$ y el índice común para acumulación de productos parciales.

### Multiplicación Paralela con MPI

La función `MPI_Matrix_Multiplication` coordina la ejecución distribuida del cálculo, contemplando:

- Cálculo del tamaño de bloque (filas) asignado a cada proceso.
- Distribución de las filas de $A$ usando `MPI_Scatterv`.
- Difusión de la matriz completa $B$ mediante `MPI_Bcast`.
- Cálculo local independiente de la submatriz correspondiente.
- Recolección y ensamblaje de los bloques de resultado en el proceso raíz con `MPI_Gatherv`.

Se facilita una comunicación estructurada mediante las primitivas MPI mencionadas, optimizadas para operaciones colectivas de este tipo.
El algoritmo intenta repartir las filas de filas de $A$ de forma equitativa. Si se presenta el caso en que el número de procesos disponible no divide número de filas, se asignará el resto a los procesos de menor rango (identificador dentro del comunicador MPI).

---

## 5. Diseño Experimental

### 5.1 Objetivos del Experimento

- Cuantificar el tiempo total de ejecución y tiempos parciales por proceso.
- Evaluar la escalabilidad mediante incremento progresivo del número de procesos.
- Calcular métricas de speedup y eficiencia para diferentes configuraciones.
- Identificar cuellos de botella, especialmente aquellos relacionados con la comunicación.
- Verificar la equidad en la distribución de carga computacional.

### 5.2 Tamaños de Matrices Analizados

Se experimentará con matrices cuadradas de las siguientes dimensiones:

- $1000 \times 1000$
- $2000 \times 2000$
- $3000 \times 3000$
- $4000 \times 4000$
- $8000 \times 8000$

Este rango permite observar tendencias de desempeño bajo distintas escalas computacionales.

### 5.3 Configuración de Paralelismo

Se ejecutarán pruebas con el siguiente número de procesos:

- 2, 4, 6, 8, 10, 12, 14, 16

### 5.4 Métricas a Registrar

- Tiempo total de ejecución.
- Tiempo de cómputo local por proceso.
- Speedup: $S(n) = \frac{T_1}{T_n}$
- Eficiencia: $E(n) = \frac{S(n)}{n}$
- Balanceo de carga: tiempos de cómputo y comunicación/ocio de cada proceso

### 5.5 Metodología de Evaluación

- Cada configuración será ejecutada en triplicado, registrando el promedio para minimizar ruido.
- Se excluirá el tiempo invertido en inicialización y generación de datos aleatorios.
- Se tomará en cuenta el tiempo de cómputo local y total para obtener la diferencia como tiempo de comunicación u ocioso
- Uso de `MPI_Wtime` para obtención precisa de tiempos.

---

## 6. Resultados

*[Esta sección será completada una vez se disponga de los datos experimentales. Se incluirán análisis gráficos de speedup y eficiencia, distribución temporal entre procesos, impactos de comunicación y se destacarán patrones relevantes relacionados con las dimensiones de las matrices y número de procesos.]*

---
$$
