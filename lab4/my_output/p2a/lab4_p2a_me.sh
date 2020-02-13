#!/bin/bash

function gen_osyms() {
    fsm=$1
    osyms=$2
    eps="<epsilon>"
    echo "<epsilon>" > ${osyms}.tmp
    awk '{print $4}' $fsm | grep -vP "^$" | grep -v "epsilon" | sort -u >> ${osyms}.tmp
    awk '{print $1,(NR-1)}' ${osyms}.tmp > $osyms
    rm ${osyms}.tmp
}

function gen_isyms() {
    fsm=$1
    isyms=$2
    eps="<epsilon>"
    echo "<epsilon>" > ${isyms}.tmp
    awk '{print $3}' $fsm | grep -vP "^$" | grep -v "epsilon" | sort -u >> ${isyms}.tmp
    awk '{print $1,(NR-1)}' ${isyms}.tmp > $isyms
    rm ${isyms}.tmp
}

function gen_ps() {
    fst=$1
    ps=$2
    fstdraw $fst | dot -Tps > $2
}

# Compile wd.fst and wd2lx.fst and then compose them
gen_osyms wd2lx.fsm lx.syms
gen_isyms wd2lx.fsm wd.syms
fstcompile --isymbols=wd.syms --osymbols=wd.syms --keep_isymbols --keep_osymbols --acceptor wd.fsm wd.fst
fstcompile --isymbols=wd.syms --osymbols=lx.syms --keep_isymbols --keep_osymbols wd2lx.fsm wd2lx.fst

fstcompose wd.fst wd2lx.fst lx.fst

# Compile lx2pn.fst and compose lx.fst with lx2pn.fst
gen_osyms lx2pn.fsm pn.syms
fstcompile --isymbols=lx.syms --osymbols=pn.syms --keep_isymbols --keep_osymbols lx2pn.fsm lx2pn.fst

fstcompose lx.fst lx2pn.fst pn.fst

# Compile pn2md.fst and compose pn.fst with pn2md.fst
gen_osyms pn2md.fsm md.syms
fstcompile --isymbols=pn.syms --osymbols=md.syms --keep_isymbols --keep_osymbols pn2md.fsm pn2md.fst

fstcompose pn.fst pn2md.fst md.fst

# Compile md2hmm.fst and compose md.fst with md2hmm.fst
gen_osyms md2hmm.fsm hmm.syms
fstcompile --isymbols=md.syms --osymbols=hmm.syms --keep_isymbols --keep_osymbols md2hmm.fsm md2hmm.fst

fstcompose md.fst md2hmm.fst hmm.fst
