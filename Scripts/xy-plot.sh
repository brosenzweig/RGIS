#!/bin/bash
TMPDIR=${TMPDIR:=/tmp}

PLOTF=${TMPDIR}/ghaasplot.$$
TEMP0=${TMPDIR}/ghaastmp0.$$
TEMP1=${TMPDIR}/ghaastmp1.$$
TEMP2=${TMPDIR}/ghaastmp2.$$

awk 'BEGIN {FS="\t"} (5==NF) { if (1 != NR) { print $3 "\t" $4 "\t" $5 } }' $1 > ${TEMP0}

LNUM1=$(grep '^0' ${TEMP0} | awk '{print $2 "\t" $3 }' | tee ${TEMP1} | wc -l)
LNUM2=$(grep '^1' ${TEMP0} | awk '{print $2 "\t" $3 }' | tee ${TEMP2} | wc -l)
echo set autoscale > ${PLOTF}
if [ $2 = log ]; then
	echo set logscale xy >> ${PLOTF}
fi
echo set title \"GHAAS Table Field Comparison\" >> ${PLOTF}
echo set xlabel `awk '(1==NR) { print $4 "\n" $5 }' $1 | head -1` >> ${PLOTF}
echo set ylabel `awk '(1==NR) { print $4 "\n" $5 }' $1 | tail -1` >> ${PLOTF}
if [[ ${LNUM1} > 0 && ${LNUM2} > 0 ]]; then
  echo plot \"$TEMP1\" title \"Unselected\", \"$TEMP2\" title \"Selected\" >> ${PLOTF}
else
   if [[ ${LNUM1} > 0 ]]; then
     echo plot \"$TEMP1\" title \"Unselected\" >> ${PLOTF}
   fi
   if [[ ${LNUM2} > 0 ]]; then
     echo plot \"$TEMP2\" title \"Selected\"   >> ${PLOTF}
  fi
fi
echo pause 60 >> ${PLOTF}
rm ${TEMP0}

sh -c "gnuplot ${PLOTF}; rm ${PLOTF}; rm ${TEMP1}; rm ${TEMP2}" &
