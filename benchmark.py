import os
import glob
import subprocess
import re
import statistics
import sys
import csv

# Configurações do Benchmark
EXECUTABLE = "./relaxacao"
INSTANCES_DIR = "instancias"
NUM_RUNS = 10

strategy = sys.argv[1].upper() if len(sys.argv) > 1 else "DFS"
if strategy not in {"DFS", "BFS", "BBS"}:
    print("Estratégia inválida. Use: DFS, BFS ou BBS")
    sys.exit(1)

OUTPUT_CSV = f"resultados_benchmark_{strategy.lower()}.csv"

def parse_output(output: str):
    """Extrai o Custo Ótimo e o Tempo de Execução do stdout do C++."""
    cost_match = re.search(r"Custo Ótimo:\s+([\d.]+)", output)
    time_match = re.search(r"Tempo de Execução:\s+([\d.]+)", output)

    cost = float(cost_match.group(1)) if cost_match else None
    exec_time = float(time_match.group(1)) if time_match else None
    
    return cost, exec_time

def run_benchmark():
    if not os.path.exists(EXECUTABLE):
        print(f"Erro: Executável '{EXECUTABLE}' não encontrado. Compile o projeto primeiro com 'make'.")
        sys.exit(1)

    instance_files = sorted(glob.glob(os.path.join(INSTANCES_DIR, "*")))
    instance_files = [f for f in instance_files if os.path.isfile(f)]

    if not instance_files:
        print(f"Erro: Nenhuma instância encontrada na pasta '{INSTANCES_DIR}'.")
        sys.exit(1)

    print(f"Iniciando Benchmark ({strategy}): {len(instance_files)} instâncias | {NUM_RUNS} execuções por instância\n")
    
    header = f"{'Instância':<25} | {'Estratégia':<10} | {'Custo Ótimo':<12} | {'Média (s)':<12} | {'Mín (s)':<12} | {'Máx (s)':<12}"
    print(header)
    print("-" * len(header))

    with open(OUTPUT_CSV, mode="w", newline="", encoding="utf-8") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(["Instancia", "Estrategia", "Custo_Otimo", "Media_Tempo_s", "Min_Tempo_s", "Max_Tempo_s"])

        for filepath in instance_files:
            filename = os.path.basename(filepath)
            exec_times = []
            costs = []

            for _ in range(NUM_RUNS):
                result = subprocess.run(
                    [EXECUTABLE, filepath, strategy],
                    capture_output=True,
                    text=True
                )

                cost, exec_time = parse_output(result.stdout)

                if exec_time is not None:
                    exec_times.append(exec_time)
                if cost is not None:
                    costs.append(cost)

            if exec_times and costs:
                avg_time = statistics.mean(exec_times)
                min_time = min(exec_times)
                max_time = max(exec_times)
                cost_val = costs[0]

                print(f"{filename:<25} | {strategy:<10} | {cost_val:<12.2f} | {avg_time:<12.6f} | {min_time:<12.6f} | {max_time:<12.6f}")
                writer.writerow([filename, strategy, f"{cost_val:.2f}", f"{avg_time:.6f}", f"{min_time:.6f}", f"{max_time:.6f}"])
            else:
                print(f"{filename:<25} | {strategy:<10} | Erro na leitura dos resultados.")

    print(f"\nResultados salvos com sucesso no arquivo '{OUTPUT_CSV}'.")

if __name__ == "__main__":
    run_benchmark()