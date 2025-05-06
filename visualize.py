import os
import matplotlib.pyplot as plt


# Visualize the results of datalogger.cpp's logs
def read_log(file_path):
    data = []
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return data

    with open(file_path, "r") as file:
        for line in file:
            parts = line.strip().split()
            if len(parts) >= 6:  # Ensure there are enough columns
                try:
                    time = int(parts[0])
                    altitude = float(parts[4])
                    data.append((time, altitude))
                except ValueError:
                    continue
    return data


def plot_logs(log_paths, output_image="chart.png"):
    plt.figure(figsize=(10, 6))

    for log_path, label in log_paths:
        data = read_log(log_path)
        if data:
            times, altitudes = zip(*data)
            plt.plot(times, altitudes, label=label)

    plt.title("Altitude Over Time")
    plt.xlabel("Time (ms)")
    plt.ylabel("Altitude (m)")
    plt.legend()
    plt.grid(True)
    plt.savefig(output_image)
    print(f"Chart saved as {output_image}")


def main():
    log_paths = [
        ("logs/log_original.txt", "Original"),
        ("logs/log_append.txt", "Appending"),
        ("logs/log_chunk.txt", "Buffered"),
    ]
    plot_logs(log_paths)


if __name__ == "__main__":
    main()
