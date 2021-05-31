#! /bin/bash
echo "Ingresa el numero de serie hasta el que cargaste"
read numSerie
msg="Cargado hasta la placa $numSerie"
git add . 
git commit -m "$msg"
git push
