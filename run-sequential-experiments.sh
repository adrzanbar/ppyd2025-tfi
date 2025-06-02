#!/bin/bash

output_file="resultados-secuencial.csv"
sizes=(1000 2000 3000 4000 8000)
repeticiones=3

echo "dimensiones,tiempo_promedio_s" > "$output_file"

for size in "${sizes[@]}"; do
    suma=0

    echo "Ejecutando para tamaño ${size}x${size}..."

    # Repetir 3 veces
    for ((i=1; i<=repeticiones; i++)); do
        # Ejecutar y capturar el tiempo real en segundos
        tiempo=$(/usr/bin/time -f "%e" ./sequential "$size" "$size" "$size" "$size") #2>&1 >/dev/null)
        # Sumar el tiempo a la variable suma
        suma=$(echo "$suma + $tiempo" | bc -l)
    done

    # Calcular promedio
    promedio=$(echo "scale=4; $suma / $repeticiones" | bc -l)

    # Escribir al archivo CSV
    echo "${size}x${size},$promedio" >> "$output_file"
done

echo "Resultados guardados en $output_file."
