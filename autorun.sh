#!/bin/bash
make all
mkdir log
timeNow=$(date +%d-%b-%H_%M_%S)  
echo "$timeNow"
touch "log/${timeNow}output.txt"

## declare an array variable that has the path of every mtx
declare -a mtx=("./matrixes/belgium_osm/belgium_osm.mtx" "./matrixes/com-Youtube/com-Youtube.mtx" "./matrixes/dblp-2010/dblp-2010.mtx" "./matrixes/mycielskian13/mycielskian13.mtx" "./matrixes/NACA0015/NACA0015.mtx")
declare -a programs=("./triangle_v3" "./triangle_v3_cilk" "./triangle_v3_openmp" "./triangle_v4" "./triangle_v4_cilk" "./triangle_v4_openmp")



for program in "${programs[@]}"
do
    for matrix in "${mtx[@]}"
        do
            echo "$program" "$matrix" 1 2 >> "log/${timeNow}output.txt"
            "$program" "$matrix" 1 2 >> "log/${timeNow}output.txt"
            # Add a blank line between each iteration 
            echo >> "log/${timeNow}output.txt"
        done
        # Add a blank line between each iteration 
        echo >> "log/${timeNow}output.txt"    
done




