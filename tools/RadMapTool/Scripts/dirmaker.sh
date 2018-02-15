for i in `seq 0 999`; do
    dir0=$(echo "$i/100" | bc);
    dir1=$(echo "($i-$dir0*100)/10" | bc)
    dir2=$(echo "$i-$dir0*100-$dir1*10" | bc)

    echo $dir0/$dir1/$dir2
    mkdir -p $dir0/$dir1/$dir2
done

