import pandas as pd
import matplotlib.pyplot as plt
import glob
import os
import re

def plot_tempos_medios():
    try:
        df = pd.read_csv("temposMedios.csv")
    except FileNotFoundError:
        print("Arquivo temposMedios.csv não encontrado.")
        return

    df['Cenario'] = df['Produtores(Np)'].astype(str) + "P / " + df['Consumidores(Nc)'].astype(str) + "C"
    
    plt.figure(figsize=(10, 6))
    
    for n_val, group in df.groupby('Tamanho_Buffer(N)'):
        plt.plot(group['Cenario'], group['Tempo_Medio_Segundos'], marker='o', label=f'N = {n_val}')
        
    plt.title('Tempo Médio de Execução vs Configuração de Threads')
    plt.xlabel('Configuração (Produtores / Consumidores)')
    plt.ylabel('Tempo Médio (segundos)')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(title='Tamanho do Buffer')
    plt.tight_layout()
    plt.savefig('graficoTemposMedios.png')
    plt.close()

def plot_ocupacao_buffer():
    arquivos_log = glob.glob("logsOcupacao/logOcupacao_Np*.csv")
    
    if not arquivos_log:
        print("Nenhum arquivo de log encontrado na pasta 'logsOcupacao'.")
        return

    if not os.path.exists('graficosOcupacao'):
        os.makedirs('graficosOcupacao')

    for arquivo in arquivos_log:
        df = pd.read_csv(arquivo)
        
        match = re.search(r'logOcupacao_Np(\d+)_Nc(\d+)_N(\d+)', arquivo)
        if match:
            np_val = match.group(1)
            nc_val = match.group(2)
            n_val = match.group(3)
        else:
            np_val, nc_val, n_val = "X", "X", "100"
        
        plt.figure(figsize=(10, 4))
        plt.plot(df['Operacao'], df['Ocupacao'], color='purple', alpha=0.6, linewidth=0.5)
        
        plt.title(f'Ocupação do Buffer - Np{np_val}, Nc{nc_val}, Buffer N={n_val}')
        plt.xlabel('Número da Operação (Iteração)')
        plt.ylabel('Quantidade de Itens no Buffer')
        
        plt.ylim(0, int(n_val))
        
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.tight_layout()
        
        nome_imagem = f"graficosOcupacao/grafico_Np{np_val}_Nc{nc_val}_N{n_val}.png"
        plt.savefig(nome_imagem, dpi=150)
        plt.close()

if __name__ == "__main__":
    plot_tempos_medios()
    plot_ocupacao_buffer()