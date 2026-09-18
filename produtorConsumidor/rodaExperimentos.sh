# Disciplina: Sistemas Distribuiídos

# Integrantes: Davi Braga, Otávio Andrade e Vitório Marcos
# Centro Federal de Educação Tecnológica de Minas Gerais (CEFET-MG)
# Belo Horizonte, 2026

gcc -o tp produtorConsumidor.c -lpthread -O3

# Parâmetros exigidos
Ns=(1 10 100 1000) # Tamanho do buffer que será utilizado para produzir e consumir
Nps=(1 1 1 1 2 4 8) # Quantidade de produtores
Ncs=(1 2 4 8 1 1 1) # Quantidade de consumidores

arquivo_saida="temposMedios.csv"
mkdir -p logsOcupacao # Pasta onde os arquivos de logs de ocupação de cada execução serão gerados

# Cabeçalho do arquivo CSV
echo "Tamanho_Buffer(N),Produtores(Np),Consumidores(Nc),Tempo_Medio_Segundos" > $arquivo_saida

for N in "${Ns[@]}"; do
    for i in "${!Nps[@]}"; do
        Np=${Nps[$i]}
        Nc=${Ncs[$i]}
        
        echo -n "Testando N=$N | Np=$Np | Nc=$Nc ... "
        
        tempo_total=0
        
        for rodada in {1..10}; do
            # Medição de tempo compatível com Bash / Git Bash
            inicio=$(date +%s.%N)
            ./tp $Np $Nc $N > /dev/null 2>&1
            fim=$(date +%s.%N)
            
            tempo_rodada=$(awk "BEGIN {print $fim - $inicio}")
            tempo_total=$(awk "BEGIN {print $tempo_total + $tempo_rodada}")
        done
        
        tempo_medio=$(awk "BEGIN {printf \"%.4f\", $tempo_total / 10}")
        
        echo "$N,$Np,$Nc,$tempo_medio" >> $arquivo_saida
    done
done