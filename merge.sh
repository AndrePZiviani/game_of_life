#!/bin/bash
CAMINHO="$HOME/gol"
ITERACOES=$(ls -l $CAMINHO/gol-0-*.out | wc -l)
ITERACOES=$((ITERACOES - 1))
for j in $(seq 0 "$ITERACOES"); do
    LINHAS=$(cat $CAMINHO/gol-0-$j.out |wc -l | awk '{print $1}')
    rm -rf $CAMINHO/all-$j.out &> /dev/null
    for i in $(seq 1 "$LINHAS"); do
        A=$(head -$i $CAMINHO/gol-0-$j.out | tail -1)
        B=$(head -$i $CAMINHO/gol-3-$j.out | tail -1)
        C=$(head -$i $CAMINHO/gol-6-$j.out | tail -1)
        echo "${A}${B}${C}#" >> $CAMINHO/all-$j.out
    done
    for i in $(seq 1 "$LINHAS"); do
        A=$(head -$i $CAMINHO/gol-1-$j.out | tail -1)
        B=$(head -$i $CAMINHO/gol-4-$j.out | tail -1)
        C=$(head -$i $CAMINHO/gol-7-$j.out | tail -1)
        echo "${A}${B}${C}#" >> $CAMINHO/all-$j.out
    done
    for i in $(seq 1 "$LINHAS"); do
        A=$(head -$i $CAMINHO/gol-2-$j.out | tail -1)
        B=$(head -$i $CAMINHO/gol-5-$j.out | tail -1)
        C=$(head -$i $CAMINHO/gol-8-$j.out | tail -1)
        echo "${A}${B}${C}#" >> $CAMINHO/all-$j.out
    done
done
