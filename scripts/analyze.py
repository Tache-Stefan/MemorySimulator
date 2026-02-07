import serial
import matplotlib.pyplot as plt
import time

def capture_benchmark(num_benchmarks=5,port='COM3', baud=9600, timeout_seconds=180):
    ser = serial.Serial(port, baud, timeout=2)
    results = []
    current_result = {}
    start_time = time.time()
    
    print("Run benchmarks on Arduino to capture results.\n")
    
    while True:
        if time.time() - start_time > timeout_seconds:
            print(f"\nTimeout after {timeout_seconds}s")
            break
        
        line = ser.readline().decode().strip()
        
        if not line:
            continue
        
        print(line)
        
        if 'Pattern:' in line:
            current_result = {}  # Start new result
            current_result['pattern'] = line.split(':')[1].strip()
        elif 'Policy:' in line:
            current_result['policy'] = line.split(':')[1].strip()
        elif 'L1 Hits:' in line:
            current_result['l1_hits'] = int(line.split(':')[1].strip())
        elif 'L2 Hits:' in line:
            current_result['l2_hits'] = int(line.split(':')[1].strip())
        elif 'Misses:' in line:
            current_result['misses'] = int(line.split(':')[1].strip())
        elif 'Hit Rate:' in line:
            current_result['hit_rate'] = float(line.split(':')[1].replace('%', '').strip())
        elif 'Avg Cycles:' in line:
            current_result['avg_cycles'] = float(line.split(':')[1].strip())
            if 'pattern' in current_result:
                results.append(current_result.copy())
                print(f"  -> Captured: {current_result['pattern']} / {current_result.get('policy', 'N/A')} ({len(results)} total)\n")
        
        if len(results) >= num_benchmarks:
            print(f"\n{num_benchmarks} benchmarks captured!")
            break
    
    ser.close()
    return results

def plot_results(results):
    if not results:
        print("No results to plot.")
        return
    
    # Group by policy
    patterns = [r['pattern'][:3].upper() for r in results]
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 5))
    
    # Hit Rate by Pattern
    hit_rates = [r['hit_rate'] for r in results]
    colors = ['steelblue' if r.get('policy') == 'LRU' 
              else 'coral' if r.get('policy') == 'LFU' 
              else 'seagreen' for r in results]
    
    bars = axes[0].bar(patterns, hit_rates, color=colors)
    axes[0].set_ylabel('Hit Rate (%)')
    axes[0].set_xlabel('Pattern')
    axes[0].set_title('Hit Rate by Pattern')
    axes[0].set_ylim(0, 100)
    axes[0].grid(axis='y', alpha=0.3)
    
    # Add value labels
    for bar, val in zip(bars, hit_rates):
        axes[0].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                     f'{val:.1f}%', ha='center', va='bottom', fontsize=9)
    
    # Average Cycles by Pattern
    avg_cycles = [r['avg_cycles'] for r in results]
    bars2 = axes[1].bar(patterns, avg_cycles, color=colors)
    axes[1].set_ylabel('Avg Cycles per Access')
    axes[1].set_xlabel('Pattern')
    axes[1].set_title('Latency Cost by Pattern')
    axes[1].grid(axis='y', alpha=0.3)
    
    for bar, val in zip(bars2, avg_cycles):
        axes[1].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
                     f'{val:.1f}', ha='center', va='bottom', fontsize=9)
    
    from matplotlib.patches import Patch
    legend_elements = [
        Patch(facecolor='steelblue', label='LRU'),
        Patch(facecolor='coral', label='LFU'),
        Patch(facecolor='seagreen', label='MRU')
    ]
    fig.legend(handles=legend_elements, loc='upper right')
    
    plt.tight_layout()
    plt.savefig('results/benchmark_results.png', dpi=150)
    print("Saved: results/benchmark_results.png")
    plt.show()

def print_summary(results):
    if not results:
        return
    
    print("\n" + "=" * 60)
    print(f"{'Pattern':<12} {'Policy':<8} {'Hit Rate':<10} {'Avg Cycles':<12}")
    print("=" * 60)
    
    for r in results:
        print(f"{r.get('pattern', 'N/A'):<12} "
              f"{r.get('policy', 'N/A'):<8} "
              f"{r.get('hit_rate', 0):>6.1f}%    "
              f"{r.get('avg_cycles', 0):>8.1f}")
    
    print("=" * 60)
    
    if results:
        best = max(results, key=lambda r: r.get('hit_rate', 0))
        print(f"\nBest: {best['pattern']} with {best.get('policy', 'N/A')} ({best['hit_rate']:.1f}% hit rate)")

def export_csv(results, filename='results/benchmark_results.csv'):
    if not results:
        return
    
    with open(filename, 'w') as f:
        f.write('pattern,policy,l1_hits,l2_hits,misses,hit_rate,avg_cycles\n')
        
        for r in results:
            f.write(f"{r.get('pattern', '')},"
                    f"{r.get('policy', '')},"
                    f"{r.get('l1_hits', 0)},"
                    f"{r.get('l2_hits', 0)},"
                    f"{r.get('misses', 0)},"
                    f"{r.get('hit_rate', 0):.2f},"
                    f"{r.get('avg_cycles', 0):.2f}\n")
    
    print(f"Exported to: {filename}")

if __name__ == '__main__':
    import sys
    port = sys.argv[1] if len(sys.argv) > 1 else 'COM3'
    
    results = capture_benchmark(port=port)
    print_summary(results)
    export_csv(results)
    plot_results(results)
