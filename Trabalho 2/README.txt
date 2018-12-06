###########
## GUIDE ##
###########

# 1) Ligação dos cabos
# 2) Configurar switch
#       -> tux3
#       -> gtkterm
#       -> enable 
#       -> copy flash:turma3-joao-luis-rui startup-config
#       -> reload
#  3) Configurar router
#       -> tux3
#       -> gtkterm 
#       -> copy flash:turma3-joao-luis-rui startup-config
#       -> reload
#  4) Executar tux4.sh
#  5) Executar tux1.sh tux2.sh
#  6) Para cada tux(1,2,4) executar
#       -> gedit /etc/resolv.conf
#       -> search netlab.fe.up.pt
#       -> nameserver 172.16.1.1
#  7) Check internet connection
#  8) Download file 

############################
## Exp$W_wireshark($X.$Y) ##
############################

$W -> Número da experiência
$X -> Número do registo a ser feito na experiência, ordenados sequencialmente
$Y -> Número do tux, no qual o registo é obtido
