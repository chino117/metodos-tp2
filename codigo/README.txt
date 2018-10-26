[] marca los parametros obligatorios
() marca los parametros opcionales

./tp2 -m [Modo de uso] -i [archivo de entrada train] -q [archivo de entrada test] -o [archivo de salida]  -k (k de kNN) -a (alpha de PCA, depende de modo) -r(archivo de salida de mediciones) -e (experimentación) -t (iteraciones) -l (cota inferior de corte del vocabullario) -u (cota superior de corte del vocabulario) -n (norma)

## Modo de uso: Debe ser alguno de los siguientes valores: 0 (kNN),1 (PCA).
## archivos de entrada train: Son los .csv armados en el formato definido en el enunciado
## archivos de entrada test: Son los .csv	""
## archivo de salida: el nombre del archivo de salida con la clasificación de los datos de test de <test_set>
## k de kNN 
## alpha de PCA: Debe ser un numero positivo mayor a 0.
## experimentación indica que estamos en modo èxperimetnacion. Es para los scripts.
## l el corte inferior del vocabulario.
## u el corte superior del vocabulario.
## n norma a elegir. 0 es norma 2 y 1 norma 1.
## Archivo de salida de mediciones: Es el archivo donde escribimos las mediciones tomadas. Por defecto, toma el valor de 
archivo de salida y le agrega ".mediciones" al final.